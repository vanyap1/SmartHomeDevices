/*
 * LoraRelayModule.c
 *ff
 * Created: 01.08.2024 21:53:58
 * Author : Vanya
 */ 
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
#include "twi_hal1.h"
#include "spi0_hall.h"
#include "adc_hal.h"
#include "eeprom.h"
#include "hw_layer.h"
#include "RFM69.h"
#include "RFM69registers.h"
#include "lora_custom_protocol.h"
#include "dto.h"
#include "stdint.h"
#include <stdio.h>
static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);


#define LEDBLUE		OCR2B
#define LEDRED		OCR0B
#define LEDGRN		OCR0A

#define RADIO_MODE	TX_UNMUTE

powerData mainBattery;



gpio debugOut = {(uint8_t *)&PORTB , PORTB1};
gpio debugOut2 = {(uint8_t *)&PORTB , PORTB2};
gpio chrgerStdby = {(uint8_t *)&PORTB , PORTB0};
gpio chrgerChrg = {(uint8_t *)&PORTD , PORTD7};
gpio ledBluePin = {(uint8_t *)&PORTD , PORTD3};
gpio ledRedPin = {(uint8_t *)&PORTD , PORTD5};
gpio ledGrnPin = {(uint8_t *)&PORTD , PORTD6};

gpio relay = {(uint8_t *)&PORTE , PORTE1};
	
nodeInfo myNode;	
nodeNetId myNodeId;





uint32_t sysTick = 0;
uint8_t rtc_int_request = 0;
uint16_t BAT_VOLT = 0;

uint8_t txLen;
uint8_t txCRC;
rfHeader rfTxDataPack;

uint8_t ldPWMB, ldPWMG, ldPWMR = 0;
uint8_t measureRequest = 0;

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
	//gpio_toggle_pin_level(&debugOut);
	measureRequest = 1;
	sysTick++;
	//printf("int/n/r");
}

int main(void)
{
	sei();
	wdt_enable(WDTO_2S);
	
	HwInitIO();
	rfm69_init(868, NODEID, NETWORKID);
	rcCalibration();
	setHighPower(true);
	
	//myNodeId.netId = NETWORKID;
	//myNodeId.nodId = 0x21;
	//strcpy(myNodeId.nodSerial, "ao1340cm01");
	//EEPROM_update_batch(1, &myNodeId, sizeof(myNodeId));
	
	
	EEPROM_read_batch(1, &myNodeId, sizeof(myNodeId));
	
	
	
	
    char char_array[128]="test_data\0";
    char txData[128];
    uart_init(250000,1);
    //twi0_init(400000);
    
    stdout = &mystdout;
   
   
    adc_init();
    
    gpio_set_pin_direction(&ledBluePin , PORT_DIR_OUT); gpio_set_pin_level(&ledBluePin, false);
	gpio_set_pin_direction(&ledRedPin , PORT_DIR_OUT); gpio_set_pin_level(&ledRedPin, true);
	gpio_set_pin_direction(&ledGrnPin , PORT_DIR_OUT); gpio_set_pin_level(&ledGrnPin, true);
	
	gpio_set_pin_direction(&relay , PORT_DIR_OUT); gpio_set_pin_level(&relay, false);
	gpio_set_pin_direction(&debugOut , PORT_DIR_OUT); gpio_set_pin_level(&debugOut, false);
	gpio_set_pin_direction(&debugOut2 , PORT_DIR_OUT); gpio_set_pin_level(&debugOut2, false);



    
	gpio_set_pin_direction(&chrgerStdby , PORT_DIR_IN);
	gpio_set_pin_direction(&chrgerChrg , PORT_DIR_IN);

	//Setup timer 1, CH B for LEDBLUE
	TCCR2A = (0b11 << COM2B0) | (0b01 << WGM20); //inverted pwm
	TCCR2B = (0b0 << WGM22) | (0b1 << CS20);
	
	//Setup timer 0, CH A and B for LEDRED and LEDGRN
	TCCR0A = (0b11 << COM0A0) | (0b11 << COM0B0) | (0b01 << WGM00); //inverted pwm
	TCCR0B = (0b0 << WGM02) | (0b1 << CS10);
	
	
	 TCCR1B |= (1 << WGM12 );   // Configure timer 1 for CTC mode
	 OCR1A = 31250;             // Set CTC compare value to 10Hz (100mS)
	 // at 8MHz AVR clock, with a prescaler of 64
	 TIMSK1 |= (1 << OCIE1A );  // Enable CTC interrupt
	 TCCR1B |= (1 << CS12 );// | (1 << CS11 ));
	
	
    //sleep_enable();
    //set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    
    
    printf("RUN\n\r");
	
	
	
	
    while (1) 
    {
		wdt_reset();
		_delay_ms(1);
		
		if(ldPWMB){
			ldPWMB--;
			LEDBLUE = ldPWMB;
		}
		if(ldPWMG){
			ldPWMG--;
			LEDGRN = ldPWMG;
		}
		if(ldPWMR){
			ldPWMR--;
			LEDRED = ldPWMR;
		}
		
		if(!gpio_get_pin_level(&chrgerChrg)){
			//Charge in process
			ldPWMR = 4;
		}
		if(!gpio_get_pin_level(&chrgerStdby)){
			//Battery is full and AC present
			ldPWMG = 4;
		}
		
		if(measureRequest){
			myNode.nodeTemperature = getNTC(ADCNTC);
			myNode.nodeVccVoltage = get_mVolt(ADCVCC);
			myNode.nodeBatVoltage = get_mVolt(ADCBAT);
			
			//printf("ID=%X; ntc=%d; vbat=%d; vcc=%d\n\r", 215,  myNode.nodeTemperature, myNode.nodeBatVoltage, myNode.nodeVccVoltage);
			printf("ID=0x%X, NetID=0x%X, Serial: %s; ntc=%d; vbat=%d; vcc=%d\r\n", myNodeId.nodId, myNodeId.netId, myNodeId.nodSerial, myNode.nodeTemperature, myNode.nodeBatVoltage, myNode.nodeVccVoltage);
			//printf("REG RTC %02X %02X %d\n\r", readReg(REG_IRQFLAGS1), readReg(REG_IRQFLAGS2), sysTick);
			
			//if(readReg(REG_IRQFLAGS2) & RF_IRQFLAGS2_FIFONOTEMPTY)
			if(readReg(REG_IRQFLAGS1) == RF_IRQFLAGS1_MODEREADY)
			{
				//clearFIFO();
				rfm69_init(868, NODEID, NETWORKID);
				rcCalibration();
				setHighPower(true);
			}
			
			measureRequest = 0;
		}
		
		
		
		
		
		//gpio_toggle_pin_level(&ledGrnPin);
		
		if (rf_isReady()) {
			rfHeader* rfRxDataMsg=rfMsgType();
			printf("raw;%02X%02X%02X%02X%02X;%03d;\r\n",  rfRxDataMsg->senderAddr, rfRxDataMsg->destinationAddr, rfRxDataMsg->opcode,  rfRxDataMsg->rxtxBuffLenght,  rfRxDataMsg->dataCRC, rfRxDataMsg->rssi);
			
			//for (int i = 0; i < rfRxDataMsg->rxtxBuffLenght; ++i) {
				//printf("%02X", DATA[i]);
			//}
			//printf("\n\r");
			//sprintf((char *)rssiData, "RSSI:%04d;" , rfRxDataMsg->rssi);
			switch(rfRxDataMsg->opcode) {
				case MSG:
					//ldPWMG = 64;
				//memcpy(&testMsg, DATA, sizeof(testMsg));
				break;
				case POWERBANK:
					//ldPWMR = 64;
				//memcpy(&battery, DATA, sizeof(battery));
				break;
				
				case RTC_SYNC:
					//ldPWMB = 64;
				break;
				case MAIN_UPS:
					memcpy(&mainBattery, (void *)DATA, sizeof(mainBattery));
					//printf("MAIN UPS: %2.1fV,%2.1fA,%3.1fW\n\r", (float)mainBattery.voltage/1000, (float)mainBattery.current/1000, mainBattery.power);
					ldPWMB = 128;
				break;
				
				case GPIO_CTRL:
					printf("GPIO CTRL\n\r");
				break;
								
				default:
				break;
				//gpio_toggle_pin_level(LED_G);
			}
		}
		
    }
}

