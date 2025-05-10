
/*
 * usb_hid.c
 *
 * Created: 29.10.2024 14:34:20
 *  Author: Ivan Prints
 */ 
#include "atmel_start.h"
#include "usb_start.h"

typedef struct {
	uint8_t throttle[2];			
	int8_t axix[4];				
	uint16_t buttons;	
} gamepadData;


const static uint8_t customer_hid_report[] = {CONF_USB_HID_GENERIC_REPORT};
//const static uint8_t customer_hid_report[] = {CONF_USB_HID_GAMEPAD_REPORT};
const uint8_t keyboard_report_desc[59] = {
	0x05, 0x01, /* Usage Page (Generic Desktop)      */
	0x09, 0x06, /* Usage (Keyboard)                  */
	0xA1, 0x01, /* Collection (Application)          */
	0x05, 0x07, /* Usage Page (Keyboard)             */
	0x19, 0xE0, /* Usage Minimum (224)               */
	0x29, 0xE7, /* Usage Maximum (231)               */
	0x15, 0x00, /* Logical Minimum (0)               */
	0x25, 0x01, /* Logical Maximum (1)               */
	0x75, 0x01, /* Report Size (1)                   */
	0x95, 0x08, /* Report Count (8)                  */
	0x81, 0x02, /* Input (Data, Variable, Absolute)  */
	0x81, 0x01, /* Input (Constant)                  */
	0x19, 0x00, /* Usage Minimum (0)                 */
	0x29, 0x65, /* Usage Maximum (101)               */
	0x15, 0x00, /* Logical Minimum (0)               */
	0x25, 0x65, /* Logical Maximum (101)             */
	0x75, 0x08, /* Report Size (8)                   */
	0x95, 0x06, /* Report Count (6)                  */
	0x81, 0x00, /* Input (Data, Array)               */
	0x05, 0x08, /* Usage Page (LED)                  */
	0x19, 0x01, /* Usage Minimum (1)                 */
	0x29, 0x05, /* Usage Maximum (5)                 */
	0x15, 0x00, /* Logical Minimum (0)               */
	0x25, 0x01, /* Logical Maximum (1)               */
	0x75, 0x01, /* Report Size (1)                   */
	0x95, 0x05, /* Report Count (5)                  */
	0x91, 0x02, /* Output (Data, Variable, Absolute) */
	0x95, 0x03, /* Report Count (3)                  */
	0x91, 0x01, /* Output (Constant)                 */
	0xC0        /* End Collection                    */
};


const uint8_t wiiccReportDescriptor[74] =
{
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x05, // USAGE (Game Pad)
	0xa1, 0x01, // COLLECTION (Application)
	0x05, 0x02, // USAGE_PAGE (Simulation Controls)
	0x09, 0xbb, // USAGE (Throttle)
	0x15, 0x00, // LOGICAL_MINIMUM (0)
	0x25, 0x1f, // LOGICAL_MAXIMUM (31)
	0x75, 0x08, // REPORT_SIZE (8)
	0x95, 0x01, // REPORT_COUNT (1)
	0x81, 0x02, // INPUT (Data,Var,Abs)
	0x05, 0x02, // USAGE_PAGE (Simulation Controls)
	0x09, 0xbb, // USAGE (Throttle)
	0x15, 0x00, // LOGICAL_MINIMUM (0)
	0x25, 0x1f, // LOGICAL_MAXIMUM (31)
	0x75, 0x08, // REPORT_SIZE (8)
	0x95, 0x01, // REPORT_COUNT (1)
	0x81, 0x02, // INPUT (Data,Var,Abs)
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0xa1, 0x00, // COLLECTION (Physical)
	0x09, 0x30, // USAGE (X)
	0x09, 0x31, // USAGE (Y)
	0x09, 0x32, // USAGE (Z)
	0x09, 0x33, // USAGE (Rx)
	0x15, 0x81, // LOGICAL_MINIMUM (-127)
	0x25, 0x7f, // LOGICAL_MAXIMUM (127)
	0x75, 0x08, // REPORT_SIZE (8)
	0x95, 0x04, // REPORT_COUNT (4)
	0x81, 0x02, // INPUT (Data,Var,Abs)
	0x05, 0x09, // USAGE_PAGE (Button)
	0x19, 0x01, // USAGE_MINIMUM (Button 1)
	0x29, 0x10, // USAGE_MAXIMUM (Button 16)
	0x15, 0x00, // LOGICAL_MINIMUM (0)
	0x25, 0x01, // LOGICAL_MAXIMUM (1)
	0x75, 0x01, // REPORT_SIZE (1)
	0x95, 0x10, // REPORT_COUNT (16)
	0x81, 0x02, // INPUT (Data,Var,Abs)
	0xc0, // END_COLLECTION
	0xc0 // END_COLLECTION
};
static uint8_t single_desc_bytes[] = {
    /* Device descriptors and Configuration descriptors list. */
    HID_GENERIC_DESCES_LS_FS};




static struct usbd_descriptors single_desc[] = {{single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes)}
#if CONF_USBD_HS_SP
 ,{NULL, NULL}
#endif
};

/** Ctrl endpoint buffer */
static uint8_t ctrl_buffer[64];

void send_gamepad_report(uint16_t buttonState, uint8_t throttle1, uint8_t throttle2, int8_t x, int8_t y, int8_t z, int8_t rx) {
	gamepadData report = {
		.throttle[0] = throttle1,
		.throttle[1] = throttle1,
		.axix[0] = x,
		.axix[1] = y,
		.axix[2] = z,
		.axix[3] = rx,
		.buttons = buttonState
	};
	uint8_t ln = sizeof(report);
	int8_t report2[8];
	hiddf_generic_read(report2, ln);
	hiddf_generic_write((uint8_t*)&report, ln);
	//hiddf_generic_write((uint8_t*)&report, ln);
	
}


void send_mouse_report(int8_t x, int8_t y, uint8_t buttons, int8_t wheel) {
	int8_t report[8];
	report[0] = buttons; // ???? ??????
	report[1] = x;       // ??????????? ?? X
	report[2] = y;       // ??????????? ?? Y
	report[3] = wheel;   // ?????? ?????????
	
	hiddf_generic_write(report, sizeof(report));
}



void usb_HID_init(void)
{
	usbdc_init(ctrl_buffer);
	hiddf_generic_init(customer_hid_report, sizeof(customer_hid_report));
	usbdc_start(single_desc);
	usbdc_attach();
	delay_ms(500);
	//while (!hiddf_generic_is_enabled()) {
		 //wait hid generic to be installed
	//};
}