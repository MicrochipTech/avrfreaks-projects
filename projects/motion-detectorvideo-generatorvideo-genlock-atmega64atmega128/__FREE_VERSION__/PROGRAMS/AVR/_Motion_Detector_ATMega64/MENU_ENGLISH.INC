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
;					___  ___ _____ _   _ _   _ 
;					|  \/  ||  ___| \ | | | | |
;					| .  . || |__ |  \| | | | |
;					| |\/| ||  __|| . ` | | | |
;					| |  | || |___| |\  | |_| |
;					\_|  |_/\____/\_| \_/\___/ 
;                           
;					 _____            _ _     _     
;					|  ___|          | (_)   | |    
;					| |__ _ __   __ _| |_ ___| |__  
;					|  __| '_ \ / _` | | / __| '_ \ 
;					| |__| | | | (_| | | \__ \ | | |
;					\____/_| |_|\__, |_|_|___/_| |_|
;					             __/ |              
;					            |___/               


		;================
		;=		=
		;= MENU INICIAR =
		;=		=
		;===================================================================================================
	_MENU_ENGLISH_START:
		;Options
		.DB	"Start",0
		;Coordinates
		.DW	4,4
		;Menu check memory
		.DW	0;_MENU_START_CHECK
		;Menu Links
		.DW	_MENU_ENGLISH_PRINCIPAL
		;Menu Programs Links
		.DW	0
		;====================
		;=		    =
		;= MENU PRINCIPAL   =
		;=	 	    =
		;====================================================================================================
	_MENU_ENGLISH_PRINCIPAL:
		;Options
		.DB	"Configuration",_nc,_cr,_lf
		.DB	"Messages",_cr,_lf
		.DB	"Utilities",_nc,_cr,_lf
		.DB	"Language",_cr,_lf
		.DB	"Password",_cr,_lf
		.DB	"Programs",_cr,_lf
                .DB     "Reset(Cold)",_nc,_cr,_lf
		.DB	"Exit",_cr,0
		;Coordinates
		.DW	4+4,4+16-1
		;Menu check memory
		.DW	0;_MENU_MAIN_CHECK
		;Menu Links
		.DW	_MENU_ENGLISH_CONFIGURAR
		.DW	_MENU_ENGLISH_MENSAGENS
		.DW	_MENU_ENGLISH_UTILITARIOS
		.DW	_MENU_ENGLISH_IDIOM
		.DW	0	;no link 
		.DW	_MENU_PROGRAMS_EXECUTE
		.DW	0	;no link
                .DW     0       ;no link
		;Menu Programs Links
		.DW	0
		.DW	0
		.DW	0
		.DW	0
		.DW	_MODULE_SET_PASSWORD
		.DW	0
                .DW     _MODULE_RESET
		.DW	_MODULE_MENU_EXIT
                ;====================
		;=		    =
		;= MENU PROGRAMS    =
		;=	 	    =
		;====================================================================================================
	_MENU_PROGRAMS_EXECUTE:
		;Options
		.DB	"Program - 1",_nc,_cr,_lf
		.DB	"Program - 2",_nc,_cr,_lf
		.DB	"Program - 3",_nc,_cr,_lf
		.DB	"Program - 4",0
		;Coordinates
		.DW	12,21
		;Menu check memory
		.DW	0
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_PROGRAM_1
		.DW	_MODULE_PROGRAM_2
		.DW	_MODULE_PROGRAM_3
		.DW	_MODULE_PROGRAM_4
		;====================
		;=		    =
		;= MENU CONFIGURAR  =
		;=	 	    =
		;====================================================================================================
	_MENU_ENGLISH_CONFIGURAR:
		;Options
		.DB	"Video system",_cr,_lf
		.DB	"Move detection",_cr,_lf
		.DB	"Relay activation",_cr,_lf
		.DB	"Communication",_nc,_cr,_lf
		.DB	"MENU Screen",_nc,_cr,_lf
		.DB	"Clock",0
		;Coordinates
		.DW	12,31
		;Menu check memory
		.DW	0	;no link
		;Menu Links
		.DW	_MENU_ENGLISH_CONFIGURAR_SISTEMA
		.DW	_MENU_ENGLISH_CONFIGURAR_CAPTURA_MOVIMENTO
		.DW	_MENU_ENGLISH_CONFIGURAR_ATIVACAO
		.DW	_MENU_ENGLISH_CONFIGURAR_COMUNICACAO
		.DW	_MENU_ENGLISH_CONFIGURAR_TELA
		.DW	0	;no link
		;Menu Programs Links
		.DW	0
		.DW	0
		.DW	0
		.DW	0
		.DW 	0
		.DW	_MODULE_CLOCK_SET
		;================================
		;=		    		=
		;= MENU CONFIGURAR SISTEMA 	=
		;=	 	    		=
		;================================
	_MENU_ENGLISH_CONFIGURAR_SISTEMA:
		;itens numbers
;		.EQU	_MENU_CONFIGURAR_SISTEMA_ITEM_NTSC = 1
;		.EQU	_MENU_CONFIGURAR_SISTEMA_ITEM_PAL  = 2
		;Options
		.DB	"NTSC",_cr,_lf
		.DB	"PAL",0
		;Coordinates
		.DW	4+4+4+4+4,4+16+12+3+7
		;Menu check memory
		.DW	_MENU_CONFIGURE_SYSTEM_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_SET_SYSTEM_NTSC	
		.DW	_MODULE_SET_SYSTEM_PAL
		;========================================
		;=		    			=
		;= MENU CONFIGURAR CAPTURA DE MOVIMENTO	=
		;=	 	    			=
		;========================================
	_MENU_ENGLISH_CONFIGURAR_CAPTURA_MOVIMENTO:
		;Options
		.DB	"Grid",_cr,_lf
		.DB	"Sensibility",_nc,_cr,_lf
		.DB	"Trace",_nc,_cr,_lf
		.DB	"Test",_nc,0
		;Coordinates
		.DW	16,50
		;Menu check memory
		.DW	0	;no check
		;Menu Links
		.DW	_MENU_ENGLISH_CONFIGURAR_CAPTURA_MOVIMENTO_GRADE
		.DW	0	;no link
		.DW	_MENU_ENGLISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO
		.DW	0	;no link
		;Menu Programs Links
		.DW	0	;no link
		.DW	_MODULE_SET_SENSIBILITY
		.DW	0	;no link
		.DW	_MODULE_TEST_MOTION_PARAMETERS
		;================================================
		;=		    				=
		;= MENU CONFIGURAR CAPTURA DE MOVIMENTO	GRADE 	=
		;=	 	    				=
		;================================================
	_MENU_ENGLISH_CONFIGURAR_CAPTURA_MOVIMENTO_GRADE:
		;Options
		.DB	"Define",_cr,_lf
		.DB	"Clear",0
		;Coordinates
		.DW	20,62
		;Menu check memory
		.DW	0	;no check
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_CAPTURE_GRID
		.DW	_MODULE_CLEAR_GRID
		;========================================================
		;=		    					=
		;= MENU CONFIGURAR CAPTURA DE MOVIMENTO RASTRO		=
		;=	 	    					=
		;========================================================
	_MENU_ENGLISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO:
		;itens numbers
		;Options
		.DB	"Type",_cr,_lf
		.DB	"Time",_nc,0
		;Coordinates
		.DW	50,70
		;Menu check memory
		.DW	0	;no link
		;Menu Links
		.DW	_MENU_ENGLISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO_TIPO
		.DW	_MENU_ENGLISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO_TEMPO
		;Menu Programs Links
		.DW	0	;no link
		.DW	0	;no link
		;========================================================
		;=		    					=
		;= MENU CONFIGURAR CAPTURA DE MOVIMENTO RASTRO TIPO	=
		;=	 	    					=
		;========================================================
	_MENU_ENGLISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO_TIPO:
		;itens numbers
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_BOX 	= 1
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_BOX_FULL = 2
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_CONTORNO = 3
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_CENTRO	= 4
		;Options
		.DB	"Box",_nc,_cr,_lf
		.DB	"Box full",_cr,_lf
		.DB	"Contour",_nc,_cr,_lf
		.DB	"Cross",_nc,_cr,_lf
		.DB	"XY Graph",_nc,0
		;Coordinates
		.DW	46,25
		;Menu check memory
		.DW	_MENU_CONFIGURE_CAPTURE_MOVEMENT_TRACE_TYPE_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_SET_MOVEMENT_TRACE_TYPE
		.DW	_MODULE_SET_MOVEMENT_TRACE_TYPE
		.DW	_MODULE_SET_MOVEMENT_TRACE_TYPE
		.DW	_MODULE_SET_MOVEMENT_TRACE_TYPE
		.DW	_MODULE_SET_MOVEMENT_TRACE_TYPE
		;========================================================
		;=		    					=
		;= MENU CONFIGURAR CAPTURA DE MOVIMENTO RASTRO		=
		;=	 	    					=
		;========================================================
	_MENU_ENGLISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO_TEMPO:
		;itens numbers
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_SEM = 1
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_1P2 = 2
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_2P4 = 3
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_4P8 = 4
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_9P6 = 5
		;Options
		.DB	"0.0s",_cr,_lf
		.DB	"1.2s",_cr,_lf
		.DB	"2.4s",_cr,_lf
		.DB	"4.8s",_cr,_lf
		.DB	"9.6s",_nc,0
		;Coordinates
		.DW	79,46
		;Menu check memory
		.DW	_MENU_CONFIGURE_CAPTURE_MOVEMENT_TRACE_TIME_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_SET_MOVEMENT_TRACE_TIME
		.DW	_MODULE_SET_MOVEMENT_TRACE_TIME
		.DW	_MODULE_SET_MOVEMENT_TRACE_TIME
		.DW	_MODULE_SET_MOVEMENT_TRACE_TIME
		.DW	_MODULE_SET_MOVEMENT_TRACE_TIME
		;================================
		;=		    		=
		;= MENU CONFIGURAR ATIVACAO 	=
		;=	 	    		=
		;================================
	_MENU_ENGLISH_CONFIGURAR_ATIVACAO:
		;Options
		.DB	"By sensibility",_cr,_lf
		.DB	"By time ",_cr,_lf
		.DB	"Configuration",0
		;Coordinates
		.DW	12,59
		;Menu check memory
		.DW	_MENU_CONFIGURE_ACTIVATION_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	_MENU_ENGLISH_CONFIGURAR_ATIVACAO_CONFIGURACAO
		;Menu Programs Links
		.DW	_MODULE_ACTIVATION_MODE
		.DW	_MODULE_ACTIVATION_MODE
		.DW	0	;no link
		;================================================
		;=		    				=
		;= MENU CONFIGURAR ATIVACAO CONFIGURACAO	=
		;=	 	    				=
		;================================================
	_MENU_ENGLISH_CONFIGURAR_ATIVACAO_CONFIGURACAO:
		;Options
		.DB	"Time On",_nc,_cr,_lf
		.DB	"Time Off",_cr,_lf
		.DB	"Time Activation",_nc,_cr,_lf
		.DB	"Repeats",0
		;Coordinates
		.DW	4,38
		;Menu check memory
		.DW	0
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	_MENU_ENGLISH_CONFIGURAR_ATIVACAO_CONFIGURACAO_REPETICAO
		;Menu Programs Links
		.DW	_MODULE_DEFINE_RELAY_TIME_ON
		.DW	_MODULE_DEFINE_RELAY_TIME_OFF
		.DW	_MODULE_DEFINE_RELAY_TIME_ACTIVATION
		.DW	0	;no link
		;========================================================
		;=		    					=
		;= MENU CONFIGURAR ATIVACAO CONFIGURACAO REPETICAO	=
		;=	 	    					=
		;========================================================
	_MENU_ENGLISH_CONFIGURAR_ATIVACAO_CONFIGURACAO_REPETICAO:
		;Options
		.DB	"  1 x",_nc,_cr,_lf
		.DB	"  2 x",_nc,_cr,_lf
		.DB	"  3 x",_nc,_cr,_lf
		.DB	"  4 x",_nc,_cr,_lf
		.DB	"  5 x",_nc,_cr,_lf
		.DB	"  6 x",_nc,_cr,_lf
		.DB	"  7 x",_nc,_cr,_lf
		.DB	"  8 x",_nc,_cr,_lf
		.DB	"  9 x",_nc,_cr,_lf
		.DB	" 10 x",_nc,_cr,_lf
		.DB	" Free",0
		;Coordinates
		.DW	76,2
		;Menu check memory
		.DW	_MENU_CONFIGURE_ACTIVATION_CONFIG_RELAY_REPEATS_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link		
		.DW	0	;no link		
		;Menu Programs Links
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		.DW	_MODULE_ACTIVATION_RELAY_REPEATS
		;================================
		;=		    		=
		;= MENU CONFIGURAR COMUNICACAO 	=
		;=	 	    		=
		;================================
	_MENU_ENGLISH_CONFIGURAR_COMUNICACAO:
		;Options
		.DB	"Enable",_cr,_lf
		.DB	"Define IP",0
		;Coordinates
		.DW	20,66
		;Menu check memory
		.DW	_MENU_CONFIGURE_COMMUNICATION_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_COMMUNICATION_MODE
		.DW	_MODULE_DEFINE_IP
		;================================
		;=		    		=
		;= MENU CONFIGURAR TELA		=
		;=	 	    		=
		;================================
	_MENU_ENGLISH_CONFIGURAR_TELA:
		;itens numbers
;		.EQU	_MENU_CONFIGURAR_TELA_SOLIDA = 1
;		.EQU	_MENU_CONFIGURAR_TELA_TRANSPARENTE = 2
		;Options
		.DB	"Solid",_nc,_cr,_lf
		.DB	"Transparent",0
		;Coordinates
		.DW	20,74
		;Menu check memory
		.DW	_MENU_CONFIGURE_SCREEN_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_SCREEN
		.DW	_MODULE_SCREEN
		;================================
		;=		    		=
		;= MENU CONFIGURAR MENSAGENS 	=
		;=	 	    		=
		;============================================================================================
	_MENU_ENGLISH_MENSAGENS:
		;Options
		.DB	"Introduction",_cr,_lf
		.DB	"Identification",_cr,_lf
		.DB	"Alarm",0
		;Coordinates
		.DW	16,38
		;Menu check memory
		.DW	0	;no link
		;Menu Links
		.DW	_MENU_ENGLISH_MENSAGEM_APRESENTACAO
		.DW	_MENU_ENGLISH_MENSAGEM_IDENTIFICACAO
		.DW	_MENU_ENGLISH_MENSAGEM_ALARME
		;Menu Programs Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0
		;================================================
		;=		    				=
		;= MENU CONFIGURAR MENSAGEN APRESENTACAO 	=
		;=	 	    				=
		;================================================
	_MENU_ENGLISH_MENSAGEM_APRESENTACAO:
		;itens
;		.EQU	_MENU_MENSAGEM_APRENSENTACAO_ITEM_ATIVAR 	= 1
;		.EQU	_MENU_MENSAGEM_APRENSENTACAO_ITEM_DEFINIR	= 2
		;Options
		.DB	"Enable",_cr,_lf
		.DB	"Define",_cr,0
		;Coordinates
		.DW	24,50
		;Menu check memory
		.DW	_MENU_MESSAGE_APRESENTATION_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_MESSAGE_PRESENTATION_ACTIVATE
		.DW	_MODULE_MESSAGE_PRESENTATION_DEFINE
		;================================================
		;=		    				=
		;= MENU CONFIGURAR MENSAGEN IDENTIFICACAO 	=
		;=	 	    				=
		;================================================
	_MENU_ENGLISH_MENSAGEM_IDENTIFICACAO:
		;itens
;		.EQU	_MENU_MENSAGEM_IDENTIFICACAO_ITEM_ATIVAR 	= 1
;		.EQU	_MENU_MENSAGEM_IDENTIFICACAO_ITEM_DEFINIR	= 2
		;Options
		.DB	"Enable",_cr,_lf
		.DB	"Define",_cr,0
		;Coordinates
		.DW	24,58
		;Menu check memory
		.DW	_MENU_MESSAGE_IDENTIFICATION_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_MESSAGE_IDENTIFICATION_ACTIVATE
		.DW	_MODULE_MESSAGE_IDENTIFICATION_DEFINE
		;========================================
		;=		    			=
		;= MENU CONFIGURAR MENSAGEN ALARME 	=
		;=	 	    			=
		;========================================
	_MENU_ENGLISH_MENSAGEM_ALARME:
		;itens
;		.EQU	_MENU_MENSAGEM_ALARME_ITEM_ATIVAR 	= 1
;		.EQU	_MENU_MENSAGEM_ALARME_ITEM_DEFINIR	= 2
		;Options
		.DB	"Enable",_cr,_lf
		.DB	"Define",_cr,0
		;Coordinates
		.DW	24,66
		;Menu check memory
		.DW	_MENU_MESSAGE_ALARM_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_MESSAGE_ALARM_ACTIVATE
		.DW	_MODULE_MESSAGE_ALARM_DEFINE
		;================================
		;=		    		=
		;= MENU UTILITARIOS		=
		;=	 	    		=
		;============================================================================================
	_MENU_ENGLISH_UTILITARIOS:
		;Options
		.DB	"Video",_nc,_cr,_lf
		.DB	"Normal image",_cr,_lf
		.DB	"Difference image",_cr,_lf
		.DB	"Clock",0
		;Coordinates
		.DW	4+4+4,47
		;Menu check memory
		.DW	0;_MENU_UTILITARIOS_CHECK
		;Menu Links
		.DW	_MENU_ENGLISH_UTILITARIOS_VIDEO
		.DW	0	;no link
		.DW	0	;no link
		.DW	_MENU_ENGLISH_UTILITARIOS_RELOGIO
		;Menu Programs Links
		.DW	0	;no link
		.DW	_MODULE_SHOW_DIGITIZED_VIDEO
		.DW	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO
                .DW     0       ;no link
		;================================
		;=		    		=
		;= MENU UTILITARIOS VIDEO	=
		;=	 	    		=
		;================================
	_MENU_ENGLISH_UTILITARIOS_VIDEO:
		;Options
		.DB	"Dots",_cr,_lf
		.DB	"Grid",_cr,_lf
		.DB	"Bars",_cr,_lf
		.DB	"Superimpose",0
		;Coordinates
		.DW	4+4+4+4+4,58
		;Menu check memory
		.DW	0;_MENU_UTILITARIOS_VIDEO_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_DRAW_POINTS
		.DW	_MODULE_DRAW_GRID
		.DW	_MODULE_DRAW_PALETTE
		.DW	_MODULE_DRAW_SUPERIMPOSED
		;================================
		;=		    		=
		;= MENU UTILITARIOS RELOGIO	=
		;=	 	    		=
		;================================
	_MENU_ENGLISH_UTILITARIOS_RELOGIO:
		;Options
		.DB	"Analogic",_cr,_lf
		.DB	"Digital",0
		;Coordinates
		.DW	58,75
		;Menu check memory
		.DW	0;_MENU_UTILITARIOS_VIDEO_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_SHOW_ANALOG_CLOCK
		.DW	_MODULE_SHOW_DIGITAL_CLOCK

		;================================
		;=		    		=
		;= MENU IDIOMA			=
		;=	 	    		=
		;==============================================================================================
	_MENU_ENGLISH_IDIOM:
		;itens
;		.EQU	_MENU_IDIOMA_ITEM_PORTUGUES	= 1
;		.EQU	_MENU_IDIOMA_ITEM_INGLES	= 2
		;Options
		.DB	"Portugu",136,"s",_nc,_cr,_lf
		.DB	"English",_nc,_cr,_lf
		.DB	"Espa",164,"ol",0
		;Coordinates
		.DW	4+4+4+4+4,54
		;Menu check memory
		.DW	_MENU_IDIOM_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0
		;Menu Programs Links
		.DW	_MODULE_IDIOM_PORTUGUESE
		.DW	_MODULE_IDIOM_ENGLISH
		.DW	_MODULE_IDIOM_SPANISH
