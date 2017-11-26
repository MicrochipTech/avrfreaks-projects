/*	Main configuration file - can disable/alter some function to minimize code size.
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

#ifndef MAIN_CONFIG_H_INCLUDED
#define MAIN_CONFIG_H_INCLUDED

//#include <avr/io.h>

//Enable printing function over USB keyboard protocol-------------------------------------
#define PRINT_ENABLED               1

//Enable LED control----------------------------------------------------------------------
#define HWLED_CONNECTED             1           //Hardware LED is connected to indicate device's operation

#if HWLED_CONNECTED == 1    //Do not change this condition
#define HWLED_OPERATION_INVERTED    0			//0:normaly OFF, turns ON when sending data; 1:inverted; Can be also useful when LED is connected in oposit direction
#define HWLED_CONTROL_LED           1           //Enable hardware LED control through USB protocol
//Hardware connection od LED
#define HWLED_DDR                   DDRB
#define HWLED_PORT                  PORTB
#define HWLED_P                     PB0
#endif // HWLED_CONNECTED


//Custom defines for device memory storage------------------------------------------------
//Prepare data storage in eeprom (DATA) with corresponding generic buffer (it should not be smaller than 10 since it is also used for configuration data -> risk of accessing memory outside buffer)
#define DATA_COUNT                  8                  //Number of data blocks in eeprom
#define DATA_MAX_SIZE               62                  //Length of a single eeprom memory block (DATA_MAX_SIZE=<BUFFER_SIZE) - minimal size is 9
#define BUFFER_SIZE                 (DATA_MAX_SIZE+1)   //Generic buffer for data handling (+1 means 1st byte is storage address where the incoming message from host should be stored)

//Compile flags - which keys should be included 1: use, 0: don't use aditional keys
#define KEYS_LETTERS_ALL            1       //a-z
#define KEYS_LETTERS_CAPITAL_ALL    1       //A-Z
#define KEYS_NUMBERS_ALL            1       //0-9
#define KEYS_ASCII_SOME             1       //# / _ @
#define KEYS_ASCII_ALL              1       //ASCII_SOME has to be 1 for this to have an effect
#define KEYS_CONTROL_ALL            1       //arrow key, pgUp, pgDown,...
#define KEYS_FKEYS_ALL              1       //F1-F12
#define KEYS_CZ_KEYS_ALL            1       //Ability to use ˇ and ' for ěščřžýáíé (has to be send as sequence ˇz to create ž)
#define KEYS_HOST_OS                1       //Specify for which OS this device is used - valid values: 0:LINUX, 1:WINDOWS - see end of hid.c file


#endif // MAIN_CONFIG_H_INCLUDED
