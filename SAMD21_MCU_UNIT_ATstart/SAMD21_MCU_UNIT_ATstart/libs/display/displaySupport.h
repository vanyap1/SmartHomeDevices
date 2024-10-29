
/*
 * displaySupport.h
 *
 * Created: 27.03.2024 22:16:31
 *  Author: Vanya
 */ 
#include "u8g2.h"

#ifndef DISPLAY_SUPPORT_FNS
#define DISPLAY_SUPPORT_FNS

uint8_t u8x8_avr_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t vfd_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);


#endif