/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

static void button_on_PB01_pressed(void)
{
}

static void button_on_PB08_pressed(void)
{
}

static void button_on_PA10_pressed(void)
{
}

static void button_on_PB30_pressed(void)
{
}

/**
 * Example of using EXTERNAL_IRQ_0
 */
void EXTERNAL_IRQ_0_example(void)
{

	ext_irq_register(PIN_PB01, button_on_PB01_pressed);
	ext_irq_register(PIN_PB08, button_on_PB08_pressed);
	ext_irq_register(PIN_PA10, button_on_PA10_pressed);
	ext_irq_register(PIN_PB30, button_on_PB30_pressed);
}

/**
 * Example of using EXT_SPI to write "Hello World" using the IO abstraction.
 */
static uint8_t example_EXT_SPI[12] = "Hello World!";

void EXT_SPI_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&EXT_SPI, &io);

	spi_m_sync_enable(&EXT_SPI);
	io_write(io, example_EXT_SPI, 12);
}

/**
 * Example of using RF_SPI to write "Hello World" using the IO abstraction.
 */
static uint8_t example_RF_SPI[12] = "Hello World!";

void RF_SPI_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&RF_SPI, &io);

	spi_m_sync_enable(&RF_SPI);
	io_write(io, example_RF_SPI, 12);
}

void RTC_I2C_example(void)
{
	struct io_descriptor *RTC_I2C_io;

	i2c_m_sync_get_io_descriptor(&RTC_I2C, &RTC_I2C_io);
	i2c_m_sync_enable(&RTC_I2C);
	i2c_m_sync_set_slaveaddr(&RTC_I2C, 0x12, I2C_M_SEVEN);
	io_write(RTC_I2C_io, (uint8_t *)"Hello World!", 12);
}

void EXT_I2C_example(void)
{
	struct io_descriptor *EXT_I2C_io;

	i2c_m_sync_get_io_descriptor(&EXT_I2C, &EXT_I2C_io);
	i2c_m_sync_enable(&EXT_I2C);
	i2c_m_sync_set_slaveaddr(&EXT_I2C, 0x12, I2C_M_SEVEN);
	io_write(EXT_I2C_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using ETH_SPI to write "Hello World" using the IO abstraction.
 */
static uint8_t example_ETH_SPI[12] = "Hello World!";

void ETH_SPI_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&ETH_SPI, &io);

	spi_m_sync_enable(&ETH_SPI);
	io_write(io, example_ETH_SPI, 12);
}
