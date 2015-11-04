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
;| MUL_U16U16U16							|
;| Multiply unsigned 16bits x unsigned 16bits result unsigned 16bits	|
;|								  	|
;| Input  : R0:R1 1st operand					  	|
;|	    R2:R3 2nd operand					  	|
;| Output : R4:R5 Result					  	|
;| Destroy: R0,R1,R2,R3,Flags					  	|
;| Time   : Min=158+3 rcall  Max=174+3 rcall			  	|
;+----------------------------------------------------------------------+
#ifndef	__INTEGER_MUL_U16U16U16__
	#define __INTEGER_MUL_U16U16U16__
	.message "[ (MATH) INTEGER MUL U16U16U16 ]"
	_MUL_U16U16U16:
		push	Acc				;save Acc
		clr	R4				;clear accumulator
		clr	R5
		ldi	Acc,16				;bit counter
	_MUL_U16U16U16_00:
		ror	R1				;see 1st operant out bit
		ror	R0
		brcc	_MUL_U16U16U16_10		;zero ?, branch
		add	R4,R2				;else accumulate power
		adc	R5,R3
	_MUL_U16U16U16_10:
		lsl	R2				;next power of 2
		rol	R3
		dec	Acc				;until all bits done
		brne	_MUL_U16U16U16_00
		pop	Acc				;restore Acc
		ret
#endif
.exit

