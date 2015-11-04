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
;+--------------------------------------------------------------+---+
;+------------------------------------------------------+
;| DISPLAY DRIVE FOR T6963C TOSHIBA CHIP		|
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATE:12/12/2000 VERSION 1.0				|
;>------------------------------------------------------<
;| H A R D W A R E  D E F I N I T I O N S		|
;+------------------------------------------------------+
;
;+------------------------------------------------------------------------------------------------------+
;| HARDWARE DEFINITIONS	  										|
;|													|
;| BEFORE USE THIS DRIVE SET FOLLOWING PARAMETERS							|
;|													|
;| _DISP_PORT_DATA_OUT		TO DEFINE AVR PORT TO SEND DATA AND COMMAND TO DISPLAY			|
;| _DISP_PORT_DATA_IN											|
;| _DISP_PORT_DATA_DIR											|
;|													|
;| _DISP_PORT_CMD_OUT		TO DEFINE AVR PORT THAT HAVE BITS TO CONTROL DISPLAY INTERFACE		|
;| _DISP_PORT_CMD_IN											|
;| _DISP_PORT_CMD_DIR											|
;|													|
;| _DISP_BIT_WR			SELECT BIT NUMBER INTO _DISP_PORT_CMD FOR EACH SIGNAL RS,RW AND E	|
;| _DISP_BIT_RD												|
;| _DISP_BIT_CE												|
;| _DISP_BIT_CD												|
;| _DISP_BIT_RESET											|
;|													|
;| EXAMPLE:												|
;|													|
;|.EQU	_DISP_BIT_WR   	=7				;DISPLAY CONTROL SIGNALS BITS			|
;|.EQU	_DISP_BIT_RD   	=6										|
;|.EQU	_DISP_BIT_CE	=5										|
;|.EQU	_DISP_BIT_CD	=4										|
;|.EQU	_DISP_BIT_RESET	=3										|
;|													|
;|.EQU	_DISP_PORT_DATA_OUT	= PORTA			;PORT TO SEND DATA TO DISPLAY			|
;|.EQU	_DISP_PORT_DATA_IN	= PINA			;PORT TO READ DATA FROM DISPLAY			|
;|.EQU	_DISP_PORT_DATA_DIR	= DDRA			;PORT TO DEFINE DATA DIRECTION			|
;|.EQU	_DISP_PORT_CMD_OUT	= PORTC			;PORT TO SEND COMMANDS TO DISPLAY		|
;|.EQU	_DISP_PORT_CMD_IN	= PINC			;ONLY FOR OBJECT COMPATIBITY			|
;|.EQU	_DISP_PORT_CMD_DIR	= DDRC			;ONLY FOR OBJECT COMPATIBITY			|
;|													|
;+------------------------------------------------------------------------------------------------------+
;+-----------------------------------+
;| DISPLAY CONSTANTS AND DEFINITIONS |
;+===================================+==========================================================
.EQU	_DISP_STA0 =0 				;DISPLAY STATUS BITS
.EQU	_DISP_STA1 =1 
.EQU	_DISP_STA2 =2 
.EQU	_DISP_STA3 =3 
.EQU	_DISP_STA4 =4 
.EQU	_DISP_STA5 =5
.EQU	_DISP_STA6 =6 
.EQU	_DISP_STA7 =7 
.EQU	_DISP_STA0M =0B00000001			;DISPLAY STATUS BITS MASK
.EQU	_DISP_STA1M =0B00000010 
.EQU	_DISP_STA2M =0B00000100
.EQU	_DISP_STA3M =0B00001000
.EQU	_DISP_STA4M =0B00010000
.EQU	_DISP_STA5M =0B00100000
.EQU	_DISP_STA6M =0B01000000
.EQU	_DISP_STA7M =0B10000000
.EQU	_DISP_HARD_COLUNMS = 32			;NUMBER OF COLUNMS SET IN HARDWARE
.EQU	_DISP_HARD_LINES   = 16 		;NUMBER OF LINES SET IN HARDWARE
.EQU	_DISP_SHOW_COLUNMS = 20			;SHOWED COLUNMS 
.EQU	_DISP_SHOW_LINES	  = 16		;SHOWED LINES
.EQU	_NUMCHARS = _DISP_COLUNMS*_DISP_HARD_LINES
.EQU	_DISP_TEXT_ADDRESS  = 0X0000
.EQU	_DISP_GRAPH_ADDRESS = 0X1000
.EQU	_DP_PAGE0	= 0X00
.EQU	_DP_PAGE1 	= 0X80
.EQU	SETCONTROLWORD	= 0X40
.EQU		TEXTHOMEADDR	= 0X00
.EQU		TEXTAREA	= 0X01
.EQU		GRAPHHOMEADDR	= 0X02
.EQU		GRAPHAREA	= 0X03
.EQU	MODESET		= 0X80
.EQU		ORMODE		= 0X00
.EQU		EXORMODE	= 0X01
.EQU		ANDMODE		= 0X03
.EQU		TEXTATTRMODE	= 0X04
.EQU		CGROM		= 0X00
.EQU		CGRAM		= 0X08
.EQU	REGISTERSSETTING= 0X20
.EQU		CURSORPOINTER	= 0X01
.EQU		OFFSETREGISTER	= 0X02
.EQU		ADDRESSPOINTER	= 0X04
.EQU	DATAAUTO	= 0XB0
.EQU		DATAAUTOWRITE	= 0X00
.EQU		DATAAUTOREAD	= 0X01
.EQU		AUTORESET	= 0X02
.EQU	DISPLAYMODE	= 0X90
.EQU		DISPLAYOFF	= 0X00
.EQU		BLINKOFF	= 0X00
.EQU		BLINKON		= 0X01
.EQU		CURSOROFF	= 0X00
.EQU		CURSORON	= 0X02
.EQU		TEXTOFF		= 0X00
.EQU		TEXTON		= 0X04
.EQU		GRAPHOFF	= 0X00
.EQU		GRAPHON		= 0X08
.EQU	BITSETRESET	= 0XF0
.EQU		BITRESET	= 0X00
.EQU		BITSET		= 0X08

