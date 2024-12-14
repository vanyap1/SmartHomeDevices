
/*
 * mem25lXX.h
 *
 * Created: 13.12.2023 22:06:47
 *  Author: Vanya
 */ 
#include <stdint.h>
#include <stdbool.h>

#ifndef SPI_MEM
#define SPI_MEM

#define SPI_WREN			0x06			// write enable
#define SPI_WRDI			0x04			// write disable
#define SPI_WRSR			0x01			// write status register
#define SPI_RDID			0x9f			// read identification (2 byte [Manufacturer ID, Device ID])
#define SPI_RDSR			0x05			// read status register
#define SPI_READ			0x03			// Rad data (24 bit page sector)
#define SPI_FASTREAD		0x0b			// Rad data (24 bit page sector + n bytes)

#define SPI_RDSFDP			0x5a			// read SFDP mode (24 bit page address)
#define SPI_RES				0xAB			// read electronic ID
#define SPI_REMS			0x90			// read identification (2 byte [Manufacturer ID, Device ID]) (see NOTE1) 
#define SPI_DREAD			0x3b			// double mode command
#define SPI_SE				0x20			// sector erase (24 bit page address)	Sector Erase (20h) 4K-bytes	
#define SPI_BE32			0x52			// block erase (24 bit page address)	32KB Block Erase (52h) 32K-bytes
#define SPI_BE64			0xd8			// block erase (24 bit page address)	64KB Block Erase (D8h) 64K-bytes
#define SPI_CE				0xc7 //0x60		// to erase whole chip
#define SPI_ES				0x75			// Erase Suspend (75h)
#define SPI_ESR				0x7a			// Erase Suspend (75h)

#define SPI_PP				0x02			// page program (24 bit page address)
#define SPI_RDSCUR			0x2b			// read security register
#define SPI_WRSCUR			0x2f			// write security register
#define SPI_ENSO			0xb1			// enter secure OTP
#define SPI_EXSO			0xc1			// exit secure OTP
#define SPI_DP				0xb9			// deep power down
#define SPI_RDP				0xab			// release deep power down

#define SPI_POWERDOWN_SET		1
#define SPI_POWERDOWN_RESET		0
/*
 Note 1: ADD=00H will output the manufacturer ID first and ADD=01H will output device ID first.
 Note 2: It is not recommended to adopt any other code not in the command definition table, which will potentially
 enter the hidden mode.
 */ 



typedef struct {
	uint8_t manufacturerID;
	uint8_t memoryType;
	uint8_t memoryDensity;			
	uint32_t electronicID;			
	uint8_t	deviceID;
} ramIDS;


uint8_t chipGetIDS(ramIDS *idData);
uint8_t chipReadPage(uint32_t pageAddress, uint8_t *dataBuff,  uint16_t dataLen);	//10.2.8 Read Data (03h)

uint8_t chipWritePage(uint32_t pageAddress, uint8_t *dataBuff, uint16_t dataLen);	//10.2.14 Page Program (02h) 256-bytes
uint8_t chipEraseSector(uint32_t sectorAddress);									//10.2.16 Sector Erase (20h) 4K-bytes
uint8_t chipEraseBlock32(uint32_t blockAddress);									//10.2.17 32KB Block Erase (52h) 32K-bytes
uint8_t chipEraseBlock64(uint32_t blockAddress);									//10.2.18 64KB Block Erase (D8h) 64K-bytes
uint8_t chipErase(void);															//10.2.19 Chip Erase (C7h / 60h) 
uint8_t chipEraseSuspend(void);														//10.2.20 Erase Suspend (75h)
uint8_t chipEraseResume(void);														//10.2.21 Erase Resume (7Ah)  
uint8_t chipPowerDown(uint8_t newStatus);											//10.2.22 Power-down (B9h) 


uint8_t writeRAMCmd(uint8_t cmd);
uint8_t readRAMReg(uint8_t addr);
void spiRamSelect();
void spiRamRelease();





#endif