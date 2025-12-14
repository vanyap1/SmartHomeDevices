/*
 * usarte.h
 *
 * Created: 25/07/2025 19:17:35
 *  Author: Ivan Prints
 */ 
#ifndef _DEB_USARTE_DRIVER_H_
#define _DEB_USARTE_DRIVER_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define RXE_BUFFER_SIZE 256

#ifdef __cplusplus
extern "C" {
	#endif
	

	uint16_t serialEgetRxBytesNum(void);
	uint8_t serialEDataAvailable(void);
	const char* serialEreadDataBuff(void);
	void serialEWriteString(uint8_t *c);
	uint8_t serialEInit(uint32_t baud);
	void serialEWriteChar(uint8_t tmpChar);

	
	#ifdef __cplusplus
}
#endif
#endif