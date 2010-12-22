;****************************************************************************************
;*written by				:Luca Magnelli												*
;*Title						:Digital Voltmeter based on ADS1210							*
;*File name					:Voltmetro_2												*
;*Date						:															*
;*Target MCU				:ATmega8535L												*
;*Clock Frequency			:3.69MHz													*
;*Program function			:															*
;****************************************************************************************

;========================================================================================
;Directives

.nolist                      		
.include "C:\Programmi\Atmel\Avr Tools\Avrassembler\appnotes\m8535def.inc"
.list

.def	oldCMR2	=r12

.def	DOR2	=r13
.def	DOR1	=r14
.def	DOR0	=r15

.def	CMR3	=r16
.def	CMR2	=r17
.def	CMR1	=r18
.def	CMR0	=r19

.def	INSR	=r20

.def	temp1	=r21
.def	temp2	=r22
.def	temp3	=r23	;clear INT0 flag
.def	all1	=r24
.def	all0	=r25

.equ	t1H		=0x1	;t1=300		Xin to wait in the reset of ADS1210
.equ	t1L		=0x2C
.equ	t3H		=0x2	;t3=600 	Xin
.equ	t3L		=0x58
.equ	t4H		=0x4	;t4=1050	Xin
.equ	t4L		=0x1A

.equ	zero	=0x30	;ASCII code for '0'
.equ	uno		=0x31	;ASCII code for '1'
.equ	due		=0x32	;ASCII code for '2'
.equ	tre		=0x33	;ASCII code for '3'
.equ	quattro	=0x34	;ASCII code for '4'
.equ	cinque	=0x35	;ASCII code for '5'
.equ	ssei	=0x36	;ASCII code for '6'
.equ	sette	=0x37	;ASCII code for '7'
.equ	otto	=0x38	;ASCII code for '8'
.equ	nove	=0x39	;ASCII code for '9'
.equ	dot		=0x2E	;ASCII code for '.'	

.equ	uguale	=0x3D	;ASCII code for '='

.equ	m		=0x6D	;ASCII code for 'm'
.equ	g		=0x67	;ASCII code for 'g'
.equ	t		=0x74	;ASCII code for 't'
.equ	d		=0x64	;ASCII code for 'd'

.equ	space 	=0x20	;ASCII code for ' '
.equ	V		=0x56	;ASCII code for 'V'
.equ	CR		=0x0D	;ASCII code for carriage return
.equ	LF		=0x0A	;ASCII code for line feed

;Vfs=5V -> 24-bit values (High-Medium-Low Part @8-bit)	

.equ	VH51			=0b00011001		;1V	
.equ	VM51			=0b10011001
.equ	VL51			=0b10011001

.equ	VH501			=0b00000010		;0.1V
.equ	VM501			=0b10001111
.equ	VL501			=0b01011100

.equ	VH5001			=0b00000000		;0.01V
.equ	VM5001			=0b01000001
.equ	VL5001			=0b10001001

.equ	VH50001			=0b00000000		;0.001V
.equ	VM50001			=0b00000110
.equ	VL50001			=0b10001101

.equ	VH500001		=0b00000000		;0.0001V
.equ	VM500001		=0b00000000
.equ	VL500001		=0b10100111

.equ	VH5000001		=0b00000000		;0.00001V
.equ	VM5000001		=0b00000000
.equ	VL5000001		=0b00010000

.equ	VH50000001		=0b00000000		;0.000001V
.equ	VM50000001		=0b00000000
.equ	VL50000001		=0b00000001

;Vfs=2.5V -> 24-bit values (High-Medium-Low Part @8-bit)	

.equ	VH251			=0b00110011		;1V	
.equ	VM251			=0b00110011
.equ	VL251			=0b00110010

.equ	VH2501			=0b00000101		;0.1V
.equ	VM2501			=0b00011110
.equ	VL2501			=0b10111000

.equ	VH25001			=0b00000000		;0.01V
.equ	VM25001			=0b10000011
.equ	VL25001			=0b00010010

.equ	VH250001		=0b00000000		;0.001V
.equ	VM250001		=0b00001101
.equ	VL250001		=0b00011011

.equ	VH2500001		=0b00000000		;0.0001V
.equ	VM2500001		=0b00000001
.equ	VL2500001		=0b01001111

.equ	VH25000001		=0b00000000		;0.00001V
.equ	VM25000001		=0b00000000
.equ	VL25000001		=0b00100001

.equ	VH250000001		=0b00000000		;0.000001V
.equ	VM250000001		=0b00000000
.equ	VL250000001		=0b00000011

;Vfs=1.25V -> 24-bit values (High-Medium-Low Part @8-bit)	

.equ	VH1251			=0b01100110		;1V	
.equ	VM1251			=0b01100110
.equ	VL1251			=0b01100100

.equ	VH12501			=0b00001010		;0.1V
.equ	VM12501			=0b00111101
.equ	VL12501			=0b01110000

.equ	VH125001		=0b00000001		;0.01V
.equ	VM125001		=0b00000110
.equ	VL125001		=0b00100100

.equ	VH1250001		=0b00000000		;0.001V
.equ	VM1250001		=0b00011010
.equ	VL1250001		=0b00110110

.equ	VH12500001		=0b00000000		;0.0001V
.equ	VM12500001		=0b00000010
.equ	VL12500001		=0b10011111

.equ	VH125000001		=0b00000000		;0.00001V
.equ	VM125000001		=0b00000000
.equ	VL125000001		=0b01000011

.equ	VH1250000001	=0b00000000		;0.000001V
.equ	VM1250000001	=0b00000000
.equ	VL1250000001	=0b00000110

;========================================================================================
; Interrupt vector table
.cseg
.org	0x00

		rjmp	RESET			; Reset Handler
 		rjmp	Data_Ready		; IRQ0 Handler
 		reti					; IRQ1 Handler
 		reti					; Timer2 Compare Handler
 		reti					; Timer2 Overflow Handler
 		reti					; Timer1 Capture Handler
 		reti					; Timer1 Compare A Handler
 		reti					; Timer1 Compare B Handler
 		reti					; Timer1 Overflow Handler
 		reti					; Timer0 Overflow Handler
 		reti					; SPI Transfer Complete Handler
 		reti					; USART RX Complete Handler
 		reti					; UDR Empty Handler
 		reti					; USART TX Complete Handler
 		reti					; ADC Conversion Complete Handler
 		reti					; EEPROM Ready Handler
 		reti					; Analog Comparator Handler
 		reti					; Two-wire Serial Interface Handler
 		reti					; IRQ2 Handler
 		reti					; Timer0 Compare Handler
 		reti					; Store Program Memory Ready Handler

;========================================================================================
;Interrupt handler

;Receive data from ADS1210 
Data_Ready:
		
		ldi		INSR,0b11000000			;INSR code for reading DOR
		
		rcall	SendINSR		

		rcall	RxDOR

		ldi		XL,0x75					;start from 10^0 counter
										;clear counters
		st		X+,all0					;10^0 V	counter
		st		X+,all0					;10^-1V	counter
		st		X+,all0					;10^-2V	counter
		st		X+,all0					;10^-3V	counter
		st		X+,all0					;10^-4V	counter
		st		X+,all0					;10^-5V	counter
		st		X+,all0					;10^-6V	counter
		
		mov		temp1,CMR2				;A seconda del Gain utilizzato utilizza per i confronti
		andi	temp1,0b00011100		;l'esatta codifica binaria per 1 0.1 0.001 etc. V
		cpi		temp1,0b00000000
		breq	SetG1
		cpi		temp1,0b00000100
		breq	SetG2
		cpi		temp1,0b00001000
		breq	SetG4
SetG1:	ldi		temp1,0x60				;Vfs=5V values
		rjmp	LoadZ
SetG2:	ldi		temp1,0x8B				;Vfs=1.5V values
		rjmp	LoadZ
SetG4:	ldi		temp1,0xA0				;Vfs=1.25V values

LoadZ:	mov		ZL,temp1				;1 V MSByte 
		
		ldi		XL,0x75					;10^0 V	counter
		mov		temp2,all0

ctrl:	ld		temp1,Z+				;confronto con la Low Part
		cp		DOR0,temp1
		ld		temp1,Z+				;confronto con la Medium Part
		cpc		DOR1,temp1
		ld		temp1,Z+				;confronto con la Medium Part
		cpc		DOR2,temp1
		brmi	lowCTR					;se il numero in DOR è minore di quello di confronto => diminuisci il valore di confronto
		subi	ZL,3					;riporta Z alla Low Part per eseguire la sottrazione					
		inc		temp2					;incrementa il registro che andrà nel counter D0-1-2-3-4-5-6
		rjmp	SUBvlt

lowCTR:	st		X+,temp2				;memorizza in Di e punta al counter Di+1
		mov		temp2,all0
		cpi		XL,0x7C					;l'ultimo counter da memorizzare è in XL=0x7B
		breq	txRES
		rjmp	ctrl

SUBvlt:	ld		temp1,Z+				;Low Part Subtract			
 		sub		DOR0,temp1
		
		ld		temp1,Z+				;Medium Part Subtract
		sbc		DOR1,temp1

		ld		temp1,Z+				;High Part Subtract
		sbc		DOR2,temp1

		subi	ZL,3					;riporta Z alla Low Part per eseguire il confronto
		rjmp	ctrl

TxRES:	rcall	TxDOR2PC
		
		reti

;========================================================================================
;Initialization of ATmega8535 & ADS1210

Init:	ser		all1
		clr		all0
		ldi		temp3,0b01000000		;bit sequence for INTF0 reset

;SRAM initialization
		
		ldi		ZL,0x60

;Vfs=5V values

		ldi		temp1,VL51				;0x60
		st		Z+,temp1
		ldi		temp1,VM51				;0x61
		st		Z+,temp1				
		ldi		temp1,VH51				;0x62
		st		Z+,temp1		

		ldi		temp1,VL501				;0x63
		st		Z+,temp1
		ldi		temp1,VM501				;0x64
		st		Z+,temp1				
		ldi		temp1,VH501				;0x65
		st		Z+,temp1
		
		ldi		temp1,VL5001			;0x66
		st		Z+,temp1
		ldi		temp1,VM5001			;0x67
		st		Z+,temp1				
		ldi		temp1,VH5001			;0x68
		st		Z+,temp1

		ldi		temp1,VL50001			;0x69
		st		Z+,temp1
		ldi		temp1,VM50001			;0x6A
		st		Z+,temp1			
		ldi		temp1,VH50001			;0x6B
		st		Z+,temp1

		ldi		temp1,VL500001			;0x6C
		st		Z+,temp1
		ldi		temp1,VM500001			;0x6D
		st		Z+,temp1				
		ldi		temp1,VH500001			;0x6E
		st		Z+,temp1

		ldi		temp1,VL5000001			;0x6F
		st		Z+,temp1
		ldi		temp1,VM5000001			;0x70
		st		Z+,temp1				
		ldi		temp1,VH5000001			;0x71
		st		Z+,temp1

		ldi		temp1,VL50000001		;0x72
		st		Z+,temp1
		ldi		temp1,VM50000001		;0x73
		st		Z+,temp1				
		ldi		temp1,VH50000001		;0x74
		st		Z+,temp1

;Counters 

		st		Z+,all0					;0x75	10^0	V counter location
		st		Z+,all0					;0x76	10^-1 	V counter location
		st		Z+,all0					;0x77	10^-2	V counter location
		st		Z+,all0					;0x78	10^-3	V counter location
		st		Z+,all0					;0x79	10^-4	V counter location
		st		Z+,all0					;0x7A	10^-5	V counter location
		st		Z+,all0					;0x7B	10^-6	V counter location

		ldi		ZL,0x7F

;Xin to wait in the reset of ADS1210

		ldi		temp1,t3H				;0x7F
		st		Z+,temp1
		ldi		temp1,t3L				;0x80
		st		Z+,temp1
		ldi		temp1,t1H				;0x81
		st		Z+,temp1
		ldi		temp1,t1L				;0x82
		st		Z+,temp1
		ldi		temp1,t3H				;0x83
		st		Z+,temp1
		ldi		temp1,t3L				;0x84
		st		Z+,temp1
		ldi		temp1,t4H				;0x85
		st		Z+,temp1
		ldi		temp1,t4L				;0x86
		st		Z+,temp1

		ldi		temp1,space				;0x87
		st		Z+,temp1
		ldi		temp1,V					;0x88
		st		Z+,temp1
		ldi		temp1,CR				;0x89
		st		Z+,temp1
		ldi		temp1,LF				;0x8A
		st		Z+,temp1

;Vfs=2.5V values

		ldi		temp1,VL251				;0x8B
		st		Z+,temp1
		ldi		temp1,VM251				;0x8C
		st		Z+,temp1				
		ldi		temp1,VH251				;0x8D
		st		Z+,temp1		

		ldi		temp1,VL2501			;0x8E
		st		Z+,temp1
		ldi		temp1,VM2501			;0x8F
		st		Z+,temp1				
		ldi		temp1,VH2501			;0x90
		st		Z+,temp1
		
		ldi		temp1,VL25001			;0x91
		st		Z+,temp1
		ldi		temp1,VM25001			;0x92
		st		Z+,temp1				
		ldi		temp1,VH25001			;0x93
		st		Z+,temp1

		ldi		temp1,VL250001			;0x94
		st		Z+,temp1
		ldi		temp1,VM250001			;0x95
		st		Z+,temp1			
		ldi		temp1,VH250001			;0x96
		st		Z+,temp1

		ldi		temp1,VL2500001			;0x97
		st		Z+,temp1
		ldi		temp1,VM2500001			;0x98
		st		Z+,temp1				
		ldi		temp1,VH2500001			;0x99
		st		Z+,temp1

		ldi		temp1,VL25000001		;0x9A
		st		Z+,temp1
		ldi		temp1,VM25000001		;0x9B
		st		Z+,temp1				
		ldi		temp1,VH25000001		;0x9C
		st		Z+,temp1

		ldi		temp1,VL250000001		;0x9D
		st		Z+,temp1
		ldi		temp1,VM250000001		;0x9E
		st		Z+,temp1				
		ldi		temp1,VH250000001		;0x9F
		st		Z+,temp1

;Vfs=1.25V values

		ldi		temp1,VL1251			;0xA0
		st		Z+,temp1
		ldi		temp1,VM1251			;0xA1
		st		Z+,temp1				
		ldi		temp1,VH1251			;0xA2
		st		Z+,temp1		

		ldi		temp1,VL12501			;0xA3
		st		Z+,temp1
		ldi		temp1,VM12501			;0xA4
		st		Z+,temp1				
		ldi		temp1,VH12501			;0xA5
		st		Z+,temp1
		
		ldi		temp1,VL125001			;0xA6
		st		Z+,temp1
		ldi		temp1,VM125001			;0xA7
		st		Z+,temp1				
		ldi		temp1,VH125001			;0xA8
		st		Z+,temp1

		ldi		temp1,VL1250001			;0xA9
		st		Z+,temp1
		ldi		temp1,VM1250001			;0xAA
		st		Z+,temp1			
		ldi		temp1,VH1250001			;0xAB
		st		Z+,temp1

		ldi		temp1,VL12500001		;0xAC
		st		Z+,temp1
		ldi		temp1,VM12500001		;0xAD
		st		Z+,temp1			
		ldi		temp1,VH12500001		;0xAE
		st		Z+,temp1

		ldi		temp1,VL125000001		;0xAF
		st		Z+,temp1
		ldi		temp1,VM125000001		;0xB0
		st		Z+,temp1				
		ldi		temp1,VH125000001		;0xB1
		st		Z+,temp1

		ldi		temp1,VL1250000001		;0xB2
		st		Z+,temp1
		ldi		temp1,VM1250000001		;0xB3
		st		Z+,temp1				
		ldi		temp1,VH1250000001		;0xB4
		st		Z+,temp1

;ADS1210 reset handling (send a sequence of bit from PB7 to SCLK)

		ldi		ZL,0x7F					;t3H location	
		ldi 	temp2,(1<<CS10)			;turn on T/C1 with freq. Xin  
		out 	TCCR1B,temp2			;Prescaler @ Xin/1
		ldi		temp2,4					;3 cycles
						
NEWtHL:	ld		temp1,Z+				;load t1-3-4H
		out		OCR1AH,temp1
		ld		temp1,Z+				;load t1-3-4L
		out		OCR1AL,temp1
		out		TIFR,all1				;clear output compare flag 1
		out		TCNT1H,all0				;clear timer 1
		out		TCNT1L,all0
		sbi		PORTB,7					;set PB7 for t1-3-4
STP134:	in		temp1,TIFR
		sbrs	temp1,4					;wait until T/C1 reaches t1-3-4
		rjmp	STP134
		
		cbi		PORTB,7					;clear PB7 at least 6 Xin
		dec		temp2
		brne	NEWtHL

		ldi		temp1,(0<<CS10)			;stop timer/counter 1
		out 	TCCR1B,temp1			
		out		TIFR,all1				;clear output compare flag 1

;Initialize USART PC<-->ATmega8535 communication

		ldi		temp1,(0<<UDRE)|(0<<U2X);Set normal(not double) speed
		out		UCSRA,temp1
		ldi		temp1,0					;Set baud rate (115.2 kbps @ 3.69MHz internal clock)
		out		UBRRH,temp1				
		ldi		temp1,1					;in simulation set UBBRL to 0 instead of 1
		out		UBRRL,temp1
		ldi		temp1,(1<<RXEN)|(1<<TXEN);Enable Rx complete Interrupt,Receiver,Transmitter
		out		UCSRB,temp1
		ldi		temp1,(1<<URSEL)|(1<<USBS)|(3<<UCSZ0);Set frame format:8data bit, 
										;2stop bit,Asynchronous operation,no Parity mode
		out		UCSRC,temp1

;Enable External Interrupt 0 (PD2->Pin #16) for DATA READY (falling edge)

		out 	DDRD,all0
		ldi		temp1,(2<<ISC00)		;Interrupt on falling edge of INT0
		out		MCUCR,temp1
		ldi		temp1,(1<<INT0)			;External Interrupt 0 enable
		out		GICR,temp1
				
;Initialization of ADS1210 internal register replica
		
		ldi		INSR,0b01100100			;set INSR for CMR2:0 writing
		ldi		CMR3,0b01000000			;Bias off,REFout enabled,2's complement,Bipolar bit
										;MSByte first,MSB first,SDOUT on
		ldi		CMR2,0b00000000			;set Normal mode,Gain=1
		ldi		CMR1,0b10011111			;Turbo mode rate=16
		ldi		CMR0,0b11111111			;Decimation ratio=8191
		;Sampling frequency	= 	115.3kHz
		;Modulator frequency=	115.3kHz
		;Data frequency		=	14.1Hz		

		ret

;=============================================================================================		
;Subroutines

;Send Register values for ADS1210 initialization
SETT:	
		in		temp1,GIFR				
		or		temp1,temp3				
		out		GIFR,temp1				;clear INTF0
		
		mov		temp1,oldCMR2			;if ADS1210 was in sleep mode don't wait
		andi	temp1,0b11100000		;falling edge of DRDY to send INSR
		cpi		temp1,0b11000000
		breq	noDRDY

WDRDY:	in		temp1,GIFR				
		sbrs	temp1,6					;Wait falling edge of DRDY													
		rjmp	WDRDY					
		in		temp1,GIFR				
		or		temp1,temp3				
		out		GIFR,temp1				;clear INTF0
		nop								;Wait falling edge of DRDY to first rising edge of SCLK
		
noDRDY:	rcall	SendINSR
		rcall	SendCMR
		ret

;Send INSR to ADS1210

SendINSR:
		ldi		temp1,0b10100000
		out		DDRB,temp1				;PB7,PB5 output (PB7->SCLK,PB5->SDIO)
		ldi		temp1,8
		
clkL:	cbi		PORTB,7					;clk Low
		rol		INSR					;MSB in Carry Flag
		brcs	Send1					;if Carry set => send 1
		cbi		PORTB,5					;if Carry clear => send 0
		rjmp	clkH
Send1:	sbi		PORTB,5
clkH:	sbi		PORTB,7					;clk High
		nop
		nop
		dec		temp1
		brne	clkL
		cbi		PORTB,7
		ret

;Send CMR to ADS1210

SendCMR:
		ldi		temp1,0b10100000
		out		DDRB,temp1				;PB7,PB5 output (PB7->SCLK,PB5->SDIO)
		ldi		temp1,8					;send 8 bit
		ldi		temp2,4					;send 4 byte
		ldi		ZL,16					;CMR3 SRAM address
		nop
		ld		temp3,Z+				;load CMR3, Z<-Z+1
		
clkL2:	cbi		PORTB,7					;clk Low
		rol		temp3					;MSB in Carry flag
		brcs	Send12					;if Carry set => send 1
		cbi		PORTB,5					;if Carry clear => send 0
		rjmp	clkH2
Send12:	sbi		PORTB,5
clkH2:	sbi		PORTB,7					;clk High
		nop
		nop
		dec		temp1
		brne	clkL2

		ld		temp3,Z+				;load next CMR		
		ldi		temp1,8
		dec		temp2
		brne	clkL2
		cbi		PORTB,7

		ldi		temp3,0b01000000
		ret

;Receive DOR from ADS1210->ATmega8535

RxDOR:	
		ldi		temp1,0b10000000
		out		DDRB,temp1				;PB7->SCLK,PB5<-SDIO
		ldi		temp1,8					;receive 8 bit
		ldi		ZL,13					;DOR2 SRAM address
		nop

clkH3:	sbi		PORTB,7					;serial clock high
		nop
		sbis	PINB,5					;receive 1 bit
		rjmp	Rx0
		sec								;set carry if PINB5 is set
		rjmp	Lshift
Rx0:	clc								;clear carry if PINB5 is clear
Lshift:	rol		temp2					
clkL3:	cbi		PORTB,7
		nop
		nop
		nop
		dec		temp1
		brne	clkH3

		st		Z+,temp2				;store in DOR0:2		
		ldi		temp1,8
		cpi		ZL,16
		brne	clkH3

		mov		temp1,DOR2
		com		temp1					;LED acceso=1
		out		PORTC,temp1				;MSByte to LEDs
		ret

;Transmit DOR received to PC via USART

TxDOR2PC:	
		ldi		ZL,0x75					;D0 address, counter of unit
				
Count:	ld		temp2,Z+				;load Counters
		cpi		temp2,9
		breq	load9
		cpi		temp2,8
		breq	load8
		cpi		temp2,7
		breq	load7
		cpi		temp2,6
		breq	load6
		cpi		temp2,5
		breq	load5
		cpi		temp2,4
		breq	load4
		cpi		temp2,3
		breq	load3
		cpi		temp2,2
		breq	load2
		cpi		temp2,1
		breq	load1
		rjmp	load0

load9:	ldi		temp1,nove
		rjmp	TxPC
load8:	ldi		temp1,otto
		rjmp	TxPC
load7:	ldi		temp1,sette
		rjmp	TxPC
load6:	ldi		temp1,ssei
		rjmp	TxPC
load5:	ldi		temp1,cinque
		rjmp	TxPC
load4:	ldi		temp1,quattro
		rjmp	TxPC
load3:	ldi		temp1,tre
		rjmp	TxPC
load2:	ldi		temp1,due
		rjmp	TxPC
load1:	ldi		temp1,uno
		rjmp	TxPC
load0:	ldi		temp1,zero
		rjmp	TxPC

TxPC:	sbis	UCSRA,UDRE				
		rjmp	TxPC
		out		UDR,temp1
		
		cpi		ZL,0x7C					;10^-6 counter has been sent
		breq	Sent		

		cpi		ZL,0x76					;after 10^0 counter send '.'
		brne	Count
		
		ldi		temp1,dot
TxDOT:	sbis	UCSRA,UDRE				
		rjmp	TxDOT
		out		UDR,temp1
		rjmp	Count

Sent:	rcall	END1
		ret

;Send volt symbol at the end of misure to PC via USART

END1:	ldi		ZL,0x87
		
ldSPV:	ld		temp1,Z+
SPVtx:	sbis	UCSRA,UDRE				;Send space and 'V'
		rjmp	SPVtx
		out		UDR,temp1
		cpi		ZL,0x89
		brne	ldSPV
			
;Send Carriage Return & Line feed to PC via Usart

END:	ldi		ZL,0x89
		
ldCRLF:	ld		temp1,Z+
CRLFtx:	sbis	UCSRA,UDRE				;Send Carriage Return and Line Feed
		rjmp	CRLFtx
		out		UDR,temp1
		cpi		ZL,0x8B
		brne	ldCRLF	
		
		ret

;Receive new settings in CMR from PC

NewCMR:	ldi		temp1,(0<<INT0)			;External Interrupt 0 disable
		out		GICR,temp1
				
		ldi		temp2,uguale
EQtx:	sbis	UCSRA,UDRE				;ATmega8535 invia '=' al PC per indicare che è 
		rjmp	EQtx					;pronto a ricevere l'istruzione
		out		UDR,temp2

getI:	sbis	UCSRA,RXC				;Wait for first character of the 
		rjmp	getI					;instruction to be sent
		in		temp1,UDR				
		
txI:	sbis	UCSRA,UDRE				;visualizza il primo carattere dell' 
		rjmp	txI						;istruzione scelta
		out		UDR,temp1
		
getII:	sbis	UCSRA,RXC				;Wait for second character of the 
		rjmp	getII					;instruction to be sent
		in		temp2,UDR				
		
txII:	sbis	UCSRA,UDRE				;visualizza il secondo carattere dell' 
		rjmp	txII					;istruzione scelta
		out		UDR,temp2		
	
		ldi		INSR,0b01100100			;set INSR for CMR2:0 writing
		cpi		temp1,m					
		breq	MODE					;change mode of operation
		cpi		temp1,g				
		breq	GAIN					;modify gain settings
		cpi		temp1,t
		breq	TURBOm					;modify turbo mode rate
		cpi		temp1,d
		breq	dRATIO					;choose new Decimation Ratio
		rjmp	noCMR					;else return from interrupt without writing CMR					

MODE:	cpi		temp2,zero
		breq	NORMAL					;Normal mode select
		cpi		temp2,uno
		breq	SelfC					;Self Calibration Mode select
		cpi		temp2,ssei
		breq	SLEEPM					;Sleep Mode select
		rjmp	noCMR
NORMAL:	andi	CMR2,0b00011111			;clear MD2:0=000 to Normal Mode
		rjmp	Return
SelfC:	ori		CMR2,0b00100000			;MD2:0=110 -> Sleep Mode
		andi	CMR2,0b00111111
		rjmp	Return
SLEEPM:	ori		CMR2,0b11000000
		andi	CMR2,0b11011111
		rjmp	Return
GAIN:	cpi		temp2,uno
		breq	G1						;PGA=1
		cpi		temp2,due
		breq	G2						;PGA=2
		cpi		temp2,tre		
		breq	G3						;PGA=4
		rjmp	noCMR
G1:		andi	CMR2,0b11100011			;G2:0=000
		rjmp	Return
G2:		ori		CMR2,0b00000100			;G2:0=001
		andi	CMR2,0b11100111
		rjmp	Return
G3:		ori		CMR2,0b00001000			;G2:0=010
		andi	CMR2,0b11101011
		rjmp	Return

TURBOm:	cpi		temp2,uno
		breq	T1						;TMR=16
		cpi		temp2,due
		breq	T2						;TMR=8
		cpi		temp2,tre		
		breq	T3						;TMR=4
		rjmp	noCMR
T1:		ori		CMR1,0b10000000
		andi	CMR1,0b10011111			;SF2:0=100
		rjmp	Return
T2:		ori		CMR1,0b01100000			;SF2:0=011
		andi	CMR1,0b01111111
		rjmp	Return
T3:		ori		CMR1,0b01000000			;SF2:0=010
		andi	CMR1,0b01011111
		rjmp	Return
		
dRATIO:	cpi		temp2,uno
		breq	DR1FFF					;Decimation Ratio =0x1FFF
		cpi		temp2,due
		breq	DR17F9					;Decimation Ratio =0x17F9
		cpi		temp2,tre		
		breq	DR0FF5					;Decimation Ratio =0x0FF5
		cpi		temp2,quattro
		breq	DR03E9					;Decimation Ratio =0x03E9
		cpi		temp2,cinque
		breq	DR01FF					;Decimation Ratio =0x01FF
		cpi		temp2,ssei		
		breq	DR0013					;Decimation Ratio =0x0013
		rjmp	noCMR
DR1FFF:	ori		CMR1,0b00011111
		ldi		CMR0,0xFF			
		rjmp	Return
DR17F9:	ori		CMR1,0b00010111			
		andi	CMR1,0b11110111
		ldi		CMR0,0xF9
		rjmp	Return
DR0FF5:	ori		CMR1,0b00001111			
		andi	CMR1,0b11101111
		ldi		CMR0,0xF5
		rjmp	Return
DR03E9:	ori		CMR1,0b00000011
		andi	CMR1,0b11100011
		ldi		CMR0,0xE9			
		rjmp	Return
DR01FF:	ori		CMR1,0b00000001			
		andi	CMR1,0b11100001
		ldi		CMR0,0xFF
		rjmp	Return
DR0013:	andi	CMR1,0b11100000
		ldi		CMR0,0x13
		rjmp	Return			

Return:	rcall	SETT		
noCMR:	rcall	END
		mov		oldCMR2,CMR2			;save CMR2,useful to exit sleep mode
		ldi		temp1,(1<<INT0)			;External Interrupt 0 enable
		out		GICR,temp1
		in		temp1,GIFR				;clear INTF0,evita che l'interrupt Data_Ready 
		or		temp1,temp3				;sia eseguita dopo questa interrupt in quanto 
		out		GIFR,temp1				;l'intervallo di tempo utile per leggere il dato
										;da DOR è stato utilizzato per scrivere su CMR.
		ret
;========================================================================================
RESET: 
;Initialize Stack Pointer		
		ldi		temp1,low(RAMEND)
		out		SPL,temp1
		ldi		temp1,high(RAMEND)
		out		SPH,temp1

		sbi		DDRB,7					;PB7 (SCK) as output for RESET
		cbi		PORTB,7
		out		DDRC,all1
			
		rcall	Init					;Set Up sequence

		rcall	SETT					;Send new CMR values to ADS1210
		
		sei								;start DOR2:0 reading

loop:	sbis	UCSRA,RXC				;Wait for first character of the 
		rjmp	loop					;instruction to be sent
		rcall	NewCMR
		rjmp	loop