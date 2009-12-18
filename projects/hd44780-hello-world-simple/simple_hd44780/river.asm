; This project implements an LCD driver for a 16x2 display based on the HD44780 controller.

; definitions file for the ATMega8515
.include "m8515def.inc"
; my .inc file with some definitions and a character table
.include "lcd_44780_driver.inc"

; the .org directive sets vector jump locations
.org $0000
	rjmp Reset
.org INT0addr
	rjmp IntV0	; External Interrupt Request 0
.org INT1addr
	rjmp IntV1	; External Interrupt Request 1
;.org	ICP1addr	; Timer/Counter Capture Event
;.org OC1Aaddr	; Timer/Counter1 Compare Match A
;.org OC1Baddr	; Timer/Counter1 Compare Match B
;.org OVF1addr	; Timer/Counter1 Overflow
;.org OVF0addr	; Timer/Counter0 Overflow
;.org SPIaddr	; Serial Transfer Complete
;.org URXCaddr	; UART, Rx Complete
;.org UDREaddr	; UART Data Register Empty
;.org UTXCaddr	; UART, Tx Complete
;.org ACIaddr	; Analog Comparator

; registers used throughout the program
.def TEMP=r16
.def TEMP2=r17

; these variable configure the cursor, set to 1 to turn on, 0 to turn off
.equ CURSORON=0x01
.equ BLINKINGON=0x01

; this is where we start at power on or after a reset
Reset:
	; always set up the stack first
	ldi TEMP,low(RAMEND)
	out SPL,TEMP
	ldi TEMP,high(RAMEND)
	out SPH,TEMP

	; configure the appropriate port as output to drive the LCD data pins and set all pins to low
	ser TEMP
	out DDATAPORT,TEMP
	clr TEMP
	out DATAPORT,TEMP

	; configure the appropriate port as input to read the switches, except pins 4 and 5
	ldi TEMP,(1<<RSPIN)+(1<<EPIN)
	out DCTRLPORT,TEMP
	clr TEMP
	out CTRLPORT,TEMP

	; configure PORTC as output to drive the LEDs
	ser TEMP
	out DDRC,TEMP
	out PORTC,TEMP

	; configure sleep mode
	ldi TEMP,$20
	out MCUCR,TEMP
	
	; enable external interrupts INT0 and INT1
	ldi TEMP,(1<<INT0)+(1<<INT1)
	out GIMSK,TEMP
	in TEMP,MCUCR
	; interrupt INT0 and INT1 on falling edge only
	ori TEMP,(0<<ISC00)+(1<<ISC01)+(0<<ISC10)+(1<<ISC11)
	out MCUCR,TEMP

	; initialized the lcd
	rcall lcd_initialize

	; set up the main features of the display the way we want
	; display on, cursor on, blinking of cursor position character
	ldi TEMP,(1<<DB3)+(1<<DB2)+(CURSORON<<DB1)+(BLINKINGON<<DB0)
	out DATAPORT,TEMP
	cbi CTRLPORT,RSPIN
	rcall pulse_e

	rcall hello_world
		
	sei				; global interrupt enable

; the main loop of the program - the uC just sleeps most of the time until
; it receives an interrupt
loop:

	sleep
	rjmp loop

; this routine follows the Initialization by Instruction outlined in Figure 23 of HD44780 datasheet
; Initialization by instruction should be used when the hw reset condition are not met, but it's
; probably not a bad precaution to use it all the time.
lcd_initialize:
	; initialize the LCD
	rcall delay_10ms

	; function set (8-bit mode)
	ldi TEMP,(1<<DB5)+(1<<DB4)
	out DATAPORT,TEMP
	cbi CTRLPORT,RSPIN
	rcall pulse_e

	; wait
	rcall delay_10ms

	; function set (8-bit mode)
	ldi TEMP,(1<<DB5)+(1<<DB4)
	out DATAPORT,TEMP
	cbi CTRLPORT,RSPIN
	rcall pulse_e

	; wait
	rcall delay_128us

	; function set (8-bit mode)
	ldi TEMP,(1<<DB5)+(1<<DB4)
	out DATAPORT,TEMP
	cbi CTRLPORT,RSPIN
	rcall pulse_e

	; wait
	rcall delay_128us

	; function set (8-bit mode, 2 lines)
	ldi TEMP,(1<<DB5)+(1<<DB4)+(1<<DB3)
	out DATAPORT,TEMP
	cbi CTRLPORT,RSPIN
	rcall pulse_e

	; wait
	rcall delay_128us

	; turn off display
	ldi TEMP,(1<<DB3)
	out DATAPORT,TEMP
	cbi CTRLPORT,RSPIN
	rcall pulse_e

	; wait
	rcall delay_128us

	; clear the display
	ldi TEMP,(1<<DB0)
	out DATAPORT,TEMP
	cbi CTRLPORT,RSPIN
	rcall pulse_e

	; wait
	rcall delay_128us

	; entry mode set
	ldi TEMP,(1<<DB2)+(1<<DB1)
	out DATAPORT,TEMP
	cbi CTRLPORT,RSPIN
	rcall pulse_e

	; wait
	rcall delay_128us

	ret

; this routine prints 'HELLO WORLD' on the display
hello_world:
	; display a character
	ldi TEMP,H
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	ldi TEMP,E
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	ldi TEMP,L
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	ldi TEMP,L
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	ldi TEMP,O
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	ldi TEMP,space
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	ldi TEMP,W
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	ldi TEMP,O
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	ldi TEMP,R
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	ldi TEMP,L
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	ldi TEMP,D
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e

	; "new line"
	ldi TEMP,newline
	out DATAPORT,TEMP
	cbi CTRLPORT,RSPIN
	rcall pulse_e


; this routine pulses the uC pin going to the E pin of the LCD
; 256@8MHz, 512us@4MHz, ~1ms@2MHz, ~2ms@1MHz
pulse_e:
	sbi CTRLPORT,EPIN
	clr TEMP
pulse_delay1:
	inc TEMP
	cpi TEMP,0xFF
	brne pulse_delay1
	cbi CTRLPORT,EPIN
pulse_delay2:
	inc TEMP
	cpi TEMP,0xFF
	brne pulse_delay2
	ret

; 128us@8MHz, 256us@4MHz, 512us@2MHz, ~1ms@1MHz
delay_128us:
	clr TEMP
delay_128us_1:
	inc TEMP
	cpi TEMP,0xFF
	brne delay_128us_1
	ret

; 0.5ms@8MHz, 1ms@4MHz, 2ms@2MHz, 4ms@1MHz
delay_500us:
	push TEMP
	push TEMP2
	clr TEMP
delay_500us_1:
	inc TEMP
	clr TEMP2
delay_500us_2:
	inc TEMP2
	cpi TEMP2,0xFF
	brne delay_500us_2
	cpi TEMP,0x04
	brne delay_500us_1
	pop TEMP2
	pop TEMP
	ret

; 10ms@8MHz, 20ms@4MHz, 40ms@2MHz, 80ms@1MHz
delay_10ms:
	push TEMP
	push TEMP2
	clr TEMP
delay_10ms_1:
	inc TEMP
	clr TEMP2
delay_10ms_2:
	inc TEMP2
	cpi TEMP2,0xFF
	brne delay_10ms_2
	cpi TEMP,0x4E
	brne delay_10ms_1
	pop TEMP2
	pop TEMP
	ret

; external interrupt request 0 handling routine
IntV0:
	; print 'A' to the screen
	ldi TEMP,A
	out DATAPORT,TEMP
	sbi CTRLPORT,RSPIN
	rcall pulse_e
	reti

; external interrupt request 1 handling routine
IntV1:
	; here we simply clear the display
	ldi TEMP,(1<<DB0)
	out DATAPORT,TEMP
	cbi CTRLPORT,RSPIN
	rcall pulse_e
	reti
