;*******************************************************************
;
; Common-Law Copyright (C) 2006 .Kenneth-Scott: Vitale, a sovran man
;
;*******************************************************************
;
;Description
;
;Poor-mans DDS Synthesizer
;
;Heavily Modified by: Kenneth-Scott: Vitale, a sovran man
;Target 			= AT90S2313
;Date   			= 2006-0708
;
;Original Author 	= Jesper Hansen Copyright (C) 2000 Jesper Hansen <jesperh@telia.com>.
;Target 			= AT90S2313
;Date   			= 2001-02-15
;
;PB0..7 = D/A Data out
;
;PD0		RXD
;PD1		TXD
;PD2		INT0 - used to wake up AVR once a terminial is connected (active low)
;PD6		Enable charge-pump for +5V supply
;PD3..5	not used
;
;*******************************************************************
;
;Output frequency (using 24 bit accumulator) :
;
;	f = deltaPhase * fClock/2^24
;
;  fClock is in this case the CPU clock divided by the
;	number of cycles to output the data ( 9 cycles )
;
;	f = addrl/addrm/addrh * (10000000/9)/16777216
;
;	f = addrl/addrm/addrh * 0.0662274 Hz resolution
;
;	fMax (theoretical) = 0.5 * fClock
;	fMax (empiracle - square wave) = 0.05 * fClock
;
.nolist
.include "2313def.inc"			;chip definition
.list
;
;******************************************************************************
;set some values
;******************************************************************************
.equ	test	= 0					;"0" for normal operation
.equ	tblset	= 1					;wavetable set 0 or 1 

;.equ	fclk 	= 10000000			;system clock frequency
.equ	fclk 	= 10002926			;Actual Measured system clock frequency
;.equ	fclk	= 3701342			;CPU Oscillator	(Actual Measured STK500 Clock Source)

.equ	baud 	= 9600				;desired baud rate
.equ	uartclk	= fclk/16			;the UART clock
.equ	baudclk	= (uartclk/baud)-1	;get the UBRR value

.equ	zero	= 0					;create a zero

.equ	cr		= 0x0d				;carriage character
.equ	lf		= 0x0a				;line feed character
.equ	space	= 0x20				;ASCII space character
.equ	colon	= 0x3a				;ASCII colon character
.equ	frmfd	= 0x0c				;ASCII formfeed (clear screen)

.equ	loopcyc	= 10				;9 instruction cycles per main loop
;									;cummulative add/subtract values
.equ	_1Hz	= (1*loopcyc*EXP2(24)/fclk)
.equ	_10Hz	= (10*loopcyc*EXP2(24)/fclk)
.equ	_100Hz	= (100*loopcyc*EXP2(24)/fclk)
.equ	_1KHz	= (1000*loopcyc*EXP2(24)/fclk)
.equ	_10KHz	= (10000*loopcyc*EXP2(24)/fclk)
.equ	_100KHz	= (100000*loopcyc*EXP2(24)/fclk)

.equ	f_pu	= 1					;power-up frequency in KHz
.equ	s_pu	= 2					;power-up waveform type

									;1=sine, 2=square, 3=triangle, 4=ramp
.equ	wavtbl	= 0x200				;start address of wavetable

.equ	DACPort	= PORTB				;Port B is for DAC
.equ	DAC_DR	= DDRB				;data direction register for DAC port

.equ	CtlDR	= DDRD				;data direction register for control port
.equ	CtlOut	= PORTD				;Port we use for controlling hardware
.equ	CtlIn	= PIND				;Input Port for controlling hardware
.equ	CP_En	= PIND6				;pin for charge-pump enable
.equ	Wake	= PIND2				;pin to wake us up

.def	dec1s	= r2				;register defs to hold decimal digits
.def	dec10s	= r3
.def	dec100s	= r4
.def	dec1ks	= r5
.def	dec10ks	= r6
.def	dec100ks= r7

.def	al		= r11				;storage for BIN2BCD routine
.def	ah		= r12

.def	EEdata	= r14				;data byte read from EEPROM
.def	EEaddr	= r15				;address low byte to read from

.def	temp	= r16				;set temporary registers
.def	temp2	= r17

.def	bcd0	= r18				;packed BCD of frequency lsd to msd
.def	bcd1	= r19
.def	bcd2	= r20

.def	bcdt	= r21				;packed BCD temp register

.def	addrl	= r22				;setup adder low-byte
.def	addrm	= r23				;setup adder mid-byte
.def	addrh	= r24				;setup adder high-byte

.def	bcd0a	= r25				;packed BCD adder registers (are temporary)
.def	bcd1a	= r26
.def	bcd2a	= r27

.def	accl	= r28				;phase accumulator low-byte
.def	accm	= r29				;phase accumulator mid-byte
.def	acch	= r30				;phase accumulator high-byte

;************************************************************************
;MACRO Definitions
.include	"Include\Delay-Macros.inc"
;	Usage:
;		_WAIT_uS	CPU_FREQUENCY,8		;MAKES 8uS DELAY
;		_WAIT_5uS	CPU_FREQUENCY,8		;MAKES 40uS DELAY
;		_WAIT_10uS	CPU_FREQUENCY,8		;MAKES 80uS DELAY
;		_WAIT_mS	CPU_FREQUENCY,8		;MAKES 8mS DELAY
;		_WAIT_10mS	CPU_FREQUENCY,8		;MAKES 80mS DELAY
;

;************************************************************************
;Data Segment Definitions
; in RAM
;************************************************************************
;.dseg
;.org	0x0

;bcd0:	.byte	1					;packed BCD of frequency lsd to msd
;bcd1:	.byte	1
;bcd2:	.byte	1

;bcd0a:	.byte	1					;packed BCD adder registers (are temporary)
;bcd1a:	.byte	1
;bcd2a:	.byte	1

;bcdt:	.byte	1					;packed BCD overflow/underflow register

;******************************************************************************
;start of code
;******************************************************************************

.cseg
.org	0
;set up interupt vectors
	rjmp	RESET				;reset vector
	rjmp	EXT_INT0			;IRQ0 Handler
;	rjmp	EXT_INT1			;IRQ1 Handler
	reti
;	rjmp	TIM_CAPT1			;Timer1 Capture Handler
	reti
;	rjmp	TIM_COMP1			;Timer1 Compare Handler
	reti
;	rjmp	TIM_OVF1			;Timer1 Overflow Handler
	reti
;	rjmp	TIM_OVF0			;Timer0 Overflow Handler
	reti
	rjmp	UART_RXC			;UART RX Complete Handler
;	rjmp	UART_DRE			;UDR Empty Handler
	reti
;	rjmp	UART_TXC			;UART TX Complete Handler
	reti
;	rjmp	ANA_COMP			;Analog Comparator Handler
	reti

;******************************************************************************
;code
;******************************************************************************

RESET:
INIT_Machine:
;Set the stack pointer to 025F, stack grows down. 
;Can't call any subroutines till we have a stack!
;
	ldi		temp, RAMEND
	out		SPL, temp			;setup stack pointer

;Set Watchdog Inactive  Databook 5-43
;Gotta handle this before it gets a chance to bite me.
;  
	ldi		TEMP,$1F				;00011011   
	out		WDTCR,TEMP
	ldi 	TEMP,$17				;00010111
			;XXX1XXXX Watch dog turn off enable, must be 1 to wrt D3
			;XXXX0XXX WDE 0=disable 1=enable
			;XXXXX111 000 =   16mS WD prescaler bits
			;        001 =   32mS
			;    010 =   64mS
			;    011 =  128mS
			;    100 =  256mS
			;    101 =  512mS
			;    110 = 1024mS
			;        111 = 2048mS

	out		WDTCR,TEMP
;
;This section allows ints which are apparently 
;caused by reset, to occur harmlessly.
;Databook 5-23, 5-29

	ldi		TEMP,$40
								;0XXXXXXX 1= enable int 1
								;X1XXXXXX 1= enable int 0
	out		GIMSK,TEMP			;Enable int 0,1
   
	ldi		TEMP,$02
									;0XXXXXXX TC1 overflow int enable
									;X0XXXXXX TC1 OCA Match int enable
									;XX0XXXXX TC1 OCB Match int enable
									;XXX0XXXX Reserved =0
									;XXXX0XXX TC1 input capture int enable
									;XXXXX0XX Reserved =0
									;XXXXXX1X TC0 overflow int enable
									;XXXXXXX0 Reserved =0
	out		TIMSK,TEMP				;Enable all timer ints
   
	in		TEMP,SREG				;(5-23)
	ori		TEMP,$80				;Global int enable
	out		SREG,TEMP
	nop
	nop
	nop								;Let the ints happen, so they
	nop								;clear out
	nop
	nop
	nop								;NOTE: I have not exhaustively researched the need 
	nop								;for this EI-NOP-DI section, but it is working well.
	nop

	in		TEMP,SREG
	andi	TEMP,$7F				;Turn off all ints (5-23)
	out		SREG,TEMP

	ldi		TEMP,$00				;Disable all timer ints (5-29)
	out		TIMSK,TEMP

	ldi		TEMP,$00				;Disable all ints. (5-29)
									;1XXXXXXX enable int 1
									;X1XXXXXX enable int 0

	out		GIMSK,TEMP

;At this point, we should have a clean machine, in that there
;are no bogus pending ints from powerup. Later, when we enable
;various ints for real, they will only trigger after an event 
;has occured, rather than because the int logic was scrambled
;at powerup.

	clr		temp				;set all ports to inputs
	out		DAC_DR,temp
	out		CtlDR,temp
	sbi 	CtlOut,Wake			;enable pull-up on INT0

;	sbic 	CtlIn,Wake			;skip if we need to power up	
;	rjmp	Power_Off			;Power Off the unit
	rjmp	Power_On			;Power Up the unit


;**********************************************************;
;main loop
;
;	accl,accm,ZL is the phase accumulator
; 	addrl,addrm,addrh is the adder value determining frequency
;
;	add value to accumulator
;	load byte from current table in ROM
;	output byte to port
;	repeat 
;
MAIN:
	add		accl,addrl				;1
	adc		accm,addrm				;1
	adc		ZL,addrh				;1
	lpm								;3
	out		DACPort,r0				;1
	nop								;1 (needed because otherwise we get more jitter and frequency is off)
	rjmp	MAIN					;2 => 10 cycles

;--------------------------------------- 
;Power unit on
;--------------------------------------- 
POWER_ON:
	ldi		temp, RAMEND
	out		SPL, temp				;setup stack pointer

;	rjmp	BCDsub

;setup ports
	ser		temp					;
	out		DAC_DR,temp				;set all DACPort bits as output
	
	sbi		CtlDR,CP_En				;enable charge-pump pin as output
	sbi		CtlOut,CP_En			;enable charge pump
	sbi 	CtlOut,Wake				;enable pull-up on INT0

;clear accumulators 
	clr 	accl					;clear accumulators 
	clr 	accm
	clr 	acch

;setup UART
	ldi		temp,baudclk			;set uart speed
	out		UBRR,temp

;enable RXint and enable tx/rx
	ldi		temp,1<<RXCIE|1<<RXEN|1<<TXEN
	out		UCR,temp

;enable INT0 interupts on rising edge, no sleep mode
	ldi		temp,1<<ISC00|1<<ISC01|0<<SE|0<<SM
	out		MCUCR,Temp
	ldi		temp,1<<INT0			;ensure INT0 interupts
	out		GIMSK,Temp

	sei								;global enable interrupts

	rcall	RE_INIT					;initialize the counters

	rjmp	MAIN

;--------------------------------------- 
;Power unit off
;--------------------------------------- 
POWER_OFF:
	clr		temp
	out		CtlDR,Temp				;set direction register, all inputs
	out		CtlOut,Temp				;set outputs all low, no pullups
;	sbi 	CtlOut,Wake				;enable pull-up on INT0

	sbi		CtlDR,CP_En				;enable charge-pump pin as output
	cbi		CtlOut,CP_En			;disable charge pump

;disable RXint tx/rx
	ldi		temp,0<<RXCIE|0<<RXEN|0<<TXEN
	out		UCR,temp

;set sleep mode to "Power-Down and INT0 = low-level
	ldi		temp,0<<ISC00|0<<ISC01|1<<SE|1<<SM
	out		MCUCR,Temp

	ldi		temp,1<<INT0			;ensure INT0 interupts
	out		GIMSK,Temp

	sei								;ensure interupts enabled

	sleep							;sleep and power down
	rjmp	MAIN					;go back to main loop

;*************************************************************
;Interrupt routine for serial link connected
;
EXT_INT0:							;IRQ0 Handler
	rcall	DELAY250				;delay 250ms

	sbis 	CtlIn,Wake				;skip if we need to power off	
	rjmp	Power_On				;Power Up the unit
	rjmp	Power_Off				;Power Off the unit

	reti							;should never arrive here!

;*************************************************************
;Interrupt routine for incoming bytes on the RS232 link
;
	
UART_RXC:							;UART RX Complete Handler
	push	temp					;save registers
	in 		temp,UDR

	clr		BCD0a					;clear temp registers
	clr		BCD1a
	clr		BCD2a

tx_10:
	cpi		temp,'+'				;up one
	brne	tx_11
	ldi		temp,MSG_U1				;set string pointer
	rcall	send_string				;spit it out
	ldi		BCD0a,0x01
;	ldi		BCD1a,0x00
;	ldi		BCD2a,0x00
	rcall	BCDadd					;add "1" to the current count 
	rjmp	tx_98		
tx_11:
	cpi		temp,'D'				;up ten
	brne	tx_12
	ldi		temp,MSG_U10			;set string pointer
	rcall	send_string				;spit it out
	ldi		BCD0a,0x10
;	ldi		BCD1a,0x00
;	ldi		BCD2a,0x00
	rcall	BCDadd					;add "10" to the current count 
	rjmp	tx_98		
tx_12:
	cpi		temp,'C'				;up hundred
	brne	tx_13
	ldi		temp,MSG_U100			;set string pointer
	rcall	send_string				;spit it out
;	ldi		BCD0a,0x00
	ldi		BCD1a,0x01
;	ldi		BCD2a,0x00
	rcall	BCDadd					;add "10" to the current count 
	rjmp	tx_98		
tx_13:
	cpi		temp,'K'				;up thousand
	brne	tx_20
	ldi		temp,MSG_U1000			;set string pointer
	rcall	send_string				;spit it out
;	ldi		BCD0a,0x00
	ldi		BCD1a,0x10
;	ldi		BCD2a,0x00
	rcall	BCDadd					;add "1000" to the current count 
	rjmp	tx_98		

tx_20:
	cpi		temp,'-'				;down one
	brne	tx_21
	ldi		temp,MSG_D1				;set string pointer
	rcall	send_string				;spit it out
	ldi		BCD0a,0x01
;	ldi		BCD1a,0x00
;	ldi		BCD2a,0x00
	rcall	BCDsub					;subtract "1" from the current count 
	rjmp	tx_98		
tx_21:
	cpi		temp,'d'				;down ten
	brne	tx_22
	ldi		temp,MSG_D10			;set string pointer
	rcall	send_string				;spit it out
	ldi		BCD0a,0x10
;	ldi		BCD1a,0x00
;	ldi		BCD2a,0x00
	rcall	BCDsub					;subtract "10" from the current count 
	rjmp	tx_98		
tx_22:
	cpi		temp,'c'				;down hundred
	brne	tx_23
	ldi		temp,MSG_D100			;set string pointer
	rcall	send_string				;spit it out
;	ldi		BCD0a,0x00
	ldi		BCD1a,0x01
;	ldi		BCD2a,0x00
	rcall	BCDsub					;subtract "100" from the current count 
	rjmp	tx_98		
tx_23:
	cpi		temp,'k'				;down thousand
	brne	tx_70
	ldi		temp,MSG_D1000			;set string pointer
	rcall	send_string				;spit it out
;	ldi		BCD0a,0x00
	ldi		BCD1a,0x10
;	ldi		BCD2a,0x00
	rcall	BCDsub					;subtract "1000" from the current count 
	rjmp	tx_98		

tx_70:
	cpi		temp,'R'
	brne	tx_80
	rcall	RE_INIT					;re-init the counters
	rjmp	tx_98		

tx_80:
	cpi		temp,'s'				;direct frequency setting
	breq	tx_80a
	cpi		temp,'S'
	brne	tx_90
tx_80a:
	ldi		temp,MSG_SET			;set string pointer
	rcall	send_string				;spit it out
	rcall	read_6		
	rcall	send_crlf				;send CR/LF
	rjmp	tx_98		

;set wavetable outputs
tx_90:	
	cpi		temp,'1'				;request sinewave output
	brne	tx_91
	ldi		ZH,HIGH(2*sine)			;setup Z pointer hi
	ldi		ZL,LOW(2*sine)			;setup Z pointer lo
	ldi		temp,MSG_SINE			;set string pointer
	rcall	send_string				;spit it out
	rjmp	tx_99
tx_91:
	cpi		temp,'2'				;request square output
	brne	tx_92
	ldi		ZH,HIGH(2*square)		;setup Z pointer hi
	ldi		ZL,LOW(2*square)		;setup Z pointer lo
	ldi		temp,MSG_SQUARE			;set string pointer
	rcall	send_string				;spit it out
	rjmp	tx_99
tx_92:
	cpi		temp,'3'				;request triangle output
	brne	tx_93
	ldi		ZH,HIGH(2*triangle)		;setup Z pointer hi
	ldi		ZL,LOW(2*triangle)		;setup Z pointer lo
	ldi		temp,MSG_TRI			;set string pointer
	rcall	send_string				;spit it out
	rjmp	tx_99
tx_93:
	cpi		temp,'4'				;request sawtooth output
	brne	tx_94
	ldi		ZH,HIGH(2*sawtooth)		;setup Z pointer hi
	ldi		ZL,LOW(2*sawtooth)		;setup Z pointer lo
	ldi		temp,MSG_RAMP			;set string pointer
	rcall	send_string				;spit it out
	rjmp	tx_99
tx_94:
;	cpi		temp,'5'				;request sawtooth output
;	brne	tx_95
;	ldi		ZH,HIGH(2*revsawtooth)	;setup Z pointer hi
;	ldi		ZL,LOW(2*revsawtooth)	;setup Z pointer lo
;	ldi		temp,MSG_REVRAMP		;set string pointer
;	rcall	send_string				;spit it out
;	rjmp	tx_99
tx_95:
;	cpi		temp,'4'				;random "white noise" output
;	brne	tx_99
;	ldi		ZH,HIGH(2*white)		;setup Z pointer hi
;	ldi		ZL,LOW(2*white)			;setup Z pointer lo
;	ldi		temp,MSG_NOISE			;set string pointer
;	rcall	send_string				;spit it out
;	rjmp	tx_99

tx_98:
	rcall	SET_FREQ				;set up registers for new frequency

;unknown command, just ignore it
tx_99:
	rcall	send_data				;always reply with the current frequency

tx_exit:
	pop		temp
	reti

;**********************************************************************
;communication functionality
;**********************************************************************
;
;send the current frequency to the PC
;as a 7-byte sequence :
;'FRQ: ' folowed by a 6-digit ASCII of phase accumulator value and " Hz"
;
send_data:
	ldi		temp,MSG_FRQ			;set string pointer
	rcall	send_string				;spit it out
	rcall	send_6					;spit out all 6 digits

	ldi		temp,MSG_HZ				;set string pointer
	rcall	send_string				;spit it out

	rcall	send_crlf				;send CR/LF

	ret

;read the 6-digit packed BCD in registers bcd0, bcd1 and bcd2
;and convert to 6 ASCII characters sending to the serial 
;link (char stored in 'temp')
send_6:
	mov		temp,bcd2				;get high-digits
	rcall	send_2BCD				;send them

	mov		temp,bcd1				;get middle-digits
	rcall	send_2BCD				;send them

	mov		temp,bcd0				;get low-digits
	rcall	send_2BCD				;send them

	ret

;convert "digits" stored in 'temp' to two ASCII BCD digits and send 
;  them out the serial link
send_2BCD:
	push	temp					;save a copy
	andi	temp,0xf0				;strip off low-nibble
	swap	temp					;put into low-nibble
	subi	temp,-(0x30)			;make it ASCII
	rcall	send_char				;send 10's digit
	pop		temp					;retrieve the copy
	andi	temp,0x0f				;strip off low-nibble
	subi	temp,-(0x30)			;make it ASCII
	rcall	send_char				;send 1's digit
	ret
;
;read in 6 characters from the serial link (char stored in 'temp')
;and convert to 6-digit packed BCD
;in registers bcd0, bcd1 and bcd2
read_6:
	rcall	read_2BCD				;convert to packed BCD
	mov		bcd2,temp				;store the digit

	rcall	read_2BCD				;convert to packed BCD
	mov		bcd1,temp				;store the digit

	rcall	read_2BCD				;convert to packed BCD
	mov		bcd0,temp				;store the digit

	ret

;Read two ASCII digits in the serial link and convert them into 
;  packed BCD.  Returns in 'temp2'
read_2BCD:
	rcall	get_char				;read upper digit position
	rcall	send_char				;echo it back
	subi	temp,0x30				;strip ASCII
	swap	temp					;put into high-nibble
	mov		temp2,temp				;save it
	rcall	get_char				;read lower digit position
	rcall	send_char				;echo it back
	subi	temp,0x30				;strip ASCII
	or		temp,temp2				;combine with prior digit
	ret

;***********************************************************
;Send String.  Pointer in temp.  Data from EEPROM returned 
;  in "temp" register.  Null terminates string
;
send_string:
	mov 	EEaddr,temp				;set string pointer
send_string1:
	rcall	EERead_seq				;get the byte
	tst		EEdata					;check for null terminator
	breq	send_string99			;null, end of string
	mov		temp,EEdata				;put the data byte into "temp"
	rcall	send_char				;send it
	inc		EEaddr					;next character
	rjmp	send_string1			;loop
send_string99:
	ret
;
;get char in temp
;
get_char:
	in 		temp,USR				;wait for a byte to be ready
	sbrs	temp,7					;ready ?
	rjmp	get_char				;no, wait some more
	in		temp,UDR				;get the byte
	ret								;and return
;
;send char in temp
;
send_char:
	push	temp					;save temp
send_c2:
	in 		temp,USR				;wait for the transmitter to be ready
	sbrs	temp,UDRE				;ready ? 
	rjmp	send_c2					;no, wait some more
	pop		temp					;restore temp
	out		UDR,temp				;send char
	ret								;ans return

;sends a CR/LF
send_crlf:
	ldi		temp,MSG_CRLF			;set string pointer
	rcall	send_string				;spit it out
	ret

;***************************************************************************
;* "set_freq" - sets up the frequency registers once 6-digit frequency 
;*  is entered from the serial link
;***************************************************************************

SET_FREQ:							;set up registers for new frequency
	clr		addrl					;zero the frequency registers
	clr		addrm
	clr		addrh

	ldi		temp,0xf0				;set up "striper"
	mov		r1,temp

SET_FREQ100KHZ:
	mov		r0,BCD2					;get upper digits, 100K and 10K
	and		r0,r1					;strip low nible
	swap	r0						;and swap nible, 100K
SET_FREQ100KHZa:
	tst		r0						;check for zero
	breq	SET_FREQ10KHZ			;is zero, so skip
	ldi		BCD0a,LOW(_100KHz)		;add up the count values
	adc		addrl,BCD0a
	ldi		BCD1a,BYTE2(_100KHz)
	adc		addrm,BCD1a
	ldi		BCD2a,BYTE3(_100KHz)
	adc		addrh,BCD2a
	dec		r0						;dec register
	rjmp	SET_FREQ100KHZa			;and loop till zero

SET_FREQ10KHZ:
	mov		r0,BCD2					;get upper digits, 100K and 10K
	swap	r1
	and		r0,r1					;strip high nible, 10K
SET_FREQ10KHZa:
	tst		r0						;check for zero
	breq	SET_FREQ1KHZ			;is zero, so skip
	ldi		BCD0a,LOW(_10KHz)		;add up the count values
	adc		addrl,BCD0a
	ldi		BCD1a,BYTE2(_10KHz)
	adc		addrm,BCD1a
	ldi		BCD2a,BYTE3(_10KHz)
	adc		addrh,BCD2a
	dec		r0						;dec register
	rjmp	SET_FREQ10KHZa			;and loop till zero

SET_FREQ1KHZ:
	mov		r0,BCD1					;get middle digits, 1K and 100
	swap	r1
	and		r0,r1					;strip low nible
	swap	r0						;and swap nible, 1K
SET_FREQ1KHZa:
	tst		r0						;check for zero
	breq	SET_FREQ100HZ			;is zero, so skip
	ldi		BCD0a,LOW(_1KHz)		;add up the count values
	adc		addrl,BCD0a
	ldi		BCD1a,BYTE2(_1KHz)
	adc		addrm,BCD1a
	ldi		BCD2a,BYTE3(_1KHz)
	adc		addrh,BCD2a
	dec		r0						;dec register
	rjmp	SET_FREQ1KHZa			;and loop till zero

SET_FREQ100HZ:
	mov		r0,BCD1					;get upper digits, 1K and 100
	swap	r1
	and		r0,r1					;strip high nible, 100
SET_FREQ100HZa:
	tst		r0						;check for zero
	breq	SET_FREQ10HZ			;is zero, so skip
	ldi		BCD0a,LOW(_100Hz)		;add up the count values
	adc		addrl,BCD0a
	ldi		BCD1a,BYTE2(_100Hz)
	adc		addrm,BCD1a
	ldi		BCD2a,BYTE3(_100Hz)
	adc		addrh,BCD2a
	dec		r0						;dec register
	rjmp	SET_FREQ100HZa			;and loop till zero

SET_FREQ10HZ:
	mov		r0,BCD0					;get low digits, 10 and 1
	swap	r1
	and		r0,r1					;strip low nible
	swap	r0						;and swap nible, 10
SET_FREQ10HZa:
	tst		r0						;check for zero
	breq	SET_FREQ1HZ			;is zero, so skip
	ldi		BCD0a,LOW(_10Hz)		;add up the count values
	adc		addrl,BCD0a
	ldi		BCD1a,BYTE2(_10Hz)
	adc		addrm,BCD1a
	ldi		BCD2a,BYTE3(_10Hz)
	adc		addrh,BCD2a
	dec		r0						;dec register
	rjmp	SET_FREQ10HZa			;and loop till zero

SET_FREQ1HZ:
	mov		r0,BCD0					;get upper digits, 10 and 1
	swap 	r1
	and		r0,r1					;strip high nible, 1
SET_FREQ1HZa:
	tst		r0						;check for zero
	breq	SET_FREQ_OUT			;is zero, so skip
	ldi		BCD0a,LOW(_1Hz)			;add up the count values
	adc		addrl,BCD0a
	ldi		BCD1a,BYTE2(_1Hz)
	adc		addrm,BCD1a
	ldi		BCD2a,BYTE3(_1Hz)
	adc		addrh,BCD2a
	dec		r0						;dec register
	brne	SET_FREQ1HZa			;and loop till zero

SET_FREQ_OUT:
	ret

;***************************************************************************
;* "BCDadd" - 6-digit packed BCD addition
;*
;* This subroutine adds the two unsigned 6-digit BCD numbers held in 
;*   "BCD0a", "BCD1a" and "BCD2a" to those in "BCD0", "BCD1", and "BCD2".
;* The result is returned in "BCD0", "BCD1" and "BCD2".  The overflow
;*   carry in "BCDt".
;***************************************************************************

BCDadd:
	clr		BCDt					;clear the overflow bit
	ldi		temp,6					;value to be added later
	mov		r0,temp					;save it in r0
	mov		temp,BCD0				;set up for LSD
	mov		temp2,BCD0a
	rcall	BCDadd_10				;call the routine
	mov		BCD0,temp				;put the new digit back

	swap	r0						;restore add value
	add		BCD1a,BCDt				;we need to add the carry from the prior operation
	mov		temp,BCD1				;set up for MMD digit
	mov		temp2,BCD1a
	rcall	BCDadd_10				;call the routine
	mov		BCD1,temp				;put the new digit back

	swap	r0						;restore add value
	add		BCD2a,BCDt				;we need to add the carry from the prior operation
	mov		temp,BCD2				;set up for MSD
	mov		temp2,BCD2a
	rcall	BCDadd_10				;call the routine
	mov		BCD2,temp				;put the new digit back

	ret

BCDadd_10:
	add		temp,temp2				;add the numbers binary
	clr		BCDt					;clear BCD carry
	brcc	BCDadd_11				;if carry not clear
	ldi		BCDt,1					;set BCD carry
BCDadd_11:
	brhs	BCDadd_12				;if half carry not set
	add		temp,r0					;add 6 to LSD
	brhs	BCDadd_13				;if half carry not set (LSD <= 9)
	subi	temp,6					;restore value
	rjmp	BCDadd_13				;else
BCDadd_12:
	add		temp,r0					;add 6 to LSD
BCDadd_13:
	brcc	BCDadd_14
	ldi		BCDt,1	
BCDadd_14:
	swap	r0
	add		temp,r0					;add 6 to MSD
	brcs	BCDadd_15				;if carry not set (MSD <= 9)
	sbrs	BCDt,0					;if previous carry not set
	subi	temp,$60				;restore value
	rjmp	BCDadd_16				;next set of digits
BCDadd_15:
	ldi		BCDt,1					;set BCD carry
BCDadd_16:
	ret

;***************************************************************************
;*
;* "BCDsub" - 6-digit packed BCD subtraction
;*
;* This subroutine subtracts the two unsigned 6-digit BCD numbers held in 
;*   "BCD0a", "BCD1a" and "BCD2a" from those in "BCD0", "BCD1", and "BCD2".
;* The result is returned in "BCD0", "BCD1" and "BCD2".  The underflow
;*   carry is in "BCDt".
;***************************************************************************

BCDsub:
	clr		BCDt					;clear the underflow bit
	mov		temp,BCD0				;set up for LSD
	mov		temp2,BCD0a
	rcall	BCDsub_10				;call the routine
	mov		BCD0,temp				;put the new digit back
	add		BCD1a,BCDt				;add borrow to next digit

	mov		temp,BCD1				;set up for MMD digit
	mov		temp2,BCD1a
	rcall	BCDsub_10				;call the routine
	mov		BCD1,temp				;put the new digit back
	add		BCD2a,BCDt				;add borrow to next digit

	mov		temp,BCD2				;set up for MSD
	mov		temp2,BCD2a
	rcall	BCDsub_10				;call the routine
	mov		BCD2,temp				;put the new digit back

	clr		BCD0a					;clear add/subract regs
	clr		BCD1a
	clr		BCD2a

	ret

BCDsub_10:
	sub		temp,temp2				;subtract the numbers binary
	clr		BCDt
	brcc	BCDsub_11				;if carry not clear
	ldi		BCDt,1					;store carry in BCD0t, bit 0
BCDsub_11:
	brhc	BCDsub_12				;if half carry not clear
	subi	temp,0x06				;LSD = LSD - 6
BCDsub_12:
	sbrs	BCDt,0					;if previous carry set
	rjmp	BCDsub_13
	subi	temp,0x60				;subtract 6 from MSD
	ldi		BCDt,1					;set underflow carry
	brcc	BCDsub_13				;if carry not clear
	ldi		BCDt,1					;clear underflow carry	
BCDsub_13:	
	ret

;***********************************************************************
;Re-Initializes the counters then returns
;***********************************************************************
RE_INIT:
;set power-up waveform output as default
	ldi		ZH,HIGH(2*sine)			;setup Z pointer hi
	ldi		ZL,LOW(2*sine)			;setup Z pointer lo

	ldi		BCD0,0x00				;set default frequency count
	ldi		BCD1,0x10
	ldi		BCD2,0x00
	rcall	SET_FREQ				;set the frequency registers

	ldi		temp,MSG_HELLO			;set string pointer
	rcall	send_string				;spit it out

	ret

;***********************************************************************
;Include the Bin2BCD routine
;***********************************************************************
.include	"Include\Bin2BCD.asm"

;***********************************************************************
;DELAYS 100mS AND THEN RETURNS - USES NO INTERUPTS
;***********************************************************************
DELAY250:
	_WAIT_10mS	fclk,25				;Delay 250ms
DELAY250_OUT:
	ret								;return from subroutine

;***********************************************************************
;Reads Sequential "Message" data from EEPROM.
; Address pointer is in register "EEaddr"
; returns data in EEdata
;***********************************************************************
EERead_seq:
	out		EEAR,EEaddr				;output address low byte
	sbi		EECR,EERE				;set EEPROM Read strobe
									;This instruction takes 4 clock cycles since
									;it halts the CPU for two clock cycles
	in		EEdata,EEDR				;get data
	ret

;--------------------------------------- 
;Interrupt Handlers
;--------------------------------------- 
EXT_INT1:						;IRQ1 Handler
;	reti
TIM_CAPT1:						;Timer1 Capture Handler
;	reti
TIM_COMP1:						;Timer1 Compare Handler
;	reti
TIM_OVF1:						;Timer1 Overflow Handler
;	reti
TIM_OVF0:						;Timer0 Overflow Handler
;	reti
UART_DRE:						;UDR Empty Handler
;	reti
UART_TXC:						;UART TX Complete Handler
;	reti
ANA_COMP:						;Analog Comparator Handler
;	reti
NO_VEC:	
;	reti

;******************************************************************************
;Include Waveform Data Tables
;******************************************************************************

.org	wavtbl
;.include "datatable.asm"		;Waveform data tables
.include "sine.asm"				;Waveform data tables
.include "square.asm"			;Waveform data tables
.include "triangle.asm"			;Waveform data tables
.include "sawtooth.asm"			;Waveform data tables
;.include "rev-sawtooth.asm"		;Waveform data tables
;.include "white.asm"			;Waveform data tables

;******************************************************************************
;Include Message Data Tables in EEPROM
;******************************************************************************
.include	"Include\EEProm_Data.inc"

;******************************************************************************
;end of file	
;******************************************************************************
