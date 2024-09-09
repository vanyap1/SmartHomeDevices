
/*
 * mem25lXX.c
 *
 * Created: 13.12.2023 22:06:39
 *  Author: Vanya
 */ 

#include "driver.h"
#include "mem25lXX.h"




uint8_t ramGetIDS(ramIDS * idData){
	
	uint8_t ramData[3];
	// get and parse RDID data
	spiRamSelect();
	readRAMReg(RDID);
	SPI_RAM_ReadBuff(&ramData, sizeof(ramData));
	idData->manufacturerID = ramData[2];
	idData->memoryType = ramData[0];
	idData->memoryDensity=ramData[1];
	spiRamRelease();
	// get and parse RES data
	spiRamSelect();
	readRAMReg(RES);
	SPI_RAM_ReadBuff(&ramData, sizeof(ramData));
	idData->electronicID |= (ramData[0] << 16);
	idData->electronicID |= (ramData[1] << 8);
	idData->electronicID |= (ramData[2]);    
	spiRamRelease();
	// get and parse RES data
	spiRamSelect();
	readRAMReg(REMS);
	SPI_RAM_ReadBuff(&ramData, sizeof(ramData));
	idData->deviceID = ramData[1];
	spiRamRelease();
	return (idData->manufacturerID == 0xc2) ? true : false;
}


uint8_t ramReadPage(uint8_t * dataBuff, uint32_t pageAddress, uint16_t dataLen){
	uint8_t opCodeAndAddress[4] = {READ, (pageAddress >> 16) & 0xFF, (pageAddress >> 8) & 0xFF, pageAddress & 0xFF};
	spiRamSelect();
	SPI_RAM_WriteBuff(&opCodeAndAddress, sizeof(opCodeAndAddress));
	SPI_RAM_ReadBuff(dataBuff, dataLen);
	spiRamRelease();
	return true;
}

uint8_t ramErasePage(uint32_t pageAddress){
	uint8_t opCodeAndAddress[4] = {SE, (pageAddress >> 16) & 0xFF, (pageAddress >> 8) & 0xFF, pageAddress & 0xFF};
	
	//Unlock memory to write
	spiRamSelect();
	writeRAMCmd(WREN);
	spiRamRelease();
	//Erase page memory by page address
	spiRamSelect();
	SPI_RAM_WriteBuff(&opCodeAndAddress, sizeof(opCodeAndAddress));
	spiRamRelease();
	//Erase check if operation complete
	uint8_t eraseStatus = 0xff; 
	while (eraseStatus & 0x03) {
		spiRamSelect();
		eraseStatus = readRAMReg(RDSR);
		spiRamRelease();
	}
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