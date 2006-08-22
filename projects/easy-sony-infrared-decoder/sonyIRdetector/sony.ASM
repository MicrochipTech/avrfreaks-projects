;Receive Sony IR Remote Control Codes with Atmel AT90S2313
;Read key presses and output to terminal @ 9600 Baud
;
;I was tired of downloading useless RC5 routines that
;would never work in a million years! Or at least not
;on the remotes that 99% of the people have which are NOT RC5!
;most of the source code available for newbies like me to 
;learn from ALMOST ALWAYS has something wrong, or is not commented 
;correctly. This particular file didn't even mention what
;clock crystal to use, I had to guess it was 4MHz!
;kind of frustrating for newbies like me.
;
;so I found this file on the net(can't remember where I got it)
;modified it to output the keys pressed to any terminal
;running at 9600 baud
;this file is a collection of various routines I found
;put them all together and made it work
;I hope it will help some others learn, without
;the frustration!
;
;sorry no schematic, real easy anyways!
;connect 2313 to max232
;works with 4MHz clock
;I used a Sharp GP1UV70Q2 36KHz ir detector on PD2
;(Digi-Key # 425-1852-ND)
;connect IR detector with 47uF cap on VCC + GND
;use 47 ohm resistor from VCC to detector VCC
;LOOK AT THE SPEC SHEET on how to hook up the ir detector
;I included the spec sheet so there are no guessing games
;on how to hook up the IR detector 
;I also included everything you need to assemble the code,(run assemble.bat)
;and even the hex file if you are still having problems!!
;
.nolist				
.include "2313def.inc"		
.list
;------------------------------------------------------------
;Function code definitions
;see codes.html file for each key code

;equates for terminal
.equ  space = 0x20	;ascii space
.equ  cr	= 0x0D	;carriage return
.equ  lf	= 0x0A	;line feed
.equ  esc	= 0x1B	;ESCAPE character

;Constant 'downt' specifies length of time each port pin
;is held high for each key press
.equ	downt	=150		;Function output debounce time in mS

;	Constant 'which' specifies device code (i.e. 0x05 is VCR)
.equ	which	=0x05		;Sony VCR device code
				;Sony TV device code = 0x00
;	Global register variables
.def	irsaver	=R12		;Save SREG in IR ISR
.def	saver		=R13		;Save SREG in T1 ISR
.def	last		=R14		;Last code received
;.def		=R15		;
.def	wreg		=R16		;General use working register
.def	devcode	=R17		;IR device code
.def	irfunc	=R18		;IR function code
.def	flags		=R19		;Bit variables
.def	timeout	=R20		;Delay time passed to subroutine
.def	bitcnt	=R21		;Received character bit counter
.def	count		=R22		;Count of IR functions received
.def	temp		=R23		;Temp storage in timer 1 ISR
;.def		=R24		;
;.def		=R25
;.def		=R26
;.def		=R27
;.def		=R28
;.def		=R29
;.def		=R30
;.def		=R31

;	Flag definitions
.equ	gotir		=0
.equ	startb	=1

;	Timer/Counter prescaler values
.equ	TSTOP	=0		;Stop Timer/Counter
.equ	TCK64	=3		;Timer/Counter runs from CK / 64
.equ	TCK256 =4		;Timer/Counter runs from CK / 256
.equ	TCK1024 =5		;Timer/Counter runs from CK / 1024
.equ	T1VAL	=-4		;Timer 1 reload value
;------------------------------------------------------------
.cseg
;	Interrupt vectors
.org	0				;Reset Vector Address
	rjmp	reset
.org	INT0addr			;External Interrupt0 Vector Address
	rjmp	readir
.org	INT1addr			;External Interrupt1 Vector Address
	reti
.org	ICP1addr			;Input Capture1 Interrupt Vector Address
	reti
.org	OC1addr 			;Output Compare1 Interrupt Vector Address
	reti
.org	OVF1addr			;Overflow1 Interrupt Vector Address
	rjmp	timer1
.org	OVF0addr			;Overflow0 Interrupt Vector Address
	reti
.org	URXCaddr			;UART Receive Complete Interrupt Vector Address
	reti
.org	UDREaddr			;UART Data Register Empty Interrupt Vector Address
	reti
.org	UTXCaddr			;UART Transmit Complete Interrupt Vector Address
	reti
.org	ACIaddr 			;Analog Comparator Interrupt Vector Address
	reti
;------------------------------------------------------------
;	Main program entry point on reset
reset:
	ldi	wreg,RAMEND		;Init Stack Pointer
	out	SPL,wreg

	ldi	wreg,0x00		;Turn off all function outputs
	out	PORTB,wreg
	ldi	wreg,0xFF		;Make port B pins all outputs
	out	DDRB,wreg

	out	PORTD,wreg		;Put power on all port D pins
	ldi	wreg,0b11111011	;Make PD2 an input, all others outputs
	out	DDRD,wreg

	ldi	temp,1<<ISC01	;Use falling edge for interrupt
	out	MCUCR,temp

	ldi	wreg,TSTOP		;Timer 1 off (just in case)
	out	TCCR1B,wreg		;Stop timer
	ldi	wreg,0b10000000	;Enable Timer 1 interrupt
	out	TIMSK,wreg

	ldi	wreg,TCK1024	;Load timer 1 prescaler
	out	TCCR1B,wreg

	ldi	wreg,HIGH(T1VAL)	;Load timer 1
	out	TCNT1H,wreg
	ldi	wreg,LOW(T1VAL)
	out	TCNT1L,wreg

	clr	flags
	clr	count
	clr	last
	ldi	wreg,1<<INT0	;Enable external interrupt 0
	out	GIMSK,wreg

	ldi   wreg,25	  	; 9600 Baud, 2313 @ 4MHz
     	out   UBRR, wreg    	; UART Baud Rate Reg
     	ldi   wreg, 0x18    	; 
     	out   UCR, wreg     	; UART control reg = 0x18 RxEnable, TxEnable

	rcall	ClearScreen		; clear the terminal screen
	sei				; Enable interrupts
;---------------------------------------------------------------
main: 
	sbrs	flags,gotir		;See if anything received yet
	rjmp	main			;Wait until something available
	cbr	flags,1<<gotir	;Clear status flag

;I commented this out since the eight bit was not removed from
;all of the device codes I found on the net!
;also the device code check is disabled because I only had a Sony TV remote
;which is device code 00

;;	andi	irfunc,0x7f		;Eighth bit set on some?
;;	cpi	devcode,which	;Verify device code
;;	brne	main
	cp	last,irfunc		;See if same as last(used for debouncing)
	breq	irsame
	clr	count
	mov	last,irfunc
;--------------------------------------------------------------------------
;Print S[system ID] space C[command] Newline	
	ldi	wreg,'S'
	rcall	Snd_Byte
	mov	wreg,devcode	;print the system ID
	rcall	hex2ascii

	ldi	wreg,space
	rcall	Snd_Byte

	ldi	wreg,'C'
	rcall	Snd_Byte
	mov	wreg,irfunc		;print the command
	rcall	hex2ascii
	rcall	NewLine	
	rjmp	main
irsame:
	inc	count
	cpi	count,5		;(to debounce buttons)
	breq	irclear
	rjmp	main
irclear:
	clr	last
	clr	count
	rjmp	main			;Loop forever
;------------------------------------------------------------
;Interrupt service routine for IR input
;Entered on falling edge of IR receiver
readir:
	in	irsaver,SREG	;Save SREG
	sbrc	flags,startb
	rjmp	readnext		;If startb is set, start flag has been found
	ldi	timeout,141		;Delay to 3.5 T to qualify start bit
	rcall	delay
	sbic	PIND,2		;Pin should still be low
	rjmp	readexit		;Otherwise exit ISR
	ldi	timeout,36		;Delay 1 T to qualify space after start bit
	rcall	delay
	sbis	PIND,2		;Pin should now be high
	rjmp	readexit		;Otherwise exit ISR
	sbr	flags,1<<startb	;Set 'start bit found' flag
	ldi	bitcnt,12		;Set counter to get 12 bits
	clr	devcode		;Clear received bytes and flags
	clr	irfunc
	cbr	flags,1<<gotir
	rjmp	readexit		;Exit ISR
readnext:
	ldi	timeout,16		;Delay to center of this bit
	rcall	delay
	sbic	PIND,2		;Should still be low here
	rjmp	readbad		;Otherwise bail out
	ldi	timeout,38		;Delay 1T to check for a 1 or 0
	rcall	delay
	clc				;Assume next bit is a zero
	sbis	PIND,2		;Read next bit from receiver
	sec				;Set carry if it was a 1
	ror	devcode		;Shift received bit into registers
	ror	irfunc
	dec	bitcnt		;Count down number of bits
	brne	readexit
	swap	devcode		;Rearrange nibbles in proper order
	mov	wreg,devcode
	andi	devcode,0x0f
	swap	irfunc
	andi	wreg,0xf0
	or	irfunc,wreg
	sbr	flags,1<<gotir	;Tell everyone we have a valid code
readbad:
	cbr	flags,1<<startb	;Clear 'start bit found' flag
readexit:
	out	SREG,irsaver	;Restore SREG
	reti
;--------------------------------------------------------------------------
;Delay n*16 microseconds using timer 0, delay time passed in timeout
;Overhead, due to call and return, not taken into account
delay:
	clr	wreg
	sub	wreg,timeout	;Convert count down into count up
	out	TCNT0,wreg		;Put count up time into counter
	ldi	wreg,TCK64		;Timer 0 prescaler, CK / 64
	out	TCCR0,wreg		;Run timer
dwait:
	in	wreg,TIFR		;Check timer interrupt flags
	sbrs	wreg,TOV0		;Wait for timer to overflow
	rjmp	dwait
	clr	wreg
	sbr	wreg,1<<TOV0	;Set bit to clear it (huh?)
	out	TIFR,wreg
	ldi	wreg,TSTOP		;Timer 0 off
	out	TCCR0,wreg		;Stop timer
	ret
;------------------------------------------------------------
;	Timer 1 interrupt service routine
timer1:
	sei				;Re-enable interrupts for IR reciever
	in	saver,sreg
	ldi	temp,HIGH(T1VAL)	;Reload timer 1
	out	TCNT1H,temp
	ldi	temp,LOW(T1VAL)
	out	TCNT1L,temp
	out	sreg,saver
	reti
;*********************************************	
;Send a char to the uart from wreg register
;enter with char in wreg
Snd_Byte:
	sbis	USR, UDRE		; wait until clear then send one char
	rjmp	Snd_Byte
	out UDR,wreg 		; Transfer this char to the transmitter port
	ret
;*********************************************	
;routine to input one char
;exit with char in wreg
Get_Byte: 
	sbis	USR, RXC		;wait until ready then get one char
	rjmp	Get_Byte
	in	wreg, UDR	
	ret
;---------------hex2ascii--------------
;this routine converts a hex byte (from wreg) to ascii equivalent
;and sends the byte via the Snd_Byte routine
hex2ascii:
	push	wreg 			; save for lower nibble
	swap	wreg 			; get upper nibble
	andi	wreg ,0x0f		; mask
	cpi	wreg ,0x0a		; is it 0-9?
	brlo	okhi			; yup
	subi	wreg ,-(7)		; else add 7 to make it a-f
okhi:
	subi	wreg ,-(0x30)	; make it ascii
	rcall	Snd_Byte		; send upper nibble
 
	pop	wreg 			; now do the lower nibble
	andi	wreg ,0x0f
	cpi	wreg ,0x0a
	brlo	oklo
	subi	wreg ,-(7)
oklo:
	subi	wreg ,-(0x30)

	rcall	Snd_Byte		; send lower nibble
	ret
;-------------------------------------------
NewLine:
	ldi	wreg,cr		;go to new line
	rcall	Snd_Byte
	ldi	wreg,lf
	rcall	Snd_Byte
	ret
;-------------------------------------------
;clear the screen on terminal
;On some terminal programs this doesn't work 
;works good on windows terminals
ClearScreen:
	ldi	wreg,esc
	rcall	Snd_Byte

	ldi	wreg,'['
	rcall	Snd_Byte

	ldi	wreg,'H'
	rcall	Snd_Byte

	ldi	wreg,esc
	rcall	Snd_Byte

	ldi	wreg,'['
	rcall	Snd_Byte

	ldi	wreg,'J'
	rcall	Snd_Byte
	ret

.exit
