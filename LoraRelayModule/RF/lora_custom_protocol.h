
/*
 * customProtocol.h
 *
 * Created: 02.08.2024 12:52:22
 *  Author: Vanya
 */ 
#ifndef HW_CustomProtocol_H_
#define HW_CustomProtocol_H_

#define POWERBANKID		0x21
#define VFDSCREEN		0x03
#define DEVMODULE		0x22
#define DEVMODULE2		0xFE
#define DEVMODULE3		0xFA


#define NETWORKID		33
#define NODEID			DEVMODULE3
#define ALLNODES		0xff
#define SMARTSCREEN		0xf0
#define RX_MODE			1
#define RTC_SYNC		0x81
#define MSG				0x82
#define POWERBANK		0x83

#define TX_MUTE			0
#define TX_UNMUTE		1

#define GPIO_CTRL		0x84		//reserved for lora relay module (Send with tis ID to module)
#define GPIO_INFO		0x85		//reserved for lora relay module (Module will answer with this ID)
#define GPIO_ALARM		0x86		//reserved for lora relay module (Alarm message)





typedef struct {
	uint8_t netId;
	uint8_t nodId;
	char nodSerial[10];
} nodeNetId;

typedef struct{				//GPIO_CTRL
	uint8_t relayNum;		//Num of relay (1 - default)
	uint8_t relayState;		//Write new state (0/1)
	uint16_t relayOnTime;	//New state delay in second
	uint8_t crc;			//message CRC
} nodeCtrl; 
/* For example - 
					-Write relay 1, state 1, OnTime 0
					-Write relay 1, state 0, OnTime 10
					relay will switch on immediately and turn off after 10 seconds
*/

typedef struct {		//GPIO_INFO
	uint8_t nodeId;
	int16_t nodeTemperature;
	uint16_t nodeBatVoltage;
	uint16_t nodeVccVoltage;
	uint8_t chargerStatus;		//0 - idle (AC fail), 1 - stdb (Battery is full), 2 - process (charging the battery)
	uint8_t relayStatus;		//Bit value (bit 0 = relay 1)
	uint8_t alarmStatus;
	uint8_t crc;
} nodeInfo;


typedef struct {		//GPIO_INFO
	uint8_t nodeId;
	uint8_t alarmStatus;
	uint8_t alarmID;
	uint8_t crc;
} nodeAlarm;



#endif