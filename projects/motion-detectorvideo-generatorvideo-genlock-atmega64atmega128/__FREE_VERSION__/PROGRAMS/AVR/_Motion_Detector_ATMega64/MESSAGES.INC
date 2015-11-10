;				___  ___                                    
;				|  \/  |                                    
;				| .  . | ___  ___ ___  __ _  __ _  ___  ___ 
;				| |\/| |/ _ \/ __/ __|/ _` |/ _` |/ _ \/ __|
;				| |  | |  __/\__ \__ \ (_| | (_| |  __/\__ \
;				\_|  |_/\___||___/___/\__,_|\__, |\___||___/
;				                             __/ |          
;				                            |___/           
;


;	>--------------------------------------------------------------------------------------------<
;-------|											     |
;	| 					M E S S A G E S			     		     |
;	| 											     |
; 	 --------------------------------------------------------------------------------------------

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


;///////////////////////////////////////////////
;//   LANGUAGE DEPENDENT MESSAGES INDEXES     //
;///////////////////////////////////////////////

.EQU	_MSG_SERIAL		= 1
.EQU	_MSG_NO_SIGNAL		= 2
.EQU	_MSG_SIGNAL_PRESENT	= 3
.EQU	_MSG_CONFIRM		= 4
.EQU	_MSG_INPUT_BOX_OK	= 5
.EQU	_MSG_INPUT_BOX_YES	= 6
.EQU	_MSG_INPUT_BOX_NO	= 7
.EQU	_MSG_INPUT_BOX_CANCEL	= 8
.EQU	_MSG_CLEAR_PAGE		= 9
.EQU	_MSG_DEFINE_IP		= 10
.EQU	_MSG_MOVE_DETECTOR	= 11
.EQU	_MSG_RTC_NOT_SET	= 12
.EQU	_MSG_RELEASE		= 13
.EQU	_MSG_REALLY_CLEAR	= 14
.EQU	_MSG_RELEASE_ENTER	= 15
.EQU	_MSG_SET_PASSWORD	= 16
.EQU	_MSG_CHECK_PASSWORD	= 17
.EQU	_MSG_DEFAULT_PASSWORD	= 18
.EQU	_MSG_RELAY_TIME_ON	= 19
.EQU	_MSG_RELAY_TIME_OFF	= 20
.EQU	_MSG_RELAY_TIME_ACTIVATION= 21
.EQU	_MSG_PROGRAM_1		= 22
.EQU	_MSG_PROGRAM_2		= 23
.EQU	_MSG_PROGRAM_3		= 24
.EQU	_MSG_PROGRAM_4		= 25

;/////////////////////////
;//			//
;// PORTUGUESE MESSAGES	//
;//			//
;////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
_MSG_LANGUAGE_PORTUGUESE:

.DW _MSG_NO_SIGNAL
	.DB "Sem sinal de Video",_nc,0
.DW _MSG_SIGNAL_PRESENT
	.DB "  Sinal presente  ",_nc,0
.DW _MSG_CONFIRM
	.DB "Confirma",_cr,_lf
	.DB "altera",135,177,"o?",_nc,0
.DW _MSG_INPUT_BOX_OK
	.DB "Ok",_nc,0
.DW _MSG_INPUT_BOX_YES
	.DB "Sim",0
.DW _MSG_INPUT_BOX_NO
	.DB "N",177,"o",0
.DW _MSG_INPUT_BOX_CANCEL
	.DB "Cancela",0
.DW _MSG_CLEAR_PAGE
	.DB "Limpa p",160,"gina?",0
.DW _MSG_DEFINE_IP
	.DB "    Defina IP    ",_nc,_cr,_lf
	.DB "                 ",_nc,_cr,_lf
	.DB "                 ",_nc,_cr,_lf
	.DB "                 ",_nc,_cr,_lf
	.DB "                 ",0
.DW _MSG_MOVE_DETECTOR
	.DB " Detector ",_cr,_lf
	.DB "    de    ",_cr,_lf
	.DB " Movimento",_nc,0
.DW _MSG_RTC_NOT_SET
	.DB "Acerte o",_cr,_lf
	.DB "Rel",162,"gio",0
.DW _MSG_RELEASE
	.DB "Vers",177,"o Liberada ",_cr,_lf
	.DB "      Para      ",_cr,_lf,_cr,_lf
	.DB " ----------------",0
.DW _MSG_REALLY_CLEAR
	.DB "Confirma",_cr,_lf
	.DB "Limpar?",0
.DW _MSG_RELEASE_ENTER
	.DB "Liberar Tecla",_nc,_cr,_lf
	.DB "    Enter  ",0
.DW _MSG_SET_PASSWORD
	.DB "  Defina senha ",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_CHECK_PASSWORD
	.DB " Forne",135,"a senha ",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_DEFAULT_PASSWORD
	.DB " Forne",135,"a senha",_cr,_lf
	.DB " padr",177,"o",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_RELAY_TIME_ON
	.DB " Tempo ligado ",_nc,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_RELAY_TIME_OFF
	.DB " Tempo desligado ",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_RELAY_TIME_ACTIVATION
	.DB " Tempo ativa",135,177,"o ",_nc,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_PROGRAM_1
	.DB " Prog#1 livre para ",_nc,_cr,_lf
	.DB " Implementa",135,177,"o ",0
.DW _MSG_PROGRAM_2
	.DB " Prog#2 livre para",_cr,_lf
	.DB " Implementa",135,177,"o ",0
.DW _MSG_PROGRAM_3
	.DB " Prog#3 livre para",_cr,_lf
	.DB " Implementa",135,177,"o ",0
.DW _MSG_PROGRAM_4
	.DB " Prog#4 Livre para",_cr,_lf
	.DB " Implementa",135,177,"o ",0


;/////////////////////////
;//			//
;// ENGLISH MESSAGES	//
;//			//
;////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
_MSG_LANGUAGE_ENGLISH:

.DW _MSG_NO_SIGNAL
	.DB " No video signal  ",_nc,0
.DW _MSG_SIGNAL_PRESENT
	.DB "  Signal present  ",_nc,0

.DW _MSG_CONFIRM
	.DB " Confirm changes  ",_nc,0
.DW _MSG_INPUT_BOX_OK
	.DB "Ok",_nc,0
.DW _MSG_INPUT_BOX_YES
	.DB "Yes",0
.DW _MSG_INPUT_BOX_NO
	.DB "No",_nc,0
.DW _MSG_INPUT_BOX_CANCEL
	.DB "Cancel",_nc,0
.DW _MSG_CLEAR_PAGE
	.DB "Clear page?",0
.DW _MSG_DEFINE_IP
	.DB "    Define IP    ",_nc,_cr,_lf
	.DB "                 ",_nc,_cr,_lf
	.DB "                 ",_nc,_cr,_lf
	.DB "                 ",_nc,_cr,_lf
	.DB "                 ",0
.DW _MSG_MOVE_DETECTOR
	.DB "  Motion  ",_cr,_lf
	.DB " Detector ",_nc,0
.DW _MSG_RTC_NOT_SET
	.DB " Set up",_nc,_cr,_lf
	.DB "the Clock",0
.DW _MSG_RELEASE
	.DB "Version Released",_cr,_lf
	.DB "      For       ",_cr,_lf,_cr,_lf
	.DB " ---------------",_nc,0
.DW _MSG_REALLY_CLEAR
	.DB "Confirm",_nc,_cr,_lf
	.DB "Clear? ",0
.DW _MSG_RELEASE_ENTER
	.DB "Release Enter",_nc,_cr,_lf
	.DB "    Key    ",0
.DW _MSG_SET_PASSWORD
	.DB "  Set Password ",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_CHECK_PASSWORD
	.DB "Enter Password ",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_DEFAULT_PASSWORD
	.DB "Enter default ",_cr,_lf
	.DB "Password ",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_RELAY_TIME_ON
	.DB " Time On ",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_RELAY_TIME_OFF
	.DB " Time Off ",_nc,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_RELAY_TIME_ACTIVATION
	.DB " Time activation ",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_PROGRAM_1
	.DB " Prog#1 free for",_cr,_lf
	.DB " implementations ",0
.DW _MSG_PROGRAM_2
	.DB " Prog#2 free for",_cr,_lf
	.DB " implementations ",0
.DW _MSG_PROGRAM_3
	.DB " Prog#3 free for",_cr,_lf
	.DB " implementations ",0
.DW _MSG_PROGRAM_4
	.DB " Prog#4 free for",_cr,_lf
	.DB " implementations ",0
;/////////////////////////
;//			//
;// SPANISH MESSAGES	//
;//			//
;////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
_MSG_LANGUAGE_SPANISH:

.DW _MSG_NO_SIGNAL
	.DB "Sin se",164,"al de Video ",0
.DW _MSG_SIGNAL_PRESENT
	.DB "  Se",164,"al presente ",0
.DW _MSG_CONFIRM
	.DB 168,"Confirmar la",_nc,_cr,_lf
	.DB     " alteraci",162,"n?",_nc,0
.DW _MSG_INPUT_BOX_OK
	.DB "Ok",_nc,0
.DW _MSG_INPUT_BOX_YES
	.DB "S",161,_nc,0
.DW _MSG_INPUT_BOX_NO
	.DB "No",_nc,0
.DW _MSG_INPUT_BOX_CANCEL
	.DB "Cancelar",_nc,0
.DW _MSG_CLEAR_PAGE
	.DB 168,"Limpiar p",160,"gina?",_nc,0
.DW _MSG_DEFINE_IP
	.DB "   Definir IP    ",_nc,_cr,_lf
	.DB "                 ",_nc,_cr,_lf
	.DB "                 ",_nc,_cr,_lf
	.DB "                 ",_nc,_cr,_lf
	.DB "                 ",0
.DW _MSG_MOVE_DETECTOR
	.DB " Detector ",_cr,_lf
	.DB "    de    ",_cr,_lf
	.DB "Movimiento",_nc,0
.DW _MSG_RTC_NOT_SET
	.DB "Acertar o",_nc,_cr,_lf
	.DB "  Reloj",0
.DW _MSG_RELEASE
	.DB "Vers",161,"on Liberada",_cr,_lf
	.DB "      Para      ",_cr,_lf,_cr,_lf
	.DB " ----------------",0
.DW _MSG_REALLY_CLEAR
	.DB "Confirma",_cr,_lf
	.DB 168,"Limpiar?",0
.DW _MSG_RELEASE_ENTER
	.DB "Soltar tecla",_cr,_lf
	.DB "    Enter  ",0
.DW _MSG_SET_PASSWORD
	.DB "Definir contrase",164,"a",_nc,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_CHECK_PASSWORD
	.DB "Entrar contrase",164,"a",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_DEFAULT_PASSWORD
	.DB "Entrar contrase ",164,"a",_cr,_lf
	.DB "patr",162,"n ",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_RELAY_TIME_ON
	.DB " Tiempo ligado ",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_RELAY_TIME_OFF
	.DB " Tiempo desligado ",_nc,_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_RELAY_TIME_ACTIVATION
	.DB " Tiempo ativaci",162,"n",_cr,_lf,_cr,_lf,_cr,_lf,_cr,_lf,0
.DW _MSG_PROGRAM_1
	.DB " Prog#1 libre para",_cr,_lf
	.DB " Implementaci",162,"n",0
.DW _MSG_PROGRAM_2
	.DB " Prog#2 libre para",_cr,_lf
	.DB " Implementaci",162,"n",0
.DW _MSG_PROGRAM_3
	.DB " Prog#3 libre para",_cr,_lf
	.DB " Implementaci",162,"n",0
.DW _MSG_PROGRAM_4
	.DB " Prog#4 libre para",_cr,_lf
	.DB " Implementaci",162,"n",0


