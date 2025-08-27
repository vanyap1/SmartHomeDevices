
/*
 * mem25lXX.h
 *
 * Created: 13.12.2023 22:06:47
 *  Author: Vanya
 */ 
#include <stdint.h>


#ifndef SPI_MEM
#define SPI_MEM

#define WREN			0x06			// write enable
#define WRDI			0x04			// write disable
#define WRSR			0x01			// write status register
#define RDID			0x9f			// read identification (2 byte [Manufacturer ID, Device ID])
#define RDSR			0x05			// read status register
#define READ			0x03			// Rad data (24 bit page sector)
#define FASTREAD		0x0b			// Rad data (24 bit page sector + n bytes)

#define RDSFDP			0x5a			// read SFDP mode (24 bit page address)
#define RES				0xAB			// read electronic ID
#define REMS			0x90			// read identification (2 byte [Manufacturer ID, Device ID]) (see NOTE1) 
#define DREAD			0x3b			// double mode command
#define SE				0x20			// sector erase (24 bit page address)
#define BE				0x52 //or 0xd8	// block erase (24 bit page address)
#define CE				0x5a			// to erase whole chip

#define PP				0x02			// page program (24 bit page address)
#define RDSCUR			0x2b			// read security register
#define WRSCUR			0x2f			// write security register
#define ENSO			0xb1			// enter secure OTP
#define EXSO			0xc1			// exit secure OTP
#define DP				0xb9			// deep power down
#define RDP				0xab			// release deep power down

/*
 Note 1: ADD=00H will output the manufacturer ID first and ADD=01H will output device ID first.
 Note 2: It is not recommended to adopt any other code not in the command definition table, which will potentially
 enter the hidden mode.
 */ 


typedef struct ramInfoStruc {
	uint8_t manufacturerID;
	uint8_t memoryType;
	uint8_t memoryDensity;			
	uint32_t electronicID;			
	uint8_t	deviceID;
} ramIDS;


uint8_t ramGetIDS(ramIDS * idData);
uint8_t ramReadPage(uint8_t * dataBuff, uint32_t pageAddress, uint16_t dataLen);
uint8_t ramErasePage(uint32_t pageAddress);

uint8_t writeRAMCmd(uint8_t cmd);
uint8_t readRAMReg(uint8_t addr);
void spiRamSelect();
void spiRamRelease();





#endif