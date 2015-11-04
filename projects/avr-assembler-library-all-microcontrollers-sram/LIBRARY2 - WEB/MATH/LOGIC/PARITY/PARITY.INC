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
;| DATE:2/9/2003 VERSION 1.0						|
;>----------------------------------------------------------------------<
;| I N T E G E R  R O U T I N E S	     				|
;| P A R I T Y  C H E C K 						|
;+----------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM			|
;+----------------------------------------------------------------------+
;+----------------------------------------------------------------------+
;| PARITY_CHECK								|
;| Check byte parity							|
;|								  	|
;| Input  : R0  byte to see a parity			  		|
;| Output : C=1 parity odd else even			  		|
;| Destroy: Flags,r1						  	|
;| Time   : 22+3 rcall						  	|
;+----------------------------------------------------------------------+
#ifndef	__INTEGER_PARITY8__
	#define __INTEGER_PARITY8__
	.message "[ (MATH) INTEGER PARITY8 ]"
	_INTEGER_PARITY8:
		clr	r1			;clear parity bit counter
		sbrc	r0,0			;skip if bit 0=0
		inc	r1			;else increment parity counter
		sbrc	r0,1			;skip if bit 0=0
		inc	r1			;else increment parity counter
		sbrc	r0,2			;skip if bit 0=0
		inc	r1			;else increment parity counter
		sbrc	r0,3			;skip if bit 0=0
		inc	r1			;else increment parity counter
		sbrc	r0,4			;skip if bit 0=0
		inc	r1			;else increment parity counter
		sbrc	r0,5			;skip if bit 0=0
		inc	r1			;else increment parity counter
		sbrc	r0,6			;skip if bit 0=0
		inc	r1			;else increment parity counter
		sbrc	r0,7			;skip if bit 0=0
		inc	r1			;else increment parity counter
		ror	r1			;transfer parity bit to carry
		ret
#endif
.exit

