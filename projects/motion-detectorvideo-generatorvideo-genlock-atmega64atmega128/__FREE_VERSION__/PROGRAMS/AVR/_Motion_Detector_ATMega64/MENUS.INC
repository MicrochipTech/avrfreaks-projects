; 	 ____________________________________________________________________________________________
;	|////////////////////////////////////////////////////////////////////////////////////////////|
;	|///|											|////|
;	|///| PROJETO	: DETECTOR DE MOVIMENTO VERSÃO 5.0(MOTION DETECTOR V5.0)		|////|
;	|///| VERSAO	: 1.0									|////|
;	|///| PROGRAMDOR: JOÃO DARTAGNAN ANTUNES OLIVEIRA				   	|////|
;	|///| DATA	: 6 de Dezembro de 2006 (00:06)						|////|
;	|///| DESCRIÇÃO	: Esse projeto possui 2 conectores BNC, 1 para entrada de video e outro	|////|
;	|///|		  para saida, 1 saida de rele NC E NO,1 porta de comunicação 485 com 	|////|
;	|///| 		  leds de TX E RX, 1 porta de expansão e 1 para programacao ISP		|////|
;	|///| OBJETIVO:	  O circuito destina-se a captura de sinal composto de video padrao PAL/|////|
;	|///| 		  NTSC color ou BW, digitalizar e processar a ocorrencia de movimento	|////|
;	|///| 		  em areas especificas e avisar atraves de comunicação serial e/ou ati-	|////|
;	|///| 		  vação de relé. Possuindo ainda um software para geração de video com-	|////|
;	|///| 		  posto BW com MENU de configuração e GENLOCK com sinal de entrada de	|////|
;	|///| 		  video.								|////|
;	|///| 											|////|
;	|///| 											|////|
;	|///|___________________________________________________________________________________|////|
;-------|////////////////////////////////////////////////////////////////////////////////////////////|
;	>--------------------------------------------------------------------------------------------<
;-------|											     |
;	| 			M E N U S  - P O R T U G U E S E,  E N G L I S H		     |
;	| 											     |
; 	 --------------------------------------------------------------------------------------------


;
;
; PORTUGUESE
;
;
		;================
		;=		=
		;= MENU INICIAR =
		;=		=
		;===================================================================================================
	_MENU_PORTUGUESE_START:
		;Options
		.DB	"Iniciar",0
		;Coordinates
		.DW	4,4
		;Menu check memory
		.DW	0;_MENU_START_CHECK
		;Menu Links
		.DW	_MENU_PORTUGUESE_PRINCIPAL
		;Menu Programs Links
		.DW	0
		;====================
		;=		    =
		;= MENU PRINCIPAL   =
		;=	 	    =
		;====================================================================================================
	_MENU_PORTUGUESE_PRINCIPAL:
		;Options
		.DB	"Configurar",_cr,_lf
		.DB	"Mensagens",_cr,_cr,_lf
		.DB	"Utilit",160,"rios",_cr,_cr,_lf
		.DB	"Idioma",_cr,_lf
		.DB	"Programas",_cr,_cr,_lf
		.DB	"Sair",_cr,0
		;Coordinates
		.DW	4+4,4+16-1
		;Menu check memory
		.DW	0;_MENU_MAIN_CHECK
		;Menu Links
		.DW	_MENU_PORTUGUESE_CONFIGURAR
		.DW	_MENU_PORTUGUESE_MENSAGENS
		.DW	_MENU_PORTUGUESE_UTILITARIOS
		.DW	_MENU_PORTUGUESE_IDIOM
		.DW	0	;no link 
		.DW	0	;no link
		;Menu Programs Links
		.DW	0
		.DW	0
		.DW	0
		.DW	0
		.DW	0
		.DW	0
		;====================
		;=		    =
		;= MENU CONFIGURAR  =
		;=	 	    =
		;====================================================================================================
	_MENU_PORTUGUESE_CONFIGURAR:
		;Options
		.DB	"Sistema de v",161,"deo",_cr,_lf
		.DB	"Det.de movimento",_cr,_lf
		.DB	"Ativa",135,177,"o",_cr,_lf
		.DB	"Comunica",135,177,"o",0
		;Coordinates
		.DW	12,31
		;Menu check memory
		.DW	0	;no link
		;Menu Links
		.DW	_MENU_PORTUGUESE_CONFIGURAR_SISTEMA
		.DW	_MENU_PORTUGUESE_CONFIGURAR_CAPTURA_MOVIMENTO
		.DW	_MENU_PORTUGUESE_CONFIGURAR_ATIVACAO
		.DW	_MENU_PORTUGUESE_CONFIGURAR_COMUNICACAO
		;Menu Programs Links
		.DW	0
		.DW	0
		.DW	0
		.DW	0
		;================================
		;=		    		=
		;= MENU CONFIGURAR SISTEMA 	=
		;=	 	    		=
		;================================
	_MENU_PORTUGUESE_CONFIGURAR_SISTEMA:
		;itens numbers
		.EQU	_MENU_CONFIGURAR_SISTEMA_ITEM_NTSC = 1
		.EQU	_MENU_CONFIGURAR_SISTEMA_ITEM_PAL  = 2
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
	_MENU_PORTUGUESE_CONFIGURAR_CAPTURA_MOVIMENTO:
		;Options
		.DB	"Grade",_cr,_cr,_lf
		.DB	"Sensibilidade",_cr,_cr,_lf
		.DB	"Rastro",_cr,_lf
		.DB	"Tempo captura",_cr,_cr,_lf
		.DB	"Tempo manuten",135,177,"o",_cr,0
		;Coordinates
		.DW	16,50
		;Menu check memory
		.DW	0	;no check
		;Menu Links
		.DW	0	;no link
		.DW	_MENU_PORTUGUESE_CONFIGURAR_CAPTURA_MOVIMENTO_SENSIBILIDADE
		.DW	_MENU_PORTUGUESE_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_CAPTURE_GRID
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		;========================================================
		;=		    					=
		;= MENU CONFIGURAR CAPTURA DE MOVIMENTO SENSIBILIDADE	=
		;=	 	    					=
		;========================================================
	_MENU_PORTUGUESE_CONFIGURAR_CAPTURA_MOVIMENTO_SENSIBILIDADE:
		;Options
		.DB	" 1",_cr,_lf
		.DB	" 2",_cr,_lf
		.DB	" 3",_cr,_lf
		.DB	" 4",_cr,_lf
		.DB	" 5",_cr,_lf
		.DB	" 6",_cr,_lf
		.DB	" 7",_cr,_lf
		.DB	" 8",_cr,_lf
		.DB	" 9",_cr,_lf
		.DB	"10",_cr,0
		;Coordinates
		.DW	99,4
		;Menu check memory
		.DW	_MENU_CONFIGURE_CAPTURE_MOVEMENT_SENSIBILITY_CHECK
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
		;Menu Programs Links
		.DW	_MODULE_SET_SENSIBILITY
		.DW	_MODULE_SET_SENSIBILITY
		.DW	_MODULE_SET_SENSIBILITY
		.DW	_MODULE_SET_SENSIBILITY
		.DW	_MODULE_SET_SENSIBILITY
		.DW	_MODULE_SET_SENSIBILITY
		.DW	_MODULE_SET_SENSIBILITY
		.DW	_MODULE_SET_SENSIBILITY
		.DW	_MODULE_SET_SENSIBILITY
		.DW	_MODULE_SET_SENSIBILITY
		;========================================================
		;=		    					=
		;= MENU CONFIGURAR CAPTURA DE MOVIMENTO RASTRO		=
		;=	 	    					=
		;========================================================
	_MENU_PORTUGUESE_CONFIGURAR_CAPTURA_MOVIMENTO_RASTRO:
		;itens numbers
		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_SEM = 1
		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_1P2 = 2
		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_2P4 = 3
		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_4P8 = 4
		.EQU	_MENU_CONFIGURAR_SISTEMA_CAPTURA_MOVIMENTO_RASTRO_9P6 = 5
		;Options
		.DB	"Sem",_cr,_cr,_lf
		.DB	"1.2s ",_cr,_cr,_lf
		.DB	"2.4s ",_cr,_cr,_lf
		.DB	"4.8s ",_cr,_cr,_lf
		.DB	"9.6s ",0
		;Coordinates
		.DW	76,47
		;Menu check memory
		.DW	_MENU_CONFIGURE_CAPTURE_MOVEMENT_PATH_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_SET_MOVEMENT_PATH
		.DW	_MODULE_SET_MOVEMENT_PATH
		.DW	_MODULE_SET_MOVEMENT_PATH
		.DW	_MODULE_SET_MOVEMENT_PATH
		.DW	_MODULE_SET_MOVEMENT_PATH
		;================================
		;=		    		=
		;= MENU CONFIGURAR ATIVACAO 	=
		;=	 	    		=
		;================================
	_MENU_PORTUGUESE_CONFIGURAR_ATIVACAO:
		;Options
		.DB	"Por sensibilidade",_cr,_cr,_lf
		.DB	"Por tempo",0
		;Coordinates
		.DW	12,59
		;Menu check memory
		.DW	_MENU_CONFIGURE_ACTIVATION_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_ACTIVATION_MODE
		.DW	_MODULE_ACTIVATION_MODE
		;================================
		;=		    		=
		;= MENU CONFIGURAR COMUNICACAO 	=
		;=	 	    		=
		;================================
	_MENU_PORTUGUESE_CONFIGURAR_COMUNICACAO:
		;Options
		.DB	"Ativar",_cr,_lf
		.DB	"Definir IP",_cr,0
		;Coordinates
		.DW	20,66
		;Menu check memory
		.DW	_MENU_CONFIGURE_COMMUNICATION_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_COMMUNICATION_MODE
		.DW	0	;no link
		;================================
		;=		    		=
		;= MENU CONFIGURAR MENSAGENS 	=
		;=	 	    		=
		;============================================================================================
	_MENU_PORTUGUESE_MENSAGENS:
		;Options
		.DB	"Apresenta",135,177,"o",_cr,_lf
		.DB	"Identifica",135,177,"o",_cr,_cr,_lf
		.DB	"Alarme",_cr,0
		;Coordinates
		.DW	16,38
		;Menu check memory
		.DW	0	;no link
		;Menu Links
		.DW	_MENU_PORTUGUESE_MENSAGEM_APRESENTACAO
		.DW	_MENU_PORTUGUESE_MENSAGEM_IDENTIFICACAO
		.DW	_MENU_PORTUGUESE_MENSAGEM_ALARME
		;Menu Programs Links
		.DW	0	;no link
		.DW	0	;no link
		.DW	0
		;================================================
		;=		    				=
		;= MENU CONFIGURAR MENSAGEN APRESENTACAO 	=
		;=	 	    				=
		;================================================
	_MENU_PORTUGUESE_MENSAGEM_APRESENTACAO:
		;itens
		.EQU	_MENU_MENSAGEM_APRENSENTACAO_ITEM_ATIVAR 	= 1
		.EQU	_MENU_MENSAGEM_APRENSENTACAO_ITEM_DEFINIR	= 2
		;Options
		.DB	"Ativar",_cr,_lf
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
	_MENU_PORTUGUESE_MENSAGEM_IDENTIFICACAO:
		;itens
		.EQU	_MENU_MENSAGEM_IDENTIFICACAO_ITEM_ATIVAR 	= 1
		.EQU	_MENU_MENSAGEM_IDENTIFICACAO_ITEM_DEFINIR	= 2
		;Options
		.DB	"Ativar",_cr,_lf
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
	_MENU_PORTUGUESE_MENSAGEM_ALARME:
		;itens
		.EQU	_MENU_MENSAGEM_ALARME_ITEM_ATIVAR 	= 1
		.EQU	_MENU_MENSAGEM_ALARME_ITEM_DEFINIR	= 2
		;Options
		.DB	"Ativar",_cr,_lf
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
	_MENU_PORTUGUESE_UTILITARIOS:
		;Options
		.DB	"V",161,"deo",_cr,_cr,_lf
		.DB	"Imagem normal",_cr,_cr,_lf
		.DB	"Imagem diferen",135,"a",_cr,_lf
		.DB	132,"rea",_cr,_lf
		.DB	"Gr",160,"fico",0
		;Coordinates
		.DW	4+4+4,47
		;Menu check memory
		.DW	0;_MENU_UTILITARIOS_CHECK
		;Menu Links
		.DW	_MENU_PORTUGUESE_UTILITARIOS_VIDEO
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	0	;no link
		.DW	_MODULE_SHOW_DIGITIZED_VIDEO
		.DW	_MODULE_SHOW_DIGITIZED_MOVEMENT_VIDEO
		.DW	_MODULE_SHOW_DIGITIZED_AREA
		.DW	0	;no link
		;================================
		;=		    		=
		;= MENU UTILITARIOS VIDEO	=
		;=	 	    		=
		;================================
	_MENU_PORTUGUESE_UTILITARIOS_VIDEO:
		;Options
		.DB	"Pontos",_cr,_lf
		.DB	"Grade",_cr,_cr,_lf
		.DB	"Degrade",_cr,_cr,_lf
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
		;= MENU IDIOMA			=
		;=	 	    		=
		;==============================================================================================
	_MENU_PORTUGUESE_IDIOM:
		;itens
		.EQU	_MENU_IDIOMA_ITEM_PORTUGUES	= 1
		.EQU	_MENU_IDIOMA_ITEM_INGLES	= 2
		;Options
		.DB	"Portugu",136,"s",_cr,_cr,_lf
		.DB	"Ingl",136,"s",_cr,0
		;Coordinates
		.DW	4+4+4+4+4,54
		;Menu check memory
		.DW	_MENU_IDIOM_CHECK
		;Menu Links
		.DW	0	;no link
		.DW	0	;no link
		;Menu Programs Links
		.DW	_MODULE_IDIOM_PORTUGUESE
		.DW	_MODULE_IDIOM_ENGLISH
