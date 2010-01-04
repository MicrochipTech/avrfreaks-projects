//=============================================================================
// Standard USB HID-compatible game controller (joystick) with buttons.
//
// $Id: out_joystick_btn.h,v 1.1.2.1 2006/08/10 20:58:23 SOY Exp $
//=============================================================================

#ifndef __OUT_JOYSTICK_BTN_H_INCLUDED__
#define __OUT_JOYSTICK_BTN_H_INCLUDED__

#if OUT_JOYSTICK_BTN

//=============================================================================
// Function prototypes
extern void outSendData(void);

//=============================================================================
// USB Descriptor options and definitions.
// The VID and PID values are bogus. Please do not distribute binary
// code with such values, use your own registered values instead.
#define USB_CFG_VENDOR_ID           0x00, 0xff
#define USB_CFG_DEVICE_ID           0x01, 0x92
#define USB_CFG_DEVICE_VERSION      0x00, 0x01
#define USB_CFG_VENDOR_NAME         'O', 'l', 'e', 'g', ' ', 'S', 'e', 'm', 'y', 'o', 'n', 'o', 'v'
#define USB_CFG_VENDOR_NAME_LEN     13
#define USB_CFG_DEVICE_NAME         'R', 'C', ' ', 'U', 'S', 'B', ' ', 'C', 'o', 'n', 't', 'r', 'o', 'l', 'l', 'e', 'r'
#define USB_CFG_DEVICE_NAME_LEN     17
#define USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH    70

#endif  // OUT_JOYSTICK_BTN

#endif  // __OUT_JOYSTICK_BTN_H_INCLUDED__
