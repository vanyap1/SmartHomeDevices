/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

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
#include <hal_spi_m_sync.h>

#include "hal_usb_device.h"

extern struct spi_m_sync_descriptor EXT_SPI;
extern struct spi_m_sync_descriptor RF_SPI;

extern struct i2c_m_sync_desc RTC_I2C;

extern struct i2c_m_sync_desc       EXT_I2C;
extern struct spi_m_sync_descriptor ETH_SPI;

void EXT_SPI_PORT_init(void);
void EXT_SPI_CLOCK_init(void);
void EXT_SPI_init(void);

void RF_SPI_PORT_init(void);
void RF_SPI_CLOCK_init(void);
void RF_SPI_init(void);

void RTC_I2C_CLOCK_init(void);
void RTC_I2C_init(void);
void RTC_I2C_PORT_init(void);

void EXT_I2C_CLOCK_init(void);
void EXT_I2C_init(void);
void EXT_I2C_PORT_init(void);

void ETH_SPI_PORT_init(void);
void ETH_SPI_CLOCK_init(void);
void ETH_SPI_init(void);

void USB_0_CLOCK_init(void);
void USB_0_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED
