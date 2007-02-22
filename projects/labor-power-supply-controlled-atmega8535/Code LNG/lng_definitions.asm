;***********************************************
; Alimentation laboratoire       avec Atmega8535
;
; définitions LaborNetzGerät
;
; hirschmann vitalis
; 19.04.2006
;***********************************************

.nolist			; do not include in listing
.set	clock	= 8000000

.def	char	= r0	; character (ASCII)
.def	_sreg 	= r1	; saves the status during interrupts
.def	_u 	= r2	; saves working reg u during interrupt
.def	u	= r3	; scratch register (macros, routines)

.def	seuil	= r4
.def	e1	= r5

.equ	c	= 8
.def	c0	= r8	; 8-byte register c
.def	c1	= r9
.def	c2	= r10
.def	c3	= r11

.equ	d	= 12	; 4-byte register d (overlapping with c)
.def	d0	= r12
.def	d1	= r13
.def	d2	= r14
.def	d3	= r15

.def	w	= r16	; working register for macros
.def	_w	= r17	; working register for interrupts

.equ	a	= 18
.def	a0	= r18	; 4-byte register a
.def	a1	= r19
.def	a2	= r20
.def	a3	= r21

.equ	b	= 22
.def	b0	= r22	; 4-byte register b
.def	b1	= r23
.def	b2	= r24
.def	b3	= r25	

.equ	x	= 26	; pointer x
.equ	y	= 28	; pointer y
.equ	z	= 30	; pointer z

; === ASCII codes
.equ	BEL	=0x07	; bell
.equ	HT	=0x09	; horizontal tab
.equ	TAB	=0x09	; tab
.equ	LF	=0x0a	; line feed
.equ	VT	=0x0b	; vertical tab
.equ	FF	=0x0c	; form feed
.equ	CR	=0x0d	; carriage return
.equ	SPACE	=0x20	; space code
.equ	DEL	=0x7f	; delete
.equ	BS	=0x08	; back space

; === module I2C/1Wire ===
.equ	SCL	= 1	; I2C serial clock
.equ	SDA	= 3	; I2C serial data
.equ	DQ	= 5	; Dallas 1Wire

;-----------------------------------------------
; PORTC:
.equ	LED1 = 1	; LED on / off
.equ	LED2 = 0
.equ	UP1  = 2	; up / down bouton
.equ	DN1  = 3
.equ	UP2  = 4
.equ	DN2  = 5
.equ	PGM1 = 7	; Program bouton
.equ	PGM2 = 6

; PORTD:
.equ	ON1  = 2
.equ	ON2  = 3
.equ  OUT2 = 5

; PORTA:
.equ  Relon = 0
.equ  Rel05 = 2
.equ  Rel12 = 1
;-----------------------------------------------
.equ  anz_prog1 = 3                             ; nombre de progs à choix
.equ  anz_prog2 = 3
.equ  T = 6
;-----------------------------------------------
; servo-moteur:
.equ	  deg_0 = 50
.equ	 deg_45 = 74
.equ	 deg_90 = 98
.equ	deg_135 = 124
.equ	deg_180 = 149
;-----------------------------------------------
.list





