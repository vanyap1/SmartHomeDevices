
/*
 * spi_driver.h
 *
 * Created: 02.01.2025 15:58:31
 *  Author: Ivan Prints
 */ 

#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include "gpio_driver.h"

#ifdef __cplusplus
	extern "C" {
	#endif
	

//SPIC initdefs
#define SPIC_CLK2X	7
#define SPIC_ENABLE	6
#define SPIC_DORD	5
#define SPIC_MASTER	4
#define SPIC_MODE	2
#define SPIC_PRESCALLER	0

#define DUMMY_BYTE 0x00

uint8_t initSpic(void);
	
void spicTransfer(uint8_t *data, uint16_t len);
void spicTxBuff(uint8_t *tx,uint16_t len);
void spicRxBuff(uint8_t *tx,uint16_t len);
uint8_t spicRxByte(void);
void spicTxByte(uint8_t byte);
uint8_t spicRxTXByte(uint8_t byte);

void CAN_Select(void);
void CAN_Unselect(void);
uint8_t CAN_GetIntPin(void);

void SD_Select(void);
void SD_Unselect(void);

void W5500_Select(void);
void W5500_Unselect(void);
void W5500_ReadBuff(uint8_t* buff, uint16_t len);
void W5500_WriteBuff(uint8_t* buff, uint16_t len);
uint8_t W5500_ReadByte(void);
void W5500_Write_byte(uint8_t byte);

	
#ifdef __cplusplus
}
#endif
	
#endif	
	
