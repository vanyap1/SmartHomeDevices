/*
 * SMART_UPS_Unit_XMEGA_CANBUS.c
 *
 * Created: 19/07/2025 22:50:00
 * Author : Ivan Prints
 */ 

#include "config.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "spic_driver.h"
#include "twie_driver.h"
#include "adc.h"
#include "gpio_driver.h"
#include "debugSerial.h"
#include "sysTickTimer.h"
#include "rtc.h"
#include "canspi.h"


	
uint8_t adcVal = 0 ;
uint8_t counter = 0;
uCAN_MSG txMessage;
uCAN_MSG rxMessage;

uint32_t oneSecondEvent = 0;


int main(void)
{
	//OSC.CTRL |= OSC_RC32MEN_bm;
	//while ((OSC.STATUS & OSC_RC32MRDY_bm) == 0)
	//_PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_RC32M_gc);
	
	
	
	
	//NVM.CTRLB = NVM_WAIT2_bm; // 2 wait states ??? > 33 ???
	
	// 1. ????????? RC 2 MHz
	OSC.CTRL |= OSC_RC2MEN_bm;
	while (!(OSC.STATUS & OSC_RC2MRDY_bm));  // ??????? ??????????
	//// 2. ??????????? PLL: ??????? = RC2M, ??????? = 24
	OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | 24;
	//// 3. ????????? PLL
	OSC.CTRL |= OSC_PLLEN_bm;
	while (!(OSC.STATUS & OSC_PLLRDY_bm));   // ???????, ???? PLL ??????????????
	//// 4. ?????????? ??????? ?????????? ????a = PLL
	_PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_PLL_gc);
	
	//while (WDT.STATUS & WDT_SYNCBUSY_bm); // ?????? ??????????
	//CCP = CCP_IOREG_gc;
	//WDT.CTRL = WDT_PER_4KCLK_gc | WDT_ENABLE_bm | WDT_CEN_bm;

	
	
	sei();
	debugSerialInit(DEB_SERIAL_BAUD);
	
	gpioInit();
	adc_init();
	gpio_set_pin_level(&GLD, false);
	
	initSpic();
	CANSPI_Initialize();
	sysTickInit();
	_delay_ms(150);
	gpio_set_pin_level(&GLD, true);
	
	
	
	

    while (1) 
    {
		
		if ((getSysTick() - oneSecondEvent) >= 1000)
		{
			oneSecondEvent = getSysTick();
			//gpio_toggle_pin_level(&GLD);
			wdt_reset();
			
			adcVal = adc_read(0);
			printf("adc: %d; systick: %ld\n\r", adcVal, getSysTick());
			
			txMessage.frame.idType = 0x555;
			txMessage.frame.id = dSTANDARD_CAN_MSG_ID_2_0B;
			txMessage.frame.dlc = 8;
			txMessage.frame.data0 = counter;
			txMessage.frame.data1 = adcVal;
			txMessage.frame.data2 = 0x01;
			txMessage.frame.data3 = 0x02;
			txMessage.frame.data4 = 0x03;
			txMessage.frame.data5 = 0x04;
			txMessage.frame.data6 = 0x05;
			txMessage.frame.data7 = 0x06;
			CANSPI_Transmit(&txMessage);
			counter++;
		}

		
		if(CANSPI_Receive(&rxMessage))
		{
			 printf("ID:%08lx; DLC:%x; D:%02x %02x %02x %02x %02x %02x %02x %02x\r\n", rxMessage.frame.id, rxMessage.frame.dlc,
			 rxMessage.frame.data0,
			 rxMessage.frame.data1,
			 rxMessage.frame.data2,
			 rxMessage.frame.data3,
			 rxMessage.frame.data4,
			 rxMessage.frame.data5,
			 rxMessage.frame.data6,
			 rxMessage.frame.data7);
			 gpio_set_pin_level(&FAN, rxMessage.frame.data7 != 0 ? false : true);
		 }
		 _delay_ms(50);
    }
}

