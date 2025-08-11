
/*
 * sysTickTimer.h
 *
 * Created: 03.01.2025 12:59:08
 *  Author: Ivan Prints
 */ 

#ifndef _SYS_TICK_TIMER_H_
#define _SYS_TICK_TIMER_H_
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "gpio_driver.h"

#define TIMER_FREQUENCY 1000


#ifdef __cplusplus
extern "C" {
	#endif
	
	


	uint8_t sysTickInit(void);
	uint32_t getSysTick(void);
	uint32_t getUpTime(void);
	


	
	#ifdef __cplusplus
}
#endif

#endif