/*
             MyUSB Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com

 Released under the LGPL Licence, Version 3
*/

// Configuration:

   /* Buffer length - select static size of created ringbuffers: */
	 #define BUFF_STATICSIZE 128      // Set to the static ringbuffer size for all ringbuffers (place size after define)

   /* Volatile mode - uncomment to make buffers volatile, for use in ISRs, etc: */
	 #define BUFF_VOLATILE            // Uncomment to cause all ring buffers to become volatile (and atomic if multi-byte) in access

   /* Drop mode - select behaviour when Buffer_StoreElement called on a full buffer: */
     #define BUFF_DROPOLD             // Uncomment to cause full ring buffers to drop the oldest character to make space when full
     // #define BUFF_DROPNEW          // Uncomment to cause full ring buffers to drop the new character when full
     // #define BUFF_NODROPCHECK      // Uncomment to ignore full ring buffer checks - checking left to user!

   /* Underflow behaviour - select behaviour when Buffer_GetElement is called with an empty ringbuffer: */
     //#define BUFF_EMPTYRETURNSZERO  // Uncomment to return 0 when an empty ringbuffer is read
     #define BUFF_NOEMPTYCHECK        // Uncomment to disable checking of empty ringbuffers - checking left to user!
	
   /* Buffer storage type - set the datatype for the stored data */
     #define BUFF_DATATYPE uint8_t    // Change to the data type that is going to be stored into the buffer
	
   /* Peek routine - uncomment to include the peek routine (fetches next byte without removing it from the buffer */
     //#define BUFF_USEPEEK
	 	
#ifndef _RINGBUFF_H_
#define _RINGBUFF_H_

	/* Includes: */
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <limits.h>

	/* Defines and checks: */
	#if defined(BUFF_STATICSIZE)
		#define BUFF_LENGTH BUFF_STATICSIZE
	#else
		#error No buffer length specified!
	#endif

	#if !(defined(BUFF_DROPOLD) || defined(BUFF_DROPNEW) || defined(BUFF_NODROPCHECK))
		#error No buffer drop mode specified.
	#endif

	#if !defined(BUFF_DATATYPE)
		#error Ringbuffer storage data type not specified.
	#endif

	#if (defined(BUFF_VOLATILE) && (BUFF_STATICSIZE > CHAR_MAX))
		#define BUFF_MODE            volatile
		#define BUFF_ATOMIC_BLOCK    ATOMIC_BLOCK
	#else
		#define BUFF_MODE
		#define BUFF_ATOMIC_BLOCK
	#endif

	#if (BUFF_STATICSIZE   > LONG_MAX)
		#define RingBuff_Elements_t uint64_t
	#elif (BUFF_STATICSIZE > INT_MAX)
		#define RingBuff_Elements_t uint32_t
	#elif (BUFF_STATICSIZE > CHAR_MAX)
		#define RingBuff_Elements_t uint16_t
	#else
		#define RingBuff_Elements_t uint8_t
	#endif
	
	/* Type Defines: */
	typedef BUFF_DATATYPE RingBuff_Data_t;

	typedef BUFF_MODE struct
	{
		RingBuff_Data_t      Buffer[BUFF_LENGTH];
		RingBuff_Data_t*     InPtr;
		RingBuff_Data_t*     OutPtr;
		RingBuff_Elements_t  Elements;
	} RingBuff_t;
		
	/* Function Prototypes: */
	void            Buffer_Initialize(RingBuff_t* Buff);
	void            Buffer_StoreElement(RingBuff_t* Buffer, RingBuff_Data_t Data);
	RingBuff_Data_t Buffer_GetElement(RingBuff_t* Buffer);
	#if defined(BUFF_USEPEEK)
		RingBuff_Data_t Buffer_PeekElement(const RingBuff_t* Buffer);
	#endif
	
#endif
