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
;=======> 			M O D U L E  -  S H O W  D I G I T I Z E D  I M A G E  A R E A		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module show digitized image capture area movemented with genlock 			|
;	 -----------------------------------------------------------------------------------------------

	_MODULE_SHOW_DIGITIZED_AREA:
		call	_KEYBOARD_READ_FLASH				;wait last key release
		cpi	Acc,_KEY_NONE
		brne	_MODULE_SHOW_DIGITIZED_AREA
		call	_KEYBOARD_DEBOUNCE
		call	_VIDEO_OUT_GET_MODE				;get actual video mode
		push	Acc						;save mode
		call	fn_VIDEO_PRESENT				;se if video present ?
		lbrcc	_MODULE_SHOW_DIGITIZED_AREA_10A			;yes, branch
		call	_KEYBOARD_READ
		rjmp	_MODULE_SHOW_DIGITIZED_AREA_EXIT
	_MODULE_SHOW_DIGITIZED_AREA_10A:
		ldi	Acc,_VIDEO_MODE_128x96x2G			;set appropriate video mode with genlock
		call	_VIDEO_OUT_SET_MODE
		ldi	Acc,_VIDEO_OUT_LEVEL_WHITE_TOP/2		;set white level to min
		call	_VIDEO_OUT_SET_WHITE_LEVEL
		call	_VIDEO_OUT_CLS					;clear screen
	_MODULE_SHOW_DIGITIZED_AREA_10:
		call	_VIDEO_DIGITIZING_GET_STATE			;get digitizing state
		cpi	Acc,_ON						;ready ?
		brne	_MODULE_SHOW_DIGITIZED_AREA_10			;no, wait
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL			;synchronize with vertical retrace
		ldiw	Z,_VIDEO_OUT_BUFFER_DIG_DIF			;Z-->digitized moviment video
		ldiw	X,0						;start dot video (0,0)
		ldiw	Y,0
	_MODULE_SHOW_DIGITIZED_AREA_20:
		call	_VIDEO_DIGITIZING_GET_SENSIBILITY		;get sensibility
		mov	AccH,Acc
		ld	Acc,Z+						;get video data
		cp	Acc,AccH					;8 bits range ?
		ldi	Temp,_OFF
		brlo	_MODULE_SHOW_DIGITIZED_AREA_30			;yes, branch
		ldi	Temp,_ON						;else saturate
	_MODULE_SHOW_DIGITIZED_AREA_30:
		pushw	X
		pushw	Y
		ldi	XH,5
		ldi	YH,4
		call	_RASTER_SET_BOX_FAST				;set pixel
		popw	Y
		popw	X
		addiw	X,5						;next col
		cpiw	X,120
		brlo	_MODULE_SHOW_DIGITIZED_AREA_20			;no, until row done
		ldiw	X,0						;X=0
		addiw	Y,4						;next row
		cpiw	Y,SCREEN_HEIGHT
		brlo	_MODULE_SHOW_DIGITIZED_AREA_20			;no, until screen done
		call	_KEYBOARD_READ_FLASH				;read keyboard
		cpi	Acc,_KEY_NONE					;no key  pressed ?
		breq	_MODULE_SHOW_DIGITIZED_AREA_10			;no, show digitized screen again
	_MODULE_SHOW_DIGITIZED_AREA_EXIT:
		call	_KEYBOARD_DEBOUNCE
		pop	Acc
		call	_VIDEO_OUT_SET_MODE				;restore default mode
		ldi	Acc,_VIDEO_OUT_LEVEL_WHITE_TOP			;set white level to min
		call	_VIDEO_OUT_SET_WHITE_LEVEL
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
