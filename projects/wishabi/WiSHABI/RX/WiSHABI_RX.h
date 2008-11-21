#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "usbdrv.h"
#include "usbconfig.h"

#include "serial.h"//Serial (USART) related functions specific to this application.
#include "usb.h"//USB related functions specific to this application.

///////////////////////////////////////////////////////////////////////////////////////////
//The following #defines are common to mouse_mode and keyboard_mode
//WiSHABI Buttons A through E are connected to PORTB from PB0 to PB4
#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_C 2
#define BUTTON_D 3
#define BUTTON_E 4

//Used to tell Set_LEDs function what colour the RGB LED should be.
#define MODE_MOUSE		1
#define MODE_KEYBOARD	2
#define MODE_NO_SIGNAL	3

//Number of loop iterations without receiving data on the USART before
//the RGB LED is set to RED to indicate no signal.
#define MAX_TIMEOUT 20

#define MODE_CHANGE_ITERATIONS 50

//Sector Designations
//   |   |
// 1 | 2 | 3
//___|___|___
//   |   |
// 4 | 5 | 6
//___|___|___
//   |   |
// 7 | 8 | 9
//   |   |
#define SECT_1 0
#define SECT_2 1
#define SECT_3 2
#define SECT_4 3
#define SECT_5 4
#define SECT_6 5
#define SECT_7 6
#define SECT_8 7
#define SECT_9 8
///////////////////////////////////////////////////////////////////////////////////////////

//BAUDRATE value of  77 sets the baudrate to 9600 bps using a 12MHz crystal = UBRR value
//BAUDRATE value of 155 sets the baudrate to 4800 bps using a 12MHz crystal = UBRR value
//BAUDRATE value of 312 sets the baudrate to 2400 bps using a 12MHz crystal = UBRR value
#define BAUDRATE 155

//Packet Data Bytes
#define NOP 0x00	//NULL
#define SOH 0x01	//Start Of Header
#define STX 0x02	//Start Of Text
#define ETX 0x03	//End Of Text
#define EOT 0x04	//End Of Transmission

//Value of counter is found in register TCNT1 - Timer/Counter 1
#define TIMER TCNT1
//With Prescaler set to F/1024, One increment On TIMER = 1/11719 Of A Second
#define FIVE_SECONDS 58595
#define THREE_SECONDS 35157
#define ONE_SECOND 11719
#define TEN_MILLISECONDS 117

//9 LEDs in a grid to indicate tilt orientation.
//3 LEDs (1xRGB LED) to indicate operating mode/data reception timeout.
#define LED_1 0
#define LED_2 1
#define LED_3 2
#define LED_4 3
#define LED_5 4
#define LED_6 5
#define LED_7 6
#define LED_8 7
#define LED_9 8
#define LED_MODE_1 9
#define LED_MODE_2 10
#define LED_MODE_3 11
