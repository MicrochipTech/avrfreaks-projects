;*** Definition of Memory Map 2313 ****************************************
;
; Filename		:MemoryMap_2313.asm
;
; Project		;Brick For Programs
; First Target	:Roulette With Fiskal Counter (Rulette_F.asm)
; Device		:AT90S2313
; Created		:2002.10.17
; Last Updated	:
; Version		:1.0
;
; DESCRIPTION	Definition of a location of the data in SRAM & EEPROM
;**************************************************************************


;******************************************
; SRAM area: $60, RAMEND = $DF	(128 bytes)
;******************************************
;--------------------------
.dseg
.org $60

;--------------------------
S_SYM0:		.byte 1	; "Symbol, .......X" from Display
S_SYM1:		.byte 1	; "Symbol, ......X."
S_SYM2:		.byte 1	; "Symbol, .....X.."
S_SYM3:		.byte 1	; "Symbol, ....X..."
