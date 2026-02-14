
/*
 * gpio_driver.h
 *
 * Created: 02.01.2025 19:07:58
 *  Author: Ivan Prints
 */ 
#ifndef _GPIO_DRIVER_H_
#define _GPIO_DRIVER_H_
#include <avr/io.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif
	

	
enum port_pull_mode {
	PORT_PULL_OFF,
	PORT_PULL_UP,
};

enum port_dir {
	PORT_DIR_IN,
	PORT_DIR_OUT,
	PORT_DIR_OFF,
};


typedef struct gpio_setup
{
	uint8_t *port;
	uint8_t pin ;
} gpio;


void gpio_set_pin_level(gpio *GPIOx, const bool level);
void gpio_set_port_level(gpio *GPIOx, const uint8_t mask);
void gpio_set_pin_direction(gpio *GPIOx, const enum port_dir direction);
void gpio_set_pin_pull_mode(gpio *GPIOx, const enum port_pull_mode pull_mode);
void gpio_set_port_direction(gpio *GPIOx, const uint8_t mask);
uint8_t gpio_get_port_level(gpio *GPIOx);
uint8_t gpio_get_pin_level(gpio *GPIOx);
void gpio_toggle_pin_level(gpio *GPIOx);
	
	
	
#ifdef __cplusplus
	}
#endif

#endif