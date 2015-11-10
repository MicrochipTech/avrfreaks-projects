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
;=======> 		M O D U L E  -  S E T  M E N U  S C R E E N  M O D E 				|	 	 		 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This Module define MENU screen mode SOLID or TRANSPARENT				|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_SCREEN:
		cpi	AccH,_MENU_CONFIGURAR_TELA_SOLIDA	;item TELA SOLIDA ?
		brne	_MODULE_SCREEN_10			;no, branch
		ldi	Acc,_VIDEO_MODE_128X96X2N		;set SOLID SCREEN MODE (no genlock)
		call	_VIDEO_OUT_SET_MODE
		rjmp	_MODULE_SCREEN_20
	_MODULE_SCREEN_10:	
		cpi	AccH,_MENU_CONFIGURAR_TELA_TRANSPARENTE	;item TELA TRANSPARENTE ?
		brne	_MODULE_SCREEN_20			;no, branch
		ldi	Acc,_VIDEO_MODE_128X96X2G		;set TRANSPARENT SCREEN MODE (with genlock)
		call	_VIDEO_OUT_SET_MODE
	_MODULE_SCREEN_20:	
		call	_MODULE_SET_ONE_ITEM_MENU		;set one item
		call	fn_MENU_DRAW_BACKGROUND			;redraw background
		call	_MENU_DRAW_STACK			;redraw menus in stack
		ret

