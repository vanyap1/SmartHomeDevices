
/*
 * displaySupport.c
 *
 * Created: 27.03.2024 22:16:39
 *  Author: Vanya
 */ 
#include "displaySupport.h"

uint8_t u8x8_avr_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr){
	switch(msg)
	{
		case U8X8_MSG_DELAY_NANO:     // delay arg_int * 1 nano second
		break;
		case U8X8_MSG_DELAY_100NANO:    // delay arg_int * 100 nano seconds
		break;
		case U8X8_MSG_DELAY_10MICRO:    // delay arg_int * 10 micro seconds
		break;
		case U8X8_MSG_DELAY_MILLI:      // delay arg_int * 1 milli second
		break;
		default:
		return 0;
	}
	return 0;
}

uint8_t vfd_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	
	switch(msg)
	{
		case U8X8_MSG_BYTE_SEND:
		vfd_write_data(arg_ptr, arg_int);
		break;
		case U8X8_MSG_BYTE_INIT:
		/* disable chipselect */
		vfd_cs_set(0);
		break;
		case U8X8_MSG_BYTE_SET_DC:
		cmd_pin_handle(arg_int);
		break;
		case U8X8_MSG_BYTE_START_TRANSFER:
		vfd_cs_set(1);
		break;
		case U8X8_MSG_BYTE_END_TRANSFER:
		vfd_cs_set(0);
		break;
		default:
		return 0;
	}
	return 1;
}