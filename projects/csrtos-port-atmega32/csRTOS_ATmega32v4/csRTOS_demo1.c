// ******************************************************************
//
// File Name:     csRTOS_demo1.c
// Author:        based on work done by Glen Worstell
// Created:       
// Revised:       David Festing
// Date revised:  14/6/2007
// Version:       1.0
// Target MCU:    Atmel ATmega32
//
// The only changes required for your own task setup are:
// - the task number enum (in csRTOS.h)
// - the semaphore enum (in csRTOS.h) and 
// - the const u16 startAdr[] array below
// If wanting up to 16 tasks see notes at beginning of csRTOS.c 
// ******************************************************************


#include <avr/interrupt.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "csRTOS.h"


// csRTOS user code global variables
u08 secs;
u08 mins;
u08 hrs;
u08 clockSet;         // 1 to set minutes, 2 to set hours
char terminalBuf[20]; // A buffer for characters to send to terminal.
u08 terminalBufN;     // The number of the next character to send.


// Task related declarations go here. Tasks are subroutines that are
// declared to return void, with no parameters. They are written to
// never return. Each task has a number, 0 through 7 in this
// implementation. It would be very easy to allow for 16 tasks
// instead of 8.

// The priority of a task is fixed at compile time, and is equal to
// the task number. Lower numbers correspond to higher priority.

// The tasks are listed here, in no particular order.

void blinkTask(void);
void clockTask(void);
void adcTask(void);
void setClockTask(void);

void int2ascii(u16 x, u08 width, char* buffer, u08 slz);


// There are two arrays of constants that are stored in flash to save ram:
//	1) a word array of powers of 10
//	2) a word array of task starting addresses
const u16 ten2nArray[] PROGMEM = {0,1,10,100,1000,10000}; // used by int2ascii conversion
// To initialize tasks (see main() below) it is useful to have a rom array giving the
// starting address of each tasks. List the tasks in the same order as in the enum 
// found in csRTOS.h
const u16 startAdr[nTasks] PROGMEM = {(u16)blinkTask, (u16)clockTask,\
	(u16)adcTask,(u16)setClockTask};


int main(void)
{
// PORT initialisation
// Inputs - Tri-state DDxn = 0 and PORTxn = 0
// Inputs - Pull-up   DDxn = 0 and PORTxn = 1
// Outputs - DDxn = 1 and PORTxn = 0

   PORTB = 0x02; // 0000 0010 turn off the blinkTask LED
   DDRB = 0x02;

   PORTC = 0x04; // 0000 0100 turn off clockTask LED
   DDRC = 0x04;
 
// Timer/Counter 0 initialisation
// Clock source: System Clock
// Clock value: 62.500 kHz
// Clock value: 57.600 kHz with 3.6864 MHz system clock
   TCCR0 = 0x03;
   TIMSK |= (1<<TOIE0); // enable timer0 overflow int

// UART initialisation
// Communication Parameters: 8 Data, 1 Stop, No Parity
// UART Transmitter: On
// UART Baud rate: 9600
// set 9600 baud rate for 3.6864MHz CPU clock
   UBRRL = 0x17; // (fCPUosc / 16Baud) - 1, for 3.6864MHz crystal
   UBRRH = 0;
   UCSRB |= (1<<TXCIE) | (1<<TXEN);

// ADC initialisation
//   ADMUX |= (1<<REFS1) | (1<<REFS0); // select on chip 2.56V ref
// The internal voltage reference options may not be used if an external
// reference voltage is being applied to the AREF pin.

// enable ADC & ints, clock / 64 = 62.5 KHz
   ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1);

// External interrupt initialisation
// initialise INT0, used to adjust clock minutes   
   GICR &= ~(1<<INT0);   // disable INT0 to ensure no interrupts are generated
   MCUCR &= ~(1<<ISC00); // clear bit 0 for falling edge 
   MCUCR |= (1<<ISC01);  // set bit 1 for falling edge interrupt
   GIFR = 1<<INTF0;      // clear any pending interrupts
   GICR |= (1<<INT0);    // enable INT0
  
// initialise INT1, used to adjust clock hours   
   GICR &= ~(1<<INT1);   // disable INT1 to ensure no interrupts are generated
   MCUCR &= ~(1<<ISC10); // set bit 0 for falling edge interrupt
   MCUCR |= (1<<ISC11);  // clear bit 1 for falling edge interrupt
   GIFR = 1<<INTF1;      // clear any pending interrupts
   GICR |= (1<<INT1);    // enable INT1

// initialise semaphores
   for (thisTask = 0; thisTask < nSemas; thisTask++) // using thisTask for a different purpose here
	{
   //wantSema[thisTask] = 0; // Nobody wants the semaphore. NOTE: not required as
 	   semaOwner[thisTask] = 0xFF; // c initializes variables to zero.
   }

// initialise user tasks
   for (thisTask = 0; thisTask < nTasks; thisTask++)
   {
	   ( (void (*)(void)) pgm_read_word(&(startAdr[thisTask])) )();
   // run each task, which will run osTASK_INIT
   // typecast to a function pointer that takes no paramters and returns void
   }

// set some tasks ready to run
   osSET_RTR(blinkTaskNumber); 
   osSET_RTR(adcTaskNumber);

   asm("sei");

   __schedule(); // start multitasking.
}
/* ---------- end of main() ---------- */


// ******************************************************************
//
// Function Name:     int2ascii()
// Description:       convert integers to ASCII format for USART TX 
// Preconditions:     None
// Input Parameters:  integer to convert
// Return Parameter:  powers of ten
// Effects:           
//
// Notes:
//
// ******************************************************************
u16 ten2n(u08 n)
{
   return pgm_read_word((char*)&(ten2nArray[n]));
}
/* ---------- end of ten2n() ---------- */


/* ---------- USER TASKS ---------- */
// ******************************************************************
//
// Function Name:     blinkTask()
// Description:       blink LED at a period of approx. 2 seconds 
// Effects:           
//
// Notes: use osWAIT() to create the delay periods, so that other
//        tasks can run IF ready
//
// ******************************************************************
void blinkTask(void) 
{                    
   osTASK_INIT;
 
   for(;;)
   {
      PORTB &= ~0x02; // turn on the LED on PORTB.1  
      osWAIT(15); // Wait about 0.3 seconds
      PORTB |= 0x02; // turn off the LED on PORTB.1  
      osWAIT(85); // Wait about 1.7 secs
   }
}
/* ---------- end of blinkTask() ---------- */


// ******************************************************************
//
// Function Name:     setClockTask()
// Description:       Set the clock. to a period of approximately
//                    2 seconds 
// Effects:           When incrementing minutes the
//                    seconds are set to zero.
// Notes: use osWAIT to create the delay periods, so that other
//        tasks can run IF ready
//
//        Re-write using setClockMinutesTask() and setClockHoursTask()
//        for easier debouncing of the switches, ie 5 loops as per
//        NZRifle code
//
// ******************************************************************
void setClockTask(void) 
{                       
   osTASK_INIT;			

   for(;;)
   {
      GICR &= ~((1<<INT0) | (1<<INT1)); // disable INT0/1

      osWAIT(5); // 16ms * 5 = 90ms, a simple debounce period
   
   // after 90ms check to see the required port is still low
   // if PORTD2(INT0) or PORTD3(INT1)   
      if (((PIND & 0x04) == 0) || ((PIND & 0x08) == 0))
      {
         if (clockSet & 1)
         {
            secs = 0;
            mins += 1;

            if(mins > 59)
            {
               mins = 0;
            }    
		   }
		      
         if (clockSet & 2)
         {
		 	   hrs += 1;
		
            if(hrs > 23)
            {
               hrs = 0;
            }   
		   }
      }   

      GIFR = ((1<<INTF0) | (1<<INTF1)); // clear pending interrupts
      GICR |= ((1<<INT0) | (1<<INT1)); // enable INT0/1
   
      osSUSPEND;
   }
}
/* ---------- end of setClockTask() ---------- */


// ******************************************************************
//
// Function Name:     clockTask()
// Description:       write the time to the terminal and blink an LED
// Effects:           
//
// Notes: this task blinks the LED at "exactly" a two second period,
//        if using a 4.0000MHz xtal, as per original article.
//
//        see comment about clockTicks 
// ******************************************************************
void clockTask(void)
{                      
   osTASK_INIT;

   for(;;)
   {
// clockTicks -= 1000000; in original code
// seems to me that this would make for an inconsistent clock period.
// With reference to ISR (TIMER0_OVF_vect) the clock rollsover at
// if((clockTicks += 4096) >= 1000000) or at a count 1003520, on
// the first iteration, then the original code resets it back to 3520.
// On the 2nd iteration the clock would rollover at a different value, etc.

      clockTicks = 0;

      PORTC ^= 0x04;
      secs += 1; 
      
		if (secs > 59) 	
      {
         mins += 1; 
         secs -= 60;
         
         if (mins > 59)
         {
            hrs += 1;
            mins -= 60;
         }
      }

      osGET_SEMA(terminal_SEMA); // suspend until terminal semaphore is avail

      int2ascii(hrs,2,terminalBuf,0); // last param: include leading zeros
      terminalBuf[2] = ':';
      int2ascii(mins,2,&terminalBuf[3],0);
      terminalBuf[5] = ':';
      int2ascii(secs,2,&terminalBuf[6],0);
      terminalBufN = 1; // index of next char to send
      UDR = terminalBuf[0]; // send the first char

      osSUSPEND; // clears rtr bit and waits for terminal ISR
                 // to set rtr bit when finished sending
  
      osRELEASE_SEMA(terminal_SEMA);

		osSUSPEND;
   }
}
/* ---------- end of ClockTask() ---------- */


// ******************************************************************
//
// Function Name:     adcTask()
// Description:       measure Aref, which is connected to 5Volts
// Effects:           output value on terminal is 1023
//
// Notes:             The internal voltage reference options may not
//                    be used if an external reference voltage is
//                    being applied to the AREF pin.
//
// Useful for when you want FULL control over when you process the 
// result of an interrupt.
//
// ******************************************************************
void adcTask(void)
{
   static u16 result; // variables used across os calls need to be static
//   u08 low;


   osTASK_INIT;

   for(;;)
   {
      osCLEAR_RTR(thisTask); // make this task not ready to run 
      
      ADCSRA |= (1<<ADSC); // start ADC conversion

      osYIELD; // don't do suspend because isr MAY make rtr before suspend executes!
               // when adc interrupt occurs this task is made ready to run again and
               // will do so when all other higher priority tasks have completed  

//      low = ADCL;
//      result = ADCH;
//      result = (result << 8) + low;
      result = ADCW;

      osGET_SEMA(terminal_SEMA);

      strcpy(terminalBuf, "ADC1 = ");
      int2ascii(result, 4, &terminalBuf[7], 0); // 7 is the "size" of "ADC1 = "
      terminalBufN = 1; // index of next char to send
      UDR = terminalBuf[0]; // send the first char

      osSUSPEND; // clears rtr bit and waits for terminal ISR
                 // to set rtr bit when finished sending
  
      osRELEASE_SEMA(terminal_SEMA);

      osWAIT(150); // wait approx 3 seconds, ie no ADC conversions
	}
}
/* ---------- end of adcTask() ---------- */
/* ---------- end of USER TASKS ---------- */


/* ---------- ISR CODE ---------- */
ISR (INT0_vect) // Press the switch on external interrupt 0 to bump the minutes.
{
   clockSet = 1;
   osSET_RTR(setClockTaskNumber);
}


ISR (INT1_vect) // Press the switch on external interrupt 1 to bump the hours.
{
   clockSet = 2;
   osSET_RTR(setClockTaskNumber);
}


ISR (ADC_vect) // this ISR just clears the ADC interrupt flag.
{
	osSET_RTR(adcTaskNumber);
}


ISR (TIMER0_OVF_vect) // timer0 overflows every 4 mSec.
{
   static u08 delay;
   u08 i;


   if((clockTicks += 4096) >= 1000000) // every 245 loops
                                       // approx 1 second
   {
      osSET_RTR(clockTaskNumber);
   }

   if ((delay -= 1) == 0)	// every 20 mSec update the wait timers for tasks that are waiting.
   {	
      for (i = 0; i < nTasks; i++)
      {      
         if (ticks[i]) // this task is counting
         {         
            ticks[i] -= 1;

            if (ticks[i] == 0) // this task is now ready to run.
            {
               osSET_RTR(i);
            }
         }

         delay = 5; // about 20.5 mS total delay
      }
   }
}


ISR (USART_TXC_vect)
{
   char toSend;


   if ((toSend = terminalBuf[terminalBufN++])) // if there is another non-zero character
   {                                           // in the buffer, send it.
      UDR = toSend; 
	}
   else // done sending zero-terminated terminalBuf[]
   {
      osSET_RTR(semaOwner[terminal_SEMA]); // whomever had the terminal semaphore can run again now.
   }
}
/* ---------- end of ISR code ---------- */


/* ---------- Integer to ASCII conversion ---------- */
void int2ascii(u16 x, u08 width, char* buffer, u08 slz)
{
   while (width > 0)
   {
      *buffer = '0';
		
      while (x >= ten2n(width))
      {
         *buffer += 1;
         x -= ten2n(width);
      }
		
      if(slz)
      {
         if ((*buffer == '0') && (width > 1))
         {
            *buffer = ' ';
         } 
         else
         {
            slz = 0;
         }   
      }
   	
      width -= 1;
   	buffer += 1;
  	}
   
   *buffer++ = 10;
   *buffer++ = 13;
   *buffer++ = 0;
}
/* ---------- end of int2ascii() ---------- */
