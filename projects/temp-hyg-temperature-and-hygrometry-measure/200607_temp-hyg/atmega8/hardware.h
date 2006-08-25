#ifndef HARDWARE_H
#define HARDWARE_H

#include <avr/io.h>

// atMega168

// USART definitions
#define USART_BAUDRATE 19200

// LCD definitions
#define DMX973B_HEIGHT	32
#define DMX973B_WIDTH	100
#define DMX973B_PORT 	PORTD
#define DMX973B_DDR 	DDRD
#define DMX973B_PORT_MASK 	0x3C
#define DMX973B_A0		4
#define DMX973B_RST		3
#define DMX973B_CLK		2
#define DMX973B_D		5

#define LCD_WIDTH DMX973B_WIDTH
#define LCD_HEIGHT DMX973B_HEIGHT

// SENSORS definition
#define SHTXX_D_PIN PINB
#define SHTXX_D_PORT PORTB
#define SHTXX_D_DDR DDRB
#define SHTXX_CLK_PIN PINB
#define SHTXX_CLK_PORT PORTB
#define SHTXX_CLK_DDR DDRB
#define SHTXX_CLK 0
#define SHTXX_D 1

// BUTTONS definition
#define BUTTONS_NB 4
#define BUTTONS_FIRST 2
#define BUTTONS_MASK 0x3C
#define BUTTONS_PORT_DDR DDRC
#define BUTTONS_PORT PINC
#define BUTTONS_PORT_PULLUPS PORTC
#define BUTTONS_ESC 0
#define BUTTONS_UP 2
#define BUTTONS_DOWN 1
#define BUTTONS_ENTER 3


#endif

