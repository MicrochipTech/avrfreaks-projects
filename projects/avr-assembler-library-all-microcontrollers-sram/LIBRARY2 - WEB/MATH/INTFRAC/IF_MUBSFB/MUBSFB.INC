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
;+--------------------------------------------------------------+---+
;|								|
;| DATE:november,16,2000					|
;>--------------------------------------------------------------<
;| M U L T I P L Y						|
;| U N S I G N E D  B Y T E  X  S I G N E D  B Y T E		|
;+--------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM		|
;+--------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N  		|
;+------------------------------------------------------+---------------------------------------+
;| _IF_MUL_UBSFB	Multiply unsigned integer by signed fraction 8 bits			|
;|												|
;|			ldi	Acc,100								|
;|			mov	r0,Acc			;r0=100					|
;|			ldi	Acc,34*128/100							|
;|			mov	r1,Acc			;r1=0.34				|
;|			RCALL	_IF_MUL_UBSFB		;r2:r3 signed 16 product		|
;|												|
;+----------------------------------------------------------------------------------------------+
;+--------------------------------------------------------------+--------------------------------------------------------------------------
;| Multiply Unsigned Integer 8 Bits By Signed Fraction 8 bits	|
;|								|
;| Number formats						|
;|		    	  7 6 5 4 3 2 1 0			|
;|		   	 +-+-+-+-+-+-+-+-+			|
;| Unsigned Integer byte |n|n|n|n|n|n|n|n|			|
;|			 +-+-+-+-+-+-+-+-+			|
;| 								|
;| n - Bit Number Field						|
;|			 7 6 5 4 3 2 1 0 			|
;|		   	+-+-+-+-+-+-+-+-+			|
;| Signed Fraction byte |s|f|f|f|f|f|f|f|			|
;|		   	+-+-+-+-+-+-+-+-+			|
;|								|
;| s - Fraction Signal 0-Positive 1-Negative			|
;| f - Bit Fraction Field					|
;|								|
;| Input:							|
;|	  r0 Unsigned Integer					|
;|	  r1 Signed Fraction					|
;|								|
;| Output:							|
;|	  r2:r3 Signed Product					|
;|								|
;| Destroy: r0							|
;|								|
;| Clocks: 34 with rcall for a positive numbers			|
;|	   41 with rcall for a negative numbers			|
;+--------------------------------------------------------------+
#ifndef __INTFRAC_UBSFB__
	#define	__INTFRAC_UBSFB__
	.message "[ (MATH) INTFRAC UBSFB ]"
	_IF_MUL_UBSFB:
		clr	r2					;clear accumulator
		clr	r3
		bst	r1,7					;t=fraction signal
		sbrc	r1,7					;skip if fraction positive
		neg	r1					;fraction abs
		lsr	r0					;use Left Shift With Add method
		sbrc	r1,6					;skip if bit=6 
		add	r2,r0					;else accumulate result
		lsr	r0					;use Left Shift With Add method
		sbrc	r1,5					;skip if bit=5 
		add	r2,r0					;else accumulate result
		lsr	r0					;use Left Shift With Add method
		sbrc	r1,4					;skip if bit=4 
		add	r2,r0					;else accumulate result
		lsr	r0					;use Left Shift With Add method
		sbrc	r1,3					;skip if bit=3 
		add	r2,r0					;else accumulate result
		lsr	r0					;use Left Shift With Add method
		sbrc	r1,2					;skip if bit=2 
		add	r2,r0					;else accumulate result
		lsr	r0					;use Left Shift With Add method
		sbrc	r1,1					;skip if bit=1 
		add	r2,r0					;else accumulate result
		sbrc	r1,0					;skip if bit=0
		add	r2,r0					;else accumulate result
		brtc	_IF_UB00				;branch if number is positive
		clr	r0
		inc	r0
		com	r2
		com	r3
		add	r2,r0
		clr	r0					;else two complement result
		adc	r3,r0					;extend signal of result
	_IF_UB00:
		ret
#endif
.exit

