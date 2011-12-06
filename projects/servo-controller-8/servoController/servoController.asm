;switch compA and compB (A acts as top) 
;
; atmega88
; 18.432 MHz
;o  include, def, and equ
.include	"m88def.inc"

.def temp = r16
.def	spiData = r17
.def	resolutionCounter = r18

; flags
.def	flags = r19			
	
; bit0 - spia, 0 spi not received / 1 spi received
; bit1 - spia, 0 lsbyte ready / 1 msbyte ready
; bit2 - UNDEFINED
; bit3 - UNDEFINED
; bit4 - UNDEFINED
; bit5 - UNDEFINED
; bit6 - UNDEFINED
; bit7 - UNDEFINDE

.equ	spiaMask		=	0b00000001	
.equ 	spiaBit		=	0
.equ	spibMask		=	0b00000010
.equ	spibBit		=	1
;.equ	n1mSecMask	=	0b00000100
;.equ	n18mSecMask	=	0b00001000
.equ servoPorta	=	portd
.equ resolution	=	8
.equ _1mSecWait	=	2303
.equ _18mSecWait	=	41471
 ;c
.cseg
.org 0x00
;o reset vectors
rjmp reset				; Reset
; rjmp reset				; int0
; rjmp reset				; int1
; rjmp reset				; pcint0
; rjmp reset				; pcint1
; rjmp reset				; pcint2
; rjmp reset				; wdt
; rjmp reset				; timer2compA
; rjmp reset				; timer2compB
; rjmp reset				; timer2ovf
; rjmp reset				; timer1capt
.org 0x0b
rjmp int_timer1compA		; timer1compA
rjmp int_timer1compB		; timer1compB
; rjmp reset				; timer1Ovf
; rjmp reset				; timer0compA
; rjmp reset				; timer0compB
; rjmp reset				; timer0ovf
.org 0x11
rjmp int_spi				; spi
; rjmp reset				; usart rx
; rjmp reset				; usart udre
; rjmp reset				; usart tx
; rjmp reset				; adc
; rjmp reset				; eeReady
;c
;o reset
reset:
	; initialize stack
	ldi temp, low(ramend)
	out spl, temp
	ldi temp, high(ramend)
	out sph, temp

	; set portd for all output
	ldi temp, 0xff	
	out ddrd, temp
	
	; enable global interupts
	sei				
	; initialize timer1
	ldi temp, (1<<wgm12)|(1<<cs11)		; ctc fclk/8
	sts tccr1b, temp
	ldi temp, (1<<ocie1a)				; compA enabled
	sts timsk1, temp
	; initialize spi
	ldi temp, (1<<spie)|(1<<spe)	; interrupt enabled, spi enabled, msb first, slave mode, sample rising edge
	out spcr, temp				; setup falling edge, Fosc/4, mosi, sck, and ss pin functions are overidden as inputs
	
	
	
;c
;o prime
prime:
		
	ldi temp, high(_1mSecWait)	; load timer/counter1 compare registiters
	sts ocr1ah, temp 
	ldi temp, low(_1mSecWait)
	sts ocr1al, temp
	ldi temp, high(_18mSecWait)
	sts ocr1bh, temp
	ldi temp, low(_18mSecWait)
	sts ocr1bl, temp
	
	ldi r27, 0x01				; all servos at zero
	ldi temp, 0
	st x+, temp
	st x+, temp
	st x+, temp
	st x+, temp
	st x+, temp
	st x+, temp
	st x+, temp
	st x, temp
	ldi r26, 0x00
	
	ldi temp, 0xff
	out servoPorta, temp
	;c
;o main

main:
	sbrc flags, spiaBit
		rcall parse
	rjmp main
;c
;o subroutines
;o parse
parse:
	cbr flags, spiaMask
	sbrc flags, spibBit		; if lsbyte then set pointer to table, else store data in table
		rjmp storeData 
	add r26, spidata
	sbr flags, spibMask
	ret
	storeData:
		st x, spidata
		ldi r26, 0x00			; restore pointer		
		cbr flags, spibMask
		ret
;c
;c 
;o interupts
;o int_spi
int_spi:
	in spiData , spdr			; move data into register, then flip flag bit
	sbr flags, spiaMask	
	reti
;c
;o int_timer1compA
int_timer1compA:
	ldi temp, high(resolution)
	sts ocr1ah, temp
	ldi temp, low(resolution)		; set resolution to 1/256 mSecs
	sts ocr1al, temp			
	cpi resolutionCounter, 255	; after 2mSecs goto 18 mSecs wait mode
	breq compAoff
;o servoOut:
servoOut:
	servo0:
		ld temp, x+
		cpse temp, resolutionCounter
		rjmp servo1
		cbi servoPorta, 0
	servo1:
		ld temp, x+
		cpse temp, resolutionCounter
		rjmp servo2
		cbi servoPorta, 1
	servo2:
		ld temp, x+
		cpse temp, resolutionCounter
		rjmp servo3
		cbi servoPorta, 2
	servo3:
		ld temp, x+
		cpse temp, resolutionCounter
		rjmp servo4
		cbi servoPorta, 3
	servo4:
		ld temp, x+
		cpse temp, resolutionCounter
		rjmp servo5
		cbi servoPorta, 4
	servo5:
		ld temp, x+
		cpse temp, resolutionCounter
		rjmp servo6
		cbi servoPorta, 5
	servo6:
		ld temp, x+
		cpse temp, resolutionCounter
		rjmp servo7
		cbi servoPorta, 6
	servo7:
		ld temp, x
		cpse temp, resolutionCounter
		rjmp serovOutCleanUp
		cbi servoPorta, 7
		serovOutCleanUp:
			ldi r26, 0x00				; restore xpointer to beginning of sram		
;c
	inc resolutionCounter
	reti
	compAoff:
		inc resolutionCounter
		ldi temp, 0xff
		sts ocr1ah, temp
		sts ocr1al, temp
		ldi temp, (1<<ocie1B)	; compareA is now off
		sts timsk1, temp
		reti
;c
;o int_timer1compB
int_timer1compB:
	ldi temp, 0x00
	sts tcnt1h, temp
	sts tcnt1l, temp
	ldi temp, (1<<ocie1A)		; turn compareA back on
	sts timsk1, temp
	ldi temp, high(_1mSecWait)	; restore compareA to 1mSecWait
	sts ocr1ah, temp 
	ldi temp, low(_1mSecWait)
	sts ocr1al, temp
	ldi temp, 0xff			; pull all servo's high
	out servoPorta, temp
	reti
	
;c
;c
