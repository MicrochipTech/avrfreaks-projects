/***************************************************************************
* 
* File              : avr.h
* Author			: Kurt E. Clothier
* Date				: November 7, 2011
*
* Description       : Header file for all AVR projects
*					: Code: #include "avr.h"
*					: If placed in WinAVR\avr\include\custom: #include <custom/avr.h>
*					: See the extended description below for more information.
*
* Compiler			: AVR-GCC
* Hardware			: Any AVR micro-controller
*
* Associated Files	: 
*
* Preprocessor Info	: http://gcc.gnu.org/onlinedocs/gcc-2.95.3/cpp_1.html
*
****************************************************************************/
/** For easy use, a folder can be made in the default AVR include directory. 
	The location of this directory is dependent upon the machine in use. For
	example: C:\Program Files\WinAVR-20100110\avr\include. Name the folder 
	"custom" and place any multipurpose files inside, such as avr.h. In doing
	so, the file can be included in any project by using: #include <custom/avr.h>.
****************************************************************************/

/***************************************************************************
		Header Files to Include
****************************************************************************/
 	

#ifndef _CUSTOM_AVR_
#define _CUSTOM_AVR_

#ifndef _AVR_INTERRUPT_H_
#include <avr/interrupt.h>
#endif /* _AVR_INTERRUPT_H_ */

#ifndef _AVR_IO_H_
#include <avr/io.h>
#endif /* _AVR_IO_H_ */

#ifndef __STDINT_H_
#include <stdint.h>
#endif /* __STDINT_H_ */

#ifndef _BV()
#define	_BV(BIT)	(1 << (BIT))
#endif /* _BV() */

/***************************************************************************
		Custom Macros
****************************************************************************/

#define SET_BIT(BYTE, BIT)		(BYTE |= BIT)
#define CLEAR_BIT(BYTE, BIT)	(BYTE &= ~BIT)
#define TOGGLE_BIT(BYTE, BIT)	(BYTE ^= BIT)

#define SET_BV(BYTE, BV)		(BYTE |= (1<<BV))
#define CLEAR_BV(BYTE, BV)		(BYTE &= ~(1<<BV))
#define TOGGLE_BV(BYTE, BV)		(BYTE ^= (1<<BV))

#define	BIT_IS_HI(BYTE, BIT)	(BYTE & BIT)			// Check to see if BIT in BYTE is High
#define	BIT_IS_LO(BYTE, BIT)	(!(BYTE & BIT))			// Check to see if BIT in BYTE is 

#define	BV_IS_HI(BYTE, BIT)	(BYTE & _BV(BIT))			// Check to see if Bit Value of BIT in BYTE is High
#define	BV_IS_LO(BYTE, BIT)	(!(BYTE & _BV(BIT)))		// Check to see if Bit Value of BIT in BYTE is Low

#define	LOOP_UNTIL_BIT_HI(BYTE, BIT)	do{ } while (BIT_IS_LO(BYTE, BIT))	// Loop while BIT in BYTE is High
#define	LOOP_UNTIL_BIT_LO(BYTE, BIT)	do{ } while (BIT_IS_HI(BYTE, BIT))	// Loop while BIT in BYTE is Low

#define	LOOP_UNTIL_BV_HI(BYTE, BIT)	do{ } while (BV_IS_LO(BYTE, BIT))	// Loop while BIT in BYTE is High
#define	LOOP_UNTIL_BV_LO(BYTE, BIT)	do{ } while (BV_IS_HI(BYTE, BIT))	// Loop while BIT in BYTE is Low

#define	SWAP(A, B)	(do { A^=B; B^=A; A^=B; } while(0))	// Swap the values of A and B - Don't pass expressions!


#endif /* _CUSTOM_AVR_ */

