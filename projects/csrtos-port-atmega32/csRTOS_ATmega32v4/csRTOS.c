// csRTOS.c 2-MAY-04 Atmel/Circuit Cellar contest entry number A3650
// Written by Glenn Worstell

// Multitasking with small AVRs - a cooperative multitasking demo
// ported to the ATmega32 by Dave Festing, dave_festing@hotmail.com
// saveTask and restoreTask modification by curtvm,
// see AVRfreaks thread "RTOS for AVR"

// This cooperative single-stack RTOS (csRTOS) is coded mostly
// in C. Smaller RAM requirements and faster task switching can be
// achieved by coding the C library routines  <setJump.h>
// and <longjmp.h> in assembly language, as is done here.

// This code was written and tested using the free WinAVR C compiler.
// N.B. using optimisation level 0s

// CSRTOS requires two simple rules to work:

//  1) Operating calls that may result in task switches can occur only
//     at the top level of a task. That is, you cannot make os calls
//     from subroutines called within a task.

//  2) Local variables of a task subroutine are not preserved across
//     os calls. If you need to preserve the value of a local, declare
//     it to be "static".


#include <avr/pgmspace.h>

#include "csRTOS.h"


// Task switching occurs only when a task makes certain os calls. After an os call the highest
// priority task that is ready to run (rtr) is resumed.

// The os calls give the following functionality:

// osTASK_INIT must be called for each task, before starting multitasking. The initialization
// code in main() takes care of this.

// osWAIT(ticks) suspends the calling task for some number of ticks. In this implementation
// there are 64 ticks per second, or about 16 mSec per tick. Forces the highest priority
// task, other than the calling task to run.  Also, clears the rtr bit.

// osYIELD resumes the highest priority task that is rtr. The rtr status of the calling
// task is not changed - so if it is the highest priority rtr it immediately resumes.

// osSUSPEND makes the calling task not rtr (clears rtr bit). Usually it will be made rtr
// at some future time by an interrupt service routine or by another task.

// osCLEAR_RTR(task) clears the rtr bit of the given task, but DOES NOT call the scheduler
// for a task switch.

// osSET_RTR(task) sets the rtr bit of the given task, but DOES NOT call the scheduler
// for a task switch.

// osGET_SEMA(number) makes the calling task the owner of the given semaphore. If the semaphore
// is not available the task is suspended. When the semaphore becomes available the highest
// priority task that is waiting for it is set rtr. As with most os calls, a call to this
// routine results in a task switch if a higher priority task is rtr.

// osRELEASE_SEMA(number) releases the semaphore, which results in another task being set
// rtr if another task is waiting for the semaphore. Of course a task should not release
// a semaphore unless it owns it - no error checking is done for this condition.

// csRTOS can be written entirely in c, using the above routines from the standard c library.
// For smaller code size and greater speed, this demo uses similar routines with different
// names, coded in assembly language.  See original project documents for the details.

// OS FUNCTION NOTES:

// These functions are normally not called from user code, except via the supplied os macros.

// The schedule function finds the highest priority task that is rtr, and resumes it.
// If no task is rtr, the code waits until one is. In real application
// code you may want to put the CPU in a low power mode when no task is rtr.

// You can use a 256 byte array to more quickly find the highest priority task that is
//	ready to run (if you have enough eeprom or flash space available).

// Also see original documentation for the use of bigger containers 
// thisTask = lsBit[rtr]; // find the highest priority task that is rtr.
                          // do this if the array lsBit[256] is used.

// If you want to allow 16 tasks, use a u16 for rtr, and look at the low order byte of
// rtr first. If it is zero, look at the high order byte.  thisTask would need to be a
// u16 as well?  Also, the const u08 two2nArray[] would have to go to u32
// so that it can hold 65536.  N.B. an int can only hold 65535

// An array of constants that are stored in flash to save ram:
//	1) a byte array of powers of 2
const u08 two2nArray[] PROGMEM = {1,2,4,8,16,32,64,128};
                          	// used to convert a task number
                          	// to the corresponding bit in rtr.


// variables used by the whole project:
volatile u08 rtr;	   // If a task is ready to run, a bit corresponding to
                     //  the task number is set in rtr. If you want to
                     //  allow up to 16 tasks, use a u16 variable for rtr.
volatile u08 thisTask; // The number of the currently running task.
u08 ticks[nTasks]; 	// If a task is suspended for ticks,
                    	// the corresponding variable will be non-zero.
                    	// With a tick period of 20 mSec, a task can be
                    	// delayed for up to 5 seconds. If longer delays
                    	// are required, simply call osWAIT() several
                    	// times (perhaps in a for() loop - but be sure to
							// use a "static" loop variable).
u32 clockTicks; // for keeping clock time
u08 taskSpace[nTasks * 2]; // Two bytes of status information are kept for each
							      // suspended task
u08 wantSema[nSemas];  // One bit high for each task waiting for the sema.
u08 semaOwner[nSemas]; // Task number of sema owner, or 255 if sema is available.


// convert a task number to the corresponding bit in rtr.
u08 two2n(u08 n)
{
   return pgm_read_byte(&(two2nArray[n]));
}


// returns the address of the first byte of
//	the space used to store a task's state info
u16 getAdr(void)
{
	return (u16)(taskSpace + (2 * thisTask));
}

// the following 2 functions have been modified by curtvm, see AVRfreaks
// thread "RTOS for AVR" for details
u08 saveTask(uint16_t adr) 	//adr is address of task space
{
   asm("mov r26,r24"); //r25:r24 has address of task space
   asm("mov r27,r25"); //copy it to X register (r27:r26)
   asm("pop r24"); //copy return address from hardware stack
   asm("pop r25"); //will be address of where saveTask was called
   asm("push r25"); //put address back on stack
   asm("push r24");
   asm("st x+,r24"); //store that address in task space
   asm("st x+,r25");

   return 0; // indicates saveTask was just run
}


u08 restoreTask(uint16_t adr)	//adr is address of task space
{   
   asm("mov r26,r24"); //r25:r24 has address of task space
   asm("mov r27,r25"); //copy it to X register (r27:r26)
   asm("ldi r28, %0" : : "M" (csSTACK_L) ); //load r29:r28 with stack pointer value
   asm("ldi r29, %0" : : "M" (csSTACK_H) );
   asm("in __tmp_reg__,__SREG__"); //get SREG (only to get I bit)
   asm("cli"); //turn off irq's
   asm("out __SP_H__,r29"); //copy r29:r28 -> SPH:SPL
   asm("out __SREG__,__tmp_reg__"); // restore SREG(turn irq back on if was on previously)
   asm("out __SP_L__,r28"); //SPL (this instruction will always execute if irq just turned on)
   asm("ld r24,x+"); //copy saved return address from task space
   asm("ld r25,x+");
   asm("push r25"); //and put it on the stack, to get us to the new task
   asm("push r24");

   return 1; // indicates restoreTask was just run
}


void __schedule(void)
{
   u08 osTemp;
   
   
   while(rtr == 0)
      {}; // could put CPU in low power mode here
	
   osTemp = rtr;
   thisTask = 0;
   
   while ((osTemp & 1) == 0) 	// find highest priority
   {
      osTemp >>= 1;
      thisTask += 1;
   }

   restoreTask(getAdr()); // resume the new task.
}


// Suspend the calling task for some number of ticks.
// Calling this with nTicks=0 will result in a
// suspension that will not be resumed by ticks,
// but can only be resumed by an ISR or another
// task setting the rtr bit.
void __wait(u08 nTicks)
{
   osCLEAR_RTR(thisTask);
   ticks[thisTask] = nTicks;
   __schedule();
}


// acquire a semaphore, or suspend
void __getSema(u08 semaNumber)
{
   if (semaOwner[semaNumber] == 255)
   {   
      semaOwner[semaNumber] = thisTask;
   }   
   else
   {
      wantSema[semaNumber] |= two2n(thisTask);
      osCLEAR_RTR(thisTask);
   }
   
    __schedule();
}


void __releaseSema(u08 semaNumber)
{
   u08 temp;
   u08 osTemp;


   temp = two2n(thisTask);
   wantSema[semaNumber] &= ~temp;
   osTemp = wantSema[semaNumber];
   if (osTemp != 0) // someone else wants the sema
   {
      temp = 0;
		
      while ((osTemp & 1) == 0) 	// find highest priority
      {      
         osTemp >>= 1;
         temp += 1;
      }
      
      semaOwner[semaNumber] = temp;
      osSET_RTR(temp);
   }
   else
   {
      semaOwner[semaNumber] = 255;
   }
   __schedule();
}
