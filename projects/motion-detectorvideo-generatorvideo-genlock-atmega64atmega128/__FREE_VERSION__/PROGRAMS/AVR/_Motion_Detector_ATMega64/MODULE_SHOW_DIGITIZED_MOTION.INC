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
;=======> 	      M O D U L E  -  S H O W  D I G I T I Z E D  M O V E M E N T  I M A G E 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module show digitized image capture from camera and show on screen	only region	|
;	|	where has movement exit when pressed UP KEY								|
;	 -----------------------------------------------------------------------------------------------

	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO:
		call	_KEYBOARD_READ_FLASH				;wait last key release
		cpi	Acc,_KEY_NONE
		brne	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO
		call	_KEYBOARD_DEBOUNCE
		call	_VIDEO_OUT_GET_MODE				;get actual video mode
		push	Acc						;save mode
		call	fn_VIDEO_PRESENT				;se if video present ?
		lbrcc	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_10A	;yes, branch
		call	_KEYBOARD_READ
		rjmp	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_EXIT
	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_10A:
		ldi	Acc,_VIDEO_MODE_24x24x16D			;set appropriate video mode
		call	_VIDEO_OUT_SET_MODE
	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_10:
		call	_VIDEO_DIGITIZING_GET_STATE			;get digitizing state
		cpi	Acc,_ON						;ready ?
		brne	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_10	;no, wait
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL			;synchronize with vertical retrace
		ldiw	X,0						;start dot video (0,0)
		ldiw	Y,0
	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_20:
		call	_VIDEO_DIGITIZING_GET_DIFFERENCE_DOT		;get difference dot
		cpi	Acc,16
		brlo	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_30	;yes, branch
		ldi	Acc,15						;else saturate
	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_30:
		call	_RASTER_SET_PIXEL				;set pixel
		addiw	X,1						;next col
		lds	Acc,_VIDEO_OUT_GRAPH_WIDTH			;X>graph width
		ldi	AccH,0
		cp	XL,Acc
		cpc	XH,AccH	
		brlo	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_20	;no, until row done
		ldiw	X,0						;X=0
		addiw	Y,1						;next row
		lds	Acc,_VIDEO_OUT_GRAPH_HEIGHT			;Y>graph height
		ldi	AccH,0
		cp	YL,Acc
		cpc	YH,AccH
		brlo	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_20	;no, until screen done
		call	_KEYBOARD_READ_FLASH				;read keyboard
		cpi	Acc,_KEY_NONE					;no key  pressed ?
		breq	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_10	;no, show digitized screen again
	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO_EXIT:
		call	_KEYBOARD_DEBOUNCE
		pop	Acc
		call	_VIDEO_OUT_SET_MODE				;restore default mode
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
