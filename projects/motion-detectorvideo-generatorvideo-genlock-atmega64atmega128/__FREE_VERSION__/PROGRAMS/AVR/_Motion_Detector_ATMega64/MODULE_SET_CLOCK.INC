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
;=======> 			M O D U L E  -  C L O C K  S E T					|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module set RTC clock					 			|
;	 -----------------------------------------------------------------------------------------------
	
	.INCLUDE	"BMPS\BMP_CLOCK_SET_1.INC"		;clock image

	_MODULE_CLOCK_SET:
		ldiw	Z,BMP_CLOCK_SET_1*2			;clock image
		call	_RASTER_BMP_GET_WIDTH			;center on screen
		ldiw	X,SCREEN_WIDTH
		sub	XL,Acc
		sbc	XH,AccH
		lsrw	X
		call	_RASTER_BMP_GET_HEIGHT
		ldiw	Y,SCREEN_HEIGHT
		sub	YL,Acc
		sbc	YH,AccH
		lsrw	Y
		;/////////////////////////////////
		;// MAIN CLOCK SET LOOP
		;/////////////////////////////////
		ldi	AccT,0					;0 for hour, 1 for minute
	_MODULE_CLOCK_SET_LOOP:
		;//////////////////
		;// DRAW BACKGROUND CLOCK IMAGE
		;//////////////////
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL		;synchronize with vertical retrace
		call 	_RASTER_SET_BMP_FAST			;show clock set image
		;//////////////////
		;// DRAW CLOCK NUMBERS
		;//////////////////
		pushw	X					;save coordinates and image
		pushw	Y
		pushw	Z
		addiw	X,7
		addiw	Y,40
		call	_RTC_GET_HOUR				;get hour
		call	_UBTOSU					;to ascii
		adiw	ZL,2
		call	_RASTER_SET_TEXT_OUT_SRAM		;plot on screen
		addiw	X,2
		call	_RTC_GET_MINUTE				;get minute
		call	_UBTOSU					;to ascii
		adiw	ZL,2
		call	_RASTER_SET_TEXT_OUT_SRAM		;plot on screen
		addiw	Y,8
		subiw	X,26
		cpi	AccT,0					;set minute ?
		breq	_MODULE_CLOCK_SET_LOOP_10		;yes, branch
		addiw	X,14
	_MODULE_CLOCK_SET_LOOP_10:
		ldi	XH,11
		ldi	YH,2
		ldi	Temp,_OFF
		call	_RASTER_SET_BOX_FULL_FAST		;DRAW SELECTION
		popw	Z
		popw	Y
		popw	X
		;//////////////////
		;// DRAW CLOCK POINTERS
		;//////////////////
		pushw	X
		pushw	Y
		push	AccT
		;//////////////////
		;// SECOND ARROW
		;//////////////////
		addiw	X,19					;center clock pointer
		addiw	Y,19
		call	_ROT2D_SET_CENTER_POINT			;set clock pointers center
		subiw	Y,18					;start at 12 o clock
		call	_ROT2D_SET_POINT
		call	_RTC_GET_SECOND				;get minute
		ldi	AccH,6					;minute*6
		mul	Acc,AccH
		movw	Acc,r0
		call	_ROT2D_SET_ANGLE			;set minute pointer
		call	_ROT2D_ROTATE				;rotate pointer
		call	_ROT2D_GET_CENTER_POINT
		call	_GRAPH_MOVE_TO_EX			
		call	_ROT2D_GET_ROTATED_POINT
		ldi	Acc,_OFF				;black pointer
		call	_GRAPH_LINE_TO				;draw minute pointer
		;//////////////////
		;// MINUTE ARROW
		;//////////////////
		call	_ROT2D_GET_CENTER_POINT			;set clock pointers center
		subiw	Y,11					;start at 12 o clock
		call	_ROT2D_SET_POINT
		call	_RTC_GET_MINUTE				;get minute
		ldi	AccH,6					;minute*6
		mul	Acc,AccH
		movw	Acc,r0
		call	_ROT2D_SET_ANGLE			;set minute pointer
		call	_ROT2D_ROTATE				;rotate pointer
		call	_ROT2D_GET_CENTER_POINT
		call	_GRAPH_MOVE_TO_EX			
		call	_ROT2D_GET_ROTATED_POINT
		ldi	Acc,_OFF				;black pointer
		call	_GRAPH_LINE_TO				;draw minute pointer
		;//////////////////
		;// HOUR ARROW
		;//////////////////
		call	_ROT2D_GET_CENTER_POINT			;set clock pointers center
		subiw	Y,8					;start at 12 o clock
		call	_ROT2D_SET_POINT
		call	_RTC_GET_HOUR				;get hour
		cpi	Acc,12					;hour>=12?
		brlo	PC+2					;no, branch
		subi	Acc,12					;else hour-=12
		ldi	AccH,60					;(hour*60+minute)/2
		mul	Acc,AccH
		call	_RTC_GET_MINUTE
		ldi	AccH,0
		add	Acc,r0
		adc	AccH,r1
		lsr	AccH
		ror	Acc
		call	_ROT2D_SET_ANGLE			;set minute pointer
		call	_ROT2D_ROTATE				;rotate pointer
		call	_ROT2D_GET_CENTER_POINT
		call	_GRAPH_MOVE_TO_EX			
		call	_ROT2D_GET_ROTATED_POINT
		ldi	Acc,_OFF				;black pointer
		call	_GRAPH_LINE_TO				;draw hour pointer
		pop	AccT
		popw	Y
		popw	X
		;/////////////////////////////////
		;// GET KEYBOARD CONTROLS
		;/////////////////////////////////
		call	_KEYBOARD_READ_FLASH			;read key flash
		cpi	Acc,_KEY_NONE				;none key pressed?
		lbreq	_MODULE_CLOCK_SET_LOOP			;yes, loop
		_M_DELAY_MILLISECONDS	50
		cpi	AccT,0					;hour set ?
		brne	_MODULE_CLOCK_SET_LOOP_10A		;no, branch 
		_M_DELAY_MILLISECONDS	100
	_MODULE_CLOCK_SET_LOOP_10A:
		cpi	Acc,_KEY_LEFT				;left key ?
		brne	_MODULE_CLOCK_SET_LOOP_20		;no, branch
		ldi	AccT,0					;set hour flag
		rjmp	_MODULE_CLOCK_SET_LOOP			;wait another key
	_MODULE_CLOCK_SET_LOOP_20:
		cpi	Acc,_KEY_RIGHT				;right key ?
		brne	_MODULE_CLOCK_SET_LOOP_30		;no, branch
		ldi	AccT,1					;set minute flag
		rjmp	_MODULE_CLOCK_SET_LOOP			;wait another key
	_MODULE_CLOCK_SET_LOOP_30:
		cpi	Acc,_KEY_UP				;up key ?
		brne	_MODULE_CLOCK_SET_LOOP_40		;no, branch
		;/////////////////////////////////
		;// INCREASE HOUR OR MINUTE
		;/////////////////////////////////
		cpi	AccT,0					;hour?
		brne	_MODULE_CLOCK_SET_LOOP_40B		;no, branch
		call	_RTC_GET_HOUR				;get hour
		inc	Acc					;hour++
		cpi	Acc,24					;>23?
		brlo	_MODULE_CLOCK_SET_LOOP_40A		;no, branch
		ldi	Acc,0					;else hour=0
	_MODULE_CLOCK_SET_LOOP_40A:
		call	_RTC_SET_HOUR
		rjmp	_MODULE_CLOCK_SET_LOOP			;wait another key
	_MODULE_CLOCK_SET_LOOP_40B:
		call	_RTC_GET_MINUTE				;get MINUTE
		inc	Acc					;minute++
		cpi	Acc,60					;>59?
		brlo	_MODULE_CLOCK_SET_LOOP_40C		;no, branch
		ldi	Acc,0					;eslse minute=0
	_MODULE_CLOCK_SET_LOOP_40C:
		call	_RTC_SET_MINUTE
		rjmp	_MODULE_CLOCK_SET_LOOP			;wait another key
	_MODULE_CLOCK_SET_LOOP_40:
		cpi	Acc,_KEY_DOWN				;down key ?
		brne	_MODULE_CLOCK_SET_LOOP_50		;no, branch
		;/////////////////////////////////
		;// DECREASE HOUR OR MINUTE
		;/////////////////////////////////
		cpi	AccT,0					;hour?
		brne	_MODULE_CLOCK_SET_LOOP_50B		;no, branch
		call	_RTC_GET_HOUR				;get hour
		dec	Acc					;hour--
		cpi	Acc,0					;<0?
		brpl	_MODULE_CLOCK_SET_LOOP_50A		;no, branch
		ldi	Acc,23					;else hour=23
	_MODULE_CLOCK_SET_LOOP_50A:
		call	_RTC_SET_HOUR
		rjmp	_MODULE_CLOCK_SET_LOOP			;wait another key
	_MODULE_CLOCK_SET_LOOP_50B:
		call	_RTC_GET_MINUTE				;get MINUTE
		dec	Acc					;minute--
		cpi	Acc,0					;<0?
		brpl	_MODULE_CLOCK_SET_LOOP_50C		;no, branch
		ldi	Acc,59					;eslse minute=59
	_MODULE_CLOCK_SET_LOOP_50C:
		call	_RTC_SET_MINUTE
		rjmp	_MODULE_CLOCK_SET_LOOP			;wait another key
	_MODULE_CLOCK_SET_LOOP_50:
		call	_BOOT_SET_RTC_OK			;set clock ok
		call	fn_MENU_DRAW_BACKGROUND			;redraw background
		call	_MENU_DRAW_STACK			;redraw menus in stack
		ret
