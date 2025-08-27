#include <atmel_start.h>
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
//#include "stdio.h"
#include <stdio.h>
#include <stdlib.h>

//#include "driver_examples.h"
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
#define RADIOMODE		TX_MUTE


#define NETWORKID		33
#define NODEID			POWERBANKID
#define ALLNODES		0xfe
#define SMARTSCREEN		0xf0
#define RX_MODE			1
#define RTC_SYNC		0x81
#define MSG				0x82
#define POWERBANK		0x83
#define REPORTMSGTIME	3
#define GPIO_CTRL		0x84		//reserved for lora relay module (Send with tis ID to module)
#define GPIO_INFO		0x85		//reserved for lora relay module (Module will answer with this ID)
#define GPIO_ALARM		0x86		//reserved for lora relay module (Alarm message)
#define MAIN_UPS		0x12		//Home ups




#define BAT_ICON		1
#define BAT_CHARGE		0
#define BAT_DISCHARGE	2
 
#define LCD_BLK			4
#define EXT_LED			3
#define EXT_SV1			2
#define EXT_SV2			1
#define EXT_SV3			0
#define UI_MODULE		0x27

#define BACKLIGHTTIME	10
#define TX_MUTE			0
#define TX_UNMUTE		1

#define INVERT_BIT(value, bitNumber) ((value) ^= (1 << (bitNumber)))
#define SET_BIT(value, bitNumber) ((value) |= (1 << (bitNumber)))
#define RESET_BIT(value, bitNumber) ((value) &= ~(1 << (bitNumber)))
#define GET_BIT(value, bitNumber) (((value) >> (bitNumber)) & 0x01)


//BMS
#define  INA226ADR					0x40
powerData battery;// = {.energy = 170.0f};
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

uint32_t sysTickSecond = 0;
uint32_t lastRfMsg = 0;



rtc_date sys_rtc = {
	.date = 4,
	.month = 12,
	.year = 23,
	.dayofweek = 1,
	.hour = 23,
	.minute = 54,
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
	

	
	uint8_t rssiData[24];
	uint8_t keyVal[24];
	uint8_t testMsg[256];
	uint8_t rtcData[64];
	uint8_t batData[64];
	uint8_t upsData[64];
	//uint8_t spiRamData[33];
	//uint8_t spiDataBuffer[16];
	char debugString[256];
	uint8_t outputs_regs[] = {0x06, 0x07, 0x00};
	uint8_t portValue[] = {0x02, 0xff, 0xff};
	uint8_t keyValues[2];
	uint8_t intCount = 0;
	rtc_set(&sys_rtc);
	uint8_t scrUpdateRequest = 0;
	uint8_t backLightTime = 10;
	uint8_t rfReportTime = REPORTMSGTIME;
	I2C_write_batch(0x27, (uint8_t *)&outputs_regs, 3);

	
	gpio_set_pin_level(LCD_RST, false);
	delay_ms(10);
	gpio_set_pin_level(LCD_RST, true);
	delay_ms(10);
	
	u8g2_Setup_st7565_zolen_128x64_f( &lcd, U8G2_MIRROR_VERTICAL, vfd_spi, u8x8_avr_gpio_and_delay);	// contrast: 110
	u8g2_InitDisplay(&lcd);

	u8g2_SetPowerSave(&lcd, 0);
	u8g2_SetFlipMode(&lcd, 1);
	u8g2_SetContrast(&lcd, 110); //For LCD screen

	u8g2_ClearBuffer(&lcd);
	u8g2_SendBuffer(&lcd);
	
	
	
	//WDT->CLEAR.reg=0x5a;
	//u8g2_SetFont(&lcd, u8g2_font_ncenB14_tr);
	//u8g2_SetFont(&lcd, u8g2_font_courR08_tr);
	//u8g2_SetFont(&lcd, u8g2_font_cu12_t_symbols);
	u8g2_SetFont(&lcd, u8g2_font_Terminal_tr);
	//u8g2_SetFont(&lcd, u8g2_font_battery19_tn); //battery icons
	
	u8g2_DrawRFrame(&lcd, 0, 0, 128 ,64, 5);
	u8g2_DrawStr(&lcd, 20, 15, (char *)__DATE__);//
	u8g2_DrawStr(&lcd, 35, 30, (char *)__TIME__);//
	u8g2_SetFont(&lcd, u8g2_font_courR08_tr);
	u8g2_DrawStr(&lcd, 7, 40, (char *)__TIMESTAMP__);//
	//u8g2_DrawStr(&lcd, 7, 49, (char *)__FILE__);//
	
	u8g2_SendBuffer(&lcd);
	u8g2_ClearBuffer(&lcd);
	
	PowerMeterInit(INA226ADR);
	delay_ms(2150);
	
	 
		
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
	GCLK_CLKCTRL_CLKEN |
	GCLK_CLKCTRL_GEN_GCLK3;	
	WDT->CLEAR.bit.CLEAR = 0xA5;
	gpio_set_pin_level(LED_G, false);
	
	//for(uint8_t i = 0; i<128; i++){
		//u8g2_SetContrast(&lcd, i);
		//sprintf((char *)rssiData, "%04d" , i);
		//u8g2_SetFont(&lcd, u8g2_font_courR08_tr);
		//u8g2_DrawStr(&lcd, 100, 11, (char *)rssiData);//
		//u8g2_SendBuffer(&lcd);
		//u8g2_ClearBuffer(&lcd);
		//gpio_toggle_pin_level(LED_G);
		//delay_ms(100);
		//
	//}
	while (1) {
		WDT->CLEAR.bit.CLEAR = 0xA5;
		while(WDT->STATUS.bit.SYNCBUSY);
		
		if (EXT_I2C_IRQ_isReady()){
			//delay_ms(1);
			I2C_read_batch(UI_MODULE, (uint8_t *)&keyValues, sizeof(keyValues));
			
			if (GET_BIT(keyValues[0], EXT_SV3) && GET_BIT(keyValues[0], EXT_SV1) == 0){
				SET_BIT(portValue[1], LCD_BLK);
				backLightTime = BACKLIGHTTIME;
			}
			if (GET_BIT(keyValues[0], EXT_SV1) && GET_BIT(keyValues[0], EXT_SV3) == 0){
				SET_BIT(portValue[1], LCD_BLK);
				backLightTime = BACKLIGHTTIME;
			}
			I2C_write_batch(UI_MODULE, (uint8_t *)&portValue, sizeof(portValue));
			
			scrUpdateRequest = 1;
		}
		//sprintf(keyVal, "%02d;%02d;" , keyValues[0], keyValues[1]);
		//u8g2_DrawStr(&lcd, 70, 23, (char *)keyVal);

		if (rf_isReady()) {
			gpio_set_pin_level(LED_G, true);
			rfHeader* rfRxDataMsg=rfMsgType();
			//sprintf((char *)rf_str , "%02X%02X%02X%02X%02X; RSSI: %d | ",  rfRxDataMsg->senderAddr, rfRxDataMsg->destinationAddr, rfRxDataMsg->opcode,  rfRxDataMsg->rxtxBuffLenght,  rfRxDataMsg->dataCRC, rfRxDataMsg->rssi);
			sprintf((char *)rssiData, "%04d" , rfRxDataMsg->rssi);
			//sprintf(debugString, "FLG=:%02X[D8]\n\r", readReg(REG_IRQFLAGS1));

			switch(rfRxDataMsg->opcode) {
				case MSG:
					memcpy(&testMsg, DATA, rfRxDataMsg->rxtxBuffLenght);
					DebugSerialWrite(testMsg, strlen(testMsg));					
					break;
				case RTC_SYNC:
					memcpy(&sys_rtc, DATA, sizeof(sys_rtc));
					rtc_set(&sys_rtc);
					break;
				
				case MAIN_UPS:
					memcpy(&mainBattery, (void *)DATA, sizeof(mainBattery));
					break;
				default:
					delay_us(1);
			}
			lastRfMsg = sysTickSecond;
			gpio_set_pin_level(LED_G, false);
			}
		
		if((sysTickSecond - lastRfMsg) > 60){
			lastRfMsg = sysTickSecond;
			rfm69_init(868, NODEID, NETWORKID);
			setHighPower(true);
		}
		
		if (RTC_IRQ_Ready())
		{	rtc_sync(&sys_rtc);
			PowerMeterMeasure(&battery);
			sysTickSecond++;
			if (backLightTime !=0){
				SET_BIT(portValue[1], LCD_BLK);
				backLightTime--;
			}else{
				RESET_BIT(portValue[1], LCD_BLK);
			}
			if (abs(battery.current) > 50){
				backLightTime=BACKLIGHTTIME;
			}
			
			
			
			if(mainBattery.current >= 2500){
				SET_BIT(portValue[2], BAT_CHARGE);
			}else{
				RESET_BIT(portValue[2], BAT_CHARGE);
			}
			
			if(mainBattery.current <= -2500){
				SET_BIT(portValue[2], BAT_DISCHARGE);
				}else{
				RESET_BIT(portValue[2], BAT_DISCHARGE);
			}
			
			
			
			if (battery.current > 50 || /*mainBattery*/ abs(mainBattery.current) > 1000){
				RESET_BIT(portValue[1], EXT_LED);
			}else{
				SET_BIT(portValue[1], EXT_LED);
			}
			
			if(rfReportTime==0 && RADIOMODE){
				rfReportTime=REPORTMSGTIME;
				rfTxDataPack.destinationAddr = ALLNODES;
				rfTxDataPack.senderAddr = NODEID;
				rfTxDataPack.opcode = POWERBANK;
				rfTxDataPack.rxtxBuffLenght = sizeof(battery);
				rfTxDataPack.dataCRC = simpleCRC(&battery, sizeof(battery));
				sendFrame(&rfTxDataPack, &battery);
			}else{
				rfReportTime--;
			}
			I2C_write_batch(UI_MODULE, (uint8_t *)&portValue, sizeof(portValue));
			scrUpdateRequest = 1;
		}
		
		if (scrUpdateRequest){
			//u8g2_InitDisplay(&lcd);
			
			#ifdef DEBUG
				u8g2_SetFont(&lcd, u8g2_font_courR08_tr);
				u8g2_DrawStr(&lcd, 100, 10, (char *)rssiData);//
				u8g2_DrawStr(&lcd, 58, 9, (char *)"|debug|");
				
				u8g2_SetFont(&lcd, u8g2_font_Terminal_tr);
				sprintf(rtcData, "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
				u8g2_DrawStr(&lcd, 5, 11, (char *)rtcData);
			#else
				u8g2_SetFont(&lcd, u8g2_font_courR08_tr);
				u8g2_DrawStr(&lcd, 100, 10, (char *)rssiData);//
				
				u8g2_SetFont(&lcd, u8g2_font_Terminal_tr);
				sprintf(rtcData, "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
				u8g2_DrawStr(&lcd, 30, 11, (char *)rtcData);
			#endif
			
			
			
			u8g2_DrawRFrame(&lcd, 0, 0, 128 ,64, 5);
			u8g2_DrawLine(&lcd, 3,12, 124,12);
			sprintf(batData, "%05dmV  %05dmA", battery.voltage, battery.current);
			u8g2_DrawStr(&lcd, 3, 23, (char *)batData);
						
			sprintf(batData, "%3.1fW  %3.3fWh", battery.power, battery.energy);
			u8g2_DrawStr(&lcd, 3, 33, (char *)batData);
			
			u8g2_SetFont(&lcd, u8g2_font_courR08_tr);
			
			sprintf(batData, "Cap: %02d%% Err: %2.1fWh", battery.capacity, battery.lastChargerErr);
			u8g2_DrawStr(&lcd, 3, 41, (char *)batData);
			
			u8g2_DrawLine(&lcd, 3,44, 124,44);
			sprintf((void *)upsData, "%2.1fV,%2.1fA,%3.1fW      ", (float)mainBattery.voltage/1000, (float)mainBattery.current/1000, mainBattery.power);
			u8g2_DrawStr(&lcd, 3, 54, (char *)upsData);
			
			sprintf((void *)upsData, "%dWh,%d%%", (int)mainBattery.energy, mainBattery.capacity);
			u8g2_DrawStr(&lcd, 3, 62, (char *)upsData);
			u8g2_SendBuffer(&lcd);
			u8g2_ClearBuffer(&lcd);
			scrUpdateRequest = 0;
		}
		
				
		//		
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
