//------------------------------------------------------------------------------
// main.c in project StateMachineBlink3
//
// The overall structure of this program follows the approach of Tom Baugh
// found in "MSP430 State Machine Programming."
// 
// This program was written to explore the use of non-hierarchical state
// machines as a way of organizing a program.  The state machine structures
// and functions used here are based on the QEP FSM processor found in Chapter
// 3 of "Practical UML Statecharts in C/C++" by Miro Samek.  
// 
// The modules in this program:
//
//   Main:  General shell program for initialization and execution of the
//          system tick loop.  Program sleeps in LPM3 when not executing a
//          tick.  This version uses the watchdog timer to generate system
//          ticks, leaving TimerA free for other modules.
//
//   System:  Does general low-level hardware initialization and contains
//            the WDT ISR.
//
//   StateMachine:  Contains base state machine and event structures and
//                  performs base state machine functions.  This code is
//                  modified from code copyrighted by Miro Samek under the
//                  GNU Public License Version 2.  This code is free for any
//                  use or distribution provided that the copywrite notice
//                  at the beginning of the module files is retained.
//
//   Blink:  Sets up and runs state machines that blink three LEDs.
//
//   PushButton:  Sets up and runs state machines that control pushbutton
//                functions, including debouncing and optional alternate
//                function or acceleration feature.  The functions actually
//                executed on a push are determined by "event sink registration
//                functions" called by the client, which in this case is the
//                Blink module.
//  
// See the individual module headers for further descriptions.
//
// This version is ported to the ATmega168.  Note that LED outputs are
// 0 = on, 1 = off for the development board used.  See System.c for the
// hardware connections.
//
// Andrew Palm
// 2011.10.09
//------------------------------------------------------------------------------
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "System.h"
#include "Blink.h"
#include "PushButton.h"

void InitializeHW(void);
void InitializeApps(void);
void main(void) __attribute__((noreturn));

//------------------------------------------------------------------------------
// Early stop of watchdog timer in initialization section
void early_wdt_stop(void) __attribute__((naked)) \
    __attribute__((section(".init3")));

void early_wdt_stop(void) {
    MCUSR = 0;
    wdt_disable();
}

//------------------------------------------------------------------------------
void main(void) {

    InitializeHW();
    InitializeApps();

     set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // Set to power-down sleep mode
     sei();

    while(1) {                       // Event loop
        sleep_mode();                // Sleep between ticks
        Blink_ProcessTimerEvents();  // Process LED state machine timer events
        PushButton_ProcessTimerEvents();  // Process PB SM timer events 
    }
}

//------------------------------------------------------------------------------
void InitializeHW(void) {
    System_InitializeHW();
    Blink_InitializeHW();
    PushButton_InitializeHW();
}

//------------------------------------------------------------------------------
void InitializeApps(void) {
    Blink_InitializeApp();
    PushButton_InitializeApp();
}
