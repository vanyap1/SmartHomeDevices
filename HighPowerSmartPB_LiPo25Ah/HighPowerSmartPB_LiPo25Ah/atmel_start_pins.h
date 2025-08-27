/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMD20 has 8 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7

#define PA00 GPIO(GPIO_PORTA, 0)
#define PA01 GPIO(GPIO_PORTA, 1)
#define RF_RST GPIO(GPIO_PORTA, 2)
#define LED_SD GPIO(GPIO_PORTA, 3)
#define PA04 GPIO(GPIO_PORTA, 4)
#define PA05 GPIO(GPIO_PORTA, 5)
#define PA06 GPIO(GPIO_PORTA, 6)
#define PA08 GPIO(GPIO_PORTA, 8)
#define PA09 GPIO(GPIO_PORTA, 9)
#define VFD_RST GPIO(GPIO_PORTA, 7)
#define RTC_IRQ GPIO(GPIO_PORTA, 10)
#define PA16 GPIO(GPIO_PORTA, 16)
#define PA17 GPIO(GPIO_PORTA, 17)
#define PA18 GPIO(GPIO_PORTA, 18)
#define ROM_CS GPIO(GPIO_PORTA, 23)
#define RF_CS GPIO(GPIO_PORTA, 28)
#define LED_G GPIO(GPIO_PORTB, 0)
#define BTN GPIO(GPIO_PORTB, 1)
#define SD_CS GPIO(GPIO_PORTB, 4)
#define SD_DETECT GPIO(GPIO_PORTB, 5)
#define FILAMENT_EN GPIO(GPIO_PORTB, 6)
#define IO4 GPIO(GPIO_PORTB, 7)
#define IO3 GPIO(GPIO_PORTB, 8)
#define VFD_CS GPIO(GPIO_PORTB, 9)
#define LCD_RST GPIO(GPIO_PORTA, 7)

#define PB12 GPIO(GPIO_PORTB, 12)
#define PB13 GPIO(GPIO_PORTB, 13)
#define LED_R GPIO(GPIO_PORTB, 31)

#define RF_IRQ GPIO(GPIO_PORTB, 30)
#define EXT_I2C_IRQ GPIO(GPIO_PORTB, 8)




#endif // ATMEL_START_PINS_H_INCLUDED
