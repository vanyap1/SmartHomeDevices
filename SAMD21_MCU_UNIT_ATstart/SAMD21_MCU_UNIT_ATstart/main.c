#include "hw_driver.h"
#include "socket.h"
#include "http_parser.h"
#include "rtc.h"
#include "RFM69registers.h"
#include "RFM69.h"
#include "u8g2.h"
#include "displaySupport.h"
#include "usb_hid.h"
#include "stdint.h"
#include "stdbool.h"
#include "ff.h"
#include "mem25lXX.h"
#include "bms_ina22x.h"


u8g2_t lcd;
FATFS FatFs;
FIL Fil;
ramIDS spiFlash;
powerData mainBattery;



#define POWERBANKID		0x21
#define VFDSCREEN		0x03
#define DEVMODULE		0x22
#define DEVMODULE2		0xFE
#define RFGATEWAY		0xFC
#define RADIOMODE		TX_UNMUTE


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
#define TELEGRAM_MSG	0x87
#define MAIN_UPS		0x12		//Home ups



#define TX_MUTE			0
#define TX_UNMUTE		1
#define TX_MODE			TX_MUTE
uint8_t txLen;
uint8_t txCRC;
rfHeader rfTxDataPack;
uint8_t testMsg[65];
uint8_t upsData[64];

//#define ETHERNET
//#define  DISPLAY_VFD
#define  DISPLAY_OLED
//#define  DISPLAY_LCD
//#define I2C_USI


uint8_t updateScreen = 1;
uint32_t hidPacketCounter;
uint32_t rfPacketCounter;
uint8_t lcdMsg[64];

uint8_t trottle[2];
int8_t asics[4];
uint16_t btns;

#ifdef I2C_USI
	#define BOARD_ADDR	 0x27
	#define LCD_BLK			4
	#define EXT_LED			3
	#define EXT_SV1			2
	#define EXT_SV2			1
	#define EXT_SV3			0
	uint8_t outputs_regs[] = {0x06, 0x07, 0xff};
	uint8_t portValue[] = {0x02, 0xff, 0xff};
	uint8_t keyValues[2];
	uint8_t backLightTime = 10;
	
	#define INVERT_BIT(value, bitNumber) ((value) ^= (1 << (bitNumber)))
	#define SET_BIT(value, bitNumber) ((value) |= (1 << (bitNumber)))
	#define RESET_BIT(value, bitNumber) ((value) &= ~(1 << (bitNumber)))
	#define GET_BIT(value, bitNumber) (((value) >> (bitNumber)) & 0x01)
	#define BACKLIGHTTIME	10
	
#endif


#ifdef ETHERNET
	wiz_NetInfo netInfo = { .mac  = {0x20, 0xcf, 0xF0, 0x83, 0x78, 0x00}, // Mac address
	.ip   = {192, 168, 1, 101},         // IP address
	.sn   = {255, 255, 255, 0},         // Subnet mask
	.dns =  {8,8,8,8},			  // DNS address (google dns)
	.gw   = {192, 168, 1, 1}, // Gateway address
	.dhcp = NETINFO_STATIC};    //Static IP configuration

	uint16_t socketPort[8] = {80, 23, 23, 80, 8080, 8080, 8080, 5000};
	uint8_t rx_tx_buff_sizes[]={2,2,2,2,2,2,2,2};
	uint8_t udpTxDatagram[128];

	char http_ansver[128]="\0";
	//UDP pkg sender
	uint8_t result;
	uint8_t *testBuffer 	= "MCU USB HID says hello )";
	uint8_t  UdpDestAddress[4]		= { 192,168,1,255 };
	uint16_t UdpTxPort			= 3000;
	uint8_t	 UdpTxSockNum			= 0;
	uint16_t UdpRxPort			= 3001;
	uint8_t	 UdpRxSockNum			= 1;
#endif	

rtc_date sys_rtc = {
	.date = 4,
	.month = 12,
	.year = 2023,
	.dayofweek = 1,
	.hour = 23,
	.minute = 54,
	.second = 00
};
uint8_t timeSyncRequest = 0;




static uint8_t hid_generic_in_report[64];
static uint8_t hid_generic_out_report[64];
uint8_t packNum = 0;

static bool usb_device_cb_generic_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	hiddf_generic_read(hid_generic_out_report, sizeof(hid_generic_out_report));
	//hiddf_generic_read(hid_generic_out_report, 8);
	hidPacketCounter++;
	if (hid_generic_out_report[0] == 0) {
		// led be off
		switch (hid_generic_out_report[1]) {
			case 1:
			//gpio_set_pin_level(DB_LED, true);
			break;
			case 2:
			//gpio_set_pin_level(DB_LED, false);
			break;
		}
	}
	hid_generic_in_report[0] = 1;
	hid_generic_in_report[5] = 0x55;
	hid_generic_in_report[6] = GetBtnState() ? 0x00 : 0xff;
	hid_generic_in_report[60] = 0x55;
	hiddf_generic_write(hid_generic_in_report, sizeof(hid_generic_in_report));
	//hiddf_generic_write(hid_generic_in_report, 2);
	if(packNum == 255) packNum=0;
	packNum++;
	
	return true;
}

static bool usb_device_cb_generic_in(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	hidPacketCounter++;
	return true;
}




int main(void)
{
	//atmel_start_init();
	sys_hw_init();
	//usb_init();
	#ifdef ETHERNET
	reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_Write_byte);
	reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);
	
	wizphy_reset();
	delay_ms(100);
	wizchip_init(rx_tx_buff_sizes,rx_tx_buff_sizes);
	wizchip_setnetinfo(&netInfo);
	ctlnetwork(CN_SET_NETINFO, (void*) &netInfo);
	
	#endif
	
	#ifdef DISPLAY_VFD
		u8g2_Setup_gp1287ai_256x50_f(&lcd, U8G2_R0, vfd_spi, u8x8_avr_gpio_and_delay);
		u8g2_InitDisplay(&lcd);
		u8g2_SetPowerSave(&lcd, 0);
		u8g2_SetFlipMode(&lcd, 1);
		u8g2_SetContrast(&lcd, 7); //For VFD screen
		u8g2_ClearBuffer(&lcd);
		u8g2_SetFont(&lcd, u8g2_font_lucasarts_scumm_subtitle_o_tf);
		u8g2_DrawStr(&lcd, 1, 17, (void *)"RX MODULE");
		u8g2_SendBuffer(&lcd);
		
	#elif defined(DISPLAY_LCD)
		u8g2_Setup_st7586s_ymc240160_f(&lcd, U8G2_R0, vfd_spi, u8x8_avr_gpio_and_delay); // contrast: 80
		u8g2_InitDisplay(&lcd);
		u8g2_SetPowerSave(&lcd, 0);
		u8g2_SetFlipMode(&lcd, 1);
		u8g2_SetContrast(&lcd, 80); 
		u8g2_ClearBuffer(&lcd);
		u8g2_SendBuffer(&lcd);
		
	#elif defined(DISPLAY_OLED)	
		u8g2_Setup_ssd1306_i2c_128x64_noname_f(&lcd, U8G2_R0, u8x8_byte_sw_i2c, u8x8_avr_gpio_and_delay);
		u8g2_SetI2CAddress(&lcd, 0x3c);//3c
		u8g2_InitDisplay(&lcd);
		u8g2_SetPowerSave(&lcd, 0);
		u8g2_ClearBuffer(&lcd);
		u8g2_SetFont(&lcd, u8g2_font_lucasarts_scumm_subtitle_o_tf);
		u8g2_DrawStr(&lcd, 1, 17, (void *)"HELLO WORLD");
		u8g2_SendBuffer(&lcd);
	#endif
	
	#ifdef I2C_USI
		I2C_write_batch(BOARD_ADDR, (uint8_t *)&outputs_regs, 3);
	#endif
	
	chipGetIDS(&spiFlash);
	
	rfm69_init(868, NODEID, NETWORKID);
	rcCalibration();
	setHighPower(true);
	
	usb_HID_init();
	hiddf_generic_register_callback(HIDDF_GENERIC_CB_READ, (FUNC_PTR)usb_device_cb_generic_out);
	//hiddf_generic_register_callback(HIDDF_GENERIC_CB_WRITE, (FUNC_PTR)usb_device_cb_generic_in);
	hiddf_generic_read(hid_generic_out_report, sizeof(hid_generic_out_report));
	
	rtc_int_enable(&sys_rtc);
	
	if(!gpio_get_pin_level(SD_DETECT)){
		gpio_set_pin_level(LED_SD, true);
		UINT bw;
		FRESULT fr;
		f_mount(&FatFs, "", 0);		/* Give a work area to the default drive */
		fr = f_open(&Fil, "SAMD.txt", FA_WRITE | FA_CREATE_ALWAYS);	/* Create a file */
		if (fr == FR_OK) {
			f_write(&Fil, "It works!\r\n", 11, &bw);	/* Write data to the file */
			fr = f_close(&Fil);							/* Close the file */
		}
	}
	
	//u8g2_SendBuffer(&lcd);
	u8g2_ClearBuffer(&lcd);
	
	while (1) {
		//if(hiddf_generic_is_enabled() == 0){
		//	usb_HID_init();
		//	hiddf_generic_register_callback(HIDDF_GENERIC_CB_READ, (FUNC_PTR)usb_device_cb_generic_out);
		//	hiddf_generic_read(hid_generic_out_report, sizeof(hid_generic_out_report));
		//}
		u8g2_DrawRFrame(&lcd, 0, 0, 127 ,63, 0);
		
		
		#ifdef I2C_USI
			if (EXT_I2C_IRQ_isReady()){
				//delay_ms(1);
				I2C_read_batch(BOARD_ADDR, (uint8_t *)&keyValues, sizeof(keyValues));
				
				if (GET_BIT(keyValues[0], EXT_SV2) && GET_BIT(keyValues[0], EXT_SV3) == 0){
					SET_BIT(portValue[1], LCD_BLK);
					//backLightTime = BACKLIGHTTIME;
				}
				if (GET_BIT(keyValues[0], EXT_SV3) && GET_BIT(keyValues[0], EXT_SV2) == 0){
					RESET_BIT(portValue[1], LCD_BLK);
					//backLightTime = BACKLIGHTTIME;
				}
				if (GET_BIT(keyValues[0], EXT_SV1)==0){
					INVERT_BIT(portValue[1], EXT_LED);
					//backLightTime = BACKLIGHTTIME;
				}
				I2C_write_batch(BOARD_ADDR, (uint8_t *)&portValue, sizeof(portValue));
				
				//updateScreen = 1;
			}
		#endif
				
		
		if (RTC_IRQ_Ready())
		{
			gpio_toggle_pin_level(RLD);
			rtc_sync(&sys_rtc);
			updateScreen = 1;
			//sprintf(rtcData, "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
			if(readReg(REG_IRQFLAGS1) == RF_IRQFLAGS1_MODEREADY)
			{
				//clearFIFO();
				rfm69_init(868, NODEID, NETWORKID);
				rcCalibration();
				setHighPower(true);
			}
		}
		
		if (rf_isReady()) {
			gpio_set_pin_level(GLD, true);
			rfHeader* rfRxDataMsg=rfMsgType();
			rfPacketCounter++;
			switch(rfRxDataMsg->opcode) {
				case MSG:
				memcpy(&testMsg, DATA, rfRxDataMsg->rxtxBuffLenght);
				//DebugSerialWrite(testMsg, strlen(testMsg));
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
			gpio_set_pin_level(GLD, false);
		}
		
		#ifdef ETHERNET
		for(uint8_t HTTP_SOCKET = 3; HTTP_SOCKET <= 7; HTTP_SOCKET++){
			if(getSn_SR(HTTP_SOCKET) == SOCK_ESTABLISHED){
				uint8_t rIP[4];
				getsockopt(HTTP_SOCKET, SO_DESTIP, rIP);
				
				uint16_t res_size = getSn_RX_RSR(HTTP_SOCKET);
				if (res_size > sizeof(TCP_RX_BUF)){
					res_size = sizeof(TCP_RX_BUF);
				}
				memset(TCP_RX_BUF, 0, sizeof(DATA_BUFF_SIZE));

				sprintf(http_ansver ,"<p><span style=\"color: #00ff00;\"><strong>data</strong></span></p>\n\r");
				send(HTTP_SOCKET, (uint8_t*)http_ansver, strlen(http_ansver));    //Uncomment for TCP

				recv(HTTP_SOCKET, (uint8_t*)TCP_RX_BUF, res_size);
				
				//if(res_size != 0){ // Actual for telnet connection
				//send(HTTP_SOCKET, (uint8_t*)http_ansver, strlen(http_ansver));
				//}
				
				result = socket(UdpTxSockNum, Sn_MR_UDP, UdpTxPort, SF_IO_NONBLOCK);
				result = sendto(UdpTxSockNum, TCP_RX_BUF, res_size, UdpDestAddress, UdpTxPort);
				
				sprintf(http_ansver, "SOCKET NUM: %d;<br>RTC: %02d:%02d:%02d; \nRead bytes: %d<br>" , HTTP_SOCKET, sys_rtc.hour, sys_rtc.minute, sys_rtc.second,res_size);
				send(HTTP_SOCKET, (uint8_t*)http_ansver, strlen(http_ansver));	//Uncomment for TCP
				sprintf(http_ansver ,"IP:%d.%d.%d.%d<br>", rIP[0],rIP[1],rIP[2],rIP[3]);
				send(HTTP_SOCKET, (uint8_t*)http_ansver, strlen(http_ansver));	//Uncomment for TCP
				
				//send(HTTP_SOCKET, (uint8_t*)TCP_RX_BUF, strlen(TCP_RX_BUF));		//Uncomment for TCP
				
				
				
				
				char *get_request = strtok(TCP_RX_BUF, "\r");
				
				disconnect(HTTP_SOCKET);			//Uncomment for TCP
				
				if(getSn_SR(HTTP_SOCKET) == SOCK_CLOSE_WAIT){
					disconnect(HTTP_SOCKET);
					//close(HTTP_SOCKET);
				}
				
				if(getSn_SR(HTTP_SOCKET) == SOCK_CLOSED){
					socket(HTTP_SOCKET, Sn_MR_TCP, socketPort[HTTP_SOCKET], 0);
					listen(HTTP_SOCKET);
				}
			}
		}
		#endif
		
		
		if(updateScreen){
			//send_mouse_report(1, 0, 0x0, 0);
			//send_mouse_report(0, 0, 0x0, 0);
			
			
			updateScreen = 0;
			sprintf((void *)lcdMsg, "%02d:%02d:%02d. %d   ", sys_rtc.hour, sys_rtc.minute, sys_rtc.second,rfPacketCounter);
			
			u8g2_SetFont(&lcd, u8g2_font_smart_patrol_nbp_tr);
			u8g2_DrawStr(&lcd, 3, 12, (char *)lcdMsg);
			
			u8g2_SetFont(&lcd, u8g2_font_courR08_tr);
			u8g2_DrawLine(&lcd, 3, 13, 123, 13);
			sprintf((void *)upsData, "%2.1fV,%2.1fA,%3.1fW      ", (float)mainBattery.voltage/1000, (float)mainBattery.current/1000, mainBattery.power);
			u8g2_DrawStr(&lcd, 3, 23, (char *)upsData);
			
			sprintf((void *)upsData, "%dWh,%d%%", (int)mainBattery.energy, mainBattery.capacity);
			u8g2_DrawStr(&lcd, 3, 23+10, (char *)upsData);
			
			
			u8g2_SendBuffer(&lcd);
			u8g2_ClearBuffer(&lcd);
		}
		
		
	}
}
