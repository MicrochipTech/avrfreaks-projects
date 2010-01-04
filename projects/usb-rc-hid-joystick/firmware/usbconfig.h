/* Name: usbconfig.h
 * Project: AVR USB driver
 * Author: Christian Starkjohann
 * Creation Date: 2005-04-01
 * Tabsize: 4
 * Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: Proprietary, free under certain conditions. See Documentation.
 * This Revision: $Id: usbconfig.h,v 1.4.2.3 2006/08/10 16:04:29 SOY Exp $
 */

#ifndef __usbconfig_h_included__
#define __usbconfig_h_included__

/*
General Description:
This file is an example configuration (with inline documentation) for the USB
driver. It configures AVR-USB for an ATMega8 with USB D+ connected to Port D
bit 2 (which is also hardware interrupt 0) and USB D- to Port D bit 0. You may
wire the lines to any other port, as long as D- is on bit 0 and D+ is also
wired to INT0.
To create your own usbconfig.h file, copy this file to the directory
containing "usbdrv" (that is your project firmware source directory) and
rename it to "usbconfig.h". Then edit it accordingly.
*/

// A lot of device description options are interface-specific
#include "options.h"

/* ---------------------------- Hardware Config ---------------------------- */

//#define USB_CFG_IOPORTNAME      D
/* This is the port where the USB bus is connected. When you configure it to
 * "B", the registers PORTB, PINB and DDRB will be used.
 */
//#define USB_CFG_DMINUS_BIT      0
/* This is the bit number in USB_CFG_IOPORT where the USB D- line is connected.
 * This may be any bit in the port.
 */
//#define USB_CFG_DPLUS_BIT       1
/* This is the bit number in USB_CFG_IOPORT where the USB D+ line is connected.
 * This may be any bit in the port. Please note that D+ must also be connected
 * to interrupt pin INT0!
 */

/* ----------------------- Optional Hardware Config ------------------------ */

/* #define USB_CFG_PULLUP_IOPORTNAME   D */
/* If you connect the 1.5k pullup resistor from D- to a port pin instead of
 * V+, you can connect and disconnect the device from firmware by calling
 * the macros usbDeviceConnect() and usbDeviceDisconnect() (see usbdrv.h).
 * This constant defines the port on which the pullup resistor is connected.
 */

/* #define USB_CFG_PULLUP_BIT          4 */
/* This constant defines the bit number in USB_CFG_PULLUP_IOPORT (defined
 * above) where the 1.5k pullup resistor is connected. See description
 * above for details.
 */

/* #define  USB_BUFFER_SECTION         ".bss" */
/* The USB receive buffer (variable "usbRxBuf") with a length of 22 bytes
 * MUST NOT cross a 256 byte boundary. We have introduced this configuration
 * option to allow you to change the data segment where this buffer is
 * allocated. If you have problems with the default segment (start of .bss),
 * you may change this setting. See the comment in usbdrv.h for details.
 * On IAR C, the default is the TINY_Z segment (first 256 bytes). You must
 * change this default for devices which don't have RAM below 0x100.
 */

/* --------------------------- Functional Range ---------------------------- */

#define USB_CFG_HAVE_INTRIN_ENDPOINT    1
/* Define this to 1 if you want to compile a version with two endpoints: The
 * default control endpoint 0 and an interrupt-in endpoint 1.
 */
#define USB_CFG_HAVE_INTRIN_ENDPOINT3   0
/* Define this and USB_CFG_HAVE_INTRIN_ENDPOINT to 1 if you want to compile a
 * version with three endpoints: The default control endpoint 0, an
 * interrupt-in endpoint 1 and an interrupt-in endpoint 3.
 */
#define USB_CFG_IMPLEMENT_HALT          0
/* Define this to 1 if you also want to implement the ENDPOINT_HALT feature
 * for endpoint 1 (interrupt endpoint). Although you may not need this feature,
 * it is required by the standard. We have made it a config option because it
 * bloats the code considerably.
 */
#define USB_CFG_INTR_POLL_INTERVAL      10
/* If you compile a version with endpoint 1 (interrupt-in), this is the poll
 * interval. The value is in milliseconds and must not be less than 10 ms for
 * low speed devices.
 */
#define USB_CFG_IS_SELF_POWERED         0
/* Define this to 1 if the device has its own power supply. Set it to 0 if the
 * device is powered from the USB bus.
 */
#define USB_CFG_MAX_BUS_POWER           50
/* Set this variable to the maximum USB bus power consumption of your device.
 * The value is in milliamperes. [It will be divided by two since USB
 * communicates power requirements in units of 2 mA.]
 */
#define USB_CFG_SAMPLE_EXACT            1
/* This variable affects Sampling Jitter for USB receiving. When it is 0, the
 * driver guarantees a sampling window of 1/2 bit. The USB spec requires
 * that the receiver has at most 1/4 bit sampling window. The 1/2 bit window
 * should still work reliably enough because we work at low speed. If you want
 * to meet the spec, set this value to 1. This will unroll a loop which
 * results in bigger code size.
 * If you have problems with long cables, try setting this value to 1.
 */
#define USB_CFG_IMPLEMENT_FN_WRITE      0
/* Set this to 1 if you want usbFunctionWrite() to be called for control-out
 * transfers. Set it to 0 if you don't need it and want to save a couple of
 * bytes.
 */
#define USB_CFG_IMPLEMENT_FN_READ       0
/* Set this to 1 if you need to send control replies which are generated
 * "on the fly" when usbFunctionRead() is called. If you only want to send
 * data from a static buffer, set it to 0 and return the data from
 * usbFunctionSetup(). This saves a couple of bytes.
 */
#define USB_CFG_IMPLEMENT_FN_WRITEOUT   0
/* Define this to 1 if you want to use interrupt-out (or bulk out) endpoint 1.
 * You must implement the function usbFunctionWriteOut() which receives all
 * interrupt/bulk data sent to endpoint 1.
 */
#define USB_CFG_HAVE_FLOWCONTROL        0
/* Define this to 1 if you want flowcontrol over USB data. See the definition
 * of the macros usbDisableAllRequests() and usbEnableAllRequests() in
 * usbdrv.h.
 */
#define USB_CFG_EXTERNAL_CONFIG_DESCRIPTOR_LENGH    0
/* Define this to the size of your configuration descriptor if you provide
 * your own configuration descriptor at the global program memory address
 * 'usbDescrConfig'.
 */

/* -------------------------- Device Description --------------------------- */

//#define  USB_CFG_VENDOR_ID       0xc0, 0x16
/* USB vendor ID for the device, low byte first. If you have registered your
 * own Vendor ID, define it here. Otherwise you use obdev's free shared
 * VID/PID pair. Be sure to read USBID-License.txt for rules!
 * This template uses obdev's shared VID/PID pair for HIDs: 0x16c0/0x5df.
 * Use this VID/PID pair ONLY if you understand the implications!
 */
//#define  USB_CFG_DEVICE_ID       0xdf, 0x05
/* This is the ID of the product, low byte first. It is interpreted in the
 * scope of the vendor ID. If you have registered your own VID with usb.org
 * or if you have licensed a PID from somebody else, define it here. Otherwise
 * you use obdev's free shared VID/PID pair. Be sure to read the rules in
 * USBID-License.txt!
 * This template uses obdev's shared VID/PID pair for HIDs: 0x16c0/0x5df.
 * Use this VID/PID pair ONLY if you understand the implications!
 */
//#define USB_CFG_DEVICE_VERSION  0x00, 0x01
/* Version number of the device: Minor number first, then major number.
 */
//#define USB_CFG_VENDOR_NAME     'w', 'w', 'w', '.', 'o', 'b', 'd', 'e', 'v', '.', 'a', 't'
//#define USB_CFG_VENDOR_NAME_LEN 12
/* These two values define the vendor name returned by the USB device. The name
 * must be given as a list of characters under single quotes. The characters
 * are interpreted as Unicode (UTF-16) entities.
 * If you don't want a vendor name string, undefine these macros.
 * ALWAYS define a vendor name containing your Internet domain name if you use
 * obdev's free shared VID/PID pair. See the file USBID-License.txt for
 * details.
 */
//#define USB_CFG_DEVICE_NAME     'T', 'e', 'm', 'p', 'l', 'a', 't', 'e'
//#define USB_CFG_DEVICE_NAME_LEN 8
/* Same as above for the device name. If you don't want a device name, undefine
 * the macros. See the file USBID-License.txt before you assign a name if you
 * use a shared VID/PID.
 */
#define USB_CFG_SERIAL_NUMBER_LENGTH  0
/* Set this define to the number of charcters in the serial number if your
 * device should have a serial number to uniquely identify each hardware
 * instance. You must supply the serial number in a string descriptor with the
 * name "usbCfgSerialNumberStringDescriptor", e.g.:
 * #define USB_CFG_SERIAL_NUMBER_LENGTH  5
 * int usbCfgSerialNumberStringDescriptor[] PROGMEM = {
 *     USB_STRING_DESCRIPTOR_HEADER(USB_CFG_SERIAL_NUMBER_LENGTH),
 *     '1', '2', '3', '4', '5'
 * };
 * See usbdrv.h for more information about the USB_STRING_DESCRIPTOR_HEADER()
 * macro or usbdrv.c for example string descriptors.
 * You may want to put "usbCfgSerialNumberStringDescriptor" at a constant
 * flash memory address (with magic linker commands) so that you don't need
 * to recompile if you change it.
 */
#define USB_CFG_DEVICE_CLASS        0
#define USB_CFG_DEVICE_SUBCLASS     0
/* See USB specification if you want to conform to an existing device class.
 */
#define USB_CFG_INTERFACE_CLASS     3   /* HID */
#define USB_CFG_INTERFACE_SUBCLASS  0
#define USB_CFG_INTERFACE_PROTOCOL  0
/* See USB specification if you want to conform to an existing device class or
 * protocol.
 * This template defines a HID class device. If you implement a vendor class
 * device, set USB_CFG_INTERFACE_CLASS to 0 and USB_CFG_DEVICE_CLASS to 0xff.
 */
//#define USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH    42  /* total length of report descriptor */
/* Define this to the length of the HID report descriptor, if you implement
 * an HID device. Otherwise don't define it or define it to 0.
 * Since this template defines a HID device, it must also specify a HID
 * report descriptor length. You must add a PROGMEM character array named
 * "usbHidReportDescriptor" to your code which contains the report descriptor.
 * Don't forget to keep the array and this define in sync!
 */

/* ----------------------- Optional MCU Description ------------------------ */

/* The following configurations have working defaults in usbdrv.h. You
 * usually don't need to set them explicitly. Only if you want to run
 * the driver on a device which is not yet supported or with a compiler
 * which is not fully supported (such as IAR C) or if you use a differnt
 * interrupt than INT0, you may have to define some of these.
 */
/* #define USB_INTR_CFG            MCUCR */
/* #define USB_INTR_CFG_SET        ((1 << ISC00) | (1 << ISC01)) */
/* #define USB_INTR_CFG_CLR        0 */
/* #define USB_INTR_ENABLE         GIMSK */
/* #define USB_INTR_ENABLE_BIT     INT0 */
/* #define USB_INTR_PENDING        GIFR */
/* #define USB_INTR_PENDING_BIT    INTF0 */

#endif /* __usbconfig_h_included__ */
