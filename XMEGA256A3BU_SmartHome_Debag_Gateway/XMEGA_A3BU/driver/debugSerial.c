
/*
 * debugSerial.c
 *
 * Created: 03.01.2025 10:16:26
 *  Author: Ivan Prints
 */ 
#include "debugSerial.h"
uint8_t receivedByte = 10;

//PORTF2 - TX
//PORTF3 - RX
static FILE debugSerialStream = FDEV_SETUP_STREAM(debugSerialPutChar, NULL, _FDEV_SETUP_WRITE);

ISR(USARTF0_RXC_vect) {
	receivedByte = USARTF0.DATA;
}

uint8_t getRxByte(void){
	return receivedByte;
}

uint16_t debugSerialPutChar(char c, FILE *stream) {
	if (c == '\n') {
		debugSerialWriteChar('\r');
	}
	debugSerialWriteChar(c);
	return 0;
}

uint8_t debugSerialInit(uint32_t baud) {
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
	PORTF.DIRSET = PIN3_bm;  // TX
	PORTF.DIRCLR = PIN2_bm;  // RX

	// ???????????? USART
	USARTF0.BAUDCTRLB = (uint8_t)(((div >> 8) & 0x0F) | (exp << 4));
	USARTF0.BAUDCTRLA = (uint8_t)div;

	USARTF0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc
	| USART_PMODE_DISABLED_gc
	| USART_CHSIZE_8BIT_gc;

	USARTF0.CTRLB = 0 << USART_CLK2X_bp
	| 0 << USART_MPCM_bp
	| 1 << USART_RXEN_bp
	| 1 << USART_TXEN_bp;
	
	USARTF0.CTRLA = USART_RXCINTLVL_MED_gc;
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;

	sei();
	stdout = &debugSerialStream;
	return 0;
}

void debugSerialWriteChar(uint8_t tmpChar){
	while (!(USARTF0.STATUS & USART_DREIF_bm)); 
	USARTF0.DATA = tmpChar;
}

//void sendString(char *text)
//{
	//while(*text)
	//{
		//sendChar(*text++);
	//}
//}