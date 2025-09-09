
/*
 * cig14.c
 *
 * Created: 07.05.2023 18:57:42
 * Author: Ivan Prints 
 * Email: princ.va@gmail.com
 */ 

#include <avr/io.h>


#define INIT_DISPLAY				0xe0
#define FIRST_CHAR_POSITION			0x20
#define BRIGHTNESS_CMD				0xe4
#define UPDATE_SCREEN				0xe8


#define VFD_16B_DCRAM				0x20
#define VFD_16B_CGRAM				0x40
#define VFD_16B_ADRAM				0x60
#define VFD_16B_URAM				0x80
#define VFD_16B_GLASS_TYPE			0xE0
#define VFD_16B_BRIGHTNESS			0xE4
#define VFD_16B_ON					0xE8
#define VFD_16B_OFF					0xEA
#define VFD_16B_STANDBY_ON			0xED
#define VFD_16B_STANDBY_OFF			0xEC




void vfd_init(uint8_t char_places);

void vfd16_init();

void vfd_set_brightness(uint8_t brightness);
void vfd_write_byte(uint8_t data);
void vfd_write_wbyte(uint8_t data1, uint8_t data2);
void vfd_string(uint8_t position, uint8_t *c);
void vfd16b_string(uint8_t position, uint8_t *c);
	