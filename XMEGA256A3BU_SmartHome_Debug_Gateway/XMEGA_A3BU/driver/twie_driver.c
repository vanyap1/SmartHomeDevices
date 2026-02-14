
/*
 * twie_driver.c
 *
 * Created: 02.01.2025 21:26:02
 *  Author: Ivan Prints
 */ 
#include "twie_driver.h"

uint16_t intCounter;

ISR(PORTD_INT0_vect) {
	intCounter++;
}


uint8_t twieInit(void){
	PORTE.DIRSET = PIN1_bm; 
	PORTE.DIRCLR = PIN0_bm; 
	PORTE.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc; 
	PORTE.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc; 
	TWIE.MASTER.BAUD = (uint8_t)TWIE_BAUD(F_CPU, TWIE_BAUDRATE); 
	TWIE.MASTER.CTRLA = TWI_MASTER_INTLVL_LO_gc     
	| TWI_MASTER_RIEN_bm        
	| TWI_MASTER_WIEN_bm        
	| TWI_MASTER_ENABLE_bm;     
	TWIE.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
	
	return 0;
}

void initTwieInt(void){
	PORTD.DIRCLR = PIN0_bm;
	PORTD.PIN0CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
	PORTD.INTCTRL = PORT_INT0LVL_MED_gc;
	PORTD.INT0MASK = PIN0_bm;
	//PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();
}

uint8_t getTwieInt(void){
	uint16_t tmpVal = intCounter;
	intCounter = 0;
	return (tmpVal > 0xff) ? 0xff : (tmpVal & 0xff);
}

bool twieTxBuff(uint8_t address, uint8_t reg, uint8_t *data, uint8_t data_len) {
	TWIE.MASTER.ADDR = (address << 1) | TWI_WRITE;
	while (!(TWIE.MASTER.STATUS & TWI_MASTER_WIF_bm));
	if (TWIE.MASTER.STATUS & TWI_MASTER_RXACK_bm) {
		TWIE.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		return false;
	}

	TWIE.MASTER.DATA = reg;
	while (!(TWIE.MASTER.STATUS & TWI_MASTER_WIF_bm));
	if (TWIE.MASTER.STATUS & TWI_MASTER_RXACK_bm) {
		TWIE.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		return false;
	}

	for (uint8_t i = 0; i < data_len; i++) {
		TWIE.MASTER.DATA = data[i];
		while (!(TWIE.MASTER.STATUS & TWI_MASTER_WIF_bm));
		if (TWIE.MASTER.STATUS & TWI_MASTER_RXACK_bm) {
			TWIE.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
			return false;
		}
	}
	TWIE.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
	return true;
}


bool twieRxBuff(uint8_t address, uint8_t reg, uint8_t *data, uint8_t data_len) {
	TWIE.MASTER.ADDR = (address << 1) | TWI_WRITE;
	while (!(TWIE.MASTER.STATUS & TWI_MASTER_WIF_bm));
	if (TWIE.MASTER.STATUS & TWI_MASTER_RXACK_bm) {
		TWIE.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		return false;
	}

	TWIE.MASTER.DATA = reg;
	while (!(TWIE.MASTER.STATUS & TWI_MASTER_WIF_bm));
	if (TWIE.MASTER.STATUS & TWI_MASTER_RXACK_bm) {
		TWIE.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		return false;
	}

	TWIE.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
	TWIE.MASTER.ADDR = (address << 1) | TWI_READ;
	while (!(TWIE.MASTER.STATUS & TWI_MASTER_RIF_bm));
	if (TWIE.MASTER.STATUS & TWI_MASTER_RXACK_bm) {
		TWIE.MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		return false;
	}

	for (uint8_t i = 0; i < data_len; i++) {
		while (!(TWIE.MASTER.STATUS & TWI_MASTER_RIF_bm));
		data[i] = TWIE.MASTER.DATA;
		if (i < (data_len - 1)) {
			TWIE.MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
		}
	}
	TWIE.MASTER.CTRLC = TWI_MASTER_ACKACT_bm | TWI_MASTER_CMD_STOP_gc;
	return true;
}




