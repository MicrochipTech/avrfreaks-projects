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
;=======> 		M O D U L E  -  D E F I N E  I P	 	 		 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module define IP of ATMEGA64							|
;	 -----------------------------------------------------------------------------------------------

	_MODULE_DEFINE_IP:
		call	_RASTER_GET_CHAR_MODE				;get actual char mode
		push	Acc						;save it
		lds	Acc,_MENU_CONFIGURE_COMMUNICATION_IP+0		;SAVE IP ACTUAL
		lds	AccH,_MENU_CONFIGURE_COMMUNICATION_IP+1
		pushaw
		lds	Acc,_MENU_CONFIGURE_COMMUNICATION_IP+2
		lds	AccH,_MENU_CONFIGURE_COMMUNICATION_IP+3
		pushaw
		ldi	Acc,_RASTER_CHAR_MODE_INVERTED			;set mode inverted
		call	_RASTER_SET_CHAR_MODE				;set
		ldiw	Z,_MSG_DEFINE_IP				;define ip msg
		call	_MENU_TRANSLATE_MESSAGE				;translate
		call	_RASTER_GET_TEXT_WIDTH				;get width and center msg on screen at top
		ldiw	X,SCREEN_WIDTH
		addiaw	8
		sub	XL,Acc
		sbc	XH,AccH
		lsr	XH
		ror	XL
		call	_RASTER_GET_TEXT_HEIGHT
		ldiw	Y,SCREEN_HEIGHT
		addiaw	8
		sub	YL,Acc
		sbc	YH,AccH
		lsr	YH
		ror	YL
		;//////////////////////////////
		;/// DRAW IP PANEL
		;//////////////////////////////
		ldi	AccT,1						;IP field index =1
	_MODULE_DEFINE_IP_LOOP:
		call	_VIDEO_OUT_SYNC_WITH_VERTICAL
		call	_MENU_MESSAGE_BOX				;show
		;//////////////////////////////
		;/// DRAW IP NUMBER
		;//////////////////////////////	
		pushw	X
		pushw	Y
		pushw	Z
		call	_RASTER_GET_TEXT_WIDTH
		addiaw	8
		lsr	AccH
		ror	Acc
		subiaw	((_RASTER_CHAR_WIDTH*4)*4)/2
		add	XL,Acc
		adc	XH,AccH
		call	_RASTER_GET_TEXT_HEIGHT
		lsr	AccH
		ror	Acc
		add	YL,Acc
		adc	YH,AccH
		lds	Acc,_MENU_CONFIGURE_COMMUNICATION_IP+0		;1st IP field
		call	_UBTOSUS
		adiw	ZL,1
		call	_RASTER_SET_TEXT_OUT_SRAM
		ldi	Acc,'.'
		call	_RASTER_SET_CHAR
		lds	Acc,_MENU_CONFIGURE_COMMUNICATION_IP+1		;2nd IP field
		call	_UBTOSUS
		adiw	ZL,1
		call	_RASTER_SET_TEXT_OUT_SRAM
		ldi	Acc,'.'
		call	_RASTER_SET_CHAR
		lds	Acc,_MENU_CONFIGURE_COMMUNICATION_IP+2		;3rd IP field
		call	_UBTOSUS
		adiw	ZL,1
		call	_RASTER_SET_TEXT_OUT_SRAM
		ldi	Acc,'.'
		call	_RASTER_SET_CHAR
		lds	Acc,_MENU_CONFIGURE_COMMUNICATION_IP+3		;4th IP field
		call	_UBTOSUS
		adiw	ZL,1
		call	_RASTER_SET_TEXT_OUT_SRAM
		addiw	Y,10
		mov	Acc,AccT					;draw selection bar
		dec	Acc						;adjust 0..N
		ldi	AccH,(_RASTER_CHAR_WIDTH*4)			;space between IP fields
		mul	Acc,AccH
		subiw	X,(_RASTER_CHAR_WIDTH*4)*4-6
		add	XL,r0
		adc	XH,r1
		ldi	XH,17
		ldi	YH,2
		ldi	Temp,_OFF
		call	_RASTER_SET_BOX_FULL_FAST			;draw selection bar
		popw	Z
		popw	Y
		popw	X
		;//////////////////////////////
		;/// READ KEYBOARD CONTROLS
		;//////////////////////////////
		call	_KEYBOARD_READ
		cpi	Acc,_KEY_UP					;UP KEY ?
		breq	_MODULE_DEFINE_IP_KEY_UP			;yes, branch
		cpi	Acc,_KEY_DOWN					;DOWN KEY ?
		breq	_MODULE_DEFINE_IP_KEY_DOWN			;yes, branch
		cpi	Acc,_KEY_LEFT					;LEFT KEY ?
		breq	_MODULE_DEFINE_IP_KEY_LEFT			;yes, same as key none
		cpi	Acc,_KEY_RIGHT					;RIGHT KEY ?
		breq	_MODULE_DEFINE_IP_KEY_RIGHT			;yes, same as key none
		cpi	Acc,_KEY_ENTER					;ENTER KEY ?
		breq	_MODULE_DEFINE_IP_KEY_ENTER			;yes, same as key none
		;//////////////////////////////
		;// PROCESS KEY UP
		;//////////////////////////////
	_MODULE_DEFINE_IP_KEY_UP:
		pushw	Z
		ldiw	Z,_MENU_CONFIGURE_COMMUNICATION_IP		;increment IP(FIELD)
		mov	Acc,AccT					;get field
		dec	Acc						;adjust 0..N
		add	ZL,Acc
		ldi	Acc,0
		adc	ZH,Acc
		ld	Acc,Z
		inc	Acc
		st	Z,Acc
		popw	Z
		rjmp	_MODULE_DEFINE_IP_LOOP			;loop AGAIN
		;//////////////////////////////
		;// PROCESS KEY DOWN
		;//////////////////////////////
	_MODULE_DEFINE_IP_KEY_DOWN:
		pushw	Z
		ldiw	Z,_MENU_CONFIGURE_COMMUNICATION_IP		;decrement IP(FIELD)
		mov	Acc,AccT					;get field
		dec	Acc						;adjust 0..N
		add	ZL,Acc
		ldi	Acc,0
		adc	ZH,Acc
		ld	Acc,Z
		dec	Acc
		st	Z,Acc
		popw	Z
		rjmp	_MODULE_DEFINE_IP_LOOP			;loop AGAIN
		;//////////////////////////////
		;// PROCESS KEY LEFT
		;//////////////////////////////
	_MODULE_DEFINE_IP_KEY_LEFT:
		cpi	AccT,1					;at 1st field ?
		brne	_MODULE_DEFINE_IP_KEY_LEFT_10		;no, branch
		ldi	AccT,4					;else set to 4th field
		rjmp	_MODULE_DEFINE_IP_LOOP			;loop AGAIN
	_MODULE_DEFINE_IP_KEY_LEFT_10:
		dec	AccT
		rjmp	_MODULE_DEFINE_IP_LOOP			;loop AGAIN
		;//////////////////////////////
		;// PROCESS KEY RIGHT
		;//////////////////////////////
	_MODULE_DEFINE_IP_KEY_RIGHT:
		cpi	AccT,4					;at 4th field ?
		brne	_MODULE_DEFINE_IP_KEY_RIGHT_10		;no, branch
		ldi	AccT,1					;else set to 4th field
		rjmp	_MODULE_DEFINE_IP_LOOP			;loop AGAIN
	_MODULE_DEFINE_IP_KEY_RIGHT_10:
		inc	AccT
		rjmp	_MODULE_DEFINE_IP_LOOP			;loop AGAIN
		;//////////////////////////////
		;// PROCESS KEY ENTER
		;//////////////////////////////
	_MODULE_DEFINE_IP_KEY_ENTER:
		popw	Y					;restore IP
		popw	X
		lds	Acc,_MENU_CONFIGURE_COMMUNICATION_IP+0	;see if was changes
		cp	Acc,XL
		lds	Acc,_MENU_CONFIGURE_COMMUNICATION_IP+1
		cpc	Acc,XH
		lds	Acc,_MENU_CONFIGURE_COMMUNICATION_IP+2
		cpc	Acc,YL
		lds	Acc,_MENU_CONFIGURE_COMMUNICATION_IP+3
		cpc	Acc,YH
		breq	_MODULE_DEFINE_IP_EXIT			;exit if no changes
		ldiw	Z,_MSG_CONFIRM				;confirm msg?
		call	fn_INPUT_BOX_CONFIRM			;confirma
		cpi	Acc,_MENU_INPUT_BOX_ANSWER_YES		;YES?
		brne	_MODULE_DEFINE_IP_EXIT			;no, branch
		call	fn_SAVE_MENU				;else save IP
	_MODULE_DEFINE_IP_EXIT:
		pop	Acc					;restore old mode
		call	_RASTER_SET_CHAR_MODE			;set mode
		call	fn_MENU_DRAW_BACKGROUND			;redraw background
		call	_MENU_DRAW_STACK			;redraw menus in stack
		ret
		
