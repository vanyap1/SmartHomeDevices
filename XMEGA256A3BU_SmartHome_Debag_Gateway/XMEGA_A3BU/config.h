
/*
 * config.h
 *
 * Created: 02.01.2025 15:56:56
 *  Author: Ivan Prints
 */ 

#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

#include "stdint.h"
#include "stdbool.h"

#define DEBUG

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


typedef struct {
	uint8_t boilerTemperature;      // ??????????? ????????????????????
	uint8_t solarCollectorTemp;     // ??????????? ????????? ?????????
	uint8_t coolantTemperature;     // ??????????? ??????????
	uint8_t lightLevel;             // ?????? ????????????
	uint8_t auxAdcValue;            // ???????? ?????????? ADC ??????
	uint32_t timestamp;             // ?????????
} SensorUnit;

typedef struct{
	char timeStamp[32];
	bool digitalIn[2];
	bool digitalOut[2];
	
	uint32_t upsMsgTimestamp;
	uint32_t waterTankMsgTimestamp;
	uint32_t sensorUnitMsgTimestamp;
	uint32_t kotelMsgTimestamp;
	
	uint32_t upsMsgInteval;
	uint32_t waterTankMsgInteval;
	uint32_t sensorUnitMsgInteval;
	uint32_t kotelMsgInteval;
	
	//Sensor unit section
	float boilerTemperature;
	float solarCollectorTemp;
	float coolantTemperature;
	uint16_t lightLevel;
	uint16_t auxAdcValue;
	
	//UPS section
	float batVoltage;
	float batCurrent;
	float batEnergyf;
	uint32_t batEnergy;
	uint8_t currentState;
	char currentSource[8];
	
	//Circulation Lump
	uint8_t pumpCurrent;
	char pumpMode[8];
	double dallasTemp;
	uint8_t dallasTemp8;
	
	
	
	//Water tank values
	uint8_t waterTankDIN;
	uint8_t waterTankDOUT;
	uint16_t waterPress;
	uint16_t watetPressLoLim;
	uint8_t waterPressDelta;
	uint8_t waterLevel;
	
	//Kotel values
	float kotelActTemp;
	uint8_t kotelStatus;
	uint8_t kotelTemLo;
	uint8_t kotelTemHi;
	uint8_t kotelTemRun;
	uint8_t kotelTemStop;
	uint8_t kotelTemDelta;
}gatewayDto;

#ifndef F_CPU
#define F_CPU 32000000UL
#endif

#ifndef DEB_SERIAL_BAUD
#define DEB_SERIAL_BAUD 115200UL
#endif

typedef enum {
	TEMPERATURE_REPORT,
	RTC_REPORT,
	KEEP_ALIVE_REPORT,
	MESSAGE_TYPE_COUNT
} MessageType;



#define DALLAS_MEASURE_INTERVAL 2500
#define	CAN_MSG_INTERVAL	500	
// <<< end of configuration section >>>

#endif // CLOCK_CONFIG_H

