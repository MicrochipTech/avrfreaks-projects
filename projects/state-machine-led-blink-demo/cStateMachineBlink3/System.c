//------------------------------------------------------------------------------
// System.c in project StateMachineBlink3
//
// This version uses the watchdog timer in interval mode to generate the
// system tick every ~16 ms.  Device is ATmega168 with fuse settings
// Ext: 0xFF, High: 0xDF, Low: 0x52 (CKDIV8 enabled, 8 MHz internal RC osc).
//
// Andrew Palm
// 2011.10.09
//------------------------------------------------------------------------------
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "System.h"

void System_InitializePorts(void);
void System_SetupWatchdogTimer(void);

//------------------------------------------------------------------------------
void System_InitializeHW(void) {
    System_InitializePorts();
    System_SetupWatchdogTimer();
}

//------------------------------------------------------------------------------
void System_InitializePorts(void) {
    // PortB initialization
    // PB0 - LED1
    // PB1 - LED2
    // PB2 - LED3
    // PB3 - PSHBTN1
    // PB4 - PSHBTN2
    // PB5 - PSHBTN3
    // PB6 - n/c
    // PB7 - n/c
    PORTB = (BIT2|BIT1|BIT0);            // LEDs initially off
    DDRB  = (BIT7|BIT6|BIT2|BIT1|BIT0);  // Set outputs
//  Use next statement only if external pullup resistors not present
//  PORTB |= (PSHBTN1|PSHBTN2|PSHBTN3);  // Prepare pushbutton pin pullups
    PCICR = (1<<PCIE0);                  // Pin interrupts in set 0 enabled
  
    // PortC initialization
    // PC0 - n/c
    // PC1 - n/c
    // PC2 - n/c
    // PC3 - n/c
    // PC4 - n/c
    // PC5 - n/c
    // PC6 - Reset button
    // PC7 - Not present this device
    PORTC = (0x00);
    DDRC  = (0xFF);

    // PortD initialization
    // PD0 - n/c
    // PD1 - n/c
    // PD2 - n/c
    // PD3 - n/c
    // PD4 - n/c
    // PD5 - n/c
    // PD6 - n/c
    // PD7 - n/c
    PORTD = (0x00);
    DDRD  = (0xFF);
}
//------------------------------------------------------------------------------
void System_SetupWatchdogTimer(void) { // System tick setup
    wdt_reset();
    WDTCSR |= (1<<WDCE) | (1<<WDE);    // Start timed sequence
// *** Set TICK_DIVISOR in System.h to match next statement ***
    WDTCSR = (1<<WDIE);                // Set timer value to ~16 ms
}

//------------------------------------------------------------------------------
EMPTY_INTERRUPT(WDT_vect);     // System tick interrupt to wake the processor

