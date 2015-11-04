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
;| DATE:24/05/2001 VERSION 1.0							|
;>--------------------------------------------------------------+---------------<
;| D O U B L E   F L O A T  P O I N T  R O U T I N E S		|
;| F L O A T  T O  I N T E G E R 				|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;| USE DFFTOL ROUTINE TO CONVERT FLOAT POINT TO INTEGER |
;| OR BYTE TO REDUCE AMOUNT OF PROGRAM MEMORY		|
;+------------------------------------------------------+
;+---------------------------------------+
;| CONVERT FLOAT POINT TO LONG INTEGER	 |
;|                                       |
;| Input  : Floating Accumulator         |
;| Output : Acc:AccH:AccT:AccTH long     |
;|	    cy=1 if overflow occur	 |
;| Destroy: Flags, Float Op1		 |
;|  					 |
;+---------------------------------------+
#ifndef __MATH_FLOAT_TO_LONG__
	#define	__MATH_FLOAT_TO_LONG__
	.message "[ (MATH) FLOAT TO LONG ]"
	_DFFTOL:
		cpi	_acc_e,0		;Floating Zero ?
		brne	_DFFTOL10		;no, branch
	_DFFTOL00:
		clr	Acc			;else clear result
		clr	AccH
		clr	AccT
		clr	AccTH
		clc
		ret
	_DFFTOL10:
		cpi	_acc_e,_FBIAS		;expoent lower 0 ?
		brlo	_DFFTOL00		;yes, branch
		cpi	_acc_e,(_FBIAS+32)	;expoent > 32
		brlo	_DFFTOL30		;no, branch
		ser	Acc			;assume max positive number
		ser	AccH
		ser	AccT
		ldi	AccTH,0x7f
		tst	_acc_s			;positive number ?
		brpl	_DFFTOL20
		ori	AccTH,0x80		;else max negative number
	_DFFTOL20:
		sec				;set overflow
		ret
	_DFFTOL30:
		mov	AccH,_acc_e		;AccH=Expoint without FBIAS
		subi	AccH,_FBIAS
		ldi	Acc,31
		sub	Acc,AccH		;Acc number of shifts i need
		mov	_op1_e,_acc_s		;save signal into op1
		mov	_op1_3,_acc_3
		mov	_op1_4,_acc_4
		mov	_op1_5,_acc_5
		mov	_op1_s,_acc_s
		ldi	AccH,0x80
		or	_op1_s,AccH		;set MSB of mantisa
	_DFFTOL40:
		tst	Acc			;conversion end ?
		breq	_DFFTOL50		;yes, done
		lsr	_op1_s
		ror	_op1_5
		ror	_op1_4
		ror	_op1_3
		dec	Acc			;shifts--
		rjmp	_DFFTOL40		;until done
	_DFFTOL50:
		mov	Acc,_op1_3		;move result to corretly position
		mov	AccH,_op1_4
		mov	AccT,_op1_5
		mov	AccTH,_op1_s
		tst	_op1_e			;positive ?
		brpl	_DFFTOL60		;yes, branch	
		com	Acc			;else turn negative
		com	AccH
		com	AccT
		com	AccTH
		subi	Acc,-1
		sbci	AccH,-1
		sbci	AccT,-1
		sbci	AccTH,-1
	_DFFTOL60:
		clc
		ret
#endif
.exit

