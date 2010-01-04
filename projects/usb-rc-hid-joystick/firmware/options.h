//=============================================================================
// User-defined compilation options.
//
// $Id: options.h,v 1.1.2.6 2006/08/10 20:58:23 SOY Exp $
//=============================================================================

#ifndef __OPTIONS_H_INCLUDED__
#define __OPTIONS_H_INCLUDED__

//=============================================================================
// Set this to non-zero for schematic from RCDESIGN.RU web site,
// http://www.rcdesign.ru/articles/electronics/rcusb
#define RC_DESIGN       1

//=============================================================================
// INPUT and OUTPUT interface options.
// You have to choose exactly one input and one output interface.
// The input interface is responsible for data buffer definition.
// Input and output interfaces must have compatible data buffer
// structure and HID Report Descriptor.

// Input interfaces (select one)
#define IN_PPM              0       // standard RC PPM decoder
#define IN_PPM_ADV          1       // advanced RC PPM decoder
#define IN_ADC              0       // internal ADC interface

// Output interfaces (select one)
#define OUT_JOYSTICK        0       // standard USB HID-compatible game controller (joystick)
#define OUT_JOYSTICK_BTN    1       // standard USB HID-compatible game controller (joystick)  with buttons
#define OUT_REFLEX          0       // Reflex RC USB device

// Maximum number of channels to read (may be used by some interfaces)
#define MAX_CHANNELS    8

//=============================================================================
// Advanced interface options.
// See detailed option description in corresponding source files.

#define IN_PPM_ADV_N_CHANNELS_ONLY      0   // range is 0 or [1..MAX_CHANNELS]
#define IN_PPM_ADV_N_CHANNELS_EXACT     0   // range is 0 or [1..MAX_CHANNELS]
#define IN_PPM_ADV_N_CHANNELS_AUTO      3   // range is 0 or 2-10 (recommended option)
#define IN_PPM_ADV_POLARITY             0   // 0 for direct, 1 for inverted
#define IN_PPM_ADV_SERVO_TEST_CHANNEL   3   // range is 0 or 1..MAX_CHANNELS

#if RC_DESIGN
#define ICP_PORT_NAME   B                   // ATmega8 ICP1 port and bit (timer capture input)
#define ICP_PORT_BIT    0
#define ST_PORT_NAME    B                   // ATmega8 OC1A port and bit (servo tester output)
#define ST_PORT_BIT     1
#else
#define ICP_PORT_NAME   D                   // ATmega32 ICP1 port and bit (timer capture input)
#define ICP_PORT_BIT    6
#define ST_PORT_NAME    D                   // ATmega32 OC1A port and bit (servo tester output)
#define ST_PORT_BIT     5
#endif

//-----------------------------------------------------------------------------

#define IN_ADC_USE_ONLY_4_CHANNELS      0   // provide some values for 4-channel controllers
#define IN_ADC_USE_CH5_VALUE            1000
#define IN_ADC_USE_CH6_VALUE            1350
#define IN_ADC_USE_CH7_VALUE            1650
#define IN_ADC_USE_CH8_VALUE            2000

#if RC_DESIGN
#define REG_ADCSR   ADCSR                   // for ATmega8
#define REG_ADMUX   ADMUX
#else
#define REG_ADCSR   ADCSRA                  // for ATmega32
#define REG_ADMUX   ADMUX
#endif

//=============================================================================
// USB interface port and bits
#if RC_DESIGN
#define USB_CFG_IOPORTNAME          D
#define USB_CFG_DMINUS_BIT          0
#define USB_CFG_DPLUS_BIT           1
#define usbDeviceConnect()
#define usbDeviceDisconnect()
#else
#define USB_CFG_IOPORTNAME          B
#define USB_CFG_DMINUS_BIT          0
#define USB_CFG_DPLUS_BIT           1
#define USB_CFG_PULLUP_IOPORTNAME   B
#define USB_CFG_PULLUP_BIT          4
#endif

//=============================================================================
// Boot loader support
#if RC_DESIGN
#define BOOT_SUPPORT_ENABLED    0           // do not use boot loader interface
#else
#define BOOT_SUPPORT_ENABLED    1           // use boot loader interface
#define BOOT_VIA_WATCHDOG       1           // use watchdog instead of jump to boot
#define BOOT_START_ADDRESS      0x7000      // address of boot loader entry point
#define BOOT_SWITCH_PORT_OUT    PORTD       // boot switch ports (drive low to jump)
#define BOOT_SWITCH_PORT_IN     PIND
#define BOOT_SWITCH_PORT_DD     DDRD
#define BOOT_SWITCH_BIT         3           // boot switch pin
#endif // RC_DESIGN

//=============================================================================
// Interface-specific options (must be processed before usbconfig.h)
#ifndef __ASSEMBLER__
#include "ppm.h"         // standard RC PPM decoder
#include "ppm_adv.h"         // advanced RC PPM decoder
#include "in_adc.h"         // advanced RC PPM decoder
#include "joystick.h"   // standard USB HID-compatible game controller (joystick)
#endif

#endif  // __OPTIONS_H_INCLUDED__
