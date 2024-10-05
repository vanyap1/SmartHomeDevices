#include <atmel_start.h>
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
//#include "stdio.h"
#include <stdio.h>
#include <stdlib.h>

#include "driver_examples.h"
#include "driver.h"
#include "u8g2.h"

#include "RFM69.h"
#include "RFM69registers.h"
#include "ws28_led.h"
#include "rtc.h"
#include "bms_ina22x.h"
#include "mem25lXX.h"

#define POWERBANKID		0x21
#define VFDSCREEN		0x03
#define DEVMODULE		0x22
#define DEVMODULE2		0xFE



#define NETWORKID		33
#define NODEID			DEVMODULE2
#define ALLNODES		0xfe
#define SMARTSCREEN		0xf0
#define RTC_SYNC		0x81
#define MSG				0x82
#define POWERBANK		0x83
#define GPIO_CTRL		0x84		//reserved for lora relay module (Send with tis ID to module)
#define GPIO_INFO		0x85		//reserved for lora relay module (Module will answer with this ID)
#define GPIO_ALARM		0x86		//reserved for lora relay module (Alarm message)
#define MAIN_UPS		0x12		//Home ups


#define LCD_BLK			4
#define EXT_LED			3
#define EXT_SV1			2
#define EXT_SV2			1
#define EXT_SV3			0
#define UI_MODULE		0x27

#define TX_MUTE			0
#define TX_UNMUTE		1

#define TX_MODE			TX_UNMUTE

#define INVERT_BIT(value, bitNumber) ((value) ^= (1 << (bitNumber)))
#define SET_BIT(value, bitNumber) ((value) |= (1 << (bitNumber)))
#define RESET_BIT(value, bitNumber) ((value) &= ~(1 << (bitNumber)))
#define GET_BIT(value, bitNumber) (((value) >> (bitNumber)) & 0x01)


//BMS
#define  INA226ADR					0x40
powerData battery;
powerData mainBattery;


u8g2_t lcd;
ramIDS ramInfo;


uint8_t txLen;
uint8_t txCRC;
rfHeader rfTxDataPack;


uint16_t seco, mine, hour;
uint32_t pack_counter = 0;
uint32_t last_pkg_time;
uint8_t rf_tx_str[32] = "\0";
uint8_t updateScreen = 1;


rtc_date sys_rtc = {
	.date = 31,
	.month = 3,
	.year = 24,
	.dayofweek = 1,
	.hour = 13,
	.minute = 45,
	.second = 00
};
//u8g2 ref url - https://github.com/olikraus/u8g2/wiki/u8g2reference
//u8g2.setDrawColor(1);
//u8g2.setBitmapMode(1);

rgb_led led1 = {255, 128, 1};

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	EXT_SPI_init();
	RF_SPI_init();
	EXT_I2C_init();
	RTC_I2C_init();
	rtc_int_enable(&sys_rtc);
	DEBUG_SERIAL_init();
	rfm69_init(868, NODEID, NETWORKID);
	setHighPower(true);
	
	
	
	//EXTERNAL_IRQ_0_example();
	
	//uint8_t rssiData[] = "                    \n\r";
	//uint8_t rf_str[64];
	//uint8_t keyVal[] =    "                    \n\r";
	uint8_t testMsg[64];
	uint8_t rtcData[64];
	uint8_t batData[64];
	uint8_t upsData[64];
	//uint8_t spiRamData[33];
	//uint8_t spiDataBuffer[16];
	//char debugString[128];
	//uint8_t outputs_regs[] = {0x06, 0x07, 0xff};
	uint8_t portValue[] = {0x02, 0xff, 0xff};
	uint8_t keyValues[2];
	//uint8_t intCount = 0;
	uint8_t lcdInitReq = 0;
	rtc_set(&sys_rtc);
	
	
	
	//I2C_write_batch(0x27, (uint8_t *)&outputs_regs, 3);

	
	
	
	u8g2_Setup_gp1287ai_256x50_f(&lcd, U8G2_R0, vfd_spi, u8x8_avr_gpio_and_delay);
	//u8g2_Setup_st7565_zolen_128x64_f( &lcd, U8G2_MIRROR_VERTICAL, vfd_spi, u8x8_avr_gpio_and_delay);	// contrast: 110
	//u8g2_Setup_st7586s_ymc240160_f(&lcd, U8G2_R0, vfd_spi, u8x8_avr_gpio_and_delay); // contrast: 80
	u8g2_InitDisplay(&lcd);
	//
	u8g2_SetPowerSave(&lcd, 0);
	u8g2_SetFlipMode(&lcd, 1);
	//u8g2_SetContrast(&lcd, 80); //For LCD screen

	u8g2_SetContrast(&lcd, 5); //For VFD screen
	//
	u8g2_ClearBuffer(&lcd);
	u8g2_SendBuffer(&lcd);
	
	
	
	//WDT->CLEAR.reg=0x5a;
	//u8g2_SetFont(&lcd, u8g2_font_ncenB14_tr);
	//u8g2_SetFont(&lcd, u8g2_font_courR08_tr);
	//u8g2_SetFont(&lcd, u8g2_font_cu12_t_symbols);
	u8g2_SetFont(&lcd, u8g2_font_Terminal_tr);
	//u8g2_SetFont(&lcd, u8g2_font_battery19_tn); //battery icons
	
	/* Replace with your application code */
	u8g2_DrawRFrame(&lcd, 0, 0, 256 ,48, 5);
	u8g2_DrawStr(&lcd, 4, 12, (char *)__DATE__);//
	u8g2_DrawStr(&lcd, 90, 12, (char *)__TIME__);//
	u8g2_SetFont(&lcd, u8g2_font_courR08_tr);
	u8g2_DrawStr(&lcd, 4, 20, (char *)__TIMESTAMP__);//
	u8g2_SendBuffer(&lcd);
	
	//PowerMeterInit(INA226ADR);
	
	delay_ms(100);
	u8g2_ClearBuffer(&lcd);
	
	
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
	GCLK_CLKCTRL_CLKEN |
	GCLK_CLKCTRL_GEN_GCLK3;
	//WDT->INTENSET.bit.EW  = 0;      // Enable early warning interrupt
	//WDT->EWCTRL.bit.EWOFFSET = 0x0B;  // Early Warning Interrupt Time Offset 0xA
	//WDT->CONFIG.bit.PER    = 0xB;  // Set period for chip reset 0xB 16384 clock cycles
	WDT->CLEAR.bit.CLEAR = 0xA5;
	//WDT->CTRL.bit.WEN      = 0;      // Disable window mode
	//while(WDT->STATUS.bit.SYNCBUSY); // Sync CTRL write
	//WDT->CTRL.bit.ENABLE = 1; // Start watchdog now!
	//while(WDT->STATUS.bit.SYNCBUSY);

	//I2C_write_batch(panel_addr, reset_cmd, sizeof(reset_cmd));
	//delay_ms(10);
	//I2C_write_batch(panel_addr, init_cmd, sizeof(init_cmd));
	//delay_ms(10);
	
	
	gpio_set_pin_level(LED_G, false);
	//SPI_PIXEL_init();
	
	
	
	
	while (1) {
		WDT->CLEAR.bit.CLEAR = 0xA5;
		while(WDT->STATUS.bit.SYNCBUSY);
		//gpio_toggle_pin_level(LED_G);
		//delay_ms(100);
		
		//neopixelWrite();
		//u8g2_SetFont(&lcd, u8g2_font_Terminal_tr);
		//u8g2_DrawRFrame(&lcd, 0, 0, 256 ,48, 5); //LCD 128, 64 // vfd 256,48
		//u8g2_DrawLine(&lcd, 3,12, 124,12);
		//u8g2_DrawStr(&lcd, 20, 11, (char *)"LIFEPO4");//
		
		//DebugSerialWrite((uint8_t *)"test_msg\n\r" , 10);
		
		
		
		
		//GET_BIT(keyValues[0], EXT_SV1) //Get UI button status
		//INVERT_BIT(port_values[1], 3);
		
		
		
		
		if (EXT_I2C_IRQ_isReady()){
			delay_ms(10);
			I2C_read_batch(UI_MODULE, (uint8_t *)&keyValues, sizeof(keyValues));
			
			if (GET_BIT(keyValues[0], EXT_SV3) && GET_BIT(keyValues[0], EXT_SV1) == 0){
				RESET_BIT(portValue[1], LCD_BLK);
			}
			if (GET_BIT(keyValues[0], EXT_SV1) && GET_BIT(keyValues[0], EXT_SV3) == 0){
				SET_BIT(portValue[1], LCD_BLK);
			}
			I2C_write_batch(UI_MODULE, (uint8_t *)&portValue, sizeof(portValue));
			updateScreen = 1;
		}
		
		
		
		
		
		
		if (rf_isReady()) {
			rfHeader* rfRxDataMsg=rfMsgType();
			//sprintf((char *)rf_str , "%02X%02X%02X%02X%02X;%03d",  rfRxDataMsg->senderAddr, rfRxDataMsg->destinationAddr, rfRxDataMsg->opcode,  rfRxDataMsg->rxtxBuffLenght,  rfRxDataMsg->dataCRC, rfRxDataMsg->rssi);
			//sprintf((char *)rssiData, "RSSI:%04d;" , rfRxDataMsg->rssi);
			
			//sprintf(debugString, "FLG=:%02X[D8]\n\r", readReg(REG_IRQFLAGS1));
			
			//for (int i = 0; i < rfRxDataMsg->rxtxBuffLenght; ++i) {
			//	sprintf(&debugString[i * 2], "%02X", DATA_BUFF[i]);
			//}
			
			//DebugSerialWrite(rf_str, strlen(rf_str));
			//DebugSerialWriteln(debugString, strlen(debugString));
			
			switch(rfRxDataMsg->opcode) {
				case MSG:
					if(simpleCRC(&DATA_BUFF, rfRxDataMsg->rxtxBuffLenght) == rfRxDataMsg->dataCRC){
						memcpy(&testMsg, (void *)DATA_BUFF, sizeof(testMsg));
					}
				break;
				case POWERBANK:
					if(simpleCRC(&DATA_BUFF, rfRxDataMsg->rxtxBuffLenght) == rfRxDataMsg->dataCRC){
						memcpy(&battery, (void *)DATA_BUFF, sizeof(battery));
					}
				break;
				
				case MAIN_UPS:
					if(simpleCRC(&DATA_BUFF, rfRxDataMsg->rxtxBuffLenght) == rfRxDataMsg->dataCRC){
						memcpy(&mainBattery, (void *)DATA_BUFF, sizeof(mainBattery));
					}
				break;
				
				case RTC_SYNC:
					if(simpleCRC(&DATA_BUFF, rfRxDataMsg->rxtxBuffLenght) == rfRxDataMsg->dataCRC){
						memcpy(&sys_rtc, (void *)DATA_BUFF, sizeof(sys_rtc));
						rtc_set(&sys_rtc);
					}
				if(sys_rtc.second == 0){lcdInitReq=1;}
				break;
				default:
				delay_us(1);
				//gpio_toggle_pin_level(LED_G);
			}
			updateScreen = 1;
		}
		
		
		
		
		
		if (RTC_IRQ_Ready())
		{
			rtc_sync(&sys_rtc);
			//PowerMeterMeasure(&battery);
			
			sprintf((void *)rtcData, "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);			

			
			if(TX_MODE){
				if (sys_rtc.second == 0)
				{
					rfTxDataPack.destinationAddr = ALLNODES;
					rfTxDataPack.senderAddr = NODEID;
					rfTxDataPack.opcode = RTC_SYNC;
					rfTxDataPack.rxtxBuffLenght = sizeof(sys_rtc);
					rfTxDataPack.dataCRC = simpleCRC((void *)&sys_rtc, sizeof(sys_rtc));
					sendFrame(&rfTxDataPack, &sys_rtc);
				}
				
			}
			
			
			//delay_ms(100);
			//
			//rfTxDataHeader.destinationAddr = ALLNODES;
			//rfTxDataHeader.senderAddr = NODEID;
			//rfTxDataHeader.opcode = MSG;
			//rfTxDataHeader.rxtxBuffLenght = sizeof(testMsg);
			//rfTxDataHeader.dataCRC = simpleCRC(&testMsg, sizeof(testMsg));
			//sendFrame(&rfTxDataHeader, &testMsg);
			updateScreen = 1;
		}
		
		if(updateScreen){
			updateScreen = 0;
			
			//sprintf(keyVal, "%02d;%02d;" , keyValues[0], keyValues[1]);
			//u8g2_DrawStr(&lcd, 70, 23, (char *)keyVal);
			//u8g2_DrawStr(&lcd, 3, 23, (char *)rssiData);
			
			

			
			
			
			//LCD
			
			sprintf((void *)rtcData, "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
			u8g2_SetFont(&lcd, u8g2_font_ncenB14_tr);
			//u8g2_SetFont(&lcd, u8g2_font_smart_patrol_nbp_tr);
			u8g2_DrawStr(&lcd, 1, 15, (char *)rtcData);
			
			
			
			
			u8g2_SetFont(&lcd, u8g2_font_courR08_tr);
			u8g2_DrawLine(&lcd, 82, 0, 82, 18);
			sprintf((void *)batData, "%2.1f,%2.1f,%3.1fW,%dWh,%d%%", (float )battery.voltage/1000, (float)battery.current/1000, battery.power, (int)battery.energy, battery.capacity);
			u8g2_DrawStr(&lcd, 84, 7, (char *)batData);
			
			sprintf((void *)upsData, "%2.1f,%2.1f,%3.1fW,%dWh,%d%%", (float)mainBattery.voltage/1000, (float)mainBattery.current/1000, mainBattery.power, (int)mainBattery.energy, mainBattery.capacity);
			u8g2_DrawStr(&lcd, 84, 17, (char *)upsData);
			
			u8g2_DrawStr(&lcd, 1, 48, (char *)testMsg);
			
			
			
			
			
			
			
			//u8g2_DrawStr(&lcd, 166, 8, (char *)rf_str);
			
			
			
			if(lcdInitReq){
				lcdInitReq=0;
			}
			
			
			//u8g2_DrawStr(&lcd, 160, 26, (char *)batData);
			if(sys_rtc.second == 0){
				u8g2_InitDisplay(&lcd);
				u8g2_SetPowerSave(&lcd, 0);
				u8g2_SetFlipMode(&lcd, 1);
				u8g2_SetContrast(&lcd, 5);
			}
			u8g2_SendBuffer(&lcd);
			u8g2_ClearBuffer(&lcd);
		}
		
		
		
		
		
		//VFD
		//u8g2_DrawLine(&lcd, 110, 12, 110, 48);
		//u8g2_DrawStr(&lcd, 113, 23, (char *)rtcData);
		//u8g2_DrawStr(&lcd, 113, 33, (char *)testMsg);
		
		
		
		
		//u8g2_DrawStr(&lcd, 113, 43, (char *)testMsg);
		
		//if(lcdInitReq){
		//u8g2_InitDisplay(&lcd);
		//u8g2_SetPowerSave(&lcd, 0);
		//u8g2_SetFlipMode(&lcd, 1);
		//u8g2_SetContrast(&lcd, 5); //For VFD screen
		//lcdInitReq = 0;
		//}
		
		//u8g2_UserInterfaceMessage(&lcd, "title1", "title2", "title3", "OK");
		//u8g2_SetFontRefHeightAll(&lcd);  	/* this will add some extra space for the text inside the buttons */
		//u8g2_UserInterfaceMessage(&lcd,"Title1", "Title2", "Title3", " Ok \n Cancel ");
		//u8g2_UserInterfaceSelectionList(&lcd, "Title", 2, "abcdef\nghijkl\nmnopqr");
		
		//ramGetIDS(&ramInfo);
		//sprintf(spiRamData, "%02X %02X %02X, %02X", ramInfo.manufacturerID, ramInfo.memoryType, ramInfo.memoryDensity, ramInfo.electronicID);
		
		//spiRamData[] = "123456890123456";
		
		
		
		//ramReadPage(&spiDataBuffer, 0, sizeof(spiDataBuffer));
		//for (int i = 0; i < 16; ++i) {
		//sprintf(&spiRamData[i * 2], "%02X", spiDataBuffer[i]);
		//}
		//spiRamData[32] = '\0';
		//u8g2_DrawStr(&lcd, 3, 90, (char *)spiRamData);
		
		
		
		
		//ramErasePage(0);
		
		//DebugSerialWriteByte('D');
		
	}
}
