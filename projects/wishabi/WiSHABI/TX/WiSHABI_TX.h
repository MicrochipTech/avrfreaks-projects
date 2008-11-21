#define F_CPU 12000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "serial.h"

#define NUL 0x00 //NUll Byte
#define SOH 0x01 //Start Of Header
#define STX 0x02 //Start Of Text
#define ETX 0x03 //End Of Text
#define EOT 0x04 //Ent Of Transmission

//The following is the number of times an identical packet is to be transmitted in succession.
#define TX_ITERATIONS 1

// BAUDRATE is the value to be sent to the UBRR (USART Baud Rate Register).
// The following values relate to the shown baud rates for a 12MHz crystal.
// BAUDRATE =  77: baud rate set to 9600bps
// BAUDRATE = 155: baud rate set to 4800bps
// BAUDRATE = 312: baud rate set to 2400bps
#define BAUDRATE 155

//ADMUX - ADC Multiplexer Selection Register
//MUX3..0 - Analog Channel Selection Bits
//Atmega8	ADXL330		MUX(2,1,0)
//PC5(ADC5)		X			1,0,1
//PC4(ADC4)		Y`			1,0,0
//PC3(ADC3)		Z			0,1,1
#define CHANNEL_X 0b101
#define CHANNEL_Y 0b100
#define CHANNEL_Z 0b011