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
;=======> 			M O D U L E  -  C H E C K  P A S S W O R D				|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	 -----------------------------------------------------------------------------------------------

	_MODULE_CHECK_PASSWORD:		
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
		call	_MODULE_CHECK_PASSWORD_INTEGRITY		;check password integrit
		ldiw	Z,_MSG_DEFAULT_PASSWORD				;set check defalt password
		brcs	_MODULE_CHECK_PASSWORD_10			;branch if no integrity
		ldiw	Z,_MSG_CHECK_PASSWORD				;set check password
	_MODULE_CHECK_PASSWORD_10:
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
	_MODULE_CHECK_PASSWORD_20:
		lds	Acc,PASSWORD_DIGIT_COUNT			;get digit count
		cpi	Acc,8						;8 digits reach?
		breq	_MODULE_CHECK_PASSWORD_EXIT			;yes exit
		call	_KEYBOARD_READ					;read keyboard
		call	_MODULE_SET_PASSWORD_DRAW_KEY			;draw key
		rjmp	_MODULE_CHECK_PASSWORD_20			;get another key
		;@---------------------------------------------------------@
		;| Check if password is correct				   |
		;@---------------------------------------------------------@
	_MODULE_CHECK_PASSWORD_EXIT:
		call	_MODULE_CHECK_PASSWORD_INTEGRITY		;check password integrit again
		ldi	AccT,_ON					;assume defalt
		brcs	PC+2						;yes, branch
		ldi	AccT,_OFF					;normal
		ldiw	Y,E_PASSWORD_MAIN				;Y-->main password
		ldiw	X,PASSWORD_DIGITS				;X-->password digits
		ldiw	Z,ATMEGA64_DEFALT_PASSWORD*2			;Z-->default password
		ldi	AccH,8						;digits count
	_MODULE_CHECK_PASSWORD_EXIT_10:
		call	_EEPROM_R_BYTE					;get from eeprom
		cpi	AccT,_ON					;get default?
		brne	PC+2						;no, branch
		lpm	Acc,Z+						;get from flash
		ld	Temp,X+						;get typed digits
		cp	Acc,Temp					;correct?
		breq	_MODULE_CHECK_PASSWORD_EXIT_20			;yes, branch
		jmp	_BOOT						;else boot
	_MODULE_CHECK_PASSWORD_EXIT_20:
		dec	AccH
		brne	_MODULE_CHECK_PASSWORD_EXIT_10			;until all digits checked
		ret
	;@----------------------------------------------@
	;| DRAW KEY					|
	;| Output: CY=1 if password has no integrity	|
	;@----------------------------------------------@
	_MODULE_CHECK_PASSWORD_INTEGRITY:
		;@---------------------------------------------------------@
		;| First check if Main Password is same as Backup Password |
		;@---------------------------------------------------------@
		ldiw	X,E_PASSWORD_MAIN				;X-->main password
		ldi	Acc,8						;digits count
	_MODULE_CHECK_PASSWORD_INTEGRITY_10:
		push	Acc
		movw	YL,XL						;get main password digit
		call	_EEPROM_R_BYTE
		mov	Temp,Acc
		movw	YL,XL						;get backup password digit
		addiw	Y,8
		call	_EEPROM_R_BYTE
		cp	Acc,Temp					;igual?
		pop	Acc
		brne	_MODULE_CHECK_PASSWORD_INTEGRITY_ERROR		;no, branch
		adiw	XL,1						;next digit
		dec	Acc
		brne	_MODULE_CHECK_PASSWORD_INTEGRITY_10		;until all digits checked
		;@---------------------------------------------------------@
		;| Search for invalid digits				   |
		;@---------------------------------------------------------@
		ldiw	X,E_PASSWORD_MAIN				;Z-->main password
		ldi	AccH,8						;digits count
	_MODULE_CHECK_PASSWORD_INTEGRITY_20:
		movw	YL,XL
		call	_EEPROM_R_BYTE					;get ditit
		mov	Temp,Acc
		cpi	Temp,_KEY_ENTER					;enter key?
		breq	_MODULE_CHECK_PASSWORD_INTEGRITY_30		;yes, branch
		cpi	Temp,_KEY_UP					;up key?
		breq	_MODULE_CHECK_PASSWORD_INTEGRITY_30		;yes, branch
		cpi	Temp,_KEY_DOWN					;down key?
		breq	_MODULE_CHECK_PASSWORD_INTEGRITY_30		;yes, branch
		cpi	Temp,_KEY_RIGHT					;right key?
		breq	_MODULE_CHECK_PASSWORD_INTEGRITY_30		;yes, branch
		cpi	Temp,_KEY_LEFT					;left key?
		breq	_MODULE_CHECK_PASSWORD_INTEGRITY_30		;yes, branch
		rjmp	_MODULE_CHECK_PASSWORD_INTEGRITY_ERROR		;else set an error
	_MODULE_CHECK_PASSWORD_INTEGRITY_30:
		adiw	XL,1
		dec	AccH						;digit count--
		brne	_MODULE_CHECK_PASSWORD_INTEGRITY_20		;until check all
		;@---------------------------------------------------------@
		;| CY=0 Valid Password					   |
		;@---------------------------------------------------------@
		clc
		ret
	_MODULE_CHECK_PASSWORD_INTEGRITY_ERROR:
		;@---------------------------------------------------------@
		;| CY=1 Password has no integrity			   |
		;@---------------------------------------------------------@
		sec							;set no integrity
		ret
