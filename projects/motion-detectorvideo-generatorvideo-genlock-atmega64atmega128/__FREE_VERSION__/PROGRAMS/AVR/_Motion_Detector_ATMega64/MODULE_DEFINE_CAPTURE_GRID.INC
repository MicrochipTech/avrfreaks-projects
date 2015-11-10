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
;=======> 	      M O D U L E  -  D E F I N E  C A P T U R E  G R I D		 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module show capture grid in screen and allow user modify.				|
;	 -----------------------------------------------------------------------------------------------

	_MODULE_CAPTURE_GRID:
		call	_VIDEO_OUT_GET_MODE				;get actual mode
		push	Acc						;save
	_MODULE_CAPTURE_GRID_10A:
		ldi	Acc,_VIDEO_MODE_128X96X2G			;set new mode
		call	_VIDEO_OUT_SET_MODE
		call	_VIDEO_OUT_CLS					;clear screen
		call	fn_LOAD_CAPTURE_GRID				;load data from EEPROM
		call	fn_DRAW_CAPTURE_GRID				;draw grid
		ldi	AccT,0
		ldi	AccTH,0
		ldi	AccH,_OFF					;pen=off
	_MODULE_CAPTURE_GRID_10:
		call	fn_GET_GRID_CELL				;get grid state
		call	fn_DRAW_CAPTURE_GRID_CELL			;draw
		ldi	Temp,_ON					;mark selected 
		call	fn_DRAW_CAPTURE_GRID_SELECTION			;draw
		call	_KEYBOARD_READ					;read keybord state
		brcc	_MODULE_CAPTURE_GRID_20				;time out , no branch
		cpi	Acc,_KEY_ENTER					;ENTER KEY ?
		breq	_MODULE_CAPTURE_GRID_EXIT			;yes, exit
	_MODULE_CAPTURE_GRID_20:
		ldi	Temp,_OFF					;unmark selected 
		call	fn_DRAW_CAPTURE_GRID_SELECTION			;draw
		cpi	Acc,_KEY_UP					;UP KEY ?
		breq	_MODULE_CAPTURE_GRID_KEY_UP			;yes, brancH
		cpi	Acc,_KEY_DOWN					;DOWN KEY ?
		breq	_MODULE_CAPTURE_GRID_KEY_DOWN			;yes, brancH
		cpi	Acc,_KEY_LEFT					;LEFT KEY ?
		breq	_MODULE_CAPTURE_GRID_KEY_LEFT			;yes, brancH
		cpi	Acc,_KEY_RIGHT					;RIGHT KEY ?
		breq	_MODULE_CAPTURE_GRID_KEY_RIGHT			;yes, brancH
		;//////////////////////////////////
		;// PROCESS KEY ENTER NO TIMEOUT
		;//////////////////////////////////
		com	AccH						;change pen state
		mov	Temp,AccH
		call	fn_SET_GRID_CELL				;set grid
		mov	TempH,AccH					;update last state
		rjmp	_MODULE_CAPTURE_GRID_10				;read key
		;//////////////////////////////////
		;// PROCESS UP
		;//////////////////////////////////
	_MODULE_CAPTURE_GRID_KEY_UP:
		dec	AccT						;row--
		cpi	AccT,0						;row<0 ?
		brge	_MODULE_CAPTURE_GRID_KEY_UP_10			;no, branch
		ldi	AccT,_VIDEO_DIGITIZING_MASK_ROWS-1				;else row=max
	_MODULE_CAPTURE_GRID_KEY_UP_10:
		mov	Temp,AccH
		call	fn_SET_GRID_CELL				;set grid
		rjmp	_MODULE_CAPTURE_GRID_10				;read key
		;//////////////////////////////////
		;// PROCESS DOWN
		;//////////////////////////////////
	_MODULE_CAPTURE_GRID_KEY_DOWN:
		inc	AccT						;row++
		cpi	AccT,_VIDEO_DIGITIZING_MASK_ROWS				;row>row max ?
		brlo	_MODULE_CAPTURE_GRID_KEY_DOWN_10		;no, branch
		ldi	AccT,0						;else row=0
	_MODULE_CAPTURE_GRID_KEY_DOWN_10:
		mov	Temp,AccH
		call	fn_SET_GRID_CELL				;set grid
		rjmp	_MODULE_CAPTURE_GRID_10				;read key
		;//////////////////////////////////
		;// PROCESS LEFT
		;//////////////////////////////////
	_MODULE_CAPTURE_GRID_KEY_LEFT:
		dec	AccTH						;col--
		cpi	AccTH,0						;col<0 ?
		brge	_MODULE_CAPTURE_GRID_KEY_LEFT_10		;no, branch
		ldi	AccTH,_VIDEO_DIGITIZING_MASK_COLS-1				;else col=max
	_MODULE_CAPTURE_GRID_KEY_LEFT_10:
		mov	Temp,AccH
		call	fn_SET_GRID_CELL				;set grid
		rjmp	_MODULE_CAPTURE_GRID_10				;read key
		;//////////////////////////////////
		;// PROCESS RIGHT
		;//////////////////////////////////
	_MODULE_CAPTURE_GRID_KEY_RIGHT:
		inc	AccTH						;col++
		cpi	AccTH,_VIDEO_DIGITIZING_MASK_COLS				;col>col max ?
		brlo	_MODULE_CAPTURE_GRID_KEY_RIGHT_10		;no, branch
		ldi	AccTH,0						;else row=0
	_MODULE_CAPTURE_GRID_KEY_RIGHT_10:
		mov	Temp,AccH
		call	fn_SET_GRID_CELL				;set grid
		rjmp	_MODULE_CAPTURE_GRID_10				;read key
	_MODULE_CAPTURE_GRID_EXIT:
		call	fn_GET_GRID_CELL				;to preserve last state
		com	Temp
		call	fn_SET_GRID_CELL
		pop	Acc
		call	_VIDEO_OUT_SET_MODE				;restore old mode
		call	_VIDEO_OUT_CLS
		ldi	Acc,_RASTER_CHAR_MODE_INVERTED
		call	_RASTER_SET_CHAR_MODE
	_MODULE_CAPTURE_GRID_EXIT_A:
		ldiw	Z,_MSG_CONFIRM
		call	fn_INPUT_BOX_CONFIRM				;input box confirm msg
		cpi	Acc,_MENU_INPUT_BOX_ANSWER_YES			;YES?
		brne	_MODULE_CAPTURE_GRID_EXIT_B			;no, branch
		call	fn_SAVE_CAPTURE_GRID				;else save data into EEPROM
		rjmp	_MODULE_CAPTURE_GRID_EXIT_C			;and exit
	_MODULE_CAPTURE_GRID_EXIT_B:
		cpi	Acc,_MENU_INPUT_BOX_ANSWER_NO			;NO?
		brne	_MODULE_CAPTURE_GRID_EXIT_A
	_MODULE_CAPTURE_GRID_EXIT_C:
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
