

/*
 * gpio_driver.c
 *
 * Created: 02.01.2025 19:07:26
 *  Author: Ivan Prints
 */ 
#include "gpio_driver.h"



void gpio_set_pin_level(gpio *GPIOx, const bool level){
	uint8_t *tmp_port = GPIOx->port+4;
	if (level) {
		*tmp_port |= 1 << GPIOx->pin;
		} else {
		*tmp_port &= ~(1 << GPIOx->pin);
	}
}
void gpio_set_port_level(gpio *GPIOx, const uint8_t mask){
	
}
void gpio_set_pin_direction(gpio *GPIOx, const enum port_dir direction){
	
	uint8_t *tmp_port = GPIOx->port;
	
	switch (direction) {
		case PORT_DIR_IN:
		*tmp_port &= ~(1 << GPIOx->pin);
		break;
		case PORT_DIR_OUT:
		*tmp_port |= 1 << GPIOx->pin;
		break;
		case PORT_DIR_OFF:
		*tmp_port &= ~(1 << GPIOx->pin);
		*GPIOx->port &= ~(1 << GPIOx->pin);
		break;
		default:
		break;
	}
}
void gpio_set_pin_pull_mode(gpio *GPIOx, const enum port_pull_mode pull_mode){
	
}
void gpio_set_port_direction(gpio *GPIOx, const uint8_t mask){
	
}
uint8_t gpio_get_port_level(gpio *GPIOx){
	return 0;	
}


uint8_t gpio_get_pin_level(gpio *GPIOx){
	uint8_t *tmp_port = GPIOx->port+8;
	return *tmp_port & (1 << GPIOx->pin) ? 1 : 0; 
}
void gpio_toggle_pin_level(gpio *GPIOx){
	uint8_t *tmp_port = GPIOx->port+7;
	*tmp_port |= 1 << GPIOx->pin;
}