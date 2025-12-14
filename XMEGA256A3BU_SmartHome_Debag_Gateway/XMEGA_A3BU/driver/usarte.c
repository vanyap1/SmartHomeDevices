/*
 * usarte.c
 *
 * Created: 25/07/2025 19:17:27
 *  Author: Ivan Prints
 */ 
#include "usarte.h"
uint8_t serialErxByte = 0;
uint16_t serialErxByteNum = 0;
uint8_t rxSerialEBuff[RXE_BUFFER_SIZE];
uint8_t serialE_dataAvailable = 0;
//PORTE2 - TX
//PORTE3 - RX


ISR(USARTE0_RXC_vect) {
	serialErxByte = USARTE0.DATA;
	if(serialErxByte == '\r'){
		serialE_dataAvailable = 1;
		rxSerialEBuff[serialErxByteNum] = '\0';
	}else{
		if (serialErxByteNum < RXE_BUFFER_SIZE - 1) {
			if(serialErxByte != '\n') rxSerialEBuff[serialErxByteNum++] = serialErxByte;
		}
	}
}

uint16_t serialEgetRxBytesNum(void){
	return serialErxByteNum;
}

uint8_t serialEDataAvailable(void){
	return serialE_dataAvailable;
}

const char* serialEreadDataBuff(void){
	serialErxByteNum = 0;
	serialE_dataAvailable = 0;
	return rxSerialEBuff;
}
void serialEWriteString(uint8_t *c){
	uint16_t i = 0;
	do{
		serialEWriteChar(c[i]);
		i++;
		
	}while(c[i] != '\0');
	//uart_send_byte(c[i]);
}

uint8_t serialEInit(uint32_t baud) {
	int8_t   exp;
	uint32_t div;
	uint32_t limit;
	uint32_t ratio;
	uint32_t min_rate;
	uint32_t max_rate;

	uint32_t cpu_hz = F_CPU;
	max_rate = cpu_hz / 8;
	min_rate = cpu_hz / 4194304;
	max_rate /= 2;
	min_rate /= 2;

	if ((baud > max_rate) || (baud < min_rate)) {
		return -1;
	}

	baud *= 2;

	limit = 0xfffU >> 4;
	ratio = cpu_hz / baud;

	for (exp = -7; exp < 7; exp++) {
		if (ratio < limit) {
			break;
		}

		limit <<= 1;

		if (exp < -3) {
			limit |= 1;
		}
	}

	if (exp < 0) {
		cpu_hz -= 8 * baud;

		if (exp <= -3) {
			div = ((cpu_hz << (-exp - 3)) + baud / 2) / baud;
			} else {
			baud <<= exp + 3;
			div = (cpu_hz + baud / 2) / baud;
		}
		} else {
		baud <<= exp + 3;
		div = (cpu_hz + baud / 2) / baud - 1;
	}

	// ???????????? ???????? ?????
	PORTE.DIRSET = PIN3_bm;  // TX
	PORTE.DIRCLR = PIN2_bm;  // RX

	// ???????????? USART
	USARTE0.BAUDCTRLB = (uint8_t)(((div >> 8) & 0x0F) | (exp << 4));
	USARTE0.BAUDCTRLA = (uint8_t)div;

	USARTE0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc
	| USART_PMODE_DISABLED_gc
	| USART_CHSIZE_8BIT_gc;

	USARTE0.CTRLB = 0 << USART_CLK2X_bp
	| 0 << USART_MPCM_bp
	| 1 << USART_RXEN_bp
	| 1 << USART_TXEN_bp;
	
	USARTE0.CTRLA = USART_RXCINTLVL_MED_gc;
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;

	sei();
	return 0;
}

void serialEWriteChar(uint8_t tmpChar){
	while (!(USARTE0.STATUS & USART_DREIF_bm));
	USARTE0.DATA = tmpChar;
}