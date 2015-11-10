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
;| DIV_U8U8U8								|
;| Divide unsigned 8bits/unsigned 8 bits result unsigned 8bits		|
;|								  	|
;| Input  : R0  Dividend				  		|
;|	    R1  Divisor				  			|
;| Output : R0  Quotient				  		|
;|	    R2  Rest							|
;| Destroy: Flags						  	|
;| Time   : Min=77+3 rcall  Max=77+3 rcall			  	|
;+----------------------------------------------------------------------+
#ifndef	__INTEGER_DIV_U8U8U8__
	#define __INTEGER_DIV_U8U8U8__
	.message "[ (MATH) INTEGER DIV U8U8U8 ]"
	_DIV_U8U8U8:
		push	Acc				;Save Acc
		ldi	Acc,8				;quotient bit count
		clr	r2				;clear my accumulator	
		clc					;clear my first carry
	_DIV_U8U8U8_00:
		rol	r0				;rotate left dividend to see if fit inside quotient
		rol	r2
		cp	r2,r1				;fit ?
		brcs	_DIV_U8U8U8_10			;no, branch
		sub	r2,r1				;else subtract divisor from dividend
	_DIV_U8U8U8_10:
		dec	Acc				;next bit
		brne	_DIV_U8U8U8_00			;until all done
		rol	r0				;place last bit into quotient
		com	r0				;bucause all bit are inverted during computation
		pop	Acc				;Restore Acc
		ret
#endif
.exit

