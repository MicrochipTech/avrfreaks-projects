;*************************************************************************
;*									 *
;*				AVR universal Bootloader		 *
;*									 *
;*			Author: Peter Dannegger				 *
;*									 *
;*************************************************************************
.nolist
.include "fastload.h"
;.listmac
.list
	.org	BootStart
init:
	cli
	ldi	a0, low (RamEnd)
	out	SPL, a0
.ifdef SPH
	ldi	a0, high(RamEnd)
	out	SPH, a0
.endif
	clr	zerol
	clr	zeroh

.if WDTRIGGER
.include "watchdog.inc"
.endif

	IOPortInit
.if CRC
	ldi	polynoml, 0x01
	ldi	polynomh, 0xA0
.endif
.if FLASHEND > 0x7FFF
	ldi	a0, byte3(2*Password)
	out	RAMPZ, a0
.endif
;-------------------------------------------------------------------------
.include "abaud.inc"
.include "password.inc"
;-------------------------------------------------------------------------
connected:
	ldi	a0, CONNECT		; password recognized
.if ONEWIRE
	rcall	syncputchar		; avoid message garbage
.else
	rcall	putchar
.endif
	rcall	getchar
	brne	connected			; until COMMAND received
;-------------------------------------------------------------------------
;				Receive commands
;-------------------------------------------------------------------------
;00	get bootloader revision
;01	get buffer size
;02	get target signature
;03	get user flash size
;04	program flash
;05	start application
;06	check crc
;07	verify flash
;-------------------------------------------------------------------------
main_ok:
	ldi	a0, SUCCESS
_cex1:
	rcall	putchar
_cex2:
	rcall	getchar
	brne	_cex2			; ignore until COMMAND
_cex3:
	rcall	getchar
	breq	_cex3			; ignore further COMMAND
.if FLASHEND > 0x7FFF
	ldi	zx, 0
.endif
	movw	zh:zl, zeroh:zerol      ; Z = 0x0000,
	clt				; T = 0 (for program, verify)
	cpi	a0, 4
	brcs	sendmessage		; 0 ... 3
	breq	program			; 4
	cpi	a0, 6
	brcc	_cex5
;-------------------------------------------------------------------------
timeout:				; 5
	out	STX_DDR, zerol
	out	STX_PORT, zerol
.if SRX_PORT != STX_PORT
	out	SRX_PORT, zerol
.endif
.ifdef BootFuse
.if FlashEnd > 0x0FFF
	jmp	0x0000
.else
	rjmp	0x0000			; run application
.endif
.else
	rjmp	BootStart-1		; run user application
.endif
;-------------------------------------------------------------------------
_cex5:
.if CRC
	brne	_cex6
.include "checkcrc.inc"			; 6
_cex6:
.endif
;-------------------------------------------------------------------------
.if VERIFY
	cpi	a0, 7
	brne	_cex7
.include "verify.inc"			; 7
_cex7:
.endif
;-------------------------------------------------------------------------
	ldi	a0, BADCOMMAND		; no match found
	rjmp	_cex1
main_error:
	ldi	a0, FAIL
	rjmp	_cex1
;-------------------------------------------------------------------------
SendMessage:
.include "message.inc"			; 0 ... 3
;-------------------------------------------------------------------------
Program:
.ifdef BootFuse
.if FLASHEND > 0x7FFF
.include "progmegx.inc"
.else
.include "progmega.inc"		; mega with bootstart fuse set
.endif
.else
.include "progtiny.inc"		; tiny, mega without RWW section
.endif
;-------------------------------------------------------------------------
.include "uart.inc"
;-------------------------------------------------------------------------
Password:
	.db	"Peda", 0, 0		; 'a' was recognized by ABAUD
.list
Messages:
	.db \
	3, high(Version), low(Version), \
	3, high(2*BufferSize), low(2*BufferSize), \
	4, SIGNATURE_000, SIGNATURE_001, SIGNATURE_002, \
	4, byte3(UserFlash), byte2(UserFlash), byte1(UserFlash)
;-------------------------------------------------------------------------
