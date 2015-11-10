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
;				      _____                   _     _     
;				     /  ___|                 (_)   | |    
;				     \ `--. _ __   __ _ _ __  _ ___| |__  
;				      `--. \ '_ \ / _` | '_ \| / __| '_ \ 
;			 	     /\__/ / |_) | (_| | | | | \__ \ | | |
;				     \____/| .__/ \__,_|_| |_|_|___/_| |_|
;				           | |                            
;				           |_|                            


		;================
		;=		=
		;= MENU INICIAR =
		;=		=
		;===================================================================================================
	_MENU_SPANISH_START:
		;Options
		.DB	"Comenzar",_nc,0
		;Coordinates
		.DW	4,4
		;Menu check memory
		.DW	0;_MENU_START_CHECK
		;Menu Links
		.DW	_MENU_SPANISH_PRINCIPAL
		;Menu Programs Links
		.DW	0
		;====================
		;=		    =
		;= MENU PRINCIPAL   =
		;=	 	    =
		;====================================================================================================
	_MENU_SPANISH_PRINCIPAL:
		;Options
		.DB	"Configurar",_cr,_lf
		.DB	"Mensajes",_cr,_lf
		.DB	"Utilidades",_cr,_lf
		.DB	"Idioma",_cr,_lf
		.DB	"Contrase",164,"a",_cr,_lf
		.DB	"Programas",_nc,_cr,_lf
                .DB     "Reset(Cold)",_nc,_cr,_lf
		.DB	"Salida",_nc,0
		;Coordinates
		.DW	4+4,4+16-1
		;Menu check memory
		.DW	0;_MENU_MAIN_CHECK
		;Menu Links
		.DW	_MENU_SPANISH_CONFIGURAR
		.DW	_MENU_SPANISH_MENSAGENS
		.DW	_MENU_SPANISH_UTILITARIOS
		.DW	_MENU_SPANISH_IDIOM
		.DW	0	;no link 
		.DW	_MENU_SPANISH_PROGRAMS_EXECUTE
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
	_MENU_SPANISH_PROGRAMS_EXECUTE:
		;Options
		.DB	"Programa - 1",_cr,_lf
		.DB	"Programa - 2",_cr,_lf
		.DB	"Programa - 3",_cr,_lf
		.DB	"Programa - 4",_nc,0
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
	_MENU_SPANISH_CONFIGURAR:
		;Options
		.DB	"Sistema de Video",_cr,_lf
		.DB	"Det. de movimiento",_cr,_lf
		.DB	"Activatici",162,"n Rel",130,_nc,_cr,_lf
		.DB	"Comunicaci",162,"n",_cr,_lf
		.DB	"Pantalla de MENU",_cr,_lf
		.DB	"Reloj",0
		;Coordinates
		.DW	4,31
		;Menu check memory
		.DW	0	;no link
		;Menu Links
		.DW	_MENU_SPANISH_CONFIGURAR_SISTEMA
		.DW	_MENU_SPANISH_CONFIGURAR_CAPTURA_MOVIMENTO
		.DW	_MENU_SPANISH_CONFIGURAR_ATIVACAO
		.DW	_MENU_SPANISH_CONFIGURAR_COMUNICACAO
		.DW	_MENU_SPANISH_CONFIGURAR_TELA
		.DW	0	;no link
		;Menu Programs Links
		.DW	0
		.DW	0
		.DW	0
		.DW	0
		.DW	0
		.DW	_MODULE_CLOCK_SET
		;================================
		;=		    		=
		;= MENU CONFIGURAR SISTEMA 	=
		;=	 	    		=
		;================================
	_MENU_SPANISH_CONFIGURAR_SISTEMA:
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
	_MENU_SPANISH_CONFIGURAR_CAPTURA_MOVIMENTO:
		;Options
		.DB	"Cuadr",161,"cula",_cr,_lf
		.DB	"Sensibilidad",_cr,_lf
		.DB	"Rastro",_cr,_lf
		.DB	"Probar",_nc,0
		;Coordinates
		.DW	4,50
		;Menu check memory
		.DW	0	;no check
		;Menu Links
		.DW	_MENU_SPANISH_CONFIGURAR_CAPTURA_MOVIMENTO_GRADE
		.DW	0	;no link
		.DW	_MENU_SPANISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO
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
	_MENU_SPANISH_CONFIGURAR_CAPTURA_MOVIMENTO_GRADE:
		;Options
		.DB	"Definir",_nc,_cr,_lf
		.DB	"Limpiar",0
		;Coordinates
		.DW	8,62
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
	_MENU_SPANISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO:
		;itens numbers
		;Options
		.DB	"Tipo",_cr,_lf
		.DB	"Tiempo",_nc,0
		;Coordinates
		.DW	44,70
		;Menu check memory
		.DW	0	;no link
		;Menu Links
		.DW	_MENU_SPANISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO_TIPO
		.DW	_MENU_SPANISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO_TEMPO
		;Menu Programs Links
		.DW	0	;no link
		.DW	0	;no link
		;========================================================
		;=		    					=
		;= MENU CONFIGURAR CAPTURA DE MOVIMENTO RASTRO TIPO	=
		;=	 	    					=
		;========================================================
	_MENU_SPANISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO_TIPO:
		;itens numbers
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_BOX 	= 1
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_BOX_FULL = 2
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_CONTORNO = 3
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_CENTRO	= 4
;		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_TIPO_GRAFICO	= 5
	;Options
		.DB	"Cuadr",161,"cula",_cr,_lf
		.DB	"Cuadr",161,"cula plena",_cr,_lf
		.DB	"Contorno",_cr,_lf
		.DB	"Cruz",_cr,_lf
		.DB	"Gr",160,"fico XY",_nc,0
		;Coordinates
		.DW	14,25
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
	_MENU_SPANISH_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO_TEMPO:
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
		.DB	"9.6s",_cr,0
		;Coordinates
		.DW	85,46
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
	_MENU_SPANISH_CONFIGURAR_ATIVACAO:
		;Options
		.DB	"por sensibilidad",_cr,_lf
		.DB	"por Tiempo",_cr,_lf
		.DB	"Configurar ",0
		;Coordinates
		.DW	12,59
		;Menu check memory
		.DW	_MENU_CONFIGURE_ACTIVATION_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	_MENU_SPANISH_CONFIGURAR_ATIVACAO_CONFIGURACAO
		;Menu Programs Links
		.DW	_MODULE_ACTIVATION_MODE
		.DW	_MODULE_ACTIVATION_MODE
		.DW	0	;no link
		;================================================
		;=		    				=
		;= MENU CONFIGURAR ATIVACAO CONFIGURACAO	=
		;=	 	    				=
		;================================================
	_MENU_SPANISH_CONFIGURAR_ATIVACAO_CONFIGURACAO:
		;Options
		.DB	"Tiempo rel",130," lig.",_cr,_lf
		.DB	"Tiempo rel",130," desl.",_nc,_cr,_lf
		.DB	"Tiempo activaci",162,"n",_nc,_cr,_lf
		.DB	"Repetici",162,"n ",0
		;Coordinates
		.DW	4,38
		;Menu check memory
		.DW	0
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	_MENU_SPANISH_CONFIGURAR_ATIVACAO_CONFIGURACAO_REPETICAO
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
	_MENU_SPANISH_CONFIGURAR_ATIVACAO_CONFIGURACAO_REPETICAO:
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
		.DB	"Libre",0
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
	_MENU_SPANISH_CONFIGURAR_COMUNICACAO:
		;Options
		.DB	"Permitir",_cr,_lf
		.DB	"Definir IP",_nc,0
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
	_MENU_SPANISH_CONFIGURAR_TELA:
		;itens numbers
;		.EQU	_MENU_CONFIGURAR_TELA_SOLIDA = 1
;		.EQU	_MENU_CONFIGURAR_TELA_TRANSPARENTE = 2
		;Options
		.DB	"S",162,"lida",_cr,_lf
		.DB	"Transparente",_nc,0
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
	_MENU_SPANISH_MENSAGENS:
		;Options
		.DB	"Introducci",162,"n",_cr,_lf
		.DB	"Identificaci",162,"n",_cr,_lf
		.DB	"Alarma",_nc,0
		;Coordinates
		.DW	16,38
		;Menu check memory
		.DW	0	;no link
		;Menu Links
		.DW	_MENU_SPANISH_MENSAGEM_APRESENTACAO
		.DW	_MENU_SPANISH_MENSAGEM_IDENTIFICACAO
		.DW	_MENU_SPANISH_MENSAGEM_ALARME
		;Menu Programs Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0
		;================================================
		;=		    				=
		;= MENU CONFIGURAR MENSAGEN APRESENTACAO 	=
		;=	 	    				=
		;================================================
	_MENU_SPANISH_MENSAGEM_APRESENTACAO:
		;itens
;		.EQU	_MENU_MENSAGEM_APRENSENTACAO_ITEM_ATIVAR 	= 1
;		.EQU	_MENU_MENSAGEM_APRENSENTACAO_ITEM_DEFINIR	= 2
		;Options
		.DB	"Permitir",_cr,_lf
		.DB	"Definir",0
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
	_MENU_SPANISH_MENSAGEM_IDENTIFICACAO:
		;itens
;		.EQU	_MENU_MENSAGEM_IDENTIFICACAO_ITEM_ATIVAR 	= 1
;		.EQU	_MENU_MENSAGEM_IDENTIFICACAO_ITEM_DEFINIR	= 2
		;Options
		.DB	"Permitir",_cr,_lf
		.DB	"Definir",0
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
	_MENU_SPANISH_MENSAGEM_ALARME:
		;itens
;		.EQU	_MENU_MENSAGEM_ALARME_ITEM_ATIVAR 	= 1
;		.EQU	_MENU_MENSAGEM_ALARME_ITEM_DEFINIR	= 2
		;Options
		.DB	"Permitir",_cr,_lf
		.DB	"Definir",0
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
	_MENU_SPANISH_UTILITARIOS:
		;Options
		.DB	"Video",_nc,_cr,_lf
		.DB	"Imagen normal",_nc,_cr,_lf
		.DB	"Imagen diferencia",_nc,_cr,_lf
		.DB	"Reloj",0
		;Coordinates
		.DW	4+4,47
		;Menu check memory
		.DW	0;_MENU_UTILITARIOS_CHECK
		;Menu Links
		.DW	_MENU_SPANISH_UTILITARIOS_VIDEO
		.DW	0	;no link
		.DW	0	;no link
		.DW	_MENU_SPANISH_UTILITARIOS_RELOGIO
		;Menu Programs Links
		.DW	0	;no link
		.DW	_MODULE_SHOW_DIGITIZED_VIDEO
		.DW	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO
		.DW	0
		;================================
		;=		    		=
		;= MENU UTILITARIOS VIDEO	=
		;=	 	    		=
		;================================
	_MENU_SPANISH_UTILITARIOS_VIDEO:
		;Options
		.DB	"Punto",_nc,_cr,_lf
		.DB	"Cuadr",161,"cula",_cr,_lf
		.DB	"Barras",_cr,_lf
		.DB	"Sobreponer",_nc,0
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
	_MENU_SPANISH_UTILITARIOS_RELOGIO:
		;Options
		.DB	"Anal",162,"gico",_nc,_cr,_lf
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
	_MENU_SPANISH_IDIOM:
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
