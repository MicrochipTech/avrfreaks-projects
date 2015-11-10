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
;=======> 		M O D U L E  -  A C T I V A T E  A P R E S E N T A T I O N  M E S S A G E 	|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module activate prosentation message						|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_MESSAGE_PRESENTATION_ACTIVATE:
		movw	ZL,XL						;get MENU
		call	_MENU_GET_ITEM_CHECK				;get old state
		cpi	Acc,_ON						;ON?
		ldi	Acc,_OFF					;assume off
		breq	_MODULE_MESSAGE_PRESENTATION_ACTIVATE_10	;branch if yes
		ldi	Acc,_ON
	_MODULE_MESSAGE_PRESENTATION_ACTIVATE_10:
		call	_MENU_SET_ITEM_CHECK				;set new state
		call	_MENU_DRAW					;redraw this MENU
		call	fn_SAVE_MENU
		ret
	
;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======> 		M O D U L E  -  D E F I N E  A P R E S E N T A T I O N  M E S S A G E 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module put a keyboard on screen to input apresentation message			|
;	 -----------------------------------------------------------------------------------------------

	_MODULE_MESSAGE_PRESENTATION_DEFINE:
		ldiw	X,E_MSG_APRESENTACAO
		call	fn_LOAD_MESSAGE					;load message
		call	fn_KEYBOARD_INPUT_INIT				;initialize keyboard
		call	fn_KEYBOARD_INPUT_EDIT				;edit buffer
		ldiw	Z,_MSG_CONFIRM					;confirm message
		call	fn_INPUT_BOX_CONFIRM				;confirma
		cpi	Acc,_MENU_INPUT_BOX_ANSWER_YES			;YES?
		brne	_MODULE_MESSAGE_PRESENTATION_DEFINE_10		;no, branch
		ldiw	Y,E_MSG_APRESENTACAO
		call	fn_SAVE_MESSAGE					;save message
	_MODULE_MESSAGE_PRESENTATION_DEFINE_10:
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
