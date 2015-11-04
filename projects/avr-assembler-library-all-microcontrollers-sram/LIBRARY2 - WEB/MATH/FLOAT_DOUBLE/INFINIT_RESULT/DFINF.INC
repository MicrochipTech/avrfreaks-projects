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
;| I N F I N I T  S E T T I N G S 				|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
#ifndef __MATH_FLOAT_INFINIT__
	#define __MATH_FLOAT_INFINIT__
	.message "[ (MATH) FLOAT INFINIT RESULTS ]"
	;+---------------------------------------+
	;! SET FLOAT POINT UNDERFLOW 	         !
	;! set accumulator to zero and Acc with  !
	;! float point underflow 	         !
	;!                                       !
	;! Input  : none                         !
	;! Output : Floating Accumulator 0(zero) !
	;!	    Acc Underflow code		 ! 
	;! Destroy: Flags                        !
	;+---------------------------------------+
	_DFUNDER:
		ldi	Acc,_FEUNDER		;set Floating Pointer Underflow
	_DFUNDER00:
		clr	_acc_0			;Clear Floating pointer accumulator
		clr	_acc_1
		clr	_acc_2
		clr	_acc_3
		clr	_acc_4
		clr	_acc_5
		clr	_acc_s
		clr	_acc_e
		ret
	_DFCLEAR:
		ldi	Acc,_FPOK
		rjmp	_DFUNDER00
	;+-----------------------------------------------+
	;! SET FLOAT POINT +OVERFLOW OR -OVERFLOW 	 !
	;! set max positive or negative and code into    !
	;! acc                                           !
	;!						 !
	;! Input  : acc_s bit 7 number signal            !
	;! Output : floating Accumulator +/-1.701412e+38 !
	;!	    Acc Overflow code			 !
	;! Destroy: Flags                                !
	;+-----------------------------------------------+
	_DFOVER:
		ldi	Acc,_FEOVER		;set Floating Pointer Overflow
	_DFOVER00:
		ori	_acc_s,0b01111111	;hold original signal
		ser	_acc_e			;set max number
		ser	_acc_5
		ser	_acc_4
		ser	_acc_3
		ser	_acc_2
		ser	_acc_1
		ser	_acc_0
		ret
		
	;+-----------------------------------------------+
	;! SET FLOAT POINT DIVISION BY ZERO	         !
	;! set max positive or negative and code into    !
	;! acc                                           !
	;!                                               !
	;! Input  : acc_s bit 7 number signal            !
	;! Output : floating Accumulator +/-1.701412e+38 !
	;!	    Acc Overflow code			 !
	;! Destroy: Flags                                !
	;+-----------------------------------------------+
	_DFDIV0:
		ldi	Acc,_FEDIV0		;set Floating Pointer Division by Zero
		rjmp	_DFOVER00
#endif
.exit

