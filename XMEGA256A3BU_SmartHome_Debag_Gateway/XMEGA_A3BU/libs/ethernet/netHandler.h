
/*
 * netHandler.h
 *
 * Created: 29.12.2024 12:50:03
 *  Author: Ivan Prints
 */ 

#ifndef  _NETHANDLER_H_
#define  _NETHANDLER_H_



uint8_t netInit(gatewayDto *gatewayData);
void udpDataSend(uint8_t *udpDatabuff, uint16_t length);
uint16_t udpDataAvailable(uint8_t *udpDatabuff, uint16_t buffLength);
void netHandler(void);


#endif
