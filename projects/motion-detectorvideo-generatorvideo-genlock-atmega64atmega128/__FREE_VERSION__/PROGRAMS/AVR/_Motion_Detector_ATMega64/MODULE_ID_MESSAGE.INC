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
;=======> 		M O D U L E  -  A C T I V A T E  I D E N T I F I C A T I O N   M E S S A G E 	|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module activate identification message						|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_MESSAGE_IDENTIFICATION_ACTIVATE:
		rjmp	_MODULE_MESSAGE_PRESENTATION_ACTIVATE		;save code
	
;	 -----------------------------------------------------------------------------------------------
;	|												|
;=======> 		M O D U L E  -  D E F I N E  I D E N T I F I C A T I O N  M E S S A G E		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module put a keyboard on screen to input identification message			|
;	 -----------------------------------------------------------------------------------------------

	_MODULE_MESSAGE_IDENTIFICATION_DEFINE:
		ldiw	X,E_MSG_IDENTIFICACAO
		call	fn_LOAD_MESSAGE					;load message
		call	fn_KEYBOARD_INPUT_INIT				;initialize keyboard
		call	fn_KEYBOARD_INPUT_EDIT				;edit buffer
		ldiw	Z,_MSG_CONFIRM					;confirm message
		call	fn_INPUT_BOX_CONFIRM				;confirma
		cpi	Acc,_MENU_INPUT_BOX_ANSWER_YES			;YES?
		brne	_MODULE_MESSAGE_IDENTIFICATION_DEFINE_10	;no, branch
		ldiw	Y,E_MSG_IDENTIFICACAO				;else SAVE
		call	fn_SAVE_MESSAGE					;save message
	_MODULE_MESSAGE_IDENTIFICATION_DEFINE_10:
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
