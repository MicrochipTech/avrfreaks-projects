;**********************************
;*                                *
;*    Fast lookup table driven    *
;*     16 bit CRC Routine for     *
;*   ATMEL AVR microcontrollers   *
;*                                *
;*          17-Oct-2002           *
;*                                *
;*       By Anthony Barrett       *
;*                                *
;**********************************
;
; byte to add in [temp1]
; CRC result is returned in crc_low (low byte) & crc_high (high byte).
; Uses regisisters:  crc_A, crc_low, crc_high, crc_B, crc_C, crc_D

;21/06/10
; MODIFIED from the authors original, so now does byte-at-a-time. Byte to add in temp1
; Call init_crc before adding bytes.
; tested at http://www.lammertbies.nl/comm/info/crc-calculation.html
; Uses CRC16, x-modem style. Stewart




init_crc:
	clr	crc_low
	clr	crc_high
	clr	crc_D
	ret


; data to add in temp1
addtoCRC:
	push	zl
	push	zh

	mov	crc_B,temp1
	mov	crc_A,crc_high
	eor	crc_A,crc_B
	ldi	ZL,low(2 * CRC16Tab)
	ldi	ZH,high(2 * CRC16Tab)
	add	ZL,crc_A
	adc	ZH,crc_D
	add	ZL,crc_A
	adc	ZH,crc_D
	lpm	crc_C,z
	mov	crc_high,crc_low
	mov	crc_low,crc_C
	adiw	ZL,1
	lpm	crc_C,z
	eor	crc_high,crc_C

	pop	zh
	pop	zl
	ret


;High speed 16 bit CRC lookup table:
;Polynomial: x^12 + x^5 + 1

CRC16Tab:	.dw	$0000,$1021,$2042,$3063,$4084,$50a5,$60c6,$70e7
	.dw	$8108,$9129,$a14a,$b16b,$c18c,$d1ad,$e1ce,$f1ef
	.dw	$1231,$0210,$3273,$2252,$52b5,$4294,$72f7,$62d6
	.dw	$9339,$8318,$b37b,$a35a,$d3bd,$c39c,$f3ff,$e3de
	.dw	$2462,$3443,$0420,$1401,$64e6,$74c7,$44a4,$5485
	.dw	$a56a,$b54b,$8528,$9509,$e5ee,$f5cf,$c5ac,$d58d
	.dw	$3653,$2672,$1611,$0630,$76d7,$66f6,$5695,$46b4
	.dw	$b75b,$a77a,$9719,$8738,$f7df,$e7fe,$d79d,$c7bc
	.dw	$48c4,$58e5,$6886,$78a7,$0840,$1861,$2802,$3823
	.dw	$c9cc,$d9ed,$e98e,$f9af,$8948,$9969,$a90a,$b92b
	.dw	$5af5,$4ad4,$7ab7,$6a96,$1a71,$0a50,$3a33,$2a12
	.dw	$dbfd,$cbdc,$fbbf,$eb9e,$9b79,$8b58,$bb3b,$ab1a
	.dw	$6ca6,$7c87,$4ce4,$5cc5,$2c22,$3c03,$0c60,$1c41
	.dw	$edae,$fd8f,$cdec,$ddcd,$ad2a,$bd0b,$8d68,$9d49
	.dw	$7e97,$6eb6,$5ed5,$4ef4,$3e13,$2e32,$1e51,$0e70
	.dw	$ff9f,$efbe,$dfdd,$cffc,$bf1b,$af3a,$9f59,$8f78
	.dw	$9188,$81a9,$b1ca,$a1eb,$d10c,$c12d,$f14e,$e16f
	.dw	$1080,$00a1,$30c2,$20e3,$5004,$4025,$7046,$6067
	.dw	$83b9,$9398,$a3fb,$b3da,$c33d,$d31c,$e37f,$f35e
	.dw	$02b1,$1290,$22f3,$32d2,$4235,$5214,$6277,$7256
	.dw	$b5ea,$a5cb,$95a8,$8589,$f56e,$e54f,$d52c,$c50d
	.dw	$34e2,$24c3,$14a0,$0481,$7466,$6447,$5424,$4405
	.dw	$a7db,$b7fa,$8799,$97b8,$e75f,$f77e,$c71d,$d73c
	.dw	$26d3,$36f2,$0691,$16b0,$6657,$7676,$4615,$5634
	.dw	$d94c,$c96d,$f90e,$e92f,$99c8,$89e9,$b98a,$a9ab
	.dw	$5844,$4865,$7806,$6827,$18c0,$08e1,$3882,$28a3
	.dw	$cb7d,$db5c,$eb3f,$fb1e,$8bf9,$9bd8,$abbb,$bb9a
	.dw	$4a75,$5a54,$6a37,$7a16,$0af1,$1ad0,$2ab3,$3a92
	.dw	$fd2e,$ed0f,$dd6c,$cd4d,$bdaa,$ad8b,$9de8,$8dc9
	.dw	$7c26,$6c07,$5c64,$4c45,$3ca2,$2c83,$1ce0,$0cc1
	.dw	$ef1f,$ff3e,$cf5d,$df7c,$af9b,$bfba,$8fd9,$9ff8
	.dw	$6e17,$7e36,$4e55,$5e74,$2e93,$3eb2,$0ed1,$1ef0






