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
;=======> 	      M O D U L E  -  R E S T O R E  S Y S T E M  C O N F I G U R A T I O N		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------

	_MODULE_RESTORE_SYSTEM_CONFIGURATION:
		;/////////////////////////////////
		;//				//
		;//	RESTORE MOVE GRID	//
		;//				//
		;/////////////////////////////////
		call	fn_LOAD_CAPTURE_GRID
		;/////////////////////////////////
		;//				//
		;//	RESTORE MENU ITENS	//
		;//				//
		;/////////////////////////////////
		call	fn_LOAD_MENU
		;/////////////////////////////////
		;//				//
		;//	RESTORE VIDEO SYSTEM	//
		;//				//
		;/////////////////////////////////
		ldiw	Z,_MENU_PORTUGUESE_CONFIGURAR_SISTEMA*2
		ldi	AccH,_MENU_CONFIGURAR_SISTEMA_ITEM_NTSC
		call	_MENU_GET_ITEM_CHECK
		cpi	Acc,_ON
		ldi	Acc,_VIDEO_OUT_NTSC
		breq	_MODULE_RESTORE_SYSTEM_CONFIGURATION_10
		ldi	Acc,_VIDEO_OUT_PAL
	_MODULE_RESTORE_SYSTEM_CONFIGURATION_10:
		call	_VIDEO_OUT_SET_SYSTEM
		;/////////////////////////////////////////
		;//					//
		;//	RESTORE DIGITIZING SENSIBILITY	//
		;//					//
		;/////////////////////////////////////////
		lds	Acc,_MENU_CONFIGURE_CAPTURE_MOVEMENT_SENSIBILITY
		call	_VIDEO_DIGITIZING_SET_SENSIBILITY		;set sensibility
		;/////////////////////////////////////////
		;//					//
		;//	RESTORE DIGITIZING TRACE	//
		;//					//
		;/////////////////////////////////////////
		lds	Acc,_MENU_CONFIGURE_CAPTURE_MOVEMENT_TRACE_TIME_VALUE
		call	_VIDEO_DIGITIZING_SET_DECAY			;set trace value
		;/////////////////////////////////////////
		;//					//
		;// SET MESSAGE DEFINE ITEM TO OFF	//
		;//					//
		;/////////////////////////////////////////
		ldiw	Z,_MENU_PORTUGUESE_MENSAGEM_APRESENTACAO*2
		ldi	AccH,_MENU_MENSAGEM_APRENSENTACAO_ITEM_DEFINIR
		ldi	Acc,_OFF
		call	_MENU_SET_ITEM_CHECK
		ldiw	Z,_MENU_PORTUGUESE_MENSAGEM_IDENTIFICACAO*2
		ldi	AccH,_MENU_MENSAGEM_IDENTIFICACAO_ITEM_DEFINIR
		ldi	Acc,_OFF
		call	_MENU_SET_ITEM_CHECK
		ldiw	Z,_MENU_PORTUGUESE_MENSAGEM_ALARME*2
		ldi	AccH,_MENU_MENSAGEM_ALARME_ITEM_DEFINIR
		ldi	Acc,_OFF
		call	_MENU_SET_ITEM_CHECK
		;/////////////////////////
		;//			//
		;// RESTORE RELAYS SETS	//
		;//			//
		;/////////////////////////
		ldiw	Y,E_RELAY_TIME_ON
		call	_EEPROM_R_WORD
		call	_MOTION_RELAY_SET_TIME_ON
		ldiw	Y,E_RELAY_TIME_OFF
		call	_EEPROM_R_WORD
		call	_MOTION_RELAY_SET_TIME_OFF
		ldiw	Y,E_RELAY_TIME_ACTIVATION
		call	_EEPROM_R_WORD
		stsaw	RELAY_TIME_ACTIVATION
		ret
