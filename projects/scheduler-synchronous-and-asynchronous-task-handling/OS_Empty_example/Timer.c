#include    <avr/io.h>
#include    <stdio.h>
#include    "OS.h"

/*******************************************************************/
// Function name: Delay 
// Task: Stops the uP for a number of ms - Xtal must be 16 MHz
// Input: Required delay in ms, range [0..65535]
// Return:
// NB: Don't call delay from a function - only during initialization
/*******************************************************************/
extern void Delay (uint16_t ms)
{
   volatile uint16_t i;

   while (ms > 0)
   {
      for (i = 0; i < 857; i++);    // found by test, delay = 1 ms
      ms--;                         // count number of ms
   }
}

/******************************************************************************/
// Function name: Timer1Init
// Function task: set Timer1 to 10 ms reoccuring interrupts
// Input: 
// Return:
/******************************************************************************/
extern void Timer1Init (void)
{
   TCNT1 = 0;                       // count from 0
   OCR1A = 19999;                   // divide 16 MHz with 8 * 20000 to 100 Hz
   TIMSK = TIMSK | 0x10;            // enable intr on OCR1A
   TCCR1A = 0x00;                   // mode = 4, 
   TCCR1B = 0x0A;                   // prescaler = 8, start running
}

