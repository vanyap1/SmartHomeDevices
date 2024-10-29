/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
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

/**
 * Example of using HID Generic Function.
 * \note
 * In this example, we will use a PC as a USB host:
 * - Connect the DEBUG USB on XPLAINED board to PC for program download.
 * - Connect the TARGET USB on XPLAINED board to PC for running program.
 * The application will behave as a generic which can be recongnized by PC.
 */
void hid_generic_example(void)
{
	/* usb stack init */
	usbdc_init(ctrl_buffer);

	/* usbdc_register_funcion inside */
	hiddf_generic_init(customer_hid_report, CONF_USB_HID_GENERIC_REPORT_LEN);

	usbdc_start(single_desc);
	usbdc_attach();
	//hiddf_generic_is_enabled();
	while (!hiddf_generic_is_enabled()) {
		// wait hid generic to be installed
	};
}

void usb_init(void)
{
}
