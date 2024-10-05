#include <atmel_start.h>
#include "pin_config.h"
#include <string.h>
#include "hw_driver.h"

#include "socket.h"
#include "http_parser.h"

#include "RFM69registers.h"
#include "RFM69.h"
#include "rtc.h"
#include "u8g2.h"
#include "displaySupport.h"
#include "bms_ina22x.h"
#include "string.h"

u8g2_t lcd;
powerData mainBattery;


#define POWERBANKID		0x21
#define VFDSCREEN		0x03
#define DEVMODULE		0x22
#define DEVMODULE2		0xFE
#define DEVUSBHID		0xFD


#define NETWORKID		33
#define NODEID			DEVUSBHID
#define ALLNODES		0xfe
#define SMARTSCREEN		0xf0
#define RX_MODE			1
#define RTC_SYNC		0x81
#define MSG				0x82
#define POWERBANK		0x83
#define MAIN_UPS		0x12		//Home ups



#define TX_MUTE			0
#define TX_UNMUTE		1
#define TX_MODE			TX_UNMUTE


uint8_t txLen;
uint8_t txCRC;
rfHeader rfTxDataPack;



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

wiz_NetInfo netInfo = { .mac  = {0x20, 0xcf, 0xF0, 0x82, 0x76, 0x40}, // Mac address
.ip   = {192, 168, 1, 10},         // IP address
.sn   = {255, 255, 255, 0},         // Subnet mask
.dns =  {8,8,8,8},			  // DNS address (google dns)
.gw   = {192, 168, 1, 1}, // Gateway address
.dhcp = NETINFO_STATIC};    //Static IP configuration

uint16_t socketPort[8] = {80, 23, 23, 80, 8080, 8080, 8080, 5000};
uint8_t rx_tx_buff_sizes[]={2,2,2,2,2,2,2,2};
	
uint8_t udpTxDatagram[128]; 


//UDP pkg sender 
uint8_t result;
uint8_t *testBuffer 	= "Wiznet Says Hi!";
uint8_t  UdpDestAddress[4]		= { 192,168,1,255 };
uint16_t UdpTxPort			= 3000;
uint8_t	 UdpTxSockNum			= 0;
uint16_t UdpRxPort			= 3001;
uint8_t	 UdpRxSockNum			= 1;

uint8_t	TelnetSockNum		= 2;
uint8_t lcdUpdateReq = 0;
uint8_t displayString[32];

uint8_t rtcData[64];	
uint8_t testMsg[64];	
uint8_t udpRxBuff[64];

static uint8_t hid_generic_in_report[64];
static uint8_t hid_generic_out_report[64];
static uint8_t b_btn_last_state;

static bool usb_device_cb_generic_out(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	hiddf_generic_read(hid_generic_out_report, 64);
	
	if (hid_generic_out_report[0] == 0) {
		// led be off
		switch (hid_generic_out_report[1]) {
			case 1:
			gpio_set_pin_level(GLD, true);
			break;
			case 2:
			gpio_set_pin_level(GLD, false);
			break;
		}
	}
	hid_generic_in_report[5] = 0x55;
	hid_generic_in_report[6] = GetBtnState() ? 0x00 : 0xff;
	hiddf_generic_write(hid_generic_in_report, 64);
	return false;
}

//static void usbd_sof_event(void)
//{
	//static uint16_t interval;
	//if (interval++ > 100) {
		//interval = 0;
		////usb routine
		////gpio_toggle_pin_level(GLD);
	//}
//}
//
//static struct usbdc_handler usbd_sof_event_h = {NULL, (FUNC_PTR)usbd_sof_event};


int main(void)
{
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_WDT |
	GCLK_CLKCTRL_CLKEN |
	GCLK_CLKCTRL_GEN_GCLK3;
	WDT->CLEAR.bit.CLEAR = 0xa5;

	
	//atmel_start_init();
	RTC_init();
	rtc_set(&sys_rtc);
	rtc_int_enable(&sys_rtc);
	mcu_init();
	//rtc_set(&sys_rtc);
	rtc_int_enable(&sys_rtc);
	
		reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
		reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_Write_byte);
		reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);
		
		//u8g2_Setup_ssd1306_i2c_128x32_univision_f(&lcd, U8G2_R0, u8x8_byte_sw_i2c, fake_delay_fn);
		//delay_ms(100);
		u8g2_Setup_ssd1309_i2c_128x64_noname2_f(&lcd, U8G2_R0, u8x8_byte_sw_i2c, fake_delay_fn);
				
		u8g2_SetI2CAddress(&lcd, 0x3d);//3c
		u8g2_InitDisplay(&lcd);
		WDT->CLEAR.bit.CLEAR = 0xa5;

		
		WDT->CLEAR.bit.CLEAR = 0xa5;
		
		u8g2_SetPowerSave(&lcd, 0);
		//u8g2_SetFlipMode(&lcd, 1);
		//u8g2_SetContrast(&lcd, 120);
		
		u8g2_ClearBuffer(&lcd);
		//u8g2_SetFont(&lcd, u8g2_font_5x8_t_cyrillic);
		//u8g2_SetFont(&lcd, u8g2_font_6x10_mf);
		u8g2_SetFont(&lcd, u8g2_font_ncenB10_tr);
		//u8g2_SetFont(&lcd, u8g2_font_lucasarts_scumm_subtitle_o_tf);
		
		sprintf(displayString, "ip: %d.%d.%d.%d", netInfo.ip[0], netInfo.ip[1], netInfo.ip[2], netInfo.ip[3]);
		u8g2_DrawStr(&lcd, 1, 12, (void *)"RF GATEWAY");
		u8g2_DrawStr(&lcd, 1, 25, displayString);
		u8g2_SendBuffer(&lcd);
		
		
		rfm69_init(868, NODEID, NETWORKID);
		WDT->CLEAR.bit.CLEAR = 0xa5;
		setHighPower(true);
		
		
		wizphy_reset();
		WDT->CLEAR.bit.CLEAR = 0xa5;
		delay_ms(100);

		wizchip_init(rx_tx_buff_sizes,rx_tx_buff_sizes);
		wizchip_setnetinfo(&netInfo);
		ctlnetwork(CN_SET_NETINFO, (void*) &netInfo);


		char http_ansver[128]="\0";
		
	
	
	//hid_generic_example();
	//usbdc_register_handler(USBDC_HDL_SOF, &usbd_sof_event_h);
	
	//hiddf_generic_register_callback(HIDDF_GENERIC_CB_READ, (FUNC_PTR)usb_device_cb_generic_out);
	//hiddf_generic_read(hid_generic_out_report, 64);

	uint8_t iAddr = 10;
	//u8g2_SetContrast(&lcd, 0);
	
	/* Replace with your application code */
	
	while (1) {
		WDT->CLEAR.bit.CLEAR = 0xa5;
		while(WDT->STATUS.bit.SYNCBUSY);
		
		
		
		if (RTC_IRQ_Ready())
		{
			rtc_sync(&sys_rtc);
			sprintf(rtcData, "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
			
			
			if(TX_MODE){
				if(timeSyncRequest){
					rfTxDataPack.destinationAddr = ALLNODES;
					rfTxDataPack.senderAddr = NODEID;
					rfTxDataPack.opcode = RTC_SYNC;
					rfTxDataPack.rxtxBuffLenght = sizeof(sys_rtc);
					rfTxDataPack.dataCRC = simpleCRC(&sys_rtc, sizeof(sys_rtc));
					sendFrame(&rfTxDataPack, &sys_rtc);
					timeSyncRequest = 0;
				}
				
			}
			
			//result = socket(0, Sn_MR_UDP, port, SF_IO_NONBLOCK);
			//result = sendto(0, testBuffer, strlen(testBuffer), address, port);
			
			
			gpio_toggle_pin_level(RLD);
			lcdUpdateReq = 1;
			
			
		}
		
		if (rf_isReady()) {
			gpio_set_pin_level(GLD, true);
			gpio_set_pin_level(RF_LED, true);
			rfHeader* rfRxDataMsg=rfMsgType();
			
			size_t msgHeaderSize = sizeof(rfHeader);			
			memcpy(udpTxDatagram, rfRxDataMsg, msgHeaderSize);
			memcpy(udpTxDatagram + sizeof(rfHeader), DATA, rfRxDataMsg->rxtxBuffLenght);
			
			result = socket(UdpTxSockNum, Sn_MR_UDP, UdpTxPort, SF_IO_NONBLOCK);
			result = sendto(UdpTxSockNum, udpTxDatagram, msgHeaderSize+rfRxDataMsg->rxtxBuffLenght, UdpDestAddress, UdpTxPort);
			
			
			switch(rfRxDataMsg->opcode) {
				case MSG:
				memcpy(&testMsg, DATA, sizeof(testMsg));
				break;
				case RTC_SYNC:
				
				memcpy(&sys_rtc, DATA, sizeof(sys_rtc));
				rtc_set(&sys_rtc);
				//if(sys_rtc.second == 0){lcdInitReq=1;}
				break;
				default:
				delay_us(1);
				//gpio_toggle_pin_level(GLD);
			}
			gpio_set_pin_level(GLD, false);
			gpio_set_pin_level(RF_LED, false);
			}
		
		
		if(getSn_SR(UdpRxSockNum) == SOCK_CLOSED){
			socket(UdpRxSockNum, Sn_MR_UDP, UdpRxPort, SF_MULTI_ENABLE);
			listen(UdpRxSockNum);
		}
		
		uint16_t udp_size = getSn_RX_RSR(UdpRxSockNum);
		if (udp_size != 0) {
			uint8_t rIP[4];
			getsockopt(TelnetSockNum, SO_DESTIP, rIP);
			
			gpio_set_pin_level(ETH_LED, true);
			uint8_t ip[4];
			uint16_t port;
			if (udp_size > TCP_RX_BUF) udp_size = TCP_RX_BUF;
			
			memset(TCP_RX_BUF, 0, sizeof(TCP_RX_BUF));
			uint16_t ret = recvfrom(UdpRxSockNum, (uint8_t*)TCP_RX_BUF, udp_size, ip, &port);

			if(TCP_RX_BUF[0] == 0xaa & TCP_RX_BUF[1] == RTC_SYNC){
				memcpy(&sys_rtc, &TCP_RX_BUF[2], sizeof(sys_rtc));
				rtc_set(&sys_rtc);
				timeSyncRequest = 1;
			}
			else if(TCP_RX_BUF[0] == 0xaa & TCP_RX_BUF[1] == MSG){
				memcpy(&http_ansver, &TCP_RX_BUF[2], strlen(TCP_RX_BUF));
				if(TX_MODE == TX_UNMUTE){
					rfTxDataPack.destinationAddr = ALLNODES;
					rfTxDataPack.senderAddr = NODEID;
					rfTxDataPack.opcode = MSG;
					rfTxDataPack.rxtxBuffLenght = strlen(http_ansver);
					rfTxDataPack.dataCRC = simpleCRC(&http_ansver, strlen(TCP_RX_BUF));
					gpio_set_pin_level(RF_LED, true);
					sendFrame(&rfTxDataPack, &http_ansver);
					gpio_set_pin_level(RF_LED, false);
				}	
			}
			else if(TCP_RX_BUF[0] == 0xaa & TCP_RX_BUF[1] == MAIN_UPS){
				memcpy(&mainBattery, &TCP_RX_BUF[2], sizeof(mainBattery));
				if(TX_MODE == TX_UNMUTE){
					rfTxDataPack.destinationAddr = ALLNODES;
					rfTxDataPack.senderAddr = NODEID;
					rfTxDataPack.opcode = MAIN_UPS;
					rfTxDataPack.rxtxBuffLenght = sizeof(mainBattery);
					rfTxDataPack.dataCRC = simpleCRC((void *)&mainBattery, sizeof(mainBattery));
					gpio_set_pin_level(RF_LED, true);
					sendFrame(&rfTxDataPack, &mainBattery);
					gpio_set_pin_level(RF_LED, false);
				}
			}else{
				memset(&udpRxBuff, 0, sizeof(udpRxBuff));
				memcpy(&udpRxBuff, &TCP_RX_BUF,sizeof(TCP_RX_BUF));
				
				if(strcasecmp(udpRxBuff, "OUTP:STAT ON") == 0){
					sprintf(http_ansver, "ok");
					gpio_set_pin_level(LED_SD, true);
				
				}else if(strcasecmp(udpRxBuff, "OUTP:STAT OFF") == 0){
					sprintf(http_ansver, "ok");
					gpio_set_pin_level(LED_SD, false);
				
				}else if(strcasecmp(udpRxBuff, "MEAS:CURR?") == 0){
					sprintf(http_ansver, "0.121");
				
				}else if(strcasecmp(udpRxBuff, "MEAS:VOLT?") == 0){
					sprintf(http_ansver, "48.031");
					
				}else if(strcasecmp(udpRxBuff, "*RST") == 0){
				sprintf(http_ansver, "ok");
				}else{
					sprintf(http_ansver, "err");
				}
				result = socket(UdpTxSockNum, Sn_MR_UDP, UdpTxPort, SF_IO_NONBLOCK);
				result = sendto(UdpTxSockNum, (uint8_t*)http_ansver, strlen(http_ansver), UdpDestAddress, UdpTxPort);
			}
			gpio_set_pin_level(ETH_LED, false);
			lcdUpdateReq = 1;
		}
		
		
		//Telnet handler
		if(getSn_SR(TelnetSockNum) == SOCK_ESTABLISHED){
			uint8_t rIP[4];
			getsockopt(TelnetSockNum, SO_DESTIP, rIP);
			
			uint16_t res_size = getSn_RX_RSR(TelnetSockNum);
			if (res_size > sizeof(TCP_RX_BUF)){
				res_size = sizeof(TCP_RX_BUF);
			}
			memset(TCP_RX_BUF, 0, sizeof(DATA_BUFF_SIZE));
			recv(TelnetSockNum, (uint8_t*)TCP_RX_BUF, res_size);
			
			if(res_size != 0){
				sprintf(http_ansver ,"DONE\n\r");
				send(TelnetSockNum, (uint8_t*)http_ansver, strlen(http_ansver));
			}
			result = socket(UdpTxSockNum, Sn_MR_UDP, UdpTxPort, SF_IO_NONBLOCK);
			result = sendto(UdpTxSockNum, TCP_RX_BUF, res_size, UdpDestAddress, UdpTxPort);
		}
		if(getSn_SR(TelnetSockNum) == SOCK_CLOSE_WAIT){
			disconnect(TelnetSockNum);
			//close(HTTP_SOCKET);
		}
		
		if(getSn_SR(TelnetSockNum) == SOCK_CLOSED){
			socket(TelnetSockNum, Sn_MR_TCP, socketPort[TelnetSockNum], 0);
			listen(TelnetSockNum);
		}
		
		
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
				//if(strstr(get_request, "GET") != NULL && strstr(get_request, "favicon") == NULL){
					////uint16_t index = position - get_request;
					//printf("IP:%d.%d.%d.%d\r\n", rIP[0],rIP[1],rIP[2],rIP[3]);
					//printf("%s\n\r",get_request);
//
					//int value;
					//if(extractValue(get_request,"value1", &value)){
						//printf("value1 = %d\n\r", value);
					//}
					//
					//if(extractValue(get_request,"value2", &value)){
						//printf("value2 = %d\n\r", value);
					//}
					//char valueStr[20];
					//if(extractString(get_request,"time", valueStr)){
						//printf("time = %s\n\r", valueStr);
						//sscanf(valueStr, "%hhu:%hhu:%hhu", &sys_rtc.hour, &sys_rtc.minute, &sys_rtc.second);
						//rtc_set(&sys_rtc);
					//}
					//if(extractString(get_request,"help", valueStr)){
						//send(HTTP_SOCKET, (uint8_t*)TCP_RX_BUF, strlen(TCP_RX_BUF));
					//}
					////sprintf(http_ansver ,"HTTP/1.0 200 OK\n\r");
					////send(HTTP_SOCKET, (uint8_t*)http_ansver, strlen(http_ansver));
				//}
				//
			disconnect(HTTP_SOCKET);			//Uncomment for TCP
				//close(HTTP_SOCKET);
				
			}
			
			if(getSn_SR(HTTP_SOCKET) == SOCK_CLOSE_WAIT){
				disconnect(HTTP_SOCKET);
				//close(HTTP_SOCKET);
			}
			
			if(getSn_SR(HTTP_SOCKET) == SOCK_CLOSED){
				socket(HTTP_SOCKET, Sn_MR_TCP, socketPort[HTTP_SOCKET], 0);
				listen(HTTP_SOCKET);
			}
		}
		
		
		
		if(lcdUpdateReq){
			u8g2_ClearBuffer(&lcd);
			
			//u8g2_SetFont(&lcd, u8g2_font_6x10_mf);
			//u8g2_SetFont(&lcd, u8g2_font_ncenB14_tr);
			//u8g2_SetFont(&lcd, u8g2_font_lucasarts_scumm_subtitle_o_tf);
			//u8g2_SetFont(&lcd, u8g2_font_sirclive_tr);
			
			
			
			u8g2_SetFont(&lcd, u8g2_font_ncenB10_tr);
			
			u8g2_DrawStr(&lcd, 1, 12, (void *)"RF GATEWAY");
			
			sprintf(displayString, "IP: %d.%d.%d.%d", netInfo.ip[0], netInfo.ip[1], netInfo.ip[2], netInfo.ip[3]);
			u8g2_DrawStr(&lcd, 1, 25, displayString);
			
			u8g2_SetFont(&lcd, u8g2_font_haxrcorp4089_tr);
			sprintf(displayString, "mac: %02X:%02X:%02X:%02X:%02X:%02X", netInfo.mac[0], netInfo.mac[1], netInfo.mac[2], netInfo.mac[3], netInfo.mac[4], netInfo.mac[5]);
			u8g2_DrawStr(&lcd, 1, 37, displayString);
			
			
			
			
			
			u8g2_DrawStr(&lcd, 50, 47, (void *)udpRxBuff);
			
			sprintf(displayString, "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
			u8g2_DrawStr(&lcd, 86, 63, (void *)displayString);
			
			//sprintf(displayString, "%01d", (getPHYCFGR() & PHYCFGR_LNK_ON));
			//u8g2_DrawStr(&lcd, 1, 63, (void *)displayString);
			
			u8g2_SetFont(&lcd, u8g2_font_streamline_all_t);
			u8g2_DrawGlyph(&lcd, 1, 63, 0x01fd);
			if(TX_MODE == TX_MUTE){
				u8g2_DrawLine(&lcd, 1, 63-24, 24, 63);
				u8g2_DrawLine(&lcd, 1, 63, 24, 63-24);
			}
			if((getPHYCFGR() & PHYCFGR_LNK_ON)){
			u8g2_DrawGlyph(&lcd, 25, 63, 0x0081);
			}else{
			u8g2_DrawGlyph(&lcd, 25, 63, 0x0080);
			}
			
			u8g2_SendBuffer(&lcd);
			lcdUpdateReq=0;
		}
		
		
	}
}
