/*	This is the main file implementing USB-HID functionality.
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

#include "hid.h"

#include <util/delay.h>

// This is the same report descriptor as in USB HID documentation for boot-keyboard
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
	0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	0x09, 0x06,                    // USAGE (Keyboard)
	0xA1, 0x01,                    // COLLECTION (Application)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x95, 0x08,                    //   REPORT_COUNT (8)
	0x05, 0x07,                    //   USAGE_PAGE (Keyboard)(Key Codes)
	0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)(224)
	0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)(231)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	0x81, 0x02,                    //   INPUT (Data,Var,Abs) ; Modifier byte
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x81, 0x03,                    //   INPUT (Cnst,Var,Abs) ; Reserved byte
	0x95, 0x05,                    //   REPORT_COUNT (5)
	0x75, 0x01,                    //   REPORT_SIZE (1)
	0x05, 0x08,                    //   USAGE_PAGE (LEDs)
	0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
	0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
	0x91, 0x02,                    //   OUTPUT (Data,Var,Abs) ; LED report
	0x95, 0x01,                    //   REPORT_COUNT (1)
	0x75, 0x03,                    //   REPORT_SIZE (3)
	0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs) ; LED report padding
	0x95, 0x06,                    //   REPORT_COUNT (6)
	0x75, 0x08,                    //   REPORT_SIZE (8)
	0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)                   //? - in some documentations this is 255 for unknown reason (this is corresponding to 104-layout boot keyboard)
	0x05, 0x07,                    //   USAGE_PAGE (Keyboard)(Key Codes)
	0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))(0)
	0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)(101)
	0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
	0xC0                           // END_COLLECTION
};

// Global variables
keyboard_report_t keyboardReport;

//HID related global variables
uint8_t idleRate = (500 / 4);   // see HID1_11.pdf sect 7.2.4
uint8_t protocolVersion = 0;    // see HID1_11.pdf sect 7.2.6
uint8_t ledState = 0;           // see HID1_11.pdf appendix B section 1

//Lock-key sensing
volatile uint8_t lockTimeout = 0;
volatile uint8_t lockChangesNumNo = 0;
volatile uint8_t lockChangesCapsNo = 0;

//Bulk data transfer
uint8_t bulkBytesRemaining = 0;
uint8_t bulkBufferPointer = 0;


// see http://vusb.wikidot.com/driver-api
// constants are found in usbdrv.h
usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
	// see HID1_11.pdf sect 7.2 and http://vusb.wikidot.com/driver-api
	usbRequest_t* rq = (void*)data;

	//HID keyboard class specific commands - set/get is relative to host
	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) { // ignore request if it's not a class specific request - in this case keyboard
		// see HID1_11.pdf sect 7.2
		switch(rq->bRequest) {
			case USBRQ_HID_GET_IDLE:
				usbMsgPtr = (usbMsgPtr_t)&idleRate; // send data starting from this byte
				return 1; // send 1 byte

			case USBRQ_HID_SET_IDLE:
				idleRate = rq->wValue.bytes[1]; // read in idle rate
				return 0; // send nothing

			case USBRQ_HID_GET_PROTOCOL:
				usbMsgPtr = (usbMsgPtr_t)&protocolVersion; // send data starting from this byte
				return 1; // send 1 byte

			case USBRQ_HID_SET_PROTOCOL:
				protocolVersion = rq->wValue.bytes[1];
				return 0; // send nothing

			case USBRQ_HID_GET_REPORT:
				usbMsgPtr = (usbMsgPtr_t)&keyboardReport; // send the report data
				return sizeof(keyboardReport);

			case USBRQ_HID_SET_REPORT:
				if(rq->wLength.word == 1) { // check data is available
					// 1 byte, we don't check report type (it can only be output or feature)
					// we never implemented "feature" reports so it can't be feature
					// so assume "output" reports
					// this means set LED status
					// since it's the only one in the descriptor
					flag1 |= FLG1_LED_CHANGE;   //Store the information in global flag
					return USB_NO_MSG; // send nothing but call usbFunctionWrite
				} else { // no data or do not understand data, ignore
					return 0; // send nothing
				}

			default: // do not understand data, ignore
				return 0; // send nothing
		}

		//Specific "vendor" commands - see "hid_rq.h"
	} else if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_VENDOR) {

		switch(rq->bRequest) {
			case VENDOR_RQ_GET_DATA:
				//In lower nibble store memory location and then set trigger flag for password sending in main loop
				flag1 = (flag1 & (~FLG1_SEND_DATA_ADDRESS)) | FLG1_SEND_DATA_TRIGGER | (rq->wValue.bytes[0] & FLG1_SEND_DATA_ADDRESS);
				return 0;
				break;

			case VENDOR_RQ_SET_BULK:
				flag1 |= FLG1_BULK_TRANSFER_ACTIVE;
				bulkBytesRemaining = (rq->wLength.bytes[0]);    //Limited to small data - no more than 256 expected (in fact no more than 64 expected)
				return USB_NO_MSG;
				break;

#if PRINT_ENABLED == 1

			case VENDOR_RQ_PRINT_MEMORY:
				flag2 |= FLG2_PRINT_MEMORY;
				break;
#endif // PRINT_ENABLED

			case VENDOR_RQ_MCU_RESET:
				mcuReset();
				break;

#if HWLED_CONTROL_LED == 1
			case VENDOR_RQ_SET_LED_STATUS:
				if(rq->wValue.bytes[0] & 0x01) {        //set LED
                    hwLedOn();
				} else {                                //clear LED
					hwLedOff();
				}
                return 0;
				break;

			case VENDOR_RQ_GET_LED_STATUS:
				;
				static uchar dataBuffer[1];     // buffer must stay valid when usbFunctionSetup returns
				dataBuffer[0] = ((HWLED_PORT & (1<<HWLED_P)) != 0);
				usbMsgPtr = (usbMsgPtr_t)dataBuffer;         // tell the driver which data to return
				return 1;                       // tell the driver to send 1 byte
				break;
#endif // CONTROL_LED

			default: // do not understand data, ignore
				return 0; // send nothing
		}
	}

	return 0;
}

usbMsgLen_t usbFunctionWrite(uint8_t* data, uchar len)
{
	//Store Lock-LEDs values (num-lock, caps-lock and scroll-lock)
	//Also detect if num-lock changed - this is used for sending password 0 in BIOS mode when "host-vendor" application is not yet running

	if((flag1 & FLG1_BULK_TRANSFER_ACTIVE) && len) {    //Bulk transfer

		if(bulkBytesRemaining == 0) {
			bulkBufferPointer = 0;
			flag1 &= ~(FLG1_BULK_TRANSFER_ACTIVE | FLG1_BULK_DATA_RECEIVED);      //Reset flags - no data received, nothing should run in main loop
			return 0x01;                                //Everything received - or rather nothing received since nothing was send
		}

		if(len > bulkBytesRemaining) {                  //Message shorter than len-block
			len = bulkBytesRemaining;
		}

		uint8_t cntr = 0;

		for(cntr = 0; cntr < len; ++cntr) {
			buffer[cntr + bulkBufferPointer] = data[cntr];
		}

		bulkBufferPointer += len;
		bulkBytesRemaining -= len;

		if(bulkBytesRemaining == 0) {   //TODO - duplicity/complex - mayb there is simplier code structure but this also work
			bulkBufferPointer = 0;
			flag1 &= ~FLG1_BULK_TRANSFER_ACTIVE;        //Bulk transfer comleted
			flag1 |= FLG1_BULK_DATA_RECEIVED;           //Buffer filled with data - ready for processing
			return 0x01;                                //Everything received
		} else {
			return 0x00;                                //Some data remaining
		}


	} else if((flag1 & FLG1_LED_CHANGE) && len) {       //Detect change on lock-LEDs
		flag1 &= ~FLG1_LED_CHANGE;                      //Reset flag for lock-LED change

		if((ledState & LEDSTATE_NUM_LOCK) == (data[0]& LEDSTATE_NUM_LOCK) &&
		   (ledState & LEDSTATE_CAPS_LOCK) == (data[0]& LEDSTATE_CAPS_LOCK)) {
			//No change was made on num- and caps-lock
			ledState = data[0];                         // Store received LED status
			return 0x01;                                // Success: 1 byte read

		} else if((ledState & LEDSTATE_CAPS_LOCK) == (data[0]& LEDSTATE_CAPS_LOCK)) {
			//Change was made on num-lock because caps-lock is still the same
			ledState = data[0];                         // Store received LED status

			if((lockChangesCapsNo == 0) || (lockChangesCapsNo > lockConfig.triggerCountCapsNum[0])) {
				++lockChangesNumNo;

				if(lockChangesCapsNo > lockConfig.triggerCountCapsNum[0]) {
					flag2 &= ~(FLG2_NUM_CAPS_ORDER);
				}
			} else {
				++lockChangesNumNo;
				lockChangesCapsNo = 0;                  //Reset odd caps-lock counter
			}

			lockTimeout = lockConfig.triggerCountTimeout;
			return 0x01;                                // Success: 1 byte read

		} else {
			//last outcome - caps-lock was changed (improbable outcome that both locks are changed at the same time will degenerate into this state)
			ledState = data[0];

			if((lockChangesNumNo == 0) || (lockChangesNumNo > lockConfig.triggerCountNumCaps[0])) {
				++lockChangesCapsNo;

				if(lockChangesNumNo > lockConfig.triggerCountNumCaps[0]) {
					flag2 |= FLG2_NUM_CAPS_ORDER;
				}
			} else {
				++lockChangesCapsNo;
				lockChangesNumNo = 0;                   //Reset odd num-lock counter
			}

			lockTimeout = lockConfig.triggerCountTimeout;
			return 0x01;                                // Success: 1 byte read
		}
	}

	return 0xFF;                                        //Error occured - unknown data
}


//Send single pres and release key signal
//This function should be called
void sendKeyPress(uint8_t ascii)
{
	//Set key (press key on a keyboard)
	atok(ascii);
	sendReport();

	//Reset key (release key on a keyboard)
	//keyboardReportReset();        //Can be used instead of atok(KEY_NULL) but it will generate 6 byte larger code - has to be used if more than one keystroke is send (e.g. keyboardReport.keycode[1] is also used)
	atok(KEY_NULL);                 //Since values to keyboardReport are set only by atok() function it can be also used for reset
	sendReport();

	_delay_ms(10);                  //Added delay for smoother operation - without it sometimes keys were missed especialy on repeated character
}

//Flush value of keyboardReport to USB stream
//This function should not be directly called from main
void sendReport()
{
	do {
		;
	} while(!usbInterruptIsReady());   //Wait for USB interrupt to be ready

	usbSetInterrupt((void*)&keyboardReport, sizeof(keyboardReport));
}


//Default num- caps-lock status for keyboard operation
void lockDefault()
{
	if(!(ledState & LEDSTATE_NUM_LOCK)) {    //Turn on num lock
		sendKeyPress(KEY_NUM_LOCK);
	}

	if(ledState & LEDSTATE_CAPS_LOCK) {      //Turn off caps lock
		sendKeyPress(KEY_CAPS_LOCK);
	}
}


//ASCII to keyboard_report_t -> store in global variable
//This function is expecting that Caps-lock is OFF and Num-lock is ON.
//Main table of keyboard codes
void atok(uint8_t ascii)
{
// Load key press into keyboard report without any modifier          //Non-capital leters and numbers
#define CASEC(ASCII_, KEY_)  case ASCII_:                                       \
                                /*keyboardReport.modifier     = 0x00; // Non essential part which will generate a lot of useless code
                                                                        - this value is always reseted by sendKeyPress() on exit so it doesn't have to be reset on next call*/         \
                                keyboardReport.keycode[0]   = KEY_;             \
                                break;

// Load key press into keyboard report with pressed special modifier
#define CASEF(ASCII_, MOD_, KEY_) case ASCII_:                                  \
                                keyboardReport.modifier     = MOD_;             \
                                keyboardReport.keycode[0]   = KEY_;             \
                                break;

// Load key press into keyboard report with pressed shift key        //Capital leters
#define CASES(ASCII_, KEY_)     CASEF(ASCII_, KB_SHIFT_LEFT, KEY_)

	switch(ascii) {
			CASEF(KEY_NULL, 0x00, 0x00);            //Special keyboardReport reset functionality - has to be alway present in the code or keyboardReportReset() has to be used instead

			//Lock change
			CASEC(KEY_NUM_LOCK, 83);                //Has to be alway present in the code - set default lock //Keypad num lock - non-locking key
			CASEC(KEY_CAPS_LOCK, 57);               //Has to be alway present in the code - set default lock //Caps lock - non-locking key

			//Special cases
			CASEC(KEY_RETURN, 40);                  //Return key (enter) OS independent (CR/LF combination)
			CASEC(KEY_TAB_HORIZONTAL, 43);
			CASEC(KEY_SPACE, 44);

#if KEYS_HOST_OS==1
			CASEF(KEY_CTRL_ALT_DEL, (KB_CTRL_LEFT | KB_ALT_LEFT), 76);      //useful mostly for Windows7 login
#endif // HOST_OS

#if KEYS_CONTROL_ALL==1
			CASEC(KEY_ESC, 41);
			CASEC(KEY_BACKSPACE, 42);

			//Navigation keys
			CASEC(KEY_ARROW_UP, 82);
			CASEC(KEY_ARROW_DOWN, 81);
			CASEC(KEY_ARROW_LEFT, 80);
			CASEC(KEY_ARROW_RIGHT, 79);
			CASEC(KEY_PGUP, 75);
			CASEC(KEY_PGDOWN, 78);
			CASEC(KEY_HOME, 74);
			CASEC(KEY_END, 77);
			CASEC(KEY_INSERT, 73);
			CASEC(KEY_DEL, 76);

			CASEF(KEY_WINDOWS, KB_GUI_LEFT, 0)      //Press left "Windows" key - useful mostly for application starting/searching
#endif // KEYS_CONTROL_ALL

			//F-keys
#if KEYS_FKEYS_ALL==1
			CASEC(KEY_F1, 58);
			CASEC(KEY_F2, 59);
			CASEC(KEY_F3, 60);
			CASEC(KEY_F4, 61);
			CASEC(KEY_F5, 62);
			CASEC(KEY_F6, 63);
			CASEC(KEY_F7, 64);
			CASEC(KEY_F8, 65);
			CASEC(KEY_F9, 66);
			CASEC(KEY_F10, 67);
			CASEC(KEY_F11, 68);
			CASEC(KEY_F12, 69);
#endif // KEYS_FKEYS_ALL

			//Letters
#if KEYS_LETTERS_ALL==1
			CASEC('a', 4);
			CASEC('b', 5);
			CASEC('c', 6);
			CASEC('d', 7);
			CASEC('e', 8);
			CASEC('f', 9);
			CASEC('g', 10);
			CASEC('h', 11);
			CASEC('i', 12);
			CASEC('j', 13);
			CASEC('k', 14);
			CASEC('l', 15);
			CASEC('m', 16);
			CASEC('n', 17);
			CASEC('o', 18);
			CASEC('p', 19);
			CASEC('q', 20);
			CASEC('r', 21);
			CASEC('s', 22);
			CASEC('t', 23);
			CASEC('u', 24);
			CASEC('v', 25);
			CASEC('w', 26);
			CASEC('x', 27);
			CASEC('y', 29);  //CZ - qwertz layout
			CASEC('z', 28);  //CZ - qwertz layout
#endif // KEYS_LETTERS_ALL

			//Numbers - numeric keyboard
#if KEYS_NUMBERS_ALL==1
			CASEC('0', 98);
			CASEC('1', 89);
			CASEC('2', 90);
			CASEC('3', 91);
			CASEC('4', 92);
			CASEC('5', 93);
			CASEC('6', 94);
			CASEC('7', 95);
			CASEC('8', 96);
			CASEC('9', 97);
#endif // KEYS_NUMBERS_ALL

			//Capital letters - with Shift key
#if KEYS_LETTERS_CAPITAL_ALL==1
			CASES('A', 4);
			CASES('B', 5);
			CASES('C', 6);
			CASES('D', 7);
			CASES('E', 8);
			CASES('F', 9);
			CASES('G', 10);
			CASES('H', 11);
			CASES('I', 12);
			CASES('J', 13);
			CASES('K', 14);
			CASES('L', 15);
			CASES('M', 16);
			CASES('N', 17);
			CASES('O', 18);
			CASES('P', 19);
			CASES('Q', 20);
			CASES('R', 21);
			CASES('S', 22);
			CASES('T', 23);
			CASES('U', 24);
			CASES('V', 25);
			CASES('W', 26);
			CASES('X', 27);
			CASES('Y', 29);  //CZ - qwertz layout
			CASES('Z', 28);  //CZ - qwertz layout
#endif // KEYS_LETTERS_CAPITAL_ALL

			//Remaining printable characters from Ascii set
#if KEYS_ASCII_SOME==1
			CASEF(0x23, KB_ALTGR, 27);      // #    r_alt + x
			CASEC(0x2F, 84);                // /    Keypad /
			CASEF(0x40, KB_ALTGR, 25);      // @    r_alt + v
			CASES(0x5F, 56);                // _    r_alt + -(next to right shift)
#if KEYS_ASCII_ALL==1
			CASEC(0x2D, 86);                // -    Keypad -
			CASES(0x21, 52);                // !    shift + §
			CASES(0x22, 51);                // "    shift + ů
			CASEF(0x24, KB_ALTGR, 51);      // $    r_alt + ů
			CASES(0x25, 45);                // %    shift + =
			CASEF(0x26, KB_ALTGR, 6);       // &    r_alt + c
			CASES(0x27, 50);                // '    next to enter
			CASES(0x28, 48);                // (    shift + )
			CASEC(0x29, 48);                // )
			CASEC(0x2A, 85);                // *    Keypad *
			CASEC(0x2B, 87);                // +    Keypad +
			CASEC(0x2C, 54);                // ,
			CASEC(0x2E, 55);                // .
			CASES(0x3A, 55);                // :    shift + .
			CASEC(0x3B, 53);                // ;
			CASEF(0x3C, KB_ALTGR, 54);      // <    r_alt + ,
			CASEC(0x3D, 45);                // =
			CASEF(0x3E, KB_ALTGR, 55);      // >    r_alt + .
			CASES(0x3F, 54);                // ?    shift + ,
			CASEF(0x5B, KB_ALTGR, 9);       // [    r_alt + f
			CASEF(0x5C, KB_ALTGR, 20);      // \    r_alt + q
			CASEF(0x5D, KB_ALTGR, 10);      // ]    r_alt + g
			CASEF(0x7B, KB_ALTGR, 5);       // {    r_alt + b
			CASEF(0x7C, KB_ALTGR, 26);      // |    r_alt + w
			CASEF(0x7D, KB_ALTGR, 17);      // }    r_alt + n
#if KEYS_HOST_OS==0         //Linux
			CASEF(0x5E, KB_ALTGR, 35);      // ^    r_alt + ž
			CASEF(0x60, KB_ALTGR, 53);      // `    r_alt + ;(below ESC)
			CASEF(0x7E, KB_ALTGR, 4);       // ~    r_alt + a
#elif KEYS_HOST_OS==1       //Windows
			CASEF(0x5E, KB_ALTGR, 32);      // ^    r_alt + š
			CASEF(0x60, KB_ALTGR, 36);      // `    r_alt + ý
			CASEF(0x7E, KB_ALTGR, 30);      // ~    r_alt + + (1 above q)
#endif // KEYS_HOST_OS
#endif // KEYS_ASCII_ALL
#endif // KEYS_ASCII_SOME

			//cs_CZ specific key modifiers
#if KEYS_CZ_KEYS_ALL==1
			CASEC(KEY_CARKA, 46);
			CASES(KEY_HACEK, 46);
#endif // KEYS_CZ_KEYS_ALL

#undef CASES
#undef CASEF
#undef CASEC

	}
}
