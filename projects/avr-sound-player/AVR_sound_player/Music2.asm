.include "8535def.inc"
.include "notedef.inc"

.def	temp	=r16
.def	null	=r17
.def	tonel	=r20
.def	toneh	=r21
.def	dura	=r22
.def	tone	=r23
.def	notepl	=r24
.def	noteph	=r25
.equ    TEMPO	=185	; Lower is faster


	rjmp	RESET

.org OC2addr
	rjmp	OC2int

RESET:
	ldi	temp,high(RAMEND)
	out	SPH,temp
	ldi	temp,low(RAMEND)
	out	SPL,temp

	ldi	temp,0xFF
	out	DDRB,temp
	ldi	null,0

	; OC1A output pin
	ldi	temp,(1<<PD5)
	out	DDRD,temp

	; Count T/C1 at 4 MHz, clear on OC1A
	ldi	temp,(1<<COM1A0)
	out	TCCR1A,temp

	; Count T/C2 at 0.25 ms / tick
	ldi	temp,(7<<CS20)+(1<<CTC2)
	out	TCCR2,temp
	ldi	temp,(1<<OCIE2)
	out	TIMSK,temp
	ldi	temp,TEMPO
	out	OCR2,temp
	sei

	ldi	toneh,0x40
	ldi	tonel,0x00
	ldi	dura,1
	ldi	tone,0

	ldi	noteph,high(NOTES*2)
	ldi	notepl,low(NOTES*2)

	ldi	temp,0xaa
	out	PORTB,temp
	
end:
	rjmp	end

	; Load new tone value
OC2int:
	dec	dura
	breq	newnote
	reti
newnote:	
	mov	ZL,notepl
	mov	ZH,noteph
	lpm
	mov	tone,r0
	adiw	ZL,1
	lpm
	mov	dura,r0
	adiw	notepl,2

	; If end of song: reset pointer, stop timer, wait 24 ticks
	cpi	tone,$00
	brne	noendsong
	ldi	noteph,high(NOTES*2)
	ldi	notepl,low(NOTES*2)
	ldi	temp,(0<<CS10)+(1<<CTC1)
	out	TCCR1B,temp
	ldi	dura,24
	rjmp	endnote

noendsong:
	; If $FF then pause
	cpi	tone,$FF
	brne	playtone
	ldi	temp,(0<<CS10)+(1<<CTC1)
	out	TCCR1B,temp
	rjmp	endnote

playtone:
	; Read frequency for halftone
	ldi	ZH,high(HALFTONETBL*2)
	ldi	ZL,low(HALFTONETBL*2)
	mov	temp,tone
	andi	temp,0x0f
	lsl	temp
	add	ZL,temp
	adc	ZH,null

	; Read OCR value for halftone from table
	lpm
	mov	tonel,r0
	ori	ZL,1
	lpm
	mov	toneh,r0

	; Shift to correct octave
	mov	temp,tone
;	andi	temp,0xf0
octloop:
	subi	temp,0x10
	brcs	goon4
	lsr	toneh
	ror	tonel
	rjmp	octloop
goon4:

	; Set up new frequency and start counter (in case it was stopped)
	out	OCR1AH,toneh
	out	OCR1AL,tonel
	ldi	temp,(1<<CS10)+(1<<CTC1)
	out	TCCR1B,temp
endnote:
	reti

HALFTONETBL:
.dw	61156,57724,54484,51426,48540,45815
.dw	43244,40817,38526,36364,34323,32396

NOTES:
.include "bumtest.mus"

; Example of notation: "In the hall of the mountain king" theme (Grieg)
;.db	c2,8, d2,8, e2,8, f2,8,  g2,8, e2,8, g2,16,  a2,8, f2,8, a2,16,  g2,8, e2,8, g2,16,
;.db	c2,8, d2,8, e2,8, f2,8,  g2,8, e2,8, g2,16,  a2,8, f2,8, a2,16,  g2,32
;.db	$00, $00
