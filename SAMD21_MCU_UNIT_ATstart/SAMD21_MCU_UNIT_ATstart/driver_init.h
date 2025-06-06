/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

#include "atmel_start_pins.h"

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
#include <hal_spi_m_sync.h>

#include "hal_usb_device.h"

extern struct spi_m_sync_descriptor SPI_EXT;
extern struct spi_m_sync_descriptor SPI_RF;

extern struct i2c_m_sync_desc I2C_RTC;

extern struct i2c_m_sync_desc       I2C_EXT;
extern struct spi_m_sync_descriptor SPI_ETH;

void SPI_EXT_PORT_init(void);
void SPI_EXT_CLOCK_init(void);
void SPI_EXT_init(void);

void SPI_RF_PORT_init(void);
void SPI_RF_CLOCK_init(void);
void SPI_RF_init(void);

void I2C_RTC_CLOCK_init(void);
void I2C_RTC_init(void);
void I2C_RTC_PORT_init(void);

void I2C_EXT_CLOCK_init(void);
void I2C_EXT_init(void);
void I2C_EXT_PORT_init(void);

void SPI_ETH_PORT_init(void);
void SPI_ETH_CLOCK_init(void);
void SPI_ETH_init(void);

void USB_DEVICE_INSTANCE_CLOCK_init(void);
void USB_DEVICE_INSTANCE_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED
