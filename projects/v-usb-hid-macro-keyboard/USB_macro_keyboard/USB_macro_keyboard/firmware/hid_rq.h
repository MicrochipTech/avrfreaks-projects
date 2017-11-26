/*	Signal that are used by Linux OS to setup the device.
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

#ifndef HID_RQ_H_INCLUDED
#define HID_RQ_H_INCLUDED

/* This header is shared between the firmware and the host software.
*  It defines the USB request numbers (and optionally data types) used to
*  communicate between the host and the device.
*
*  GET is direction for main procedure device->host
*  SET is direction for main procedure host->device
*/


#define VENDOR_RQ_GET_DATA          0x11
// Send keystrokes stored at eeprom memory data block No. "rq->wValue.bytes[0]"
// This address has to be smaller than "(uint8_t)DATA_COUNT"


#define VENDOR_RQ_SET_BULK          0x12
// Store data in device
// Receiving data Buffer[]. Buffer[0] is storage address of eeprom block (smaller than "(uint8_t)DATA_COUNT").
// Buffer[0]==0xFF means storage in "eeLockConfig".
// Received data musn't be larger than "BUFFER_SIZE" and has to be NULL terminated


#define VENDOR_RQ_PRINT_MEMORY      0x13
//Read all defined eeprom memory - config block and each key memory blocks
//Convert it to ASCII + #xx values and send it over usb as key-press
//Requires to capture it on some text editor


#define VENDOR_RQ_MCU_RESET         0xFF
// Enter infinite loop to reset device with WDT


//LED flashing - it is not related to keyboard lock-LEDs (those are only in software)
#define VENDOR_RQ_GET_LED_STATUS    0xF0
//Read current state of hardware LED
// Get the current LED status. Control-IN.
// This control transfer involves a 1 byte data phase where the device sends
// the current status to the host. The status is in bit 0 of the byte.

#define VENDOR_RQ_SET_LED_STATUS    0xF1
//Set status of hardware LED
// Set the LED status. Control-OUT.
// The requested status is passed in the "wValue" field of the control
// transfer. No OUT data is sent. Bit 0 of the low byte of wValue controls
// the LED.

#endif // HID_RQ_H_INCLUDED
