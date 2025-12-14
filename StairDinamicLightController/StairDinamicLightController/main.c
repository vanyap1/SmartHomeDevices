#include "config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


#include "string.h"
#include "stdbool.h"
#include "uart_hal.h"
#include "gpio_driver.h"
#include "adc_hal.h"
#include "eeprom.h"
#include "rtc.h"
#include "tlc59_driver.h"

#include "stdint.h"
#include <stdio.h>
static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);

#define AIN_VIN		3
#define AIN_VBAT	2
#define AIN_LIGHT	7
#define VIN_1		0
#define VIN_2		1

#define OUT_A						OCR0B
#define OUT_B						OCR0A
#define LIGHT_SENSOR				VIN_1
#define BAT_LOW						3500
#define ALOVED_LIGHT_LEVEL			2000
gpio ledRedPin = {(uint8_t *)&PORTB , PORTB1};
gpio ledGrnPin = {(uint8_t *)&PORTB , PORTB2};
gpio chrgerChrg = {(uint8_t *)&PORTE , PORTE1};
gpio chrgerStdby = {(uint8_t *)&PORTB , PORTB0};
gpio pwmDrvRST = {(uint8_t *)&PORTD , PORTD4};

gpio pir1_int = {(uint8_t *)&PORTD, PORTD3};
gpio pir2_int = {(uint8_t *)&PORTD, PORTD7};

uint8_t rtc_int_request = 0;
uint8_t pir1_trigger = 0;
uint8_t pir2_trigger = 0;
uint8_t action_request = 0;
uint8_t active_timer = 0;
uint8_t isActive = 0;

rtc_date sys_rtc = {
	.date = 17,
	.month = 9,
	.year = 25,
	.dayofweek = 5,
	.hour = 11,
	.minute = 19,
	.second = 30
};	


#define BAT		0
#define AC		1
#define POCESS	1
#define DONE	0
uint8_t powerSrc, charge;	


	
ISR(INT0_vect){
	rtc_int_request=1;
}	

ISR(INT1_vect){
	pir1_trigger=1;
}

ISR(PCINT2_vect){
	if(gpio_get_pin_level(&pir2_int)==false){
		pir2_trigger=1;
	}
}


int main(void)
{
    sei();
    wdt_enable(WDTO_4S);
	//EEPROM_read_batch(1, &myNodeId, sizeof(myNodeId));
	//char char_array[128]="test_data\n\r";
	//uart_init(19200,1);
	twi0_init(400000);
	
	stdout = &mystdout;
	
	adc_init();
	
	gpio_set_pin_direction(&ledRedPin , PORT_DIR_OUT); gpio_set_pin_level(&ledRedPin, false);
	gpio_set_pin_direction(&ledGrnPin , PORT_DIR_OUT); gpio_set_pin_level(&ledGrnPin, false);
	gpio_set_pin_direction(&pwmDrvRST , PORT_DIR_OUT); gpio_set_pin_level(&pwmDrvRST, true);
	
	gpio_set_pin_direction(&chrgerStdby , PORT_DIR_IN);
	gpio_set_pin_direction(&chrgerChrg , PORT_DIR_IN);
	
	gpio_set_pin_direction(&pir1_int, PORT_DIR_IN);
	gpio_set_pin_direction(&pir2_int, PORT_DIR_IN);
	
	
	//TCCR0A = (0b11 << COM0A0) | (0b11 << COM0B0) | (0b01 << WGM00);
	///TCCR0B = (0b0 << WGM02) | (0b1 << CS10);
	
	rtc_set(&sys_rtc);
	rtc_int_enable(&sys_rtc ,0);
	EICRA |= (0b10 << ISC00) | (0b10 << ISC10);
	EIMSK = 0x03;
	
	PCICR |= (1 << 2);							//PCIE2 Pin Change Interrupt Enable 2
	PCMSK2 |= (1 << PCINT23);
	
	sei();
	
	led_board_init(TLC_ADDR);
	
	//printf("RUN\n\r");
	uint8_t adr = 0;
	
	sleep_enable();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    while (1) 
    {
		
		wdt_reset();
		if (rtc_int_request != 0){
			rtc_sync(&sys_rtc);
			//printf("%02d:%02d:%02d\n\r", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
			rtc_int_request = 0;
			powerSrc = (!gpio_get_pin_level(&chrgerChrg) && !gpio_get_pin_level(&chrgerStdby)) ? BAT : AC;
			charge = (gpio_get_pin_level(&chrgerChrg) && !gpio_get_pin_level(&chrgerStdby)) ? POCESS : DONE;
			
			//printf("ADC Light: %d\n\r", get_adc(LIGHT_SENSOR));
			//printf("ADC 1;2: %d;%d\n\r", get_adc(VIN_1), get_adc(VIN_2));
			//printf("Vbat; Vin: %d, %d\n\r", get_mVolt(AIN_VIN), get_mVolt(AIN_VBAT));
			//printf("Charger: Charge %d; STDB %d; src %d; chrg %d\n\r", gpio_get_pin_level(&chrgerChrg), gpio_get_pin_level(&chrgerStdby), powerSrc, charge);
			
			gpio_set_pin_level(&ledRedPin, get_mVolt(AIN_VBAT) < BAT_LOW ? true : false);
			gpio_set_pin_level(&ledGrnPin, get_adc(LIGHT_SENSOR) <= ALOVED_LIGHT_LEVEL ? true : false);
			
			if(active_timer != 0){
				if(gpio_get_pin_level(&pir2_int) == false || gpio_get_pin_level(&pir1_int) == false){
					//uart_send_string((uint8_t *)"Pir sensors wait\n\r");
					}else{
					active_timer--;
					//uart_send_string((uint8_t *)"countdown\n\r");
				}
			}
			
		}
		
		if(charge == POCESS){
			gpio_set_pin_level(&ledGrnPin, true);
			_delay_ms(100);
		}
		
		
		if (pir1_trigger == 1){
			if(isActive == 0 && get_adc(LIGHT_SENSOR) <= ALOVED_LIGHT_LEVEL){
				action_request = ON_UP_DOWN;
			}
			pir1_trigger=0;
		}
		
		if (pir2_trigger == 1){
			if(isActive == 0 && get_adc(LIGHT_SENSOR) <= ALOVED_LIGHT_LEVEL){
				action_request = ON_DOWN_UP;
			}	
			pir2_trigger=0;
		}
		
		
		if(action_request != 0 && active_timer == 0){
			//uart_send_string((uint8_t *)"CASE\n\r");
			switch (action_request) {
				case ON_UP_DOWN:
				if(isActive == 0){
					isActive = 1;
					active_timer = 5;
					gpio_set_pin_level(&pwmDrvRST, true);
					_delay_ms(25);
					led_board_init(TLC_ADDR);
					run_wave(action_request);
					action_request = OFF_UP_DOWN;
				}
				break;
				case ON_DOWN_UP:
				if(isActive == 0){
					isActive = 1;
					active_timer = 5;
					gpio_set_pin_level(&pwmDrvRST, true);
					_delay_ms(25);
					led_board_init(TLC_ADDR);
					run_wave(action_request);
					action_request = OFF_DOWN_UP;
				}
				break;
				default:
				//uart_send_string((uint8_t *)"CASE\n\r");
				if(isActive != 0){
					isActive = 0;
					run_wave(action_request);
					gpio_set_pin_level(&pwmDrvRST, false);
					action_request = 0;
				}
				break;
			}
		}
		
		
		//printf("SLEEP\n\r");
		gpio_set_pin_level(&ledGrnPin, false);
		gpio_set_pin_level(&ledRedPin, false);
		if(isActive == 0) gpio_set_pin_level(&pwmDrvRST, false);
		
		sleep_cpu();
		
    }
}

