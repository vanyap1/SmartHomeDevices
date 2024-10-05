/*
 * dto.h
 *
 * Created: 30.09.2024 23:28:43
 *  Author: Ivan Prints
 */ 

#ifndef DTO_H_
#define DTO_H_

typedef struct powerMeterData {
	uint32_t pmId;				//PowerMeter ID and Die Revision ID Bits
	int32_t current;			//current shunt reg
	uint32_t voltage;			//voltage reg
	uint8_t capacity;			//SOC capacity in percent
	float power;				//power reg (load or charge consumption)
	float energy;
	float lastChargerErr;
} powerData;





#endif