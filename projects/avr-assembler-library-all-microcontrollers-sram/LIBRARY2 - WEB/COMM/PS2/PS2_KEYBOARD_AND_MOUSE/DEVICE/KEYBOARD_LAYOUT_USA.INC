; 	 ____________________________________________________________________________________________
;	|////////////////////////////////////////////////////////////////////////////////////////////|
;	|///|											|////|
;	|///| DRIVE	 : PS2 KEYBOARD AND MOUSE DEVICE					|////|
;	|///| VERSAO	 : 2.0									|////|
;	|///| PROGRAMADOR: JOÃO DARTAGNAN ANTUNES OLIVEIRA				   	|////|
;	|///| DATA 	 : 10 de Julho de 2007 (19:39)						|////|
;	|///| DESCRIÇÃO	 : Emula Keyboard e Mouse PS2 						|////|
;	|///| OBJETIVO:	   Emular interface PS2 processar todos os comandos e enviar codigos    |////|
;	|///| 		   de teclado e rotinas de emulação de translação de ASCII codes para	|////|
;	|///|	           keyboard scan codes.							|////|
;	|///| RESOURCES	 : This drive use TIMER0 and 4 I/O bits					|////|
;	|///|											|////|
;	|///| OBS:	   This software is inteded to use in AVR with SRAM			|////|
;	|///|		   PS2 LINE RECQUIRE 10K RESISTORS AT CLOCK AND DATA LINES		|////|
;	|///|___________________________________________________________________________________|////|
;-------|////////////////////////////////////////////////////////////////////////////////////////////|
;	>--------------------------------------------------------------------------------------------<
;-------|											     |
;	|   		P S 2  K E Y B O A R D  L A Y O U T   U S A 				     |
;	| 											     |
;       \--------------------------------------------------------------------------------------------/
;
; POSITION ASSINMENT
;
;/------------\
;| KEY NAME   |
;| KEY NAME   |
;| KEY NUMBER |
;\------------/
;
;/-----\   /-----+-----+-----+-----\   /-----+-----+-----+-----\   /-----+-----+-----+-----\  /------+------+------\ 
;|     |   |     |     |     |     |   |     |     |     |     |   |     |     |     |     |  |PRINT |SCROLL|      |
;| ESC |   |  F1 |  F2 |  F3 |  F4 |   |  F5 |  F6 |  F7 |  F8 |   |  F9 | F10 | F11 | F12 |  |SCREEN| LOCK |PAUSE |
;| 110 |   | 112 | 113 | 114 | 115 |   | 116 | 117 | 118 | 119 |   | 120 | 121 | 122 | 123 |  | 124  | 125  | 126  |
;\-----/   \-----+-----+-----+-----/   \-----+-----+-----+-----/   \-----+-----+-----+-----/  \------+------+------/

;/-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----------\  /------+------+------\  /-----+-----+-----+-----\
;|  ~  |  !  |  @  |  #  |  $  |  %  |  ^  |  &  |  *  |  (  |  )  |  _  |  +  |           |  |      |      | PAGE |  | NUM |     |     |     | 
;|  `  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  0  |  -  |  =  | BACKSPACE |  |INSERT| HOME |  UP  |  | LOCK|  /  |  *  |  -  |
;|  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10  | 11  | 12  | 13  |   15      |  |  75  |  80  |  85  |  |  90 | 95  | 100 | 105 |
;>-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--------<  >------+------+------<  >-----+-----+-----+-----<
;|        |     |     |     |     |     |     |     |     |     |     |  {  |  }  |  |     |  |      |      | PAGE |  |     |     |     |     |
;|   TAB  |  Q  |  W  |  E  |  R  |  T  |  Y  |  U  |  I  |  O  |  P  |  [  |  ]  |  \     |  |DELETE| END  | DOWN |  |  7  |  8  |  9  |     |
;|   16   | 17  | 18  | 19  | 20  | 21  | 22  | 23  | 24  | 25  | 26  | 27  | 28  |  29    |  |  76  |  81  |  86  |  |  91 | 96  | 101 |     |
;>--------+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+-+---+--------<  \------+------+------/  >-----+-----+-----+  +  |
;|          |     |     |     |     |     |     |     |     |     |  :  |  "  |            |                          |     |     |     |     |
;|CAPS LOCK |  A  |  S  |  D  |  F  |  G  |  H  |  J  |  K  |  L  |  ;  |  '  |   ENTER    |  			      |  4  |  5  |  6  |     |
;|   30     | 31  | 32  | 33  | 34  | 35  | 36  | 37  | 38  | 39  | 40  | 41  |    43      |                          |  92 | 97  | 102 | 106 |
;>----------+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+------------<         /------\         >-----+-----+-----+-----<
;|             |     |     |     |     |     |     |     |  <  |  >  |  ?  |               |         |      |         |     |     |     |     |
;|    SHIFT    |  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |     SHIFT     |         |  UP  |         |  1  |  2  |  3  |  E  |
;|     44      | 46  | 47  | 48  | 49  | 50  | 51  | 52  | 53  | 54  | 55  |      57       |         |  83  |         |  93 | 98  | 103 |  N  |
;>--------+----+-+-----+---+-----+-----+-----+-----+-----+----++----++-----+------+--------<  /------+------+------\  >-----+-----+-----+  T  |
;|        |      |     |                                      |     |      |      |        |  |      |      |      |  |           |     |  E  |
;|  CTRL  | LWIN | ALT |                SPACE                 | ALT | RWIN | MENU |  CTRL  |  | LEFT | DOWN |RIGHT |  |     0     |  .  |  R  |
;|   58   |      |  60 |                  61                  | 62  |      |      |  64    |  |  79  |  84  |  89  |  |     99    | 104 | 108 |
;\--------+------+-----+--------------------------------------+-----+------+------+--------/  \------+------+------/  \-----------+-----+-----/

#ifndef __PS2_KEYBOARD_LAYOUT_USA__

	#define __PS2_KEYBOARD_LAYOUT_USA__

	.message "+-------------------------+"
	.message "|                         |"
	.message "| PS2 KEYBOARD LAYOUT USA |"
	.message "|                         |"
	.message "+-------------------------+"

	;/////////////////////////////////
	;// PC KEYBOARD KEY NAMES	//
	;/////////////////////////////////

	.EQU	_KEY_ALT_LEFT		= 0x80+0x01
	.EQU	_KEY_ALT_RIGHT		= 0x80+0x02
	.EQU	_KEY_CTRL_LEFT		= 0x80+0x04
	.EQU	_KEY_CTRL_RIGHT		= 0x80+0x08
	.EQU	_KEY_SHIFT_LEFT		= 0x80+0x10
	.EQU	_KEY_SHIFT_RIGHT	= 0x80+0x20
	.EQU	_KEY_ENTER		= 0x80+0x40
	.EQU	_KEY_ESCAP		= 0x80+0x41
	.EQU	_KEY_TAB		= 0x80+0x42
	.EQU	_KEY_HOME		= 0x80+0x43
	.EQU	_KEY_END		= 0x80+0x44
	.EQU	_KEY_INSERT		= 0x80+0x45
	.EQU	_KEY_DELETE		= 0x80+0x46
	.EQU	_KEY_PAGEUP		= 0x80+0x47
	.EQU	_KEY_PAGEDOWN		= 0x80+0x48
	.EQU	_KEY_UP			= 0x80+0x49
	.EQU	_KEY_DOWN		= 0x80+0x4a
	.EQU	_KEY_LEFT		= 0x80+0x4b
	.EQU	_KEY_RIGHT		= 0x80+0x4c
	.EQU	_KEY_CAPS_LOCK		= 0x80+0x4d
	.EQU	_KEY_NUM_LOCK		= 0x80+0x4e
	.EQU	_KEY_SCROLL_LOCK	= 0x80+0x4f
	.EQU	_KEY_F1			= 0x80+0x50
	.EQU	_KEY_F2			= 0x80+0x51
	.EQU	_KEY_F3			= 0x80+0x52
	.EQU	_KEY_F4			= 0x80+0x53
	.EQU	_KEY_F5			= 0x80+0x54
	.EQU	_KEY_F6			= 0x80+0x55
	.EQU	_KEY_F7			= 0x80+0x56
	.EQU	_KEY_F8			= 0x80+0x57
	.EQU	_KEY_F9			= 0x80+0x58
	.EQU	_KEY_F10		= 0x80+0x59
	.EQU	_KEY_F11		= 0x80+0x5a
	.EQU	_KEY_F12		= 0x80+0x5b
	.EQU	_KEY_BACKSPACE		= 0X80+0x5c
	.EQU	_KEY_PAUSEBREAK		= 0X80+0x5d
	.EQU	_KEY_PAUSE		= 0X80+0x5d
	.EQU	_KEY_POWER		= 0X80+0X5e
	.EQU	_KEY_SLEEP		= 0X80+0X5f
	.EQU	_KEY_WAKEUP		= 0X80+0X60
	.EQU	_KEY_APP		= 0X80+0X61
	.EQU	_KEY_LWIN		= 0X80+0X62
	.EQU	_KEY_RWIN		= 0X80+0X63

	;/////////////////////////////////////////////////
	;// SCAN CODE TABLE				//
	;//						//
	;// SCAN 1   - XT 				//
	;// SCAN 2,3 - AT				//
	;//						//
	;/////////////////////////////////////////////////
	_PS2_KEYBOARD_LAYOUT_USA_SINGLE_CODE:
	;+----------------------------------------------
	;| SCAN 1
	;|
	;| MAKE		BREAK
	;| ----		-----
	;| CODE 		CODE OR 0X80
	;|
	;| SCAN 2
	;|
	;| MAKE		BREAK
	;| ----		-----
	;| CODE		0XF0,CODE
	;|
	;|		Normal		Shifted	Scan1	Scan2&3	Complete Make/Break Scan 1		Complete Make/Break Scan 2,3
	;|		------		-------	------	-------	--------------------------		----------------------------
	;<START>
		.DB	' ',		' ',	0X39,	0X29	;0X39/0XB9				0X29/0XF0,0X29
		.DB	'0',		')',	0X0B,	0X45	;0X0B/0X8B				0X45/0XF0,0X45
		.DB	'1',		'!',	0X02,	0X16	;0X02/0X82				0X16/0XF0,0X16
		.DB	'2',		'@',	0X03,	0X1E	;0X03/0X83				0X1E/0XF0,0X1E
		.DB	'3',		'#',	0X04,	0X26	;0X04/0X84				0X26/0XF0,0X26
		.DB	'4',		'$',	0X05,	0X25	;0X05/0X85				0X25/0XF0,0X25
		.DB	'5',		'%',	0X06,	0X2E	;0X06/0X86				0X2E/0XF0,0X2E
		.DB	'6',		'^',	0X07,	0X36	;0X07/0X87				0X36/0XF0,0X36
		.DB	'7',		'&',	0X08,	0X3D	;0X08/0X88				0X3D/0XF0,0X3D
		.DB	'8',		'*',	0X09,	0X3E	;0X09/0X89				0X3E/0XF0,0X3E
		.DB	'9',		'(',	0X0A,	0X46	;0X0A/0X8A				0X46/0XF0,0X46
		.DB	'A',		'a',	0X1E,	0X1C	;0X1E/0X9E				0X1C/0XF0,0X1C
		.DB	'B',		'b',	0X30,	0X32	;0X30/0XB0				0X32/0XF0,0X32
		.DB	'C',		'c',	0X2E,	0X21	;0X2E/0XAE				0X21/0XF0,0X21
		.DB	'D',		'd',	0X20,	0X23	;0X20/0XA0				0X23/0XF0,0X23
		.DB	'E',		'e',	0X12,	0X24	;0X12/0X92				0X24/0XF0,0X24
		.DB	'F',		'f',	0X21,	0X2B	;0X21/0XA1				0X2B/0XF0,0X2B
		.DB	'G',		'g',	0X22,	0X34	;0X22/0XA2				0X34/0XF0,0X34
		.DB	'H',		'h',	0X23,	0X33	;0X23/0XA3				0X33/0XF0,0X33
		.DB	'I',		'i',	0X17,	0X43	;0X17/0X97				0X43/0XF0,0X43
		.DB	'J',		'j',	0X24,	0X3B	;0X24/0XA4				0X3B/0XF0,0X3B
		.DB	'K',		'k',	0X25,	0X42	;0X25/0XA5				0X42/0XF0,0X42
		.DB	'L',		'l',	0X26,	0X4B	;0X26/0XA6				0X4B/0XF0,0X4B
		.DB	'M',		'm',	0X32,	0X3A	;0X32/0XB2				0X3A/0XF0,0X3A
		.DB	'N',		'n',	0X31,	0X31	;0X31/0XB1				0X31/0XF0,0X31
		.DB	'O',		'o',	0X18,	0X44	;0X18/0X98				0X44/0XF0,0X44
		.DB	'P',		'p',	0X19,	0X4D	;0X19/0X99				0X4D/0XF0,0X4D
		.DB	'Q',		'q',	0X10,	0X15	;0X10/0X90				0X15/0XF0,0X15
		.DB	'R',		'r',	0X13,	0X2D	;0X13/0X93				0X2D/0XF0,0X2D
		.DB	'S',		's',	0X1F,	0X1B	;0X1F/0X9F				0X1B/0XF0,0X1B
		.DB	'T',		't',	0X14,	0X2C	;0X14/0X94				0X2C/0XF0,0X2C
		.DB	'U',		'u',	0X16,	0X3C	;0X16/0X96				0X3C/0XF0,0X3C
		.DB	'V',		'v',	0X2F,	0X2A	;0X2F/0XAF				0X2A/0XF0,0X2A
		.DB	'W',		'w',	0X11,	0X1D	;0X11/0X91				0X1D/0XF0,0X1D
		.DB	'X',		'x',	0X2D,	0X22	;0X2D/0XAD				0X22/0XF0,0X22
		.DB	'Y',		'y',	0X15,	0X35	;0X15/0X95				0X35/0XF0,0X35
		.DB	'Z',		'z',	0X2C,	0X1A	;0X2C/0XAC				0X1A/0XF0,0X1A
		.DB	'`',		'~',	0X29,	0X0E	;0X29/0XA9				0X0E/0XF0,0X0E
		.DB	',',		'<',	0X33,	0X41	;0X33/0XB3				0X41/0XF0,0X41
		.DB	'.',		'>',	0X34,	0X49	;0X34/0XB4				0X49/0XF0,0X49
		.DB	'/',		'?',	0X35,	0X4A	;0X35/0XB5				0X4A/0XF0,0X4A
		.DB	';',		':',	0X27,	0X4C	;0X27/0XA7				0X4C/0XF0,0X4C
		.DB	''',		'"',	0X28,	0X52	;0X28/0XA8				0X52/0XF0,0X52
		.DB	'[',		'{',	0X1A,	0X54	;0X1A/0X9A				0X54/0XF0,0X54
		.DB	']',		'}',	0X1B,	0X5B	;0X1B/0X9B				0X5B/0XF0,0X5B
		.DB	'-',		'_',	0X0C,	0X4E	;0X0C/0X8C				0X4E/0XF0,0X4E
		.DB	'=',		'+',	0X0D,	0X55	;0X0D/0X8D				0X55/0XF0,0X55
		.DB	0X5C,		'|',	0X56,	0X61	;0X56/0XD6				0X61/0XF0,0X61 '\'
		.DB	_KEY_ESCAP	,0x00,	0X01,	0X76	;0X01/0X81				0X76/0XF0,0X76
		.DB	_KEY_BACKSPACE	,0x00,	0X0E,	0X66	;0X0E/0X8E				0X66/0XF0,0X66		
		.DB	_KEY_TAB	,0x00,	0X0F,	0X0D	;0X0F/0X8F				0X0D/0XF0,0X0D
		.DB	_KEY_F1		,0x00,	0X3B,	0X05	;0X3B/0XBB				0X05/0XF0,0X05
		.DB	_KEY_F2		,0x00,	0X3C,	0X06	;0X3C/0XBC				0X06/0XF0,0X06
		.DB	_KEY_F3		,0x00,	0X3D,	0X04	;0X3D/0XBD				0X04/0XF0,0X04
		.DB	_KEY_F4		,0x00,	0X3E,	0X0C	;0X3E/0XBE				0X0C/0XF0,0X0C
		.DB	_KEY_F5		,0x00,	0X3F,	0X03	;0X3F/0XBF				0X03/0XF0,0X03
		.DB	_KEY_F6		,0x00,	0X40,	0X0B	;0X40/0XC0				0X0B/0XF0,0X0B
		.DB	_KEY_F7		,0x00,	0X41,	0X83	;0X41/0XC1				0X83/0XF0,0X83
		.DB	_KEY_F8		,0x00,	0X42,	0X0A	;0X42/0XC2				0X0A/0XF0,0X0A
		.DB	_KEY_F9		,0x00,	0X43,	0X01	;0X43/0XC3				0X01/0XF0,0X01
		.DB	_KEY_F10	,0x00,	0X44,	0X09	;0X44/0XC4				0X09/0XF0,0X09
		.DB	_KEY_F11	,0x00,	0X45,	0X78	;0X57/0XD7				0X78/0XF0,0X78
		.DB	_KEY_F12	,0x00,	0X46,	0X07	;0X58/0XD8				0X07/0XF0,0X07		
		.DB	_KEY_CAPS_LOCK	,0x00,	0X3A,	0X58	;0X3A/0XBA				0X58/0XF0,0X58
		.DB	_KEY_NUM_LOCK	,0x00,	0X3A,	0X77	;0X45/0XC5				0X77/0XF0,0X77
		.DB	_KEY_SCROLL_LOCK,0x00,	0X3A,	0X7E	;0X46/0XC6				0X7E/0XF0,0X7E
		.DB	_KEY_PAUSEBREAK	,0x00,	0XE1,	0XE1	;0XE1/0XE1				0XE1/0XF0,0XE1
		.DB	_KEY_SHIFT_LEFT ,0x00,	0X2A,	0X12	;0X2A/0XAA				0X12/0XF0,0X12
		.DB	_KEY_SHIFT_RIGHT,0x00,	0X36,	0X59	;0X36/0XB6				0X59/0XF0,0X59
		.DB	_KEY_CTRL_LEFT	,0x00,	0X1D,	0X14	;0X1D/0X9D				0X14/0XF0,0X14
		.DB	_KEY_ALT_LEFT	,0x00,	0X38,	0X11	;0X38/0XB8				0X11/0XF0,0X11
	;+----------------------------------------------
	;| SCAN 1
	;|
	;| MAKE		BREAK
	;| ----		-----
	;| CODE 		CODE OR 0X80
	;|
	;| SCAN 2
	;|
	;| MAKE		BREAK
	;| ----		-----
	;| CODE		0XF0,CODE
	;|
	;|		Normal		Shifted	Scan1	Scan2&3	Complete Make/Break Scan 1		Complete Make/Break Scan 2,3
	;|		------	-------	------	-------	--------------------------		----------------------------
	_PS2_KEYBOARD_LAYOUT_USA_EXTENDED_CODE:
		.DB	_KEY_UP		,0x00,	0X48,	0X75	;0X48/0XC8				0X75/0XF0,0X75
		.DB	_KEY_DOWN	,0x00,	0X50,	0X72	;0X50/0XD0				0X72/0XF0,0X72
		.DB	_KEY_LEFT	,0x00,	0X4B,	0X6B	;0X4B/0XCB				0X6B/0XF0,0X6B
		.DB	_KEY_RIGHT	,0x00,	0X4D,	0X74	;0X4D/0XCD				0X74/0XF0,0X74
		.DB	_KEY_PAGEUP	,0x00,	0X49,	0X7D	;0X49/0XC9				0X7D/0XF0,0X7D
		.DB	_KEY_PAGEDOWN	,0x00,	0X51,	0X7A	;0X51/0XD1				0X7A/0XF0,0X7A
		.DB	_KEY_HOME	,0x00,	0X47,	0X6C	;0X47/0XC7				0X6C/0XF0,0X6C
		.DB	_KEY_END	,0x00,	0X4F,	0X69	;0X4F/0XCF        			0X69/0XF0,0X69
		.DB	_KEY_INSERT	,0x00,	0X52,	0X70	;0X52/0XD2				0X70/0XF0,0X70
		.DB	_KEY_DELETE	,0x00,	0X53,	0X71	;0X53/0XD3				0X71/0XF0,0X71
		.DB	_KEY_POWER	,0x00,	0X5E,	0X37	;0XE0,0X5E/0XEO,0XDE			0XE0,0X37/0XE0,0XF0,0X37
		.DB	_KEY_SLEEP	,0x00,	0X5F,	0X3F	;0XE0,0X5F/0XE0,0XDF			0XE0,0X3F/0XE0,0XF0,0X3F
		.DB	_KEY_WAKEUP	,0x00,	0X63,	0X5E	;0XE0,0X63/0XE0,0XE3			0XE0,0X5E/0XE0,0XF0,0X5E
		.DB	_KEY_LWIN	,0x00,	0X5B,	0X1F	;0XE0,0X5B/0XE0,0X5B			0XE0,0X1F/0XE0,0XF0,0X1F
		.DB	_KEY_RWIN	,0x00,	0X5C,	0X27	;0XE0,0X5C/0XE0,0XDC			0XE0,0X27/0XE0,0XF0,0X27
		.DB	_KEY_APP	,0x00,	0X5D,	0X2F	;0XE0,0X5D/0XE0,0XDD			0XE0,0X2F/OXEO,0XF0,0X2F
		.DB	_KEY_ENTER	,0x00,	0X1C,	0X5A	;0XE0,0X1C/0XE0,0X9C			0XE0,0X5A/0XE0,0XF0,0X5A
		.DB	_KEY_CTRL_RIGHT	,0x00,	0X1D,	0X14	;0XE0,0X1D/0XE0,0X9D			0XE0,0X14/0XE0,0XE0,0XF0,0X14
		.DB	_KEY_ALT_RIGHT	,0x00,	0X38,	0X11	;0XE0,0X38/0XE0,0XB8			0XE0,0X11,0XE0/0XF0,0X11
	;<END>
#endif
