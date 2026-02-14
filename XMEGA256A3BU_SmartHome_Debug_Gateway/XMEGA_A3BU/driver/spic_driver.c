
/*
 * spi_driver.c
 *
 * Created: 02.01.2025 15:58:21
 *  Author: Ivan Prints
 */ 

#include "spic_driver.h"



gpio SPI_RST = {(uint8_t *)&PORTC , PIN0_bp};
gpio CAN_CS = {(uint8_t *)&PORTC , PIN1_bp};
gpio CAN_INT = {(uint8_t *)&PORTC , PIN2_bp};
gpio CAN_TERM = {(uint8_t *)&PORTF , PIN7_bp};
gpio ETH_INT = {(uint8_t *)&PORTC , PIN3_bp};
gpio ETH_CS = {(uint8_t *)&PORTC , PIN4_bp};

gpio SD_CS = {(uint8_t *)&PORTD , PIN3_bp};
gpio SD_DET = {(uint8_t *)&PORTD , PIN2_bp};
gpio LCD_BLK = {(uint8_t *)&PORTD , PIN1_bp};
gpio RTC_INT = {(uint8_t *)&PORTD , PIN0_bp};
	
	


uint8_t initSpic(void){
	PORTC.DIRSET = PIN4_bm | PIN5_bm | PIN7_bm; // SCK, MOSI, SS - ?? ??????
	PORTC.DIRCLR = PIN6_bm;                     // MISO - ?? ????
	
	SPIC.CTRL = (0 << SPIC_CLK2X) | (1 << SPIC_ENABLE) | (0 << SPIC_DORD) | (1 << SPIC_MASTER) | (0b00 << SPIC_MODE) | (0b00 << SPIC_PRESCALLER);
	//SPIC.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_0_gc | SPI_PRESCALER_DIV4_gc;
	
	gpio_set_pin_direction(&SPI_RST , PORT_DIR_OUT);
	gpio_set_pin_direction(&CAN_CS , PORT_DIR_OUT);
	gpio_set_pin_direction(&ETH_CS , PORT_DIR_OUT);
	gpio_set_pin_direction(&SD_CS , PORT_DIR_OUT);
	gpio_set_pin_direction(&LCD_BLK , PORT_DIR_OUT);
	gpio_set_pin_direction(&CAN_TERM , PORT_DIR_OUT);
	
	gpio_set_pin_direction(&CAN_INT , PORT_DIR_IN);
	gpio_set_pin_direction(&ETH_INT , PORT_DIR_IN);
	gpio_set_pin_direction(&RTC_INT , PORT_DIR_IN);
	gpio_set_pin_direction(&SD_DET , PORT_DIR_IN);
	
	gpio_set_pin_level(&SPI_RST, false);
	_delay_ms(50);
	
	gpio_set_pin_level(&CAN_CS, true);
	gpio_set_pin_level(&ETH_CS, true);
	gpio_set_pin_level(&SD_CS, true);
	
	gpio_set_pin_level(&SPI_RST, true);
	return 1;
}

void spicTransfer(uint8_t *data, uint16_t len) {
	for (uint16_t i = 0; i < len; i++) {
		SPIC.DATA = data[i];               
		while (!(SPIC.STATUS & SPI_IF_bm)); 
		data[i] = SPIC.DATA;              
	}
}

void spicTxBuff(uint8_t *tx,uint16_t len){
	for(uint16_t i=0; i < len;i++){
		SPIC.DATA = tx[i];
		while (!(SPIC.STATUS & SPI_IF_bm));
	}
}

void spicRxBuff(uint8_t *tx,uint16_t len){
	for(uint16_t i=0; i < len;i++){
		SPIC.DATA = DUMMY_BYTE;
		while (!(SPIC.STATUS & SPI_IF_bm));
		tx[i] = SPIC.DATA;
	}
}

uint8_t spicRxByte(void){
	SPIC.DATA = DUMMY_BYTE;
	while (!(SPIC.STATUS & SPI_IF_bm));
	return SPIC.DATA;
}

void spicTxByte(uint8_t byte){
	SPIC.DATA = byte;
	while (!(SPIC.STATUS & SPI_IF_bm));
}

uint8_t spicRxTXByte(uint8_t byte){
	SPIC.DATA = byte;
	while (!(SPIC.STATUS & SPI_IF_bm));
	return SPIC.DATA;
}


void CAN_Select(void){
	gpio_set_pin_level(&CAN_CS, false);
}
void CAN_Unselect(void){
	gpio_set_pin_level(&CAN_CS, true);
}

uint8_t CAN_GetIntPin(void){
	return gpio_get_pin_level(&CAN_INT);
}

void SD_Select(void){
	gpio_set_pin_level(&SD_CS, false);
}
void SD_Unselect(void){
	gpio_set_pin_level(&SD_CS, true);
}

void W5500_Select(void){
	gpio_set_pin_level(&ETH_CS, false);
}
void W5500_Unselect(void){
	gpio_set_pin_level(&ETH_CS, true);
}
void W5500_ReadBuff(uint8_t* buff, uint16_t len){
	spicRxBuff(buff, len);
}
void W5500_WriteBuff(uint8_t* buff, uint16_t len){
	spicTxBuff(buff, len);
}
uint8_t W5500_ReadByte(void){
	return spicRxByte();
}
void W5500_Write_byte(uint8_t byte){
	spicTxByte(byte);
}