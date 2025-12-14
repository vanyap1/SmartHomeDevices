
/*
 * tlc59_driver.h
 *
 * Created: 05.10.2023 22:26:37
 *  Author: Vanya
 */ 
#include "string.h"
#include "stdbool.h"

#ifndef TLC59_DRIVER_H_
#define TLC59_DRIVER_H_

#define TLC_ADDR		0x61
#define TLC_ALLCALL		0x61
#define SHIF_DELAY		50

//Wave config
#define ON_UP_DOWN		1
#define OFF_UP_DOWN		2
#define ON_DOWN_UP		3
#define OFF_DOWN_UP		4


//9.5.1ModeRegister1(MODE1)
#define MODE1			0x00
#define AI2				7
#define AI1				6
#define AI0				5
#define OSC				4
#define SUB1			3
#define SUB2			2
#define SUB3			1
#define ALLCALL			0

//9.5.2ModeRegister2(MODE2)
#define MODE2			0x01
#define EFCLR			7
#define DMBLNK			5
#define OCH				3

//9.5.3BrightnessControlRegisters0to15(PWM0toPWM15)
#define PWM0			0x02
#define PWM1			0x03
#define PWM2			0x04
#define PWM3			0x05
#define PWM4			0x06
#define PWM5			0x07
#define PWM6			0x08
#define PWM7			0x09
#define PWM8			0x0A
#define PWM9			0x0B
#define PWM10			0x0C
#define PWM11			0x0D
#define PWM12			0x0E
#define PWM13			0x0F
#define PWM14			0x10
#define PWM15			0x11

//9.5.4GroupDutyCycleControlRegister(GRPPWM)
#define GRPPWM			0x12

//9.5.5GroupFrequencyRegister(GRPFREQ)
#define GRPFREQ			0x13

//9.5.6 LEDDriverOutputStateRegisters0to3(LEDOUT0toLEDOUT3)
//	LDRx=00:LEDdriverxisoff(defaultpower-upstate). 
//	LDRx=01:LEDdriverxisfullyon(individualbrightnessandgroupdimming/blinkingnotcontrolled). 
//	LDRx=10:LEDdriverxisindividualbrightnesscanbecontrolledthroughitsPWMxregister. 
//	LDRx=11:LEDdriverxisindividualbrightnessandgroupdimming/blinkingcanbecontrolledthroughitsPWMx registerandtheGRPPWMregisters.

#define LEDOUT0			0x14
#define LDR3			6
#define LDR2			4
#define LDR1			2
#define LDR0			0

#define LEDOUT1			0x15
#define LDR7			6
#define LDR6			4
#define LDR5			2
#define LDR4			0

#define LEDOUT2			0x16
#define LDR11			6
#define LDR10			4
#define LDR9			2
#define LDR8			0

#define LEDOUT3			0x17
#define LDR15			6
#define LDR14			4
#define LDR13			2
#define LDR12			0

//9.5.7 I2CBusSubaddressRegisters1to3(SUBADR1toSUBADR3)
#define SUBADR0			0x18
#define SUBADR1			0x19
#define SUBADR2			0x1A

//9.5.8 LEDAllCallI2CBusAddressRegister(ALLCALLADR)
#define ALLCALLADR		0x1B

//9.5.9OutputGainControlRegister(IREF)
#define IREF			0x1C
#define CM				7	//High/lowcurrentmultiplier
#define HC				6	//Subcurrent
#define CC				0	//Currentmultiplier

//9.5.10 ErrorFlagsRegisters(EFLAG1,EFLAG2)
#define EFLAG1			0x1D
#define EFLAG2			0x1E

#define SWRST			0xd6
#define SWRST_byte_1	0xa5
#define SWRST_byte_2	0x5a




void led_board_sw_reset(uint8_t board_address);
uint8_t led_board_init(uint8_t board_address);
uint8_t led_board_sleep(uint8_t board_address);
void led_write_batch(uint8_t board_address, int *pwmChannels, uint8_t channels_num);
void led_write_single(uint8_t board_address, uint8_t led_num, uint8_t brightness);
void run_wave(uint8_t cmd);

#endif

