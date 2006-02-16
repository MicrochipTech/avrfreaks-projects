;*** Definition of Ports **************************************************
;
; Filename		:Ports_2313.asm
;
; Project		;Brick For Programs
; First Target	:Roulette With Fiskal Counter (Rulette_F.asm)
; Device		:AT90S2313
; Created		:2003.03.30
; Last Updated	:
; Version		:1.0
;
; DESCRIPTION	Definition of Ports for display on LED (4-digits) diodes
;			with BCD-to-decimal decoder/driver 74XX145 & driver ULN2003. 
;**************************************************************************

;--------------------------------------------------
; DDRn PORTn  I/O    Pull up   Comment
;  0     0   Input     No    Tri-state (Hi-Z)
;  0     1   Input     Yes   PBn will source current if ext. pulled low
;  1     0   Output    No    Push-Pull Zero Output
;  1     1   Output    No    Push-Pull One Output;
;--------------------------------------------------

;--------------------------
; PORT D
;--------------------------
.equ D_port		= PORTD
.equ D_port_DDR	= DDRD
.equ D_port_PIN	= PIND
;--------------------------
; A_port bits definitions
.equ BIN1pinD		= 0	;Output for DC Display Counter
.equ BIN2pinD		= 1	;Output for DC Display Counter
;.equ pin_SDA		= 2	;Serial Data Line
;.equ pin_SCL		= 3 ;Serial Clock Line

;						76543210
.equ set_D_port_DDR	= 0b00000011
.equ set_D_port		= 0b00000000
;--------------------------	

;--------------------------
; PORT B
;--------------------------
.equ B_port		= PORTB
.equ B_port_DDR	= ddrB
.equ B_port_PIN	= pinB
;--------------------------
.equ pin_chA		= 0     ; Output init 0
.equ pin_chB		= 1     ; Output init 0
.equ pin_chC		= 2     ; Output init 0
.equ pin_chD		= 3     ; Output init 0
.equ pin_chE		= 4     ; Output init 0
.equ pin_chF		= 5     ; Output init 0
.equ pin_chG		= 6     ; Output init 0
.equ pin_chH		= 7     ; Output init 0
;						76543210
.equ set_B_port_DDR	= 0b11111111
.equ set_B_port		= 0b00000000
;--------------------------
;**************************************************************************



