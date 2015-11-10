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
;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======> 			M O D U L E  -  S H O W  M O T I O N  D E T E C T I O N  T Y P E S	|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module show motion detection type according setting motion parameters		|
;	|												|
;	| Input:  AccH ITEM OF _MENU_PORTUGUESE_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO_TIPO 		|
;	|												|
;	| Observations: Assume that screen already in _VIDEO_MODE_128X96X2G otherwise a error occur	|
;	 -----------------------------------------------------------------------------------------------

		
	_MODULE_SHOW_MOTION_DETECTION:
		push	AccH
		call	fn_VIDEO_PRESENT				;video present ?
		pop	AccH
	_MODULE_SHOW_MOTION_DETECTION_10:
		call	_VIDEO_DIGITIZING_GET_STATE			;get digitizing state
		cpi	Acc,_ON						;ready ?
		brne	_MODULE_SHOW_MOTION_DETECTION_10		;no, wait
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL			;synchronize with vertical retrace
		call	_VIDEO_DIGITIZING_GET_SENSIBILITY		;get sensibility
		mov	TempH,Acc					;TempH=sensibility
		;///////////////////////////////
		;// 
		;// JUMP TO APPROPRIATE CODE
		;//
		;///////////////////////////////
		cpi	AccH,_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_BOX
		lbreq	_MODULE_SHOW_MOTION_DETECTION_BOX
		cpi	AccH,_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_BOX_FULL
		lbreq	_MODULE_SHOW_MOTION_DETECTION_BOX_FULL
		cpi	AccH,_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_CONTORNO
		lbreq	_MODULE_SHOW_MOTION_DETECTION_CONTORNO
		cpi	AccH,_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_CENTRO
		lbreq	_MODULE_SHOW_MOTION_DETECTION_CENTRO
		cpi	AccH,_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_GRAFICO
		lbreq	_MODULE_SHOW_MOTION_DETECTION_GRAFICO
		call	fn_DRAW_LEVEL_BAR
		call	fn_DRAW_EYE
		ret
		;/////////////////////////////////////////
		;//					//
		;// 	SHOW MOTION USING BOXES		//
		;//					//
		;/////////////////////////////////////////
	_MODULE_SHOW_MOTION_DETECTION_BOX:
		call	fn_CLEAR_MOTION_AREA			;clear screen
		ldi	XL,0					;column index
		ldi	YL,0					;row index
		clr	r15					;max level =0
	_MODULE_SHOW_MOTION_DETECTION_BOX_LOOP:
		call	_VIDEO_DIGITIZING_GET_DIFFERENCE_DOT	;get difference dot
		cp	Acc,r15					;great than max level?
		brlo	PC+2
		mov	r15,Acc					;yes, update max, level
		cp	Acc,TempH				;has movement?
		brlo	_MODULE_SHOW_MOTION_DETECTION_BOX_LOOP_10	;no, branch
		ldi	Temp,_ON				;else assume has motion
		push	XL					;save indexes
		push	YL
		ldi	Acc,5					;column*5
		mul	XL,Acc
		mov	XL,r0
		ldi	Acc,4					;row*4
		mul	YL,Acc
		mov	YL,r0
		ldi	XH,5
		ldi	YH,4
		call	_RASTER_SET_BOX_FAST			;DRAW BOX
		pop	YL					;restore indexes
		pop	XL
	_MODULE_SHOW_MOTION_DETECTION_BOX_LOOP_10:
		inc	XL					;column++
		cpi	XL,_VIDEO_DIGITIZING_COLS		;last column ?
		brlo	_MODULE_SHOW_MOTION_DETECTION_BOX_LOOP	;no, continue
		ldi	XL,0					;clear column
		inc	YL					;row++
		cpi	YL,_VIDEO_DIGITIZING_ROWS		;last row ?
		brlo	_MODULE_SHOW_MOTION_DETECTION_BOX_LOOP	;no, continue
		mov	Acc,r15
		call	fn_DRAW_LEVEL_BAR
		call	fn_DRAW_EYE
		ret
		;/////////////////////////////////////////
		;//					//
		;// 	SHOW MOTION USING BOXES FULL	//
		;//					//
		;/////////////////////////////////////////
	_MODULE_SHOW_MOTION_DETECTION_BOX_FULL:
		call	fn_CLEAR_MOTION_AREA				;clear screen
		ldi	XL,0					;column index
		ldi	YL,0					;row index
		clr	r15					;max level=0
	_MODULE_SHOW_MOTION_DETECTION_BOX_FULL_LOOP:
		call	_VIDEO_DIGITIZING_GET_DIFFERENCE_DOT	;get difference dot
		cp	Acc,r15					;great than max level?
		brlo	PC+2
		mov	r15,Acc					;yes, update max level
		cp	Acc,TempH				;has movement?
		brlo	_MODULE_SHOW_MOTION_DETECTION_BOX_FULL_LOOP_10	;no, branch
		ldi	Temp,_ON				;else assume has motion
		push	XL					;save indexes
		push	YL
		ldi	Acc,5					;column*5
		mul	XL,Acc
		mov	XL,r0
		ldi	Acc,4					;row*4
		mul	YL,Acc
		mov	YL,r0
		ldi	XH,5
		ldi	YH,4
		call	_RASTER_SET_BOX_FULL_FAST		;DRAW BOX
		pop	YL					;restore indexes
		pop	XL
	_MODULE_SHOW_MOTION_DETECTION_BOX_FULL_LOOP_10:
		inc	XL					;column++
		cpi	XL,_VIDEO_DIGITIZING_COLS		;last column ?
		brlo	_MODULE_SHOW_MOTION_DETECTION_BOX_FULL_LOOP	;no, continue
		ldi	XL,0					;clear column
		inc	YL					;row++
		cpi	YL,_VIDEO_DIGITIZING_ROWS		;last row ?
		brlo	_MODULE_SHOW_MOTION_DETECTION_BOX_FULL_LOOP	;no, continue
		mov	Acc,r15
		call	fn_DRAW_LEVEL_BAR
		call	fn_DRAW_EYE
		ret
		;/////////////////////////////////////////
		;//					//
		;// 	SHOW MOTION USING COUNTOR	//
		;//					//
		;/////////////////////////////////////////
	_MODULE_SHOW_MOTION_DETECTION_CONTORNO:
		call	fn_CLEAR_MOTION_AREA				;clear screen	
		ldi	XL,0					;column index
		ldi	YL,0					;row index
		clr	r15
	_MODULE_SHOW_MOTION_DETECTION_CONTORNO_LOOP:
		call	_VIDEO_DIGITIZING_GET_DIFFERENCE_DOT	;get difference dot(X+0,Y+0)
		cp	Acc,r15
		brlo	PC+2
		mov	r15,Acc
		ldi	Temp,_ON
		cp	Acc,TempH
		brsh	PC+2
		ldi	Temp,_OFF
		mov	r0,Temp					;r0=dot(X+0,Y+0)
		inc	XL
		call	_VIDEO_DIGITIZING_GET_DIFFERENCE_DOT	;get difference dot(X+1,Y+0)
		cp	Acc,r15
		brlo	PC+2
		mov	r15,Acc
		ldi	Temp,_ON
		cp	Acc,TempH
		brsh	PC+2
		ldi	Temp,_OFF
		mov	r1,Temp					;r1=dot(X+1,Y+0)
		dec	XL
		inc	YL
		call	_VIDEO_DIGITIZING_GET_DIFFERENCE_DOT	;get difference dot(X+0,Y+1)
		cp	Acc,r15
		brlo	PC+2
		mov	r15,Acc
		ldi	Temp,_ON
		cp	Acc,TempH
		brsh	PC+2
		ldi	Temp,_OFF
		mov	r2,Temp					;r2=dot(X+0,Y+1)
		dec	YL
		push	XL					;save indexes
		push	YL
		push	r0					;save dots
		push	r1
		push	r2
		ldi	Temp,_OFF
		cp	r0,r1					;dot(X+0,Y+0)=dot(X+1,Y+0)?
		breq	PC+2					;yes, branch
		ldi	Temp,_ON
		ldi	Acc,5					;column*5
		mul	XL,Acc
		mov	XL,r0
		ldi	Acc,4					;row*4
		mul	YL,Acc
		mov	YL,r0
		ldi	YH,5					;height = 4
		addi	XL,5
		cpi	Temp,_ON
		brne	PC+3
		call	_RASTER_SET_VLINE_FAST			;DRAW BOX
	_MODULE_SHOW_MOTION_DETECTION_CONTORNO_LOOP_10:
		pop	r2					;restore dots
		pop	r1
		pop	r0
		ldi	Temp,_OFF
		cp	r0,r2					;dot(X+0,Y+0)=dot(X+0,Y+1)?
		breq	PC+2					;yes, branch
		ldi	Temp,_ON
		subi	XL,5
		addi	YL,4
		ldi	XH,5					;width = 5
		cpi	Temp,_ON
		brne	PC+3
		call	_RASTER_SET_HLINE_FAST			;DRAW BOX
		pop	YL					;restore indexes
		pop	XL
		inc	XL					;column++
		cpi	XL,_VIDEO_DIGITIZING_COLS		;last column ?
		lbrlo	_MODULE_SHOW_MOTION_DETECTION_CONTORNO_LOOP	;no, continue
		ldi	XL,0					;clear column
		inc	YL					;row++
		cpi	YL,_VIDEO_DIGITIZING_ROWS		;last row ?
		lbrlo	_MODULE_SHOW_MOTION_DETECTION_CONTORNO_LOOP	;no, continue
		mov	Acc,r15
		call	fn_DRAW_LEVEL_BAR
		call	fn_DRAW_EYE
		ret
		;/////////////////////////////////////////
		;//					//
		;// 	SHOW MOTION USING CENTER CROSS	//
		;//					//
		;/////////////////////////////////////////
	_MODULE_SHOW_MOTION_DETECTION_CENTRO:
		ldi	XL,0					;column index
		ldi	YL,0					;row index
		ldiw	Z,0					;clear X center
		ldiawt	0					;clear Y center
		clr	r2					;r3:r2=motion pixels count
		clr	r3
		clr	r15					;max value
	_MODULE_SHOW_MOTION_DETECTION_CENTRO_LOOP:
		call	_VIDEO_DIGITIZING_GET_DIFFERENCE_DOT	;get difference dot
		cp	Acc,r15					;dif>max?
		brlo	PC+2					;no, branch
		mov	r15,Acc					;else r15=max
		cp	Acc,TempH				;has movement?
		brsh	PC+2					;yes, branch
		clr	Acc

		mul	Acc,XL					;compute graph mass of X coord
		add	ZL,r0
		adc	ZH,r1

		mul	Acc,YL					;compute graph mass of Y coord
		add	AccT,r0
		adc	AccTH,r1

		add	r2,Acc					;compute total mass
		ldi	Acc,0
		adc	r3,Acc

	_MODULE_SHOW_MOTION_DETECTION_CENTRO_LOOP_10:
		inc	XL					;column++
		cpi	XL,_VIDEO_DIGITIZING_COLS		;last column ?
		brlo	_MODULE_SHOW_MOTION_DETECTION_CENTRO_LOOP	;no, continue
		ldi	XL,0					;clear column
		inc	YL					;row++
		cpi	YL,_VIDEO_DIGITIZING_ROWS		;last row ?
		brlo	_MODULE_SHOW_MOTION_DETECTION_CENTRO_LOOP	;no, continue
		mov	r0,ZL					;compute center Xc
		mov	r1,ZH
		call	_DIV_U16U16U16				;Xc=(x0+x1...+xn)/total.graph.mass
		ldi	XL,5					;Xc*=5
		mul	XL,r0
		mov	XL,r0
		mov	r0,AccT					;compute center Yc
		mov	r1,AccTH
		call	_DIV_U16U16U16				;Yc=(y0+y1...+yn)/total.graph.mass
		ldi	YL,4					;Yc*=4
		mul	YL,r0
		mov	YL,r0
		;////////////////////////
		;/// PUT NEW COORDINATE INTO A SHIFT QUEUE BUFFER OF POINT
		;////////////////////////

	.EQU	POINTS_LENGHT	= 32

		pushw	X
		pushw	Y
		ldiw	Y,POINTS+2				;Y-->Points(1)
		ldiw	X,POINTS+0				;X-->Points(0)
		ldi	Acc,POINTS_LENGHT			;points.count
	_MODULE_SHOW_MOTION_DETECTION_CENTRO_10:
		ld	Temp,Y+					;shif a pair of coordinates
		st	X+,Temp
		ld	Temp,Y+
		st	X+,Temp
		dec	Acc					;points--
		brne	_MODULE_SHOW_MOTION_DETECTION_CENTRO_10	;continue
		movw	ZL,XL					;Z-->last coordinate
		popw	Y
		popw	X
		st	Z+,XL					;store new points
		st	Z+,YL
		;////////////////////////
		;/// DRAW PATH CROSSES
		;////////////////////////
		call	fn_CLEAR_MOTION_AREA			;clear screen
		push	XL
		push	YL
		ldiw	Z,POINTS				;Z-->POINTS
		ldi	Acc,POINTS_LENGHT-1			;points.count-1
	_MODULE_SHOW_MOTION_DETECTION_CENTRO_20:
		ld	XL,Z+
		ld	YL,Z+
		subi	XL,2					;because little cross is 10 by 10
		ldi	XH,5
		ldi	Temp,_ON
		call	_RASTER_SET_HLINE_FAST			;draw horizontal cross line
		addi	XL,2
		subi	YL,2
		ldi	YH,5
		call	_RASTER_SET_VLINE_FAST			;draw vertical cross line
		dec	Acc					;points--
		brne	_MODULE_SHOW_MOTION_DETECTION_CENTRO_20	;continue
		pop	YL
		pop	XL
		;////////////////////////
		;/// DRAW MAIN CROSS
		;////////////////////////
		push	YL					;save Yc
		ldi	YL,0
		ldi	YH,SCREEN_HEIGHT			;screen height
		ldi	Temp,_ON				;draw vertical line
		call	_RASTER_SET_VLINE_FAST
		pop	YL
		ldi	XL,0
		ldi	XH,SCREEN_WIDTH				;screen width
		ldi	Temp,_ON
		call	_RASTER_SET_HLINE_FAST			;draw horizontal line
		mov	Acc,r15					;draw max value captured
		call	fn_DRAW_LEVEL_BAR
		call	fn_DRAW_EYE
		ret
		;/////////////////////////////////////////
		;//					//
		;// 	SHOW MOTION USING XY GRAPHIC	//
		;//					//
		;/////////////////////////////////////////

		.INCLUDE "BMPS\BMP_TIME_XY_NTSC.INC"		;for 60Hz

	.EQU	POINTS_SIZE	= 128
	.EQU	POINT_WIDTH	= 1

	_MODULE_SHOW_MOTION_DETECTION_GRAFICO:
		call	fn_CLEAR_MOTION_AREA			;clear screen
		ldiw	X,0
		ldiw	Y,SCREEN_HEIGHT-48
		ldiw	Z,BMP_TIME_XY_NTSC*2
		call 	_RASTER_SET_BMP_FAST			;show time bar
		ldi	XL,0					;column index
		ldi	YL,0					;row index
		clr	r15
		;//////////////////////////
		;// GET DOT DIFF LEVEL
		;//////////////////////////
	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_10:
		call	_VIDEO_DIGITIZING_GET_DIFFERENCE_DOT	;get difference dot
		cp	Acc,r15					;get max signal	
		brlo	PC+2
		mov	r15,Acc
		inc	XL					;column++
		cpi	XL,_VIDEO_DIGITIZING_COLS		;last column ?
		brlo	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_10	;no, continue
		ldi	XL,0					;clear column
		inc	YL					;row++
		cpi	YL,_VIDEO_DIGITIZING_ROWS		;last row ?
		brlo	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_10	;no, continue
		;//////////////////////////
		;// SHIFT OLD LEVELS
		;//////////////////////////
		mov	Acc,r15
		call	fn_DRAW_LEVEL_BAR
		ldiw	X,POINTS				;shift and store new value
		ldiw	Y,POINTS+1
		ldi	Acc,POINTS_SIZE
	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_20:
		ld	AccH,Y+					;get front
		st	X+,AccH					;store back
		dec	Acc
		brne	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_20
		st	X,r15					;store new value
		;//////////////////////////
		;// PLOT DOT LEVELS
		;//////////////////////////
		ldiw	Z,POINTS				;Z-->queue values
		ldi	XL,0					;draw bars
		ldi	AccT,POINTS_SIZE
	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_30:
		ld	Temp,Z+					;get data(n)
		lsr	Temp					;data(n)/4
		lsr	Temp
		ldi	Acc,SCREEN_HEIGHT-16			;Acc=Y(n)=data(n)/4
		sub	Acc,Temp

		ld	Temp,Z					;get data(n+1)
		lsr	Temp					;data(n+1)/4
		lsr	Temp
		ldi	AccH,SCREEN_HEIGHT-16			;AccH=Y(n+1)=data(n+1)/4
		sub	AccH,Temp

		cp	AccH,Acc				;Y(n+1)>=Y(n)?
		brsh	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_40	;yes, branch
		mov	YL,AccH					;Yi
		sub	Acc,AccH
		mov	YH,Acc					;h
		rjmp	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_50
	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_40:
		mov	YL,Acc
		sub	AccH,Acc
		mov	YH,AccH
	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_50:
		ldi	Temp,_ON
		ldi	XH,POINT_WIDTH
		inc	YH
		call	_RASTER_SET_BOX_FULL_FAST
		addi	XL,POINT_WIDTH
		dec	AccT
		brne	_MODULE_SHOW_MOTION_DETECTION_GRAFICO_30
		call	fn_DRAW_EYE
		ret
