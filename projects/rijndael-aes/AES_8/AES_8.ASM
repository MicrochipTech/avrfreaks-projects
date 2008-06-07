;Author: Tayfun Guemues		(the one and only)
;Date:   July 2001

;The first ever AES_128 implementation for AVR platform


;> MAMP-OS V1.2 for 3232C
;> ======================
;> (July 2001)
;> 
;> New features:
;> - AES-128 encryption corrected (2.9 ms @ 3.579 MHz)
;> - AES-128 decryption added (4.3 ms @ 3.579 MHz)



.include	"m163def.inc"

.equ	keybuf=$400			;mandatory $xy0, in this code y = 0

.cseg




start:	rcall	init
	rcall	aes_e
	rcall	init_k
	rcall	aes_d
	rjmp	start

;50 68 12 A4 5F 08 C8 89 B9 7F 59 80 03 8B 83 59
;00 01 02 03 05 06 07 08 0A 0B 0C 0D 0F 10 11 12

init:	ldi	r16,$50
	mov	r0,r16
	ldi	r16,$68
	mov	r1,r16
	ldi	r16,$12
	mov	r2,r16
	ldi	r16,$a4
	mov	r3,r16
	ldi	r16,$5f
	mov	r4,r16
	ldi	r16,$08
	mov	r5,r16
	ldi	r16,$c8
	mov	r6,r16
	ldi	r16,$89
	mov	r7,r16
	ldi	r16,$b9
	mov	r8,r16
	ldi	r16,$7f
	mov	r9,r16
	ldi	r16,$59
	mov	r10,r16
	ldi	r16,$80
	mov	r11,r16
	ldi	r16,$03
	mov	r12,r16
	ldi	r16,$8b
	mov	r13,r16
	ldi	r16,$83
	mov	r14,r16
	ldi	r16,$59
	mov	r15,r16

;00 01 02 03 05 06 07 08 0A 0B 0C 0D 0F 10 11 12

init_k:	ldi	zh,4
	clr	zl
	ldi	r16,$0
	st	z+,r16
	ldi	r16,$1
	st	z+,r16
	ldi	r16,$2
	st	z+,r16
	ldi	r16,$3
	st	z+,r16
	ldi	r16,$5
	st	z+,r16
	ldi	r16,$6
	st	z+,r16
	ldi	r16,$7
	st	z+,r16
	ldi	r16,$8
	st	z+,r16
	ldi	r16,$a
	st	z+,r16
	ldi	r16,$b
	st	z+,r16
	ldi	r16,$c
	st	z+,r16
	ldi	r16,$d
	st	z+,r16
	ldi	r16,$f
	st	z+,r16
	ldi	r16,$10
	st	z+,r16
	ldi	r16,$11
	st	z+,r16
	ldi	r16,$12
	st	z+,r16
	ret

;==============================================================================
;=                               AES encryption                               =
;==============================================================================

aes_e:	rcall	xkey			;initial XOR

	ldi	r17,$01
	rcall	round_e
	ldi	r17,$02
	rcall	round_e
	ldi	r17,$04
	rcall	round_e
	ldi	r17,$08
	rcall	round_e
	ldi	r17,$10
	rcall	round_e
	ldi	r17,$20
	rcall	round_e
	ldi	r17,$40
	rcall	round_e
	ldi	r17,$80
	rcall	round_e
	ldi	r17,$1B
	rcall	round_e

	ldi	r17,$36
	rcall	rkey_e			;last round
	ldi	zh,high(t_sbox*2)
	rcall	sbox
	rcall	srow_e
	rcall	xkey
	ret

round_e:	rcall	rkey_e			;generate round key
	ldi	zh,high(t_sbox*2)
	rcall	sbox
	rcall	srow_e
	rcall	mixc_e
	rcall	xkey
	ret

rkey_e:	ldi	yl,low(keybuf)		;$400-$40F
	ldi	yh,high(keybuf)

	mov	r16,r0
	ldi	zh,high(t_sbox*2)

	ldd	zl,y+13
	lpm
	eor	r0,r17			;rcon, round constant
	ld	r17,y
	eor	r0,r17
	st	y,r0

	ldd	zl,y+14
	lpm
	ldd	r17,y+1
	eor	r0,r17
	std	y+1,r0

	ldd	zl,y+15
	lpm
	ldd	r17,y+2
	eor	r0,r17
	std	y+2,r0

	ldd	zl,y+12
	lpm
	ldd	r17,y+3
	eor	r0,r17
	std	y+3,r0

	mov	r0,r16

_rky0:	ld	r16,y+
	ldd	r17,y+3
	eor	r16,r17
	std	y+3,r16
	cpi	yl,low(keybuf + 12)	;$0C
	brne	_rky0
	ret

srow_e:	mov	r16,r1			;shift row for encryption
	mov	r1,r5
 	mov	r5,r9
	mov	r9,r13
 	mov	r13,r16
	mov	r16,r2
	mov	r2,r10
 	mov	r10,r16
	mov	r16,r6
	mov	r6,r14
 	mov	r14,r16
	mov	r16,r15
	mov	r15,r11
 	mov	r11,r7
	mov	r7,r3
 	mov	r3,r16
	ret

;10.45  2.9 ms
;15.39  4.3 ms


;r0..r15
;r16..r19
;r20..r23
;y, z
mixc_e:	clr	yh			;e: 25,1,0,0	d: $DF,$68,$EE,$C7

	movw	r21:r20,r1:r0
	movw	r23:r22,r3:r2

	ldi	yl,20			;r0-r3
	rcall	_mxc0
	ldi	yl,4			;r4-r7
	rcall	_mxc0
	ldi	yl,8			;r8-r11
	rcall	_mxc0
	ldi	yl,12			;r12-r15
	rcall	_mxc0

	movw	r1:r0,r21:r20
	movw	r3:r2,r23:r22

	ret

_mxc0:	clr	r16
	ld	zl,y
	tst	zl
	breq	_p__0
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,25
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	mov	r16,r0
_p__0:	ldd	zl,y+1
	tst	zl
	breq	_p__1
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,1
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r16,r0
_p__1:	ldd	zl,y+2
	tst	zl
	breq	_q__0
	eor	r16,zl
_q__0:	ldd	zl,y+3
	tst	zl
	breq	_q__1
	eor	r16,zl

_q__1:	clr	r17
	ldd	zl,y+1
	tst	zl
	breq	_p__2
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,25
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	mov	r17,r0
_p__2:	ldd	zl,y+2
	tst	zl
	breq	_p__3
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,1
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r17,r0
_p__3:	ldd	zl,y+3
	tst	zl
	breq	_q__2
	eor	r17,zl
_q__2:	ld	zl,y
	tst	zl
	breq	_q__3
	eor	r17,zl

_q__3:	clr	r18
	ldd	zl,y+2
	tst	zl
	breq	_p__4
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,25
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	mov	r18,r0
_p__4:	ldd	zl,y+3
	tst	zl
	breq	_p__5
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,1
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r18,r0
_p__5:	ld	zl,y
	tst	zl
	breq	_q__4
	eor	r18,zl
_q__4:	ldd	zl,y+1
	tst	zl
	breq	_q__5
	eor	r18,zl

_q__5:	clr	r19
	ldd	zl,y+3
	tst	zl
	breq	_p__6
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,25
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	mov	r19,r0
_p__6:	ld	zl,y
	tst	zl
	breq	_p__7
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,1
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r19,r0
_p__7:	ldd	zl,y+1
	tst	zl
	breq	_q__6
	eor	r19,zl
_q__6:	ldd	zl,y+2
	tst	zl
	breq	_q__7
	eor	r19,zl

_q__7:	st	y+,r16
	st	y+,r17
	st	y+,r18
	st	y,r19
	ret

;==============================================================================
;=                               AES decryption                               =
;==============================================================================

aes_d:	rcall	lastrk

	rcall	xkey
	ldi	zh,high(t_sboxi*2)
	rcall	sbox
	rcall	srow_d

	ldi	r17,$36
	rcall	round_d
	ldi	r17,$1B
	rcall	round_d
	ldi	r17,$80
	rcall	round_d
	ldi	r17,$40
	rcall	round_d
	ldi	r17,$20
	rcall	round_d
	ldi	r17,$10
	rcall	round_d
	ldi	r17,$08
	rcall	round_d
	ldi	r17,$04
	rcall	round_d
	ldi	r17,$02
	rcall	round_d
	ldi	r17,$01
	rcall	rkey_d

	rcall	xkey
	ret

round_d:	rcall	rkey_d			;generate round key
	rcall	xkey
	rcall	mixc_d
	ldi	zh,high(t_sboxi*2)
	rcall	sbox
	rcall	srow_d
	ret

rkey_d:	ldi	yl,low(keybuf + 12)	;$400-$40F, $40C
	ldi	yh,high(keybuf + 12)

	push	r17			;xor rcon with first key byte

_rky1:	ld	r16,-y
	ldd	r17,y+4
	eor	r16,r17
	std	y+4,r16
	cpi	yl,low(keybuf)		;$00
	brne	_rky1

	pop	r17

	mov	r16,r0
	ldi	zh,high(t_sbox*2)

	ldd	zl,y+13
	lpm
	eor	r0,r17			;rcon, round constant
	ld	r17,y
	eor	r0,r17
	st	y,r0

	ldd	zl,y+14
	lpm
	ldd	r17,y+1
	eor	r0,r17
	std	y+1,r0

	ldd	zl,y+15
	lpm
	ldd	r17,y+2
	eor	r0,r17
	std	y+2,r0

	ldd	zl,y+12
	lpm
	ldd	r17,y+3
	eor	r0,r17
	std	y+3,r0

	mov	r0,r16
	ret

mixc_d:	clr	yh			;e: 25,1,0,0	d: $DF,$68,$EE,$C7

	movw	r21:r20,r1:r0
	movw	r23:r22,r3:r2

	ldi	yl,20			;r0-r3
	rcall	_mxc1
	ldi	yl,4			;r4-r7
	rcall	_mxc1
	ldi	yl,8			;r8-r11
	rcall	_mxc1
	ldi	yl,12			;r12-r15
	rcall	_mxc1

	movw	r1:r0,r21:r20
	movw	r3:r2,r23:r22

	ret

_mxc1:	clr	r16
	ld	zl,y
	tst	zl
	breq	_pd_0
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$DF
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	mov	r16,r0
_pd_0:	ldd	zl,y+1
	tst	zl
	breq	_pd_1
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$68
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r16,r0
_pd_1:	ldd	zl,y+2
	tst	zl
	breq	_qd_0
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$EE
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r16,r0
_qd_0:	ldd	zl,y+3
	tst	zl
	breq	_qd_1
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$C7
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r16,r0

_qd_1:	clr	r17
	ldd	zl,y+1
	tst	zl
	breq	_pd_2
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$DF
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	mov	r17,r0
_pd_2:	ldd	zl,y+2
	tst	zl
	breq	_pd_3
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$68
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r17,r0
_pd_3:	ldd	zl,y+3
	tst	zl
	breq	_qd_2
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$EE
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r17,r0
_qd_2:	ld	zl,y
	tst	zl
	breq	_qd_3
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$C7
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r17,r0

_qd_3:	clr	r18
	ldd	zl,y+2
	tst	zl
	breq	_pd_4
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$DF
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	mov	r18,r0
_pd_4:	ldd	zl,y+3
	tst	zl
	breq	_pd_5
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$68
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r18,r0
_pd_5:	ld	zl,y
	tst	zl
	breq	_qd_4
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$EE
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r18,r0
_qd_4:	ldd	zl,y+1
	tst	zl
	breq	_qd_5
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$C7
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r18,r0

_qd_5:	clr	r19
	ldd	zl,y+3
	tst	zl
	breq	_pd_6
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$DF
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	mov	r19,r0
_pd_6:	ld	zl,y
	tst	zl
	breq	_pd_7
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$68
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r19,r0
_pd_7:	ldd	zl,y+1
	tst	zl
	breq	_qd_6
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$EE
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r19,r0
_qd_6:	ldd	zl,y+2
	tst	zl
	breq	_qd_7
	ldi	zh,high(t_mul0*2)
	lpm
	ldi	zl,$C7
	add	zl,r0
	adc	zl,yh			;add carry because of modulo % 255
	ldi	zh,high(t_mul1*2)
	lpm
	eor	r19,r0

_qd_7:	st	y+,r16
	st	y+,r17
	st	y+,r18
	st	y,r19
	ret

srow_d: 	mov	r16,r13			;shift row for decryption
	mov	r13,r9
 	mov	r9,r5
	mov	r5,r1
	mov	r1,r16
 	mov	r16,r10
	mov	r10,r2
	mov	r2,r16
 	mov	r16,r14
	mov	r14,r6
	mov	r6,r16
 	mov	r16,r3
	mov	r3,r7
 	mov	r7,r11
	mov	r11,r15
	mov	r15,r16
	ret

;==============================================================================
;=                        calculates last round key                           =
;==============================================================================

lastrk:	ldi	r17,$01
	rcall	rkey_e
	ldi	r17,$02
	rcall	rkey_e
	ldi	r17,$04
	rcall	rkey_e
	ldi	r17,$08
	rcall	rkey_e
	ldi	r17,$10
	rcall	rkey_e
	ldi	r17,$20
	rcall	rkey_e
	ldi	r17,$40
	rcall	rkey_e
	ldi	r17,$80
	rcall	rkey_e
	ldi	r17,$1B
	rcall	rkey_e
	ldi	r17,$36
	rcall	rkey_e
	ret


sbox:	mov	r16,r0
	mov	zl,r1
	lpm
	mov	r1,r0
	mov	zl,r2
	lpm
	mov	r2,r0
	mov	zl,r3
	lpm
	mov	r3,r0
	mov	zl,r4
	lpm
	mov	r4,r0
	mov	zl,r5
	lpm
	mov	r5,r0
	mov	zl,r6
	lpm
	mov	r6,r0
	mov	zl,r7
	lpm
	mov	r7,r0
	mov	zl,r8
	lpm
	mov	r8,r0
	mov	zl,r9
	lpm
	mov	r9,r0
	mov	zl,r10
	lpm
	mov	r10,r0
	mov	zl,r11
	lpm
	mov	r11,r0
	mov	zl,r12
	lpm
	mov	r12,r0
	mov	zl,r13
	lpm
	mov	r13,r0
	mov	zl,r14
	lpm
	mov	r14,r0
	mov	zl,r15
	lpm
	mov	r15,r0
	mov	zl,r16
	lpm
	ret

;xkey:	ldi	zl,low(keybuf)
;	ldi	zh,high(keybuf)
;	clr	yl			;r0-r15
;	clr	yh
;_xky0:	ld	r16,z+
;	ld	r17,y
;	eor	r16,r17
;	st	y+,r16
;	cpi	yl,$10
;	brne	_xky0
;	ret

xkey:	ldi	zl,low(keybuf)
	ldi	zh,high(keybuf)
	ld	r16,z+
	eor	r0,r16
	ld	r16,z+
	eor	r1,r16
	ld	r16,z+
	eor	r2,r16
	ld	r16,z+
	eor	r3,r16
	ld	r16,z+
	eor	r4,r16
	ld	r16,z+
	eor	r5,r16
	ld	r16,z+
	eor	r6,r16
	ld	r16,z+
	eor	r7,r16
	ld	r16,z+
	eor	r8,r16
	ld	r16,z+
	eor	r9,r16
	ld	r16,z+
	eor	r10,r16
	ld	r16,z+
	eor	r11,r16
	ld	r16,z+
	eor	r12,r16
	ld	r16,z+
	eor	r13,r16
	ld	r16,z+
	eor	r14,r16
	ld	r16,z+
	eor	r15,r16
	ret


.org $1000				;mandatory $xx00

t_sbox:	.DB	$63,$7C,$77,$7B,$F2,$6B,$6F,$C5,$30,$01,$67,$2B,$FE,$D7,$AB,$76
	.DB	$CA,$82,$C9,$7D,$FA,$59,$47,$F0,$AD,$D4,$A2,$AF,$9C,$A4,$72,$C0
	.DB	$B7,$FD,$93,$26,$36,$3F,$F7,$CC,$34,$A5,$E5,$F1,$71,$D8,$31,$15
	.DB	$04,$C7,$23,$C3,$18,$96,$05,$9A,$07,$12,$80,$E2,$EB,$27,$B2,$75
	.DB	$09,$83,$2C,$1A,$1B,$6E,$5A,$A0,$52,$3B,$D6,$B3,$29,$E3,$2F,$84
	.DB	$53,$D1,$00,$ED,$20,$FC,$B1,$5B,$6A,$CB,$BE,$39,$4A,$4C,$58,$CF
	.DB	$D0,$EF,$AA,$FB,$43,$4D,$33,$85,$45,$F9,$02,$7F,$50,$3C,$9F,$A8
	.DB	$51,$A3,$40,$8F,$92,$9D,$38,$F5,$BC,$B6,$DA,$21,$10,$FF,$F3,$D2
	.DB	$CD,$0C,$13,$EC,$5F,$97,$44,$17,$C4,$A7,$7E,$3D,$64,$5D,$19,$73
	.DB	$60,$81,$4F,$DC,$22,$2A,$90,$88,$46,$EE,$B8,$14,$DE,$5E,$0B,$DB
	.DB	$E0,$32,$3A,$0A,$49,$06,$24,$5C,$C2,$D3,$AC,$62,$91,$95,$E4,$79
	.DB	$E7,$C8,$37,$6D,$8D,$D5,$4E,$A9,$6C,$56,$F4,$EA,$65,$7A,$AE,$08
	.DB	$BA,$78,$25,$2E,$1C,$A6,$B4,$C6,$E8,$DD,$74,$1F,$4B,$BD,$8B,$8A
	.DB	$70,$3E,$B5,$66,$48,$03,$F6,$0E,$61,$35,$57,$B9,$86,$C1,$1D,$9E
	.DB	$E1,$F8,$98,$11,$69,$D9,$8E,$94,$9B,$1E,$87,$E9,$CE,$55,$28,$DF
	.DB	$8C,$A1,$89,$0D,$BF,$E6,$42,$68,$41,$99,$2D,$0F,$B0,$54,$BB,$16

t_sboxi:	.DB	 82,  9,106,213, 48, 54,165, 56,191, 64,163,158,129,243,215,251
	.DB	124,227, 57,130,155, 47,255,135, 52,142, 67, 68,196,222,233,203
	.DB	 84,123,148, 50,166,194, 35, 61,238, 76,149, 11, 66,250,195, 78
	.DB	  8, 46,161,102, 40,217, 36,178,118, 91,162, 73,109,139,209, 37
	.DB	114,248,246,100,134,104,152, 22,212,164, 92,204, 93,101,182,146
	.DB	108,112, 72, 80,253,237,185,218, 94, 21, 70, 87,167,141,157,132
	.DB	144,216,171,  0,140,188,211, 10,247,228, 88,  5,184,179, 69,  6
	.DB	208, 44, 30,143,202, 63, 15,  2,193,175,189,  3,  1, 19,138,107
	.DB	 58,145, 17, 65, 79,103,220,234,151,242,207,206,240,180,230,115
	.DB	150,172,116, 34,231,173, 53,133,226,249, 55,232, 28,117,223,110
	.DB	 71,241, 26,113, 29, 41,197,137,111,183, 98, 14,170, 24,190, 27
	.DB	252, 86, 62, 75,198,210,121, 32,154,219,192,254,120,205, 90,244
	.DB	 31,221,168, 51,136,  7,199, 49,177, 18, 16, 89, 39,128,236, 95
	.DB	 96, 81,127,169, 25,181, 74, 13, 45,229,122,159,147,201,156,239
	.DB	160,224, 59, 77,174, 42,245,176,200,235,187, 60,131, 83,153, 97
	.DB	 23, 43,  4,126,186,119,214, 38,225,105, 20, 99, 85, 33, 12,125

t_mul0:	.DB	$00,$00,$19,$01,$32,$02,$1A,$C6,$4B,$C7,$1B,$68,$33,$EE,$DF,$03
	.DB	$64,$04,$E0,$0E,$34,$8D,$81,$EF,$4C,$71,$08,$C8,$F8,$69,$1C,$C1
	.DB	$7D,$C2,$1D,$B5,$F9,$B9,$27,$6A,$4D,$E4,$A6,$72,$9A,$C9,$09,$78
	.DB	$65,$2F,$8A,$05,$21,$0F,$E1,$24,$12,$F0,$82,$45,$35,$93,$DA,$8E
	.DB	$96,$8F,$DB,$BD,$36,$D0,$CE,$94,$13,$5C,$D2,$F1,$40,$46,$83,$38
	.DB	$66,$DD,$FD,$30,$BF,$06,$8B,$62,$B3,$25,$E2,$98,$22,$88,$91,$10
	.DB	$7E,$6E,$48,$C3,$A3,$B6,$1E,$42,$3A,$6B,$28,$54,$FA,$85,$3D,$BA
	.DB	$2B,$79,$0A,$15,$9B,$9F,$5E,$CA,$4E,$D4,$AC,$E5,$F3,$73,$A7,$57
	.DB	$AF,$58,$A8,$50,$F4,$EA,$D6,$74,$4F,$AE,$E9,$D5,$E7,$E6,$AD,$E8
	.DB	$2C,$D7,$75,$7A,$EB,$16,$0B,$F5,$59,$CB,$5F,$B0,$9C,$A9,$51,$A0
	.DB	$7F,$0C,$F6,$6F,$17,$C4,$49,$EC,$D8,$43,$1F,$2D,$A4,$76,$7B,$B7
	.DB	$CC,$BB,$3E,$5A,$FB,$60,$B1,$86,$3B,$52,$A1,$6C,$AA,$55,$29,$9D
	.DB	$97,$B2,$87,$90,$61,$BE,$DC,$FC,$BC,$95,$CF,$CD,$37,$3F,$5B,$D1
	.DB	$53,$39,$84,$3C,$41,$A2,$6D,$47,$14,$2A,$9E,$5D,$56,$F2,$D3,$AB
	.DB	$44,$11,$92,$D9,$23,$20,$2E,$89,$B4,$7C,$B8,$26,$77,$99,$E3,$A5
	.DB	$67,$4A,$ED,$DE,$C5,$31,$FE,$18,$0D,$63,$8C,$80,$C0,$F7,$70,$07

t_mul1:	.DB	$01,$03,$05,$0F,$11,$33,$55,$FF,$1A,$2E,$72,$96,$A1,$F8,$13,$35
	.DB	$5F,$E1,$38,$48,$D8,$73,$95,$A4,$F7,$02,$06,$0A,$1E,$22,$66,$AA
	.DB	$E5,$34,$5C,$E4,$37,$59,$EB,$26,$6A,$BE,$D9,$70,$90,$AB,$E6,$31
	.DB	$53,$F5,$04,$0C,$14,$3C,$44,$CC,$4F,$D1,$68,$B8,$D3,$6E,$B2,$CD
	.DB	$4C,$D4,$67,$A9,$E0,$3B,$4D,$D7,$62,$A6,$F1,$08,$18,$28,$78,$88
	.DB	$83,$9E,$B9,$D0,$6B,$BD,$DC,$7F,$81,$98,$B3,$CE,$49,$DB,$76,$9A
	.DB	$B5,$C4,$57,$F9,$10,$30,$50,$F0,$0B,$1D,$27,$69,$BB,$D6,$61,$A3
	.DB	$FE,$19,$2B,$7D,$87,$92,$AD,$EC,$2F,$71,$93,$AE,$E9,$20,$60,$A0
	.DB	$FB,$16,$3A,$4E,$D2,$6D,$B7,$C2,$5D,$E7,$32,$56,$FA,$15,$3F,$41
	.DB	$C3,$5E,$E2,$3D,$47,$C9,$40,$C0,$5B,$ED,$2C,$74,$9C,$BF,$DA,$75
	.DB	$9F,$BA,$D5,$64,$AC,$EF,$2A,$7E,$82,$9D,$BC,$DF,$7A,$8E,$89,$80
	.DB	$9B,$B6,$C1,$58,$E8,$23,$65,$AF,$EA,$25,$6F,$B1,$C8,$43,$C5,$54
	.DB	$FC,$1F,$21,$63,$A5,$F4,$07,$09,$1B,$2D,$77,$99,$B0,$CB,$46,$CA
	.DB	$45,$CF,$4A,$DE,$79,$8B,$86,$91,$A8,$E3,$3E,$42,$C6,$51,$F3,$0E
	.DB	$12,$36,$5A,$EE,$29,$7B,$8D,$8C,$8F,$8A,$85,$94,$A7,$F2,$0D,$17
	.DB	$39,$4B,$DD,$7C,$84,$97,$A2,$FD,$1C,$24,$6C,$B4,$C7,$52,$F6,$01


;rkey:		;z, y , r16-r17
;mixc:		;z, y , r16-r23
;srow:		;r16

;sbox:		;z, (y), r16
;xkey:		;z, (y), r16, (r17)
