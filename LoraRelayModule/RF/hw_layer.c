/*
 * hw_layer.c
 *bb
 * Created: 18.02.2024 21:28:18
 *  Author: Vanya
 */ 
#include "RFM69.h"
#include "hw_layer.h"
#include "gpio_driver.h"
#include <util/delay.h>

uint8_t rfIntEvent = 0;

gpio rfINT = {(uint8_t *)&PORTD , PORTD2};
gpio rfSelect = {(uint8_t *)&PORTD , PORTD4};
gpio rfReset = {(uint8_t *)&PORTE , PORTE0};	

gpio rfMISO = {(uint8_t *)&PORTB , PORTB4};	
gpio rfMOSI = {(uint8_t *)&PORTB , PORTB3};	
gpio rfClk = {(uint8_t *)&PORTB , PORTB5};	

rfHeader* ptr_rfHeader;

rfHeader* rfMsgType(void) {
	return ptr_rfHeader;
}


ISR(INT0_vect){
	rfIntEvent++;
}



void HwInitIO(){
	//gpio_set_pin_level(&rfMISO, true);
	gpio_set_pin_direction(&rfSelect , PORT_DIR_IN);
	
	//gpio_set_pin_level(&rfSelect, true);
	gpio_set_pin_direction(&rfSelect , PORT_DIR_OUT);
	
	//gpio_set_pin_level(&rfReset, true);
	gpio_set_pin_direction(&rfReset , PORT_DIR_OUT);
	
	
	gpio_set_pin_direction(&rfINT , PORT_DIR_IN);
	
	EICRA |= (0b11 << ISC00);
	EIMSK |= (0x01 << INT0); //0b00000001
	RF_HW_Reset();
	spi0_init();
	
}

void RF_HW_Reset(){
	
	gpio_set_pin_level(&rfReset, true);
	_delay_us(200);
	gpio_set_pin_level(&rfReset, false);
	_delay_ms(6);
}


void RFM_69_sel(uint8_t newState){
	if(newState)
	{
		gpio_set_pin_level(&rfSelect, true);
	}else{
		gpio_set_pin_level(&rfSelect, false);
	}
	
}


void SPI_write(uint8_t data){
	spi0_write_byte(data);
}

uint8_t SPI_read(void){
	return spi0_read_byte();
}

void RFM69_WriteBuff(uint8_t* buff, uint16_t len){
	spi0TxBuff(buff, len);
}

void RFM69_ReadBuff(uint8_t* buff, uint16_t len){
	spi0RxBuff(buff, len);
}


bool rf_isReady(){
	if (rfIntEvent != 0){
		ptr_rfHeader = data_ready();
		rfIntEvent = 0;
		return (ptr_rfHeader->dataValid == 0 ) ? false : true;
	}
	return false;
}