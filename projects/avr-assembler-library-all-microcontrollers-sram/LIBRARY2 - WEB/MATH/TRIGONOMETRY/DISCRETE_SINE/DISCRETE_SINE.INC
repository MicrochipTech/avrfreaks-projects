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
;| DATA:1/12/2002								|
;>------------------------------------------------------------------------------<
;| C A L C U L A T E   A   D I S C R E T E  S I N E  F U N C T I O N		|
;+------------------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM				|
;+------------------------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N  		|
;+------------------------------------------------------+---------------------------------------+
;| _DSSIN16B	return a scaled sine of angle A from -32768 to 32677				|
;|												|
;|			ldi	Acc,low(315)		;calculate a sine of 315 degree		|
;|			ldi	AccH,high(315)							|
;|			ldi	AccT,low(100)		;scale factor = 100			|
;|			ldi	AccTH,high(100)							|
;|			rcall	_DSSIN16B		;Acc:AccH has a s*sin(Acc:AccH) previous|
;|												|
;+----------------------------------------------------------------------------------------------+
;| _DSIN16B	return a sine of angle A from -32768 to 32677					|
;|												|
;|			ldi	Acc,low(315)		;calculate a sine of 315 degree		|
;|			ldi	AccH,high(315)							|
;|			rcall	_DSIN16B		;Acc:AccH has a sin(Acc:AccH) previous	|
;|												|
;+----------------------------------------------------------------------------------------------+
#ifndef	__INTEGER_DISCRETE_SINE__
	#define __INTEGER_DISCRETE_SINE__
	.message "[ (MATH) INTEGER DISCRETE SINE ]"
	;+----------------------+
	;| NECESSARY INCLUDES	|
	;+----------------------+
	#ifndef	__INTEGER_MUL_U32U32U32__
		.INCLUDE	"MATH\INTEGER\MUL_U32U32U32\MUL_U32U32U32.INC"
	#endif
	;+----------------------------------------------------------------------+
	;| Calculate a discrete scaled sine of angle A from -32768 to 32677, and|
	;| other hands perform the follow equation:				|
	;|									|
	;|	y=S*Sine(A)							|
	;| where:								|
	;|	S unsigned integer Scale					|
	;|	A signed integer Angle						|
	;| 	Y integer scaled sine function					|
	;|									|
	;| Input  : Acc:AccH signed integer angle A				|
	;|	    AccT:AccTH signed integer scale factor			|
	;| Output : Acc:AccH signed integer sine of angle A			|
	;| Destroy: Flags,R0,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12		|
	;| obs    :								|
	;+----------------------------------------------------------------------+
	_DSSIN16B:
		rcall	_DSIN16B				;get sine of angle A first
	_DSSIN16B_COS_ENTRY:
		mov	r0,AccH					;multiply signals
		eor	r0,AccTH
		push	r0					;save signal multiply
		andi	AccH,0x7f				;extract signal
		mov	r0,Acc
		mov	r1,AccH
		clr	r2
		clr	r3
		tst	AccTH					;positive ?
		brpl	_DSSIN16B00				;yes, branch
		com	AccT					;else complement
		com	AccTH
		subi	AccT,-1
		sbci	AccTH,-1
	_DSSIN16B00:
		mov	r4,AccT					;move to correctly registers
		mov	r5,AccTH
		clr	r6
		clr	r7
		rcall	_MUL_U32U32U32				;multiply sine by Scale
		lsl	r9					;put result into correctly registers and position
		rol	r10
		rol	r11
		lsl	r9
		rol	r10
		rol	r11
		pop	AccH
		tst	AccH					;negative ?
		brpl	_DSSIN16B10				;no, branch
		com	r10					;else turn a negative number
		com	r11
		ldi	Acc,1
		add	r10,Acc
		ldi	Acc,0
		adc	r11,Acc
	_DSSIN16B10:
		mov	Acc,r10
		mov	AccH,r11
		ret
	
	;+--------------------------------------------------------------+
	;| Calculate a discrete sine of angle A from -32768 to 32677	|
	;|								|
	;| Input  : Acc:AccH signed integer angle A			|
	;| Output : Acc:AccH signed integer sine of angle A		|
	;| Destroy: Flags,R0						|
	;| obs    :							|
	;|								|
	;| returned number format into Acc:AccH				|
	;|								|
	;|  15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0		|
	;| +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+		|
	;| | s| i| f| f| f| f| f| f| f| f| f| f| f| f| f| f|		|
	;| +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+		|
	;|								|
	;| s = signal 0=positive 1=negative				|
	;| i = integer part 						|
	;| f = fractionary part						|
	;+--------------------------------------------------------------+
	_DSIN16B:
		push	AccT					;save useds
		push	AccTH
		push	ZL
		push	ZH
		push	AccH					;save angle signal
		tst	AccH					;see if a negative number
		brpl	_DSIN16B00				;no, branch
		com	Acc					;else turn a positive number
		com	AccH
		subi	Acc,-1
		sbci	AccH,-1
	_DSIN16B00:
	;+----------------------------------
	;| reduce angle to 1..4 quadrant
	;+----------------------------------
		ldi	AccT,low(360)				;AccT:AccTH=360
		ldi	AccTH,high(360)
	_DSIN16B20:
		cp	Acc,AccT				;Angle >=360o ?
		cpc	AccH,AccTH
		brlo	_DSIN16B30				;yes, exit of this loop
		sub	Acc,AccT				;else Angle=Angle-360
		sbc	AccH,AccTH
		rjmp	_DSIN16B20				;loop until in range, in other hands between 1..4 quadrants
	_DSIN16B30:
	;+----------------------------------------------
	;| angle < 90o	if yes then y=sin(Angle)
	;+----------------------------------------------
		ldi	AccT,low(90)				;AccT:AccTH=90
		ldi	AccTH,high(90)
		cp	Acc,AccT				;angle < 90o ?
		cpc	AccH,AccTH
		brsh	_DSIN16B40				;no, branch
		rcall	_DSIN16GET				;get sine
		rjmp	_DSIN16BDONE				;done
	;+----------------------------------------------
	;| angle < 180o if yes then y=sin(180-Angle)
	;+----------------------------------------------
	_DSIN16B40:
		ldi	AccT,low(180)				;AccT:AccTH=180
		ldi	AccTH,high(180)
		cp	Acc,AccT				;angle < 180o ?
		cpc	AccH,AccTH
		brsh	_DSIN16B50				;no, branch
		sub	AccT,Acc				;angle=180-angle
		sbc	AccTH,AccH
		mov	Acc,AccT
		mov	AccH,AccTH
		rcall	_DSIN16GET				;get sine
		rjmp	_DSIN16BDONE				;done
	;+----------------------------------------------
	;| angle < 270o if yes then y=-sin(Angle-180)
	;+----------------------------------------------
	_DSIN16B50:
		ldi	AccT,low(270)				;AccT:AccTH=270
		ldi	AccTH,high(270)
		cp	Acc,AccT				;angle < 270o ?
		cpc	AccH,AccTH
		brsh	_DSIN16B60				;no, branch
		subi	Acc,low(180)				;Angle=Angle-180
		sbci	AccH,high(180)	
		rcall	_DSIN16GET				;get sine
		ori	AccH,0x80				;set sine a negative value
		rjmp	_DSIN16BDONE				;done
	;+----------------------------------------------
	;| otherwise y=-sin(360-Angle)
	;+----------------------------------------------
	_DSIN16B60:
		ldi	AccT,low(360)				;other wise Angle=360-Angle
		ldi	AccTH,high(360)
		sub	AccT,Acc
		sbc	AccTH,AccH
		mov	Acc,AccT
		mov	AccH,AccTH
		rcall	_DSIN16GET				;get sine
		ori	AccH,0x80				;set sine a negative value
	_DSIN16BDONE:
		pop	AccT					;restore signal
		tst	AccT					;negative number ?
		brpl	_DSIN16BDONE00				;no, branch
		ldi	AccT,0x80				;else change sinal of sine
		eor	AccH,AccT
	_DSIN16BDONE00:	
		pop	ZL
		pop	ZH
		pop	AccTH					;restore useds
		pop	AccT
		ret						;return
	;+----------------------------------
	;| get sine indexed by angle = Acc:AccH
	;+----------------------------------
	_DSIN16GET:
		ldi	ZL,low(_MATH_DICRETE_SINE_TABLE*2)	;Z--> sine table
		ldi	ZH,high(_MATH_DICRETE_SINE_TABLE*2)
		lsl	Acc					;because a sine table is 16 bit size
		rol	AccH
		add	ZL,Acc					;Z--> sine(Angle)
		adc	ZH,AccH
		lpm						;get sine low part
		mov	Acc,r0					;move it to Acc
		adiw	ZL,1					;next part
		lpm
		mov	AccH,r0					;get sine high part
		ret
	
	
	;+----------------------------------------------------------------------+
	;| Sine table of angles from 0 to 90 degrees with a step of 1 degree  	|
	;| 									|
	;| Table Number formation use a 16 bits. The bit 14 is a integer part	|
	;| 2^0, bit 13 is a first bit fraction 2^-1, bit 12 is a second bit 	|
	;| fraction 2^-2 and so on						|
	;|									|
	;|  15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0			|
	;| +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+			|
	;| | s| i| f| f| f| f| f| f| f| f| f| f| f| f| f| f|			|
	;| +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+			|
	;|									|
	;| s = signal 0=positive 1=negative					|
	;| i = integer part 							|
	;| f = fractionary part							|
	;| all values from 0 to 90 return a positive sine(values)		|
	;+----------------------------------------------------------------------+
	_MATH_DICRETE_SINE_TABLE:
		.dw 0x0000 ; 0o
		.dw 0x011E ; 1o
		.dw 0x023C ; 2o
		.dw 0x0359 ; 3o
		.dw 0x0477 ; 4o
		.dw 0x0594 ; 5o
		.dw 0x06B1 ; 6o
		.dw 0x07CD ; 7o
		.dw 0x08E8 ; 8o
		.dw 0x0A03 ; 9o
		.dw 0x0B1D ; 10o
		.dw 0x0C36 ; 11o
		.dw 0x0D4E ; 12o
		.dw 0x0E66 ; 13o
		.dw 0x0F7C ; 14o
		.dw 0x1090 ; 15o
		.dw 0x11A4 ; 16o
		.dw 0x12B6 ; 17o
		.dw 0x13C7 ; 18o
		.dw 0x14D6 ; 19o
		.dw 0x15E4 ; 20o
		.dw 0x16F0 ; 21o
		.dw 0x17FA ; 22o
		.dw 0x1902 ; 23o
		.dw 0x1A08 ; 24o
		.dw 0x1B0C ; 25o
		.dw 0x1C0E ; 26o
		.dw 0x1D0E ; 27o
		.dw 0x1E0C ; 28o
		.dw 0x1F07 ; 29o
		.dw 0x2000 ; 30o
		.dw 0x20F6 ; 31o
		.dw 0x21EA ; 32o
		.dw 0x22DB ; 33o
		.dw 0x23CA ; 34o
		.dw 0x24B5 ; 35o
		.dw 0x259E ; 36o
		.dw 0x2684 ; 37o
		.dw 0x2767 ; 38o
		.dw 0x2847 ; 39o
		.dw 0x2923 ; 40o
		.dw 0x29FD ; 41o
		.dw 0x2AD3 ; 42o
		.dw 0x2BA6 ; 43o
		.dw 0x2C75 ; 44o
		.dw 0x2D41 ; 45o
		.dw 0x2E0A ; 46o
		.dw 0x2ECE ; 47o
		.dw 0x2F90 ; 48o
		.dw 0x304D ; 49o
		.dw 0x3107 ; 50o
		.dw 0x31BD ; 51o
		.dw 0x326F ; 52o
		.dw 0x331D ; 53o
		.dw 0x33C7 ; 54o
		.dw 0x346D ; 55o
		.dw 0x350F ; 56o
		.dw 0x35AD ; 57o
		.dw 0x3646 ; 58o
		.dw 0x36DC ; 59o
		.dw 0x376D ; 60o
		.dw 0x37FA ; 61o
		.dw 0x3882 ; 62o
		.dw 0x3906 ; 63o
		.dw 0x3986 ; 64o
		.dw 0x3A01 ; 65o
		.dw 0x3A78 ; 66o
		.dw 0x3AEA ; 67o
		.dw 0x3B57 ; 68o
		.dw 0x3BC0 ; 69o
		.dw 0x3C24 ; 70o
		.dw 0x3C83 ; 71o
		.dw 0x3CDE ; 72o
		.dw 0x3D34 ; 73o
		.dw 0x3D85 ; 74o
		.dw 0x3DD2 ; 75o
		.dw 0x3E19 ; 76o
		.dw 0x3E5C ; 77o
		.dw 0x3E9A ; 78o
		.dw 0x3ED3 ; 79o
		.dw 0x3F07 ; 80o
		.dw 0x3F36 ; 81o
		.dw 0x3F61 ; 82o
		.dw 0x3F86 ; 83o
		.dw 0x3FA6 ; 84o
		.dw 0x3FC2 ; 85o
		.dw 0x3FD8 ; 86o
		.dw 0x3FEA ; 87o
		.dw 0x3FF6 ; 88o
		.dw 0x3FFE ; 89o
		.dw 0x4000 ; 90o
#endif
.exit

