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
;=======> 			M O D U L E  -  S E T  P A S S W O R D					|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	 -----------------------------------------------------------------------------------------------
	
	.INCLUDE	"BMPS\BMP_INPUT_BOX_CANCEL_INVERTED.INC"	;LEFT KEY
	.INCLUDE	"BMPS\BMP_INPUT_BOX_YES_INVERTED.INC"		;UP KEY
	.INCLUDE	"BMPS\BMP_INPUT_BOX_NO_INVERTED.INC"		;DOWN KEY
	.INCLUDE	"BMPS\BMP_INPUT_BOX_RIGHT.INC"			;RIGHT KEY
	.INCLUDE	"BMPS\BMP_INPUT_BOX_ENTER.INC"			;ENTER KEY

	_MODULE_SET_PASSWORD:		
		ldi	Acc,0						;clear digit count
		sts	PASSWORD_DIGIT_COUNT,Acc
		ldiw	X,PASSWORD_DIGITS				;set all to enter
		ldi	Acc,_KEY_ENTER
		st	X+,Acc
		st	X+,Acc
		st	X+,Acc
		st	X+,Acc
		st	X+,Acc
		st	X+,Acc
		st	X+,Acc
		st	X+,Acc
		ldiw	Z,_MSG_SET_PASSWORD				;set password box message
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_WIDTH				;center message
		movw	XL,Acc
		call	_RASTER_GET_TEXT_WIDTH
		addiaw	_RASTER_CHAR_WIDTH
		sub	XL,Acc
		sbc	XH,AccH
		lsrw	X
		ldiw	Y,16
		call	_MENU_MESSAGE_BOX				;show message
		call	_MODULE_SET_PASSWORD_DRAW_ASTERISK		;draw asterisks
		ldi	Acc,_KEY_ENTER
		call	_KEYBOARD_READ_AND_WAIT
		ldiw	X,PASSWORD_DIGITS				;X--> DIGITS
	_MODULE_SET_PASSWORD_10:
		lds	Acc,PASSWORD_DIGIT_COUNT			;get digit count
		cpi	Acc,8						;8 digits reach?
		breq	_MODULE_SET_PASSWORD_EXIT			;yes exit
		call	_KEYBOARD_READ					;read keyboard
		call	_MODULE_SET_PASSWORD_DRAW_KEY			;draw key
		rjmp	_MODULE_SET_PASSWORD_10				;get another key
	_MODULE_SET_PASSWORD_EXIT:
		ldiw	Z,_MSG_CONFIRM					;confirm clear?
		call	fn_INPUT_BOX_CONFIRM				;input box confirm msg
		cpi	Acc,_MENU_INPUT_BOX_ANSWER_YES			;YES?
		brne	_MODULE_SET_PASSWORD_EXIT_10			;no, branch
		;+-----------------------
		;| SET NEW PASSWORD
		;+-----------------------
		ldiw	X,PASSWORD_DIGITS				;set it in eeprom main
		ldiw	Y,E_PASSWORD_MAIN
		ldiaw	8
		call	_EEPROM_W_STR_SRAM
		ldiw	X,PASSWORD_DIGITS				;set it in eeprom backup
		ldiw	Y,E_PASSWORD_BACKUP
		call	_EEPROM_W_STR_SRAM
	_MODULE_SET_PASSWORD_EXIT_10:
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
	;@----------------------@
	;| DRAW KEY		|
	;| Input: Acc key code	|
	;@----------------------@
	_MODULE_SET_PASSWORD_DRAW_KEY:
		st	X+,Acc						;store new character
		pushw	X
		push	Acc						;save key code
		call	_RASTER_GET_WIDTH				;get width
		movw	XL,Acc
		subiw	X,8*8						;minus eight digits
		lsrw	X
		ldiw	Y,44						;middle screen height
		lds	Acc,PASSWORD_DIGIT_COUNT
		lsl	Acc						;acc*8
		lsl	Acc
		lsl	Acc
		add	XL,Acc
		ldi	Acc,0
		adc	XH,Acc
		pop	Acc
		ldiw	Z,BMP_INPUT_BOX_CANCEL_INVERTED*2		;left draw
		cpi	Acc,_KEY_LEFT					;left key?
		breq	_MODULE_SET_PASSWORD_DRAW_KEY_10		;yes, branch
		ldiw	Z,BMP_INPUT_BOX_YES_INVERTED*2			;up draw
		cpi	Acc,_KEY_UP					;up key?
		breq	_MODULE_SET_PASSWORD_DRAW_KEY_10		;yes, branch
		ldiw	Z,BMP_INPUT_BOX_NO_INVERTED*2			;down draw
		cpi	Acc,_KEY_DOWN					;left key?
		breq	_MODULE_SET_PASSWORD_DRAW_KEY_10		;yes, branch
		ldiw	Z,BMP_INPUT_BOX_RIGHT*2				;right draw
		cpi	Acc,_KEY_RIGHT					;right key?
		breq	_MODULE_SET_PASSWORD_DRAW_KEY_10		;yes, branch
		ldiw	Z,BMP_INPUT_BOX_ENTER*2				;enter draw
	_MODULE_SET_PASSWORD_DRAW_KEY_10:
		pushw	X
		ldi	Acc,' '
		call	_RASTER_SET_CHAR				;clear before
		popw	X
		call	_RASTER_SET_BMP_FAST				;DRAW char
		lds	Acc,PASSWORD_DIGIT_COUNT			;char count++
		inc	Acc
		sts	PASSWORD_DIGIT_COUNT,Acc
		popw	X
		ret
	;@----------------------@
	;| DRAW ASTERISKS	|
	;@----------------------@
	_MODULE_SET_PASSWORD_DRAW_ASTERISK:
		call	_RASTER_GET_WIDTH				;get width
		movw	XL,Acc
		subiw	X,8*8						;minus eight digits
		lsrw	X
		ldiw	Y,44						;middle screen height
		ldi	Acc,8						;8 chars
	_MODULE_SET_PASSWORD_DRAW_ASTERISK_10:
		push	Acc
		ldi	Acc,'*'						;ASCII '*'
		call	_RASTER_SET_CHAR
		addiw	X,2
		pop	Acc
		dec	Acc
		brne	_MODULE_SET_PASSWORD_DRAW_ASTERISK_10
		ret
