
/*
 * bms_ina22x.c
 *
 * Created: 16.11.2023 21:05:21
 *  Author: Vanya
 */ 
//#include "rtc.h"

#include "bms_ina22x.h"
#include <math.h>

uint8_t INA_ADDR = 0;


uint8_t PowerMeterInit(uint8_t addr){
	uint8_t data_reg[3];
	uint16_t config_reg = 0;
	INA_ADDR = addr;
	config_reg |= (SHUNT_AND_BUS_CONTINUOUS << MODE1) | (CONVERSION_TIME_1100us << VSHCT0)  | (CONVERSION_TIME_1100us << VBUSCT0) | (AVERAGES_512 << AVG0) | (0 << RST);
	data_reg[0] = CONFIG_REG;
	data_reg[1] = (config_reg >> 8) & 0xFF;
	data_reg[2] = config_reg & 0xFF;
	
	return I2C_write_batch(INA_ADDR, data_reg, sizeof(data_reg));
	
	uint16_t callibrationReg = 0;
	data_reg[0] = CALIBRATION_REG;
	data_reg[1] = (callibrationReg >> 8) & 0xFF;
	data_reg[2] = callibrationReg & 0xFF;
	return I2C_write_batch(INA_ADDR, data_reg, sizeof(data_reg));
}


uint16_t PowerMeterGetId(){
	
	return PowerMeterGetReg(MANUFACTURER_ID_REG);
}

uint16_t PowerMeterGetReg(uint8_t reg_addr){
	uint8_t reg_raw[2] = {0, 0};	//Read one 16 bit word
	
	I2C_read_batch_addr(INA_ADDR, reg_addr, reg_raw, sizeof(reg_raw));
	return (reg_raw[0] << 8) | reg_raw[1];
}

uint8_t PowerMeterMeasure(powerData * pmStruc){
	int16_t tmpCurrent = PowerMeterGetReg(SHUNT_VOLTAGE_REG);
	float milliAmptVal = (tmpCurrent > 0)? (tmpCurrent*POS_CURRENT_MULTIPLIER)+CURRENT_OFFSET : (tmpCurrent*NEG_CURRENT_MULTIPLIER)+CURRENT_OFFSET;
	float milliVoltVal = (PowerMeterGetReg(BUS_VOLTAGE_REG)*VOLTAGE_MULTIPLIER)+VOLTAGE_OFFSET;
	
	pmStruc->voltage = (uint16_t)milliVoltVal;
	pmStruc->current = (int16_t)milliAmptVal;
	pmStruc->power = ((milliAmptVal)*milliVoltVal)/1000000;
	//pmStruc->energy= (milliVoltVal < 14250) ? pmStruc->energy+pmStruc->power/3600 : BAT_FULL_ENEERGY;
	
	if (milliVoltVal < 14250)
	{
		pmStruc->energy = pmStruc->energy+pmStruc->power/3600;
	}else{
		
		if (pmStruc->energy > BAT_FULL_ENEERGY)
		{
			pmStruc->lastChargerErr = pmStruc->energy - BAT_FULL_ENEERGY;
		}
		pmStruc->energy =  BAT_FULL_ENEERGY;
	}
	
	pmStruc->capacity = (uint8_t)((pmStruc->energy / BAT_FULL_ENEERGY) * 100);
	return 1;
}