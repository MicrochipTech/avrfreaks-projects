;<$DS_HEADER>
;+--------------------------------------------------------------------------------+              
;|Jo�o D�Artagnan A. Oliveira Programmer and Author; 				  |
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
;|   Bras�lia, Brazil November 30,2015 	06:28 am				  |
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;+------------------------------------------------------------------------------+
;|										|
;| DATE:24/01/2005 VERSION 1.0							|
;>------------------------------------------------------------------------------<
;| I N T E G E R  R O U T I N E S	     					|
;| B I N A R Y  T O  B C D		 					|
;+------------------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM				|
;+------------------------------------------------------------------------------+
;+----------------------------------------------------------------------+
;| _BCDTOB								|
;| Convert Binary to BCD "Binary Codec Decimal"			 	|
;|								  	|
;| Input  : Acc byte input					  	|
;| Output : Acc BCD						  	|
;|	    cy=1 if BCD>99						|
;|									|
;| Input format								|
;|									|
;|	|      binary number 8 bits     |				|
;|	+---+---+---+---+---+---+---+---+				|
;|	| b7| b6| b5| b4| b3| b2| b1| b0|				|
;|	+---+---+---+---+---+---+---+---+				|
;|	if binary input exced 99 then output is saturated to 99 and 	|
;|	carry is set							|
;|									|
;| output format							|
;|									|
;|	|-Bcd digit hi -|- Bcd digit lo |				|
;|	+---+---+---+---+---+---+---+---+				|
;|	| d3| d2| d1| d0| d3| d2| d1| d0|				|
;|	+---+---+---+---+---+---+---+---+				|
;|									|
;+----------------------------------------------------------------------+
#ifndef	__INTEGER_BTOBCD__
	#define __INTEGER_BTOBCD__
	.message "[ (MATH) INTEGER BTOBCD ]"
	_BTOBCD:
		cpi	acc,100			;input number < 100?
		brlo	_BTOBCD_00		;yes, branch
		sec				;else set overflow
		rjmp	_BTOBCD_30		;then exit
	_BTOBCD_00:
		push	AccH			;save AccH
		ldi	AccH,0			;Hi digit count
	_BTOBCD_10:
		subi	Acc,10			;subtrac from 10
		brcs	_BTOBCD_20		;branch if lower 0
		inc	AccH			;else Hi digit++
		rjmp	_BTOBCD_10
	_BTOBCD_20:
		subi	Acc,-10			;restore lo digit
		SWAP	AccH			;put into correctly postion
		or	Acc,AccH		;bound digits
		pop	AccH			;restore AccH
		clc				;set no overflow
	_BTOBCD_30:
		ret
#endif	
.exit

