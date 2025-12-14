/*
 * OfficeDeskLigtingController.c
 *
 * Created: 26/09/2025 12:04:28
 * Author : Ivan Prints
 */ 
#include "config.h"
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "string.h"
#include "stdint.h"
#include "stdbool.h"

#include "uart_hal.h"
#include "gpio_driver.h"
#include "twi_hal1.h"
#include "adc_hal.h"
#include "touchPanel.h"
#include "ISL29034.h"

uint16_t inputVoltage = 0;
uint16_t fetVoltage = 0;
uint16_t outputCurrent = 0;
int16_t ntc1Celsius = 0;
int16_t ntc2Celsius = 0;
uint8_t protectionFlg = 0;
uint32_t sysTick = 0;
uint32_t serialTxTimestamp;
uint32_t outSequencerTimestamp;


//GPIO Module variables
#define GPIO_MODULE		0x27
uint8_t outputs_regs[] =  {0xff, 0x00};
uint8_t outputRegState = 0xff;
uint8_t outputState = 0;



//Machine state vars
bool outEn = false;
bool outEnSave = false;
bool ambilightEn = false;
bool spotlightEn = false;
uint8_t fanMode = 0;
uint8_t fanStates[] = {0, 90, 110, 150, 192, 255};
uint8_t ambilightLevel = 0;
uint8_t spotLightVal = 0;
bool btnState[] = {false, false, false, false};

uint8_t seqBitNum = 4;

static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);

gpio drvEn = {(uint8_t *)&PORTD , PORTD7};
gpio ledRun = {(uint8_t *)&PORTE , PORTE3};
gpio ledFail = {(uint8_t *)&PORTC , PORTC0};

gpio ch1Config = {(uint8_t *)&PORTB , PORTB2};
gpio ch2Config = {(uint8_t *)&PORTB , PORTB3};
gpio ch3Config = {(uint8_t *)&PORTD , PORTD6};
gpio ch4Config = {(uint8_t *)&PORTD , PORTD5};

ISR(TIMER4_COMPA_vect)
{
	sysTick++;
}


int main(void)
{
    sei();
    wdt_enable(WDTO_8S);
	//wdt_disable();
    gpio_set_pin_level(&drvEn, false); gpio_set_pin_direction(&drvEn , PORT_DIR_OUT); 
    gpio_set_pin_level(&ledRun, false);gpio_set_pin_direction(&ledRun , PORT_DIR_OUT); 
    gpio_set_pin_level(&ledFail, true); gpio_set_pin_direction(&ledFail , PORT_DIR_OUT); 
    
    gpio_set_pin_level(&ch1Config, false); gpio_set_pin_direction(&ch1Config , PORT_DIR_OUT); 
    gpio_set_pin_level(&ch2Config, false); gpio_set_pin_direction(&ch2Config , PORT_DIR_OUT); 
    gpio_set_pin_level(&ch3Config, false); gpio_set_pin_direction(&ch3Config , PORT_DIR_OUT); 
    gpio_set_pin_level(&ch4Config, false); gpio_set_pin_direction(&ch4Config , PORT_DIR_OUT); 
    
	adc_init();
	uart_init(SERIAL_BAUD, 0);
	stdout = &mystdout;
	//Set channels default values
	CH1 = 0;
	CH2 = 0;
	CH3 = 0;
	CH4 = 0;
	
	//Setup timer 0, CH A and B for out 3 and 4
	TCCR0A = (0b10 << COM0A0) | (0b10 << COM0B0) | (0b01 << WGM00);
	TCCR0B = (0b0 << WGM02) | (0b1 << CS10);
	
	//Setup timer 1, CH B for out 1
	TCCR1A = (0b10 << COM1B0) | (0b01 << WGM00);
	TCCR1B = (0b0 << WGM02) | (0b1 << CS10);
	
	//Setup timer 2, CH A for out 2
	TCCR2A = (0b10 << COM2A0) | (0b01 << WGM00);
	TCCR2B = (0b0 << WGM02) | (0b1 << CS10);
	
	
	_delay_ms(100);
	gpio_set_pin_level(&ledFail, false);
	
	
	TCCR4B |= (1 << WGM12 );   // Configure timer 1 for CTC mode
	OCR4A = 312;             // Set CTC compare value to 10Hz (100mS)
	// at 16MHz AVR clock, with a prescaler of 64
	//OCR1A = (16_000_000 / 256 / 10) - 1
	//= (6250) - 1
	//= 6249
	TIMSK4 |= (1 << OCIE4A );  // Enable CTC interrupt
	TCCR4B |= (1 << CS12 );// | (1 << CS11 ));
	
	twi1_init(400000);
	
	touchPanelInit(0x2D);
	ISL29034_init(POWER_RUN, RES_16_BIT, LUX_4000);
	twi1_write(GPIO_MODULE, 0x03, &outputRegState, sizeof(outputRegState));
	twi1_write(GPIO_MODULE, 0x06, &outputs_regs, sizeof(outputs_regs));
	
	touchPanelSetLedState(LED_R_POWER, true);
	
	
	uint8_t addr = 0x2D;
	bool ld1 = false;
	uint8_t ledNum;
	while (1) 
    {
		
		
		if((sysTick - serialTxTimestamp) > 50){
			wdt_reset();
			ntc1Celsius = getNTC(NTC1);
			inputVoltage = get_mVolt(VOLTMONITOR1);
			outputCurrent = get_mAmps(CURRMONITOR);
			serialTxTimestamp = sysTick;
			//printf("V:%02u; I:%02u; T:%02d; int: %d; L: %u\n\r", inputVoltage, outputCurrent, ntc1Celsius, touchPanelIntState(), read_light_intensity());
			
			
		}
		
		if((sysTick - outSequencerTimestamp) > 100){
			outSequencerTimestamp = sysTick;
			if(outEn){
				if(seqBitNum < 4){
					outputState |= 1 << seqBitNum;
					seqBitNum ++;
				}
			}else{
				outputState = 0;
				seqBitNum = 0;
			}
			if(outEnSave) outputState |= 1 << 0; 
			
			outputRegState = ~outputState;
			twi1_write(GPIO_MODULE, 0x03, &outputRegState, sizeof(outputRegState));
			
		}
		if(!ambilightEn && (outEnSave || outEn)){
			if(read_light_intensity() <= 200){
				if(ambilightLevel != 255) ambilightLevel++;
			}else{
				if(ambilightLevel != 0) ambilightLevel--;
			}
		}else{
			if(ambilightLevel != 0) ambilightLevel--;
		}
		
		if(spotlightEn && (outEnSave || outEn)){
			if(spotLightVal != 255) spotLightVal++;
		}else{
			if(spotLightVal != 0) spotLightVal--;
		}
		
		CH4 = ambilightLevel;
		CH2 = spotLightVal;
		
		uint16_t key = touchPanelGetKeyCode();
		if(key != 0){
			if(key == BTN_POWER){
				outEn = !outEn;
				touchPanelSetLedState(LED_G_POWER, outEn);
				touchPanelSetLedState(LED_R_POWER, !outEn);
				if(!outEn){
					outEnSave = false;
					spotlightEn = false;
					touchPanelSetLedState(BTN_POWER_SAVE, false);
					touchPanelSetLedState(LED_AMBILIGHT, false);
					touchPanelSetLedState(LED_FAN, false);
					touchPanelSetLedState(LED_G_SPOT, false);
					fanMode = 0;
					CH3 = fanStates[fanMode];
				}else{
					touchPanelSetLedState(BTN_POWER_SAVE, false);
				}
				printf("BTN_POWER\n\r");
				
			}
			if(key == BTN_POWER_SAVE){
				printf("BTN_POWER_SAVE\n\r");
				if(!outEn){
					printf("1\n\r");
					outEnSave = !outEnSave;
					touchPanelSetLedState(BTN_POWER_SAVE, outEnSave);
					
					
					if(!outEnSave){
						printf("3\n\r");
						touchPanelSetLedState(LED_G_POWER, false);
						touchPanelSetLedState(LED_R_POWER, true);
						outEnSave = false;
						spotlightEn = false;
						touchPanelSetLedState(BTN_POWER_SAVE, false);
						touchPanelSetLedState(LED_AMBILIGHT, false);
						touchPanelSetLedState(LED_FAN, false);
						touchPanelSetLedState(LED_G_SPOT, false);
						fanMode = 0;
						CH3 = fanStates[fanMode];
					}
					
				}else{
				touchPanelSetLedState(BTN_POWER_SAVE, true);
				_delay_ms(100);
				touchPanelSetLedState(BTN_POWER_SAVE, false);
			}
				
			}
			
			if(key == BTN_AMBILIGHT){
				printf("BTN_AMBILIGHT\n\r");
				if(outEnSave || outEn){
					ambilightEn = !ambilightEn;
					touchPanelSetLedState(LED_AMBILIGHT, ambilightEn);
				}
			}
			if(key == BTN_FAN){
				if(outEnSave || outEn){
					fanMode++;
					if(fanMode >= sizeof(fanStates)) fanMode = 0;
					touchPanelSetLedState(LED_FAN, false);
					_delay_ms(100);
					touchPanelSetLedState(LED_FAN, fanMode ? true : false);
					CH3 = fanStates[fanMode];
				}
				printf("BTN_FAN %d; %d\n\r", fanMode, fanStates[fanMode]);
				
			}
			
			if(key == BTN_SPOTLIGHT){
				printf("BTN_SPOTLIGHT\n\r");
				if(outEnSave || outEn){
					spotlightEn = !spotlightEn;
					touchPanelSetLedState(LED_G_SPOT, spotlightEn);
				}
			}	
			
			if(key == BTN_1){
				btnState[0] = !btnState[0];
				touchPanelSetLedState(LED_1, btnState[0]);
			}
			if(key == BTN_2){
				btnState[1] = !btnState[1];
				touchPanelSetLedState(LED_2, btnState[1]);
			}
			
			if(key == BTN_3){
				btnState[2] = !btnState[2];
				touchPanelSetLedState(LED_3, btnState[2]);
			}
			
			if(key == BTN_4){
				btnState[3] = !btnState[3];
				touchPanelSetLedState(LED_4, btnState[3]);
			}
			
			
			printf("keyCode: %u\n\r", key);
			
		}		
		_delay_ms(10);
		
    }
}

