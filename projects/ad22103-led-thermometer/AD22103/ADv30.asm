; ADv30.asm     Thermometer using Analog Devices AD22103 and two 7-segment LED displays
; CPU -- Atmel AVR Tiny26   [1K words Flash, 128 bytes SRAM, 128 EEPROM, no UART; 10-bit ADC] 
;
; Alan Probandt   Portland Oregon USA   alan_probandt@yahoo.com    June 2, 2005
;
;
; Temperture display on LED 7-segment pair is displayed for 1 second and then turned off
; for three seconds.  During the ON period, the timer0 overflows 90 times a second and 
; multiplexes the lighting of each LED 7-seg digit.  Timer1 is set for one-second timeouts.
; In between each one-second timeout, the Tiny26 main program sleeps in idle mode 
; to reduce power consumption.
;
;  ver 3.0    @50mA with LEDs on,  @10mA with LEDs off
;
; 	    Atmel AVR Tiny26
;    	pb0	1 | 20 pa0 LED digit select
;    	pb1	2 | 19 pa1 LED seg A-360ohms --LED-- +5
;    	pb2	3 | 18 pa2 LED seg B-360ohms --LED-- +5 
;    	pb3	4 | 17 pa3 LED seg C-360ohms --LED-- +5 
;  +5V	VCC	5 | 16 GND
;  	GND	6 | 15 AVCC +5V
; AD22103 input 7 | 14 pa4 LED seg D-360ohms --LED-- +5 
; 	pd5	8 | 13 pa5 LED seg E-360ohms --LED-- +5 
;     	pb6	9 | 12 pa6 LED seg F-360ohms --LED-- +5 
;   reset\	10| 11 pa7 LED seg G-360ohms --LED-- +5  
;
; AVR instr that use upper registers only: ldi, ser, cbr, sbr, ori, andi, subi, sbci 

.nolist
.include "Tn26def.inc"
.list

.equ	T26Clock	= 1000000
; 16-bit Timer1 values for 1 second
.equ	T26TimerPeriod	= T26Clock/16384
.equ	OneSecT26	= ~T26TimerPeriod

;     LEDMuxReloadValue should IRQ at no less than 50 times a second
.equ	LEDMuxReloadValue = ~10  ; interrupt 95 times a second to multiplex LEDs


; common
.equ	LEDport		= porta
.equ	LEDddr		= ddra
.equ	LEDselect	= 0
.equ	NumberOfSecondsDisplayIsOff  = 4
.equ	NumberOfSecondsDisplayIsOn = 1

; register definitions
.def	SevenSegValue	= r1
.def	LowDelay	= r2
.def	LongDelay	= r3
.def	high_digit	= r4
.def	low_digit	= r5
; high registers
.def	Temp		= r16
.def	irqTemp		= r17
.def	degree		= r18
.def	ToggleFlags	= r19
.equ	LEDs_Lighted	= 7	; set=LED display is being multiplexed on,clear= no LED display
.equ	UpdateDisplay	= 6

;***** Binary2BCD Subroutine Register Variables

.def	fbin	=r20		;8-bit binary value
.def	tBCDL	=r20		;BCD result MSD
.def	tBCDH	=r21		;BCD result LSD



.cseg  ; load the following code into AVR Flash Program memory

.org 0x0  ; interrupt vector address jump table for ATTiny26
	rjmp	reset
.org	INT0addr	;=0x001
   	reti	;rjmp	EXT_INT0
.org	IOPINSaddr	;=0x002
	reti	;rjmp	PIN_CHANGE
.org	OC1Aaddr	;=0x003
	reti	;rjmp	TIM1_CMP1A
.org	OC1Baddr	;=0x004
	reti	;rjmp	TIM1_CMP1B
.org	OVF1addr	;=0x005
	rjmp	TIM1_OVF
.org 	OVF0addr	;=0x006
	rjmp	TIM0_OVF
.org	USI_STARTaddr	;=0x007
	reti	;rjmp	USI_STRT
.org	USI_OVFaddr	;=0x008
	reti	;rjmp	USI_OVF
.org	ERDYaddr	;=0x009
	reti	;rjmp	EE_RDY
.org	ACIaddr  	;=0x00a
	reti	;rjmp	ANA_COMP
.org	ADCCaddr   	;=0x00b
	rjmp	ADC_Complete


reset:
; initialize stack
	 ldi	Temp,RAMEND ; setup stack
	 out	SP,Temp	;init Stack Pointer - use SPL for AVR2313

; initialize I/O ports
	ser	temp
	out	LEDDDR, temp  ; LED port all outputs
	sbi	ddrb, 5 ; broken pin 13 replacement pin8 [pb5] is output

; initialize Timers
	ldi	Temp,  (1 << TOIE0)  | (1 << TOIE1) ; enable timers 1 and 0 overflow interrupts
	out	TIMSK, Temp	; enable timer interrupts
	out	TIFR, Temp	; clear previously-set timer interrupt flags

; Timer1 is one second period
; Timer1 for Tiny26   8-bit with large preset
	ldi	Temp, OneSecT26 ; load timer1 count register for 1 second
	out	TCNT1, Temp	; start LED segment display
	ldi	Temp, ( (1<<CS13) | (1<<CS12) | (1<<CS11) | (1<<CS10) ); start timer 1 at preset sysclk/1638
	out	TCCR1B, Temp


; Timer0 is LED display segment multiplex @ 90 times per Sec
	ldi	Temp, LEDMuxReloadValue	; load timer0 count register for 1 millisecond
	out	TCNT0, Temp	; start LED segment display
	ldi	Temp, (  (1<<CS02) | (0<<CS01) | (1<<CS00) ); start timer 0 at preset sysclk/1024 -97 interrupts/second
	out	TCCR0, Temp

; initialize general interrupts
	sei

; initialize application
; When powered on, display temperture for one second, then off for 3 or 4 seconds
;  add one to the flags because the interrupt routine decrements before checking
	ldi	ToggleFlags,(1<<UpdateDisplay) | (1<<LEDs_Lighted) + 1
	;  LEDs_Lighted set=light the LED display, clear=don't light LEDs


;********************************************************************
;  Main program code for thermometer display
;********************************************************************
;
main:
	ldi	temp, (1<<SE) | (0<<SM1) | (0<<SM0)  ; enable idle mode for Tiny26
	out	MCUCR, temp
	sleep 	; either timer interrupt will awaken MPU

	sbrs	ToggleFlags, UpdateDisplay
	rjmp	main ; still in DisplayOff mode

;  return here from timer interrupts that are entered upon awakening.
; 	LEDs_Lighted=set >> turn on LED display and start multiplexing with timer 0
;	LEDs_Lighted=clear >> turn off LED display
	cbr	ToggleFlags, (1 << UpdateDisplay)
	sbrc	ToggleFlags, LEDs_Lighted
	rjmp	Main_HaltLEDdisplay

;  LEDs_Lighted flag is off -- check if LEDdisplay count is zero
	rjmp	Main_RestartLEDdisplay

;  still in LEDoff mode - LEDdisplay count was decremented but value was not zero
;  so go back to sleep
	rjmp	main

Main_RestartLEDdisplay:
	rcall	GetDegreeValue  ; update the current temperture reading
	ser	irqTemp	; enable LED display by permitting LEDport to pull segments to ground
	out	LEDDDR, irqTemp
	sbi	ddrb,5
	ldi	irqTemp, LEDMuxReloadValue	; load timer0 count register for 1 millisecond
	out	TCNT0, irqTemp	; start LED segment display
	ldi	irqTemp, (  (1<<CS02) | (0<<CS01) | (1<<CS00) ); start timer 0 at preset sysclk/1024 -97 interrupts/second
	out	TCCR0, irqTemp

	rjmp	Main	; go back to sleep until next Timer0 Overflow for LED display mux

Main_HaltLEDdisplay:
	clr	irqTemp; stop timer 0
	out	TCCR0, irqTemp
	ser	irqTemp ; all LED segments off --- input port pullup resistors on
	out	LEDport, irqTemp
	clr	irqTemp ; LED port pins are all inputs- HiZ
	out	LEDDDR, irqTemp
	cbi	ddrb,5; broken pin replacement
	sbi	portb,5; turn on pull-up resistor in order to turn off segment lighting

	rjmp	main


; subroutine to get ADC value of temperture and format to two digit Fahrenheit degree value
GetDegreeValue:
	rcall	GetADC ; setup ADC, start conversion, sleep until finished, do ADCIRQ, return

; for Tiny26 with left alignment - bits 9-2 in highADC
	lsl	low_digit; move ADC result bit 1 into carry
	rol	high_digit ; throw away bit 9 of ADC result, shift ADC result bit 1 into LSB of high_digit


GDV1:	ldi	ZH, high(FahrTable*2)
	ldi	ZL, low(FahrTable*2)
	add	ZL, high_digit ; for Tiny26

	brcc	GDV10
	inc	ZH
GDV10:	lpm
	mov	fbin, r0
	rcall	bin2bcd8

	mov	SevenSegValue, fbin
	ret

GetADC:   ;  routine is for Tiny26
	ldi	Temp, (0<<REFS1) | (0<<REFS0) | (1<<ADLAR) |(0<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0)
; ADC reference --  00=AVCC
; ADLAR	= ADC left adjust result for ADC bits 8 & 9  1=left align>highbyte=bits 9-2
; MUX = ADC channel 7, pin 7
	out	ADMUX, Temp

	ldi	Temp, (1<<ADEN) | (1<<ADIE) | (1<<ADIF) | (0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)
;  set up noise canceling with ADCenable=on, ADCinterrupt=on, clear Int flag
;  set ADC clock preset to 8 -- 125KHz from 1MHz System Clock
	out	ADCSR, Temp

	ldi	Temp, (0<<SM1) | (1<<SM0)  | ( 1<<SE)
;  set up sleep mode for best ADC resolution
	out	MCUCR, Temp
; entering sleep will start ADC conversion, completion will wake the MCU to ADC interrupt
	sleep
	ret

;*****************************
; **** ADC interrupt routine
; ****************************
ADC_Complete:
	in	low_digit, ADCL ; always read low digit first
	in	high_digit,ADCH
	reti


;********************************************************************************
; TIM1_OVF Timer 1 Overflow interrupt routine - overflows 1 time per second
;  Activates CPU from sleep idle mode, then:
;	decrement second count (bits 3-0 of ToggleFlags)
;	if not zero, then exit interrupt routine
;	if zero, then reset second count according to LEDs_Lighted flag
;
;	tests LEDs_Lighted flag,
;	 flag clear > tests and adjusts LEDdisplay count
;	 flag set >  clears flag and returns to main
;  This routine should only set or clear the flag for lighting LEDs
;	registers used: 'ToggleFlags'
;********************************************************************************
TIM1_OVF:
	push	irqTemp
; Timer1 reload for Tiny26
	ldi	irqTemp, OneSecT26 ; load timer1 count register for 1 second
	out	TCNT1, irqTemp	; start LED segment display
	mov	irqtemp, ToggleFlags ; decrement current second count
	andi	irqtemp, 0x0f
	dec	irqtemp
	breq	T1Ovr_tglUD  ; if decremented count is zero, set UpdateDisplay flag
	andi	ToggleFlags, 0xf0
	or	ToggleFlags, irqTemp
	rjmp	T1Ovr_Exit

T1Ovr_tglUD:
	sbr	ToggleFlags, (1 << UpdateDisplay)  ; tell main program to turn LEDs off/on

; toggle LEDs_Lighted flag
	ldi	irqTemp, (1<< LEDs_Lighted)
	eor	ToggleFlags, irqTemp

T1Ovr_restoreSecondCount:
	ldi	irqtemp,  NumberOfSecondsDisplayIsOn
	sbrc	ToggleFlags, LEDs_Lighted
	ldi	irqtemp, NumberOfSecondsDisplayIsOff
	andi	ToggleFlags, 0xf0
	or	ToggleFlags, irqTemp
T1Ovr_Exit:
	pop	irqTemp
	reti


;********************************************************************************
; TIM0_OVF Timer 0 Overflow interrupt routine - overflows 97 times per second
;	LED 7-segment multiplex dual display of hex byte using Timer0 interrupt
;	Hex bite being displayed from register SevenSegValue
;	uses T-flag (as a static variable, so SREG is not stacked)
;	registers used: Zh, Zl,'irqtemp', 'SevenSegValue'
;********************************************************************************
TIM0_OVF: ; same routine used for both 4433-ICE and Tiny26
	push	ZH
	push	ZL
	push	irqTemp
	ldi	irqTemp, LEDMuxReloadValue
	out	TCNT0, irqTemp	; restore the timer count to 0.5 millisecond

	brts	RightLEDdigit
	set	; ; next Timer1 overflow displays right LED digit (LSB)
	ldi	ZH, high(SevenSegMAN6710*2)  ; load 16-bit Z register with the byte address of the LED pattern table
	ldi	ZL, low(SevenSegMAN6710*2)
	mov	irqTemp, SevenSegValue	  ;SevenSegValue is the hex digit pair to be displayed
	andi	irqTemp,0x0f	; mask out the most sig digit
	add	ZL, irqTemp	; add digit value offset to Z low
	brcc	LED_a		; add one to Z high if result crosses a 256 byte boundary
	inc	ZH
LED_a:	lpm			; move LED 7-seg digit pattern for the digit to R0 from program memory table
	mov	irqTemp,r0   	; OUT instruction uses high register bank only
	out	LEDPort,irqTemp	; output 7-seg pattern to port A
	cbi	LEDPort,LEDselect ; Port A bit 0 selects which digit gets lighted; 0=least sig;  bits 1-7 are seg_a to seg_g
	rjmp	LEDexit

RightLEDdigit:
	clt	; next Timer0 overflow displays left LED digit (MSB)
	ldi	ZH, high(SevenSegMAN6710*2)   ; reload 16-bit Z register with the byte address of the LED pattern table
	ldi	ZL, low(SevenSegMAN6710*2)
	mov	irqTemp, SevenSegValue	;SevenSegValue is the hex digit pair to be displayed
	andi	irqTemp,0xf0	; mask out the least sig digit
	swap	irqTemp     	; move most sig digit to bits 0-3
	add	ZL, irqTemp	; add digit value offset to Z low
	brcc	LED_b		; add one to Z high if result crosses a 256 byte boundary
	inc	ZH
LED_b:	lpm			; move LED 7-seg digit pattern for the digit to R0 from program memory table
	mov	irqTemp,r0
	out	LEDPort, irqTemp	; OUT instruction uses high register bank only

	sbi	LEDPort, LEDselect ; Port A bit 0 set means that the right (MSB) LED digit gets lighted
LEDexit:
	pop	irqtemp
	pop	ZL
	pop	ZH
	reti


;***************************************************************************
;*
;* "bin2BCD8" - 8-bit Binary to BCD conversion
;*
;* This subroutine converts an 8-bit number (fbin) to a 2-digit
;* BCD number (tBCDH:tBCDL), or packed BCD in fbin
;*
;* Number of words	:6 + return
;* Number of cycles	:5/50 (Min/Max) + return
;* Low registers used	:None
;* High registers used  :2 (fbin/tBCDL,tBCDH)
;*
;* Included in the code are lines to add/replace for packed BCD output.
;*
;***************************************************************************


;***** Code from Atmel Application Note   avr204 BCD Arithmetics.asm

bin2bcd8:
	clr	tBCDH		;clear result MSD
bBCD8_1:subi	fbin,10		;input = input - 10
	brcs	bBCD8_2		;abort if carry set
;	inc	tBCDH		;inc MSD
;---------------------------------------------------------------------------
;				;Replace the above line with this one
;				;for packed BCD output
	subi	tBCDH,-0x10 	;tBCDH = tBCDH + 10
;---------------------------------------------------------------------------
	rjmp	bBCD8_1		;loop again
bBCD8_2:subi	fbin,-10	;compensate extra subtraction
;---------------------------------------------------------------------------
;				;Add this line for packed BCD output
	add	fbin,tBCDH
;---------------------------------------------------------------------------
	ret



SevenSegMAN6710: ;  - bit 7 select - low lights LSD - High lights MSD  -- logic low to light segment
;	PortB 0 - 6 pins have 360 ohm resistors; #1 & #2 letter segments tied together
;   PortB 7  to PNP and NPN on common anodes (1K ohm base resistor on NPN & PNP)
.db 0x01,0x4f,0x12,0x06,0x4c,0x24,0x20,0x0f,0x00,0x0c,0x08,0x60,0x31,0x42,0x30,0x38
;       abcdefg   	  0=on 1=off  MAN6710 dual 7-seg common anode
; 0  0b00000001	  ______________________________                     IEE 1737 R
; 1  0b01001111	  | 18 17 16 15 14 13 12 11 10  \                   ___________
; 2  0b00010010	  |       __a_      __a_        |              a  1 |   __a_   |14  ca
; 3  0b00000110	  |  #1  |    |    |    | #2    |              f  2 |  |    |  |13  b
; 4  0b01001100	  |     f|__g_|b  f|__g_|b      |             ca  3 | f|__g_|b |-
; 5  0b00100100	  |      |    |    |    |       |                 - |  |    |  |11  g
; 6  0b00100000	  |     e|__d_|c  e|__d_|c      |                 - | e|__d_|c |10  c
; 7  0b00001111	  | ____________________________|            dp L 6 |          | 9  dpt R
; 8  0b00000000	   \__1  2  3  4  5  6  7  8  9 \               e 7 |__________| 8  d
; 9  0b00011000	    pinout:
; A  0b00001000	      1 #1e    7 #2g   13 #2 com Anode
; b  0b01100000	      2 #1d    8 #2c   14 #1 com Anode
; c  0b00110001	      3 #1c    9 #2Rdp 15 #1b
; d  0b01000010	      4 #1Rdp 10 #2b   16 #1a
; E  0b00110000	      5 #2e   11 #2a   17 #1g
; F  0b00001110	      6 #2d   12 #2f   18 #1f
;*************************************************************************************************


;***********************************************************************************************
;  Temperature table ; Value is the LED display of F° degrees for each shifted ADC result value
;***********************************************************************************************
FahrTable:
.db 16,17,17,18,19,19,20,20     ;  00-07 ADC shifted result
.db 21,21,22,23,23,24,24,25     ;  08-0F
.db 26,26,27,27,28,29,29,30     ;  10-17
.db 30,31,31,32,33,33,34,34     ;  18-1F
.db 35,36,36,37,37,38,39,39     ;  20-27
.db 40,40,41,41,42,43,43,44     ;  28-2F
.db 44,45,46,46,47,47,48,49     ;  30-37
.db 49,50,50,51,51,52,53,53     ;  38-3F
.db 54,54,55,56,56,57,57,58     ;  40-47
.db 59,59,60,60,61,61,62,63     ;  48-4F
.db 63,64,64,65,66,66,67,67     ;  50-57
.db 68,69,69,70,70,71,71,72     ;  58-5F
.db 73,73,74,74,75,76,76,77     ;  60-67
.db 77,78,79,79,80,80,81,81     ;  68-6F
.db 82,83,83,84,84,85,86,86     ;  70-77
.db 87,87,88,89,89,90,90,91     ;  78-7F
.db 92,92,93,93,94,94,95,96     ;  80-87
.db 96,97,97,98,99,99, 0, 0     ;  88-8F
.db  1, 2, 2, 3, 3, 4, 4, 5     ;  90-97
.db  6, 6, 7, 7, 8, 9, 9,10     ;  98-9F
.db 10,11,12,12,13,13,14,14     ;  A0-A7
.db 15,16,16,17,17,18,19,19     ;  A8-Af

; ********************** ASCII strings
ID:  ;     "****  ADv30.asm  Jun 2, 2005  Alan Probandt "
.db        "****  DA3v.0sa mJ nu2  ,0250  lAnaP orabdn t"

                                                                                                                                                                                               