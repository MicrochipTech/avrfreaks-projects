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
;| DATE:24/2/2004 VERSION 1.0						|
;>----------------------------------------------------------------------<
;| I N T E G E R  R O U T I N E S	     				|
;| D I V I D E  							|
;+----------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM			|
;+----------------------------------------------------------------------+
;+----------------------------------------------------------------------+
;| DIV_s32s32s32							|
;| Divide signed 32bits/unsigned 32bits result unsigned 32bits		|
;|								  	|
;| Input  : R0:R1:R2 :R3  Dividend				  	|
;|	    R4:R5:R6 :R7  Divisor				  	|
;| Output : R0:R1:R2 :R3  Quotient				  	|
;|	    R8:R9:R10:R11 Rest						|
;| Destroy: Flags						  	|
;| Time   : Min=566+3 rcall  Max=662+3 rcall			  	|
;+----------------------------------------------------------------------+
;corrigir sinal do resto <---- falta implementar 
#ifndef	__INTEGER_DIV_S32S32S32__
	#define __INTEGER_DIV_S32S32S32__
	.message "[ (MATH) INTEGER DIV S32S32S32 ]"
	;+----------------------+
	;| NECESSARY INCLUDES	|
	;+----------------------+
	#ifndef	__INTEGER_DIV_U32U32U32__
		.INCLUDE "MATH\INTEGER\DIV_U32U32U32\DIV_U32U32U32.INC"
	#endif
	_DIV_S32S32S32:
		push	Acc
		mov	Acc,r3			;divide signal
		eor	Acc,r7
		push	Acc
		tst	r3			;divident negative ?
		brpl	_DIV_S3200		;yes, branch
		com	r0			;else complement
		com	r1
		com	r2
		com	r3
		ser	Acc
		sub	r0,Acc
		sbc	r1,Acc
		sbc	r2,Acc
		sbc	r3,Acc
	_DIV_S3200:
		tst	r7			;divisor negative ?
		brpl	_DIV_S3210		;yes, branch
		com	r4			;else complement
		com	r5
		com	r6
		com	r7
		ser	Acc
		sub	r4,Acc
		sbc	r5,Acc
		sbc	r6,Acc
		sbc	r7,Acc
	_DIV_S3210:
		rcall	_DIV_U32U32U32
		pop	Acc			;restore signal division
		tst	Acc
		brpl	_DIV_S3220
		com	r0			;else complement
		com	r1
		com	r2
		com	r3
		ser	Acc
		sub	r0,Acc
		sbc	r1,Acc
		sbc	r2,Acc
		sbc	r3,Acc
	_DIV_S3220:
		pop	Acc
		ret	
#endif
.exit

