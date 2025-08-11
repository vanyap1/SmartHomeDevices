/*
 * spi0_hall.h
 *bb
 * Created: 18.02.2024 21:13:06
 *  Author: Vanya
 */ 
#ifndef SPI0_HAL_H_
#define SPI0_HAL_H_


#include <stdint.h>
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//void spi_master_init(void);
//void spi_tranfer(uint8_t *tx,uint8_t *rx,uint16_t len);


void spi0_init();
//void soft_spi_write_byte(uint8_t data);



void spi0_write_byte(uint8_t data);
uint8_t spi0_read_byte(void);

void spi0_tranfer(uint8_t *tx,uint16_t len);
void spi0TxBuff(uint8_t *tx,uint16_t len);
void spi0RxBuff(uint8_t *tx,uint16_t len);
void spi0_send(uint8_t *tx);


#endif /* SPI1_HAL_H_ */




//void soft_spi_write_byte(uint8_t data);




