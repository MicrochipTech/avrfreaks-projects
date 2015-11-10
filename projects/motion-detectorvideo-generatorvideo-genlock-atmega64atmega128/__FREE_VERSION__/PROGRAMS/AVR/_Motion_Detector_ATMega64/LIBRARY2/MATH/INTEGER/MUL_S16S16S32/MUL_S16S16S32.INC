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
;| DATE:2/4/2004 VERSION 1.0						|
;>----------------------------------------------------------------------<
;| I N T E G E R  R O U T I N E S	     				|
;| M U L T I P L Y 							|
;+----------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM			|
;+----------------------------------------------------------------------+
;+----------------------------------------------------------------------+
;| MUL_S16S16S32							|
;| Multiply signed 16bits x signed 16bits result signed 32bits		|
;|								  	|
;| Input  : R0:R1  1st operand				  		|
;|	    R2:R3  2nd operand				  		|
;| Output : R4:R5:R6:R7 Result					  	|
;| Destroy: R0,R1,R2,R3,Flags					  	|
;| Time   : Max=284+3 rcall					  	|
;+----------------------------------------------------------------------+
#ifndef	__INTEGER_MUL_S16S16S32__
	#define __INTEGER_MUL_S16S16S32__
	.message "[ (MATH) INTEGER MUL S16S16S32 ]"
	;+----------------------+
	;| NECESSARY INCLUDES	|
	;+----------------------+
	#ifndef	__INTEGER_MUL_U16U16U32__
		.INCLUDE "MATH\INTEGER\MUL_U16U16U32\MUL_U16U16U32.INC"
	#endif		
	_MUL_S16S16S32:
		push	Acc			;Save useds
		mov	Acc,r1			;multiply signals
		eor	Acc,r3
		push	Acc			;for future use
		tst	r1			;see 1st operand signal
		brpl	_MUL_S16S16S3200	;branch if positive
		com	r0			;else complement
		com	r1
		ldi	Acc,1
		add	r0,Acc
		ldi	Acc,0
		adc	r1,Acc
	_MUL_S16S16S3200:
		tst	r3			;see 2st operand signal
		brpl	_MUL_S16S16S3210	;branch if positive
		com	r2			;else complement
		com	r3
		ldi	Acc,1
		add	r2,Acc
		ldi	Acc,0
		adc	r3,Acc
	_MUL_S16S16S3210:
		rcall	_MUL_U16U16U32		;multiply
		pop	Acc			;restore sinals
		tst	acc
		brpl	_MUL_S16S16S3220	;branch if positive
		com	r4			;else complement result
		com	r5
		com	r6
		com	r7
		ldi	acc,1
		add	r4,Acc
		ldi	acc,0
		adc	r5,Acc
		adc	r6,Acc
		adc	r7,Acc
	_MUL_S16S16S3220:
		pop	Acc			;restore useds
		ret
#endif
.exit

