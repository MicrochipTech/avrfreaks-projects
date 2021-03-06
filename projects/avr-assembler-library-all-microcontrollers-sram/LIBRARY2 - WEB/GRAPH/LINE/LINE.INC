;<$DS_HEADER>
;+--------------------------------------------------------------------------------+              
;|Jo�o D�Artagnan A. Oliveira Programmer and Author; 				  |
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
;|   Bras�lia, Brazil November 30,2015 	06:28 am				  |
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;+--------------------------------------------------------------+---+---+
;|									|
;| DATA:12/04/2001							|
;>----------------------------------------------------------------------<
;| L I N E  1 6  B I T  R O U T I N E 					|
;+----------------------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM			|
;| AND USE A BRESEHAM ALGORITHM 					|
;+----------------------------------------------------------------------+
;>------------------------------------------------------<
;| I M P L E M E N T E D  F U N C T I O N S 		|
;+------------------------------------------------------+---------------------------------------+
;| _GRAPH_LINE_TO	Draw a line segment							|
;|												|
;|			ldi	Acc,LOW(100)	;Draw a line at (100,75) to (200,300)		|
;|			ldi	AccH,HIGH(100)	;Source coordinates (100,75)			|
;|			ldi	AccT,LOW(75)							|
;|			ldi	AccTH,HIGH(75)							|
;|			rcall	_GRAPH_MOVE_TO	;move to start coordinates			|
;|			ldi	XL,LOW(200)	;Destiny coordinates (200,300)			|
;|			ldi	XH,HIGH(200)							|
;|			ldi	YL,LOW(300)							|
;|			ldi	YH,HIGH(300)							|
;|			ldi	Acc,ON		;pixel on					|
;|			rcall	_GRAPH_LINE_TO	;Draw a line segment				|
;|----------------------------------------------------------------------------------------------+
;| _GRAPH_MOVE_TO	Move Graph Cursor							|
;|			see _GRAPH_LINE_TO example						|
;|----------------------------------------------------------------------------------------------+
;| _GRAPH_MOVE_TO_EX	Move Graph Cursor							|
;|			see _GRAPH_LINE_TO example but use X,Y register to set new start point	|
;|----------------------------------------------------------------------------------------------+
;| _GRAPH_GET_POINT	Get start point of line or end point if _GRAPH_LINE_TO is performated	|
;+----------------------------------------------------------------------------------------------+
#ifndef __GRAPH_LINE__
	#define	__GRAPH_LINE__
	.message "+------------------+"
	.message "|                  |"
	.message "| GRAPH LINE       |" 
	.message "|                  |"
	.message "+------------------+"
	;+----------------------------------------+
	;| LineTo				  |
	;| Input: X =  end coordinate	 	  |
	;|	  Y =  end coordinate	 	  |
	;|	  _GraphXStart = start coordinate |
	;|	  _GraphYStart = start coordinate | 
	;|	  Acc 0 reset 0xff set pixel	  |
	;|					  |
	;| Destroy: r0..r13			  |
	;| Obs:					  |
	;| _GraphXStart,_GraphYStart is updated	  |
	;| with X,Y end coordinates
	;+----------------------------------------+
	;					   \______________________________________________________________________________________________
	
	.DSEG
		_GraphXStart:		.byte 	_WORD		;define Start X coordinate into SRAM
		_GraphYStart:		.byte 	_WORD		;define Start Y coordinate into SRAM
		_Graph_Line_Pixel_Addr:	.byte 	_WORD		;define address of set pixel function
		_Graph_Line_Color:	.byte	_BYTE		;Pixel Color
	.CSEG
	
	_GRAPH_LINE_TO:
		pushw	X
		pushw	Y
		pushw	Z
		pushaw
		ldi	Acc,1				;xn=1;xd=1
		mov	r4,Acc
		mov	r8,Acc
		clr	Acc
		mov	r5,Acc
		mov	r9,Acc
		lds	Acc,_GraphXStart+0		;Xend-X
		sub	Acc,XL
		sts	_GraphXStart+0,Acc
		lds	Acc,_GraphXStart+1
		sbc	Acc,XH
		sts	_GraphXStart+1,Acc
		brpl	_GRAPH_L00			;branch if plus
		lds	Acc,_GraphXStart+0		;abs(Xend-X)
		com	Acc
		sts	_GraphXStart+0,Acc
		lds	Acc,_GraphXStart+1
		com	Acc
		sts	_GraphXStart+1,Acc
		lds	Acc,_GraphXStart+0
		subi	Acc,low(-1)
		sts	_GraphXStart+0,Acc
		lds	Acc,_GraphXStart+1
		sbci	Acc,high(-1)
		sts	_GraphXStart+1,Acc
		ldi	Acc,0xff
		mov	r4,Acc			;Xn=-1
		mov	r5,Acc	
		mov	r8,Acc			;Xd=-1
		mov	r9,Acc
	_GRAPH_L00:
		ldi 	Acc,1				;Yn=1;yd=1
		mov	r6,Acc
		mov	r10,Acc
		clr	Acc
		mov	r7,Acc
		mov	r11,Acc
		lds	Acc,_GraphYStart+0		;Yend-Y
		sub	Acc,YL
		sts	_GraphYStart+0,Acc
		lds	Acc,_GraphYStart+1
		sbc	Acc,YH
		sts	_GraphYStart+1,Acc
		brpl	_GRAPH_L10			;branch if plus
		lds	Acc,_GraphYStart+0		;abs(Yend-Y)
		com	Acc
		sts	_GraphYStart+0,Acc
		lds	Acc,_GraphYStart+1
		com	Acc
		sts	_GraphYStart+1,Acc
		lds	Acc,_GraphYStart+0
		subi	Acc,low(-1)
		sts	_GraphYStart+0,Acc
		lds	Acc,_GraphYStart+1
		sbci	Acc,high(-1)
		sts	_GraphYStart+1,Acc
		ldi	Acc,0xff			;yn=-1
		mov	r6,Acc
		mov	r7,Acc
		mov	r10,Acc			;yd=-1
		mov	r11,Acc
	_GRAPH_L10:
		lds	Acc,_GraphXStart+0
		lds	AccH,_GraphYStart+0
		cp	Acc,AccH			;Xend=A>Yend=B
		lds	Acc,_GraphXStart+1
		lds	AccH,_GraphYStart+1
		cpc	Acc,AccH
		brge	_GRAPH_L20			;yes, jump
		clr	Acc				;xn=0
		mov	r4,Acc
		mov	r5,Acc
		lds	Acc,_GraphXStart+1		;A<->B
		push	Acc
		lds	Acc,_GraphXStart+0
		lds	r12,_GraphYStart+0
		lds	r13,_GraphYStart+1
		sts	_GraphXStart+0,r12
		sts	_GraphXStart+1,r13
		sts	_GraphYStart+0,Acc
		pop	Acc
		sts	_GraphYStart+1,Acc
		rjmp	_GRAPH_L30
	_GRAPH_L20:	
		clr	Acc				;yn=0
		mov	r6,Acc
		mov 	r7,Acc
	_GRAPH_L30:
		lds	r0,_GraphXStart+0	;ct=a
		lds 	r1,_GraphXStart+1
		ldi	Acc,1				;ct=a+1
		add	r0,Acc
		ldi	Acc,0
		adc	r1,Acc
		lds	r2,_GraphYStart+0	;d=b
		lds	r3,_GraphYStart+1
		lsl	r2			;d=b*2
		rol	r3
		lds	Acc,_GraphXStart+0
		sub	r2,Acc			;d=b*2-a
		lds	Acc,_GraphXStart+1
		sbc	r3,Acc
	;+---------------+
	;| do while ct>0 |
	;+---------------+
	_GRAPH_WHILE:
		tst	r1
		brmi	_GRAPH_LDONE			;yes, done
		tst	r0
		brne	_GRAPH_L35
		tst	r1
		brne	_GRAPH_L35
		rjmp	_GRAPH_LDONE
	_GRAPH_L35:
		lds	Acc,_GRAPH_LINE_COLOR		;get color
		rcall	_GRAPH_LINE_SET_PIXEL		;set a pixel
		ldi	Acc,1
		sub	r0,Acc			;ct--
		ldi	Acc,0
		sbc	r1,Acc
		tst	r3			;d>0 ?
		brpl	_GRAPH_L40			;yes, jump
		add	XL,r4			;x+=xn
		adc	XH,r5
		add	YL,r6			;y+=yn
		adc	YH,r7
		lds	Acc,_GraphYStart+0
		mov	r12,Acc			;tmp=b
		lds	Acc,_GraphYStart+1
		mov	r13,Acc
		rjmp	_GRAPH_L50
	_GRAPH_L40:
		add	XL,r8			;x+=xd
		adc	XH,r9
		add	YL,r10			;y+=yd
		adc	YH,r11
		lds	Acc,_GraphYStart+0
		mov	r12,Acc			;tmp=b
		lds	Acc,_GraphYStart+1
		mov	r13,Acc
		lds	Acc,_GraphXStart+0
		sub	r12,Acc			;tmp=b-a
		lds	Acc,_GraphXStart+1
		sbc	r13,Acc	
	_GRAPH_L50:
		lsl	r12			;tmp*=2
		rol	r13
		add	r2,r12		;d+=tmp*2
		adc	r3,r13
		rjmp	_GRAPH_WHILE
	_GRAPH_LDONE:
		popaw
		popw	Z
		popw	Y
		stsw	_GraphYStart,Y			;Ystart=Yend
		popw	X
		stsw	_GraphXStart,X			;Xstart=Xend
		ret
	;+------------------------------------------+
	;| Set Pixel addr function	 	    |
	;| Input: AccH:Acc address		    |
	;+------------------------------------------+
	;					     \___________________________________________________________________________________________
	_GRAPH_LINE_SET_PIXEL_ADDR:
		stsaw	_GRAPH_LINE_PIXEL_ADDR
		ret
	;+------------------------------------------+
	;| Set Pixel function			    |
	;| Input: x,y coordinates		    |
	;|	  Acc color			    |
	;| Destroyed: Z				    |
	;+------------------------------------------+
	;					     \___________________________________________________________________________________________
	_GRAPH_LINE_SET_PIXEL:
		ldsw	Z,_GRAPH_LINE_PIXEL_ADDR
		icall
		ret
	;+------------------------------------------+
	;| Set Pixel Color			    |
	;| Input: Acc Color			    |
	;+------------------------------------------+
	;					     \___________________________________________________________________________________________
	_GRAPH_LINE_SET_COLOR:
		sts	_GRAPH_LINE_COLOR,Acc
		ret
	;+------------------------------------------+
	;| MoveTo				    |
	;| Input: Acc : AccH = x coordinate to move |
	;|	  AccT:AccTH = y coordinate to move |
	;+------------------------------------------+
	;					     \___________________________________________________________________________________________
	_GRAPH_MOVE_TO:
		sts	_GraphXStart+0,Acc
		sts	_GraphXStart+1,AccH
		sts	_GraphYStart+0,AccT
		sts	_GraphYStart+1,AccTH
		ret
	;+------------------------------------------+
	;| MoveToEx				    |
	;| Input: X coordinate to move 		    |
	;|	  Y coordinate to move 		    |
	;+------------------------------------------+
	;					     \___________________________________________________________________________________________
	_GRAPH_MOVE_TO_EX:
		sts	_GraphXStart+0,XL
		sts	_GraphXStart+1,XH
		sts	_GraphYStart+0,YL
		sts	_GraphYStart+1,YH
		ret
	;+------------------------------------------+
	;| MoveToEx				    |
	;| Output:X coordinate 			    |
	;|	  Y coordinate	 		    |
	;+------------------------------------------+
	;					     \___________________________________________________________________________________________
	_GRAPH_GET_POINT:
		lds	XL,_GraphXStart+0
		lds	XH,_GraphXStart+1
		lds	YL,_GraphYStart+0
		lds	YH,_GraphYStart+1
		ret
#endif
.exit

