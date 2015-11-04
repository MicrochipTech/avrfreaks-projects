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
;+------------------------------------------------------------------------------+
;|										|
;| DATE:24/01/2005 VERSION 1.0							|
;>------------------------------------------------------------------------------<
;| I N T E G E R  R O U T I N E S	     					|
;| S I G N E D  B Y T E  T O  S T R I N G 					|
;+------------------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM				|
;+------------------------------------------------------------------------------+
;+------------------------------------------------------------------------------+
;| _BCDTOB									|
;| Convert BCD "Binary Codec Decimal" to Binary			 		|
;|								  		|
;| Input  : AccH:Acc BCD					  		|
;| Output : AccH:Acc Binary					  		|
;|										|
;| Input format									|
;|										|
;|	/------------- AccH -----------\ /----------- Acc ------------->	|	
;|	|-Bcd digit hi -|- Bcd digit lo |-Bcd digit hi -|- Bcd digit lo |	|
;|	+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+	|
;|	| d3| d2| d1| d0| d3| d2| d1| d0| d3| d2| d1| d0| d3| d2| d1| d0|	|
;|	+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+	|
;|										|
;|	if bcd digits great than 9 the value is saturated to 9			|
;| Output format								|
;|										|
;|	|      binary number 8 bits     |					|
;|	+---+---+---+---+---+---+---+---+					|
;|	| b7| b6| b5| b4| b3| b2| b1| b0|					|
;|	+---+---+---+---+---+---+---+---+					|
;+------------------------------------------------------------------------------+
#ifndef	__INTEGER_BCDTOI__
	#define __INTEGER_BCDTOI__
	.message "[ (MATH) INTEGER BCDTOI ]"
	_BCDTOI:
		push	r0
		push	r1
		push	r2
		push	r3
		push	Temp
		clr	r0				;clear accumulator
		clr	r1
		mov	Temp,AccH			;get Hi digit of AccH
		swap	Temp
		andi	Temp,0x0f
		call	_BCDTOI_MUL10			;multiply by 10
		mov	Temp,AccH			;get Lo digit of AccH
		andi	Temp,0x0f
		call	_BCDTOI_MUL10			;multiply by 10
		mov	Temp,Acc			;get Hi digit of Acc
		swap	Temp
		andi	Temp,0x0f
		call	_BCDTOI_MUL10			;multiply by 10
		mov	Temp,Acc			;get Lo digit of Acc
		andi	Temp,0x0f
		call	_BCDTOI_MUL10
		movw	AccH:Acc,r1:r0			;put into appropriate registers
		pop	Temp
		pop	r3
		pop	r2
		pop	r1
		pop	r0
		ret
		;////////////////////////////////////////////////
		;/// MULTIPLAY r1:r0 by 10 and add Temp
		;/// Destroy: Temp
		;////////////////////////////////////////////////
	_BCDTOI_MUL10:
		lsl	r0				;r1:r0*2
		rol	r1
		movw	r3:r2,r1:r0			;save into r3:r2
		lsl	r0				;r1:r0*4
		rol	r1
		lsl	r0				;r1:r0*8
		rol	r1
		add	r0,r2				;r1:r0+=r3:r2
		adc	r1,r3
		add	r0,Temp
		ldi	Temp,0
		adc	r1,Temp
		ret
#endif	
.exit

