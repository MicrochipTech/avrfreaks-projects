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
;=======> 	 M O D U L E  -  M E N U  A C C E S S  A N D  P R O G R A M  E X E C U T I O N		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description:											|
;	| 	This perform all necessary function of MENU, option selection, check options and program|
;	|	execution										|
;	 -----------------------------------------------------------------------------------------------

	.INCLUDE	"MENU_PORTUGUESE.INC"			;load MENU Idiom=Portuguese
	.INCLUDE	"MENU_ENGLISH.INC"			;load MENU Idiom=English
	.INCLUDE	"MENU_SPANISH.INC"			;load MENU Idiom=Spanish

	_MODULE_MENU:
		;//////////////////////////////
		;//
		;// MENU INITIALIZE
		;//
		;//////////////////////////////
		ldiw	Z,_MENU_PORTUGUESE_CONFIGURAR_TELA*2	;Z-->MENU screen type
		ldi	AccH,_MENU_CONFIGURAR_TELA_SOLIDA	;SOLID ITEM
		call	_MENU_GET_ITEM_CHECK			;set
		cpi	Acc,_ON
		brne	_MODULE_MENU_I_10			;no, branch
		ldi	Acc,_VIDEO_MODE_128X96X2N		;set video mode for MENUS with internal sync
		call	_VIDEO_OUT_SET_MODE
		rjmp	_MODULE_MENU_I_20
	_MODULE_MENU_I_10:
		ldi	AccH,_MENU_CONFIGURAR_TELA_TRANSPARENTE	;TRANSPARENT ITEM
		call	_MENU_GET_ITEM_CHECK			;set
		cpi	Acc,_ON
		brne	_MODULE_MENU_I_20			;no, branch
		ldi	Acc,_VIDEO_MODE_128X96X2G		;set video mode for MENUS with external sync
		call	_VIDEO_OUT_SET_MODE
	_MODULE_MENU_I_20:
		call	_MENU_INIT				;Initialize MENU
		ldiw	Z,_MENU_PORTUGUESE_IDIOM*2		;Z-->MENU idioma
		;/////////////////
		;// PORTUGUESE
		;/////////////////
		ldi	AccH,_MENU_IDIOMA_ITEM_PORTUGUES	;set item to PORTUGUESE
		call	_MENU_GET_ITEM_CHECK			;get this setting
		ldiw	X,_MENU_PORTUGUESE_START*2		;set to PORTUGUESE
		cpi	Acc,_ON
		breq	_MODULE_MENU_START			;branch if yes
		;/////////////////
		;// ENGLISH
		;/////////////////
		ldi	AccH,_MENU_IDIOMA_ITEM_INGLES		;set item to ENGLIS
		call	_MENU_GET_ITEM_CHECK			;get this setting
		ldiw	X,_MENU_ENGLISH_START*2			;set to ENGLISH
		cpi	Acc,_ON
		breq	_MODULE_MENU_START			;branch if yes
		;/////////////////
		;// SPANISH
		;/////////////////
		ldi	AccH,_MENU_IDIOMA_ITEM_ESPANHOL		;set item to SPANISH
		call	_MENU_GET_ITEM_CHECK			;get this setting
		ldiw	X,_MENU_SPANISH_START*2			;set to SPANISH
		cpi	Acc,_ON
		breq	_MODULE_MENU_START			;branch if yes
		;
		;
		;reserved space for other languages
		;
		;
	_MODULE_MENU_START:
		movw	ZL,XL					;Z-->MENU entry point
		ldi	AccH,1
		call	_MENU_PUSH
		;//////////////////////////////
		;//
		;// MENU DRAW BACK GROUND AND STACKED MENUS
		;//
		;//////////////////////////////
	_MODULE_MENU_A:
		call	fn_MENU_DRAW_BACKGROUND
		call	_MENU_DRAW_STACK
		;//////////////////////////////
		;//
		;// MENU GET CURRENT AND WAIT SELECTION
		;//
		;//////////////////////////////
	_MODULE_MENU_B:
		call	_MENU_GET_STACK_CURRENT
		call	_MENU_INPUT
		cpi	Acc,_KEY_ENTER				;if pressed ENTER or RIGHT then LINK
		breq	_MODULE_MENU_MENU_LINK
		cpi	Acc,_KEY_RIGHT
		breq	_MODULE_MENU_MENU_LINK
		cpi	Acc,_KEY_LEFT				;key LEFT ?
		brne	_MODULE_MENU_B				;no, wait 
		call	_MENU_GET_STACK_CURRENT			;to see if only START menu present
		cpi	Acc,0
		breq	_MODULE_MENU_B				;yes, wait
		call	_MENU_POP				;else pop menu from stack
		rjmp	_MODULE_MENU_A
		;//////////////////////////////
		;//
		;// MENU LINK TO ANOTHER MENU
		;//
		;//////////////////////////////
	_MODULE_MENU_MENU_LINK:
		call	_MENU_SET_STACK_CURRENT_ITEM		;update ITEM
		call	_MENU_GET_ITEM_MENU_LINK		;get link
		breq	_MODULE_MENU_PROGRAM_LINK		;if empty , branch to program link
		ldi	AccH,1					;else link new menu
		movw	ZL,YL					;Z=MENU
		call	_MENU_PUSH				;push new menu into stack
		rjmp	_MODULE_MENU_B
		;//////////////////////////////
		;//
		;// MENU LINK TO PROGRAM
		;//
		;// AccH Item
		;// Acc	 Pressed key
		;// X	 MENU
		;//////////////////////////////
	_MODULE_MENU_PROGRAM_LINK:
		movw	XL,ZL					;X=MENU
		call	_MENU_GET_ITEM_PROGRAM_LINK		;get program link
		breq	_MODULE_MENU_B				;if empty, continue access this menu
		movw	ZL,YL					;Z=program address
		icall						;EXECUTE PROGRAM
		rjmp	_MODULE_MENU_B				;redraw MENU tree
		;//////////////////////////////
		;//
		;// MENU EXIT
	_MODULE_MENU_EXIT:
		popaw						;dummy call return
		ret						;return to main call
