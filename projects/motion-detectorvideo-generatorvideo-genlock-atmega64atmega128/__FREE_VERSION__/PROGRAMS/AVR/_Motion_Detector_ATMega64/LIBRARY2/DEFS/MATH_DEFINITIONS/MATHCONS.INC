;<$DS_HEADER>
;+--------------------------------------------------------------------------------+              
;|João D´Artagnan A. Oliveira Programmer and Author; 				  |
;|										  |
;| All rights reserved.								  |
;|										  |
;|   Redistribution and use in source and binary forms, with or without		  |
;|  modification, are permitted provided that the following conditions are met:	  |
;|										  |
;|   * Redistributions of source code must retain the above copyright		  |
;|     notice, this list of conditions and the following disclaimer.		  |
;|										  |
;|   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  |
;|   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    |
;|   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   |
;|   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     |
;|   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	  |
;|   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF	  |
;|   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS	  |
;|   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN	  |
;|   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)	  |
;|   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   |
;|   POSSIBILITY OF SUCH DAMAGE.						  |
;|										  |
;|   Brasília, Brazil November 30,2015 	06:28 am				  |
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;+--------------------------------------------------------------+
;| DOUBLE PRECISION FLOAT POINT 	I			|
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA				|
;| DATE:24/05/2001 VERSION 1.0					|
;>--------------------------------------------------------------<
;| D O U B L E  F L O A T  P O I N T  R O U T I N E S		|
;| C O N S T A N T S, R E G I S T E R S  A N D  S R A M 	|
;| D E F I N I T I O N S					|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+

#ifndef __MATH_DEFINITIONS__

	#define	__MATH_DEFINITIONS__

	.message "+------------------+"
	.message "|                  |"
	.message "| MATH DEFINITIONS |"
	.message "|                  |"
	.message "+------------------+"


	.EQU	_DF_STR_BUF_SIZE = 1+1+16+1+1+2	;total space for string conversions
						;signal(1)+point(1)+mantisa(16)+exponent signal(1)+exponent signal(1)+exponent(2)
	.EQU	_FDOUBLE_STACK_SIZE = 8		;defalt size of float point stack

	;+-------------------------+
	;! CONSTANTS DEFINITION	   !
	;+-------------------------+

	.EQU	_FDOUBLE	= 8		;size of float point double
	.EQU	_FSINGLE	= 4		;size of float point single
	.EQU	_FBCD		= 8		;size of float point in BCD format
	.EQU	_FSTRING	= _DF_STR_BUF_SIZE ;size of string formated into memory (huge memory comsupition)

	.EQU	_FBIAS   = 0x81			;float point BIAS
	.EQU	_FPOK    = 0			;float point Ok
	.EQU	_FEOVER  = 1			;float point overflow +
	.EQU	_FEUNDER = 2			;float point underflow
	.EQU	_FEDIV0  = 3			;float point division by zero
	.EQU	_FEILLEG = 4			;float point illegal

	;+----------------+
	;! USED REGISTERS !
	;+----------------+

	;+---------------------------------
	;| Double Float Point Operand 1
	;+---------------------------------

;	.DEF	_op1_0	= r0			;1st operator mantisa byte 1
;	.DEF	_op1_1	= r1			;1st operator mantisa byte 2
;	.DEF	_op1_2	= r2			;1st operator mantisa byte 3
;	.DEF	_op1_3	= r3			;1st operator mantisa byte 4
;	.DEF	_op1_4	= r4			;1st operator mantisa byte 5
;	.DEF	_op1_5	= r5			;1st operator mantisa byte 6
;	.DEF	_op1_s	= r6			;1st operator mantisa signal byte 7
;	.DEF	_op1_e	= r7			;1st operator exponent byte 8

	;+---------------------------------
	;| Double Float Point Operand 2
	;+---------------------------------

;	.DEF	_op2_0	= r8			;2st operator mantisa byte 1
;	.DEF	_op2_1	= r9			;2st operator mantisa byte 2
;	.DEF	_op2_2	= r10			;2st operator mantisa byte 3
;	.DEF	_op2_3	= r11			;2st operator mantisa byte 4
;	.DEF	_op2_4	= r12			;2st operator mantisa byte 5
;	.DEF	_op2_5	= r13			;2st operator mantisa byte 6
;	.DEF	_op2_s	= r14			;2st operator mantisa signal byte 7
;	.DEF	_op2_e	= r15			;2st operator exponent byte 8

	;+---------------------------------
	;| Double Float Point Accumulator
	;+---------------------------------

;	.DEF	_acc_0	= r18			;accumulator mantisa byte 1 same register Temp
;	.DEF	_acc_1	= r19			;accumulator mantisa byte 2 save register TempH
;	.DEF	_acc_2	= r20			;accumulator mantisa byte 3
;	.DEF	_acc_3	= r21			;accumulator mantisa byte 4
;	.DEF	_acc_4	= r22			;accumulator mantisa byte 5
;	.DEF	_acc_5	= r23			;accumulator mantisa byte 6
;	.DEF	_acc_s	= r26			;accumulator mantisa signal byte 7 (XL register)
;	.DEF	_acc_e	= r27			;accumulator exponent byte 8	   (XH register)

	.DSEG
		_MATH_VARIABLES:
			_MATH_VARIABLES_START:
;BELLOW 3 COMMENTS LINES ARE PLACED BECAUSE EARTH INVADER GAMES REQUERID MUCH MEMORY (RESTORE IF USED FLOAD ROUTINES) #########################
;			_DF_MAC:		.BYTE	_FDOUBLE	;mantisa accumulator
;			_DF_P10:		.BYTE	_FDOUBLE	;power of ten variable
;			_DF_BCD:		.BYTE	_FBCD		;float point codec into BCD digits
			_IS_SIZE:		.BYTE   1		;Define string output size of integer conversion
									;high nibble integer part size 0..15
									;low nibble decimal part size 0..15
			_DF_EAC:		.BYTE	1		;exponent accumulator
			_DF_FLAGS:		.BYTE	1		;string flags
			_DF_FMASK:		.BYTE	1		;mask definition for float point formating
			_IS_MASK:					;BIT 0=1 + sign for positive numbers
									;    1=1 use thousand separation char
									;    2=1 separation char is (point) else (comma)
									;    3=1 round result 0 no round result
			_DF_FS:			.BYTE	1		;0X00 source is FLASH 0x01 source is SRAM
			_DF_FRAC:		.BYTE	1		;this variable is used to say that FDIV is 
									;fractionary when value is 0xff
			_DF_FSREG:		.BYTE	1		;hold a STATUS REGISTER flags of float point comparation

	;below line requerid much memory for use EARTH Invader game i replaced to only 6 byte #################################
	;		_DF_STR:		.BYTE	_DF_STR_BUF_SIZE ;size for number to string conversions
			_DF_STR:		.BYTE	_BYTE*6
		_MATH_VARIABLES_END:
	.CSEG
	;+------------------------------+
	;| Formatting of _DF_BCD Fields |
	;+------------------------------+
	
	;	+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	;	|byte 0 |byte 1 |byte 2 |byte 3 |byte 4 |byte 5 |byte 6 |byte 7 |
	;	+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	;	|d00|d01|d02|d03|d04|d05|d06|d07|d08|d09|d10|d11|d12|d13|e00|e01|
	;	+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	;	  H   L   H   L   H   L   H   L   H   L   H   L   H   L   H   L
	;
	;H - High nibble
	;L - Low nibble
	;d - bcd digits
	;e - bcd exponent digits
	;bit 7 of byte 7 mantisa signal
	;bit 6 of byte 7 exponent signal

	;+------------------------------+
	;| Bit description of _DF_FLAGS |
	;+------------------------------+

	.EQU	_DF_BIT_IDS		= 0		;0 signalizing mantisa signal
							;1 signalizing exponent signal
	.EQU	_DF_BIT_MS		= 1		;0 positive mantisa
							;1 negative mantisa
	.EQU	_DF_BIT_ES		= 2		;0 positive exponent
							;1 negative exponent
	.EQU	_DF_BIT_DP		= 3		;0 point not finded
							;1 point finded
	.EQU	_DF_BIT_EP		= 4		;0 extract mantisa digits
							;1 extract exponent digits

	;+------------------------------+
	;| Bit description of _DF_FMASK |
	;+------------------------------+

	.EQU	_DF_BIT_FTYPE		= 0		;0 Normal formatting
							;1 Float formatting
	.EQU	_DF_BIT_PLUS		= 1		;0 space for positive number
							;1 plus signal for positive number
	.EQU	_DF_BIT_DEC		= 2		;0 decimal point is comma
							;1 decimal point is point
	.EQU	_DF_BIT_SEP		= 3		;0 no thousand separator
							;1 thousand separator
	.EQU	_DF_BIT_ROUND		= 4		;0 no round number
							;1 round number
	;+------------------------------+
	;| Integer constants		|
	;+------------------------------+

	.EQU	_PLUS_SIGN		= 1		;constants used by _I_USE_PLUS routine
	.EQU	_NO_PLUS_SIGN		= 0

	.EQU	_THOUSAND_SEPARATOR	= 1		;constants used by _I_USE_THOUSAND routine
	.EQU	_NO_THOUSAND_SEPARATOR	= 0

	.EQU	_COMMA_CHARACTER	= 0		;constants used by _I_USE_SEP_CHAR
	.EQU	_POINT_CHARACTER	= 1

	.EQU	_IS_MASK_BIT_SIGNAL 	= 0		;Contants used byte _IS_MASK 
	.EQU	_IS_MASK_BIT_THOUSAND 	= 1
	.EQU	_IS_MASK_BIT_SEPCHAR	= 2

	.EQU	_IS_BUFFER_SIZE		= 15

	;+--------------------+
	;| float point flags  |
	;+--------------------+

	.EQU	_FF_CARRY		= 0
	.EQU	_FF_ZERO		= 1

	;+--------------------+
	;| Formatting samples |
	;+--------------------+
	;
	;NORMAL
	;
	;    1,02		normal with 2 decimal digits
	;  -15,078		normal negative with 3 dicimal digits
	;+100			normal with plus sign for positive numbers no decimal digits
	;8.127,7		normal with separation mark in this case point 
	;7,086.47		normal with separation mark in this case comma
	;
	;FLOAT
	;
	; 1,05D+7		float with 2 decimal digits
	;+5,320D+15		float with plus sign for positive number 3 decimal digits
	; 7.31D-10		float with decimal is point 2 decimal digits
	;

.MACRO	_M_PUSH_MATH_VARIABLES
	pushw	Z					;save useds
	push	Acc
	push	AccT
	push	AccTH
	ldiz	_MATH_VARIABLES_START			;Z-->float stack start
	ldi	AccT,low(_MATH_VARIABLES_END-_MATH_VARIABLES_START)
	ldi	AccTH,high(_MATH_VARIABLES_END-_MATH_VARIABLES_START)
_M_PUSH00:
	ld	Acc,Z+					;get float data
	push	Acc					;put into avr stack
	subi	AccT,1
	sbci	AccTH,0
	brne	_M_PUSH00
.ENDMACRO

.MACRO	_M_POP_MATH_VARIABLES
	ldiz	_MATH_VARIABLES_END			;Z-->float stack end
	ldi	AccT,low(_MATH_VARIABLES_END-_MATH_VARIABLES_START)
	ldi	AccTH,high(_MATH_VARIABLES_END-_MATH_VARIABLES_START)
_M_POP00:
	pop	Acc					;get data form avr stack
	st	-Z,Acc
	subi	AccT,1
	sbci	AccTH,0
	brne	_M_POP00
	pop	AccTH					;restore useds
	pop	AccT
	pop	AcC
	popw	Z
.ENDMACRO

#endif
.exit
