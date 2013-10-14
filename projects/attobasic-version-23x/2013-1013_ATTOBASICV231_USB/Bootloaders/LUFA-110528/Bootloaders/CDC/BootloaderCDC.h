/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2010  Dean Camera (dean [at] fourwalledcubicle [dot] com)

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
*/

/** \file
 *
 *  Header file for BootloaderCDC.c.
 */

#ifndef _CDC_H_
#define _CDC_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/boot.h>
		#include <avr/eeprom.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <stdbool.h>

		#include "morepgmspace.h"
		#include "Descriptors.h"
		#include "bootldr.h"
		#include "loader.h"
		#include "des.h"
		#include "crc.h"

		#include <LUFA/Drivers/USB/USB.h>

	/* Macros: */
		/** CDC Class specific request to get the current virtual serial port configuration settings. */
		#define REQ_GetLineEncoding          0x21

		/** CDC Class specific request to set the current virtual serial port configuration settings. */
		#define REQ_SetLineEncoding          0x20

		/** CDC Class specific request to set the current virtual serial port handshake line states. */
		#define REQ_SetControlLineState      0x22

		/** Type define for a non-returning pointer to the start of the loaded application in flash memory. */
		typedef void (*AppPtr_t)(void) ATTR_NO_RETURN;

	/* Enums: */
		/** Enum for the possible line encoding formats of a virtual serial port. */
		enum CDCDevice_CDC_LineCodingFormats_t
		{
			OneStopBit          = 0, /**< Each frame contains one stop bit */
			OneAndAHalfStopBits = 1, /**< Each frame contains one and a half stop bits */
			TwoStopBits         = 2, /**< Each frame contains two stop bits */
		};

		/** Enum for the possible line encoding parity settings of a virtual serial port. */
		enum CDCDevice_LineCodingParity_t
		{
			Parity_None         = 0, /**< No parity bit mode on each frame */
			Parity_Odd          = 1, /**< Odd parity bit mode on each frame */
			Parity_Even         = 2, /**< Even parity bit mode on each frame */
			Parity_Mark         = 3, /**< Mark parity bit mode on each frame */
			Parity_Space        = 4, /**< Space parity bit mode on each frame */
		};

	/* Function Prototypes: */
		void CDC_Task(void);
		void SetupHardware(void);

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);
#endif

