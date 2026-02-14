
/*
 * sysTickTimer.c
 *
 * Created: 03.01.2025 12:59:26
 *  Author: Ivan Prints
 */ 

#include "sysTickTimer.h"
uint32_t sysTickCounter;
uint32_t sysUpTimeCoumter;
uint16_t uptimePrescaler;

ISR(TCC0_OVF_vect) {
	sysTickCounter++;
	uptimePrescaler++;
	if(uptimePrescaler == 999){
		uptimePrescaler = 0;
		sysUpTimeCoumter ++;
	}
}

uint8_t sysTickInit(void){
	CLK.PSCTRL = 0; 
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
	uint16_t compare_value = (F_CPU / 64 / TIMER_FREQUENCY) - 1;
	TCC0.PER = compare_value;
	TCC0.INTCTRLA = TC_OVFINTLVL_MED_gc;
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;
	sei();
	return 1;
}
uint32_t getSysTick(void){
	return sysTickCounter;
}

uint32_t getUpTime(void){
	return sysUpTimeCoumter;
}