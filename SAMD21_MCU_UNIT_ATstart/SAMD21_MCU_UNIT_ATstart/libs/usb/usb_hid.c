
/*
 * usb_hid.c
 *
 * Created: 29.10.2024 14:34:20
 *  Author: Ivan Prints
 */ 
#include "atmel_start.h"
#include "usb_start.h"

const static uint8_t customer_hid_report[] = {CONF_USB_HID_GENERIC_REPORT};

static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    HID_GENERIC_DESCES_LS_FS};

static struct usbd_descriptors single_desc[] = {{single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes)}
#if CONF_USBD_HS_SP
                                                ,
                                                {NULL, NULL}
#endif
};

/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64];



void usb_HID_init(void)
{
	usbdc_init(ctrl_buffer);
	uint8_t ln = sizeof(customer_hid_report);
	hiddf_generic_init(customer_hid_report, sizeof(customer_hid_report));

	usbdc_start(single_desc);
	usbdc_attach();
	delay_ms(500);
	//while (!hiddf_generic_is_enabled()) {
		// wait hid generic to be installed
	//};
}