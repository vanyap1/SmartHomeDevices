
/*
 * hw_init.h
 *
 * Created: 28.02.2024 22:16:34
 *  Author: Vanya
 */ 
#ifndef HW_DRIVER_INCLUDED
#define HW_DRIVER_INCLUDED

#include "pin_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>
#include <hal_ext_irq.h>
#include <hal_spi_m_sync.h>
#include <hal_spi_m_sync.h>
#include <hal_i2c_m_sync.h>
#include <hal_i2c_m_sync.h>
#include "hal_usb_device.h"
#include "RFM69.h"


extern struct spi_m_sync_descriptor EXT_SPI;
extern struct spi_m_sync_descriptor RF_SPI;
extern struct spi_m_sync_descriptor ETH_SPI;

extern struct i2c_m_sync_desc EXT_I2C;
extern struct i2c_m_sync_desc RTC_I2C;	
	


static void BTN_Handler(void);
uint8_t GetBtnState(void);



static void EXT_I2C_IRQ_int_Handler(void);
static void I2C_RTC_Handler(void);
static void RF_int_Handler(void);

bool rf_isReady();
rfHeader* rfMsgType(void);


uint8_t EXT_I2C_IRQ_isReady(void);
uint8_t RTC_IRQ_Ready(void);


void usb_hid_init(void);
void gpio_init(void);

void ETH_SPI_Init(void);
void ETH_SPI_Select(bool state);
void ETH_SPI_ReadBuff(uint8_t* buff, uint16_t len);
void ETH_SPI_WriteBuff(uint8_t* buff, uint16_t len);
void W5500_Select(void);
void W5500_Unselect(void);
void W5500_ReadBuff(uint8_t* buff, uint16_t len);
void W5500_WriteBuff(uint8_t* buff, uint16_t len);
uint8_t W5500_ReadByte(void);
void W5500_Write_byte(uint8_t byte);





void RTC_init(void);
bool RTC_write_batch(uint8_t addres, uint8_t *data, uint8_t data_len);
bool RTC_read_batch(uint8_t addres ,uint8_t *data, uint8_t data_len);


void I2C_init(void);			//External I2C port 
bool I2C_write_batch(uint8_t addres , uint8_t *data, uint8_t data_len);
bool I2C_read_batch(uint8_t addres ,uint8_t *data, uint8_t data_len);
bool I2C_read_batch_addr(uint8_t addres, uint8_t reg, uint8_t *data, uint8_t data_len);


void RF_init(void);

uint8_t RFM69_read_reg(uint8_t addr);
void RFM69_ReadBuff(uint8_t* buff, uint16_t len);
void RFM69_WriteBuff(uint8_t* buff, uint16_t len);
void RFM_69_sel(bool arg);
void SPI_write(uint8_t);
uint8_t SPI_read();
void RF_HW_Reset(void);


void IRQ_init(void);

void mcu_init(void);

#ifdef __cplusplus
}
#endif
#endif // HW_DRIVER_INCLUDED