/*
 * dto.h
 *
 * Created: 30.09.2024 23:28:43
 *  Author: Ivan Prints
 */ 

#ifndef DTO_H_
#define DTO_H_
#include "stdint.h"


typedef struct {
	uint32_t pmId;				//PowerMeter ID and Die Revision ID Bits
	int32_t current;			//current shunt reg
	uint32_t voltage;			//voltage reg
	uint8_t capacity;			//SOC capacity in percent
	float power;				//power reg (load or charge consumption)
	float energy;
	float lastChargerErr;
} powerData;

typedef struct {
	uint16_t pressureLoLim;			//Lo lim when pump will be on
	uint16_t pressureHiLim;			//Lo high when pump will be off
	uint8_t pressureDelta;			//LoLim+delta = HiLim to the pump stopping
	uint8_t eepromOk;				//If this value is not equal 0xaa, data will be restored to default
} pumpLimits;



#endif