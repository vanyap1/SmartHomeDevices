

/*
 * gpio_driver.c
 *
 * Created: 02.01.2025 19:07:26
 *  Author: Ivan Prints
 */ 
#include "gpio_driver.h"

gpio RLD = {(uint8_t *)&PORTA , PIN2_bp};
gpio GLD = {(uint8_t *)&PORTA , PIN3_bp};
gpio BLD = {(uint8_t *)&PORTA , PIN1_bp};

gpio AC_SEL_REL = {(uint8_t *)&PORTA , PIN6_bp};
	
gpio FAN = {(uint8_t *)&PORTB , PIN1_bp};
	
gpio CURRENT_SENSOR_AIN = {(uint8_t *)&PORTA , PIN0_bp};

	
void gpioInit(void){
	gpio_set_pin_level(&RLD, true);
	gpio_set_pin_level(&GLD, true);
	gpio_set_pin_level(&BLD, true);
	gpio_set_pin_level(&AC_SEL_REL, false);
	gpio_set_pin_level(&FAN, false);
	
	
	gpio_set_pin_direction(&RLD , PORT_DIR_OUT);
	gpio_set_pin_direction(&GLD , PORT_DIR_OUT);
	gpio_set_pin_direction(&BLD , PORT_DIR_OUT);
	gpio_set_pin_direction(&AC_SEL_REL , PORT_DIR_OUT);
	gpio_set_pin_direction(&FAN , PORT_DIR_OUT);
	gpio_set_pin_direction(&CURRENT_SENSOR_AIN , PORT_DIR_IN);
	
	
	
}	

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
	//*tmp_port |= 1 << GPIOx->pin;
	*tmp_port = (1 << GPIOx->pin);
}