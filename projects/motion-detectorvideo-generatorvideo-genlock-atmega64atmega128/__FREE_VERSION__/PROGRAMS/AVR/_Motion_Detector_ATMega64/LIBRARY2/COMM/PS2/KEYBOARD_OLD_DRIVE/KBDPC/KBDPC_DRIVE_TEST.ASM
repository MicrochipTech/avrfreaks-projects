;+------------------------------------------------------+
;| PS2 COMMUNICATION BETWEEN KEYBOARD AND PC		|
;| KEYBOARD-->PC					|
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATA:19/02/2002					|
;>------------------------------------------------------<
;| P R O G R A M  F I L E 				|
;+------------------------------------------------------+

;+------------------------------------------------------+
;| A V R  C L O C K 					|
;+------------------------------------------------------+

.EQU	_AVR_CLOCK	= 4000000

;+------------------------------------------------------+
;| S R A M  I N I T I A L I Z A T I O N 		|
;+------------------------------------------------------+

.EQU	_SRAM_BOOT_TYPE	= 1					;USE 0 NOT CLEAR SRAM DURING BOOT
								;USE 1 CLEAR DURING BOOT
;+------------------------------------------------------+
;| I N C L U D E   F I L E S				|
;+------------------------------------------------------+

;+----------------------------
;| Global Definitions
;+----------------------------
.include	"\Dart\Avr\Library\Defs\GlobDefs.Inc"		;Global Definitions
.include 	"\Dart\Avr\Library\Defs\2313Defi.Inc"		;Microcontroler Definitions
;+----------------------------
;| Time Routines
;+----------------------------
.include	"\Dart\Avr\Library\Time\Time.inc"		;Time Routines
;+----------------------------
;| PS2 Routines
;+----------------------------
.include	"\Dart\Avr\Library\COMM\PS2\KEYBOARD\KBDPC\KBDPC.inc"	;PS2 Keyboard --> PC routines

;+------------------------------------------------------+
;| M A I N   I N I T I A L I Z A T I O N  		|
;+------------------------------------------------------+------------------------------------------------------------------
_MAIN_INIT:

;+----------------------------
;| PS2 Keyboard initialization
;+----------------------------

.EQU	_PS2KBD_CLOCK_PORT_OUTPUT	= PORTB
.EQU	_PS2KBD_CLOCK_PORT_DIR		= DDRB
.EQU	_PS2KBD_CLOCK_PORT_ÌNPUT	= PINB
.EQU	_PS2KBD_CLOCK_BIT		= 0
													
.EQU	_PS2KBD_DATA_PORT_OUTPUT	= PORTB
.EQU	_PS2KBD_DATA_PORT_DIR		= DDRB
.EQU	_PS2KBD_DATA_PORT_ÌNPUT		= PINB
.EQU	_PS2KBD_DATA_BIT		= 1

;+--------------------------------------+
;| M A I N  T E S T  P R O G R A M	|	
;+--------------------------------------+

_MAIN:
	rjmp	_MAIN	