
/*
 * netHandler.c
 *
 * Created: 29.12.2024 12:50:11
 *  Author: Ivan Prints
 */ 

#include "netConfig.h"
#include "config.h"

uint8_t result;
uint8_t rx_tx_buff_sizes[]={2,2,2,2,2,2,2,2};
uint16_t socketPort[8]		= {80, 23, 23, 80, 80, 80, 80, 80};
uint8_t  UdpDestAddress[4]	= {192,168,1,255};
uint16_t UdpTxPort			= 4040;
uint8_t	 UdpTxSockNum		= 0;
uint16_t UdpRxPort			= 4030;
uint8_t	 UdpRxSockNum		= 1;
char UdpAnsver[128]="\0";




float amp = 0.5, volt = 3.3;
float ampDMM = 2.15, voltDMM = 14.4;
uint8_t outState = 2;
uint8_t remoteCtrl = 0;
uint8_t psuErr = 0;







wiz_NetInfo netInfo = {
	.mac  = {0x21, 0xaf, 0xF0, 0xc2, 0x46, 0x15},	// Mac address debug tool 
	//.mac  = {0x20, 0xcf, 0xF0, 0x82, 0x46, 0x00},	// Mac address
	.ip   = {192, 168, 1, 20},						// IP address
	.sn   = {255, 255, 255, 0},						// Subnet mask
	.dns =  {8,8,8,8},								// DNS address (google dns)
	.gw   = {192, 168, 1, 1},						// Gateway address
.dhcp = NETINFO_STATIC};						//Static IP configuration

gatewayDto* gatewayValues;
uint8_t netInit(gatewayDto *gatewayData){
	gatewayValues = gatewayData;
	psuErr = 1;
	
	reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_Write_byte);
	reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);
	wizphy_reset();
	_delay_ms(100);
	wizchip_init(rx_tx_buff_sizes,rx_tx_buff_sizes);
	//netInfo.ip[3] = 90+GetIpSwitch();
	wizchip_setnetinfo(&netInfo);
	ctlnetwork(CN_SET_NETINFO, (void*) &netInfo);
	
	
	setIMR(0xff);
	setSIMR(0xff);
	setINTLEVEL(1);
	for(uint8_t HTTP_SOCKET = 0; HTTP_SOCKET <= 7; HTTP_SOCKET++){
		//setINTLEVEL(500);
		//setSn_IMR(HTTP_SOCKET, 0x04);
		setSn_IMR(HTTP_SOCKET, 0x04);
		//setSn_IR(HTTP_SOCKET, 0x1F);
	}
	return 1;
}
void udpDataSend(uint8_t *udpDatabuff, uint16_t length){
	result = socket(1, Sn_MR_UDP, UdpRxPort, SF_IO_NONBLOCK);
	result = sendto(1, udpDatabuff, length, UdpDestAddress, UdpTxPort);
}

uint16_t udpDataAvailable(uint8_t *udpDatabuff, uint16_t buffLength) {
	if (getSn_SR(UdpRxSockNum) == SOCK_CLOSED) {
		socket(UdpRxSockNum, Sn_MR_UDP, UdpRxPort, SF_MULTI_ENABLE);
		listen(UdpRxSockNum);
	}
	
	uint16_t udp_size = getSn_RX_RSR(UdpRxSockNum);
	if (udp_size == 0) {
		return 0;
	}
	if (udp_size > buffLength) {
		udp_size = buffLength;
	}
	uint8_t remoteIP[4];
	uint16_t remotePort;

	int16_t received = recvfrom(UdpRxSockNum, udpDatabuff, udp_size, remoteIP, &remotePort);
	
	if (received < 0) {
		return 0;
	}

	return (uint16_t)received;
}



void netHandler(void){
	//if(getSn_SR(UdpRxSockNum) == SOCK_CLOSED){
		//socket(UdpRxSockNum, Sn_MR_UDP, UdpRxPort, SF_MULTI_ENABLE);
		//listen(UdpRxSockNum);
	//}
	
	//if(getSn_IR(UdpRxSockNum)){ // & Sn_CR_RECV
		//uint16_t udp_size = getSn_RX_RSR(UdpRxSockNum);
		//if (udp_size > 0) {
			//uint8_t ip[4];
			//uint16_t port;
			//if (udp_size > DATA_BUFF_SIZE) udp_size = DATA_BUFF_SIZE;
			//memset(TCP_RX_BUF, 0, sizeof(TCP_RX_BUF));
			//uint16_t ret = recvfrom(UdpRxSockNum, (uint8_t*)TCP_RX_BUF, udp_size, ip, &port);
			//// UDP Datagram - TCP_RX_BUF
			//uint8_t okFlg = 0;
			//char state[4];
			//if (sscanf(TCP_RX_BUF, "OUTP:STAT %3s", state) == 1) {
				//if (strcasecmp(state, "OFF") == 0) {
					//printf("Output is OFF\r\n");
					//sprintf(UdpAnsver, "ok");
					//okFlg = 1;
					//} else if (strcasecmp(state, "ON") == 0) {
					//printf("Output is ON\r\n");
					//sprintf(UdpAnsver, "ok");
					//okFlg = 1;
				//}
				//
			//}
			//
			//char command[10];
			//if (sscanf(TCP_RX_BUF, "MEAS:%[^?]", command) == 1) {
				//if (strcasecmp(command, "VOLT") == 0) {
					//printf("Measure Voltage command\r\n");
					//sprintf(UdpAnsver, "48.031");
					//okFlg = 1;
					//} else if (strcasecmp(command, "CURR") == 0) {
					//printf("Measure Current command\r\n");
					//sprintf(UdpAnsver, "0.121");
					//okFlg = 1;
				//}
				//
			//}
			//
			//
			//
			//if(strcasecmp(TCP_RX_BUF, "*RST") == 0){
				//sprintf(UdpAnsver, "ok");
				//}else{
				//if(okFlg == 0){
					//sprintf(UdpAnsver, "err");
				//}
			//}
			//result = socket(UdpTxSockNum, Sn_MR_UDP, UdpTxPort, SF_IO_NONBLOCK);
			//result = sendto(UdpTxSockNum, (uint8_t*)UdpAnsver, strlen(UdpAnsver), UdpDestAddress, UdpTxPort);
			////Old code
			////socket(UdpTxSockNum, Sn_MR_UDP, UdpTxPort, SF_IO_NONBLOCK);
			////sendto(UdpTxSockNum, (uint8_t *)TCP_RX_BUF, udp_size, ip, UdpTxPort);
			////setSn_IR(UdpRxSockNum, 0x1F);
		//}
	//}
	
	for (uint8_t HTTP_SOCKET = 5; HTTP_SOCKET <= 7; HTTP_SOCKET++) {
		if (getSn_SR(HTTP_SOCKET) == SOCK_ESTABLISHED) {
			uint8_t rIP[4];
			getsockopt(HTTP_SOCKET, SO_DESTIP, rIP);

			uint16_t res_size = getSn_RX_RSR(HTTP_SOCKET);
			if (res_size > 0) {
			
				printf("Read bytes: %d, SOCK: %d \r\n", res_size, HTTP_SOCKET);
				
				if (res_size > sizeof(TCP_RX_BUF)) {
					res_size = sizeof(TCP_RX_BUF);
				}
				
				
				memset(TCP_RX_BUF, 0, sizeof(TCP_RX_BUF));
				recv(HTTP_SOCKET, (uint8_t *)TCP_RX_BUF, res_size);


				
				if (strstr((char *)TCP_RX_BUF, "GET / ") != NULL) {
					 size_t total_length = strlen_P(psu_page); 
					 size_t sent_length = 0;
					 uint8_t buffer[2048];

					 printf("index.html send\r\n");
					 
					 while (sent_length < total_length) {
						 size_t chunk_size = (total_length - sent_length > sizeof(buffer)) ? sizeof(buffer) : total_length - sent_length;
						 for (size_t i = 0; i < chunk_size; i++) {
							 buffer[i] = pgm_read_byte(&psu_page[sent_length + i]);
						 }
						 send(HTTP_SOCKET, buffer, chunk_size);
						 while (getSn_TX_FSR(HTTP_SOCKET) != getSn_TxMAX(HTTP_SOCKET)) {
							 _delay_ms(1);
						 }
						 sent_length += chunk_size;
					 }
					
					} else if (strstr((char *)TCP_RX_BUF, "GET /favicon.ico") != NULL) {
					printf("favicon.ico send\r\n");
					send(HTTP_SOCKET, (uint8_t *)"HTTP/1.1 200 OK\r\nContent-Type: image/x-icon\r\r\n\n", 47);
					send(HTTP_SOCKET, (uint8_t *)favicon_ico, sizeof(favicon_ico));

					} else if (strstr((char *)TCP_RX_BUF, "GET /get_vals") != NULL) {
					char json_response[1024];
					snprintf(json_response, sizeof(json_response),
					"HTTP/1.1 200 OK\r\n"
					"Content-Type: application/json\r\n\r\n"
					"{"
						"\"timestamp\":\"%s\","
						
						"\"batVoltage\":%.2f,"
						"\"batCurrent\":%.2f,"
						"\"batEnergy\":%.1f,"
						"\"currentState\":%d,"
						"\"currentSource\":\"%s\","

						"\"pumpAmp\":%d,"
						"\"pumpState\":%d,"
						"\"pumpMode\":\"%s\","
						

						"\"boilerTemperature\":%.1f,"
						"\"coolantTemperature\":%.1f,"
						"\"solarCollectorTemp\":%.1f,"
						"\"lightLevel\":%d,"

						"\"waterTankDIN\":%d,"
						"\"waterTankDOUT\":%d,"
						"\"waterPress\":%d,"
						"\"watetPressLoLim\":%d,"
						"\"waterPressDelta\":%d,"
						"\"waterLevel\":%d,"
						

						"\"kotelActTemp\":%.1f,"
						"\"kotelStatus\":%d,"
						"\"kotelTemLo\":%d,"
						"\"kotelTemHi\":%d,"
						"\"kotelTemRun\":%d,"
						"\"kotelTemStop\":%d,"
						"\"kotelTemDelta\":%d,"
						
						

						"\"in1\":%d,"
						"\"in2\":%d,"
						"\"out1\":%d,"
						"\"out2\":%d,"
						
						"\"upsMsgInteval\":%lu,"
						"\"waterTankMsgInteval\":%lu,"
						"\"sensorUnitMsgInteval\":%lu,"
						"\"kotelMsgInteval\":%lu"
						
					"}",
					gatewayValues->timeStamp,
					
					//UPS unit
					gatewayValues->batVoltage,					// volt
					gatewayValues->batCurrent,					// amp, 
					gatewayValues->batEnergyf,					// soc
					gatewayValues->currentState,
					gatewayValues->currentSource,				// source
					
					//UPS Pump unit
					gatewayValues->pumpCurrent,					// pumpAmp
					gatewayValues->currentSource,				// pumpState
					gatewayValues->pumpMode,
					
					//Sensor unit
					gatewayValues->boilerTemperature, 
					gatewayValues->coolantTemperature, 
					gatewayValues->solarCollectorTemp, 
					gatewayValues->lightLevel, // temp1–3, light
					
					//water tank unit
					gatewayValues->waterTankDIN,				//
					gatewayValues->waterTankDOUT,			//
					gatewayValues->waterPress,				// waterLvl, pressure, mode
					gatewayValues->watetPressLoLim,
					gatewayValues->waterPressDelta,
					gatewayValues->waterLevel,


					//Koted section
					gatewayValues->kotelActTemp,
					gatewayValues->kotelStatus,
					gatewayValues->kotelTemLo,
					gatewayValues->kotelTemHi,
					gatewayValues->kotelTemRun,
					gatewayValues->kotelTemStop,
					gatewayValues->kotelTemDelta,
					

					//Gateway data section
					gatewayValues->digitalIn[0], 
					gatewayValues->digitalIn[1],                  // in1, in2
					gatewayValues->digitalOut[0], 
					gatewayValues->digitalOut[1],
					gatewayValues->upsMsgInteval,
					gatewayValues->waterTankMsgInteval,
					gatewayValues->sensorUnitMsgInteval,
					gatewayValues->kotelMsgInteval
					
					);

					send(HTTP_SOCKET, (uint8_t *)json_response, strlen(json_response));
				} else if (strstr((char *)TCP_RX_BUF, "GET /set_rel") != NULL) {
					uint8_t relayNum = 0;
					uint8_t state = 0;
					if (sscanf((char *)TCP_RX_BUF, "GET /set_rel%hhu?state=%hhu", &relayNum, &state) == 2) {
							
						if(relayNum == 1){
							gatewayValues->digitalOut[0] = (state == 0) ? false : true;
							send(HTTP_SOCKET, (uint8_t *)"HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"success\":true}", 67);
						}else if(relayNum == 2){
							gatewayValues->digitalOut[1] = (state == 0) ? false : true;
							send(HTTP_SOCKET, (uint8_t *)"HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"success\":true}", 67);					
						} else {
							send(HTTP_SOCKET, (uint8_t *)"HTTP/1.1 400 Bad Request\r\n\r\n", 30);
						}
					}
							
				} else if (strstr((char *)TCP_RX_BUF, "GET /set_vals") != NULL) {
					char *query_string = strstr((char *)TCP_RX_BUF, "GET /set_vals") + strlen("GET /set_vals?");
					sscanf(query_string, "amp=%f&volt=%f", &amp, &volt);
					printf("%f; %f\r\n", amp, volt);
					
					//SET_DAC_VOLTAGE(volt);
					//SET_DAC_CURRENT(amp);
					remoteCtrl = 1;
					send(HTTP_SOCKET, (uint8_t *)"HTTP/1.1 200 OK\r\nContent-Type: application/json\r\r\n\n{\"success\":true}", 67);
					
					} else if (strstr((char *)TCP_RX_BUF, "GET /control") != NULL) {
					char *query_string = strstr((char *)TCP_RX_BUF, "GET /control") + strlen("GET /control?");
					char device[10], action[10];
					
					sscanf(query_string, "device=%[^&]&action=%s", device, action);
					printf("%s; %s\r\n", device, action);
					
					if (strcmp(device, "fan") == 0) {
						
						//gpio_set_pin_level(O2, strcmp(action, "on") == 0);
						//gpio_set_pin_level(O3, strcmp(action, "on") == 0);
						} else if (strcmp(device, "rem") == 0) {
						
						remoteCtrl = (strcmp(action, "on") == 0);
						//gpio_set_pin_level(O1, remoteCtrl);
						} else if (strcmp(device, "psu") == 0) {
						
						outState = (strcmp(action, "on") == 0) ? 1 : 0;
						remoteCtrl = 1;
					}
					send(HTTP_SOCKET, (uint8_t *)"HTTP/1.1 200 OK\r\nContent-Type: application/json\r\r\n\n{\"success\":true}", 67);
				}
				disconnect(HTTP_SOCKET);
				
				while (getSn_TX_FSR(HTTP_SOCKET) != getSn_TxMAX(HTTP_SOCKET)) {
					//delay_ms(1);
				}
				close(HTTP_SOCKET);
			}
		}
		//if(outState == 1){
			//gpio_set_pin_level(PSU_OUT_EN, true);
			//}else{
			//gpio_set_pin_level(PSU_OUT_EN, false);
			//
		//}
		//if(remoteCtrl == 1){
			//gpio_set_pin_level(PSU_REM, true);
			//}else{
			//gpio_set_pin_level(PSU_REM, false);
			//
		//}
		//if (getSn_SR(HTTP_SOCKET) == SOCK_CLOSE_WAIT) {
		//disconnect(HTTP_SOCKET);
		//}

		if (getSn_SR(HTTP_SOCKET) == SOCK_CLOSED) {
			socket(HTTP_SOCKET, Sn_MR_TCP, socketPort[HTTP_SOCKET], 0);
			listen(HTTP_SOCKET);
		}
	}
	
}