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
;| C O M P A R A T I O N 					|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+
;+--------------------------------------+
;| Float Point compare Float Op1 & Op2  |
;| Input: Float Op1,Op2			|
;| Output: Flags			|
;|	   _DF_FSREG SREG copy		|
;| Destroy: Acc				|
;+--------------------------------------+
#ifndef __MATH_FLOAT_COMPARE__
	#define __MATH_FLOAT_COMPARE__
	.message "[ (MATH) FLOAT COMPARE ]"
	.MACRO	_DFCMPOP1OP2
		rcall	_DFCPOP1OP2
	.ENDMACRO
	_DFCPOP1OP2:
		sbrc	_op1_s,7			;skip if op1 is positive 
		rjmp	_DFCP10				;else branch *<NOCHANGE>*
		sbrc	_op2_s,7			;skip if op2 is positive too
		rjmp	_DFCP20				;else branch *<NOCHANGE>*
		cp	_op1_0,_op2_0			;compare positive numbers
		cpc	_op1_1,_op2_1
		cpc	_op1_2,_op2_2
		cpc	_op1_3,_op2_3
		cpc	_op1_4,_op2_4
		cpc	_op1_5,_op2_5
		cpc	_op1_s,_op2_s
		cpc	_op1_e,_op2_e
		rjmp	_DFCP40				;store flag state *<NOCHANGE>*
	_DFCP10:
		sbrs	_op2_s,7			;skip if op2 is negative
		rjmp	_DFCP30				;else branch
		cp	_op2_0,_op1_0			;compare negative numbers
		cpc	_op2_1,_op1_1
		cpc	_op2_2,_op1_2
		cpc	_op2_3,_op1_3
		cpc	_op2_4,_op1_4
		cpc	_op2_5,_op1_5
		cpc	_op2_s,_op1_s
		cpc	_op2_e,_op1_e
		rjmp	_DFCP40				;store flag state *<NOCHANGE>*
	_DFCP20:
		clz					;set diferents
		clc					;set op1>op2
		rjmp	_DFCP40				;store flag state *<NOCHANGE>*
	_DFCP30:
		clz					;set diferents
		sec					;set op1<op2
	_DFCP40:
		in	Acc,SREG			;save SREG state
		sts	_DF_FSREG,Acc
		ret
	;+------------------------------+
	;| MACROS FOR COMPARE FLOATS	|
	;+------------------------------+
	.MACRO	_DFJP_EQ				;Jump if igual
		rcall	_DFCPOP1OP2
		brne	PC+2				;skip if not igual
		rjmp	@0				;jump if igual *<NOCHANGE>*
	.ENDMACRO
	.MACRO	_DFJP_NEQ				;Jump if not igual
		rcall	_DFCPOP1OP2
		breq	PC+2				;skip if igual
		rjmp	@0				;jump if not igual *<NOCHANGE>*
	.ENDMACRO
	.MACRO	_DFJP_GT				;Jump if great than
		rcall	_DFCPOP1OP2
		breq	PC+3				;skip if igual
		brcs	PC+2				;skip if not great
		rjmp	@0				;jump if great *<NOCHANGE>*
	.ENDMACRO	
	.MACRO	_DFJP_LT				;Jump if less than
		rcall	_DFCPOP1OP2
		breq	PC+3				;skip if igual
		brcc	PC+2				;skip if not less
		rjmp	@0				;jump if less *<NOCHANGE>*
	.ENDMACRO	
	.MACRO	_DFJP_GTEQ				;Jump if great than or igual
		rcall	_DFCPOP1OP2	
		brcs	PC+2				;skip if not great
		rjmp	@0				;else jump *<NOCHANGE>*
	.ENDMACRO	
	.MACRO	_DFJP_LTEQ				;Jump if less than or igual
		rcall	_DFCPOP1OP2	
		brcc	PC+2				;skip if not less
		rjmp	@0				;else jump *<NOCHANGE>*
		brne	PC+2				;skip if not igual
		rjmp	@0				;else jump *<NOCHANGE>*
	.ENDMACRO	
#endif
.exit

