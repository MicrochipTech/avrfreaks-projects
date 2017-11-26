/*	Main Macro-keyboard procedures.
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

/*Fuses: // You should set fuses in Makefile
hfuse 0xD4
lfuse 0xDF
efuse 0xFF	//Default value - doesn't have to be set
*/

/*----------------------*/
/*--Includes------------*/
/*----------------------*/

#include <avr/io.h>
#include <avr/wdt.h>        //watch dog timer for handling CPU freezes
#include <avr/interrupt.h>  //for sei() an all interrupt driven events
#include <util/delay.h>     //for _delay_ms()
#include <avr/sleep.h>      //Power saving mode

#include <avr/eeprom.h>     //store data string (password)
#include <avr/pgmspace.h>
#include <string.h>

#include <stdlib.h>         //for itoa()

#include "main.h"
#include "usbdrv.h"         //main v-usb interface
#include "timer.h"          //50Hz delay routine and "USB is alive" protocol

#include "hid.h"            //HID keyboard interface - also contains some declared global variables

/*----------------------*/
/*--Global storage------*/
/*----------------------*/

//Global flags
volatile uint8_t flag1 = 0;                 //Global flag1 that determines main function
volatile uint8_t flag2 = 0;                 //Global flag1 that determines main function
lock_struct_t lockConfig;                   //Structure holding information for lock-LED send msg trigger event

//Custom defines for device memory storage
//Prepare data storage
#if BUFFER_SIZE>64
#error "Maximal buffer size is 63+NULL (there may be limit on USB transfer protocol - not confirmed)"
#endif // BUFFER_SIZE
//Allocating memory space for passwords - can be also done in a single struct
//Each block has to be NULL terminated string of uint8_t
EEMEM uint8_t eeData[DATA_COUNT][DATA_MAX_SIZE] = {{0}};

//Num-lock trigger event configuration data
EEMEM lock_struct_t eeLockConfig = {0xFF, 3, 3, {1, 1}, {1, 1}, 50, 50}; //{reserved==0xFF(eeprom default value), triggerCount is compared alway as > XY (virtualy adds 1), triggerCountTimeout (1s), dataSendDelay (1s)}

//Generic global data buffer
uint8_t buffer[BUFFER_SIZE] = {0};


#if PRINT_ENABLED == 1
PROGMEM const char prtCfg0[] = {"Config"};
PROGMEM const char prtCfg1[] = {"__Count_NumLock_trigger___"};
PROGMEM const char prtCfg2[] = {"__Count_CapsLock_trigger___"};
PROGMEM const char prtCfg3[] = {"__Count_NumCaps_trigger___"};
PROGMEM const char prtCfg4[] = {"__Count_CapsNum_trigger___"};
PROGMEM const char prtCfg5[] = {"__Counter_timeout___"};
PROGMEM const char prtCfg6[] = {"__Delay_after_trigger___"};
PROGMEM const char prtMem[]  = {"__Memory_block___"};
#endif // PRINT_ENABLED


/*----------------------*/
/*--- Main functions ---*/
/*----------------------*/
int main(void)
{
    #if HWLED_CONNECTED == 1
    initHwLed();                        //Set default state of hardware indicator LED
    #endif // HWLED_CONNECTED

	wdt_enable(WDTO_2S);                //Enable watchdog for 1s intervals - resets are called in Timer1 ISR
	set_sleep_mode(SLEEP_MODE_IDLE);    //Need timer1 interrupt

	keyboardReportReset();              //Define initial values for keyboard report
	usbInit();                          //Initialize USB protocols

	usbDeviceDisconnect();              //Enforce re-enumeration, do this while interrupts are disabled!
	uint8_t timeOut = 30;

	while(--timeOut) {                  //Fake USB disconnect for > 250 ms
		wdt_reset();                    //Prevent reset from watchdog
		_delay_ms(10);
	}

	usbDeviceConnect();

	timerInit();                        //USB polling and timeout handling
	sei();
	//All above code has to be within watch-dog timer. It shouldn't be a problem at 12 MHz clock and 1s of WDT.

	eeprom_read_block(&lockConfig, &eeLockConfig, sizeof(lock_struct_t));

	// main event loop
	while(1) {

		//Send data block from memory 0 of eeprom. Trigger is switching several times Num-lock key (or rather LED change signal)
		if(lockChangesNumNo > (lockConfig.triggerCountNum)) {
			dataSend(0);

			//Send data block from memory 1 of eeprom. Trigger is switching several times Caps-lock key (or rather LED change signal)
		} else if(lockChangesCapsNo > (lockConfig.triggerCountCaps)) {
			dataSend(1);

			//Send data block from memory 2 of eeprom. Trigger is switching several times Num-lock then Caps-lock key (or rather LED change signal)
		} else if((lockChangesNumNo > (lockConfig.triggerCountNumCaps[0])) &&
		          (lockChangesCapsNo > (lockConfig.triggerCountNumCaps[1])) &&
		          (flag2 & FLG2_NUM_CAPS_ORDER)) {                            //1: Num lock sequence was first - don't change order of "else if" blocks or it may be broken
			dataSend(2);

			//Send data block from memory 3 of eeprom. Trigger is switching several times Caps-lock then Num-lock key (or rather LED change signal)
		} else if((lockChangesCapsNo > (lockConfig.triggerCountCapsNum[0])) &&
		          (lockChangesNumNo > (lockConfig.triggerCountCapsNum[1])) &&
		          !(flag2 & FLG2_NUM_CAPS_ORDER)) {                           //0: Caps lock sequence was first
			dataSend(3);

			//Send a data block from eeprom based on received signal + address (vendor signal over USB)
		} else if(flag1 & FLG1_SEND_DATA_TRIGGER) {
			dataSend(flag1 & FLG1_SEND_DATA_ADDRESS);

			flag1 &= ~FLG1_SEND_DATA_TRIGGER;

			//Store received data into eeprom on given address
		} else if(flag1 & FLG1_BULK_DATA_RECEIVED) {
			dataStore();
			flag1 &= ~FLG1_BULK_DATA_RECEIVED;

#if PRINT_ENABLED ==1
		} else if(flag2 & FLG2_PRINT_MEMORY) {
			delayTimer1(0xFF);      //Long delay to allow user to switch into text editor to receive message    //TODO - may be changed in future
			printMemory();
			flag2 &= ~(FLG2_PRINT_MEMORY);
#endif // PRINT_ENABLED

		} else {

			//Do nothing if no control flag is set - wait for next cycle of timer1
			wdt_reset();            //Reset WDT to indicate that the device is still alive
			sleep_mode();           //Will be waked by Timer1 interrupt
		}
	}
}


//Software reset
void mcuReset()
{
	cli();          //Disable interupts - do not interfere with reseting MCU

	while(1) {};    //Enter infinite loop and wait for watchdog to reach its reset condition
}


//Decypher data stored in eeprom
uint8_t parseKeyBuffer(uint8_t* buf, const uint8_t maxLen)
{
	lockDefault();                                                  //Set num- caps-lock to value ON, OFF respectively

	uint8_t cntr;                                                   //Generic loop counter

	for(cntr = 0; cntr < maxLen; ++cntr) {
        wdt_reset();                                                //Reset WDT for possibly long cycle

		if(buf[cntr] == 0x00) {
			return 0x00;                                            //Proper return - NULL terminated buffer

		} else if(buf[cntr] < 128) {                                //Control characters 1-31, Ascii "visible" characters 32-127
			sendKeyPress(buf[cntr]);

		} else {                         //Special "undefined" Ascii characters - used for device handling (e.g. delays between key presses - stored in eeprom data)
			//Warning! - collision with other signals may happen if they are implemented in range 0x80-0xFF

			switch(buf[cntr]) {
				case DELAY_STR_100MS:
					delayTimer1((uint8_t)(F_TIMER1 * 1 / 10));      //100ms delay with 50Hz timer
					break;

				case DELAY_STR_500MS:
					delayTimer1((uint8_t)(F_TIMER1 * 5 / 10));      //500ms delay with 50Hz timer
					break;

				case DELAY_STR_1000MS:
					delayTimer1((uint8_t)(F_TIMER1 * 1));           //1000ms delay with 50Hz timer
					break;

				case DELAY_STR_2000MS:
					delayTimer1((uint8_t)(F_TIMER1 * 2));           //2000ms delay with 50Hz timer
					break;

				case DELAY_STR_3000MS:
					delayTimer1((uint8_t)(F_TIMER1 * 3));           //3000ms delay with 50Hz timer
					break;
			}
		}
	}

	return 0x01;       //Not a NULL terminated buffer - error
}


//Send data from eeprom to USB pipe
void dataSend(const uint8_t address)
{
	#if HWLED_CONNECTED == 1
	hwLedOn();
	#endif // HWLED_CONNECTED

	delayTimer1(lockConfig.dataSendDelay);                          //Add delay - user stops toggling lock-LEDs and HID driver hooks up back to the device

	eeprom_read_block(buffer, eeData[address], DATA_MAX_SIZE);      //Read - data block 0 from eeprom - only one block avalable for num lock trigger

	if(parseKeyBuffer(buffer, DATA_MAX_SIZE)) {                     //Send content of buffer to HID keyboad interface
		mcuReset();
	}

	lockChangesNumNo = 0;
	lockChangesCapsNo = 0;
//  lockTimeout = 0;                                                //Can be left on ISR to reset it back to 0. Saves 4 bytes of flash memory

    #if HWLED_CONNECTED == 1
    hwLedOff();
    #endif // HWLED_CONNECTED
}


//Store received data to proper eeprom memory location
void dataStore()
{
	#if HWLED_CONNECTED == 1
	hwLedOn();
	#endif // HWLED_CONNECTED

	if(buffer[0] == 0xFF) {                 //Store lock key trigger configuration data
		lockConfig.triggerCountNum          = buffer[1];
		lockConfig.triggerCountCaps         = buffer[2];
		lockConfig.triggerCountNumCaps[0]   = buffer[3];
		lockConfig.triggerCountNumCaps[1]   = buffer[4];
		lockConfig.triggerCountCapsNum[0]   = buffer[5];
		lockConfig.triggerCountCapsNum[1]   = buffer[6];
		lockConfig.triggerCountTimeout      = buffer[7];
		lockConfig.dataSendDelay            = buffer[8];

		eeprom_update_block(&lockConfig, &eeLockConfig, sizeof(lock_struct_t));

		mcuReset();                         //Reset device so that new setup is used

	} else if(buffer[0] < DATA_COUNT) {      //Check for valid eemem address
		eeprom_update_block((buffer + 1), eeData[buffer[0]], DATA_MAX_SIZE); //+1 means that first possition of buffer stores information on memory location
	}

	#if HWLED_CONNECTED == 1
	hwLedOff();
	#endif // HWLED_CONNECTED
}


//Printing functions-----------
#if PRINT_ENABLED == 1
void printMemory()
{
	#if HWLED_CONNECTED == 1
	hwLedOn();
	#endif // HWLED_CONNECTED

	uint8_t iii;                                //Generic loop iterator
	uint8_t jjj;                                //Generic loop iterator

	printConfig(prtCfg0, 0, 0);
	printConfig(prtCfg1, &lockConfig.triggerCountNum, 1);
	printConfig(prtCfg2, &lockConfig.triggerCountCaps, 1);
	printConfig(prtCfg3, &lockConfig.triggerCountNumCaps[0], 2);
	printConfig(prtCfg4, &lockConfig.triggerCountCapsNum[0], 2);
	printConfig(prtCfg5, (uint8_t*)&lockConfig.triggerCountTimeout, 1);
	printConfig(prtCfg6, &lockConfig.dataSendDelay, 1);

	//Print all data eeprom memory blocks
	for(iii = 0; iii < DATA_COUNT; ++iii) {

		sendKeyPress(KEY_RETURN);
		strcpy_P((char*)buffer, prtMem);
		printConvertData(buffer, strlen((char*)buffer), 1);

		decKey(iii, buffer);

		for(jjj = 0; jjj < 3; ++jjj) {
			sendKeyPress(buffer[jjj]);
		}

		sendKeyPress(KEY_RETURN);
		eeprom_read_block(buffer, eeData[iii], DATA_MAX_SIZE);

		printConvertData(buffer, DATA_MAX_SIZE, 1);
		sendKeyPress(KEY_RETURN);
		printConvertData(buffer, DATA_MAX_SIZE, 0);
		sendKeyPress(KEY_RETURN);
	}

	#if HWLED_CONNECTED == 1
	hwLedOff();
	#endif // HWLED_CONNECTED
}

//Envelope function for formating printing function regarding Config eeprom data block
void printConfig(PGM_P pDes, uint8_t* data, uint8_t dataLen)
{
	strcpy_P((char*)buffer, pDes);
	printConvertData(buffer, strlen((char*)buffer), 1);

	if(dataLen) {
		printConvertData(data, dataLen, 0);
	}

	sendKeyPress(KEY_RETURN);
}


void printConvertData(const uint8_t* dataBlock, uint8_t len, uint8_t ascii)
{
#define _TEMP_BUF_SIZE      4       //Has to be 4 or larger (3 digit+null terminator)
	uint8_t iii;
	uint8_t jjj;
	uint8_t tempBuf[_TEMP_BUF_SIZE] = {0};

	for(iii = 0; iii < len; ++iii) {
        wdt_reset();                            //Reset WDT for possibly long cycle

		if((ascii == 1) &&
		   (dataBlock[iii] < 0x7F) &&
		   (dataBlock[iii] > 0x20)) {
			sendKeyPress(dataBlock[iii]);       //Print ascii if possible
		} else if((ascii == 1) &&
		          (dataBlock[iii] == 0x00)) {
			return;                             //In ascii mode end at null termination
		} else {
			hexKey(dataBlock[iii], tempBuf);

			for(jjj = 0; jjj < _TEMP_BUF_SIZE; ++jjj) {
				sendKeyPress(tempBuf[jjj]);
			}
		}
	}

#undef _TEMP_BUF_SIZE
}


void decKey(const uint8_t data, uint8_t* buf)
{
#if KEYS_NUMBERS_ALL != 1
#error "This function requires numbers to be defined in keyboard code."
#endif // KEYS_NUMBERS_ALL

	if(data >= 100) {
		utoa(data, (char*)&buf[0], 10);
	} else if(data >= 10) {
		utoa(data, (char*)&buf[1], 10);
		buf[0] = '0';
	} else {
		utoa(data, (char*)&buf[2], 10);
		buf[0] = '0';
		buf[1] = '0';
	}
}


void hexKey(const uint8_t data, uint8_t* buf)
{
#if KEYS_NUMBERS_ALL != 1
#error "This function requires numbers to be defined in keyboard code."
#endif // KEYS_NUMBERS_ALL

#if KEYS_LETTERS_ALL != 1
#error "This function requires lowercase letters to be defined in keyboard code."
#endif // KEYS_NUMBERS_ALL

#if KEYS_ASCII_SOME != 1
#error "This function requires special ASCII character to be defined in keyboard code."
#endif // KEYS_NUMBERS_ALL

	buf[0] = '#';

	if(data > 0x0F) {
		utoa(data, (char*)&buf[1], 16);
	} else {
		utoa(data, (char*)&buf[2], 16);
		buf[1] = '0';
	}
}
#endif // PRINT_ENABLED
