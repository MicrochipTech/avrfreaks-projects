
/**** A P P L I C A T I O N   N O T E	A V R ? ? ? ************************
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
*/

#include	 <mega32.h>
#include	"bin2ascii.h"


void utoa16(unsigned int num,char *buff)
{
/****************************************************************************
;*
;* Bin2BCD == 16-bit Binary to BCD conversion
;*
;* Important: You have to disable the autamtic global register allocation in the compiler settings.
;*            Only (R0,R1,R22,R23 ,R24,R25 ,R26,R27,R30,R31) can be used in ASM routines.
;*            you have to save the other registers in the stack if wanna use them.
;***************************************************************************/
#asm

#define	fbinL	r30	; binary value Low byte
#define	fbinH	r31	; binary value High byte
#define	temp    r22
#define	tBCD0	r24	; BCD value digits 0 and 1
#define	tBCD1	r25	; BCD value digits 2 and 3
#define	tBCD2	r26	; BCD value digit 4 (MSD is lowermost nibble)

    ldd     r31,Y+3 ;//loading the unsigned integer
    ldd     r30,y+2 ;
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
		brcs	binBCD_1	;
		add	tBCD0,fbinL	;LSD
binBCD_ret:

        ldd     r31,y+1// loading buffer's address
        ld      r30,y

		MOV		temp,tBCD0
		ANDI	tBCD0,0X0F
		SUBI	tBCD0,-0x30
        push    tBCD0

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

		MOV		temp,tBCD1
		ANDI	tBCD1,0X0F
		SUBI	tBCD1,-0x30
        push    tBCD1

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

		MOV		temp,tBCD2
		ANDI	tBCD2,0X0F
		SUBI	tBCD2,-0x30
        push    tBCD2

        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp

#endasm
}

void utoa32(unsigned long num,char *buff)
{
/***************************************************************************
;*
;* Bin4BCD == 32-bit Binary to BCD conversion
;*
;* Important:   You have to disable the autamtic global register allocation in the compiler settings.
;*              Only (R0,R1,R22,R23 ,R24,R25 ,R26,R27,R30,R31) can be used in ASM routines.
;*              you have to save the other registers in the stack if wanna use them.
;*
;**************************************************************************/
//R0,R1, R22,R23  R24,R25 ,R26,R27   R30,R31
#asm
#define	fbin0	r22	; binary value byte 0 (LSB)
#define	fbin1	r23	; binary value byte 1
#define	fbin2	r24	; binary value byte 2
#define	fbin3	r25	; binary value byte 3 (MSB)
#define	tBCD0	r16	; BCD value digits 0 and 1
#define	tBCD1	r17	; BCD value digits 2 and 3
#define	tBCD2	r18	; BCD value digits 4 and 5
#define	tBCD3	r19	; BCD value digits 6 and 7
#define	tBCD4	r20	; BCD value digits 8 and 9 (MSD)
#define	TEMP    r22


Bin4BCD:
        push    r16
        push    r17
        push    r18
        push    r19
        push    r20

        ldd     fbin3,Y+5 ;//loading the unsigned long
        ldd     fbin2,y+4 ;
        ldd     fbin1,Y+3 ;
        ldd     fbin0,y+2 ;

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


        ldd     r31,y+1//loading the buffer's address
        ld      r30,y

		MOV		temp,tBCD0
		ANDI	tBCD0,0X0F
		SUBI	tBCD0,-0x30
        push    tBCD0

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

		MOV		temp,tBCD1
		ANDI	tBCD1,0X0F
		SUBI	tBCD1,-0x30
        push    tBCD1

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

		MOV		temp,tBCD2
		ANDI	tBCD2,0X0F
		SUBI	tBCD2,-0x30
        push    tBCD2

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

		MOV		temp,tBCD3
		ANDI	tBCD3,0X0F
		SUBI	tBCD3,-0x30
        push    tBCD3

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

		MOV		temp,tBCD4
		ANDI	tBCD4,0X0F
		SUBI	tBCD4,-0x30
        push    tBCD4

		ANDI	temp,0XF0
		SWAP	temp
		SUBI	temp,-0x30
        push    temp

        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
        pop     temp
        st      z+,temp
//Recovering the registers //Other registers will recover by the compiler
        pop     r20
        pop     r19
        pop     r18
        pop     r17
        pop     r16
#endasm
}