
/*
 * touchPanel.c
 *
 * Created: 26/09/2025 12:30:16
 *  Author: Ivan Prints
 */ 
#include "touchPanel.h"
#include "gpio_driver.h"
#include "twi_hal1.h"

#define SO_ADDR_U1   0x2D   // ?????? ?????? ??? ??????
#define SO_ADDR_U3   0x2C   // ?????? ???

volatile uint8_t intCounter = 0;
uint8_t ledDataU1[] = {0x00, 0x0e, 0x00, 0x00, 0x00, 0x00};
uint8_t ledDataU3[] = {0x00, 0x0e, 0x00, 0x00};


ISR(INT1_vect){
    intCounter++;
}

/**
 * @brief ????????????? ??????: INT1 + ?????? I²C ??????
 */
uint8_t touchPanelInit(uint8_t addr) {
	// --- INT1 init ---
	EICRA = (2 << ISC10);   // Falling edge ?? INT1
	EIMSK = (1 << INT1);    // ?????????? INT1

	// --- ??????? ????????????? ? BASIC ---
	uint8_t seq1[]  = {0x00, 0x00, 0x00, 0x07};
	twi1_transfer(SO_ADDR_U1, &seq1, sizeof(seq1));

	uint8_t seq2[]  = {0x00, 0x01, 0x01, 0x30};  // &B00_11_0000
	twi1_transfer(SO_ADDR_U1, &seq2, sizeof(seq2));

	uint8_t seq3[]  = {0x00, 0x02, 0x00, 0x81};
	twi1_transfer(SO_ADDR_U1, &seq3, sizeof(seq3));

	uint8_t seq4[]  = {0x00, 0x03, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq4, sizeof(seq4));

	uint8_t seq5[]  = {0x00, 0x04, 0x03, 0xDF};
	twi1_transfer(SO_ADDR_U1, &seq5, sizeof(seq5));

	uint8_t seq6[]  = {0x00, 0x05, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq6, sizeof(seq6));

	uint8_t seq7[]  = {0x00, 0x06, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq7, sizeof(seq7));

	uint8_t seq8[]  = {0x00, 0x07, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq8, sizeof(seq8));

	uint8_t seq9[]  = {0x00, 0x08, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq9, sizeof(seq9));

	uint8_t seq10[] = {0x00, 0x09, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq10, sizeof(seq10));

	uint8_t seq11[] = {0x00, 0x0D, 0x00, 0xF0};
	twi1_transfer(SO_ADDR_U1, &seq11, sizeof(seq11));

	uint8_t seq12[] = {0x00, 0x0E, 0xEF, 0xEF};
	twi1_transfer(SO_ADDR_U1, &seq12, sizeof(seq12));

	uint8_t seq13[] = {0x00, 0x0F, 0x05, 0x05};
	twi1_transfer(SO_ADDR_U1, &seq13, sizeof(seq13));

	uint8_t seq14[] = {0x00, 0x10, 0xCF, 0xC0};
	twi1_transfer(SO_ADDR_U1, &seq14, sizeof(seq14));

	uint8_t seq15[] = {0x00, 0x11, 0xCF, 0xCF};
	twi1_transfer(SO_ADDR_U1, &seq15, sizeof(seq15));

	uint8_t seq16[] = {0x00, 0x12, 0x00, 0xC4};
	twi1_transfer(SO_ADDR_U1, &seq16, sizeof(seq16));

	uint8_t seq17[] = {0x00, 0x13, 0xCD, 0xCE};
	twi1_transfer(SO_ADDR_U1, &seq17, sizeof(seq17));

	uint8_t seq18[] = {0x00, 0x14, 0xCE, 0xCC};
	twi1_transfer(SO_ADDR_U1, &seq18, sizeof(seq18));

	uint8_t seq19[] = {0x00, 0x15, 0xAA, 0xAA};
	twi1_transfer(SO_ADDR_U1, &seq19, sizeof(seq19));

	uint8_t seq20[] = {0x00, 0x16, 0xAA, 0xAA};
	twi1_transfer(SO_ADDR_U1, &seq20, sizeof(seq20));

	uint8_t seq21[] = {0x00, 0x17, 0xAA, 0xAA};
	twi1_transfer(SO_ADDR_U1, &seq21, sizeof(seq21));

	uint8_t seq22[] = {0x00, 0x18, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq22, sizeof(seq22));

	uint8_t seq23[] = {0x00, 0x19, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq23, sizeof(seq23));

	uint8_t seq24[] = {0x00, 0x1E, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq24, sizeof(seq24));

	uint8_t seq25[] = {0x00, 0x22, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq25, sizeof(seq25));

	uint8_t seq26[] = {0x00, 0x23, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq26, sizeof(seq26));

	uint8_t seq27[] = {0x00, 0x24, 0x00, 0x12};
	twi1_transfer(SO_ADDR_U1, &seq27, sizeof(seq27));

	uint8_t seq28[] = {0x00, 0x30, 0x00, 0x00};
	twi1_transfer(SO_ADDR_U1, &seq28, sizeof(seq28));

	uint8_t seq29[] = {0x00, 0x32, 0xFF, 0xFF};
	twi1_transfer(SO_ADDR_U1, &seq29, sizeof(seq29));

	uint8_t res[2];
	twi1_read16(0x2D, 0x00, &res, sizeof(res));
	twi1_read16(0x2D, 0x0109, &res, sizeof(res));

	return 0;
}


/**
 * @brief ???????????? ???? LED 
 */
uint8_t touchPanelSetLedState(uint8_t ledAddr, bool ledState) {
	if(ledAddr <= 7) {
		// ???? 0-7 -> ledDataU1[2]
		if(ledState)
		ledDataU1[2] |= 1 << ledAddr;
		else
		ledDataU1[2] &= ~(1 << ledAddr);
		} else if(ledAddr <= 17) {
		// ???? 8-17 -> ledDataU1[4]
		uint8_t bitIndex = ledAddr - 8;
		if(ledState)
		ledDataU1[4] |= 1 << bitIndex;
		else
		ledDataU1[4] &= ~(1 << bitIndex);
		} else if(ledAddr <= 24) {
		// ???? 18-24 -> ledDataU3[2]
		uint8_t bitIndex = ledAddr - 18;
		if(ledState)
		ledDataU3[2] |= 1 << bitIndex;
		else
		ledDataU3[2] &= ~(1 << bitIndex);
		} else {
		return 1; // ???????????? ledAddr
	}

	// ????????? ????? ?? I2C
	twi1_transfer(SO_ADDR_U1, &ledDataU1, sizeof(ledDataU1));
	_delay_ms(10);
	twi1_transfer(SO_ADDR_U3, &ledDataU3, sizeof(ledDataU3));

	return 0;
}

/**
 * @brief ???????? ????????? ??????????
 */
uint8_t touchPanelIntState(void) {
    return intCounter;
}

/**
 * @brief ?????????? ???? ??????
 */
uint16_t touchPanelGetKeyCode(void) {
	if(intCounter){
		uint8_t res[2];
		twi1_read16(SO_ADDR_U1, 0x0109, &res, sizeof(res));
		intCounter = 0;
		return ((uint16_t)res[0] << 8) | res[1];;
	}
	return 0;
}