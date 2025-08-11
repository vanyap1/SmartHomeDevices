
/*
 * ws28_led.c
 *
 * Created: 16.10.2023 22:28:23
 *  Author: Vanya
 */ 
#include "driver.h"
#include "ws28_led.h"

#include "driver_init.h"
#include "utils.h"
#include <peripheral_clk_config.h>
//#include <utils.h>
//#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>

struct spi_m_sync_descriptor SPI_PIXEL;
struct io_descriptor *neopixel_io;



void SPI_PIXEL_init(void){
	
	
	gpio_set_pin_direction(PA22, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(PA22, GPIO_PULL_OFF);
	gpio_set_pin_function(PA22, PINMUX_PA22D_SERCOM5_PAD0);
	gpio_set_pin_level(NEOPIXEL_OUT, false);
	gpio_set_pin_direction(NEOPIXEL_OUT, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(NEOPIXEL_OUT, PINMUX_PA20C_SERCOM5_PAD2);
	gpio_set_pin_level(PA21,false);
	gpio_set_pin_direction(PA21, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(PA21, PINMUX_PA21C_SERCOM5_PAD3);
	
	
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM5);
	_gclk_enable_channel(SERCOM5_GCLK_ID_CORE, CONF_GCLK_SERCOM5_CORE_SRC);
	spi_m_sync_init(&SPI_PIXEL, SERCOM5);
	spi_m_sync_get_io_descriptor(&SPI_PIXEL, &neopixel_io);
	spi_m_sync_enable(&SPI_PIXEL);
}


void neopixelInit(){
	SPI_PIXEL_init();
}


void neopixelWrite(){
	io_write(neopixel_io, "abc", 3);
}