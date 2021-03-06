/**************************************************************

             LUFA Library
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.

***************************************************************

	IRmega USB Infrared Receiver/Transmitter
             firmware for MCU ATmega32U4

  Based on the Dean Camera's LUFA VirtualSerial.c demo

            (C) 2011 Anthony Vanek

    See the attached HTML document for details.

  SIMPLIFIED VERSION WITH UNMODULATED OUTPUT ONLY
            AND HEXADECIMAL DATA CODING

***************************************************************

/* Header file for InfraRed.c */

#ifndef _INFRARED_H_
#define _INFRARED_H_

	/* Includes: */

		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <avr/eeprom.h>

		#include "Descriptors.h"

		#include <LUFA/Version.h>
		#include <LUFA/Drivers/USB/USB.h>
		#include <LUFA/Drivers/Board/LEDs.h>

	/* Function Prototypes: */

		void SetupHardware(void);
		void CDC_Task(void);

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_Disconnect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);
#endif

