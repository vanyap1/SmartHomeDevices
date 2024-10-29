
/*
 * usb_hid.h
 *
 * Created: 29.10.2024 14:34:13
 *  Author: Ivan Prints
 */ 
#ifndef USB_DEVICE_MAIN_H
#define USB_DEVICE_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "hiddf_generic.h"
#include "hiddf_generic_desc.h"

void usb_HID_init(void);

#ifdef __cplusplus
}
#endif

#endif