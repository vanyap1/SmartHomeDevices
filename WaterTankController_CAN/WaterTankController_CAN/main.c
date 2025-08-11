/*
 * WaterTankController_CAN.c
 *
 * Created: 12/07/2025 19:13:02
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

#define I2CEXPANDERADDR			0x27
#define CANID_PRESSURE_REPORT	0x301
#define CANID_CONFIG_UPDATE		0x303
#define CANID_MANUAL_RUN		0x304

#define TX_INTERVAL 10


uint16_t waterPressureRaw = 0;
int32_t df1 = 0;              // ?????? ???, ??? ???????? ????????????
int32_t filteredPressure = 0; // ??????????? ???????? (????? ????????? ?? int32_t ??? ????????? ?? uint16_t)
#define KFI1 10               // ?????????? ???????????? (10 = ???????? ??????????, ?????? ??????????)

pumpLimits pumpLims;

gpio outEn = {(uint8_t *)&PORTD , PORTD7};
gpio ld19 = {(uint8_t *)&PORTD , PORTD5};
gpio ld20 = {(uint8_t *)&PORTD , PORTD6};

uint32_t sysTick = 0;
uint32_t canMsgTimestamp = 0;
uCAN_MSG txMessage;
uCAN_MSG rxMessage;
uint8_t measureRequest = 0;
uint8_t txRequest = 0;
uint8_t txRequestHelperCounter = 0;

uint16_t waterPressure = 0;
uint8_t i2cData[2] = {0xff, 0xff};
uint8_t outputsState, inputstate;
uint8_t i2cRxData[2];
uint8_t lockPump = 0;
uint8_t manualRun = 0;
uint8_t pumpEn = 0;
const uint8_t* sonarData;
uint16_t sonarDist;

//#define  DEBUG
#define  PUMP		5
#define  GRN_LAMP	4
#define  YEL_LAMP	6
#define  RED_LAMP	7

#define LO_LEVEL	0
#define MID_LEVEL	1
#define HI_LEVEL	2

#define UNLOCK		0
#define LOCK		1


#define SET(pin)     (outputsState |=  (1 << (pin)))
#define RESET(pin)   (outputsState &= ~(1 << (pin)))
#define ISSET(pin)   ((i2cRxData[1] & (1 << (pin))) != 0)


ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
	measureRequest = 1;
	sysTick++;
	txRequestHelperCounter++;
	if(txRequestHelperCounter >= TX_INTERVAL){
		txRequestHelperCounter = 0;
		txRequest = 1;
	}
}


int main(void)
{
	uint8_t resetCause = MCUSR;
	MCUSR = 0;
	sei();
	wdt_enable(WDTO_4S);
	
	//EEPROM_update_batch(1, &myNodeId, sizeof(myNodeId));
	EEPROM_read_batch(1, &pumpLims, sizeof(pumpLims));
	if(pumpLims.eepromOk != 0xaa){
		pumpLims.eepromOk = 0xaa;
		pumpLims.pressureDelta = 45;
		pumpLims.pressureLoLim = 190;
		pumpLims.pressureHiLim = pumpLims.pressureLoLim + pumpLims.pressureDelta;
		EEPROM_update_batch(1, &pumpLims, sizeof(pumpLims));
	}
	
    uart_init(9600,1);
	twi0_init(400000);
	adc_init();
	spi0_init();
	CANSPI_Initialize();
	stdout = &mystdout;
	
	gpio_set_pin_direction(&outEn , PORT_DIR_OUT); gpio_set_pin_level(&outEn, true);
	gpio_set_pin_direction(&ld19 , PORT_DIR_OUT); gpio_set_pin_level(&ld19, false);
	gpio_set_pin_direction(&ld20 , PORT_DIR_OUT); gpio_set_pin_level(&ld20, true);
	
	TCCR1B |= (1 << WGM12 );   // Configure timer 1 for CTC mode
	OCR1A = 6249;             // Set CTC compare value to 10Hz (100mS)
	// at 16MHz AVR clock, with a prescaler of 64
	//OCR1A = (16_000_000 / 256 / 10) - 1
	//= (6250) - 1
	//= 6249
	TIMSK1 |= (1 << OCIE1A );  // Enable CTC interrupt
	TCCR1B |= (1 << CS12 );// | (1 << CS11 ));
	
	_delay_ms(150);
	gpio_set_pin_direction(&ld20 , PORT_DIR_OUT); gpio_set_pin_level(&ld20, false);
	
    while (1) 
    {
		wdt_reset();
		_delay_ms(1);
		 if(CANSPI_Receive(&rxMessage))
		 {	
			 canMsgTimestamp = sysTick;
			 if(rxMessage.frame.id == CANID_CONFIG_UPDATE && rxMessage.frame.data7 == 0x55){
				 #ifdef DEBUG
					gpio_set_pin_level(&ld20, true);
				 #endif
				 
				 pumpLims.pressureLoLim = (rxMessage.frame.data0 << 8) | rxMessage.frame.data1;
				 pumpLims.pressureDelta = rxMessage.frame.data2;
				 pumpLims.pressureHiLim = pumpLims.pressureLoLim + pumpLims.pressureDelta;
				 EEPROM_update_batch(1, &pumpLims, sizeof(pumpLims));
				 #ifdef DEBUG
					_delay_ms(50);
					gpio_set_pin_level(&ld20, false);
					printf("Limits updated; LoLim: %d; HiLim: %d; delta: %d", pumpLims.pressureLoLim, pumpLims.pressureHiLim, pumpLims.pressureDelta);
				 #endif
			 }
			 if(rxMessage.frame.id == CANID_MANUAL_RUN && rxMessage.frame.data7 == 0x55){
				manualRun = rxMessage.frame.data0;
			 }
			 
		 }

		
		if(txRequest){
			txMessage.frame.idType = dSTANDARD_CAN_MSG_ID_2_0B;
			txMessage.frame.id = CANID_PRESSURE_REPORT;
			txMessage.frame.dlc = 8;
			txMessage.frame.data0 = i2cRxData[1];
			txMessage.frame.data1 = i2cRxData[0];
			txMessage.frame.data2 = (waterPressure >> 8) & 0xff;
			txMessage.frame.data3 = waterPressure & 0xff;
			txMessage.frame.data4 = (pumpLims.pressureLoLim >> 8) & 0xff;
			txMessage.frame.data5 = pumpLims.pressureLoLim & 0xff;
			txMessage.frame.data6 = pumpLims.pressureDelta;
			txMessage.frame.data7 = (uint8_t)sonarDist; 
			CANSPI_Transmit(&txMessage);
			txRequest=0;
			
			#ifdef DEBUG
				printf("Sonar: %d; Press: %d; Hlim: %d; LLim: %d; Delta: %d; Manual: %d \n\r", sonarDist, waterPressure, pumpLims.pressureHiLim, pumpLims.pressureLoLim, pumpLims.pressureDelta, manualRun);
			#endif
			
			if((sysTick - canMsgTimestamp) > 100){
				CANSPI_Initialize();
				gpio_set_pin_level(&ld20, true);
				_delay_ms(100);
				gpio_set_pin_level(&ld20, false);
			}
			
		}
		if (serialBytesAvailable() >= 4) {					//Ultrasonic water level meter handler
			sonarData = (const uint8_t*)serial_read_data();
			if(sonarData[0] == 0xff){
				sonarDist = (sonarData[1] << 8 ) + sonarData[2];
				sonarDist = (uint16_t)(((uint32_t)sonarDist * 10) / 100);
				if(sonarDist >=250) sonarDist = 250;
				sonarDist = 250-sonarDist;
			}
		}
		
		if(measureRequest){
			waterPressureRaw = get_adc(3);
			int32_t w = waterPressureRaw;
			int32_t delta = w - filteredPressure;
			df1 += delta;
			int32_t temp = df1 / KFI1;
			filteredPressure += temp;
			df1 -= temp * KFI1;
			waterPressure = (uint16_t)filteredPressure;
			measureRequest = 0;
			
			twi0_read_noReg(I2CEXPANDERADDR, &i2cRxData, sizeof(i2cRxData));
			i2cRxData[1] = ~i2cRxData[1];
			i2cRxData[0] = ~i2cRxData[0];
			
			
			if(ISSET(MID_LEVEL)){
				lockPump = UNLOCK;
			}
			if(!ISSET(LO_LEVEL)){
				lockPump = LOCK;
			}
			
			if(manualRun != 0){
				if(lockPump == LOCK){
					manualRun = 0;
				}else{
					RESET(RED_LAMP);
					SET(GRN_LAMP);
					RESET(YEL_LAMP);
					SET(PUMP);
				}
			}else{
				if(pumpEn){
					if(lockPump == LOCK){
						SET(RED_LAMP);
						RESET(GRN_LAMP);
						RESET(YEL_LAMP);
						RESET(PUMP);
					}else{
							SET(PUMP);
							RESET(RED_LAMP);
							RESET(GRN_LAMP);
							SET(YEL_LAMP);
					}
				}else{
					RESET(RED_LAMP);
					RESET(GRN_LAMP);
					RESET(YEL_LAMP);
					RESET(PUMP);
				}
			}
			if(waterPressure <= pumpLims.pressureLoLim) pumpEn = 1;
			if(waterPressure >= pumpLims.pressureHiLim) pumpEn = 0;
			
			i2cData[0] = ~ outputsState;
			twi0_transfer(I2CEXPANDERADDR, &i2cData, sizeof(i2cData));
		}
    }
}

