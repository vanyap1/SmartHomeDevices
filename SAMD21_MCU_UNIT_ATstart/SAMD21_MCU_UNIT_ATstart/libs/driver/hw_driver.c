
/*
 * hw_driver.c
 *
 * Created: 29.10.2024 12:10:16
 *  Author: Ivan Prints
 */ 
#include "hw_driver.h"

#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>


struct spi_m_sync_descriptor SPI_EXT;
struct spi_m_sync_descriptor SPI_RF;
struct spi_m_sync_descriptor SPI_ETH;

struct i2c_m_sync_desc I2C_RTC;
struct i2c_m_sync_desc I2C_EXT;

struct io_descriptor *ext_spi;
struct io_descriptor *rf_spi;
struct io_descriptor *eth_spi;

struct io_descriptor *rtc_i2c;
struct io_descriptor *ext_i2c;



//IRQ handlers

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

//IRQ end



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
	
	gpio_set_pin_level(SD_CS, true);
	gpio_set_pin_direction(SD_CS, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(SD_CS, GPIO_PIN_FUNCTION_OFF);
	
	gpio_set_pin_level(ROM_CS, true);
	gpio_set_pin_direction(ROM_CS, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(ROM_CS, GPIO_PIN_FUNCTION_OFF);
	
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
	
	spi_m_sync_init(&SPI_ETH, SERCOM5);
	spi_m_sync_get_io_descriptor(&SPI_ETH, &eth_spi);
	spi_m_sync_enable(&SPI_ETH);
	
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
	
	i2c_m_sync_init(&I2C_RTC, SERCOM2);
	
	gpio_set_pin_pull_mode(PA08, GPIO_PULL_OFF);
	gpio_set_pin_function(PA08, PINMUX_PA08D_SERCOM2_PAD0);

	gpio_set_pin_pull_mode(PA09, GPIO_PULL_OFF);
	gpio_set_pin_function(PA09, PINMUX_PA09D_SERCOM2_PAD1);
	
	i2c_m_sync_get_io_descriptor(&I2C_RTC, &rtc_i2c);
	i2c_m_sync_enable(&I2C_RTC);
	
	gpio_set_pin_direction(RTC_IRQ, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(RTC_IRQ, GPIO_PULL_OFF);
	gpio_set_pin_function(RTC_IRQ, PINMUX_PA10A_EIC_EXTINT10);
	
}

bool RTC_write_batch(uint8_t addres, uint8_t *data, uint8_t data_len){
	i2c_m_sync_set_slaveaddr(&I2C_RTC, addres, I2C_M_SEVEN);
	//i2c_m_sync_cmd_write(&EXT_I2C, 0x00, data, data_len);
	return (io_write(rtc_i2c, (uint8_t *)data, data_len) >= 0) ? true : false;
}

bool RTC_read_batch(uint8_t addres ,uint8_t *data, uint8_t data_len){
	i2c_m_sync_set_slaveaddr(&I2C_RTC, addres, I2C_M_SEVEN);
	i2c_m_sync_cmd_read(&I2C_RTC, 0x00, data, data_len);
	//return (io_read(I2C_io, (uint8_t *)data, data_len) >= 0) ? true : false;
}


void I2C_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM4);
	_gclk_enable_channel(SERCOM4_GCLK_ID_CORE, CONF_GCLK_SERCOM4_CORE_SRC);
	_gclk_enable_channel(SERCOM4_GCLK_ID_SLOW, CONF_GCLK_SERCOM4_SLOW_SRC);
	
	i2c_m_sync_init(&I2C_EXT, SERCOM4);
	
	gpio_set_pin_pull_mode(PB12, GPIO_PULL_OFF);
	gpio_set_pin_function(PB12, PINMUX_PB12C_SERCOM4_PAD0);
	gpio_set_pin_pull_mode(PB13, GPIO_PULL_OFF);
	gpio_set_pin_function(PB13, PINMUX_PB13C_SERCOM4_PAD1);
	
	i2c_m_sync_get_io_descriptor(&I2C_EXT, &ext_i2c);
	i2c_m_sync_enable(&I2C_EXT);
}

bool I2C_write_batch(uint8_t addres, uint8_t *data, uint8_t data_len){
	i2c_m_sync_set_slaveaddr(&I2C_EXT, addres, I2C_M_SEVEN);
	//i2c_m_sync_cmd_write(&EXT_I2C, 0x00, data, data_len);
	return (io_write(ext_i2c, (uint8_t *)data, data_len) >= 0) ? true : false;
}

bool I2C_read_batch(uint8_t addres ,uint8_t *data, uint8_t data_len){
	i2c_m_sync_set_slaveaddr(&I2C_EXT, addres, I2C_M_SEVEN);
	i2c_m_sync_cmd_read(&I2C_EXT, 0x00, data, data_len);
	return (io_read(ext_i2c, (uint8_t *)data, data_len) >= 0) ? true : false;
}
bool I2C_read_batch_addr(uint8_t addres, uint8_t reg, uint8_t *data, uint8_t data_len){
	i2c_m_sync_set_slaveaddr(&I2C_EXT, addres, I2C_M_SEVEN);
	i2c_m_sync_cmd_read(&I2C_EXT, reg, data, data_len);
}


// LORA RF SPI init
void RF_init(void){
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM1);
	_gclk_enable_channel(SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC);
	
	spi_m_sync_init(&SPI_RF, SERCOM1);
	spi_m_sync_get_io_descriptor(&SPI_RF, &rf_spi);
	spi_m_sync_enable(&SPI_RF);
	
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

void SPI_ReadBuff(uint8_t* buff, uint16_t len){
	io_read(rf_spi,buff,len);
}

void SPI_WriteBuff(uint8_t* buff, uint16_t len){
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

void SD_SetCS(bool newValue){
	gpio_set_pin_level(SD_CS, !newValue);
}
void SD_Select(){
	gpio_set_pin_level(SD_CS, false);
}
void SD_Deselect(){
	gpio_set_pin_level(SD_CS, true);
}

void SPI_RAM_sel(bool newValue){
	gpio_set_pin_level(ROM_CS, newValue);
}


void EXT_SPI_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM0);
	_gclk_enable_channel(SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC);
	
	
	spi_m_sync_init(&SPI_EXT, SERCOM0);
	spi_m_sync_get_io_descriptor(&SPI_EXT, &ext_spi);
	spi_m_sync_enable(&SPI_EXT);
	
	gpio_set_pin_level(PA04, false);
	gpio_set_pin_direction(PA04, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(PA04, PINMUX_PA04D_SERCOM0_PAD0);
	gpio_set_pin_level(PA05, false);
	gpio_set_pin_direction(PA05, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(PA05, PINMUX_PA05D_SERCOM0_PAD1);
	gpio_set_pin_direction(PA06, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(PA06, GPIO_PULL_OFF);
	gpio_set_pin_function(PA06, PINMUX_PA06D_SERCOM0_PAD2);
	gpio_set_pin_level(VFD_RST, true);
	gpio_set_pin_level(FILAMENT_EN, true);
	
	//io_write(io, example_VFD_SPI, 12);

	//VFD additional IO setup
	gpio_set_pin_level(VFD_RST, true);
	gpio_set_pin_direction(VFD_RST, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(VFD_RST, GPIO_PIN_FUNCTION_OFF);
	
	gpio_set_pin_level(FILAMENT_EN, true);
	gpio_set_pin_direction(FILAMENT_EN, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(FILAMENT_EN, GPIO_PIN_FUNCTION_OFF);
	
	gpio_set_pin_level(VFD_CS, true);
	gpio_set_pin_direction(VFD_CS, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(VFD_CS, GPIO_PIN_FUNCTION_OFF);
	
	gpio_set_pin_level(LCD_RST, true);
	gpio_set_pin_direction(LCD_RST, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(LCD_RST, GPIO_PIN_FUNCTION_OFF);
	
	
	
}
void vfd_write_data(uint8_t* buff, uint16_t len){
	io_write(ext_spi, buff, len);
	
}

void vfd_cs_set(bool state){
	gpio_set_pin_level(VFD_CS, !state);
}

void cmd_pin_handle(bool state){
	gpio_set_pin_level(FILAMENT_EN, state);
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


void USB_INSTANCE_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBB, USB);
	_pm_enable_bus_clock(PM_BUS_AHB, USB);
	_gclk_enable_channel(USB_GCLK_ID, CONF_GCLK_USB_SRC);
	
	usb_d_init();
	
	gpio_set_pin_direction(PA24, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(PA24, false);
	gpio_set_pin_pull_mode(PA24, GPIO_PULL_OFF);

	gpio_set_pin_function(PA24, PINMUX_PA24G_USB_DM);
	gpio_set_pin_direction(PA25, GPIO_DIRECTION_OUT);
	gpio_set_pin_level(PA25, false);
	gpio_set_pin_pull_mode(PA25, GPIO_PULL_OFF);
	gpio_set_pin_function(PA25, PINMUX_PA25G_USB_DP);
}

void sys_hw_init(void)
{
	init_mcu();
	gpio_init();
	ETH_SPI_Init();
	EXT_SPI_init();
	I2C_init(); //External port
	RTC_init();
	
	RF_init();
	
	IRQ_init();
	
	
	USB_INSTANCE_init();
	
	ext_irq_register(BTN0, BTN_Handler);
	ext_irq_register(IO3, EXT_I2C_IRQ_int_Handler);
	ext_irq_register(RTC_IRQ, I2C_RTC_Handler);
	ext_irq_register(RF_IRQ, RF_int_Handler);
}
