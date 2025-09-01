/*
 * OPI_PC_PowerController.c
 *
 * Created: 01/09/2025 18:02:38
 * Author : Ivan Prints
 */ 

#include "config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "uart_hal.h"
#include "gpio_driver.h"
#include "twi_hal0.h"
#include "adc_hal.h"
#include "eeprom.h"


#define PC_EN_DELAY			200
#define REL_EN_DELAY		200
#define REL_SAFE_DELAY		500


#define VOLTAGE_LOW			9000
#define VOLTAGE_DELTA		1000

#define TEMPERATURE_HIGH	450
#define TEMPERATURE_DELTA	300



#include <stdio.h>
static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);


gpio ldr = {(uint8_t *)&PORTB , PORTB0};
gpio ldg = {(uint8_t *)&PORTB , PORTB2};
gpio ldb = {(uint8_t *)&PORTB , PORTB1};
gpio pc_en = {(uint8_t *)&PORTC , PORTC3};
gpio rel1 = {(uint8_t *)&PORTD , PORTD6};
gpio rel2 = {(uint8_t *)&PORTD , PORTD7};
gpio fanOut = {(uint8_t *)&PORTB , PORTB3};
gpio safeInput = {(uint8_t *)&PORTD , PORTD2};
	
	
uint32_t sysTick = 0;
uint32_t ledBlinkTick = 0;

uint16_t pcEnableDelayTimer = PC_EN_DELAY;
uint16_t relEnableDelayTimer = REL_EN_DELAY;
uint16_t relSafeDelayTimer = 0;

uint16_t systemVoltage = 0;
uint16_t systemTemperature = 0;

bool lowVoltageDetector = false;
bool pcEnFlg = false;
bool tempProtection = false;
const char* dataLine;
	
ISR(TIMER1_COMPA_vect)
{
	sysTick++;
	if(pcEnableDelayTimer != 0) pcEnableDelayTimer --;
	if(relEnableDelayTimer != 0) relEnableDelayTimer--;
	if(relSafeDelayTimer != 0) relSafeDelayTimer --;
}


int main(void)
{
	
    uint8_t resetCause = MCUSR;
    MCUSR = 0;
    sei();
    wdt_enable(WDTO_4S);
	
	gpio_set_pin_direction(&ldr , PORT_DIR_OUT); gpio_set_pin_level(&ldr, true);
	gpio_set_pin_direction(&ldg , PORT_DIR_OUT); gpio_set_pin_level(&ldg, true);
	gpio_set_pin_direction(&ldb , PORT_DIR_OUT); gpio_set_pin_level(&ldb, true);
	gpio_set_pin_direction(&pc_en , PORT_DIR_OUT); gpio_set_pin_level(&pc_en, false);
	
	gpio_set_pin_direction(&rel1 , PORT_DIR_OUT); gpio_set_pin_level(&rel1, false);
	gpio_set_pin_direction(&rel2 , PORT_DIR_OUT); gpio_set_pin_level(&rel2, false);
	gpio_set_pin_direction(&fanOut , PORT_DIR_OUT); gpio_set_pin_level(&fanOut, false);
	
	uart_init(250000,1);
	//twi0_init(400000);
	adc_init();
	//spi0_init();
	
	TCCR1B |= (1 << WGM12 );   // Configure timer 1 for CTC mode
	OCR1A = 24999/20;             // Set CTC compare value to 10Hz (100mS)
	// at 16MHz AVR clock, with a prescaler of 64
	//OCR1A = (8_000_000 / 256 / 10) - 1
	//= (6250) - 1
	//= 6249
	TIMSK1 |= (1 << OCIE1A );  // Enable CTC interrupt
	TCCR1B |= (1 << CS10 ) | (1 << CS11 );
	sei();
	stdout = &mystdout;
	
    while (1) 
    {
		wdt_reset();
		_delay_ms(1);
		if (serial_complete()) {
			dataLine = serial_read_data();
			
			if (strcmp(dataLine, "run") == 0){
				relSafeDelayTimer = REL_SAFE_DELAY;
			}
		}
		
		if((sysTick - ledBlinkTick) >= 10){
			ledBlinkTick = sysTick;
			systemVoltage = get_mVolt(0);
			systemTemperature = getNTC(1);
			
			if(systemVoltage < VOLTAGE_LOW) lowVoltageDetector = true;
			if(systemVoltage > VOLTAGE_LOW + VOLTAGE_DELTA) lowVoltageDetector = false;
			if(systemTemperature <= TEMPERATURE_HIGH) tempProtection = false;
			if(systemTemperature >= TEMPERATURE_HIGH + TEMPERATURE_DELTA) tempProtection = true;
			
			
			if(lowVoltageDetector) pcEnableDelayTimer = PC_EN_DELAY;
			if(gpio_get_pin_level(&safeInput) == 1 ) relEnableDelayTimer = REL_EN_DELAY;
			uint8_t safeVal = (gpio_get_pin_level(&safeInput) << 2) | (((relEnableDelayTimer != 0) ? 1 : 0) & 0x01) << 1 | ((relSafeDelayTimer == 0) ? 1 : 0) & 0x01;
			
			
			printf("mV:%d;degC:%d;sfIn:%d;fan:%d;\n\r", systemVoltage, systemTemperature, safeVal,tempProtection);
			pcEnFlg = pcEnableDelayTimer == 0 ? true : false;
			gpio_set_pin_level(&pc_en, (pcEnFlg) ? true : false);
			gpio_set_pin_level(&fanOut, tempProtection);
			gpio_set_pin_level(&rel1, relEnableDelayTimer == 0 ? true : false);
			gpio_set_pin_level(&rel2, relSafeDelayTimer != 0 ? true : false);
			
			if(pcEnFlg){
				gpio_set_pin_level(&ldr , true);
				gpio_set_pin_level(&ldg , false);
				gpio_set_pin_level(&ldb , false);
				}else{
				gpio_toggle_pin_level(&ldr);
				gpio_set_pin_level(&ldg , true);
				gpio_set_pin_level(&ldb , true);
			}
			
		}
		
		
    }
}

