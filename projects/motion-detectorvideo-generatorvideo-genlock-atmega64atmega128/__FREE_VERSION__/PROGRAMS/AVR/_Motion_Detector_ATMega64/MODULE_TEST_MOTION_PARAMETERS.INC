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
;=======> 			M O D U L E  -  T E S T  M O T I O N  P A R A M E T E R S		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module teste setting parameters for Motion detection	 			|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_TEST_MOTION_PARAMETERS:
		call	_VIDEO_OUT_GET_MODE			;get actual char mode
		push	Acc					;save it
		ldi	Acc,_VIDEO_MODE_128X96X2G
		call	_VIDEO_OUT_SET_MODE			;set new MODE
		call	_VIDEO_OUT_CLS
	_MODULE_TEST_MOTION_PARAMETERS_10:
;		call	_KEYBOARD_READ_FLASH			;wait key release
;		cpi	Acc,_KEY_NONE				;none key, wait
;		breq	_MODULE_TEST_MOTION_PARAMETERS_10
		call	_KEYBOARD_DEBOUNCE			;wait a debounce time
		ldiw	Z,_MENU_PORTUGUESE_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO_TIPO*2	;get MENU entry point
		call	_MENU_GET_ITEM_CHECK_FIRST		;get first CHECKED ITEM
		;///////////////////////////
		;//
		;// MAIN TEST MOTION LOOP
		;//
		;//////////////////////////	
	_MODULE_TEST_MOTION_PARAMETERS_LOOP:
		push	AccH					;save RASTER TYPE
		call	_MODULE_SHOW_MOTION_DETECTION		;show motion detector 
		pop	AccH					;restore RASTER TYPE
		call	_KEYBOARD_READ_FLASH			;read keyboard
		cpi	Acc,_KEY_NONE				;none key pressed ?
		breq	_MODULE_TEST_MOTION_PARAMETERS_LOOP	;yes, continue
		call	_KEYBOARD_DEBOUNCE			;wait a debounce time
		pop	Acc					;restore old mode
		call	_VIDEO_OUT_SET_MODE			;set mode
		call	fn_MENU_DRAW_BACKGROUND			;redraw background
		call	_MENU_DRAW_STACK			;redraw menus in stack
		ret



