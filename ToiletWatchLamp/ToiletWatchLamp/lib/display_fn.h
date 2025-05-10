/*
 * display_fn.h
 *
 * Created: 21.06.2023 23:45:13
 *  Author: princ
 */ 
#include "stdbool.h"
#include <u8g2.h>
#include <util/delay.h>

uint8_t lcd_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t lcd_software_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

void lcd_gpio_init();
void cmd_pin_handle(bool value);
void rst_pin_handle(bool value);
void cs_pin_handle(bool value);

uint8_t fake_delay_fn(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
