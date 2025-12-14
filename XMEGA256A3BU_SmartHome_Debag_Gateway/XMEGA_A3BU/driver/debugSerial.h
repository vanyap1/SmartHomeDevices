
/*
 * debugSerial.h
 *
 * Created: 03.01.2025 10:16:36
 *  Author: Ivan Prints
 */ 
#ifndef _DEB_SERIAL_DRIVER_H_
#define _DEB_SERIAL_DRIVER_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "gpio_driver.h"

#ifdef __cplusplus
extern "C" {
	#endif
	

	uint8_t getRxByte(void);
	uint16_t debugSerialPutChar(char c, FILE *stream);
	uint8_t debugSerialInit(uint32_t baud);
	void debugSerialWriteChar(uint8_t tmpChar);


	
#ifdef __cplusplus
}
#endif
#endif