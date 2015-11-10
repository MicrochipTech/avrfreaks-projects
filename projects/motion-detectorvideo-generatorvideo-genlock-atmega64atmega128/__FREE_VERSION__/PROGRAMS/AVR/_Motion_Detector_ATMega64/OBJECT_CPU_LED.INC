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
;						 ///////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\
;						//						\\
;-----------------------------------------------||  	       C P U  G O O D  L E D 		||-----------------------------------------------------
;						\\						//
;						 \\\\\\\\\\\\\\\\\\\\\\\/////////////////////////

;>--------------------------------------------------------------<
;| OBJECT     : CPULED						|
;>--------------------------------------------------------------<
;| DESCRIPTION: This object set state of cpu led		|
;>--------------------------------------------------------------<

;>----------------------<
;| OBJECT DEFINITIONS	|
;>----------------------<

;-------------------------< PORT DEFINITONS >----------------------------

.EQU	_CPULED_PORT_OUTPUT	= PORTG
.EQU	_CPULED_PORT_DIR	= DDRG
.EQU	_CPULED_PORT_INPUT	= PING


;-------------------------< CONSTANT DEFINITONS >------------------------

.EQU	_CPULED_LED_BIT		= 2

	;>--------------------------------------------------------------<
	;| OBJECT     : CPULED						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Initialize Object				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	Disable interrupts during initialization	|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_CPULED_INIT:
		push	Acc						;save useds
		in	Acc,SREG					;save SREG
		cli							;disable interrupts during initialization
		push	Acc
		lds	Acc,PORTG					;get old state
		andi	Acc,~(1<<_CPULED_LED_BIT)			;set led enable
		sts	PORTG,Acc
		lds	Acc,DDRG					;get old state
		ori	Acc,(1<<_CPULED_LED_BIT)			;set as output
		sts	DDRG,Acc
		pop	Acc						;restore SREG state
		out	SREG,Acc
		pop	Acc						;restore useds
		ret

	;>--------------------------------------------------------------<
	;| OBJECT     : CPULED						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : Altenate CPU LED state 				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:	----						|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_CPULED_ALTERNATE:
		push	Acc						;save useds
		in	Acc,SREG					;get SREG
		cli							;disable interrupts
		pushaw
		lds	Acc,_CPULED_PORT_OUTPUT				;get led state
		ldi	AccH,(1<<_CPULED_LED_BIT)			;eor mask
		eor	Acc,AccH
		sts	_CPULED_PORT_OUTPUT,Acc				;altered
		popaw				
		out	SREG,Acc					;restore SREG
		pop	Acc
		ret

