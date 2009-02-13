;**** A P P L I C A T I O N   N O T E	A V R ? ? ? ************************
;*
;* Title:		32-bit Arithmetic Routines with Macrolibrary
;* Project:		Math32
;* Version:		2.3
;* Last updated:	2003.09.15
;* Create Date: 	1999.10.25
;* Target MCU:		AT90S8515 (as well as others AVR uC)
;*			(C) ATMEL Corporation (mailto:avr@atmel.com)
;* Originator:		(C) 1999-2003 Andre Birua (mailto:birua@hotmail.com)
;*			This Application Note absolutely free in use anybody
;* INTERPRETATION
;* This package of assembler subprograms is developed for integer arithmetic
;* with tracing of sign bit in 32 bits calculations and data reloads.
;* It is based on microcontroller register file to the maximum.
;* In real users projects available abundant digit capacity allows to avoid
;* overflow and reduces inaccuracy of rounding errors in chain calculations.
;* Included macro definitions will increase readability of assembler source
;* at bit by bit and multibyte data operations inside AVR software model
;*
;* DESCRIPTION
;* This Application Note lists:
;*   i) Math32 subroutines for the following:
;*	Add/Subtract/Multiply/Divide/Complement 32 bits operands,
;*	Binary 16 & 24 bits operand to/back BCD conversion,
;*	Binary 32 bits operand to BCD conversion,
;*	Initialization of data memory on a pattern,
;*	Load/Store group of registers from/to data space;
;*  ii) macro definitions call mathematical and data transfer subroutines;
;* iii) useful general macroinstructions for the AVR 8-Bit RISC family
;*
;* "ADD32"      Add without Carry       Rd32 = Rd32 + Rr32
;* "SUB32"      Subtract without Carry  Rd32 = Rd32 - Rr32
;* "MUL32"      Multiply Unsigned       Rd64 = Rd32 * Rr32
;* "DIV32"      Divide Unsigned         Rd32 = Rd32 / Rr32 (Rd64)
;* "COM32"      One's Complement        Rd32 = 0xffffffff - Rd32
;* "NEG32"      Two's Complement        Rd32 = 0x00000000 - Rd32
;* "BCD2bin"    BCD to Binary 16        Rd16 = Rd24|Rr24
;* "BCD3bin"    BCD to Binary 24        Rd24 = Rd32|Rr32
;* "Bin2BCD"    Binary 16 to BCD        Rd24 = Rd16|Rr16
;* "Bin3BCD"    Binary 24 to BCD        Rd32 = Rd24|Rr24
;* "Bin4BCD"    Binary 32 to BCD        Rd40 = Rd32|Rr32 || hwrd(Rr32)&Rd16
;* "MathMem"    Init Data Memory        (MA) = 0x00|0xff
;* "MathLoad"   Load Registers          Rd32|Rr32 = (MA)
;* "MathSave"   Store Registers         (MA) = Rd32|Rd64
;*
;* Rd64: destination registers (8) in the register file
;* Rd32: destination (and source) registers (4) in the register file
;* Rr32: source registers (4) in the register file
;* (MA): address for access to variable in the internal memory (SRAM)
;* Note: Math32 use high registers, r0 and lower 512 bytes of data space,
;*    so Rd64=r20:r27, Rd32=r20:r23, Rd24=r20:r22, Rd16=r20:r21,
;*	 Rd40=r20:r24, Rr32=r16:r19, Rr24=r16:r18, Rr16=r16:r17, MA=0:511
;*
;* Number of words & cycles (Min|Max)		c o m m e n t s
;* "ADD32"      6    4|5    Size of Add32sign
;* "SUB32"     16    6|15   Size of Sub32sign
;* "MUL32"     24  460|556  Size of Mul32b, based on AVR200 16x16 unsigned
;* "DIV32"     28  528|688  Size of Div32b, based on AVR200 16/16 unsigned
;* "COM32"      5    4|4    Part of Sub32
;* "NEG32"      9    8|8    Part of Sub32
;* "BCD2bin"   26   86|89   Equivalent of AVR204, but smaller & quicker
;* "BCD3bin"   43   38|402  Different from BCD2bin translation algorithm
;* "Bin2BCD"   22   19|177  Equivalent of AVR204, but smaller & much faster
;* "Bin3BCD"   21   36|366  In the form of preamble for Bin2BCD
;* "Bin3BCD"   40   36|333  All-sufficient expansion of Bin2BCD
;* "Bin4BCD"   37  515|671  Based on AVR204 16-bit Bin to BCD conversion
;* "Bin4BCD"   48  874|878  All-sufficient transform instead of pre-Bin4BCD
;* "MathMem"   10    7|645  Size of MathMemLimit, max cycle for 128 bytes
;* "MathLoad"  15   41|46   Size and max cycle for Rr32 load
;* "MathSave"  14   13|78   Size and max cycle for Rd64 save
;* In total:  350 words     Usually +7 cycles: rcall & ret
;*
;* All routines are Code Size` optimized implementations and debugged with
;* macrocode for AVR macro assembler version 1.30 (Jan 27 1999 01:30:00) &
;*	       AVR32 macro assembler version 1.30 (Sep	8 1999 01:30:00).
;*    However, AVR32 macro assembler version 1.54 (Nov 14 2001 14:05:48) &
;*	       AVR32 macro assembler version 1.56 (May	6 2002 14:54:01)
;* generate dummy warnings: Register already defined by the .DEF directive
;* (command option for disable this kind of warning as yet is absent...)
;*			   CheckIt with AVR Studio !
;* NOTE
;* ` Bin4BCD transformations has partial loop optimization for speed-up
;* While using Math32, it is important to consider the allocation of the
;* microcontroller resources available for the program. It is required:
;* - to use r0,r16..r31 with Math32;
;* - to allocate variables used in calculation in the bottom of the memory;
;* - to use T flag as a sign bit (input, output and temporary),
;*   if you need to operate negative numbers or up-down overflow error
;*
;* VERSION
;* 1.0 Original version (in use starting with 1999.12.22)
;* 1.1 Fixed precedence bugs if macroparameter is an assembler expression
;* 1.2 Modify CBF & SBF & IBF macrocalls
;* 1.3 Full modification mathematical and data transfer macronotation
;* 1.4 Optimaze for speed and code size Mul32 & Div32 & BCD2bin & Bin2BCD
;* 2.0 Version for publication (added description, note and demo sections)
;* 2.1 Updated Bin2BCD, added Bin4BCD conversion & XCH macrocall
;* 2.2 Added functionally closed modifications of Bin3&4BCD translation
;* 2.3 Added BCD3bin conversion, normalize the comment of Bin3&4BCD
;*
;* DEMO
;* section below is a sample of macrocalls and not an ordinary Math32 usage

#include <avr/io.h>

;***************************************************************************
;*
;* Bin2BCD == 16-bit Binary to BCD conversion
;*
;* fbinL:fbinH	>>>  tBCD0:tBCD1:tBCD2
;*     hex		    dec
;*   r16r17	>>>	 r20r21r22
;*
;***************************************************************************
	fbinL	=16	; binary value Low byte
	fbinH	=17	; binary value High byte
	tBCD0	=20	; BCD value digits 0 and 1
	tBCD1	=21	; BCD value digits 2 and 3
	tBCD2	=22	; BCD value digit 4 (MSD is lowermost nibble)

Bin2BCD:
Bin2BCD16:	ldi	tBCD2,0xff	;initialize digit 4
binbcd_4:	inc	tBCD2		;
		subi	fbinL,0x10;low(10000);subiw fbin,10000
		sbci	fbinH,0x27;high(10000)
		brcc	binbcd_4	;
		ldi	tBCD1,0x9f	;initialize digits 3 and 2
binbcd_3:	subi	tBCD1,0x10	;
		subi	fbinL,0x18;low(-1000);addiw fbin,1000
		sbci	fbinH,0xfc;high(-1000)
		brcs	binbcd_3	;
binbcd_2:	inc	tBCD1		;
		subi	fbinL,0x64;low(100)	;subiw fbin,100
		sbci	fbinH,0x00;high(100) ;
		brcc	binbcd_2	;
		ldi	tBCD0,0xa0	;initialize digits 1 and 0
binbcd_1:	subi	tBCD0,0x10	;
		subi	fbinL,-10	;addi fbin,10
		brcs	binbcd_1	;
		add	tBCD0,fbinL	;LSD
binbcd_ret: ret

		ret			;




;***************************************************************************
;*
;* Bin4BCD == 32-bit Binary to BCD conversion
;*
;* fbin0:fbin1:fbin2:fbin3  >>>  tBCD0:tBCD1:tBCD2:tBCD3:tBCD4
;*	     hex			      dec
;*	 r16r17r18r19	    >>> 	r20r21r22r23r24
;*
;***************************************************************************
	fbin0	=16	; binary value byte 0 (LSB)
	fbin1	=17	; binary value byte 1
	fbin2	=18	; binary value byte 2
	fbin3	=19	; binary value byte 3 (MSB)
	tBCD0	=20	; BCD value digits 0 and 1
	tBCD1	=21	; BCD value digits 2 and 3
	tBCD2	=22	; BCD value digits 4 and 5
	tBCD3	=23	; BCD value digits 6 and 7
	tBCD4	=24	; BCD value digits 8 and 9 (MSD)
	TEMP=16
/********************
********************/
Bin4BCD:

Bin4BCD16:	clr	tBCD0		;initial result (5 bytes)
		clr	tBCD1		;	& shift
		clr	tBCD2		;	       loop
		ldi	tBCD3,0xfe	;		   counter
		ldi	tBCD4,0xff	;			  too
		rjmp	binbcd_jump	;for speed-up and skip of MSD corr
binbcd_876:	subi	tBCD4,-0x03	;add 0x03 to digit 8 only
		sbrs	tBCD4,3 	;if bit 3 clear
		subi	tBCD4,0x03	;	sub 3
		subi	tBCD3,-0x33	;add 0x33 to digits 7 and 6
		sbrs	tBCD3,3 	;if bit 3 clear
		subi	tBCD3,0x03	;	sub 3
		sbrs	tBCD3,7 	;if bit 7 clear
		subi	tBCD3,0x30	;	sub $30
binbcd_54:	subi	tBCD2,-0x33	;add 0x33 to digits 5 and 4
		sbrs	tBCD2,3 	;if bit 3 clear
		subi	tBCD2,0x03	;	sub 3
		sbrs	tBCD2,7 	;if bit 7 clear
		subi	tBCD2,0x30	;	sub $30
binbcd_3210:	subi	tBCD1,-0x33	;add 0x33 to digits 3 and 2
		sbrs	tBCD1,3 	;if bit 3 clear
		subi	tBCD1,0x03	;	sub 3
		sbrs	tBCD1,7 	;if bit 7 clear
		subi	tBCD1,0x30	;	sub $30
		subi	tBCD0,-0x33	;add 0x33 to digits 1 and 0
		sbrs	tBCD0,3 	;if bit 3 clear
		subi	tBCD0,0x03	;	sub 3
		sbrs	tBCD0,7 	;if bit 7 clear
		subi	tBCD0,0x30	;	sub $30
binbcd_jump:	lsl	fbin0		;
		rol	fbin1		;
		rol	fbin2		;
		rol	fbin3		;shift input value
		rol	tBCD0		;through all bytes
		rol	tBCD1		;
		rol	tBCD2		;
		rol	tBCD3		;
		rol	tBCD4		;
		brcs	binbcd_3210	;16_lsl w/o correction of dig_87654
		inc	fbin0		;
		brpl	binbcd_54	;+7_lsl w/o correction of dig_876
		sbrs	fbin2,0 	;
		rjmp	binbcd_876	;32_lsl in total (fbin = 0x1ffff)
		ret			;
/////////////////////////////////////////////////////////////////////

.global _itoa16_ASM
_itoa16_ASM:
		PUSH	R16
		PUSH	R17
		PUSH	R20
		PUSH	R21
		PUSH	R22
		LDS		fbinL,num16
		LDS		fbinH,num16+1
		RCALL   Bin2BCD
				
		MOV		TEMP,R20
		ANDI	R20,0X0F
		SUBI	r20,-0x30
		STS		abuff+4,r20
				
		ANDI	TEMP,0XF0
		SWAP	TEMP
		SUBI	TEMP,-0x30
		STS		abuff+3,TEMP

		MOV		TEMP,R21
		ANDI	R21,0X0F
		SUBI	r21,-0x30
		STS		abuff+2,R21

		ANDI	TEMP,0XF0
		SWAP	TEMP
		SUBI	r16,-0x30
		STS		abuff+1,TEMP

		MOV		TEMP,R22
		ANDI	R22,0X0F
		SUBI	r22,-0x30
		STS		abuff,R22

		LDI		TEMP,0
		STS		abuff+5,TEMP

		POP		R22
		POP		R21
		POP		R20
		POP		R17
		POP		R16
		RET

.global _itoa32_ASM
_itoa32_ASM:
		PUSH	R16
		PUSH	R17
		PUSH	R18
		PUSH	R19
		PUSH	R20
		PUSH	R21
		PUSH	R22
		PUSH	R23
		PUSH	R24
		LDS		fbin0,num32
		LDS		fbin1,num32+1	
		LDS		fbin2,num32+2
		LDS		fbin3,num32+3
		RCALL   Bin4BCD
				
		MOV		TEMP,R20
		ANDI	R20,0X0F
		SUBI	r20,-0x30
		STS		abuff+9,r20
				
		ANDI	TEMP,0XF0
		SWAP	TEMP
		SUBI	TEMP,-0x30
		STS		abuff+8,TEMP
//
		MOV		TEMP,R21
		ANDI	R21,0X0F
		SUBI	r21,-0x30
		STS		abuff+7,R21

		ANDI	TEMP,0XF0
		SWAP	TEMP
		SUBI	r16,-0x30
		STS		abuff+6,TEMP
//
		MOV		TEMP,R22
		ANDI	R22,0X0F
		SUBI	r22,-0x30
		STS		abuff+5,R22

		ANDI	TEMP,0XF0
		SWAP	TEMP
		SUBI	r16,-0x30
		STS		abuff+4,TEMP
//
		MOV		TEMP,R23
		ANDI	R23,0X0F
		SUBI	r23,-0x30
		STS		abuff+3,R23

		ANDI	TEMP,0XF0
		SWAP	TEMP
		SUBI	r16,-0x30
		STS		abuff+2,TEMP
//
		MOV		TEMP,R24
		ANDI	R24,0X0F
		SUBI	r24,-0x30
		STS		abuff+1,R24

		ANDI	TEMP,0XF0
		SWAP	TEMP
		SUBI	r16,-0x30
		STS		abuff+0,TEMP

		LDI		TEMP,0
		STS		abuff+10,TEMP

		POP		R24
		POP		R23
		POP		R22
		POP		R21
		POP		R20
		POP		R19
		POP		R18
		POP		R17
		POP		R16
		RET

