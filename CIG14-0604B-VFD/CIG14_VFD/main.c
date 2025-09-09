/*
 * CIG14_VFD.c
 *
 * Created: 07.05.2023 18:52:06
 * Author : User
 */ 

#include "config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "string.h"
#include "stdbool.h"
#include "lib/uart_hal.h"
#include "lib/gpio_driver.h"
#include "lib/twi_hal.h"
#include "lib/adc_hal.h"
#include "lib/rtc.h"
#include "lib/ISL29034.h"
#include "lib/cig14.h"


uint8_t light_addr = 0x44;

gpio ld1 = {(uint8_t *)&PORTB , PORTB5};
gpio rtc_int = {(uint8_t *)&PORTD , PORTD2};
gpio p_good = {(uint8_t *)&PORTC , PORTC5};
gpio lcd_blk = {(uint8_t *)&PORTE , PORTE2};




rtc_date sys_rtc = {0, 47, 15, 26, 8, 24, 2};
//second, minute, hour, date, month, year, dayofweek


uint8_t rtc_int_request = 0;

ISR(INT0_vect){
	rtc_int_request=1; //RTC New data ready
}


int main(void)
{
	char char_array[128]="\0";
	uart_init(250000,0);
	twi_init(400000);
		
	EICRA |= (0b10 << ISC00);
	EIMSK = 0x01;
	
	set_pin_dir(&ld1 , PORT_DIR_OUT); set_pin_level(&ld1, false);
	set_pin_dir(&rtc_int, PORT_DIR_IN);
		
	sei();
		
	//rtc_set(&sys_rtc);																//write time to RTC
	rtc_int_enable(&sys_rtc ,0);														//Serup RTC
	adc_init();																			//ADC init
	//ISL29034_init(POWER_RUN, RES_16_BIT, LUX_4000);										//Setup light sensor
	//vfd_init(6);
	vfd16_init();
	
	//vfd_set_brightness(0xff);															//set vfd brightness (0x00 to 0xff)
	
	uart_send_string((uint8_t *)"Program Start\n\r");
	
	
	//sleep_enable();
	//set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
    while (1) 
    {
		if (rtc_int_request != 0){
			rtc_int_request = 0;
			//set_pin_level(&ld1, true);												//Debug led
			rtc_sync(&sys_rtc);
			sprintf(char_array, "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
			//_delay_ms(1);																//If need sleep before next RTC iteration
			//sleep_cpu();
		}
		
		//sprintf(char_array, "%05u", read_light_intensity());							//Read light intensity from i2c light sensor
		//vfd_set_brightness(read_light_intensity());
		//vfd_string(0, (uint8_t *)char_array);											//Write char array to vfd screen
		//uart_send_string((uint8_t *)char_array);
		//uart_send_string((uint8_t *)"\n\r");
		
		vfd16b_string(4, (uint8_t *)char_array);	
		_delay_ms(100);
		
    }
}

