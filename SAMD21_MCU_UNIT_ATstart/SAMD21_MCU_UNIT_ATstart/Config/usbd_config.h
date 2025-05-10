/* Auto-generated config file usbd_config.h */
#ifndef USBD_CONFIG_H
#define USBD_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// ---- USB Device Stack Core Options ----

// <q> High Speed Support
// <i> Enable high speed specific descriptors support, e.g., DeviceQualifierDescriptor and OtherSpeedConfiguration Descriptor.
// <i> High speed support require descriptors description array on start, for LS/FS and HS support in first and second place.
// <id> usbd_hs_sp
#ifndef CONF_USBD_HS_SP
#define CONF_USBD_HS_SP 1
#endif

// ---- USB Device Stack HID generic Options ----

// <e> Enable String Descriptors
// <id> usb_hid_generic_str_en
#ifndef CONF_USB_HID_GENERIC_STR_EN
#define CONF_USB_HID_GENERIC_STR_EN 1
#endif
// <s> Language IDs
// <i> Language IDs in c format, split by comma (E.g., 0x0409 ...)
// <id> usb_hid_generic_langid
#ifndef CONF_USB_HID_GENERIC_LANGID
#define CONF_USB_HID_GENERIC_LANGID "0x0409"
#endif

#ifndef CONF_USB_HID_GENERIC_LANGID_DESC
#define CONF_USB_HID_GENERIC_LANGID_DESC
#endif
// </e>

// <h> HID Generic Device Descriptor

// <o> bcdUSB
// <0x0200=> USB 2.0 version
// <0x0210=> USB 2.1 version
// <id> usb_hid_generic_bcdusb
#ifndef CONF_USB_HID_GENERIC_BCDUSB
#define CONF_USB_HID_GENERIC_BCDUSB 0x200
#endif

// <o> bMaxPackeSize0
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_hid_generic_bmaxpksz0
#ifndef CONF_USB_HID_GENERIC_BMAXPKSZ0
#define CONF_USB_HID_GENERIC_BMAXPKSZ0 0x0040//0x40
#endif

// <o> idVender <0x0000-0xFFFF>
// <id> usb_hid_generic_idvender
#ifndef CONF_USB_HID_GENERIC_IDVENDER
#define CONF_USB_HID_GENERIC_IDVENDER 0x03eb
#endif

// <o> idProduct <0x0000-0xFFFF>
// <id> usb_hid_generic_idproduct
#ifndef CONF_USB_HID_GENERIC_IDPRODUCT
#define CONF_USB_HID_GENERIC_IDPRODUCT 0x2402
#endif

// <o> bcdDevice <0x0000-0xFFFF>
// <id> usb_hid_generic_bcddevice
#ifndef CONF_USB_HID_GENERIC_BCDDEVICE
#define CONF_USB_HID_GENERIC_BCDDEVICE 0x100
#endif

// <e> Enable string descriptor of iManufact
// <id> usb_hid_generic_imanufact_en
#ifndef CONF_USB_HID_GENERIC_IMANUFACT_EN
#define CONF_USB_HID_GENERIC_IMANUFACT_EN 1
#endif

#ifndef CONF_USB_HID_GENERIC_IMANUFACT
#define CONF_USB_HID_GENERIC_IMANUFACT (CONF_USB_HID_GENERIC_IMANUFACT_EN * (CONF_USB_HID_GENERIC_IMANUFACT_EN))
#endif

// <s> Unicode string of iManufact
// <id> usb_hid_generic_imanufact_str
#ifndef CONF_USB_HID_GENERIC_IMANUFACT_STR
#define CONF_USB_HID_GENERIC_IMANUFACT_STR "Atmel"
#endif

#ifndef CONF_USB_HID_GENERIC_IMANUFACT_STR_DESC
#define CONF_USB_HID_GENERIC_IMANUFACT_STR_DESC
#endif

// </e>

// <e> Enable string descriptor of iProduct
// <id> usb_hid_generic_iproduct_en
#ifndef CONF_USB_HID_GENERIC_IPRODUCT_EN
#define CONF_USB_HID_GENERIC_IPRODUCT_EN 1
#endif

#ifndef CONF_USB_HID_GENERIC_IPRODUCT
#define CONF_USB_HID_GENERIC_IPRODUCT                                                                                  \
	(CONF_USB_HID_GENERIC_IPRODUCT_EN * (CONF_USB_HID_GENERIC_IMANUFACT_EN + CONF_USB_HID_GENERIC_IPRODUCT_EN))
#endif

// <s> Unicode string of iProduct
// <id> usb_hid_generic_iproduct_str
#ifndef CONF_USB_HID_GENERIC_IPRODUCT_STR
#define CONF_USB_HID_GENERIC_IPRODUCT_STR "HID Generic Demo"
#endif

#ifndef CONF_USB_HID_GENERIC_IPRODUCT_STR_DESC
#define CONF_USB_HID_GENERIC_IPRODUCT_STR_DESC
#endif

// </e>

// <e> Enable string descriptor of iSerialNum
// <id> usb_hid_generic_iserialnum_en
#ifndef CONF_USB_HID_GENERIC_ISERIALNUM_EN
#define CONF_USB_HID_GENERIC_ISERIALNUM_EN 0
#endif

#ifndef CONF_USB_HID_GENERIC_ISERIALNUM
#define CONF_USB_HID_GENERIC_ISERIALNUM                                                                                \
	(CONF_USB_HID_GENERIC_ISERIALNUM_EN                                                                                \
	 * (CONF_USB_HID_GENERIC_IMANUFACT_EN + CONF_USB_HID_GENERIC_IPRODUCT_EN + CONF_USB_HID_GENERIC_ISERIALNUM_EN))
#endif

// <s> Unicode string of iSerialNum
// <id> usb_hid_generic_iserialnum_str
#ifndef CONF_USB_HID_GENERIC_ISERIALNUM_STR
#define CONF_USB_HID_GENERIC_ISERIALNUM_STR "123456789ABCDEF"
#endif

#ifndef CONF_USB_HID_GENERIC_ISERIALNUM_STR_DESC
#define CONF_USB_HID_GENERIC_ISERIALNUM_STR_DESC
#endif

// </e>

// </h>

// <h> HID Generic Configuration Descriptor

// <e> Enable string descriptor of iConfig
// <id> usb_hid_generic_iconfig_en
#ifndef CONF_USB_HID_GENERIC_ICONFIG_EN
#define CONF_USB_HID_GENERIC_ICONFIG_EN 1
#endif

#ifndef CONF_USB_HID_GENERIC_ICONFIG
#define CONF_USB_HID_GENERIC_ICONFIG                                                                                   \
	(CONF_USB_HID_GENERIC_ICONFIG_EN                                                                                   \
	 * (CONF_USB_HID_GENERIC_IMANUFACT_EN + CONF_USB_HID_GENERIC_IPRODUCT_EN + CONF_USB_HID_GENERIC_ISERIALNUM_EN      \
	    + CONF_USB_HID_GENERIC_ICONFIG_EN))
#endif

// <s> Unicode string of iConfig
// <id> usb_hid_generic_iconfig_str
#ifndef CONF_USB_HID_GENERIC_ICONFIG_STR
#define CONF_USB_HID_GENERIC_ICONFIG_STR ""
#endif

#ifndef CONF_USB_HID_GENERIC_ICONFIG_STR_DESC
#define CONF_USB_HID_GENERIC_ICONFIG_STR_DESC
#endif

// </e>

// <o> bmAttributes
// <0x80=> Bus power supply, not support for remote wakeup
// <0xA0=> Bus power supply, support for remote wakeup
// <0xC0=> Self powered, not support for remote wakeup
// <0xE0=> Self powered, support for remote wakeup
// <id> usb_hid_generic_bmattri
#ifndef CONF_USB_HID_GENERIC_BMATTRI
#define CONF_USB_HID_GENERIC_BMATTRI 0xE0
#endif

// <o> bMaxPower <0x00-0xFF>
// <id> usb_hid_generic_bmaxpower
#ifndef CONF_USB_HID_GENERIC_BMAXPOWER
#define CONF_USB_HID_GENERIC_BMAXPOWER 0x32
#endif
// </h>

// <h> HID Generic Interface Descriptor

// <o> bInterfaceNumber <0x00-0xFF>
// <id> usb_hid_generic_bifcnum
#ifndef CONF_USB_HID_GENERIC_BIFCNUM
#define CONF_USB_HID_GENERIC_BIFCNUM 0x0
#endif

// <o> iInterface <0x00-0xFF>
// <id> usb_hid_generic_iifc
#ifndef CONF_USB_HID_GENERIC_IIFC
#define CONF_USB_HID_GENERIC_IIFC 0x0
#endif

// <o> HID Generic Report Descriptor Length <0x0000-0xFFFF>
// <id> usb_hid_generic_report_desc_length
#ifndef CONF_USB_HID_GENERIC_REPORT_LEN
#define CONF_USB_HID_GENERIC_REPORT_LEN 53//62(mouse) 53 
#endif

#ifndef CONF_USB_HID_GENERIC_REPORT
#define CONF_USB_HID_GENERIC_REPORT                                                                                    \
	0x06, 0xFF, 0xFF, 0x09, 0x01, 0xA1, 0x01, 0x09, 0x02, 0x09, 0x03, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95,  \
	    0x40, 0x81, 0x02, 0x09, 0x04, 0x09, 0x05, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x40, 0x91, 0x02,    \
	    0x09, 0x06, 0x09, 0x07, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x04, 0xB1, 0x02, 0xC0
#endif

#ifndef CONF_USB_HID_GAMEPAD_REPORT
#define CONF_USB_HID_GAMEPAD_REPORT                                                                                       \
0x05, 0x01,        /* USAGE_PAGE (Generic Desktop) */                    \
0x09, 0x05,        /* USAGE (Gamepad) */                               \
0xA1, 0x01,        /* COLLECTION (Application) */                       \
0x05, 0x09,        /* USAGE_PAGE (Button) */                           \
0x19, 0x01,        /* USAGE_MINIMUM (Button 1) */                       \
0x29, 0x08,        /* USAGE_MAXIMUM (Button 8) */                       \
0x15, 0x00,        /* LOGICAL_MINIMUM (0) */                            \
0x25, 0x01,        /* LOGICAL_MAXIMUM (1) */                            \
0x75, 0x01,        /* REPORT_SIZE (1 bit per button) */                  \
0x95, 0x08,        /* REPORT_COUNT (8 buttons) */                        \
0x81, 0x02,        /* INPUT (Data, Var, Abs) */                         \
0xC0               /* END_COLLECTION */



#endif

#ifndef CONF_USB_HID_MOUSE_REPORT
#define CONF_USB_HID_MOUSE_REPORT                                                                           \
0x05, 0x01,        /* Usage Page (Generic Desktop) */                                                  \
0x09, 0x02,        /* Usage (Mouse) */                                                                 \
0xA1, 0x01,        /* Collection (Application) */                                                     \
0x09, 0x01,        /*   Usage (Pointer) */                                                             \
0xA1, 0x00,        /*   Collection (Physical) */                                                      \
0x05, 0x09,        /*     Usage Page (Button) */                                                      \
0x19, 0x01,        /*     Usage Minimum (Button 1) */                                                 \
0x29, 0x03,        /*     Usage Maximum (Button 3) */                                                 \
0x15, 0x00,        /*     Logical Minimum (0) */                                                      \
0x25, 0x01,        /*     Logical Maximum (1) */                                                      \
0x95, 0x03,        /*     Report Count (3) */                                                         \
0x75, 0x01,        /*     Report Size (1) */                                                          \
0x81, 0x02,        /*     Input (Data, Variable, Absolute) */                                         \
0x95, 0x01,        /*     Report Count (1) */                                                         \
0x75, 0x05,        /*     Report Size (5) */                                                          \
0x81, 0x03,        /*     Input (Constant, Variable, Absolute) - Padding */                          \
0x05, 0x01,        /*     Usage Page (Generic Desktop) */                                             \
0x09, 0x30,        /*     Usage (X) */                                                                \
0x09, 0x31,        /*     Usage (Y) */                                                                \
0x15, 0x81,        /*     Logical Minimum (-127) */                                                   \
0x25, 0x7F,        /*     Logical Maximum (127) */                                                    \
0x75, 0x08,        /*     Report Size (8) */                                                          \
0x95, 0x02,        /*     Report Count (2) */                                                         \
0x81, 0x06,        /*     Input (Data, Variable, Relative) */                                         \
0x09, 0x38,        /*     Usage (Wheel) */                                                            \
0x15, 0x81,        /*     Logical Minimum (-127) */                                                   \
0x25, 0x7F,        /*     Logical Maximum (127) */                                                    \
0x75, 0x08,        /*     Report Size (8) */                                                          \
0x95, 0x01,        /*     Report Count (1) */                                                         \
0x81, 0x06,        /*     Input (Data, Variable, Relative) */                                         \
0xC0,              /*   End Collection */                                                             \
0xC0               /* End Collection */
#endif


// <o> INTERRUPT IN Endpoint Address
// <0x81=> EndpointAddress = 0x81
// <0x82=> EndpointAddress = 0x82
// <0x83=> EndpointAddress = 0x83
// <0x84=> EndpointAddress = 0x84
// <0x85=> EndpointAddress = 0x85
// <0x86=> EndpointAddress = 0x86
// <0x87=> EndpointAddress = 0x87
// <id> usb_hid_generic_intin_epaddr
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_HID_GENERIC_INTIN_EPADDR
#define CONF_USB_HID_GENERIC_INTIN_EPADDR 0x81
#endif

// <o> INTERRUPT IN Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_hid_generic_intin_maxpksz
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_HID_GENERIC_INTIN_MAXPKSZ
#define CONF_USB_HID_GENERIC_INTIN_MAXPKSZ 0x40
#endif

// <o> INTERRUPT OUT Endpoint Address
// <0x01=> EndpointAddress = 0x01
// <0x02=> EndpointAddress = 0x02
// <0x03=> EndpointAddress = 0x03
// <0x04=> EndpointAddress = 0x04
// <0x05=> EndpointAddress = 0x05
// <0x06=> EndpointAddress = 0x06
// <0x07=> EndpointAddress = 0x07
// <id> usb_hid_generic_intout_epaddr
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_HID_GENERIC_INTOUT_EPADDR
#define CONF_USB_HID_GENERIC_INTOUT_EPADDR 0x01
#endif

// <o> INTERRUPT OUT Endpoint wMaxPacketSize
// <0x0008=> 8 bytes
// <0x0010=> 16 bytes
// <0x0020=> 32 bytes
// <0x0040=> 64 bytes
// <id> usb_hid_generic_intout_maxpksz
// <i> Please make sure that the setting here is coincide with the endpoint setting in USB device driver.
#ifndef CONF_USB_HID_GENERIC_INTOUT_MAXPKSZ
#define CONF_USB_HID_GENERIC_INTOUT_MAXPKSZ 0x40
#endif

// </h>

// <<< end of configuration section >>>

#endif // USBD_CONFIG_H
