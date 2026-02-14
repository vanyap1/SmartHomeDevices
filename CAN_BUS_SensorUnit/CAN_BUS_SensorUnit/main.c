/*
 * CAN_BUS_SensorUnit.c
 *
 * Created: 11/08/2025 19:16:47
 * Author : Ivan Prints
 */ 

#include "config.h"
#include "dto.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <inttypes.h>

#include "stdbool.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "uart_hal.h"
#include "gpio_driver.h"
#include "twi_hal0.h"
#include "spi0_hall.h"
#include "adc_hal.h"
#include "eeprom.h"
#include "canspi.h"

#include <stdio.h>
static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);

#define TX_INTERVAL		10
#define GPIO_MODULE		0x27
#define NTC1			4
#define NTC2			5
#define NTC3			3
#define NTC4			2
#define NTC5			6
int16_t tempCelsius[5];

uint8_t expanderOut, expanderIn;


gpio ledR = {(uint8_t *)&PORTD , PORTD5};
gpio ledG = {(uint8_t *)&PORTD , PORTD6};

uint32_t sysTick = 0;
uint32_t canMsgTimestamp = 0;
uint32_t ntcMeasureReq = 0;
uint32_t lastCanMsgTimestamp = 0;

uint8_t counter;

uCAN_MSG txMessage;
uCAN_MSG rxMessage;
sensorUnitDto sensorUnit;

uint8_t outputs_regs[] =  {0xff, 0x00};


static int32_t temp_accumulator[5] = {0};
static bool first_run = true;
	
	

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
	sysTick++;
}


int main(void)
{
    uint8_t resetCause = MCUSR;
    MCUSR = 0;
    sei();
    wdt_enable(WDTO_4S);
	
	EEPROM_read_batch(1, &sensorUnit, sizeof(sensorUnit));
	if(sensorUnit.dataValid != 0xaa){
		sensorUnit.dataValid = 0xaa;
		sensorUnit.uid = 0x80;
		EEPROM_update_batch(1, &sensorUnit, sizeof(sensorUnit));
	}
	uart_init(9600,1);
	adc_init();
	spi1_init();
	twi1_init(400000);
	CANSPI_Initialize();
	CANSPI_SetDualFilter(0x069, 0x082, false);
	stdout = &mystdout;
	
	gpio_set_pin_direction(&ledG , PORT_DIR_OUT); gpio_set_pin_level(&ledG, false);
	gpio_set_pin_direction(&ledR , PORT_DIR_OUT); gpio_set_pin_level(&ledR, true);
	
	TCCR1B |= (1 << WGM12 );   // Configure timer 1 for CTC mode
	OCR1A = 6249;             // Set CTC compare value to 10Hz (100mS)
	// at 16MHz AVR clock, with a prescaler of 64
	//OCR1A = (16_000_000 / 256 / 10) - 1
	//= (6250) - 1
	//= 6249
	TIMSK1 |= (1 << OCIE1A );  // Enable CTC interrupt
	TCCR1B |= (1 << CS12 );// | (1 << CS11 ));
	
	printf("RUN\n\r");
	_delay_ms(500);
	gpio_set_pin_direction(&ledR , PORT_DIR_OUT); gpio_set_pin_level(&ledR, false);
    
	//twi1_write(GPIO_MODULE, 0x06, &outputs_regs, sizeof(outputs_regs));
	
	txMessage.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
	txMessage.frame.id = 0x15;
	txMessage.frame.dlc = 8;
	txMessage.frame.data0 = 1;
	txMessage.frame.data1 = 0;
	CANSPI_Transmit(&txMessage);
	printf("\033[2J\033[H");
	printf("\033[?25l");
	printf("5 CH ADC Controller, ID:%d\r\n", sensorUnit.uid);
	while (1) 
    {
		if(sysTick - lastCanMsgTimestamp < 100){
			wdt_reset();
		}
		
		_delay_ms(1);
		
		
		 if(CANSPI_Receive(&rxMessage))
		 {
			 lastCanMsgTimestamp = sysTick;
			 if (rxMessage.frame.id == 0x082)
			 {
				 if (rxMessage.frame.data0 == 1)
				 {
					 sensorUnit.reportCan = rxMessage.frame.data1;
					 EEPROM_update_batch(1, &sensorUnit, sizeof(sensorUnit));
				 }
			 }
			 
			 
			 if(sensorUnit.reportCan != 0){
				 printf("ID:%03lx; DLC:%x; D:%02x %02x %02x %02x %02x %02x %02x %02x \n\r",
				 rxMessage.frame.id,
				 rxMessage.frame.dlc,
				 rxMessage.frame.data0,
				 rxMessage.frame.data1,
				 rxMessage.frame.data2,
				 rxMessage.frame.data3,
				 rxMessage.frame.data4,
				 rxMessage.frame.data5,
				 rxMessage.frame.data6,
				 rxMessage.frame.data7
				 );
			 } 
		 }
		
		
		if((sysTick - ntcMeasureReq) >= 3){
			ntcMeasureReq = sysTick;
			gpio_set_pin_level(&ledG, true);
			
			uint8_t pins[] = {NTC1, NTC2, NTC3, NTC4, NTC5};

			for(uint8_t i = 0; i < 4; i++) {
				int16_t current_raw = getNTC(pins[i]);
				if (first_run) {
					temp_accumulator[i] = (int32_t)current_raw << 4;
					} else {
					temp_accumulator[i] = temp_accumulator[i] - (temp_accumulator[i] >> 4) + current_raw;
				}
				tempCelsius[i] = (int16_t)(temp_accumulator[i] >> 4);
			}
			first_run = false;
			
			printf("t: %" PRIi16 "; %" PRIi16 "; %" PRIi16 "; %" PRIi16 ";  %" PRIi32 "   \r",
			tempCelsius[0],
			tempCelsius[1],
			tempCelsius[2],
			tempCelsius[3],
			sysTick - lastCanMsgTimestamp);
						
						
						
			//twi1_read(GPIO_MODULE, 0x00, &expanderIn, sizeof(expanderIn));
			//expanderOut = ~counter;
			//twi1_write(GPIO_MODULE, 0x03, &expanderOut, sizeof(expanderOut));
			
			memcpy(&txMessage.frame.data0, &tempCelsius, 8);
			txMessage.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
			txMessage.frame.id = sensorUnit.uid;
			txMessage.frame.dlc = 8;
			CANSPI_Transmit(&txMessage);
			gpio_set_pin_level(&ledG, false);
		}
	
    }
}

