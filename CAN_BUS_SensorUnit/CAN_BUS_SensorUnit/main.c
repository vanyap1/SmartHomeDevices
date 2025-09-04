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

#define TX_INTERVAL 10
#define GPIO_MODULE		0x27
uint8_t expanderOut, expanderIn;


gpio ledR = {(uint8_t *)&PORTD , PORTD5};
gpio ledG = {(uint8_t *)&PORTD , PORTD6};

uint32_t sysTick = 0;
uint32_t canMsgTimestamp = 0;
uint32_t oneSecondBlink = 0;

uint8_t counter;

uCAN_MSG txMessage;
uCAN_MSG rxMessage;
sensorUnitDto sensorUnit;

uint8_t outputs_regs[] =  {0xff, 0x00};

	
	

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
		sensorUnit.uid = 0x45;
		EEPROM_update_batch(1, &sensorUnit, sizeof(sensorUnit));
	}
	uart_init(9600,1);
	adc_init();
	spi1_init();
	twi1_init(400000);
	CANSPI_Initialize();
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
    
	twi1_write(GPIO_MODULE, 0x06, &outputs_regs, sizeof(outputs_regs));
	
	while (1) 
    {
		wdt_reset();
		_delay_ms(1);
		
		
		 if(CANSPI_Receive(&rxMessage))
		 {
			 printf("ID:%03lx; DLC:%x; D:%02x %02x %02x %02x %02x %02x %02x %02x\n\r", 
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
		
		
		if((sysTick - oneSecondBlink) >= 3){
			gpio_set_pin_level(&ledG, true);
			
						
			twi1_read(GPIO_MODULE, 0x00, &expanderIn, sizeof(expanderIn));
			expanderOut = ~counter;
			twi1_write(GPIO_MODULE, 0x03, &expanderOut, sizeof(expanderOut));
			
			
			oneSecondBlink = sysTick;
			txMessage.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
			txMessage.frame.id = sensorUnit.uid;
			txMessage.frame.dlc = 8;
			txMessage.frame.data0 = ~expanderIn;
			txMessage.frame.data1 = counter;
			txMessage.frame.data2 = counter;
			txMessage.frame.data3 = counter;
			txMessage.frame.data4 = counter;
			txMessage.frame.data5 = counter;
			txMessage.frame.data6 = counter;
			txMessage.frame.data7 = counter;
			CANSPI_Transmit(&txMessage);
			counter++;
			gpio_set_pin_level(&ledG, false);
		}
	
    }
}

