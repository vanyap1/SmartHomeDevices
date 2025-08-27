
/*
 * ws28_led.h
 *
 * Created: 16.10.2023 22:27:59
 *  Author: Vanya
 */ 
#include <stdint.h>

#ifndef WS28_LED_h
#define WS28_LED_h


#define NEOPIXEL_OUT GPIO(GPIO_PORTA, 20)
#define PA21 GPIO(GPIO_PORTA, 21)
#define PA22 GPIO(GPIO_PORTA, 22)



typedef struct rgb_led {
	uint8_t component_r;
	uint8_t component_g;
	uint8_t component_b;
} rgb_led;


void SPI_PIXEL_init(void);

void neopixelInit();
void neopixelWrite();




#endif