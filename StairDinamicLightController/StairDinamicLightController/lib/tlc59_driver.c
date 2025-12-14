
/*
 * tlc59_driver.c
 *
 * Created: 05.10.2023 22:26:29
 *  Author: Vanya
 */ 

#include "twi_hal0.h" 
#include "tlc59_driver.h"
#include <util/delay.h>
#include "config.h"
uint8_t brightness_on[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 100, 250, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 0};
uint8_t brightness_off[] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 250, 100, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t led_pattern[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void led_board_sw_reset(uint8_t board_address){
	uint8_t reset_cmd[] = {SWRST_byte_1, SWRST_byte_2};
	uint8_t init_led_pattern[16] = {0x20,0xff,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x2f,0x20,0x20};
	twi0_write(board_address, PWM0 | 0xA0, init_led_pattern, sizeof(init_led_pattern));
	twi0_transfer(board_address, reset_cmd, sizeof(reset_cmd));
	uint8_t reg = (1 << OSC);
	twi0_write(board_address,MODE1, &reg,1);
	//twi_write(board_address,MODE2, &reg,1);
	//twi_write(SWRST,TIMER_COUNTER0_REG,SWRST,1);
}

uint8_t led_board_init(uint8_t board_address){
	uint8_t init_cmd[] = {0xAA, 0xAA, 0xAA, 0xAA};
	uint8_t *init_led_pattern = led_pattern;
	uint8_t reg = 0;
	reg = 0;
	twi0_write(board_address,MODE1, &reg,1);
	twi0_write(board_address,MODE2, &reg,1);
	twi0_write(board_address, PWM0 | 0xA0, init_led_pattern, sizeof(init_led_pattern));
	reg = 0x00;
	twi0_write(board_address,GRPPWM,&reg,1);
	reg = 0xFF;
	twi0_write(board_address,GRPFREQ,&reg,1);
	reg = (2 << LDR0) | (2 << LDR1) | (2 << LDR2) | (2 << LDR3);
	twi0_write(board_address,LEDOUT0,&reg,1);
	reg = (2 << LDR4) | (2 << LDR5) | (2 << LDR6) | (2 << LDR7);
	twi0_write(board_address,LEDOUT1,&reg,1);
	reg= (2 << LDR8) | (2 << LDR9) | (2 << LDR10) | (2 << LDR11);
	twi0_write(board_address,LEDOUT2,&reg,1);
	reg = (2 << LDR12) | (2 << LDR13) | (2 << LDR14) | (2 << LDR15);
	twi0_write(board_address,LEDOUT3,&reg,1);
	return 1;
	
}

uint8_t led_board_sleep(uint8_t board_address){
	
	uint8_t regMode1 = 0x10; // SLEEP=1
	twi0_write(TLC_ALLCALL, MODE1, &regMode1, 1);

	uint8_t regMode2 = 0x00; // OUTDRV=1 (totem-pole outputs)
	twi0_write(TLC_ALLCALL, MODE2, &regMode2, 1);

	uint8_t zero = 0x00;
	twi0_write(TLC_ALLCALL, GRPFREQ, &zero, 1);

	for(uint8_t r = LEDOUT0; r <= LEDOUT3; r++) {
		twi0_write(TLC_ALLCALL, r, &zero, 1);
	}

	for(uint8_t p = PWM0; p <= (PWM0+15); p++) {
		twi0_write(TLC_ALLCALL, p, &zero, 1);
	}

	return 1;
}

void led_write_batch(uint8_t board_address, int *pwmChannels, uint8_t channels_num){
	twi0_write(board_address, PWM0 | 0xA0, pwmChannels, channels_num);
}

void led_write_single(uint8_t board_address, uint8_t led_num, uint8_t brightness){
	uint8_t ledAddr = led_num+2;
	ledAddr = (ledAddr < 16) ? ledAddr : 15;
	twi0_write(board_address, ledAddr | 0xA0, brightness, 2);
}

void run_wave(uint8_t cmd) {
	uint8_t i = 0;
	uint8_t shifts = 0;
	uint8_t direction=0;
	uint8_t *brightness_ptr;
	uint8_t *leds_array = led_pattern;
	switch (cmd) {
		case ON_DOWN_UP:
		case OFF_DOWN_UP:
		direction = 0;
		brightness_ptr = (cmd == OFF_DOWN_UP || cmd == OFF_UP_DOWN) ? brightness_off : brightness_on;
		break;
		case ON_UP_DOWN:
		case OFF_UP_DOWN:
		direction = 1;
		brightness_ptr = (cmd == OFF_DOWN_UP || cmd == OFF_UP_DOWN) ? brightness_off : brightness_on;
		break;
		default:
		direction = 0;
		brightness_ptr = brightness_on;
		break;
	}
		
	if (direction == 0){
		for (shifts = 1; shifts < 20; shifts++) {
			for (i = 0; i < 16; i++) {
				leds_array[i] = brightness_ptr[i + shifts];
			}
			led_write_batch(TLC_ADDR, leds_array, 16);
			_delay_ms(SHIF_DELAY);
		}
	}
	if (direction == 1){
		for (shifts = 1; shifts < 20; shifts++) {
			uint8_t reverce_index=16;
			for (i = 0; i < 16; i++) {
				reverce_index--;
				leds_array[reverce_index] = brightness_ptr[i + shifts];
			}
			led_write_batch(TLC_ADDR, leds_array, 16);
			_delay_ms(SHIF_DELAY);
		}
	}
	
}


//How to use:
//predefine led bright level pattern
//
//uint8_t led_pattern_1[16] = {0x0f,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//uint8_t led_pattern_2[16] = {0x00,0x0f,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//
//
//Write pattern to led board
//led_write_batch(panel_addr, led_pattern_1,sizeof(led_pattern_1));
//_delay_ms(250);
//led_write_batch(panel_addr, led_pattern_2,sizeof(led_pattern_2));