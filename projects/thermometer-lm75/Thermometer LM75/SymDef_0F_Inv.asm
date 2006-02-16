;**** Definition of symbols for the display *******************************
;
; Filename		:SymDef_0F_Inv.asm
;
; Project		;Brick For Programs
; First Target	:Roulette With Fiskal Counter (Roulette_F.asm)
; Device		:All
; Created		:2002.10.11
; Last Updated	:
; Version		:1.0
;
; DESCRIPTION	:Definition of symbols for the display from 0 to F.
;				With the inverting driver ULN2003.
;				Entrance value and target symbol it is located in the r0. 
;				
;**************************************************************************

;--------------------------------------------------
; DIFINITION DISPLAY SIMBOLS
;                 hgfedcba
.equ ch_0     = 0b00111111 ; "0"
.equ ch_1     = 0b00000110 ; "1"
.equ ch_2     = 0b01011011 ; "2"
.equ ch_3     = 0b01001111 ; "3"
.equ ch_4     = 0b01100110 ; "4"
.equ ch_5     = 0b01101101 ; "5"
.equ ch_6     = 0b01111101 ; "6"
.equ ch_7     = 0b00000111 ; "7"

.equ ch_8     = 0b01111111 ; "8"
.equ ch_9     = 0b01101111 ; "9"
.equ ch_A     = 0b01110111 ; "A"
.equ ch_b     = 0b01111100 ; "b"
.equ ch_C     = 0b00111001 ; "C"
.equ ch_d     = 0b01011110 ; "d"
.equ ch_E     = 0b01111001 ; "E"
.equ ch_F     = 0b01110001 ; "F"
;end of table
.equ ch_g     = 0b01101111 ; "g"
.equ ch_H     = 0b01110110 ; "H"
.equ ch_i     = 0b00010000 ; "i"
.equ ch_J     = 0b00001110 ; "J"
.equ ch_L     = 0b00111000 ; "L"
.equ ch_n     = 0b01010100 ; "n"
.equ ch_o     = 0b01011100 ; "o"
.equ ch_P     = 0b01110011 ; "P"

.equ ch_r     = 0b01010000 ; "r"
.equ ch_S     = 0b01101101 ; "S"
.equ ch_t     = 0b01111000 ; "t"
.equ ch_U     = 0b00111110 ; "U"
.equ ch_minus = 0b01000000
.equ ch_blank = 0b00000000

;--------------------------------------------------
sym_tab:
	.db ch_0,ch_1,ch_2,ch_3,ch_4,ch_5,ch_6,ch_7
	.db ch_8,ch_9,ch_a,ch_b,ch_c,ch_d,ch_e,ch_f
	.db	ch_g,ch_H,ch_i,ch_j,ch_L,ch_n,ch_o,ch_P
	.db	ch_r,ch_S,ch_t,ch_U,ch_minus,ch_blank
;--------------------------------------------------

;--------------------------------------------------
; INPUT:	r0 digit 0...FF
; OUTPUT:	r0 symbol for display
ATTACH_SYM:
  ldi   zh,high(sym_tab*2)
  ldi   zl,low(sym_tab*2)
  add   zl,r0
  clr   r0
  adc   zh,r0
  lpm
  ret
;-----------------------------------------------------

