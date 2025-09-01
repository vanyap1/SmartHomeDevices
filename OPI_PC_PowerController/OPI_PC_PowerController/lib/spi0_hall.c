/*
 * spi0_hall.c
 *bb
 * Created: 18.02.2024 21:12:57
 *  Author: Vanya
 */ 
#include "spi0_hall.h"
#include "gpio_driver.h"
#include <util/delay.h>

volatile uint8_t spi0_txrx_done = 0;

gpio spi0_tx_pin = {(uint8_t *)&PORTB , PORTB3};
gpio spi0_rx_pin = {(uint8_t *)&PORTB , PORTB4};
gpio spi0_clk_pin = {(uint8_t *)&PORTB , PORTB5};
//gpio spi0_ss_pin = {(uint8_t *)&PORTE , PORTE2};
gpio canEn = {(uint8_t *)&PORTD , PORTD4};

	
ISR(SPI0_STC_vect){
	//if(SPSR0 & 0x40){
		////spi_rx_data = 0;
		spi0_txrx_done = 1;
		
		//}else{
		//spi0_txrx_done = 1;
	//}
}

void spi0_init(){
	gpio_set_pin_level(&spi0_tx_pin, true);
	gpio_set_pin_direction(&spi0_tx_pin , PORT_DIR_OUT);
	gpio_set_pin_level(&spi0_rx_pin, true);
	
	
	//gpio_set_pin_direction(&spi0_ss_pin , PORT_DIR_OUT);
	//gpio_set_pin_level(&spi0_ss_pin, true);
	
	gpio_set_pin_level(&spi0_clk_pin, true);
	gpio_set_pin_direction(&spi0_clk_pin , PORT_DIR_OUT);
	
	
	gpio_set_pin_level(&canEn, true);
	gpio_set_pin_direction(&canEn , PORT_DIR_OUT);
	//DDRC |= (1 << DDC1);
	//DDRE |= (1 << DDE3);
	SPCR0 |= (1 << SPIE) | (1 << SPE) | (0 << DORD) | (1 << MSTR)  | (0 << CPOL) | (0 << CPHA) | (0b00 << SPR0);
	//SPCR0 |= (0 << MSTR);
	
	
	//Bit 7 – SPIE1: SPI1 Interrupt Enable
	//Bit 6 – SPE1: SPI1 Enable
	//Bit 5 – DORD1: Data1 Order
	//Bit 4 – MSTR1: Master/Slave1 Select
	//Bit 3 – CPOL1: Clock1 Polarity
	//Bit 2 – CPHA1: Clock1 Phase
	//Bits 1:0 – SPR1n: SPI1 Clock Rate Select n [n = 1:0]
	//
	
	SPSR0 |= (1 << SPI2X);
}


void CAN_Select(void){
	gpio_set_pin_level(&canEn, false);
	_delay_us(10);
}
void CAN_Unselect(void){
	_delay_us(10);
	gpio_set_pin_level(&canEn, true);
}

void spi0_write_byte(uint8_t data){
	spi0_txrx_done = 0;
	SPDR0 = data;
	while(spi0_txrx_done == 0);
}

uint8_t spi0_read_byte(void){
	spi0_write_byte(0);
	return SPDR0;
}

void spi0_tranfer(uint8_t *tx,uint16_t len){
	for(uint16_t i=0; i < len;i++){
		spi0_send(&tx[i]);
	}
}

void spi0TxBuff(uint8_t *tx,uint16_t len){
	for(uint16_t i=0; i < len;i++){
		spi0_send(&tx[i]);
	}
}


void spi0RxBuff(uint8_t *tx,uint16_t len){
	for(uint16_t i=0; i < len;i++){
		tx[i] = spi0_read_byte();
	}
}

void spi0_send(uint8_t *tx){
	spi0_txrx_done = 0;
	SPDR0 = *tx;
	while(spi0_txrx_done == 0);
	
}