/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>

//struct spi_m_sync_descriptor EXT_SPI;
//struct spi_m_sync_descriptor RF_SPI;

//struct usart_sync_descriptor DEBUG_SERIAL;

//struct i2c_m_sync_desc EXT_I2C;

void EXTERNAL_IRQ_0_init(void)
{
	_gclk_enable_channel(EIC_GCLK_ID, CONF_GCLK_EIC_SRC);

	// Set pin direction to input
	gpio_set_pin_direction(BTN, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(BTN, GPIO_PULL_OFF);
	gpio_set_pin_function(BTN, PINMUX_PB01A_EIC_EXTINT1);

	
	gpio_set_pin_function(EXT_I2C_IRQ, PORT_PB08A_EIC_EXTINT8);

	ext_irq_init();
}

//void EXT_SPI_PORT_init(void)
//{
//
	//gpio_set_pin_level(PA04, false);
	//gpio_set_pin_direction(PA04, GPIO_DIRECTION_OUT);
	//gpio_set_pin_function(PA04, PINMUX_PA04D_SERCOM0_PAD0);
	//gpio_set_pin_level(PA05, false);
	//gpio_set_pin_direction(PA05, GPIO_DIRECTION_OUT);
	//gpio_set_pin_function(PA05, PINMUX_PA05D_SERCOM0_PAD1);
	//gpio_set_pin_direction(PA06, GPIO_DIRECTION_IN);
	//gpio_set_pin_pull_mode(PA06, GPIO_PULL_OFF);
	//gpio_set_pin_function(PA06, PINMUX_PA06D_SERCOM0_PAD2);
//}
//
//void EXT_SPI_CLOCK_init(void)
//{
	//_pm_enable_bus_clock(PM_BUS_APBC, SERCOM0);
	//_gclk_enable_channel(SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC);
//}
//
//void EXT_SPI_init(void)
//{
	//EXT_SPI_CLOCK_init();
	//spi_m_sync_init(&EXT_SPI, SERCOM0);
	//EXT_SPI_PORT_init();
//}

//void RF_SPI_PORT_init(void)
//{
	//gpio_set_pin_level(PA00, false);
	//gpio_set_pin_direction(PA00, GPIO_DIRECTION_OUT);
	//gpio_set_pin_function(PA00, PINMUX_PA00D_SERCOM1_PAD0);
	//gpio_set_pin_level(PA01, false);
	//gpio_set_pin_direction(PA01, GPIO_DIRECTION_OUT);
	//gpio_set_pin_function(PA01, PINMUX_PA01D_SERCOM1_PAD1);
	//gpio_set_pin_direction(PA18, GPIO_DIRECTION_IN);
//
	//gpio_set_pin_pull_mode(PA18, GPIO_PULL_OFF);
	//gpio_set_pin_function(PA18, PINMUX_PA18C_SERCOM1_PAD2);
//}
//
//void RF_SPI_CLOCK_init(void)
//{
	//_pm_enable_bus_clock(PM_BUS_APBC, SERCOM1);
	//_gclk_enable_channel(SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC);
//}
//
//void RF_SPI_init(void)
//{
	//RF_SPI_CLOCK_init();
	//spi_m_sync_init(&RF_SPI, SERCOM1);
	//RF_SPI_PORT_init();
//}







void delay_driver_init(void)
{
	delay_init(SysTick);
}

void system_init(void)
{
	init_mcu();


	gpio_set_pin_level(LED_G, false);
	gpio_set_pin_direction(LED_G, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LED_G, GPIO_PIN_FUNCTION_OFF);

	gpio_set_pin_level(IO4, false);
	gpio_set_pin_direction(IO4, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(IO4, GPIO_PIN_FUNCTION_OFF);


	gpio_set_pin_level(IO3,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	//gpio_set_pin_direction(IO3, GPIO_DIRECTION_IN);
	//gpio_set_pin_function(IO3, GPIO_PIN_FUNCTION_OFF);
	// GPIO on PB31

	gpio_set_pin_level(LED_R,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(LED_R, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(LED_R, GPIO_PIN_FUNCTION_OFF);

	EXTERNAL_IRQ_0_init();

	//EXT_SPI_init();

	//RF_SPI_init();

	//DEBUG_SERIAL_init();

	delay_driver_init();
}
