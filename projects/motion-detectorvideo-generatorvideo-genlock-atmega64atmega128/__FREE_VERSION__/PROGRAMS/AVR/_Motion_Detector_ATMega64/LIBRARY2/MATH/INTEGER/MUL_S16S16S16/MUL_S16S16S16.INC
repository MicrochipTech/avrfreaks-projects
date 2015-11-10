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
;+--------------------------------------------------------------+---+-----------+
;|										|
;| DATE:17/12/2001 VERSION 1.0							|
;>------------------------------------------------------------------------------<
;| I N T E G E R  R O U T I N E S	     					|
;| M U L T I P L Y 								|
;+------------------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM				|
;+------------------------------------------------------------------------------+
;+----------------------------------------------------------------------+
;| MUL_S16S16S16							|
;| Multiply signed 16bits x signed 16bits result signed 16bits		|
;|								  	|
;| Input  : R0:R1 1st operand					  	|
;|	    R2:R3 2nd operand					  	|
;| Output : R4:R5 Result					  	|
;| Destroy: R0,R1,R2,R3,R6,Flags				  	|
;| Time   : Min=180+3 rcall  Max=211+3 rcall			  	|
;+----------------------------------------------------------------------+
#ifndef	__INTEGER_MUL_S16S16S16__
	#define __INTEGER_MUL_S16S16S16__
	;+----------------------+
	;| NECESSARY INCLUDES	|
	;+----------------------+
	#ifndef	__MUL_U16U16U16__
		.INCLUDE "MATH\INTEGER\MUL_U16U16U16\U16U16U16.INC"
	#endif
	.message "[ (MATH) INTEGER MUL S16S16S16 ]"
	_MUL_S16S16S16:
		push	Acc			;save Acc
		mov	R6,R1			;multiply signals
		eor	R6,R3			;R6 signal produts
		tst	R1			;see 1st Operand signal
		brpl	_MUL_S16S16S16_00	;positive , branch
		com	R0			;else complement 1st operant
		com	R1
		ldi	Acc,1
		add	R0,Acc
		ldi	Acc,0
		adc	R1,Acc
	_MUL_S16S16S16_00:
		tst	R3			;see 2nd Operand signal
		brpl	_MUL_S16S16S16_10	;positive , branch
		com	R2			;else complement 2st operant
		com	R3
		ldi	Acc,1
		add	R2,Acc
		ldi	Acc,0
		adc	R3,Acc
	_MUL_S16S16S16_10:
		rcall	_MUL_U16U16U16		;multiply unsigned
		tst	R6			;adjust result if negative
		brpl	_MUL_S16S16S16_20	;positive, branch
		com	R4			;complemente result
		com	R5
		ldi	Acc,1
		add	R4,Acc
		ldi	Acc,0
		Adc	R5,Acc
	_MUL_S16S16S16_20:
		pop	Acc			;Restore Acc
		ret
#endif
.exit

