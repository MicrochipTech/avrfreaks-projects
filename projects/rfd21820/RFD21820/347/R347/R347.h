#include "avr/io.h"
#include <avr/interrupt.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
//#include "intrinsics.h"

// Set bit y in byte x
#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
// Clear bit y in byte x
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
// Invert bit y in byte x
#define INVERTBIT(ADDRESS,BIT) (ADDRESS ^= (1<<BIT))
// Check bit y in byte x
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))

#define MCLK  8000000   // CPU clock in Hz
#define BAUD  9600      // ???????? USART
#define MYUBRR MCLK/16/BAUD-1

// GPIO
#define LED               PORTB, 2, H
#define LED_DDR           DDRB,  2, H
#define RFD_M0            PORTC, 2, H
#define RFD_M0_DDR        DDRC,  2, H
#define RFD_M1            PORTC, 3, H
#define RFD_M1_DDR        DDRC,  3, H
#define RFD_M2            PORTC, 4, H
#define RFD_M2_DDR        DDRC,  4, H
#define RFD_FAC_PROG      PORTC, 5, H
#define RFD_FAC_PROG_DDR  DDRC,  5, H
#define RFD_LEARN         PORTC, 6, H
#define RFD_LEARN_DDR     DDRC,  6, H
#define RFD_RESET         PORTD, 4, L
#define RFD_RESET_DDR     DDRD,  4, H
#define RFD_PWR           PORTD, 6, H
#define RFD_PWR_DDR       DDRD,  6, H
#define RFD_LOGIC_IO      PORTD, 7, H
#define RFD_LOGIC_IO_DDR  DDRD,  7, H

#define SW            PINB,  4, H
#define SW_DDR        DDRB,  4, H


