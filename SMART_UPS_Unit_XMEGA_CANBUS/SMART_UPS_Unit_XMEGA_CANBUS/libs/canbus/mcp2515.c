/*
 * mcp2515.c
 *
 *  Created on: Dec 13, 2020
 *      Author: matt
 */

#include "config.h"
#include "mcp2515.h"
#include "spic_driver.h"

#include <string.h> //Todo: this is yucky just for the memset function

/* Modify below items for your SPI configurations */

#define SPI_TIMEOUT             10


/* Prototypes */
static void SPI_Tx(uint8_t data);
static void SPI_TxBuffer(uint8_t *buffer, uint8_t length);
static uint8_t SPI_Rx(void);
static void SPI_RxBuffer(uint8_t *buffer, uint8_t length);

/* initialize MCP2515 */
bool MCP2515_Initialize(void)
{

	return true;
}

/* change mode as configuration mode */
bool MCP2515_SetConfigMode(void)
{
	/* configure CANCTRL Register */
	MCP2515_WriteByte(MCP2515_CANCTRL, 0x80);
	
	uint8_t loop = 10;
	
	do {
		/* confirm mode configuration */
		if((MCP2515_ReadByte(MCP2515_CANSTAT) & 0xE0) == 0x80)
		return true;
		
		loop--;
	} while(loop > 0);
	
	return false;
}

/* change mode as normal mode */
bool MCP2515_SetNormalMode(void)
{
	/* configure CANCTRL Register */
	MCP2515_WriteByte(MCP2515_CANCTRL, 0x00);
	
	uint8_t loop = 10;
	
	do {
		/* confirm mode configuration */
		if((MCP2515_ReadByte(MCP2515_CANSTAT) & 0xE0) == 0x00)
		return true;
		
		loop--;
	} while(loop > 0);
	
	return false;
}

/* Entering sleep mode */
bool MCP2515_SetSleepMode(void)
{
	/* configure CANCTRL Register */
	MCP2515_WriteByte(MCP2515_CANCTRL, 0x20);
	
	uint8_t loop = 10;
	
	do {
		/* confirm mode configuration */
		if((MCP2515_ReadByte(MCP2515_CANSTAT) & 0xE0) == 0x20)
		return true;
		
		loop--;
	} while(loop > 0);
	
	return false;
}

/* MCP2515 SPI-Reset */
void MCP2515_Reset(void)
{
	CAN_Select();
	
	SPI_Tx(MCP2515_RESET);
	
	CAN_Unselect();
}

/* read single byte */
uint8_t MCP2515_ReadByte (uint8_t address)
{
	uint8_t retVal;
	
	CAN_Select();
	
	SPI_Tx(MCP2515_READ);
	SPI_Tx(address);
	retVal = SPI_Rx();
	
	CAN_Unselect();
	
	return retVal;
}

/* read buffer */
void MCP2515_ReadRxSequence(uint8_t instruction, uint8_t *data, uint8_t length)
{
	CAN_Select();
	
	SPI_Tx(instruction);
	SPI_RxBuffer(data, length);
	
	CAN_Unselect();
}

/* write single byte */
void MCP2515_WriteByte(uint8_t address, uint8_t data)
{
	CAN_Select();
	
	SPI_Tx(MCP2515_WRITE);
	SPI_Tx(address);
	SPI_Tx(data);
	
	CAN_Unselect();
}

/* write buffer */
void MCP2515_WriteByteSequence(uint8_t startAddress, uint8_t endAddress, uint8_t *data)
{
	CAN_Select();
	
	SPI_Tx(MCP2515_WRITE);
	SPI_Tx(startAddress);
	SPI_TxBuffer(data, (endAddress - startAddress + 1));
	
	CAN_Unselect();
}

/* write to TxBuffer */
void MCP2515_LoadTxSequence(uint8_t instruction, uint8_t *idReg, uint8_t dlc, uint8_t *data)
{
	CAN_Select();
	
	SPI_Tx(instruction);
	SPI_TxBuffer(idReg, 4);
	SPI_Tx(dlc);
	SPI_TxBuffer(data, dlc);
	
	CAN_Unselect();
}

/* write to TxBuffer(1 byte) */
void MCP2515_LoadTxBuffer(uint8_t instruction, uint8_t data)
{
	CAN_Select();
	
	SPI_Tx(instruction);
	SPI_Tx(data);
	
	CAN_Unselect();
}

/* request to send */
void MCP2515_RequestToSend(uint8_t instruction)
{
	CAN_Select();
	
	SPI_Tx(instruction);
	
	CAN_Unselect();
}

/* read status */
uint8_t MCP2515_ReadStatus(void)
{
	uint8_t retVal;
	
	CAN_Select();
	
	SPI_Tx(MCP2515_READ_STATUS);
	retVal = SPI_Rx();
	
	CAN_Unselect();
	
	return retVal;
}

/* read RX STATUS register */
uint8_t MCP2515_GetRxStatus(void)
{
	uint8_t retVal;
	
	CAN_Select();
	
	SPI_Tx(MCP2515_RX_STATUS);
	retVal = SPI_Rx();
	
	CAN_Unselect();
	
	return retVal;
}

/* Use when changing register value */
void MCP2515_BitModify(uint8_t address, uint8_t mask, uint8_t data)
{
	CAN_Select();
	
	SPI_Tx(MCP2515_BIT_MOD);
	SPI_Tx(address);
	SPI_Tx(mask);
	SPI_Tx(data);
	
	CAN_Unselect();
}

/* SPI Tx wrapper function  */
static void SPI_Tx(uint8_t data)
{
	spicTxByte(data);
}

/* SPI Tx wrapper function */
static void SPI_TxBuffer(uint8_t *buffer, uint8_t length)
{
	spicTxBuff(buffer, length);
}

/* SPI Rx wrapper function */
static uint8_t SPI_Rx(void)
{
	return spicRxByte();
}

/* SPI Rx wrapper function */
static void SPI_RxBuffer(uint8_t *buffer, uint8_t length)
{
	spicRxBuff(buffer, length);
}
