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
;=======> 			M O D U L E  -  S H O W  A N A L O G  C L O C K 			|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module show analog RTC clock				 			|
;	 -----------------------------------------------------------------------------------------------


	.INCLUDE	"BMPS\BMP_CLOCK_ANALOGICO_2.INC"	;analog clock image

	_MODULE_SHOW_ANALOG_CLOCK:
		call	_VIDEO_OUT_CLS
	_MODULE_SHOW_ANALOG_CLOCK_10:
		call	_KEYBOARD_READ_FLASH			;wait key release
		cpi	Acc,_KEY_NONE				;no key pressed?
		brne	_MODULE_SHOW_ANALOG_CLOCK_10		;no, wait
		call	_KEYBOARD_DEBOUNCE			;debounce time
		;//////////////////////
		;/// GET RTC STATE
		;//////////////////////
		call	_BOOT_GET_RTC_STATE			;clock set?
		cpi	Acc,_ON
		breq	_MODULE_SHOW_ANALOG_CLOCK_20		;yes, branch
	_MODULE_SHOW_ANALOG_CLOCK_RTC_NOT_SET:
		ldiw	Z,_MSG_RTC_NOT_SET			;show clock not set message
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_TEXT_WIDTH			;center message on center screen
		ldiw	X,SCREEN_WIDTH
		sub	XL,Acc
		sbc	XH,AccH
		lsrw	X
		call	_RASTER_GET_TEXT_HEIGHT
		addiaw	16
		ldiw	Y,SCREEN_HEIGHT
		sub	YL,Acc
		sbc	YH,AccH
		lsrw	Y
		ldi	Acc,_MENU_INPUT_BOX_TYPE_OK		;INPUT BOX OK
		call	_MENU_INPUT_BOX
		call	fn_MENU_DRAW_BACKGROUND			;redraw background
		call	_MENU_DRAW_STACK			;redraw menus in stack
		ret
		;//////////////////////
		;// DRAW ANALOG CLOCK
		;//////////////////////
	_MODULE_SHOW_ANALOG_CLOCK_20:
		ldiw	Z,BMP_CLOCK_ANALOGICO_2*2		;analog clock image
		call	_RASTER_BMP_GET_WIDTH			;center on screen
		ldiw	X,SCREEN_WIDTH
		sub	XL,Acc
		sbc	XH,AccH
		lsr	XH
		ror	XL
		call	_RASTER_BMP_GET_HEIGHT
		ldiw	Y,SCREEN_HEIGHT
		sub	YL,Acc
		sbc	YH,AccH
		lsr	YH
		ror	YL
		;//////////////////
		;// DRAW BACKGROUND ANALOG CLOCK IMAGE
		;//////////////////
	_MODULE_SHOW_ANALOG_CLOCK_LOOP:
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL		;synchronize with vertical retrace
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL
		call 	_RASTER_SET_BMP_FAST			;show clock set image
		;//////////////////
		;// DRAW CLOCK POINTERS
		;//////////////////
		pushw	X
		pushw	Y
		push	AccT
		;//////////////////
		;// SECOND ARROW
		;//////////////////
		addiw	X,47					;center clock pointer
		addiw	Y,48
		call	_ROT2D_SET_CENTER_POINT			;set clock pointers center
		subiw	Y,42					;start at 12 o clock
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
		ldi	Acc,_ON					;(white)black pointer
		call	_GRAPH_LINE_TO				;draw minute pointer
		;//////////////////
		;// MINUTE ARROW
		;//////////////////
		call	_ROT2D_GET_CENTER_POINT			;set clock pointers center
		subiw	Y,32					;start at 12 o clock
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
		ldi	Acc,_ON					;(white)black pointer
		call	_GRAPH_LINE_TO				;draw minute pointer
		;//////////////////
		;// HOUR ARROW
		;//////////////////
		call	_ROT2D_GET_CENTER_POINT			;set clock pointers center
		subiw	Y,20					;start at 12 o clock
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
		ldi	Acc,_ON					;(white)black pointer
		call	_GRAPH_LINE_TO				;draw hour pointer
		pop	AccT
		popw	Y
		popw	X
		call	_KEYBOARD_READ_FLASH			;read keyboard
		cpi	Acc,_KEY_NONE				;none key pressed?
		lbreq	_MODULE_SHOW_ANALOG_CLOCK_LOOP		;wait wait
		call	fn_MENU_DRAW_BACKGROUND			;redraw background
		call	_MENU_DRAW_STACK			;redraw menus in stack
		ret
