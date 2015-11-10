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
;+--------------------------------------------------------------+---+---+
;|									|
;| DATE:17/12/2001 VERSION 1.0						|
;>----------------------------------------------------------------------<
;| I N T E G E R  R O U T I N E S	     				|
;| M U L T I P L Y 							|
;+----------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM			|
;+----------------------------------------------------------------------+
;+----------------------------------------------------------------------+
;| MUL_U32U32U32							|
;| Multiply unsigned 32bits x unsigned 32bits result unsigned 32bits	|
;|								  	|
;| Input  : R0:R1:R2:R3   1st operand				  	|
;|	    R4:R5:R6:R7   2nd operand				  	|
;| Output : R8:R9:R10:R11 Result				  	|
;| Destroy: R0,R1,R2,R3,R4,R5,R6,R7,Flags			  	|
;| Time   : Min=397+3 rcall  Max=525+3 rcall			  	|
;+----------------------------------------------------------------------+
#ifndef	__INTEGER_MUL_U32U32U32__
	#define __INTEGER_MUL_U32U32U32__
	.message "[ (MATH) INTEGER MUL U32U32U32 ]"
	_MUL_U32U32U32:
		push	Acc			;Save useds
		clr	R8			;clear accumulator
		clr	R9
		clr	R10
		clr	R11
		ldi	Acc,32
	_MUL_U32U32U32_00:
		ror	R3			;get out bit of 1st operand
		ror	R2
		ror	R1
		ror	R0
		brcc	_MUL_U32U32U32_10	;branch if out bit zero
		add	R8,R4			;else accumulate
		adc	R9,R5
		adc	R10,R6
		adc	R11,R7
	_MUL_U32U32U32_10:
		lsl	R4			;next power
		rol	R5
		rol	R6
		rol	R7
		dec	Acc
		brne	_MUL_U32U32U32_00
		pop	Acc
		ret
#endif
.exit
	

