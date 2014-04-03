//=============================================================================
// Common and hardware-dependant definitions.
//
// $Id: common.h,v 1.8.2.3 2006/08/10 16:04:29 SOY Exp $
//=============================================================================

#ifndef __COMMON_H_INCLUDED__
#define __COMMON_H_INCLUDED__

//=============================================================================
// System-specific headers for IAR compiler
//#include <ioavr.h>          // for AVR bit definitions
//#include <inavr.h>          // for intrinsic functions such like __enable_interrupt()

// Some compatibility defines for porting to other compilers
//#define __C_task

//=============================================================================
// Clock options, may be used for delay calculation
#define XTAL                8000000L               // XTAL frequency, Hz
#define CYCLE_nS            (1000000000L/XTAL)      // cycle period, nS



//=============================================================================
// IO port name concatenation macros
#define IO_CONCAT(a, b)            a ## b
#define IO_INPORT(name)            IO_CONCAT(PIN, name)
#define IO_OUTPORT(name)           IO_CONCAT(PORT, name)
#define IO_DDRPORT(name)           IO_CONCAT(DDR, name)

//=============================================================================
// User headers
#define MAX_CHANNELS 6

#define sbi(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define cbi(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define outp(VAL,ADRESS) ADRESS=VAL
#define inp(VAL) VAL


#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>    // include interrupt support


#endif  // __COMMON_H_INCLUDED__
