/*****************************************************************************
*
* Atmel Corporation
*
* File              : main.c
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 2.0 $
* Date              : $Date: Wednesday, February 15, 2006 09:52:02 UTC $
* Updated by        : $Author: raapeland $
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with Bootloader Capabilities 
*                     , and at least 1-KB SRAM can be used.
*                     The example is written for • ATmega8
*                                                • ATmega16
*                                                • ATmega162
*                                                • ATmega169
*                                                • ATmega32
*                                                • ATmega64
*                                                • ATmega128
*                                                • ATmega2561
*
* AppNote           : AVR231 - AES Bootloader
*
****************************************************************************/
#include <avr/io.h>

//=============================================================================
// Polynome used in CRC calculations

#define	CRC_POLYNOME 0x8005


//=============================================================================
// unsigned int CRC (unsigned int crc, unsigned char ch);
//						r25:r24				r23:r22
.global CRC


//=============================================================================
// CRC calculation routine
//=============================================================================

CRC:
	ldi		r18, 0x08
	ldi		r19, lo8(CRC_POLYNOME)
	ldi		r20, hi8(CRC_POLYNOME)

CRC_Loop:
	; Rotate left. If MSb is 1 -> divide with Generator-polynome.
	lsl		r22
	rol		r24
	rol		r25
	brcc	CRC_SkipEor

	eor		r24, r19
	eor		r25, r20

CRC_SkipEor:
	dec		r18
	brne	CRC_Loop

	ret

//=============================================================================


