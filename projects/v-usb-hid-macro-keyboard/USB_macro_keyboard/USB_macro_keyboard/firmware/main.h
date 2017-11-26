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

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include "main_config.h"

/*----------------------*/
/*--Defines-------------*/
/*----------------------*/

//Flag1 masks - LED status + eeprom read
#define FLG1_SEND_DATA_ADDRESS      0x0F        //Password address of eeprom memory block mask
#define FLG1_SEND_DATA_TRIGGER      0x80        //1: Send password from FLG1_SEND_DATA_ADDRESS on next main loop
#define FLG1_LED_CHANGE             0x40        //1: Indicate to usbFunctionWrite that receiving data are connected to LED Lock change
#define FLG1_BULK_TRANSFER_ACTIVE   0x20        //Data are currently downloading into the buffer
#define FLG1_BULK_DATA_RECEIVED     0x10        //Data are in buffer waiting to be processed

//Flag2 masks - transfer status
#define FLG2_NUM_CAPS_ORDER         0x01        //If num-lock was pressed before caps-lock set to 1, else 0
#define FLG2_PRINT_MEMORY           0x02        //Print eemem through keyboard interface in human readable format

#if DATA_COUNT < 4
#error "Data_count is less than 4 which is minimal size to utilize Num- Caps-lock sensing procedures in main() loop. Risk of undefined behavior in eeprom access."
#endif // DATA_COUNT

/*----------------------*/
/*--Prototypes----------*/
/*----------------------*/
//Enter infinite loop and wait for watchdog timer to reset MCU
void mcuReset(void);

//Use hardware led to signalize operation of the device - it is not related to keyboard lock-LEDs
#if HWLED_CONNECTED == 1
#if HWLED_OPERATION_INVERTED == 0
static inline void initHwLed()      //Set default values to OFF output
{
	HWLED_PORT &= ~(1 << HWLED_P);
	HWLED_DDR |= 1 << HWLED_P;
}
static inline void hwLedOn() {HWLED_PORT |= (1 << HWLED_P);}
static inline void hwLedOff() {HWLED_PORT &= ~(1 << HWLED_P);}
#else
static inline void initHwLed()      //Set default values to OFF output
{
	HWLED_PORT |= 1 << HWLED_P;
	HWLED_DDR |= 1 << HWLED_P;
}
static inline void hwLedOn() {HWLED_PORT &= ~(1 << HWLED_P);}
static inline void hwLedOff() {HWLED_PORT |= (1 << HWLED_P);}
#endif // HWLED_OPERATION_INVERTED
#endif // HWLED_CONNECTED


//Parse values in buffer (of "maxLen" size) and send corresponding key press(-es) to USB HID keyboard interface
//Also implements special functions for buf[] values in range 0x80-0xFF -> so for example a delay can be stored in eeprom data block
//Return 0 success; 1 if the buffer is not properly NULL terminated in the memory
uint8_t parseKeyBuffer(uint8_t* buf, const uint8_t maxLen);

//Function for reading eeprom and sending it to USB keyboard pipe; require address of eeprom data block
void dataSend(const uint8_t address);

//Store received data to proper eeprom memory location
void dataStore(void);

#if PRINT_ENABLED == 1
//Print whole eeprm memory
void printMemory(void);
//Envelope function for formating printing function regarding Config eeprom data block
void printConfig(PGM_P pDes, uint8_t* data, uint8_t dataLen);
//Convert data from memory buffer into printable format; ascii==0: print all in hex format, 1: print printable characters + non-printable hex values
void printConvertData(const uint8_t* dataBlock, uint8_t len, uint8_t ascii);
//Convert uint8_t value into decimal value in 3 digit format 000
void decKey(const uint8_t data, uint8_t* buf);
//Convert uint8_t value into hex value in 3 digit format #00
void hexKey(const uint8_t data, uint8_t* buf);
#endif // PRINT_ENABLED

/*----------------------*/
/*--Shared global var.--*/
/*----------------------*/
extern uint8_t buffer[];            //Generic buffer for data handling

extern volatile uint8_t flag1;      //Global flag1 that determines main function steps
extern volatile uint8_t flag2;      //Global flag2 that determines main function steps

typedef struct {
	uint8_t reserved;                       //TODO - dummy memory allocation at location 0 - prevent data corruption when address pointer of eemem gets damaged - maybe not necessary at all (also check eemem file if it is at the first position)
	uint8_t triggerCountNum;                //Requred number to trigger data 0 only by toggling Numlock
	uint8_t triggerCountCaps;               //Requred number to trigger data 1 only by toggling Capslock
	uint8_t triggerCountNumCaps[2];         //Requred number to trigger data 2 by toggling Numlock [0] then Capslock [1]
	uint8_t triggerCountCapsNum[2];         //Requred number to trigger data 3 by toggling Capslock [0] then Numlock [1]
	volatile uint8_t triggerCountTimeout;   //Timeout between each lockkey press
	uint8_t dataSendDelay;          //Delay for user to stop pressing keys then wait this many timer1 loop then process dataN
} lock_struct_t;

extern lock_struct_t lockConfig;       //Structure holding information for Num-lock send msg trigger event

#endif // MAIN_H_INCLUDED
