/*
 * hw_layer.h
 *bb
 * Created: 18.02.2024 21:28:31
 *  Author: Vanya
 */ 


#include "../config.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include "RFM69.h"

#ifndef HW_LAYER_H_
#define HW_LAYER_H_



#define INVERT_BIT(value, bitNumber) ((value) ^= (1 << (bitNumber)))
#define SET_BIT(value, bitNumber) ((value) |= (1 << (bitNumber)))
#define RESET_BIT(value, bitNumber) ((value) &= ~(1 << (bitNumber)))
#define GET_BIT(value, bitNumber) (((value) >> (bitNumber)) & 0x01)




#define ADCBAT	6
#define ADCVCC	7
#define ADCNTC	0




//uint8_t nodeAdcChannels[3] = {ADCBAT, ADCVCC, ADCNTC};








rfHeader* rfMsgType(void);
void HwInitIO();

void RF_HW_Reset();
void RFM_69_sel(uint8_t newState);


void SPI_write(uint8_t data);
uint8_t SPI_read(void);

void RFM69_WriteBuff(uint8_t* buff, uint16_t len);
void RFM69_ReadBuff(uint8_t* buff, uint16_t len);

bool rf_isReady();


#endif