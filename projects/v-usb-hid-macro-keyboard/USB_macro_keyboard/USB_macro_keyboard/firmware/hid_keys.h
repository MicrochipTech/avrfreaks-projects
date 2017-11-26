/*	Listing of non Ascii codes that can be stored in eeprom and used for device operation.
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

#ifndef HID_KEYS_H_INCLUDED
#define HID_KEYS_H_INCLUDED

/* This header is shared between the firmware and the host software.
*
*  This header contains key mapping for various control
*/

#include "main_config.h"

//Keyboard LOCK (and therefore LED) manipulation
#define KEY_NUM_LOCK        0x1D
#define KEY_CAPS_LOCK       0x1E

//Keyboard "symbolless" characters
#define KEY_DEL             0x7F
#define KEY_SPACE           0x20
//Special "keypress" characters
#define KEY_NULL            0x00    //Null termination of password buffer
#define KEY_RETURN          0x01    //CR or CRLF depending on OS - in this case it is only press return key once
#define KEY_ESC             0x02
#define KEY_BACKSPACE       0x03
#define KEY_TAB_HORIZONTAL  0x04
#define KEY_WINDOWS         0x05    //Left_Gui_key

//Keys for navigation
#define KEY_ARROW_UP        0x06
#define KEY_ARROW_DOWN      0x07
#define KEY_ARROW_LEFT      0x08
#define KEY_ARROW_RIGHT     0x09
#define KEY_PGUP            0x0A
#define KEY_PGDOWN          0x0B
#define KEY_HOME            0x0C
#define KEY_END             0x0D
#define KEY_INSERT          0x0E

//Special CZ modification ' ˇ
#define KEY_CARKA           0x0F
#define KEY_HACEK           0x10

//F-Keys
#define KEY_F1              0x11
#define KEY_F2              0x12
#define KEY_F3              0x13
#define KEY_F4              0x14
#define KEY_F5              0x15
#define KEY_F6              0x16
#define KEY_F7              0x17
#define KEY_F8              0x18
#define KEY_F9              0x19
#define KEY_F10             0x1A
#define KEY_F11             0x1B
#define KEY_F12             0x1C

//Send Ctrl+Alt+Del combination
#define KEY_CTRL_ALT_DEL    0x1F


//KEY values 0x80-0xFF are not defined for key press - used in device main for defined delay loops.
//This may be changed but care has to be taken not to implement value collisions.
//Device's main is defined from 0xFF down - better to use for KEY from 0x80 up

//Delays
//Delay between keypress - also stored in password string in eeprom
#define DELAY_STR_100MS     0xff
#define DELAY_STR_500MS     0xfe
#define DELAY_STR_1000MS    0xfd
#define DELAY_STR_2000MS    0xfc
#define DELAY_STR_3000MS    0xfb

#endif // HID_KEYS_H_INCLUDED
