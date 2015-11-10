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
;=======> 	      M O D U L E  -  D E F I N E  M O V E M E N T  T R A C E   T I M E	 		|
;	| 												|
;	 -----------------------------------------------------------------------------------------------
;	| Description: Define movement trace time							|											|
;	 -----------------------------------------------------------------------------------------------
	_MODULE_SET_MOVEMENT_TRACE_TIME:
		cpi	AccH,_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_SEM	;sem
		brne	_MODULE_SET_MOVEMENT_TRACE_TIME_10
		ldi	Acc,0
	_MODULE_SET_MOVEMENT_TRACE_TIME_10:
		cpi	AccH,_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_1P2	;1.2s
		brne	_MODULE_SET_MOVEMENT_TRACE_TIME_20
		ldi	Acc,64
	_MODULE_SET_MOVEMENT_TRACE_TIME_20:
		cpi	AccH,_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_2P4	;2.4s
		brne	_MODULE_SET_MOVEMENT_TRACE_TIME_30
		ldi	Acc,32
	_MODULE_SET_MOVEMENT_TRACE_TIME_30:
		cpi	AccH,_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_4P8	;4.8s
		brne	_MODULE_SET_MOVEMENT_TRACE_TIME_40
		ldi	Acc,16
	_MODULE_SET_MOVEMENT_TRACE_TIME_40:
		cpi	AccH,_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_9P6	;9.6s
		brne	_MODULE_SET_MOVEMENT_TRACE_TIME_50
		ldi	Acc,8
	_MODULE_SET_MOVEMENT_TRACE_TIME_50:
		sts	_MENU_CONFIGURE_CAPTURE_MOVEMENT_TRACE_TIME_VALUE,Acc			;set decay value
		call	_VIDEO_DIGITIZING_SET_DECAY
		jmp	_MODULE_SET_ONE_ITEM_MENU
