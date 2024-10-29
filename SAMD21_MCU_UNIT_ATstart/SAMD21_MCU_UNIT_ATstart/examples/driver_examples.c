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
 * Example of using IRQ_CONTROLLER
 */
void IRQ_CONTROLLER_example(void)
{

	ext_irq_register(PIN_PB01, button_on_PB01_pressed);
	ext_irq_register(PIN_PB08, button_on_PB08_pressed);
	ext_irq_register(PIN_PA10, button_on_PA10_pressed);
	ext_irq_register(PIN_PB30, button_on_PB30_pressed);
}

/**
 * Example of using SPI_EXT to write "Hello World" using the IO abstraction.
 */
static uint8_t example_SPI_EXT[12] = "Hello World!";

void SPI_EXT_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_EXT, &io);

	spi_m_sync_enable(&SPI_EXT);
	io_write(io, example_SPI_EXT, 12);
}

/**
 * Example of using SPI_RF to write "Hello World" using the IO abstraction.
 */
static uint8_t example_SPI_RF[12] = "Hello World!";

void SPI_RF_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_RF, &io);

	spi_m_sync_enable(&SPI_RF);
	io_write(io, example_SPI_RF, 12);
}

void I2C_RTC_example(void)
{
	struct io_descriptor *I2C_RTC_io;

	i2c_m_sync_get_io_descriptor(&I2C_RTC, &I2C_RTC_io);
	i2c_m_sync_enable(&I2C_RTC);
	i2c_m_sync_set_slaveaddr(&I2C_RTC, 0x12, I2C_M_SEVEN);
	io_write(I2C_RTC_io, (uint8_t *)"Hello World!", 12);
}

void I2C_EXT_example(void)
{
	struct io_descriptor *I2C_EXT_io;

	i2c_m_sync_get_io_descriptor(&I2C_EXT, &I2C_EXT_io);
	i2c_m_sync_enable(&I2C_EXT);
	i2c_m_sync_set_slaveaddr(&I2C_EXT, 0x12, I2C_M_SEVEN);
	io_write(I2C_EXT_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using SPI_ETH to write "Hello World" using the IO abstraction.
 */
static uint8_t example_SPI_ETH[12] = "Hello World!";

void SPI_ETH_example(void)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_ETH, &io);

	spi_m_sync_enable(&SPI_ETH);
	io_write(io, example_SPI_ETH, 12);
}
