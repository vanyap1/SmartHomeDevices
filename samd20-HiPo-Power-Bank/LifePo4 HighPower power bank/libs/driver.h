
/*
 * driver.h
 *
 * Created: 23.08.2023 11:19:52
 *  Author: Vanya
 */ 

#include <stdint.h>
#include <stdbool.h>
#include <u8g2.h>
#include "RFM69.h"

#ifndef DRIVER_H_INCLUDED
#define DRIVER_H_INCLUDED

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

	#include <hal_usart_sync.h>

	#include <hal_delay.h>

	extern struct spi_m_sync_descriptor EXT_SPI;
	extern struct spi_m_sync_descriptor RF_SPI;
	extern struct i2c_m_sync_desc EXT_I2C;
	extern struct usart_sync_descriptor DEBUG_SERIAL;
	
	
	void RF_IRQ_Enable(void);
	void RF_int_Handler(void);
	
	void EXT_I2C_IRQ_Enable(void);
	void EXT_I2C_IRQ_int_Handler(void);
	void RTC_I2C_IRQ_Enable(void);
	uint8_t RTC_IRQ_Ready(void);
	void I2C_RTC_Handler(void);
	
	uint8_t EXT_I2C_IRQ_isReady(void);
	
	bool rf_isReady();
	rfHeader* rfMsgType(void);
	uint8_t RFM69_read_reg(uint8_t addr);
	void RFM69_ReadBuff(uint8_t* buff, uint16_t len);
	void RFM69_WriteBuff(uint8_t* buff, uint16_t len);
	void RFM_69_sel(bool arg);
	
	
	void SPI_RAM_sel(bool arg);
	void SPI_ADC_sel(bool arg);							//Adc select - true; unselect - false
	void SPI_RAM_ReadBuff(uint8_t* buff, uint16_t len);
	void SPI_RAM_WriteBuff(uint8_t* buff, uint16_t len);
	void SPI_write(uint8_t);
	uint8_t SPI_read();
	bool get_io0();
	void toggle_led();
	void set_led();
	void reset_led();
	
	
	
	//Display section
	void EXT_SPI_init(void);
	void vfd_write_data(uint8_t* buff, uint16_t len);
	void vfd_cs_set(bool state);
	void cmd_pin_handle(bool state);
	uint8_t u8x8_avr_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
	uint8_t vfd_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
	
	//RF_Section
	void RF_SPI_init(void);
	void RF_HW_Reset(void);
	
	//External I2C port
	void EXT_I2C_init(void);
	void RTC_I2C_init(void);
	bool RTC_write_batch(uint8_t addres, uint8_t *data, uint8_t data_len);
	bool RTC_read_batch(uint8_t addres ,uint8_t *data, uint8_t data_len);
	bool I2C_write_batch(uint8_t addres , uint8_t *data, uint8_t data_len);
	bool I2C_read_batch(uint8_t addres ,uint8_t *data, uint8_t data_len);
	bool I2C_read_batch_addr(uint8_t addres, uint8_t reg, uint8_t *data, uint8_t data_len);
	
	void ws_wire_write_bit(bool value);
	void DEBUG_SERIAL_init(void);
	void DebugSerialWrite(uint8_t *data, uint8_t data_len);
	void DebugSerialWriteln(uint8_t *data, uint8_t data_len);
	void DebugSerialWriteByte(uint8_t byte);
	
	#ifdef __cplusplus
}
#endif
#endif // DRIVER_EXAMPLES_H_INCLUDED
