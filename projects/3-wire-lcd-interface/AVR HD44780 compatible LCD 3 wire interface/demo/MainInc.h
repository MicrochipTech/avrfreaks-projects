// LCD test
// © Portia CyberLogic.
// www.portia-cyberlogic.com
// portia2220@yahoo.com
//
// Main.cpp
// LCD test main include.

#ifndef __MAIN_INC__
#define __MAIN_INC__

#ifndef F_CPU
	#define F_CPU 4000000UL
#endif

#include <math.h>

#include <avr/io.h>
#include <avr/interrupt.h>

// ################################################################ //
// # General
// ################################################################ //

// Firmware version (1.01.01.01)
#define VER_MAJOR 1 // Major version.
#define VER_MINOR 1 // Minor version.
#define VER_BUILD 1 // Build.
#define VER_REV 1 // Revison.

// ################################################################ //
// # LCD support
// ################################################################ //

// LCD serial interface pin configuration..
#define LCD_DAT_PORT PORTB // LCD Serial data pin port.
#define LCD_DAT_DDR DDRB // LCD Serial data pin port DDR.
#define LCD_DAT_PIN PB0 // LCD Serial data pin.

#define LCD_CLK_PORT PORTB // LCD Serial clock pin port.
#define LCD_CLK_DDR DDRB // LCD Serial clock pin port DDR.
#define LCD_CLK_PIN PB1 // LCD Serial clock pin.

#define LCD_E_PORT PORTB // LCD enable pin port.
#define LCD_E_DDR DDRB // LCD enable pin port DDR.
#define LCD_E_PIN PB2 // LCD enable pin.

// LCD header..
#include "lcd.h"

// ################################################################ //
// # EOF
// ################################################################ //

#endif
