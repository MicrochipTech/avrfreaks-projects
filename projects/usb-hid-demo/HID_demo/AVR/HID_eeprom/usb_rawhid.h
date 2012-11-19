#ifndef usb_serial_h__
#define usb_serial_h__

#include <stdint.h>
#include <avr/eeprom.h>

uint8_t buffer[64];	// read write buffer

/**************************************************************************
 *
 *  Configurable Options
 *
 **************************************************************************/

// You can change these to give your code its own name.
#define STR_MANUFACTURER	L"PJRC.COM"
#define STR_PRODUCT		L"HID eeprom"

// These 4 numbers identify your device.  Set these to
// something that is (hopefully) not used by any others!
#define VENDOR_ID		0x16C0
#define PRODUCT_ID		0x0480
#define RAWHID_USAGE_PAGE	0xFFAB	// recommended: 0xFF00 to 0xFFFF
#define RAWHID_USAGE		0x0200	// recommended: 0x0100 to 0xFFFF

// These determine the bandwidth that will be allocated
// for your communication.  You do not need to use it
// all, but allocating more than necessary means reserved
// bandwidth is no longer available to other USB devices.
#define RAWHID_TX_SIZE		64	// transmit packet size
#define RAWHID_TX_INTERVAL	2	// max # of ms between transmit packets
#define RAWHID_RX_SIZE		64	// receive packet size
#define RAWHID_RX_INTERVAL	8	// max # of ms between receive packets


/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/

#define ENDPOINT0_SIZE		64
#define RAWHID_INTERFACE	0
#define RAWHID_TX_ENDPOINT	1
#define RAWHID_RX_ENDPOINT	2
#define RAWHID_TX_BUFFER	EP_SINGLE_BUFFER//EP_DOUBLE_BUFFER
#define RAWHID_RX_BUFFER	EP_SINGLE_BUFFER//EP_DOUBLE_BUFFER

void usb_init(void);			// initialize everything
uint8_t usb_configured(void);		// is the USB port configured


// Everything below this point is only intended for usb_serial.c
#ifdef USB_PRIVATE_INCLUDE
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#define EP_TYPE_CONTROL				0x00
#define EP_TYPE_BULK_IN				0x81
#define EP_TYPE_BULK_OUT			0x80
#define EP_TYPE_INTERRUPT_IN		0xC1
#define EP_TYPE_INTERRUPT_OUT		0xC0
#define EP_TYPE_ISOCHRONOUS_IN		0x41
#define EP_TYPE_ISOCHRONOUS_OUT		0x40

#define EP_SINGLE_BUFFER			0x02
#define EP_DOUBLE_BUFFER			0x06

#define EP_SIZE(s)	((s) > 32 ? 0x30 :	\
			((s) > 16 ? 0x20 :	\
			((s) > 8  ? 0x10 :	\
			            0x00)))

#define MAX_ENDPOINT		4

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

#if defined(__AVR_AT90USB162__)
#define HW_CONFIG() 
#define PLL_CONFIG() (PLLCSR = ((1<<PLLE)|(1<<PLLP0)))
#define USB_CONFIG() (USBCON = (1<<USBE))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#elif defined(__AVR_ATmega32U4__)
#define HW_CONFIG() (UHWCON = 0x01)
#define PLL_CONFIG() (PLLCSR = 0x02)	// 0x12 for 16Mhz external , 0x02 for 8Mhz external or internal
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#elif defined(__AVR_AT90USB646__)
#define HW_CONFIG() (UHWCON = 0x81)
#define PLL_CONFIG() (PLLCSR = 0x1A)
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#elif defined(__AVR_AT90USB1286__)
#define HW_CONFIG() (UHWCON = 0x81)
#define PLL_CONFIG() (PLLCSR = 0x16)
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))
#endif

// standard control endpoint request types
#define GET_STATUS			0
#define CLEAR_FEATURE			1
#define SET_FEATURE			3
#define SET_ADDRESS			5
#define GET_DESCRIPTOR			6
#define GET_CONFIGURATION		8
#define SET_CONFIGURATION		9
#define GET_INTERFACE			10
#define SET_INTERFACE			11
// HID (human interface device)
#define HID_GET_REPORT			1
#define HID_GET_IDLE			2
#define HID_GET_PROTOCOL		3
#define HID_SET_REPORT			9
#define HID_SET_IDLE			10
#define HID_SET_PROTOCOL		11
#endif
#endif
