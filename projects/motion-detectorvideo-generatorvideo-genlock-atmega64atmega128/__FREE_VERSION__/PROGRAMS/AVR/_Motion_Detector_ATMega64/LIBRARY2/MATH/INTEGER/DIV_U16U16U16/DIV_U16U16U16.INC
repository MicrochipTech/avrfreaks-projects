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
;| DATE:28/01/2002 VERSION 1.0						|
;>----------------------------------------------------------------------<
;| I N T E G E R  R O U T I N E S	     				|
;| D I V I D E  							|
;+----------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM			|
;+----------------------------------------------------------------------+
;+----------------------------------------------------------------------+
;| DIV_U16U16U16							|
;| Divide unsigned 16bits/unsigned 16 bits result unsigned 16bits	|
;|								  	|
;| Input  : R0:R1  Dividend				  		|
;|	    R2:R3  Divisor				  		|
;| Output : R0:R1  Quotient				  		|
;|	    R4:R5  Rest							|
;| Destroy: Flags						  	|
;| Time   : Min=192+3 rcall  Max=208+3 rcall			  	|
;+----------------------------------------------------------------------+
#ifndef	__INTEGER_DIV_U16U16U16__
	#define __INTEGER_DIV_U16U16U16__
	.message "[ (MATH) INTEGER DIV U16U16U16 ]"
	_DIV_U16U16U16:
		push	Acc			;save Acc
		ldi	Acc,16			;quotient bit count
		clr	r4			;clear my accumulador
		clr	r5
		clc				;clear first carry
	_DIV_U1600:
		rol	r0			;rotate left dividend to see if fit inside quotient
		rol	r1
		rol	r4
		rol	r5
		cp	r4,r2			;fit?
		cpc	r5,r3
		brcs	_DIV_U1610			;no, branch
		sub	r4,r2			;else subtract divisor from dividend 
		sbc	r5,r3
	_DIV_U1610:
		dec	Acc			;next bit
		brne	_DIV_U1600			;until all done
		rol	r0			;place last bit result into quotient
		rol	r1
		com	r0			;because all bit are inverted during computation
		com	r1
		pop	Acc			;restore Acc
		ret
#endif
.exit

