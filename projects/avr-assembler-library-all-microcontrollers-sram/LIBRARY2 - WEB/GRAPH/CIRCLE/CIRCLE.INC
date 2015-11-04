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
;| DATA:12/04/2001							|
;>----------------------------------------------------------------------<
;| C I R C L E  1 6  B I T  R O U T I N E 				|
;+----------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM			|
;| AND USE A BRESEHAM ALGORITHM 					|
;+----------------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _DISP_PSET		A USER DEFINED ROUTINES TO SET A PIXEL WITH FOLLOW INPUT PARAMETERS	|
;|												|
;|			coordenates in x,y registers						|
;|			Acc color number 0..255							|
;|												|
;+----------------------------------------------------------------------------------------------+
;| _GRAPH_CIRCLE	Draw a circle								|
;|												|
;|			ldi	XL,LOW(100)	;Draw a Circle at center 100,75 radius=50	|
;|			ldi	XH,HIGH(100)	;x=100						|
;|			ldi	YL,LOW(75)	;y=75						|
;|			ldi	YH,HIGH(75)							|
;|			ldi	AccT,LOW(50)	;radius=50					|
;|			ldi	AccTH,HIGH(50)							|
;|			ldi	Acc,ON		;Pixels ON					|
;|			rcall	_GRAPH_CIRCLE	;Draw a circle					|
;+----------------------------------------------------------------------------------------------+
#ifndef __GRAPH_CIRCLE__
	#define	__GRAPH_CIRCLE__
	.message "+------------------+"
	.message "|                  |"
	.message "| GRAPH CIRCLE     |" 
	.message "|                  |"
	.message "+------------------+"
	.DSEG
		_GRAPH_CIRCLE_PIXEL_ADDR:	.BYTE	_WORD		;set pixel function address
		_GRAPH_CIRCLE_COLOR:		.BYTE	_BYTE		;pixel circle color
	.CSEG
	;+---------------------------------------+
	;| Circle Define Set Pixel function	 |
	;| input: AccH:Acc Set Pixel Address	 |
	;+---------------------------------------+
	;					  \______________________________________________________________________________________________
	_GRAPH_CIRCLE_SET_PIXEL_ADDR:
		stsaw	_GRAPH_CIRCLE_PIXEL_ADDR
		ret
	;+---------------------------------------+
	;| Circle Set Pixel			 |
	;| input: X    = X center coordinate	 |
	;|	  Y    = Y center coordinate	 |
	;|	  Acc  = Pixel			 |
	;| Destroyed: Z				 |
	;+---------------------------------------+
	;					  \______________________________________________________________________________________________
	_GRAPH_CIRCLE_SET_PIXEL:
		ldsw	Z,_GRAPH_CIRCLE_PIXEL_ADDR
		icall
		ret
	;+---------------------------------------+
	;| Circle Set Color			 |
	;| input: Acc Color			 |
	;+---------------------------------------+
	;					  \______________________________________________________________________________________________
	_GRAPH_CIRCLE_SET_COLOR:
		sts	_GRAPH_CIRCLE_COLOR,Acc
		ret
	;+---------------------------------------+
	;| Circle				 |
	;| input: X    = X center coordinate	 |
	;|	  Y    = Y center coordinate	 |
	;|	  AccT:AccTH = Radius		 |
	;+---------------------------------------+
	;					  \______________________________________________________________________________________________
	_GRAPH_CIRCLE:
		pushaw
		pushawt
		pushw	Z
		push	r0
		push	r1
		push	R2
		push	R3
		push	R4
		push	Acc			;save color
		mov	AccH,AccT
		mov	AccT,AccTH		;AccH,AccT=Radius
		clr	r0			;X=0
		clr	r1
		mov	R2,AccH		;	Y=Radius
		mov	R3,AccT
		lsl	AccH			;2*Radius
		rol	AccT
		com	AccH			;-2*Radius
		com	AccT
		subi	AccH,-1
		sbci	AccT,-1
		ldi	Acc,3			;d=3-2*Radius
		add	AccH,Acc
		clr	Acc
		adc	AccT,Acc
		pop	Acc
	_Graph_CI00:
		cp	r0,R2			;X<Y ?
		cpc	r1,R3
		brge	_Graph_CI_DONE		;yes, done
		lds	Acc,_GRAPH_CIRCLE_COLOR	;get color
		rcall	_Graph_PSETSIM		;plot 8 simetrics pixels
		tst	AccT			;D<0 ?
		brpl	_Graph_CI01		;no, jump
		push	Acc
		push	r0
		push	r1
		lsl	r0			;xc*4
		rol	r1
		lsl	r0
		rol	r1
		ldi	Acc,6
		add	r0,Acc			;xc*4+6
		clr	Acc
		adc	r1,Acc
		add	AccH,r0			;d=d+xc*4+6
		adc	AccT,r1
		pop	r1
		pop	r0
		pop	Acc
		rjmp	_Graph_CI02
	_Graph_CI01:
		push	Acc
		push	r0
		push	r1
		sub	r0,R2			;xc-yc
		sbc	r1,R3	
		lsl	r0			;4*(xc-yc)
		rol	r1
		lsl	r0
		rol	r1
		ldi	Acc,10
		add	r0,Acc			;4*(xc-yc)+10
		clr	Acc
		adc	r1,Acc
		add	AccH,r0			;d=d+4*(xc-yc)+10
		adc	AccT,r1
		pop	r1
		pop	r0
		pop	Acc
		push	Acc
		ldi	Acc,1
		sub	R2,Acc			;yc--
		clr	Acc
		sbc	R3,Acc
		pop	Acc
	_Graph_CI02:
		push	Acc
		ldi	Acc,1
		add	r0,Acc			;xc++
		clr	Acc
		adc	r1,Acc
		pop	Acc
		rjmp	_Graph_CI00		
	_Graph_CI_DONE:
		cp	r0,R2			;xc=yc ?
		cpc	r1,R3
		brne	_Graph_CI_DONE2		;no, done
		lds	Acc,_GRAPH_CIRCLE_COLOR	;get color
		rcall	_Graph_PSETSIM		;else plot assimetric points
	_Graph_CI_DONE2:	
		pop	R4			;restore useds
		pop	R3
		pop	R2
		pop	r1
		pop	r0
		popw	Z
		popawt
		popaw
		ret
	;+---------------------------------------+
	;| Put eight simetrics pixels		 |
	;| input: X = X center coordinate	 |
	;|	  Y = Y center coordinate	 |
	;|	  XC = X extend			 |
	;|	  YC = Y extend			 |
	;|	  Acc 0 reset 0xff set pixel	 |
	;+=======================================+=======================================================
	_Graph_PSETSIM:
		push	R4
		rcall	_Graph_4SIM		;plot 4 pixels 1st
		push	Acc			;swap coordinates
		mov	Acc,r0
		mov	r0,R2
		mov	R2,Acc
		mov	Acc,r1
		mov	r1,R3
		mov	R3,Acc
		pop	Acc
		rcall	_Graph_4SIM		;plot 4 next simetrics pixels
		push	Acc			;restore original position
		mov	Acc,r0
		mov	r0,R2
		mov	R2,Acc
		mov	Acc,r1
		mov	r1,R3
		mov	R3,Acc
		pop	Acc
		pop	R4
		ret
	_Graph_4SIM:
		push	Acc
		ldi	Acc,0b0000000		;bit 0 = 0 add else subtract X bit 1 = 0 add else subtract Y
		mov	R4,Acc
		pop	Acc
		rcall	_Graph_PLOT4
		push	Acc
		ldi	Acc,0b00000001
		mov	R4,Acc
		pop	Acc
		rcall	_Graph_PLOT4
		push	Acc
		ldi	Acc,0b00000010
		mov	R4,Acc
		pop	Acc
		rcall	_Graph_PLOT4
		push	Acc
		ldi	Acc,0b00000011
		mov	R4,Acc
		pop	Acc
		rcall	_Graph_PLOT4
		ret
	_Graph_PLOT4:
		pushw	X
		pushw	Y
		sbrs	R4,0
		add	XL,r0		;x+xc
		sbrs	R4,0
		adc	XH,r1
		sbrc	R4,0
		sub	XL,r0		;x-xc
		sbrc	R4,0
		sbc	XH,r1
		sbrs	R4,1	
		add	YL,R2		;y+yc	
		sbrs	R4,1
		adc	YH,R3	
		sbrc	R4,1
		sub	YL,R2		;y-yc
		sbrc	R4,1
		sbc	YH,R3	
		rcall	_GRAPH_CIRCLE_SET_PIXEL
		popw	Y
		popw	X
		ret
#endif
.exit

