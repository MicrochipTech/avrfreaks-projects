#ifndef CSRTOS_H
#define CSRTOS_H

// ******************************************************************
//
// File Name:     csRTOS.h
// Author:        based on work done by Glen Worstell
// Created:       
// Revised:       David Festing
// Date revised:  29/6/2007
// Version:       2.0
// Target MCU:    Atmel ATmega32
//
// #defines for stack pointer added by curtvm, see AVRfreaks thread "RTOS for AVR"
// ******************************************************************


#define u08 unsigned char
#define u16 unsigned int
#define u32 unsigned long int

//these 2 stack values will be used for the reset value of the hardware and compiler stack
//set to a value that leaves enough room above it for local variables used by the compiler
//take the max value any task uses for local variables, subtract it from the RAMEND value of your avr
//the example below will leave 32 bytes of stack space for the compiler to use (ATmega32, RAMEND=0x085F,
//0x085F - 32 = 0x083F)
#define csSTACK_L 0x08
#define csSTACK_H 0x3F

// Macros for operating system calls.  If a higher priority task is ready to run,
//	an os call will result in the suspension of the calling task and the resumption
//	of the higher priority one. Exceptions: osSET_RTR and osCLEAR_RTR do not result
//	in a task switch.
#define osTASK_INIT if(saveTask(getAdr()) == 0) return
#define osWAIT(ticks) if (saveTask(getAdr()) == 0) __wait(ticks)
#define osYIELD if(saveTask(getAdr()) == 0) __schedule()
#define osSUSPEND if(saveTask(getAdr()) == 0) __wait(0)
#define osCLEAR_RTR(task) rtr = rtr & ~two2n(task)
#define osSET_RTR(task) rtr |= two2n(task)
#define osGET_SEMA(semaNumber) if(saveTask(getAdr()) == 0) __getSema(semaNumber)
#define osRELEASE_SEMA(semaNumber) if(saveTask(getAdr()) == 0) __releaseSema(semaNumber)


// The task numbers are given by an enum.
// It is vital that the order of the task addresses in startAdr[nTasks]
// (see the user project code) are in the same as the order they are listed
// in the enum below.
// NOTE: the last entry in the enum is "nTasks" - it is NOT a task,
// but serves to define the number of tasks.
enum {blinkTaskNumber, clockTaskNumber, adcTaskNumber, setClockTaskNumber, nTasks};

// Semaphores are defined here.  Each semaphore uses two bytes: one
// has a bit set for each task waiting for the semaphore, and the
// other contains the number of the task that owns the semaphore.
// If the semaphore is available, the owner byte will be 255.
// Semaphores are numbered sequentially from zero.
enum {terminal_SEMA,nSemas}; // The semaphore number for each semaphore.
                             // As for the task enumeration, the last entry
                             // is not a semaphore, but defines the number
                             // of semaphores.


extern volatile u08 rtr; // used in the os macros above		
extern volatile u08 thisTask;
extern u08 ticks[]; 	
extern u32 clockTicks; 
extern u08 taskSpace[]; 
extern PGM_P romAdr;  
extern u08 wantSema[];
extern u08 semaOwner[];


u08 two2n(u08 n); // needs to be here because of the #defines above
u16 getAdr(void);
u08 saveTask(u16 adr);
u08 restoreTask(u16 adr);
void __schedule(void);
void __wait(u08 nTicks);
void __getSema(u08 semaNumber);
void __releaseSema(u08 semaNumber);


#endif

