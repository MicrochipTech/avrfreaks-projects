/*
*   system.h  --  Platform and build dependencies are defined here
*
*   NB:  IAR compiler requires the option "--string_literals_in_flash".
*   This allows, for example, constructs of the kind:  putstr_far("Hello, world.");
*/
#ifndef  _SYSTEM_H_
#define  _SYSTEM_H_


#include  <iom128.h>             // headers for IAR AVR C compiler
#include  <inavr.h>
#include  <intrinsics.h>
#include  <signal.h>
#include  <pgmspace.h>
#include  <stdlib.h>
#include  <ctype.h>

#define  BUILD_VER_MAJOR  1
#define  BUILD_VER_MINOR  3
#define  BUILD_VER_DEBUG  100


// #define  CLOCK_FREQ     (8000000L)
#define  CLOCK_FREQ     (7372800)       // MCU XTAL clock frequency (Hz)
#define  DEBUG_BUILD    TRUE            // FALSE in final release
#define  LITTLE_ENDIAN  TRUE            // ATmega128 AVR is little-endian


#define  PROGRAM_ENTRY_POINT     (0x00000)     // Program address for software restart
#define  BOOTLDR_ENTRY_ADDRESS   (0x1E000)     // Bootloader start address (ATmega128)


// Generic aliases for compiler-dependent intrinsic functions
#define  _INTERRUPT_         __interrupt
#define  ENABLE_GLOBAL_IRQ   __enable_interrupt()
#define  DISABLE_GLOBAL_IRQ  __disable_interrupt()


#include  "gendef.h"


#endif  /* _SYSTEM_H_ */
