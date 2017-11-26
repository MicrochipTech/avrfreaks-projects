/*
*   Copyright (C) 2017  Ondra Vlk
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef HID_H_INCLUDED
#define HID_H_INCLUDED

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

#include <avr/pgmspace.h>   //required by usb_hid_report_descriptor
#include "usbdrv.h"         //v-usb interface
#include "main_config.h"
#include "main.h"
#include "hid_rq.h"         //Vendor specific requests
#include "hid_keys.h"       //Defines for keyboard layout and commands


// Prototypes
void atok(uint8_t ascii);           //ASCII to keyboard_report_t -> store in global variable
void sendKeyPress(uint8_t ascii);   //Send single pres and release key signal - call from main modul instead of sendReport

void sendReport();                  //Flush value of keyboardReport to USB stream. This function should not be directly called from main - use sendKeyPress
void lockDefault();                 //Set default locks - Caps-lock off, Numlock on -> keypress afterwards will provide correct values

// Other prototypes are already in usbdrv.h

// data structure for boot protocol keyboard report
// see HID1_11.pdf appendix B section 1
typedef struct {
	uint8_t modifier;
	uint8_t reserved;
	uint8_t keycode[6];
} keyboard_report_t;

// Global variables
extern keyboard_report_t keyboardReport;
// Reset report structure to default values
#define keyboardReportReset()               \
            keyboardReport.modifier=0;      \
            keyboardReport.reserved=0;      \
            keyboardReport.keycode[0]=0;    \
            keyboardReport.keycode[1]=0;    \
            keyboardReport.keycode[2]=0;    \
            keyboardReport.keycode[3]=0;    \
            keyboardReport.keycode[4]=0;    \
            keyboardReport.keycode[5]=0;

// HID related global variables
extern uint8_t idleRate;            //see HID1_11.pdf sect 7.2.4
extern uint8_t protocolVersion;     //see HID1_11.pdf sect 7.2.6
extern uint8_t ledState;            //see HID1_11.pdf appendix B section 1

// See http://vusb.wikidot.com/driver-api
// Constants are found in usbdrv.h
// Modifier keys masks
#define KB_SHIFT_LEFT       0x02
#define KB_SHIFT_RIGHT      0x20
#define KB_ALT_LEFT         0x04
#define KB_ALT_RIGHT        0x40
#define KB_CTRL_LEFT        0x01
#define KB_CTRL_RIGHT       0x10
#define KB_GUI_LEFT         0x08    //??? - probably left "Window key"
#define KB_GUI_RIGHT        0x80    //??? - probably right "Window key"
#define KB_ALTGR            (KB_ALT_RIGHT)      //Special modifier key 3rd level

// LED definition in HID report
#define LEDSTATE_NUM_LOCK           0x01        //1: Num ON; value has to corelate with LED state num position
#define LEDSTATE_CAPS_LOCK          0x02        //1: Caps ON; value has to corelate with LED state caps position
#define LEDSTATE_LOCKS_MASK         (LEDSTATE_CAPS_LOCK | LEDSTATE_NUM_LOCK)

// Num-lock sensing global variables
extern volatile uint8_t lockTimeout;         //Tracking if Num lock was pressed within certain time period
extern volatile uint8_t lockChangesNumNo;       //Tracking how many times Num-lock was pressed
extern volatile uint8_t lockChangesCapsNo;       //Tracking how many times Caps-lock was pressed

#endif // HID_H_INCLUDED
