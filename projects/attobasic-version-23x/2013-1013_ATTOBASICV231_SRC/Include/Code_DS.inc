.if DSI
;*****************************************************
dsdcommand:		;Send byte (top of stack) via DS port
	rcall	Get1stParm			;get the last parameter
	mov		inchar,U
	clc							;clear carry flag
	Gosub	SendDSByte	
	ret
;	
;***********************************************	
dsccommand:		;Send byte (top of stack) via DS port
	rcall	Get1stParm			;get the last parameter
	mov		inchar,U
	sec							;set carry flag
	Gosub	SendDSByte	
	ret
;	
;***********************************************	
dsrcommand:		;Receive a byte vai DS port	
	Gosub	ReceiveDSByte
	mov		U,inchar
	Gosub	pushU
	ret
	
;***********************************************	
;	SEND A BYTE
; Send carry bit as first bit, then send inchar bit-by-bit. inchar destroyed.
SendDSByte:
	rcall	WDEnable			;enable WD

	CLRB	GPIOR0,WDTIRQ		;insure WDTIRQ flag is clear

	LOAD	temp,DS_PORT-1		
	andi	temp,~( 1<<DS_SDA | \
					1<<DS_SDD )
	STORE	DS_PORT-1,temp		;set DS port bits as inputs

.if DS_PUPS
	rcall	DS_PUPSon			;enable port pull-ups
.endif

	ldi		H,$09				;load bit counter
S1:
;	sleep						;sleep till IRQ
;	nop							;benefit of the sleep instruction
	SKBC	GPIOR0,WDTIRQ		;skip next if WDTIRQ flag clear
	rjmp	S4					;watchdog timeout, exit
	SKBS	DS_PORT-2,DS_SDA	;Wait for ATT line to go high =--SEND A BIT
	rjmp	S1

	CLRB	DS_PORT-1,DS_SDD	;DAT pin to input
.if DS_PUPS
	SETB	DS_PORT,DS_SDD		;DAT pin pullup
.endif
	brcs	S2	
	SETB	DS_PORT-1,DS_SDD	;DAT pin to output
	CLRB	DS_PORT,DS_SDD		;DAT pin low
S2:	
	SKBC	GPIOR0,WDTIRQ		;skip next if WDTIRQ flag clear
	rjmp	S4					;watchdog timeout, exit
	SETB	DS_PORT-1,DS_SDA	;Pin to output
	CLRB	DS_PORT,DS_SDA		;ATT pin low
	GoSub	Delay1mS			;delay 1mS so other chip can see ATT line is low

	;Release Attention line for a peak
	CLRB	DS_PORT-1,DS_SDA	;ATT pin to input
.if DS_PUPS
	SETB	DS_PORT,DS_SDA		;ATT pin pullup
.endif
	GoSub	Delay1mS			;delay 1mS so other chip can see ATT line is low

	SKBC	DS_PORT-2,DS_SDA	;If ATT line isn't low, go back and put it low again, else continue
	rjmp	S2
	
	brcs	S2a					;Invert data line
	CLRB	DS_PORT-1,DS_SDD	;DAT pin to input
.if DS_PUPS
	SETB	DS_PORT,DS_SDD		;DAT pin pullup 
.endif
	rjmp	S3
S2a:	
	CLRB	DS_PORT,DS_SDD		;DAT pin low
	SETB	DS_PORT-1,DS_SDD	;DAT pin output
S3:
;	sleep						;sleep till IRQ
;	nop							;benefit of the sleep instruction
	SKBC	GPIOR0,WDTIRQ		;skip next if WDTIRQ flag clear
	rjmp	S4					;watchdog timeout, exit
	SKBS	DS_PORT-2,DS_SDA	;Wait for Attention line to go high
	rjmp	S3
	
	CLRB	DS_PORT-1,DS_SDD	;DAT pin input
.if DS_PUPS
	SETB	DS_PORT,DS_SDD		;DAT pin pullup
.endif
	;finished sending a bit

	rol		inchar				;Shift inchar through carry

	dec		H
	brne	S1					;Continue until all bits sent
	rjmp	SendDSByteOut

S4:
	SetZPtr	(2*emessage18)		;Z = message start
	GoSub	sendlromline		;inform user of error

SendDSByteOut:
	GoSub	WDDisable			;disable watchdog timer
	ret

;***********************************************	
;	Recieve a byte
ReceiveDSByte: ;Get a byte into inchar, and "start bit" into carry bit.
	rcall	WDEnable			;enable WD

	LOAD	temp,DS_PORT-1		
	andi	temp,~( 1<<DS_SDA | \
					1<<DS_SDD )
	STORE	DS_PORT-1,temp		;set DS port bits as inputs

.if DS_PUPS
	rcall	DS_PUPSon			;enable port pull-ups
.endif

	ldi		H,$09				;load bit counter

W3:					;Get a bit from the input into the carry
	SKBC	GPIOR0,WDTIRQ		;skip next if WDTIRQ flag clear
	rjmp	W4					;watchdog timeout, exit
	SKBC	DS_PORT-2,DS_SDA	;Wait for Attention line to go low
	rjmp	w3
	
	clc				;Latch DS_SDD into carry bit
	SKBS	DS_PORT-2,DS_SDD
	rjmp	NotaOne
	sec	
	
	CLRB	DS_PORT,DS_SDA		;ATT pin low
	SETB	DS_PORT-1, DS_SDA	;ATT pin output
	
W1:					;Wait for data line to go low
	SKBC	GPIOR0,WDTIRQ		;skip next if WDTIRQ flag clear
	rjmp	W4					;watchdog timeout, exit
	SKBC	DS_PORT-2,DS_SDD
	rjmp	W1
	rjmp	RelesaseDS_SDAAndGo
		
NotaOne:
	CLRB	DS_PORT,DS_SDA		;ATT pin low
	SETB	DS_PORT-1, DS_SDA	;ATT pin output
	
W2:					;Wait for data line to go high
	SKBC	GPIOR0,WDTIRQ		;skip next if WDTIRQ flag clear
	rjmp	W4					;watchdog timeout, exit
	SKBS	DS_PORT-2,DS_SDD
	rjmp	W2

RelesaseDS_SDAAndGo:			;Release attention line
	CLRB	DS_PORT-1, DS_SDA	;ATT pin input
.if DS_PUPS
 	SETB	DS_PORT,DS_SDA		;ATT pin PUP
.endif
	;DONE RECEIVING BIT
	rol		inchar		;Shift inchar through carry
	dec		H
	brne	W3			;Continue until all bits received
	rjmp	ReceiveDSByteOut

W4:
	SetZPtr	(2*emessage18)		;Z = message start
	GoSub	sendlromline		;inform user of error
	ser		U					;set U to 0xFF - error

ReceiveDSByteOut:
	GoSub	WDDisable			;disable watchdog timer
	ret
;
;***********************************************	
DS_PUPSon:
	LOAD	temp,DS_PORT-2
	ori		temp,(  1<<DS_SDA | \
					1<<DS_SDD )
	STORE	DS_PORT,temp		;set pullups on DS port bits
	ret
;
;***********************************************	
WDEnable:
	;enable watchdog timer for 512mS	
	STOREI	WDTCSR,(1<<WDCE|1<<WDE)
	STOREI	WDTCSR,((1<<WDIE)|(0<<WDP3)|(6<<WDP0)|(0<<WDE))
	wdr							;reset watchdog
	ret
;
;***********************************************	
;shortdelay:
	;19444 cycles [1+(1+((80*3)+3)*80)+4]
;	ldi		inbytel,settletime	;1 cycles
;outerloop:
;	ldi		inbyteh,settletime	;1 cycles
;innerloop:
;	dec		inbyteh				;1 cycles
;	brne	innerloop			;1/2 cycles
;	dec		inbytel				;1 cycles
;	brne	outerloop			;1/2 cycles
;	ret							;4/5 cycles
;
.endif
