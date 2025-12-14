
/*
 * spi_driver.h
 *
 * Created: 02.01.2025 15:58:31
 *  Author: Ivan Prints
 */ 

#ifndef _TWIC_DRIVER_H_
#define _TWIC_DRIVER_H_
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "gpio_driver.h"

#ifdef __cplusplus
	extern "C" {
	#endif

#define TWIE_BAUD(F_CPU, SCL_FREQ) ((F_CPU / (2 * SCL_FREQ)) - 5)

#define TWIE_BAUDRATE 400000UL        
#define TWI_WRITE 0x00
#define TWI_READ 0x01




uint8_t twieInit(void);

void initTwieInt(void);
uint8_t getTwieInt(void); 
	
bool twieTxBuff(uint8_t addres, uint8_t reg, uint8_t *data, uint8_t data_len);
bool twieRxBuff(uint8_t addres, uint8_t reg, uint8_t *data, uint8_t data_len);



	
#ifdef __cplusplus
}
#endif
	
#endif	
	
