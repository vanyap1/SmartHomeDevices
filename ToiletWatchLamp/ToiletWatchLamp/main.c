/*
 * ToiletWatchLamp.c
 *
 * Created: 22.11.2023 21:49:15
 * Author : Vanya
 */ 

//#include "config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


#include "display_fn.h"
#include "u8g2.h"
#include "mui.h"
#include "mui_u8g2.h"
#include "string.h"
#include "stdbool.h"
#include "uart_hal.h"
#include "gpio_driver.h"
#include "twi_hal1.h"
#include "adc_hal.h"
//#include "spi1_hall.h"
#include "rtc.h"
#include "stdint.h"
#include <stdio.h>
static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);

u8g2_t lcd;
uint8_t display_line[64];

gpio rtc_int = {(uint8_t *)&PORTD , PORTD2};
gpio lcd_blk = {(uint8_t *)&PORTE , PORTE2};
gpio ext_led = {(uint8_t *)&PORTD , PORTD4};


rtc_date sys_rtc = {
	.date = 18,
	.month = 1,
	.year = 25,
	.dayofweek = 5,
	.hour = 10,
	.minute = 35,
	.second = 00
};

#define LIGHT_TRASHEL_LEVEL			100
#define BAT_LOW_LEVEL				380
#define LCD_CONTRAST				120

#define TIMER_LB_OFFSET				20			//minus from timer if battery is critical
#define LIGHT_TIMER_MAX				30			//illumination time


uint8_t rtc_int_request = 0;
uint16_t BAT_VOLT = 0;
uint16_t LIGHT_LEVEL = 0;
uint16_t LIGHT_TIMER = 0;
uint8_t BAT_INDICATOR_VAL = 0;

ISR(INT0_vect){
	rtc_int_request=1;
}

ISR(INT1_vect){
	if(LIGHT_LEVEL <= 20){
		LIGHT_TIMER = (BAT_VOLT >= BAT_LOW_LEVEL) ? LIGHT_TIMER_MAX : LIGHT_TIMER_MAX - TIMER_LB_OFFSET;
		rtc_int_request=1;
		}
	}
	

int main(void)
{
	char char_array[128]="\0";
	char bat_level[1];
    twi1_init(400000);
    gpio_set_pin_direction(&lcd_blk , PORT_DIR_OUT); gpio_set_pin_level(&lcd_blk, false);
    gpio_set_pin_direction(&ext_led , PORT_DIR_OUT); gpio_set_pin_level(&ext_led, false);
    gpio_set_pin_direction(&rtc_int, PORT_DIR_IN);
    EICRA |= (0b10 << ISC00) | (0b10 << ISC10);
    EIMSK = 0x03; //0b00000011
    
    sei();
	stdout = &mystdout;
	//rtc_set(&sys_rtc);
	rtc_int_enable(&sys_rtc ,0);
	adc_init();
	//spi1_init();
	lcd_gpio_init();
	//u8g2_Setup_st7565_zolen_128x64_f( &lcd, U8G2_MIRROR_VERTICAL, lcd_hw_spi, fake_delay_fn);					//HW SPI
	//u8g2_Setup_ssd1306_i2c_128x32_univision_f(&lcd, U8G2_R0, u8x8_byte_sw_i2c, fake_delay_fn);					//0.91 OLED 128x32
	u8g2_Setup_st7565_zolen_128x64_f(&lcd, U8G2_MIRROR_VERTICAL, u8x8_byte_4wire_sw_spi, lcd_software_spi);	//SW SPI
	
	
	
	
	u8g2_InitDisplay(&lcd);
	u8g2_SetPowerSave(&lcd, 0);
	u8g2_SetFlipMode(&lcd, 1);
	u8g2_SetContrast(&lcd, LCD_CONTRAST);
	u8g2_ClearBuffer(&lcd);
	//u8g2_SetFont(&lcd, u8g2_font_6x10_mf);
	
	
	//u8g2_SetFont(&lcd, u8g2_font_ncenB14_tr);
	//u8g2_DrawStr(&lcd, 1, 10, (void *)"RX MODULE");
	u8g2_SendBuffer(&lcd);
	
	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
    while (1) 
    {
		if (rtc_int_request != 0){
			adc_init();
			rtc_sync(&sys_rtc);
			BAT_VOLT = get_mVolt(ADC4_PIN);
			BAT_INDICATOR_VAL = (BAT_VOLT >= 370) ? BAT_VOLT - 370 : 0;
			BAT_INDICATOR_VAL = (BAT_INDICATOR_VAL >= 50) ? 50 : BAT_INDICATOR_VAL;
			BAT_INDICATOR_VAL = BAT_INDICATOR_VAL / 10;
			if(BAT_INDICATOR_VAL != 5) BAT_INDICATOR_VAL++;
			LIGHT_LEVEL = get_mVolt(ADC5_PIN);
			
			if(LIGHT_LEVEL <= 5 && LIGHT_TIMER == 0){
				rtc_int_request = 0;
				gpio_set_pin_level(&ext_led, false);
				gpio_set_pin_level(&lcd_blk, false);
				ADCSRA &= ~(1 << ADEN);
				MCUCR |= (1 << BODS) | (1 << BODSE);
				MCUCR &= ~(1 << BODSE);
				sleep_cpu(); // Переводим МК в спящий режим
			}else{
				u8g2_ClearBuffer(&lcd);
				
				u8g2_DrawRFrame(&lcd, 0, 0, 128 ,50, 5);
				u8g2_DrawRFrame(&lcd, 0, 49, 128, 15, 5);
				u8g2_DrawLine(&lcd, 108,1, 108,26);
				u8g2_DrawLine(&lcd, 108,26, 128,26);
				sprintf(char_array, "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
				u8g2_SetFont(&lcd, u8g2_font_logisoso22_tn);
				u8g2_DrawStr(&lcd, 1, 25, (void *)char_array);
				
				sprintf(char_array, "V:%03d; T:%02d l:%04d", BAT_VOLT, LIGHT_TIMER, LIGHT_LEVEL);
				u8g2_SetFont(&lcd, u8g2_font_6x10_mf);
				u8g2_DrawStr(&lcd, 6, 60, (void *)char_array);
				
				sprintf(char_array, "%02d-%02d-20%02d", sys_rtc.date, sys_rtc.month, sys_rtc.year);
				u8g2_SetFont(&lcd, u8g2_font_ncenB14_tr);
				u8g2_DrawStr(&lcd, 14, 45, (void *)char_array);
				
				
				//battery indicator
				sprintf(bat_level, "%01d", BAT_INDICATOR_VAL);
				u8g2_SetFont(&lcd, u8g2_font_battery19_tn);
				u8g2_DrawStr(&lcd, 114, 23, bat_level);
				
				if (LIGHT_TIMER != 0){
					LIGHT_TIMER--;
					gpio_set_pin_level(&ext_led, true);
					gpio_set_pin_level(&lcd_blk, true);
					}else{
					gpio_set_pin_level(&ext_led, false);
					gpio_set_pin_level(&lcd_blk, false);
				}
				rtc_int_request = 0;
				
				u8g2_SendBuffer(&lcd);
				sleep_cpu();
			}			
		}
			
    }
}

