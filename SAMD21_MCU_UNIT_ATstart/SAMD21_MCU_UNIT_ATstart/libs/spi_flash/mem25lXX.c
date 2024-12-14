
/*
 * mem25lXX.c
 *
 * Created: 13.12.2023 22:06:39
 *  Author: Vanya
 */ 
#include "mem25lXX.h"


uint8_t chipGetIDS(ramIDS * idData){
	
	uint8_t ramData[3];
	// get and parse RDID data
	spiRamSelect();
	readRAMReg(SPI_RDID);
	SPI_ReadBuff(&ramData, sizeof(ramData));
	idData->memoryType = ramData[0];
	idData->memoryDensity=ramData[1];
	idData->manufacturerID = ramData[2];
	
	spiRamRelease();
	// get and parse RES data
	spiRamSelect();
	readRAMReg(SPI_RES);
	SPI_ReadBuff(&ramData, sizeof(ramData));
	idData->electronicID |= (ramData[0] << 16);
	idData->electronicID |= (ramData[1] << 8);
	idData->electronicID |= (ramData[2]);    
	spiRamRelease();
	// get and parse RES data
	spiRamSelect();
	readRAMReg(SPI_REMS);
	SPI_ReadBuff(&ramData, sizeof(ramData));
	idData->deviceID = ramData[1];
	spiRamRelease();
	return (idData->manufacturerID == 0xc2) ? true : false;
}


uint8_t chipReadPage(uint32_t pageAddress, uint8_t * dataBuff, uint16_t dataLen){
	uint8_t opCodeAndAddress[4] = {SPI_READ, (pageAddress >> 16) & 0xFF, (pageAddress >> 8) & 0xFF, pageAddress & 0xFF};
	spiRamSelect();
	SPI_WriteBuff(&opCodeAndAddress, sizeof(opCodeAndAddress));
	SPI_ReadBuff(dataBuff, dataLen);
	spiRamRelease();
	return true;
}

uint8_t chipWritePage(uint32_t pageAddress, uint8_t *dataBuff, uint16_t dataLen)	//10.2.14 Page Program (02h) 256-bytes
{
	return 0;
}
uint8_t chipEraseSector(uint32_t sectorAddress)										//10.2.16 Sector Erase (20h) 4K-bytes
{
	uint8_t opCodeAndAddress[4] = {SPI_SE, (sectorAddress >> 16) & 0xFF, (sectorAddress >> 8) & 0xFF, sectorAddress & 0xFF};
	//Unlock memory to write
	spiRamSelect();
	writeRAMCmd(SPI_WREN);
	spiRamRelease();
	//Erase page memory by page address
	spiRamSelect();
	SPI_WriteBuff(&opCodeAndAddress, sizeof(opCodeAndAddress));
	spiRamRelease();
	//Erase check if operation complete
	uint8_t eraseStatus = 0xff;
	while (eraseStatus & 0x03) {
		spiRamSelect();
		eraseStatus = readRAMReg(SPI_RDSR);
		spiRamRelease();
	}
	return true;
}
uint8_t chipEraseBlock32(uint32_t blockAddress)										//10.2.17 32KB Block Erase (52h) 32K-bytes
{
	uint8_t opCodeAndAddress[4] = {SPI_BE32, (blockAddress >> 16) & 0xFF, (blockAddress >> 8) & 0xFF, blockAddress & 0xFF};
	//Unlock memory to write
	spiRamSelect();
	writeRAMCmd(SPI_WREN);
	spiRamRelease();
	//Erase page memory by page address
	spiRamSelect();
	SPI_WriteBuff(&opCodeAndAddress, sizeof(opCodeAndAddress));
	spiRamRelease();
	//Erase check if operation complete
	uint8_t eraseStatus = 0xff;
	while (eraseStatus & 0x03) {
		spiRamSelect();
		eraseStatus = readRAMReg(SPI_RDSR);
		spiRamRelease();
	}
	return true;
}
uint8_t chipEraseBlock64(uint32_t blockAddress)										//10.2.18 32KB Block Erase (D8h) 64K-bytes
{
	uint8_t opCodeAndAddress[4] = {SPI_BE64, (blockAddress >> 16) & 0xFF, (blockAddress >> 8) & 0xFF, blockAddress & 0xFF};
	//Unlock memory to write
	spiRamSelect();
	writeRAMCmd(SPI_WREN);
	spiRamRelease();
	//Erase page memory by page address
	spiRamSelect();
	SPI_WriteBuff(&opCodeAndAddress, sizeof(opCodeAndAddress));
	spiRamRelease();
	//Erase check if operation complete
	uint8_t eraseStatus = 0xff;
	while (eraseStatus & 0x03) {
		spiRamSelect();
		eraseStatus = readRAMReg(SPI_RDSR);
		spiRamRelease();
	}
	return true;
}
uint8_t chipErase(void)																//10.2.19 Chip Erase (C7h / 60h)
{
	uint8_t opCodeAndAddress[1] = {SPI_CE};
	//Unlock memory to write
	spiRamSelect();
	writeRAMCmd(SPI_WREN);
	spiRamRelease();
	//Erase page memory by page address
	spiRamSelect();
	SPI_WriteBuff(&opCodeAndAddress, sizeof(opCodeAndAddress));
	spiRamRelease();
	//Erase check if operation complete
	uint8_t eraseStatus = 0xff;
	while (eraseStatus & 0x03) {
		spiRamSelect();
		eraseStatus = readRAMReg(SPI_RDSR);
		spiRamRelease();
	}
	return true;
}
uint8_t chipEraseSuspend(void)														//10.2.20 Erase Suspend (75h)
{
	uint8_t opCodeAndAddress[1] = {SPI_ES};
	spiRamSelect();
	SPI_WriteBuff(&opCodeAndAddress, sizeof(opCodeAndAddress));
	spiRamRelease();
	return 1;
}
uint8_t chipEraseResume(void)														//10.2.21 Erase Resume (7Ah)
{
	uint8_t opCodeAndAddress[1] = {SPI_ESR};
	spiRamSelect();
	SPI_WriteBuff(&opCodeAndAddress, sizeof(opCodeAndAddress));
	spiRamRelease();
	return 1;
}
uint8_t chipPowerDown(uint8_t newStatus)											//10.2.22 Power-down (B9h)
{
	if(newStatus == SPI_POWERDOWN_SET){
		uint8_t opCodeAndAddress[1] = {SPI_DP};
		spiRamSelect();
		SPI_WriteBuff(&opCodeAndAddress, sizeof(opCodeAndAddress));
		spiRamRelease();
		return true;
	}
	uint8_t ramData[3];
	//Device ID instruction, which restores the device to normal operation
	spiRamSelect();
	readRAMReg(SPI_RDID);
	SPI_ReadBuff(&ramData, sizeof(ramData));
	spiRamRelease();
	return true; 
	
}




uint8_t writeRAMCmd(uint8_t cmd){
	SPI_write(cmd);
	return true;
}
uint8_t readRAMReg(uint8_t addr){
	SPI_write(addr);
	uint8_t regval = SPI_read();
	return regval;
}


void spiRamSelect(){
	SPI_RAM_sel(false);
}

void spiRamRelease(){
	SPI_RAM_sel(true);
}