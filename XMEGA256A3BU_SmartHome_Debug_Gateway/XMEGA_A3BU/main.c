/*
 * XMEGA_A3BU.c
 *
 * Created: 02.01.2025 12:08:44
 * Author : Ivan Prints
 */ 
#include "config.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "spic_driver.h"
#include "twie_driver.h"
#include "gpio_driver.h"
#include "ds18S20.h"
#include "debugSerial.h"
#include "usarte.h"
#include "sysTickTimer.h"
#include "rtc.h"
#include "socket.h"
#include "netHandler.h"
#include "canspi.h"
#include "string.h"
#include "float.h"
#include "stdbool.h"


/*
 * System Configuration:
 * ----------------------
 * Device IP Address    : 192.168.1.18
 * Device MAC Address   : 20:CF:F0:32:46:15
 * UDP Listening Port   : 4030  (Incoming control packets)
 * UDP Answer Port      : 4031  (Outgoing broadcast responses)
 * HTTP Listening Port  : 80    (Embedded web interface for status/config)
 *
 * UDP Packet Format:
 * ------------------
 * Byte [0..3]    : uint32_t  msgHeader     (Must be 0xAA1122BB)
 * Byte [4]       : uint8_t   deviceId      (0 = local device)
 * Byte [5]       : uint8_t   msgType       (e.g. LOCAL_CMD, CAN_CMD)
 * Byte [6]       : uint8_t   idType        (0 = standard, 1 = extended)
 * Byte [7..10]   : uint32_t  canId         (CAN message ID)
 * Byte [11]      : uint8_t   dlc           (CAN data length: 0–8)
 * Byte [12..19]  : uint8_t   data[8]       (CAN data payload)
 * Byte [20]      : uint8_t   endMarker     (Must be 0x55)
 *
 * Example Packet (Hex):
 * AA 11 22 BB  00 01 00  55 03 00 00  08  61 00 64 00 3E 26 00 00  55
 */



rtc_date sys_rtc = {
	.date = 4,
	.month = 1,
	.year = 25,
	.dayofweek = 6,
	.hour = 0,
	.minute = 8,
	.second = 00
};

typedef struct {
	uint8_t msgHeader[4];
	uint8_t msgType[2];
	uCAN_MSG canMsg;
	uint8_t crc;
} udpDatagram;

gpio hbRedLed = {(uint8_t *)&PORTF , PIN6_bp};
	
gpio in1 = {(uint8_t *)&PORTB , PIN0_bp};
gpio in2 = {(uint8_t *)&PORTB , PIN1_bp};
gpio rel1 = {(uint8_t *)&PORTB , PIN2_bp};
gpio rel2 = {(uint8_t *)&PORTB , PIN3_bp};
	
gpio ldRun = {(uint8_t *)&PORTA , PIN0_bp};
gpio ldCanTx = {(uint8_t *)&PORTA , PIN1_bp};
gpio ldCanRx = {(uint8_t *)&PORTA , PIN2_bp};

gpio ldLanRx = {(uint8_t *)&PORTA , PIN3_bp};
gpio ldLanTx = {(uint8_t *)&PORTA , PIN4_bp};

gpio ldHttpHandle = {(uint8_t *)&PORTA , PIN5_bp};
gpio canTerminator = {(uint8_t *)&PORTF , PIN7_bp};
// PORTA.1 - CAN error led
// PORTA.2 - CAN activity led
// PORTA.3 - LAN activity led
// PORTA.4 - LAN error led
// PORTA.5 - NET pin
uCAN_MSG canTxMessage;
uCAN_MSG rxMessage;

uCAN_MSG temperatureSensorsCan;
uCAN_MSG rtcDataCan;
uCAN_MSG keepAliveCan;


uint32_t dallasLastMeasure = 0;
bool dallasRequestType = false;


SensorUnit extSensors;

uint32_t canCmdMessageTime = 0;
uint8_t canMsgType = 0;


udpDatagram udpRxMsg = {
	.msgHeader = {0xaa, 0x11, 0x22, 0xbb},
	.msgType = {0x0, 0x0},
	.crc = 0x55
};

udpDatagram udpRtcTxMsg = {
	.msgHeader = {0xaa, 0x11, 0x22, 0xbb},
	.msgType = {0x0, 0x0},
	.crc = 0x55
};

udpDatagram udpTempSensorUnitTxMsg = {
	.msgHeader = {0xaa, 0x11, 0x22, 0xbb},
	.msgType = {0x0, 0x0},
	.crc = 0x55
};

udpDatagram udpKeepAliveTxMsg = {
	.msgHeader = {0xaa, 0x11, 0x22, 0xbb},
	.msgType = {0x0, 0x0},
	.crc = 0x55
};


gatewayDto gatewayData;




/*
MCU - OK
SPIC - (LAN, CAN, SD) - OK
LAN - OK
CAN - OK
GPIO - OK
UART - OK (printf)
TWIE
	RTC
	i2c-rtc INT pin handler
move index.html to PROGMEM

TODO -
	SD Card
	RTC mcu	
*/
uint8_t tmpData;
char udpDataString[128]="\0";

#define		UDP_RX_BUFF_SIZE	1024
#define		LOCAL_CMD			0x01
#define		CAN_CMD				0x02
uint8_t udpRxBuff[UDP_RX_BUFF_SIZE];

int main(void)
{
	uint32_t dd = F_CPU;
	OSC.CTRL |= OSC_RC32MEN_bm;
	while ((OSC.STATUS & OSC_RC32MRDY_bm) == 0)
	_PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_RC32M_gc);
	gpio_set_pin_direction(&hbRedLed , PORT_DIR_OUT); 
	gpio_set_pin_direction(&in1 , PORT_DIR_IN); 
	gpio_set_pin_direction(&in2 , PORT_DIR_IN);
	gpio_set_pin_direction(&rel1 , PORT_DIR_OUT);
	gpio_set_pin_direction(&rel2 , PORT_DIR_OUT);
	
	gpio_set_pin_direction(&ldRun , PORT_DIR_OUT);
	gpio_set_pin_direction(&ldHttpHandle , PORT_DIR_OUT);
	gpio_set_pin_direction(&ldCanRx , PORT_DIR_OUT);
	gpio_set_pin_direction(&ldCanTx , PORT_DIR_OUT);
	gpio_set_pin_direction(&ldLanRx , PORT_DIR_OUT);
	gpio_set_pin_direction(&ldLanTx , PORT_DIR_OUT);
	gpio_set_pin_direction(&canTerminator , PORT_DIR_OUT);
	
	gpio_set_pin_level(&hbRedLed, true);
	gpio_set_pin_level(&hbRedLed, false);
	
	gpio_set_pin_level(&ldRun, false);
	gpio_set_pin_level(&ldHttpHandle, false);
	gpio_set_pin_level(&ldCanRx, false);
	gpio_set_pin_level(&ldCanTx, false);
	gpio_set_pin_level(&ldLanRx, false);
	gpio_set_pin_level(&ldLanTx, false);
	gpio_set_pin_level(&canTerminator, true);
	
	sei();
	debugSerialInit(DEB_SERIAL_BAUD);
	serialEInit(250000UL);
	initSpic();
	netInit(&gatewayData);
	CANSPI_Initialize();
	twieInit();
	rtc_int_enable();
	//rtc_set(&sys_rtc);
	sysTickInit();
	TSDS18x20 DS18x20;
	
	
	DS18x20_Init(&DS18x20,&PORTA,6);
	DS18x20_SetResolution(&DS18x20,CONF_RES_12b);
	DS18x20_WriteScratchpad(&DS18x20);
	
	CCP = CCP_IOREG_gc;
	WDT.CTRL = WDT_PER_4KCLK_gc | WDT_ENABLE_bm | WDT_CEN_bm;
	
	
	printf("RUN\n\r");
	sprintf(udpDataString, "TestUDP\n\r");
	while (1) 
    {
		wdt_reset();
		gatewayData.digitalIn[0] = gpio_get_pin_level(&in1);
		gatewayData.digitalIn[1] = gpio_get_pin_level(&in2);
		gpio_set_pin_level(&rel1, gatewayData.digitalOut[0]);
		gpio_set_pin_level(&rel2, gatewayData.digitalOut[1]);
		
		
		
		if((getSysTick() - dallasLastMeasure) > DALLAS_MEASURE_INTERVAL){
			dallasLastMeasure = getSysTick();
			if(dallasRequestType){
				DS18x20_MeasureTemperature(&DS18x20);
			}else{
				DS18x20_ReadScratchPad(&DS18x20);
			}
			gatewayData.dallasTemp = DS18x20_TemperatureValue(&DS18x20);
			gatewayData.dallasTemp8 = (uint8_t)(gatewayData.dallasTemp < 255 && gatewayData.dallasTemp > 0 ? gatewayData.dallasTemp : 255);
			//printf("temp=%04.1f; uint8: %d\n\r", dallasTemp, dallasTemp8);
			dallasRequestType = !dallasRequestType;
		}
		
		if((getSysTick() - canCmdMessageTime) > CAN_MSG_INTERVAL && getUpTime() > 5){
			gpio_set_pin_level(&ldHttpHandle, true);
			gpio_set_pin_level(&ldCanTx, true);
			canCmdMessageTime = getSysTick();
			gatewayData.upsMsgInteval = getSysTick() - gatewayData.upsMsgTimestamp;
			gatewayData.waterTankMsgInteval = getSysTick() - gatewayData.waterTankMsgTimestamp;
			gatewayData.sensorUnitMsgInteval = getSysTick() - gatewayData.sensorUnitMsgTimestamp;
			gatewayData.kotelMsgInteval = getSysTick() - gatewayData.kotelMsgTimestamp;
			uint8_t canErr = MCP2515_ReadByte(0x2d);
			//printf("UPS %lu, WT %lu, SU %lu, KT %lu; CAN %u; %u;\n\r",
				//(unsigned long)gatewayData.upsMsgInteval,
				//(unsigned long)gatewayData.waterTankMsgInteval,
				//(unsigned long)gatewayData.sensorUnitMsgInteval,
				//(unsigned long)gatewayData.kotelMsgInteval,
				//(unsigned int)(uint8_t)canErr,
				//(unsigned int)(uint8_t)MCP2515_ReadStatus());//printf("can msg %d\n\r", canMsgType);
			
			switch (canMsgType) {
					case TEMPERATURE_REPORT:
						if(udpTempSensorUnitTxMsg.canMsg.frame.id != 0){
							CANSPI_Transmit(&udpTempSensorUnitTxMsg.canMsg);
							//udpDataSend(&udpTempSensorUnitTxMsg, sizeof(udpTempSensorUnitTxMsg));
						}
						
					break;
				
					case RTC_REPORT:
						udpRtcTxMsg.canMsg.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
						udpRtcTxMsg.canMsg.frame.id = 0x070;
						udpRtcTxMsg.canMsg.frame.dlc = 8;
						udpRtcTxMsg.canMsg.frame.data0 = (gatewayData.digitalIn[1] << 1) | gatewayData.digitalIn[0];
						udpRtcTxMsg.canMsg.frame.data1 = (gatewayData.digitalOut[1] << 1) | gatewayData.digitalOut[0];
						udpRtcTxMsg.canMsg.frame.data2 = sys_rtc.date;
						udpRtcTxMsg.canMsg.frame.data3 = sys_rtc.month;
						udpRtcTxMsg.canMsg.frame.data4 = sys_rtc.year;
						udpRtcTxMsg.canMsg.frame.data5 = sys_rtc.hour;
						udpRtcTxMsg.canMsg.frame.data6 = sys_rtc.minute;
						udpRtcTxMsg.canMsg.frame.data7 = sys_rtc.second;
						if(udpRtcTxMsg.canMsg.frame.id != 0){
							CANSPI_Transmit(&udpRtcTxMsg.canMsg);
							//udpDataSend(&udpRtcTxMsg, sizeof(udpRtcTxMsg));
						}
						
					break;
				
					case KEEP_ALIVE_REPORT:
						if(udpKeepAliveTxMsg.canMsg.frame.id != 0){
							CANSPI_Transmit(&udpKeepAliveTxMsg.canMsg);
							//udpDataSend(&udpKeepAliveTxMsg, sizeof(udpKeepAliveTxMsg));
						}
						
					break;

				default:
					printf("Message is not defined\n\r");
				break;
			}

			canMsgType++;
			if (canMsgType >= MESSAGE_TYPE_COUNT)
			canMsgType = 0;
			gpio_set_pin_level(&ldCanTx, false);
		}
		
		
		if(udpDataAvailable(&udpRxBuff, UDP_RX_BUFF_SIZE)){
			gpio_set_pin_level(&ldLanRx, true);
			uint32_t msgHeader = ((uint32_t)udpRxBuff[0] << 24) |
			((uint32_t)udpRxBuff[1] << 16) |
			((uint32_t)udpRxBuff[2] << 8)  |
			(uint32_t)udpRxBuff[3];
			
			uint32_t msgId = ((uint32_t)udpRxBuff[10] << 24) |
			((uint32_t)udpRxBuff[9] << 16) |
			((uint32_t)udpRxBuff[8] << 8)  |
			(uint32_t)udpRxBuff[7];
			
			printf("msgH=%08lx; ID:%08lx; DLC:%x; D:%02x %02x %02x %02x %02x %02x %02x %02x\r\n", msgHeader, msgId, udpRxBuff[11], udpRxBuff[12],udpRxBuff[13],udpRxBuff[14],udpRxBuff[15],udpRxBuff[16],udpRxBuff[17],udpRxBuff[18],udpRxBuff[19]);
			if(msgHeader == 0xAA1122BB && udpRxBuff[20] == 0x55){
				//AA 11 22 BB 0A 0D 01 55 03 00 00 08 61 00 64 00 3E 26 00 00 55
				if (udpRxBuff[4] == 0 && udpRxBuff[5] == LOCAL_CMD){
					if(msgId == 0x100){
						uint8_t count = sizeof(gatewayData.digitalOut);
						if (count > 8) count = 8;
						for (uint8_t i = 0; i < count; i++) {
							if ((udpRxBuff[13] >> i) & 1) {
								gatewayData.digitalOut[i] = (udpRxBuff[12] >> i) & 1;
							}
						}	
					}
					
					if(msgId == 0x101){
						//sys_rtc.date = udpRxBuff[12];
						//sys_rtc.date = udpRxBuff[13];
						sys_rtc.date = udpRxBuff[14];
						sys_rtc.month = udpRxBuff[15];
						sys_rtc.year = udpRxBuff[16];
						sys_rtc.hour = udpRxBuff[17];
						sys_rtc.minute = udpRxBuff[18];
						sys_rtc.second = udpRxBuff[19];
						rtc_set(&sys_rtc);
						rtc_int_enable();
						
					}
	
				}
				
				if (udpRxBuff[4] == 0 /*&& udpRxBuff[5] == CAN_CMD*/){
					gpio_set_pin_level(&ldCanTx, true);
					canTxMessage.frame.idType = udpRxBuff[6];
					canTxMessage.frame.id = msgId;
					canTxMessage.frame.dlc = udpRxBuff[11];
					canTxMessage.frame.data0 = udpRxBuff[12];
					canTxMessage.frame.data1 = udpRxBuff[13];
					canTxMessage.frame.data2 = udpRxBuff[14];
					canTxMessage.frame.data3 = udpRxBuff[15];
					canTxMessage.frame.data4 = udpRxBuff[16];
					canTxMessage.frame.data5 = udpRxBuff[17];
					canTxMessage.frame.data6 = udpRxBuff[18];
					canTxMessage.frame.data7 = udpRxBuff[19];
					CANSPI_Transmit(&canTxMessage);
					gpio_set_pin_level(&ldCanTx, false);
				}
			}
			gpio_set_pin_level(&ldLanRx, false);
		}
		
		 if(CANSPI_Receive(&udpRxMsg.canMsg))
		 {
			gpio_set_pin_level(&ldCanRx, true);
			gpio_set_pin_level(&ldLanTx, true);
			printf("ID:%08lx; DLC:%x; D:%02x %02x %02x %02x %02x %02x %02x %02x\r\n", udpRxMsg.canMsg.frame.id, udpRxMsg.canMsg.frame.dlc, 
			udpRxMsg.canMsg.frame.data0, 
			udpRxMsg.canMsg.frame.data1, 
			udpRxMsg.canMsg.frame.data2, 
			udpRxMsg.canMsg.frame.data3, 
			udpRxMsg.canMsg.frame.data4, 
			udpRxMsg.canMsg.frame.data5, 
			udpRxMsg.canMsg.frame.data6,
			udpRxMsg.canMsg.frame.data7);
			
			udpRxMsg.msgType[0] = 0;
			udpRxMsg.msgType[1] = CAN_CMD;
			//udpDataSend(&udpRxMsg, sizeof(udpRxMsg));
			gpio_set_pin_level(&ldLanTx, false);
			if(udpRxMsg.canMsg.frame.id == 0x100){
				uint8_t count = sizeof(gatewayData.digitalOut);
				if (count > 8) count = 8;
				for (uint8_t i = 0; i < count; i++) {
					if ((udpRxMsg.canMsg.frame.data1 >> i) & 1) {
						gatewayData.digitalOut[i] = (udpRxMsg.canMsg.frame.data0 >> i) & 1;
					}
				}
			}
			
			if(udpRxMsg.canMsg.frame.id == 0x033){
				gatewayData.upsMsgTimestamp = getSysTick();
				gatewayData.batVoltage = (float)udpRxMsg.canMsg.frame.data0 * 0.1;
				gatewayData.batCurrent = (float)((udpRxMsg.canMsg.frame.data1 << 8) | udpRxMsg.canMsg.frame.data2)/1000;
				gatewayData.batEnergy = ((uint32_t)udpRxMsg.canMsg.frame.data3 << 24) |
										((uint32_t)udpRxMsg.canMsg.frame.data4 << 16) |
										((uint32_t)udpRxMsg.canMsg.frame.data5 << 8)  |
										(uint32_t)udpRxMsg.canMsg.frame.data6;
				gatewayData.batEnergyf= (float)gatewayData.batEnergy/1000;
				if(gatewayData.batCurrent <= 0){
					sprintf(&gatewayData.currentSource, "BAT");
				}else{
					sprintf(&gatewayData.currentSource, "AC");
				}
				gatewayData.currentState = udpRxMsg.canMsg.frame.data7;	
			}
			if(udpRxMsg.canMsg.frame.id == 0x043){
				gatewayData.pumpCurrent = udpRxMsg.canMsg.frame.data0;
				sprintf(&gatewayData.pumpMode, "AUTO");
			}
			
			if(udpRxMsg.canMsg.frame.id == 0x301){
				gatewayData.waterTankMsgTimestamp = getSysTick();
				gatewayData.waterTankDIN = udpRxMsg.canMsg.frame.data0;
				gatewayData.waterTankDOUT = udpRxMsg.canMsg.frame.data1;
				gatewayData.waterPress = ((uint16_t)udpRxMsg.canMsg.frame.data2 << 8) | (uint16_t)udpRxMsg.canMsg.frame.data3;
				gatewayData.watetPressLoLim = ((uint16_t)udpRxMsg.canMsg.frame.data4 << 8) | (uint16_t)udpRxMsg.canMsg.frame.data5;
				gatewayData.waterPressDelta = udpRxMsg.canMsg.frame.data6;
				gatewayData.waterLevel = udpRxMsg.canMsg.frame.data7;	
			}
			if(udpRxMsg.canMsg.frame.id == 0x247){
				gatewayData.kotelMsgTimestamp = getSysTick();
				gatewayData.kotelStatus = udpRxMsg.canMsg.frame.data0;
				gatewayData.kotelActTemp = (float)((udpRxMsg.canMsg.frame.data1 << 8) | udpRxMsg.canMsg.frame.data2)/100;
				gatewayData.kotelTemHi = udpRxMsg.canMsg.frame.data3;
				gatewayData.kotelTemLo = udpRxMsg.canMsg.frame.data4;
				gatewayData.kotelTemRun = udpRxMsg.canMsg.frame.data5;
				gatewayData.kotelTemStop = udpRxMsg.canMsg.frame.data6;
				gatewayData.kotelTemDelta = udpRxMsg.canMsg.frame.data7;
			}
			udpRxMsg.canMsg.frame.id = 0;
			gpio_set_pin_level(&ldCanRx, false);
		 }
		tmpData = getRxByte();
		uint8_t tmpChr;
		if(tmpData != 0){
			tmpChr = 10;
		}
		if (serialEDataAvailable()) {
			gpio_set_pin_level(&hbRedLed, true);
			const char* data = serialEreadDataBuff();
			char header[4] = {0};
			char status[6] = {0};
			int16_t tmp_t1, tmp_t2, tmp_t3, tmp_light, tmp_adc;

			int parsed = sscanf(data, "%3[^;];%5[^;];%hu;%hu;%hu;%hu;%hu;",
			header, status,
			&tmp_t1, &tmp_t2, &tmp_t3,
			&tmp_light, &tmp_adc);

			if (parsed == 7 && strcmp(header, "S0") == 0) {
				
				
				extSensors.boilerTemperature = (tmp_t1 >= 0) ? (uint16_t)(tmp_t1/10) & 0xff : 0xff;
				extSensors.coolantTemperature = (tmp_t2 >= 0) ? (uint16_t)(tmp_t2/10) & 0xff : 0xff;
				extSensors.solarCollectorTemp = (tmp_t3 >= 0) ? (uint16_t)(tmp_t3/10) & 0xff : 0xff;
				extSensors.lightLevel = (uint16_t)(tmp_light >> 2) & 0xff;
				extSensors.auxAdcValue = (uint16_t)(tmp_adc >> 2) & 0xff;
				extSensors.timestamp = getSysTick();
				
				gatewayData.boilerTemperature = tmp_t1/10;
				gatewayData.coolantTemperature = tmp_t2/10;
				gatewayData.solarCollectorTemp = tmp_t3/10;
				gatewayData.lightLevel = tmp_light;
				
				
				printf("OK: [%s][%s] %d %d %d %d %d; dal: %d; t1: %d; t2: %d; t3: %d\n",
				header, status,
				tmp_t1, tmp_t2, tmp_t3,
				tmp_light, tmp_adc,
				gatewayData.dallasTemp8, extSensors.boilerTemperature, extSensors.coolantTemperature, extSensors.solarCollectorTemp);
				udpTempSensorUnitTxMsg.canMsg.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
				udpTempSensorUnitTxMsg.canMsg.frame.id = 0x069;
				udpTempSensorUnitTxMsg.canMsg.frame.dlc = 8;
				udpTempSensorUnitTxMsg.canMsg.frame.data0 = gatewayData.dallasTemp8;
				udpTempSensorUnitTxMsg.canMsg.frame.data1 = extSensors.boilerTemperature;
				udpTempSensorUnitTxMsg.canMsg.frame.data2 = extSensors.coolantTemperature;
				udpTempSensorUnitTxMsg.canMsg.frame.data3 = extSensors.solarCollectorTemp;
				udpTempSensorUnitTxMsg.canMsg.frame.data4 = extSensors.lightLevel;
				udpTempSensorUnitTxMsg.canMsg.frame.data5 = extSensors.auxAdcValue;
				udpTempSensorUnitTxMsg.canMsg.frame.data6 = 0x00;
				udpTempSensorUnitTxMsg.canMsg.frame.data7 = 0xaa;
				gatewayData.sensorUnitMsgTimestamp = getSysTick();
				} else {
				printf("Parse error: parsed=%d, raw='%s'\n", parsed, data);
			}
			gpio_set_pin_level(&hbRedLed, false);
		}
		if(getRtcInt()){
			
			rtc_sync(&sys_rtc);
			sprintf(&gatewayData.timeStamp, "%02d-%02d-20%02d %02d:%02d:%02d uptime: %us", sys_rtc.date, sys_rtc.month, sys_rtc.year, sys_rtc.hour, sys_rtc.minute, sys_rtc.second, getUpTime());
			gpio_toggle_pin_level(&ldRun);
			//serialEWriteString("usartE\n\r");
			
			//printf("serialE bytes: %d; Available: %d\n\r", serialEgetRxBytesNum(), serialEDataAvailable());
			//printf("RTC %d-%d-%d %d; %d:%d:%d %d; %d \n", sys_rtc.date, sys_rtc.month, sys_rtc.year, sys_rtc.dayofweek, sys_rtc.hour, sys_rtc.minute, sys_rtc.second, getRxByte(), getUpTime());
			

		}
		
		_delay_us(100);
		
		//S0;run;381;537;634;968;1;
		//S0;run;381;537;634;946;1;
		//S0;run;380;539;634;959;1;
		//S0;run;380;537;636;957;1;
		//                       ^Not used adc ch5
		//                   ^light sensor
		//				 ^ Collector
		//			 ^Collector water
		//       ^Boiler
		// period - 2s
		// ch - RXE; 250000kbps
		//O1 - kotel GAZ
		//O2 - Boiler
		
		// PORTA.1 - CAN error led
		// PORTA.2 - CAN activity led
		// PORTA.3 - LAN activity led
		// PORTA.4 - LAN error led
		// PORTA.5 - NET pin
		// PORTA.6 - Dallas thermal sensor
		gpio_set_pin_level(&ldHttpHandle, true);
		netHandler();
		gpio_set_pin_level(&ldHttpHandle, false);
		//COM5 - UART_E0 | PORTE.3 | PORTE.2
		
		
		
    }
	
	return 0;
}

