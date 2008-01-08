/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef GLOBALMACROS_H
#define GLOBALMACROS_H

	// Typedefs:
	typedef struct
	{
		uint8_t Bytes[4];
	} ByteArray;
	
	typedef void (*FuncPtr)(void);
	
	// Version Macros:
	#define VERSION_MAJOR             3
	#define VERSION_MINOR             0
	#define VERSION_VSTRING           {'V','0' + VERSION_MAJOR,'-','0' + VERSION_MINOR, '\0'}
	
	#define MAGIC_NUM                 (0xDC | 0x1337) // Magic number, used for first-run detection
	
	// Program Type Macros:	
	#define TYPE_EEPROM               0
	#define TYPE_FLASH                1
	#define TYPE_FUSE                 2
	#define TYPE_LOCK                 3
	
	// Code Macros:
	#define TRUE                      1
	#define FALSE                     0
	
	#define ARRAY_UPPERBOUND(array)   ((sizeof(array) / sizeof(array[0])) - 1)
	
	#define MACROS                    do
	#define MACROE                    while (0)
	
	#define ROUND_UP(x)               (unsigned long)(((float)x) + .5)

	#define BYTE(x,y)                 ((ByteArray*)&x)->Bytes[y]

	// Joystick Macros:
	#define JOY_LEFT                  (1 << 2)
	#define JOY_RIGHT                 (1 << 3)
	#define JOY_UP                    (1 << 6)
	#define JOY_DOWN                  (1 << 7)
	#define JOY_PRESS                 (1 << 4)
	#define JOY_INVALID               (1 << 0)
	
	#define JOY_BMASK                 ((1 << 4) | (1 << 6) | (1 << 7))
	#define JOY_EMASK                 ((1 << 2) | (1 << 3))

	// Sleep Macros:
	#define SLEEPCPU(mode)            MACROS{ SMCR = mode; asm volatile ("sleep" ::); }MACROE

	#define SLEEP_IDLE                (1 << SE)
	#define SLEEP_POWERDOWN           ((1 << SE) | (1 << SM1))
	#define SLEEP_POWERSAVE           ((1 << SE) | (1 << SM1) | (1 << SM0))

	// Function attributes:
	#define ATTR_NO_RETURN            __attribute__ ((noreturn))
	#define ATTR_INIT_SECTION(x)      __attribute__ ((naked, section (".init" #x )))
	#define ATTR_WARN_UNUSED_RESULT   __attribute__ ((warn_unused_result))
	#define ATTR_NON_NULL_PTR_ARGS(x) __attribute__ ((nonnull (x)))
	#define ATTR_NAKED                __attribute__ ((naked))
	#define ATTR_ALWAYS_INLINE        __attribute__ ((always_inline))
#endif
