		.include "tn15def.inc"
;		.device attiny15

.def temp	= r16
.def A		= r17
.def B		= r18
.def C		= r19
.def PWM_c	= r20
.def note32duration = r3

.def tone	= r21
.def cnt_0	= r22
.def cnt_1	= r23

.equ SPEAKER_PORT = 4
.equ TNF_ENABLE	= 1
.equ TNF_SET = 2
.equ PLY_ENABLE1	= 4
.equ PLY_TONE	= 5
.equ PLY_STARTED = 8

;.equ BPM = 100			; vinnipux
.equ BPM = 150
.equ defO = 16
.equ defD = 16

.cseg

		.ORG 0
		rjmp _start
		.ORG 4
		rjmp ti1_int
		rjmp ti0_int

		.ORG 10
Notes: ;  A    A#   B      C   C#   D    D#   E      F    F#   G    G#
	.db 0x1C,0x29,0x35,0,0x40,0x4B,0x55,0x5F,0x68,0,0x70,0x78,0x80,0x87,0,0  ; 4
	.db 0x8E,0x94,0x9A,0,0xA0,0xA5,0xAA,0xAF,0xB4,0,0xB7,0xBB,0xBF,0xC3,0,0  ; 5
	.db 0xC6,0xC9,0xCC,0,0xCF,0xD2,0xD4,0xD7,0xD9,0,0xDB,0xDD,0xDF,0xE1,0,0  ; 6
	.db 0xE3,0xE5,0xE6,0,0xE8,0xE9,0xEA,0xEB,0xEC,0,0xED,0xEE,0xEF,0xF0,0,0  ; 7

ti1_int:
	in r25, sreg
	sbrs tone, 4	; if !enabled, quit
	rjmp ti1_quit


	sbrc tone, 7	; if started playing, try to quit
	rjmp ti1_stop

	ori tone, 0x80
	sbrc tone, 5	; if note, not pause, then enable playing
	ori tone, 1

ti1_ok:
	out TCNT1, note32duration
	out sreg, r25
	reti

ti1_stop:
	dec cnt_1	; if still time to wait... return cleanly
	brne ti1_ok

	andi tone, 0x0E	; else stop playing
	out TCNT1, note32duration
	out sreg, r25
	reti

ti1_quit:		; not playing
	andi tone, 0xFE
	out TCNT1, note32duration
	out sreg, r25
	reti

ti0_int:
	in r25, sreg

	sbrs tone, 0	; quit if !enabled
	rjmp ti0_quit

	sbrs tone, 1
	rjmp ti0_next
	CBI PORTB, SPEAKER_PORT
	andi tone, 0xFD
	out TCNT0, cnt_0
	out sreg, r25
	reti

ti0_next:
	sbi portb, SPEAKER_PORT
	ori tone, 2
	out TCNT0, cnt_0
	out sreg, r25
	reti

ti0_quit:
	cbi portb, SPEAKER_PORT
	andi tone, 0xFD
	out TCNT0, cnt_0
	out sreg, r25
	reti

forever: 
	ldi C, 64
f3:	ldi A, 255
f1:	ldi B, 255
f2:	dec B
	brne f2
	dec A
	brne f1
	dec C
	brne f3
	rjmp play_again


_start:
		
; ldi PWM_c, 0
; out OCR1A, PWM_c
; ldi temp, 169
; out OCR1B, temp
; ldi temp, 0x61 ; TCNT1 8-bit PWM @ 100 kHz
; out TCCR1, temp ; output DISCONNECTED


  ldi temp, 0x0D	; clk/
  out TCCR1, temp
  ldi temp, BPM
  mov note32duration, temp

  ldi temp, 0x02	; CLK/8
  out TCCR0, temp
  ldi temp, 0x06
  out TIMSK, temp
  clr tone
  cbi PORTB, SPEAKER_PORT
  sbi DDRB, SPEAKER_PORT		; turn speaker off, port on
 
  cbi PORTB, 1; Set Port B levels
  sbi DDRB, 1; Set Port B directions

  sei

play_again:
  LDi ZH, high(Melody*2)
  LDi ZL, low(Melody*2)

_loop:
	lpm
	mov A, r0
	rcall incZL
	cpi A, '8'
	breq case_8
	cpi A, '4'
	breq case_4
	cpi A, '2'
	breq case_2
	cpi A, '1'
	breq case_1
	cpi A, '3'
	breq case_32
	cpi A, '-'
	breq forever
	cpi A, ','
	breq case_colum

	ldi cnt_1, defD
	mov C, A
	rjmp get_note	; default


case_colum:
	rcall incZL
	rjmp _loop
	
case_8:
	ldi cnt_1, 8
	rjmp skip_case
case_4:
	ldi cnt_1, 16
	rjmp skip_case
case_2:
	ldi cnt_1, 32
	rjmp skip_case
case_32:
	ldi cnt_1, 2
	rcall incZL
	rjmp skip_case
case_1:
	lpm
	mov A, r0
	cpi A, '6'
	breq case_16
	ldi cnt_1, 64
	rjmp skip_case
case_16:
	ldi cnt_1, 4
	rcall incZL

	;
	; here we have note duration
	;
skip_case:
	lpm
	mov C, r0
	rcall incZL
get_note:
	cpi C, 'p'
	breq skip_pause
	subi C, 'a'

	rol C	
	;
	; here we have note, now calculate freq
	;
	lpm
	mov A, r0
	rcall incZL
	clr B
	cpi A, '.'
	brne no3by2
	rcall cnt_3by2
	lpm
	mov A, r0
	rcall incZL
no3by2:
	cpi A, ','
	breq playdef_note
	cpi A, '-'
	breq play_note
	cpi A, '#'
	brne nosharp
	inc C
	lpm
	rcall incZL
	mov A, r0
	cpi A, ','
	breq playdef_note
	cpi A, '-'
	breq play_note
nosharp:
	subi A, '4'
	clc
	rol A	;mul by 16
	rol A
	rol A
	rol A
	add C, A	; limit to 255 notes
	rcall incZL
	rjmp play_note

rjmp _loop

playdef_note:
	ldi B, defO
	add C, B

play_note:
	clr B
	mov temp, ZL
	mov A, Zh
	ldi ZL, low(Notes*2)
	ldi Zh, high(Notes*2)
	clc
	adc Zl, C
	adc Zh, B
	lpm
	mov cnt_0, r0
	mov ZL, temp
	mov Zh, A

	ori tone, 0x20
	ori tone, 0x10

wt_ln12:
	sbrs tone, 7
	rjmp wt_ln12
wt_ln1:
	sbrc tone, 7
	rjmp wt_ln1

	ldi cnt_1, 1

	rjmp _loop

skip_pause:
	lpm
	mov A, r0
	cpi A, '.'
	brne sp_no3by2
	rcall cnt_3by2
	rcall incZL
sp_no3by2:
	rcall incZL

	andi tone, 0xDF
	ori tone, 0x10

wt_lp12:
	sbrs tone, 7
	rjmp wt_lp12
wt_lp1:
	sbrc tone, 7
	rjmp wt_lp1

rjmp _loop


cnt_3by2:
	mov temp, cnt_1
	clc
	rol temp
	add cnt_1, temp
	clc
	ror cnt_1
	ret

incZL:
	inc Zl
	brne doret
	inc Zh
doret:
	ret

Melody:
;vinni pux
;.db "16g5,16f#5,16g5,16f#5,16g5,16f#5,16g5,16f#5,8g5,8e5,4c5,16g5,16f#5,16g5,16f#5,16g5,16f#5,16g5,16f#5,8g5,8d5,16p,8g.4"
;.db ",16f4,16f4,16g#4,16g#4,16c5,16c5,16d#5,16d#5,16g5,16d#5,16d#5,16d#5,16g5,16d#5,8d#5,16c#5,16c#5,16f5,16c#5,16f5,16c#5,8c#5,16g#4,16a#4,16c5,16c#5,16d5,16d#5,16d5,16p,16d5,16d5,8g4,4p,2g5,--"

;dovana
.db "8g4,8p,8g4,16e4,2g4,p,16p,16g4,16p,16g4,16p,16c,16p,8c5,8p,16c,16p,2a.,1p,p,16g4,16p,16g4,16p,16g4,16p,16e4,4g.4,4p,8p,16p,16g4,16p,16g4,16p,32c,16p,32p,8c,8p,2a.5,1p,16a.4,32p,16c5,16p,16d.5,"
.db "16p,32p,16d.,16p,32p,16d,16p,8d,16p,16e.,16p,16d,16p,32p,8c.,a.4,8p,16p,8a4,16c,16p,16d.,16p,32p,16d.,16p,16d.,16p,16d.,16p,32p,16e,16p,32p,16d,16p,32p,2c,p,8p,16c.,32p,8c,16p,8c5,32p,16c.5,"
.db "16p,8c.,8a#4,32p,8g#4,8c.,32p,2g4,8p,16p,16g4,16p,16g.4,16p,32p,16g.4,16p,32p,16g4,16p,16g.4,16p,32p,16f4,16p,32p,16e4,16p,32p,2d4,---"

     

;visa daina. love
;.db "8g,8p,8g,16e,2g,p,16p,32p,32g,16p,32p,32g,16p,16c6"
;.db ",16p,8c6,8p,16c6,16p,2a.,1p,p,16g,16p,16g,16p,16g."
;.db ",32p,16e,g.,p,8p,16p,16g,16p,16g,16p,16c6,16p,8c6,8p"
;.db ",32p,2a.,1p,16a.,32p,16c6,16p,16d.6,16p,32p,16d.6,"
;.db "16p,32p,16d6,16p,8d6,16p,16e.6,16p,16d.6,16p,8c.6,a."
;.db ",8p,16p,8a,16c6,16p,16d.6,16p,32p,16d.6,16p,32p,16d6"
;.db ",16p,16d.6,16p,32p,16e6,16p,32p,16d6,16p,32p,2c6,p,8p,"
;.db "16c.6,32p,8c6,16p,8c6,32p,16c.6,16p,8c.6,8a#,32p,8g#,"
;.db "8c.6,32p,2g,8p,16p,16g,16p,8g,16p,16g.,16p,32p,16g,16p,"
;.db "16g.,16p,32p,16f,16p,32p,16e.,16p,2d,2p,16g,32p,32g,16p,"
;.db "16g.,16p,32e,32p,2g,p,16p,32p,16g,16p,16g,16p,16c6,16p,"
;.db "32p,8c6,16p,32p,2a.,1p,p,8p,32g,32p,16g,16p,32p,16g,16p,"
;.db "16e,16p,32p,2g,p,16g.,32p,16g.,32p,16c6,16p,32p,8c6,16p,"
;.db "32p,16c.6,32p,2a.,1p,8a,16c.6,32p,16d.6,16p,32p,16d.6,"
;.db "16p,32p,16d6,16p,16d.6,16p,32p,16e6,16p,32p,16d6,16p,32p,"
;.db "8c.6,16a,2p,8a,16c.6,32p,8d6,16p,16d.6,16p,32p,16d6,16p,"
;.db "32p,16d6,16p,32p,16e.6,16p,16d.6,16p,2c6,8p,32p,16c.6,16p,"
;.db "32p,16c6,8p,16c6,16p,32p,16c.6,16p,32p,16c6,16p,32p,16c6,"
;.db "16p,32p,16a#,16p,32p,16g#,16p,32p,8c6,16p,2g,8p,16p,32g,"
;.db "32p,32g,32p,8g,16p,16g.,16p,32p,16g,16p,8g,16p,16f.,16p,"
;.db "16e,16p,32p,2d,1p,16e6,32p,32e6,16p,16e6,16p,32p,32d6,32p,--"
;.db "e6,1p,p,16p,32p,16e6,32p,32e6,16p,16e.6,16p,32g6,32p,2e6,"
;.db "2p,p,8p,16p,8e6,32p,2d.6,e6,c.6,1p,32g,32p,16g,16p,16g,16p,"
;.db "32p,32e,32p,2g,p,8p,16p,32p,16g.,32p,16c6,16p,8c6,8p,2a.,1p,"
;.db "p,8p,32g,32p,16g,16p,16g,16p,16e,32p,2g,p,16p,32p,16g,16p,16g,"
;.db "16p,16c6,16p,8c.6,16p,16c.6,32p,2a,1p,p,16a.,16c6,8p,16d.6,16p,"
;.db "16d.6,16p,32p,32d6,32c6,16p,32p,16d.6,16p,16e.6,16p,32p,16d6,"
;.db "16p,8c.6,a.,8p,16p,32p,32a,32p,8c6,16p,16d.6,16p,32p,16d6,16p,"
;.db "32p,16d6,16p,32p,16d6,16p,32p,16e6,8p,32d6,16p,32p,2c6,8p,16p,"
;.db "16c6,16p,32p,16c6,16p,32p,8c6,16p,16c.6,16p,32p,16c6,16p,8c.6,"
;.db "8a#.,8g#,8c6,16p,g.,p,16p,16g,16g,8g,16p,16g.,16p,32p,16g,16p,"
;.db "16g.,16p,32p,8f.,8e,2d,2p,32g,32p,16g,16p,32p,32g,16p,32p,32e,"
;.db "32p,2g,p,8p,16p,32p,16g,16g,32p,32c6,16p,8c6,16p,32p,8c6,32p,2a,"
;.db "1p,32g,32p,16g,16p,16g.,16p,16e,2g,p,16p,32p,16g,16p,8g,16c.6,16p,"
;.db "8c6,16p,32p,8c6,2a,1p,p,8a,8c6,32p,16d.6,16p,8d6,16p,16d.6,32p,8d.6,"
;.db "8e6,16p,8d6,32p,8c6,a.,p,8p,16p,32p,8d6,16p,8d6,16p,16d.6,16p,8d6,"
;.db "32p,8e.6,8d6,2c6,2p,16c.6,16p,32p,16c.6,16p,32p,16c6,16p,8c.6,8a#.,"
;.db "8g#,8c6,16p,32p,2g,8p,32p,32g,8p,16g,16p,32p,16g,8p,16g,16p,32p,8g,"
;.db "32p,16f.,16p,32p,16e,16p,2d.,1p,p,16e6,32p,16e6,32p,16e.6,32p,16d6,"
;.db "32p,e.6,1p,16e6,32p,32e6"
