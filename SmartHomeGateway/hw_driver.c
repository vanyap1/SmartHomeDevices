
/*
 * hw_init.c
 *
 * Created: 28.02.2024 22:16:59
 *  Author: Vanya
 */ 
#include "hw_driver.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>
#include "RFM69.h"


struct spi_m_sync_descriptor EXT_SPI;
struct spi_m_sync_descriptor RF_SPI;
struct spi_m_sync_descriptor ETH_SPI;

struct i2c_m_sync_desc EXT_I2C;
struct i2c_m_sync_desc RTC_I2C;

struct io_descriptor *ext_spi;
struct io_descriptor *rf_spi;
struct io_descriptor *eth_spi;

struct io_descriptor *rtc_i2c;
struct io_descriptor *ext_i2c;


uint8_t btnState;
uint8_t i2cIntCount = 0;
uint8_t rtcIntCount = 0;
uint8_t rfIntCount = 0;
rfHeader* ptr_rfHeader;

	
static void BTN_Handler(void){
	btnState = gpio_get_pin_level(BTN0);
	//btnState = 1;
}

uint8_t GetBtnState(void){
	return btnState;
}

static void EXT_I2C_IRQ_int_Handler(void){
	i2cIntCount++;
}

static void I2C_RTC_Handler(void){
	rtcIntCount++;
}

static void RF_int_Handler(void){
	rfIntCount++;
}

bool rf_isReady(){
	if (rfIntCount != 0){
		ptr_rfHeader = data_ready();
		rfIntCount = 0;
		return (ptr_rfHeader->dataValid == 0 ) ? false : true;
	}
	return false;
}

rfHeader* rfMsgType(void) {
	return ptr_rfHeader;
}



uint8_t EXT_I2C_IRQ_isReady(void){
	uint8_t res = i2cIntCount;
	i2cIntCount = 0;
	return res;
}



uint8_t RTC_IRQ_Ready(void){
	uint8_t res = rtcIntCount;
	rtcIntCount = 0;
	return res;
}


void usb_hid_init(void){
	//CLock init
	_pm_enable_bus_clock(PM_BUS_APBB, USB);
	_pm_enable_bus_clock(PM_BUS_AHB, USB);
	_gclk_enable_channel(USB_GCLK_ID, CONF_GCLK_USB_SRC);
	
	usb_d_init();
	//USB io pins init
	gpio_set_pin_direction(PA24, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(PA24, false);
	gpio_set_pin_pull_mode(PA24, GPIO_PULL_OFF);
	gpio_set_pin_function(PA24, PINMUX_PA24G_USB_DM);
	gpio_set_pin_direction(PA25, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(PA25, false);
	gpio_set_pin_pull_mode(PA25, GPIO_PULL_OFF);
	gpio_set_pin_function(PA25,PINMUX_PA25G_USB_DP);
	
}
#if (CONF_GCLK_USB_FREQUENCY > (48000000 + 48000000 / 400)) || (CONF_GCLK_USB_FREQUENCY < (48000000 - 48000000 / 400))
#warning USB clock should be 48MHz ~ 0.25% clock, check your configuration!
#endif


void gpio_init(void){
		gpio_set_pin_level(RLD, false);
		gpio_set_pin_direction(RLD, GPIO_DIRECTION_OUT);
		gpio_set_pin_function(RLD, GPIO_PIN_FUNCTION_OFF);
		
		gpio_set_pin_level(GLD, false);
		gpio_set_pin_direction(GLD, GPIO_DIRECTION_OUT);
		gpio_set_pin_function(GLD, GPIO_PIN_FUNCTION_OFF);

		gpio_set_pin_level(LED_SD, false);
		gpio_set_pin_direction(LED_SD, GPIO_DIRECTION_OUT);
		gpio_set_pin_function(LED_SD, GPIO_PIN_FUNCTION_OFF);
		
		gpio_set_pin_level(RF_LED, false);
		gpio_set_pin_direction(RF_LED, GPIO_DIRECTION_OUT);
		gpio_set_pin_function(RF_LED, GPIO_PIN_FUNCTION_OFF);
		
		gpio_set_pin_level(ETH_LED, false);
		gpio_set_pin_direction(ETH_LED, GPIO_DIRECTION_OUT);
		gpio_set_pin_function(ETH_LED, GPIO_PIN_FUNCTION_OFF);
		
		gpio_set_pin_direction(SD_DETECT, GPIO_DIRECTION_IN);
		gpio_set_pin_pull_mode(SD_DETECT, GPIO_PULL_OFF);
		gpio_set_pin_function(SD_DETECT, GPIO_PIN_FUNCTION_OFF);
	
}

//ETHERNET HW DRIVER
void ETH_SPI_Init(void){
	gpio_set_pin_level(ETH_CS, true);
	gpio_set_pin_direction(ETH_CS, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(ETH_CS, GPIO_PIN_FUNCTION_OFF);
	
	gpio_set_pin_level(PB16, false);
	gpio_set_pin_direction(PB16, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(PB16, PINMUX_PB16C_SERCOM5_PAD0);

	gpio_set_pin_level(PB17, false);
	gpio_set_pin_direction(PB17, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(PB17, PINMUX_PB17C_SERCOM5_PAD1);

	gpio_set_pin_direction(PA20, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(PA20, GPIO_PULL_OFF);
	gpio_set_pin_function(PA20, PINMUX_PA20C_SERCOM5_PAD2);
	
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM5);
	_gclk_enable_channel(SERCOM5_GCLK_ID_CORE, CONF_GCLK_SERCOM5_CORE_SRC);
	
	spi_m_sync_init(&ETH_SPI, SERCOM5);
	spi_m_sync_get_io_descriptor(&ETH_SPI, &eth_spi);
	spi_m_sync_enable(&ETH_SPI);
	
}
void ETH_SPI_Select(bool state){
	gpio_set_pin_level(ETH_CS, state);
}


void ETH_SPI_ReadBuff(uint8_t* buff, uint16_t len){
	io_read(eth_spi,buff,len);
}

void ETH_SPI_WriteBuff(uint8_t* buff, uint16_t len){
	io_write(eth_spi,buff,len);
}

void W5500_Select(void){
	ETH_SPI_Select(false);
}

void W5500_Unselect(void){
	ETH_SPI_Select(true);
}

void W5500_ReadBuff(uint8_t* buff, uint16_t len){
	io_read(eth_spi,buff,len);
}

void W5500_WriteBuff(uint8_t* buff, uint16_t len){
	io_write(eth_spi,buff,len);
}

uint8_t W5500_ReadByte(void){
	uint8_t data_byte = 0;
	io_read(eth_spi,&data_byte,1);
	return data_byte;
}
void W5500_Write_byte(uint8_t byte){
	io_write(eth_spi, &byte, 1);
}



void RTC_init(void){
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM2);
	_gclk_enable_channel(SERCOM2_GCLK_ID_CORE, CONF_GCLK_SERCOM2_CORE_SRC);
	_gclk_enable_channel(SERCOM2_GCLK_ID_SLOW, CONF_GCLK_SERCOM2_SLOW_SRC);
	
	i2c_m_sync_init(&RTC_I2C, SERCOM2);
	
	gpio_set_pin_pull_mode(PA08, GPIO_PULL_OFF);
	gpio_set_pin_function(PA08, PINMUX_PA08D_SERCOM2_PAD0);

	gpio_set_pin_pull_mode(PA09, GPIO_PULL_OFF);
	gpio_set_pin_function(PA09, PINMUX_PA09D_SERCOM2_PAD1);
	
	i2c_m_sync_get_io_descriptor(&RTC_I2C, &rtc_i2c);
	i2c_m_sync_enable(&RTC_I2C);
	
	gpio_set_pin_direction(RTC_IRQ, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(RTC_IRQ, GPIO_PULL_OFF);
	gpio_set_pin_function(RTC_IRQ, PINMUX_PA10A_EIC_EXTINT10);
	
}

bool RTC_write_batch(uint8_t addres, uint8_t *data, uint8_t data_len){
	i2c_m_sync_set_slaveaddr(&RTC_I2C, addres, I2C_M_SEVEN);
	//i2c_m_sync_cmd_write(&EXT_I2C, 0x00, data, data_len);
	return (io_write(rtc_i2c, (uint8_t *)data, data_len) >= 0) ? true : false;
}

bool RTC_read_batch(uint8_t addres ,uint8_t *data, uint8_t data_len){
	i2c_m_sync_set_slaveaddr(&RTC_I2C, addres, I2C_M_SEVEN);
	i2c_m_sync_cmd_read(&RTC_I2C, 0x00, data, data_len);
	//return (io_read(I2C_io, (uint8_t *)data, data_len) >= 0) ? true : false;
}


void I2C_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM4);
	_gclk_enable_channel(SERCOM4_GCLK_ID_CORE, CONF_GCLK_SERCOM4_CORE_SRC);
	_gclk_enable_channel(SERCOM4_GCLK_ID_SLOW, CONF_GCLK_SERCOM4_SLOW_SRC);
	
	i2c_m_sync_init(&EXT_I2C, SERCOM4);
	
	gpio_set_pin_pull_mode(PB12, GPIO_PULL_OFF);
	gpio_set_pin_function(PB12, PINMUX_PB12C_SERCOM4_PAD0);
	gpio_set_pin_pull_mode(PB13, GPIO_PULL_OFF);
	gpio_set_pin_function(PB13, PINMUX_PB13C_SERCOM4_PAD1);
	
	i2c_m_sync_get_io_descriptor(&EXT_I2C, &ext_i2c);
	i2c_m_sync_enable(&EXT_I2C);
}

bool I2C_write_batch(uint8_t addres, uint8_t *data, uint8_t data_len){
	i2c_m_sync_set_slaveaddr(&EXT_I2C, addres, I2C_M_SEVEN);
	//i2c_m_sync_cmd_write(&EXT_I2C, 0x00, data, data_len);
	return (io_write(ext_i2c, (uint8_t *)data, data_len) >= 0) ? true : false;
}

bool I2C_read_batch(uint8_t addres ,uint8_t *data, uint8_t data_len){
	i2c_m_sync_set_slaveaddr(&EXT_I2C, addres, I2C_M_SEVEN);
	i2c_m_sync_cmd_read(&EXT_I2C, 0x00, data, data_len);
	return (io_read(ext_i2c, (uint8_t *)data, data_len) >= 0) ? true : false;
}
bool I2C_read_batch_addr(uint8_t addres, uint8_t reg, uint8_t *data, uint8_t data_len){
	i2c_m_sync_set_slaveaddr(&EXT_I2C, addres, I2C_M_SEVEN);
	i2c_m_sync_cmd_read(&EXT_I2C, reg, data, data_len);
}

// LORA RF SPI init
void RF_init(void){
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM1);
	_gclk_enable_channel(SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC);
	
	spi_m_sync_init(&RF_SPI, SERCOM1);
	spi_m_sync_get_io_descriptor(&RF_SPI, &rf_spi);
	spi_m_sync_enable(&RF_SPI);
	
	gpio_set_pin_level(PA00, false);
	gpio_set_pin_direction(PA00, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(PA00, PINMUX_PA00D_SERCOM1_PAD0);
	gpio_set_pin_level(PA01, false);
	gpio_set_pin_direction(PA01, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(PA01, PINMUX_PA01D_SERCOM1_PAD1);
	gpio_set_pin_direction(PA18, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(PA18, GPIO_PULL_OFF);
	gpio_set_pin_function(PA18, PINMUX_PA18C_SERCOM1_PAD2);
	//Gpio setup
	
	gpio_set_pin_level(RF_RST, false);
	gpio_set_pin_direction(RF_RST, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(RF_RST, GPIO_PIN_FUNCTION_OFF);
	RF_HW_Reset();
	//Radio additional IO setup
	gpio_set_pin_level(RF_CS, true);
	gpio_set_pin_direction(RF_CS, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(RF_CS, GPIO_PIN_FUNCTION_OFF);
}

uint8_t RFM69_read_reg(uint8_t addr){
	io_write(rf_spi, &addr, 1);
	io_read(rf_spi,&addr,1);
	return addr;
}

void RFM69_ReadBuff(uint8_t* buff, uint16_t len){
	io_read(rf_spi,buff,len);
}

void RFM69_WriteBuff(uint8_t* buff, uint16_t len){
	io_write(rf_spi,buff,len);
}

void RFM_69_sel(bool arg){
	gpio_set_pin_level(RF_CS , arg);
}

void SPI_write(uint8_t arg){
	io_write(rf_spi, &arg, 1);
}


uint8_t SPI_read(){
	uint8_t data_byte = 0;
	io_read(rf_spi,&data_byte,1);
	return data_byte;
}

void RF_HW_Reset(void){
	gpio_set_pin_level(RF_RST, true);
	delay_ms(1);
	gpio_set_pin_level(RF_RST, false);
	delay_ms(20);
}


void IRQ_init(void){

	
	_gclk_enable_channel(EIC_GCLK_ID, CONF_GCLK_EIC_SRC);

	gpio_set_pin_direction(BTN0, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(BTN0, GPIO_PULL_OFF);
	gpio_set_pin_function(BTN0, PINMUX_PB01A_EIC_EXTINT1);
	
	gpio_set_pin_direction(IO3, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(IO3, GPIO_PULL_UP);
	gpio_set_pin_function(IO3, PORT_PB08A_EIC_EXTINT8);
	
	gpio_set_pin_direction(RTC_IRQ, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(RTC_IRQ, GPIO_PULL_OFF);
	gpio_set_pin_function(RTC_IRQ, PINMUX_PA10A_EIC_EXTINT10);
		
	gpio_set_pin_direction(RF_IRQ, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(RF_IRQ, GPIO_PULL_DOWN);
	gpio_set_pin_function(RF_IRQ, PINMUX_PB30A_EIC_EXTINT14);
	
	
	ext_irq_init();
}






void mcu_init(void)
{
	init_mcu();
	gpio_init();
	ETH_SPI_Init();
	
	I2C_init(); //External port
	
	
	RF_init();
	
	IRQ_init();
	usb_hid_init();
	
	
	ext_irq_register(BTN0, BTN_Handler);
	ext_irq_register(IO3, EXT_I2C_IRQ_int_Handler);
	ext_irq_register(RTC_IRQ, I2C_RTC_Handler);
	ext_irq_register(RF_IRQ, RF_int_Handler);
}