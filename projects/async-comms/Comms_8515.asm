;comms_8515.asm
;
;Comms routines for AVR using hardware UART.
;Print message routine using null-terminated string in PM added.
;Similar technique may be used for PM table lookup.
;RxD pin 10
;TxD pin 11
     	.include        "8515def.inc"


;register definitions

	.def	temp=r17
	.def	byte=r18


;equates

	.equ	CLOCK=4000000	;clock frequency in Hz
	.equ	BAUD=19200
	.equ	CR=$0D
	.equ	LF=$0A

    .cseg
	.org	0



reset:

	;initialise stack pointer
	ldi		temp,low(RAMEND)
	out		spl,temp
	ldi		temp,high(RAMEND)
	out		sph,temp
	sbi		ucr,txen		;enable UART transmitter
	sbi     ucr,rxen		;enable UART receiver
	sbi		ucr,txb8		;2 stop bits
	ldi     temp,CLOCK/(BAUD*16)-1 	;set baud rate
	out     ubrr,temp
	rcall	new_line
    rcall   print_msg
	rcall	new_line



main:
	rcall		new_line
	rcall		prompt
	rcall		gxb
	brcs		main1
	rcall		txb
	rjmp		main2
main1:
	ldi			byte,'?'
	rcall		serout
main2:
	rjmp            main                    ;do forever

print_msg:
		clr		r31
		ldi		r30,msg*2	;point to start of message
print1:
		lpm					;byte in r0
		inc		r30			;bump pointer
		mov		byte,r0		;'byte' used by serial routine
		cpi		byte,0		;done?
		breq	print2		;yes, jump
		rcall	serout		;no, print it
		rjmp	print1		;go round again
print2:
        ret


;*************************************************************************************
;*
;* Module name: prompt
;* Description: send prompt character (>) to host PC
;* Called by:
;* Calls:
;* Input parameters: none
;* Output parameters: none
;* Register usage: none
;* Stack usage: 2 bytes plus call and return
;*
;*************************************************************************************

prompt:
	ldi		byte,'>'
	rcall		serout
	rcall		space
	ret


;*************************************************************************************
;*
;* Module name: new_line
;* Description: send new line to host PC
;* Called by:
;* Calls:
;* Input parameters: none
;* Output parameters: none
;* Register usage: none
;* Stack usage: 2 bytes plus call and return
;*
;*************************************************************************************

new_line:
	ldi		byte,CR
	rcall		serout
	ldi		byte,LF
	rcall		serout
	ret


;*************************************************************************************
;*
;* Module name: space
;* Description: send space character to host PC
;* Called by:
;* Calls:
;* Input parameters: none
;* Output parameters: none
;* Register usage: none
;* Stack usage: 2 bytes plus call and return
;*
;*************************************************************************************

space:
	ldi	byte,' '
	rcall	serout
	ret



;serial output routine - output byte to UART

serout:
	out             udr,byte         ;load UART data register
serout1:
	sbis            usr,udre        ;if UART data register empty bit is clear
	rjmp            serout1         ;       loop back
	ret				;else return

;serin input routine - input byte from UART to r16

serin:
        sbis            usr,rxc     ;if UART Receive Complete bit is clear
        rjmp            serin       ;       loop back
        in              byte,udr     ;else get received byte in byte reg.
        ret                         ;and return

;*************************************************************************************
;*
;* Module name: txb
;* Description: print byte as two hex characters
;* Called by:
;* Calls:
;* Input parameters: none
;* Output parameters: none
;* Register usage: none
;* Stack usage: 2 bytes plus call and return
;*
;*************************************************************************************


txb:
	push	byte
        swap    byte            ;upper nibble first
        rcall   txn		;print it
        swap    byte            ;lower nibble
        rcall   txn		;print it
	pop	byte
        ret

;*************************************************************************************
;*
;* Module name: txn
;* Description: print lower nibble in byte as a single hex character
;* Called by:
;* Calls:
;* Input parameters: none
;* Output parameters: none
;* Register usage: none
;* Stack usage: 2 bytes plus call and return
;*
;*************************************************************************************


txn:
        push    byte            ;save byte
        andi    byte,0x0F       ;mask off upper nibble
        cpi     byte,10         ;0 - 9?
        brlo    txn1
	ldi	temp,'A' - 10   ;no, convert to ASCII hex
        add     byte,temp
        rjmp    txn2        ;
txn1:
        ldi     temp,'0'        ;convert to ASCII numeral
        add     byte,temp
txn2:
        rcall   serout          ;print nibble
        pop     byte            ;restore byte
        ret


;*************************************************************************************
;*
;* Module name: gxn
;* Description: get lower nibble in byte as a single hex character
;* Called by:
;* Calls:
;* Input parameters: none
;* Output parameters: returns with character in byte and carry set if error
;*		      returns with hex in lower nibble and carry clear if OK                    
;* Register usage: none
;* Stack usage: 2 bytes plus call and return
;*
;*************************************************************************************


gxn:
	rcall	serin		;get byte
	cpi	byte,'0'	;< '0'?
	brlt	gxn4		;yes, return error
	cpi	byte,'9'+1	;> '9'?
	brge	gxn1		;yes, check 'A' - 'F'
	subi	byte,'0'	;no, convert to numeral 0-9
	rjmp	gxn5		;and return
gxn1:
	cpi	byte,'A'	;< 'A'?
	brlt	gxn4		;yes, return error
	cpi	byte,'F'+1	;> 'F'?
	brge	gxn4		;yes, return error
	subi	byte,'A'-10	;no, convert to numeral A-F
	clc
	rjmp	gxn5		;and return
gxn4:
	sec			;error - set carry and return
gxn5:
	ret

;*************************************************************************************
;*
;* Module name: gxb
;* Description: get a byte in byte register
;* Called by:
;* Calls:
;* Input parameters: none
;* Output parameters: returns with character in byte and carry set if error
;*		      returns with hex value in byte and carry clear if OK                
;* Register usage: none
;* Stack usage: 2 bytes plus call and return
;*
;*************************************************************************************

gxb:
	rcall	gxn		;get MS hex nibble in lower nibble position
	brcs	gxb1		;return with carry set if error
	swap	byte		;put in top nibble position
	mov	temp,byte	;save it in temp
	andi	temp,$F0	;clear lower nibble
	rcall	gxn		;get LS nibble in lower nibble position
	brcs	gxb1		;return with carry set if error
	or	byte,temp	;merge nibbles
gxb1:
	ret			;and return
	


dly:
	ldi		temp,0xFF
dly1:
	dec		temp
	brne	dly1

	ret


msg:
	.db	"Hello World!", $00
