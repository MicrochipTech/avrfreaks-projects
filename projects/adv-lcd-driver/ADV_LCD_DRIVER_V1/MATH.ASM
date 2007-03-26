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
;*
;***************************************************************************
;*
;* Bin2BCD == 16-bit Binary to BCD conversion
;*
;* fbinL:fbinH	>>>  tBCD0:tBCD1:tBCD2
;*     hex		    dec
;*   r16r17	>>>	 r20r21r22
;*
;***************************************************************************
.def	fbinL	=r16	; binary value Low byte
.def	fbinH	=r17	; binary value High byte
.def	tBCD0	=r20	; BCD value digits 0 and 1
.def	tBCD1	=r21	; BCD value digits 2 and 3
.def	tBCD2	=r22	; BCD value digit 4 (MSD is lowermost nibble)

Bin2BCD20:	mov	r16,r20 	;for compatibility with Math32
		mov	r17,r21 	;
Bin2BCD16:	ldi	tBCD2,0xff	;initialize digit 4
binbcd_4:	inc	tBCD2		;
		subi	fbinL,low(10000);subiw fbin,10000
		sbci	fbinH,high(10000)
		brcc	binbcd_4	;
		ldi	tBCD1,0x9f	;initialize digits 3 and 2
binbcd_3:	subi	tBCD1,0x10	;
		subi	fbinL,low(-1000);addiw fbin,1000
		sbci	fbinH,high(-1000)
		brcs	binbcd_3	;
binbcd_2:	inc	tBCD1		;
		subi	fbinL,low(100)	;subiw fbin,100
		sbci	fbinH,high(100) ;
		brcc	binbcd_2	;
		ldi	tBCD0,0xa0	;initialize digits 1 and 0
binbcd_1:	subi	tBCD0,0x10	;
		subi	fbinL,-10	;addi fbin,10
		brcs	binbcd_1	;
		add	tBCD0,fbinL	;LSD
binbcd_ret:	ret			;
.equ Bin2BCD=Bin2BCD20 ;default registers BIN to BCD call

;***************************************************************************
;*
;* Bin4BCD == 32-bit Binary to BCD conversion	[ together with Bin2BCD ]
;*
;* fbin0:fbin1:fbin2:fbin3  >>>  tBCD0:tBCD1:tBCD2:tBCD3:tBCD4
;*	     hex			      dec
;*	 r18r19r20r21	    >>> 	r20r21r22r23r24
;*
;***************************************************************************
.def	fbin0	=r18	; binary value byte 0 (LSB)
.def	fbin1	=r19	; binary value byte 1
.def	fbin2	=r20	; binary value byte 2
.def	fbin3	=r21	; binary value byte 3 (MSB)
.def	tBCD0	=r20	; BCD value digits 0 and 1 (same as fbin2)
.def	tBCD1	=r21	; BCD value digits 2 and 3 (same as fbin3)
.def	tBCD2	=r22	; BCD value digits 4 and 5
.def	tBCD3	=r23	; BCD value digits 6 and 7
.def	tBCD4	=r24	; BCD value digits 8 and 9 (MSD)

Bin4BCD:	rcall	Bin2BCD20	;
		clr	tBCD3		;initial highest bytes of result
		ldi	tBCD4,0xfe	;
binbcd_loop:	subi	tBCD0,-0x33	;add 0x33 to digits 1 and 0
		sbrs	tBCD0,3 	;if bit 3 clear
		subi	tBCD0,0x03	;	sub 3
		sbrs	tBCD0,7 	;if bit 7 clear
		subi	tBCD0,0x30	;	sub $30
		subi	tBCD1,-0x33	;add 0x33 to digits 3 and 2
		sbrs	tBCD1,3 	;if bit 3 clear
		subi	tBCD1,0x03	;	sub 3
		sbrs	tBCD1,7 	;if bit 7 clear
		subi	tBCD1,0x30	;	sub $30
		subi	tBCD2,-0x33	;add 0x33 to digits 5 and 4
		sbrs	tBCD2,3 	;if bit 3 clear
		subi	tBCD2,0x03	;	sub 3
		sbrs	tBCD2,7 	;if bit 7 clear
		subi	tBCD2,0x30	;	sub $30
		lsl	fbin0		;
		rol	fbin1		;shift lower word
		rol	tBCD0		;through all bytes
		rol	tBCD1		;
		rol	tBCD2		;
		rol	tBCD3		;
		rol	tBCD4		;
		brmi	binbcd_loop	;7 shifts w/o correction of MSD
		rol	fbinH		;since Bin2BCD fbinH = 0xff
		brcc	binbcd_ret	;  so as to do 16_lsl in total
		subi	tBCD3,-0x33	;add 0x33 to digits 7 and 6
		sbrs	tBCD3,3 	;if bit 3 clear
		subi	tBCD3,0x03	;	sub 3
		sbrs	tBCD3,7 	;if bit 7 clear
		subi	tBCD3,0x30	;	sub $30
		subi	tBCD4,-0x03	;add 0x03 to digit 8 only
		sbrs	tBCD4,3 	;if bit 3 clear
		subi	tBCD4,0x03	;	sub 3
		rjmp	binbcd_loop	;

;***************************************************************************
;*
;* Bin4BCD == 32-bit Binary to BCD conversion
;*
;* fbin0:fbin1:fbin2:fbin3  >>>  tBCD0:tBCD1:tBCD2:tBCD3:tBCD4
;*	     hex			      dec
;*	 r16r17r18r19	    >>> 	r20r21r22r23r24
;*
;***************************************************************************
.def	fbin0	=r16	; binary value byte 0 (LSB)
.def	fbin1	=r17	; binary value byte 1
.def	fbin2	=r18	; binary value byte 2
.def	fbin3	=r19	; binary value byte 3 (MSB)
.def	tBCD0	=r20	; BCD value digits 0 and 1
.def	tBCD1	=r21	; BCD value digits 2 and 3
.def	tBCD2	=r22	; BCD value digits 4 and 5
.def	tBCD3	=r23	; BCD value digits 6 and 7
.def	tBCD4	=r24	; BCD value digits 8 and 9 (MSD)

Bin4BCD20:	mov	r16,r20 	;for compatibility with Math32
		mov	r17,r21 	;
		mov	r18,r22 	;
		mov	r19,r23 	;
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

;***************************************************************************
;*
;* Bin3BCD == 24-bit Binary to BCD conversion	[ together with Bin2BCD ]
;*
;* fbin0:fbin1:fbin2  >>>  tBCD0:tBCD1:tBCD2:tBCD3
;*	  hex			     dec
;*     r16r17r18      >>>	r20r21r22r23
;*
;***************************************************************************
.def	fbin0	=r16	; binary value byte 0 (LSB)
.def	fbin1	=r17	; binary value byte 1
.def	fbin2	=r18	; binary value byte 2 (MSB)
.def	tBCD0	=r20	; BCD value digits 0 and 1
.def	tBCD1	=r21	; BCD value digits 2 and 3
.def	tBCD2	=r22	; BCD value digits 4 and 5
.def	tBCD3	=r23	; BCD value digits 6 and 7 (MSD)

Bin3BCD:	ldi	tBCD3,0xff		;initialize digits 7 and 6
binbcd_7:	inc	tBCD3			;
		subi	fbin0,byte1(10000*100)	;subit fbin,1000000
		sbci	fbin1,byte2(10000*100)	;
		sbci	fbin2,byte3(10000*100)	;
		brcc	binbcd_7		;
		subi	tBCD3,-6		; delete decimal correction
		sbrs	tBCD3,4 		; if NUMBER<10000000 always
		subi	tBCD3,6 		;
		ldi	tBCD2,0x9f		;initialize digits 5 and 4
binbcd_6:	subi	tBCD2,0x10		;
		subi	fbin0,byte1(-10000*10)	;addit fbin,100000
		sbci	fbin1,byte2(-10000*10)	;
		sbci	fbin2,byte3(-10000*10)	;
		brcs	binbcd_6		;
binbcd_5:	inc	tBCD2			;
		subi	fbin0,byte1(10000)	;subit fbin,10000
		sbci	fbin1,byte2(10000)	;
		sbci	fbin2,byte3(10000)	;
		brcc	binbcd_5		;
		rjmp	binbcd_3-1		;initialize digits 3 and 2

;***************************************************************************
;*
;* Bin3BCD == 24-bit Binary to BCD conversion
;*
;* fbin0:fbin1:fbin2  >>>  tBCD0:tBCD1:tBCD2:tBCD3
;*	  hex			     dec
;*     r16r17r18      >>>	r20r21r22r23
;*
;***************************************************************************
.def	fbin0	=r16	; binary value byte 0 (LSB)
.def	fbin1	=r17	; binary value byte 1
.def	fbin2	=r18	; binary value byte 2 (MSB)
.def	tBCD0	=r20	; BCD value digits 0 and 1
.def	tBCD1	=r21	; BCD value digits 2 and 3
.def	tBCD2	=r22	; BCD value digits 4 and 5
.def	tBCD3	=r23	; BCD value digits 6 and 7 (MSD)

Bin3BCD20:	mov	r16,r20 	;for compatibility with Math32
		mov	r17,r21 	;
		mov	r18,r22 	;
Bin3BCD16:	ldi	tBCD3,0xfa		;initialize digits 7 and 6
binbcd_107:	subi	tBCD3,-0x10		;
		subi	fbin0,byte1(10000*1000) ;subit fbin,10^7
		sbci	fbin1,byte2(10000*1000) ;
		sbci	fbin2,byte3(10000*1000) ;
		brcc	binbcd_107		;
binbcd_106:	dec	tBCD3			;
		subi	fbin0,byte1(-10000*100) ;addit fbin,10^6
		sbci	fbin1,byte2(-10000*100) ;
		sbci	fbin2,byte3(-10000*100) ;
		brcs	binbcd_106		;
		ldi	tBCD2,0xfa		;initialize digits 5 and 4
binbcd_105:	subi	tBCD2,-0x10		;
		subi	fbin0,byte1(10000*10)	;subit fbin,10^5
		sbci	fbin1,byte2(10000*10)	;
		sbci	fbin2,byte3(10000*10)	;
		brcc	binbcd_105		;
binbcd_104:	dec	tBCD2			;
		subi	fbin0,byte1(-10000)	;addit fbin,10^4
		sbci	fbin1,byte2(-10000)	;
		sbci	fbin2,byte3(-10000)	;
		brcs	binbcd_104		;
		ldi	tBCD1,0xfa		;initialize digits 3 and 2
binbcd_103:	subi	tBCD1,-0x10		;
		subi	fbin0,byte1(1000)	;subiw fbin,10^3
		sbci	fbin1,byte2(1000)	;
		brcc	binbcd_103		;
binbcd_102:	dec	tBCD1			;
		subi	fbin0,byte1(-100)	;addiw fbin,10^2
		sbci	fbin1,byte2(-100)	;
		brcs	binbcd_102		;
		ldi	tBCD0,0xfa		;initialize digits 1 and 0
binbcd_101:	subi	tBCD0,-0x10		;
		subi	fbin0,10		;subi fbin,10^1
		brcc	binbcd_101		;
		add	tBCD0,fbin0		;LSD
		ret				;


;Neg32:		subi	sub10,1 	;if result<0
;		sbci	sub11,0 	;   neg result
;		sbci	sub12,0 	;
;		sbci	sub13,0 	;   (dec result)
;Com32:		com	sub10		;	&
;		com	sub11		;   (com result)
;		com	sub12		;
;		com	sub13		;   return set carry after com
;Return32u:	ret			;

NEG16:
		SUBI	R16,1
		SUBI	R17,0
		COM		R16
		COM		R17
		RET

NEG24:
		SUBI	R16,1
		SUBI	R17,0
		SUBI	R18,0
		COM		R16
		COM		R17
		COM		R18
		RET


NEG32:
		SUBI	R16,1
		SUBI	R17,0
		SUBI	R18,0
		SUBI	R19,0
		COM		R16
		COM		R17
		COM		R18
		COM		R19
		RET
