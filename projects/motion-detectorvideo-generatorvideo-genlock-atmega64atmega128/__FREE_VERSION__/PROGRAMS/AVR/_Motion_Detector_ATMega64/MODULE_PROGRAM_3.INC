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
;=======> 		M O D U L E  -  P R O G R A M # 1		 		 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module is free to implement a program #1						|
;	 -----------------------------------------------------------------------------------------------

	_MODULE_PROGRAM_3:
		call	_RASTER_GET_CHAR_MODE				;get actual char mode
		push	Acc
		ldiw	Z,_MSG_PROGRAM_3
		call	_MENU_TRANSLATE_MESSAGE
		call	_RASTER_GET_WIDTH				;center on screen horizontaly
		movw	XL,Acc
		call	_RASTER_GET_TEXT_WIDTH
		addiaw	_RASTER_CHAR_WIDTH
		sub	XL,Acc
		sbc	XH,AccH
		lsrw	X
		call	_RASTER_GET_HEIGHT
		movw	YL,Acc
		call	_RASTER_GET_TEXT_HEIGHT				;center on screen verticaly
		addiaw	_RASTER_CHAR_HEIGHT
		sub	YL,Acc
		sbc	YH,AccH
		lsrw	Y
		call	_MENU_MESSAGE_BOX				;Show message box
		call	_KEYBOARD_READ
		pop	Acc						;restore old mode
		call	_RASTER_SET_CHAR_MODE				;set mode
		call	fn_MENU_DRAW_BACKGROUND				;redraw background
		call	_MENU_DRAW_STACK				;redraw menus in stack
		ret
