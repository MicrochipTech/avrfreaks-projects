;accel.asm
;simple test program for ADXL202E
;accelerometer, using 16-bit Timer/Counter 1
;X (two ASCII hex bytes) and Y (two ASCII hex bytes)
;data transmitted to host PC via RS-232
;higher baud rates possible with a suitable crystal
;
;      |<-----------T2--------->| 
;       ---------                -----
;      |         |              |
;      |         |              |
;______|         |______________|
;
;      |<---T1-->|
;
;
;
;ADXL202E PWM period (T2)
;is a nominal 13 ms
;
;inputs:
;	Y		PD2/INT0 (pin 12)
;	X		PD3/INT1 (pin 13)

;outputs:
;	RxD
;	TxD
;
;clock: 4 MHz MHz crystal (250 ns cycle time)

.include "8515def.inc"

;****************************************
;equates
;****************************************
	.equ	Y_IN	=PD2	;(pin 12)
	.equ	X_IN	=PD3	;(pin 13)
	.equ	CLOCK	=4000000;clock freq. in Hz
	.equ	BAUD	=19200	;RS-232 baud rate
	.equ	CR		=$0D
	.equ	LF		=$0A

	

	.def	temp	=r16
	.def	count	=r17
	.def	T1_L	=r18
	.def	T1_H	=r19
	.def	T2_L	=r20
	.def	T2_H	=r21
	.def	byte	=r22

	.cseg
	.org	0

;*****************************************
;reset and interrupt vectors
;*****************************************
	rjmp	reset
	rjmp	EXT_INT0
	rjmp	EXT_INT1
	rjmp	TIM_CAPT1
	rjmp	TIM_COMP1
	rjmp	TIM_OVF1
	rjmp	TIM_OVF0
	rjmp	UART_RXC
	rjmp	UART_DRE
	rjmp	UART_TXC
	rjmp	ANA_COMP

;******************************************
;reset
;******************************************
reset:
	ldi		temp, low(RAMEND);initialise stack pointer
	out   	SPL, temp
	ldi		temp,high(RAMEND)
	out		SPH,temp
	sbi		ucr,txen		;enable UART transmitter
	sbi     ucr,rxen		;enable UART receiver
	sbi		ucr,txb8		;2 stop bits
	ldi     temp,CLOCK/(BAUD*16)-1 	;set baud rate
	out     ubrr,temp
	cbi		DDRD,PD2		;PD2 input
	cbi		DDRD,PD3		;PD3 input
	

;*******************************************
;main routine
;*******************************************


main:
	;get Y value and transmit it
	clr		temp			;clear counter
	out		TCNT1H,temp
	out		TCNT1L,temp
main1:
	sbic	pind,Y_IN		;wait for rising edge
	rjmp	main1			
main1_1:
	sbis	pind,Y_IN		
	rjmp	main1_1			
	ldi		temp,0b00000001	;start timing (CK/8)
	out		TCCR1B,temp
main2:
	sbic	pind,Y_IN		;wait for falling edge
	rjmp	main2		
	in		T1_L,TCNT1L		;save T1 count in T1_L and T1_H
	in		T1_H,TCNT1H
main3:
	sbis	pind,Y_IN		;wait for rising edge
	rjmp	main3	
	in		T2_L,TCNT1L		;save T2 count in T2_L and T2_H
	in		T2_H,TCNT1H
	clr		temp			;stop timing
	out		TCCR1B,temp
	mov		byte,T1_H		;transmit data to PC
	rcall	TXB
	mov		byte,T1_L
	rcall	TXB
	rcall	space
	mov		byte,T2_H
	rcall	TXB
	mov		byte,T2_L
	rcall	TXB
	rcall	space

	;get X value and transmit it
	clr		temp			;clear counter
	out		TCNT1H,temp
	out		TCNT1L,temp
main1_2:
	sbic	pind,X_IN		;wait for rising edge
	rjmp	main1_2		
main4:
	sbis	pind,X_IN	
	rjmp	main4		
	ldi		temp,0b00000001	;start timing (CK/8)
	out		TCCR1B,temp
main5:
	sbic	pind,X_IN		;wait for falling edge
	rjmp	main5		
	in		T1_L,TCNT1L		;save T1 count in T1_L and T1_H
	in		T1_H,TCNT1H
main6:
	sbis	pind,X_IN		;wait for rising edge	
	rjmp	main6		
	in		T2_L,TCNT1L		;save T2 count in T2_L and T2_H
	in		T2_H,TCNT1H
	clr		temp			;stop timing
	out		TCCR1B,temp
	mov		byte,T1_H		;transmit data to PC
	rcall	TXB
	mov		byte,T1_L
	rcall	TXB
	rcall	space
	mov		byte,T2_H
	rcall	TXB
	mov		byte,T2_L
	rcall	TXB
	rcall	new_line
	rjmp	main			;go round again



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
	rcall	serout
	rcall	space
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
	rcall	serout
	ldi		byte,LF
	rcall	serout
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
	ldi		byte,' '
	rcall	serout
	ret



;serial output routine - output byte to UART

serout:
	out		udr,byte	;load UART data register
serout1:
	sbis	usr,udre	;if UART data register empty bit is clear
	rjmp	serout1		;       loop back
	ret					;else return

;serin input routine - input byte from UART to r16

serin:
        sbis           usr,rxc     ;if UART Receive Complete bit is clear
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
        swap    byte	;upper nybble first
        rcall   txn		;print it
        swap    byte	;lower nybble
        rcall   txn		;print it
		pop		byte
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
        andi    byte,0x0F       ;mask off upper nybble
        cpi     byte,10         ;0 - 9?
        brlo    txn1
		ldi		temp,'A' - 10   ;no, convert to ASCII hex
        add     byte,temp
        rjmp    txn2        ;
txn1:
        ldi     temp,'0'        ;convert to ASCII numeral
        add     byte,temp
txn2:
        rcall   serout          ;print nybble
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
	cpi		byte,'0'	;< '0'?
	brlt	gxn4		;yes, return error
	cpi		byte,'9'+1	;> '9'?
	brge	gxn1		;yes, check 'A' - 'F'
	subi	byte,'0'	;no, convert to numeral 0-9
	rjmp	gxn5		;and return
gxn1:
	cpi		byte,'A'	;< 'A'?
	brlt	gxn4		;yes, return error
	cpi		byte,'F'+1	;> 'F'?
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
	mov		temp,byte	;save it in temp
	andi	temp,$F0	;clear lower nibble
	rcall	gxn		;get LS nibble in lower nibble position
	brcs	gxb1		;return with carry set if error
	or		byte,temp	;merge nibbles
gxb1:
	ret			;and return
	




;*********************************************
;vectors to interrupt handlers
;*********************************************
EXT_INT0:	reti
EXT_INT1:	reti
TIM_CAPT1:	reti
TIM_COMP1:	reti
TIM_OVF1:	reti	
TIM_OVF0:	reti
UART_RXC:	reti
UART_DRE:	reti
UART_TXC:	reti
ANA_COMP:	reti

