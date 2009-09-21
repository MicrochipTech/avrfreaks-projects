; Power Monitor
; Shows power usage in kilowatts (kW) (switch open), or
; Kilowat hours (kWh) used since reset (switch closed)

; The phototransistor, Q4, detects infrared pulses put out by electonic power meters.
; The LED in the power meter emits a 25 msec pulse each time 1 watt-hour of electric energy is used.

; This power monitor measures the time, T, between pulses to compute the power usage, P, in kW.
; P (kW) =  3600 (sec/hr) X .001 kW/watt / [ T (sec / watt-hr)]
;        =  3.6 / T

; Counter 0 is set for a clock frequency of 15,625 / sec.
; A pulse from the electric meter generates an interrupt that gets the count from Counter 0 and then resets it.
; The time (seconds) between pulses, T, = Counter 0 count / 15625.

; When the switch is set in the kWh position, This Power Monitor shows the accumulated kWh usage since it was plugged in

; When programming the ATtiny461: 
;   1) The CKSEL3..1 Fuses should be set to 110 to set External crystal oscillator 3.0 - 8.0 MHz
;   2) The CKSEL0 fuse should be set to 1 and the SUT1..0 Fuses should be set to 11 for Crystal Oscillator, slowly rising power 
;   3) The CKDIV8 Fuse should be left programmed to divide the 8 MHz crystal oscillator frequency by 8
;        The clock frequency must be exactly 1 Mhz or the power calculation will be incorrect.
;        The internal 8 MHz oscillator could be used instead of the crystal, but it could cause several % error.
;   4) The RSTDISBL Fuse must be programmed for the kWh function to work
;         If RSTDISBL is not programmed, only the kW function will work and the other switch position will reset the ATtiny
;         CAUTION: programming the RSTDISBL fuse makes the ATtiny permanently inaccessible to ISP programmers like the AVRISP.
;           An ISP programmer can program the fuse RSTDISBL but cannot unprogram it. A high voltage programmer is required to unprogram the fuse.

; The code uses only 640 bytes of flash memory, so the ATtiny261 should work equally well.

; Pinout ATtiny261/461/861

;Pin Meter    Dir   Act  Pgm    IC                                  Pin Meter    Dir   Act  IC
; 1 Digit 3 | out | lo | MOSI | PB0 (MOSI/DI/SDA/OC1A/PCINT8)        20 Seg a  | out | lo | PA0 (ADC0/DI/SDA/PCINT0)
; 2 Digit 2 | out | lo | MISO | PB1 (MISO/DO/OC1A/PCINT9)            19 Seg f  | out | lo | PA1 (ADC1/DO/PCINT1)           
; 3 Digit 1 | out | lo | SCK  | PB2 (SCK/USCK/SCL/OC1B/PCINT10)      18 Seg b  | out | lo | PA2 (ADC2/INT1/USCK/SCL/PCINT2)
; 4 Digit 0 | out | lo |      | PB3 (OC1B/PCINT11)                   17 Seg g  | out | lo | PA3 (AREF/PCINT3)              
; 5  VCC    |     |    | VCC  | VCC                                  16 GND    |     |    | AGND                           
; 6  GND    |     |    | GND  | GND                                  15 VCC    |     |    | AVCC                           
; 7  XTAL   | in  |    |      | PB4 (ADC7/OC1D/CLKI/XTAL1/PCINT12)   14 Seg e  | out | lo | PA4 (ADC3/ICP0/PCINT4)         
; 8  XTAL   | in  |    |      | PB5 (ADC8/OC1D/CLKO/XTAL2/PCINT13)   13 Seg c  | out | lo | PA5 (ADC4/AIN2/PCINT5)         
; 9  LED    | out | lo |      | PB6 (ADC9/INT0/T0/PCINT14)           12 Seg d  | out | lo | PA6 (ADC5/AIN0/PCINT6)         
; 10 Switch | in  |    | RSET | PB7 (ADC10/RESET/PCINT15)            11 Meter  | in  | ^  | PA7 (ADC6/AIN1/PCINT7)

; NOTE: kWh can be reset by interrupting power to processor

; Pins used for programming with AVRISP:
;VTG     5  VCC
;GND     6  GND
;MOSI    1  (MOSI/DI/SDA/OC1A/PCINT8) PB0 (Input: microcontroller to AVRISP; use series resistor to load)
;MISO    2  (MISO/DO/OC1A/PCINT9) PB1  (Output: AVRISP to microcontroller; use series resistor to load)
;SCK     3  (SCK/USCK/SCL/OC1B/PCINT10) PB2  (Output: AVRISP to microcontroller; use series resistor to load)
;RESET  10 (ADC10/RESET/PCINT15) PB7  (Output: AVRISP to microcontroller; max load: 4.7K pullup)

.include "tn461def.inc"
.DEVICE ATtiny461; Use the ATtiny461

; 11 bytes in SRAM to define which segments are active for digits 0-9 and blank 
.equ SegmentArray = 0x0060 ; SRAM begins at 0x0060

.def mod0   = R0 ; registers for division & binary to BCD conversion
.def mod1   = R1
.def mod2   = R2
.def mod3   = R3
.def var10  = R4 ; dividend
.def var11  = R5
.def var12  = R6
.def var13  = R7
.def var20  = R8  ; divisor
.def var21  = R9
.def var22  = R10
.def var23  = R11
.def watthourx  = R12 ; 1 watt-hour accumulator 0-10 (not displayed)
.def watthour0 = R13 ; 10 watt-hour accumulator - low byte 
.def watthour1 = R14 ; 10 watt-hour accumulator - hi byte

.def AccumByte2  = R15 ; bytes 3 & 4 time accumulators; bytes 1 & 2 are the 16 bit counter
.def AccumByte3  = R16

.def x0     = R17 ; general use register
.def x1     = R18 ; general use register

.def div0 = R19 ; saved divisor; updated by IntMeterPulse
.def div1 = R20
.def div2 = R21
.def div3 = R22

.def InterruptFlag = R23 ; set = 1 by IntMeterPulse routine to show that data may have changed
.def xd = R24 ; digit counter for Display routine

.CSEG

; Interrupt vectors
.org 0x0000 
rjmp START
.org 0x0001 
reti
.org 0x0002 
reti
.org 0x0003 
reti
.org 0x0004 
reti
.org 0x0005 
reti
.org 0x0006 
rjmp IntTimerOverflow ; when Timer 0 overflows
.org 0x0007 
reti
.org 0x0008 
reti
.org 0x0009 
reti
.org 0x000A 
rjmp IntMeterPulse ; when a pulse from the electric meter is detected by analog comparator
.org 0x000B 
reti
.org 0x000C 
reti
.org 0x000D 
reti
.org 0x000E 
reti
.org 0x000F 
reti
.org 0x0010 
reti
.org 0x0011 
reti
.org 0x0012 
reti

;############################################################################################

IntTimerOverflow: ; routine to increment bytes 2 and 3 (if required) of time accumulator
  ; This routine runs every 65536 (counts) / 15625 (counts/sec) = 4.194304 sec 

  push x0 ; save temp register to stack
  push x1
  in x0,SREG ; read status register
  push x0 ; and put on stack

  ldi x0, 1
  ldi x1, 0 
  add AccumByte2, x0 ; 16 bit timer (bytes 0 & 1) of time accumulator overflowed; increment byte 2
  adc AccumByte3, x1 ; add carry bit, if any, to byte 3
  ; at 15,625 counts/sec, the 4 byte accumulator will not overflow until
  ; time between meter pulses > 76 hours !
  
  ; If time accumulated is > last used value, estimate a lower power based on the time since last meter pulse
  

  cp AccumByte3, div3 ; Compare Byte 3
  brlo NoEstimate 
  cp div3, AccumByte3  
  brlo Estimate
   
  cp AccumByte2, div2 ; They were equal; Compare Byte 2
  brlo NoEstimate
  cp div2, AccumByte2   
  brlo Estimate

  in x0,TCNT0L ;                                                      
  in x1,TCNT0H ;                                                      

  cp x1, div1 ; They were equal; Compare Byte 1
  brlo NoEstimate
  cp div1, x1  
  brlo Estimate

  cp x0, div0 ; They were equal; Compare Byte 0
  brlo NoEstimate
  cp div0, x0  
  brlo Estimate
  
  Estimate:
  ; Read 16 bit counter TCNT0 and bytes 3 & 4 into the divisor, 
  mov div0, x0                                                      
  mov div1, x1                                                      
  mov div2, AccumByte2 ;                                                
  mov div3, AccumByte3 ;                                                

  NoEstimate:


  ldi InterruptFlag, 1 ; Set = 1 to show that data may have changed

  pop x0 ; get previous status register from stack
  out SREG,x0 ; restore status register
  pop x1 ; get previous content of x1 from the stack
  pop x0 ; get previous content of x0 from the stack
reti

;############################################################################################

IntMeterPulse: ; a meter pulse has occurred indicating a watt hour has been used
  ;                                                                   
  ; 4 clock cycles to get to here after interrupt is triggered          

  push x0 ; save temp register to stack                                 
  push x1 ; save temp register to stack                                 
  in x0,SREG ; read status register                                     
  push x0 ; and put on stack                                            

  ; There is an intermittent false trigger of this interrupt routine
  ; at a count of 58 to 60 ???? (3712 to 3840 clock cycles) after a real interrupt
  ; Can't figure out why!!!
  ; The two lines below avoid the problem by bypassing this routine until the flashing light times out (33 msec)
  sbis PORTB, 6  ; test if light is still on 
  rjmp EndIntMeterPulse ; Don't accept another interrupt until light times out
  ;                                                                  CYCLES Cumulative
  ; Read 16 bit counter TCNT0 and bytes 3 & 4 into the divisor, 
  in div0,TCNT0L ;                                                      1        1
  in div1,TCNT0H ;                                                      1        2
  mov div2, AccumByte2 ;                                                1        3
  mov div3, AccumByte3 ;                                                1        4

  ; Preload TCNT0 to  8 - 1 = 0x0007 so next read will show actual time elapsed between interrupts
  ldi x1,0x00 ;                                                         1        5
  ldi x0,0x07 ;                                                         1        6
  out TCNT0H,x1 ;                                                       1        7
  out TCNT0L,x0 ;                                                       1        8

  ; set bytes 2 & 3 to zero
  ldi x0,0
  mov AccumByte2, x0 ; clear accumulator registors
  mov AccumByte3, x0

  cbi PORTB,6 ; set bit LO to turn LED on & trigger output to computer
  ; only leading edge has precise timing

  ; increment watt-hour accumulator (BCD)
  inc watthourx ; .001 kWh (1 watt-hour) accumulator 0-10 (not displayed)
  ldi x1, 10
  cp watthourx, x1
  brlo WatthourDone
  ldi x0, 0
  mov watthourx, x0 ; clear watthourx
  
  ldi x0, 0
  ldi x1,1 
  add watthour0, x1  ; increment watthour1:watthour0
  adc watthour1, x0
   
  ldi x1, 0x27
  cp watthour1, x1  ; check for overflow of 99.99 kWh (9999 = 0x270F)
  brlo WatthourDone
  ldi x1, 0x0F
  cp watthour0, x1 ; check for overflow of 99.99 kWh (9999 = 0x270F)
  brlo WatthourDone
  mov  watthourx, x0 ; hit a count of 9999, reset to zero
  mov  watthour0, x0
  mov  watthour1, x0
  WatthourDone:

  ldi InterruptFlag, 1 ; Set = 1 to show that data may have changed

  EndIntMeterPulse:

  pop x0 ; get previous status register from stack
  out SREG,x0 ; restore status register
  pop x1 ; get previous content of x1 from the stack
  pop x0 ; get previous content of x0 from the stack

reti

;############################################################################################

START:

; WDIF WDIE WDP3 WDCE WDE WDP2 WDP1 WDP0
ldi x0, 0b00001011 ; Enable watchdog timer; automatic reset if wdr is not called every 125 msec.
out WDTCR, x0 ; to WDTCR

; Set stack pointer to end of SRAM for subroutine call
LDI x0, HIGH(RAMEND) ; Upper byte
OUT SPH,x0 ; to stack pointer
LDI x0, LOW(RAMEND) ; Lower byte
OUT SPL,x0 ; to stack pointer

; Seven segment display programming:
; initialize SRAM to define which segments are active for digits 0-9, 10 (blank), 11 (dash)
; Set bit 7 = 0; Bits 6:0 are segments d c e g b f a
; 0 = segment on; 1 = segment off
; Segment arrangement:
;     a
;   f   b
;     g
;   e   c
;     d
;
LDI ZH, 0
LDI ZL, SegmentArray
;          dcegbfa
ldi x0, 0b00001000 ; 0
ST Z+, x0 
;          dcegbfa
ldi x0, 0b01011011 ; 1
ST Z+, x0 
;          dcegbfa
ldi x0, 0b00100010 ; 2
ST Z+, x0 
;          dcegbfa
ldi x0, 0b00010010 ; 3
ST Z+, x0 
;          dcegbfa
ldi x0, 0b01010001 ; 4
ST Z+, x0 
;          dcegbfa
ldi x0, 0b00010100 ; 5
ST Z+, x0 
;          dcegbfa
ldi x0, 0b00000100 ; 6
ST Z+, x0 
;          dcegbfa
ldi x0, 0b01011010 ; 7
ST Z+, x0 
;          dcegbfa
ldi x0, 0b00000000 ; 8
ST Z+, x0 
;          dcegbfa
ldi x0, 0b00010000 ; 9
ST Z+, x0 
;          dcegbfa
ldi x0, 0b01111111 ; blank
ST Z+, x0 

; set prescaler for timer 0 to divide by 64 to yield a frequency of 1 MHz / 64 = 15,625
ldi x0, 0b00000011 
out TCCR0B,x0

; set up counter 0 for 16 bit operation, interrupt on overflow
ldi x0, 0b10000000 
out TCCR0A,x0
ldi x0, 0b00000010 
out TIMSK,x0

ldi x0,0
mov AccumByte2, x0 ; clear accumulator registors
mov AccumByte3, x0
mov  watthourx, x0
mov  watthour0, x0
mov  watthour1, x0

ldi x0, 0b01111111 ; Set port A bits 0:6 for output, 7 for input
out DDRA,x0

ldi x0, 0b00000000 ; Set port A bits 0:6 lo (on for segment test); bit 7 low (no pullup)
out PORTA,x0

ldi x0, 0b01001111 ; Set port B bits 0:3 and 6 for output; others for input
out DDRB,x0

ldi x0, 0b00001111 ; Set port B bits 0:3 (off) and 6 lo (on for segment test)
out PORTB,x0

; wait 1 sec for segment test to complete
  SegmentTest:
  wdr ; reset watchdog timer
  cbi PORTB, 0 ; turn each digit on and off in sequence
  nop
  nop
  nop
  sbi PORTB, 0 
  cbi PORTB, 1 
  nop
  nop
  nop
  sbi PORTB, 1  
  cbi PORTB, 2 
  nop
  nop
  nop
  sbi PORTB, 2  
  cbi PORTB, 3 
  nop
  nop
  nop
  sbi PORTB, 3  
  in x0,TCNT0L ; Read 16 bit counter TCNT0; counts at 15,625/sec                                                      
  in x1,TCNT0H ;                                                     
  cpi x1, 0x3d ; a count of 15,616
  brlo SegmentTest

sei ; set global interrupt enable

; set up analog comparator interrupt
; triggers on rising edge (falling edge of ACO after inverting comparator)
; threshold 1.1 volt, 50 mV hysteresis
; input from meter sensor to Pin 11 (AIN1).

;ACD ACBG ACO ACI ACIE ACME ACIS1 ACIS0
ldi x0, 0b01001010 
out ACSRA,x0

; HSEL HLEV - - - ACM2 ACM1 ACM0
ldi x0, 0b11000000 ;  
out ACSRB,x0

; REFS1 REFS0 ADLAR MUX4 MUX3 MUX2 MUX1 MUX0
ldi x0, 0b10000000 ; multiplexer. REFS1:REFS0 - 1.1 volt reference  
out ADMUX,x0

; ########################################## BEGIN MAIN LOOP ##########################################################
LOOP: ; main loop; repeats forever

wdr ; reset watchdog timer

in x0,TCNT0L  ; turn off LED & output pulse after about 33 msec.  
in x1,TCNT0H  
cpi x1, 2 ; counter 0 has counted to 512; time = 512/15625 = .033
brlo LEDdone
sbi PORTB,6 ; set bit HI to turn LED off. Only leading (falling) edge has precise timing 
LEDdone:

; If RSTDISBL is not programmed, DDB7, PORTB7 and PINB7 will all read 0.
; see if the switch is set to read kWh (hi) or kW (lo)

sbis PINB, 7 ; test bit 7 of port b; skip next instruction if high
rjmp ShowkW 

SetkWh:
  ldi InterruptFlag, 0 ; clear interrupt flag
  mov var10, watthour0 ; send watthour accumulator to BCD conversion routine
  mov var11, watthour1
  rcall Bin2ToBcd5 ; sets mod0:mod3 BCD variables for display
  cpi InterruptFlag, 1  
  breq SetkWh ; Data may have been changed by IntMeterPulse routine; start over

  rjmp UpdateDisplay

ShowkW: ; show current power usage in kW

  rcall div32u  ; computes power in kW
  rcall Bin2ToBcd5 ; sets mod0:mod3 BCD variables for display

UpdateDisplay: ; Multiplex the 4 7-segment display digits; one digit enabled each time around the main loop

  ldi ZH, 0
  inc xd ; digit counter for Display routine
  andi xd, 0b00000011 ; mod 4

  cpi xd, 0
  breq Digit0
  cpi xd, 1
  breq Digit1
  cpi xd, 2
  breq Digit2
  rjmp Digit3

  Digit0:
    sbi PORTB, 0 ; turn off digit 3 
    ldi ZL, LOW(SegmentArray) 
    add ZL, mod0
    ld x0, Z ; retrieve the appropriate segment array from SRAM
	out PORTA, x0 ; send it to the port 
    cbi PORTB, 3 ; turn on digit 0
	rjmp UpdateDisplayDone 

  Digit1:
    sbi PORTB, 3 ; turn off digit 0 
    ldi ZL, LOW(SegmentArray) 
    add ZL, mod1
    ld x0, Z ; retrieve the appropriate segment array from SRAM
	out PORTA, x0 ; send it to the port 
    cbi PORTB, 2 ; turn on digit 1
	rjmp UpdateDisplayDone

  Digit2:
    mov x0, mod2 ; Check if Digit 2 = 0
    cpi x0, 0
    brne Digit2Out

    mov x1, mod3 ; Digit 2 = 0; check if Digit 3 = 0 also
    cpi x1, 0
    brne Digit2Out

    ldi x1, 10
	mov mod2, x1 ; Digits 2 & 3 both = 0; blank Digit 2

    Digit2Out:
    sbi PORTB, 2 ; turn off digit 1 
    ldi ZL, LOW(SegmentArray) 
    add ZL, mod2
    ld x0, Z ; retrieve the appropriate segment array from SRAM
	out PORTA, x0 ; send it to the port 
    cbi PORTB, 1 ; turn on digit 2
	rjmp UpdateDisplayDone

  Digit3:
    mov x1, mod3 ; Check if Digit 3 = 0
    cpi x1, 0
    brne Digit3Out

    ldi x1, 10
    mov mod3, x1 ; Blank Digit 3

    Digit3Out:
    sbi PORTB, 1 ; turn off digit 2 
    ldi ZL, LOW(SegmentArray) 
    add ZL, mod3
    ld x0, Z ; retrieve the appropriate segment array from SRAM
	out PORTA, x0 ; send it to the port 
    cbi PORTB, 0 ; turn on digit 3

  UpdateDisplayDone:

rjmp LOOP

; ################## END MAIN LOOP ###########################


; 32 BIT / 32 BIT DIVISION - THIS WORKS!
;  Call:  var1[3:0] = dividend (0x00000000..0xffffffff)
;         var2[3:0]  = divisor (0x00000001..0x7fffffff)
;         mod[3:0]  = don't care
;         x0            = don't care 
;                        (high register must be allocated)
;  Result:var1[3:0] = var1[3:0] / var2[3:0]
;         var2[3:0]   = not changed
;         mod[3:0]    = var1[3:0] % var2[3:0]
;                      x0 = 0
; Size  = 26 words
; Clock = 549..677 cycles (+ret)
; Stack = 0 bytes
div32u:

	    ;set the divisor, as saved by IntMeterPulse
		SetDivisor:
		ldi InterruptFlag, 0 ; clear interrupt flag 
	    mov var20, div0
	    mov var21, div1
	    mov var22, div2
	    mov var23, div3
		cpi InterruptFlag, 1 
        breq SetDivisor ; Data may have been changed by IntMeterPulse routine; start over

		; set the dividend so result will be in units of decawatts
		; Let N = total count between meter pulses
		;     Kt = watt-hr per meter pulse = 1
		; P (decawatts) = 3600 [sec/hr] X Kt [watt-hr / meter pulse] X .1 [decawatt / watt] X (1/N) [meter pulse / count] X 15625 [counts / sec]  
		; = 5,625,000 / N
		; So, set dividend = 5,625,000 = 0x00 55 D4 A8

		ldi x0, 0xA8
		mov var10, x0
		ldi x0, 0xD4
		mov var11, x0
		ldi x0, 0x55
		mov var12, x0
		ldi x0, 0x00
		mov var13, x0

		clr	mod0
		clr	mod1
		clr	mod2
		clr	mod3
		ldi	x0,32
		lsl	var10
		rol	var11
		rol	var12
		rol	var13
		rol	mod0
		rol	mod1
		rol	mod2
		rol	mod3
		cp	mod0,var20
		cpc	mod1,var21
		cpc	mod2,var22
		cpc	mod3,var23
		brcs	PC+6
		inc	var10
		sub	mod0,var20
		sbc	mod1,var21
		sbc	mod2,var22
		sbc	mod3,var23
		dec	x0
		brne	PC-19

		ret

;############################################################################################

; Bin2ToBcd5 - THIS WORKS!
; ==========
; converts a 16-bit-binary in range (0, 9999) to a 4-digit-BCD
; In: 16-bit-binary in var11:var10
; Out: 4-digit-BCD in mod3:mod0
; Used registers: var11:var10 (changed), var13:var12 (changed),
;   x0
; Called subroutines: Bin2ToDigit
;
Bin2ToBcd5:

	ldi x0,HIGH(1000) ; First with thousands
	mov var13,x0
	ldi x0,LOW(1000)
	mov var12,x0
	rcall Bin2ToDigit ; Calculate digit
	mov mod3, x0

	ldi x0,HIGH(100) ; Next with hundreds
	mov var13,x0
	ldi x0,LOW(100)
	mov var12,x0
	rcall Bin2ToDigit ; Calculate digit
	mov mod2, x0

	ldi x0,HIGH(10) ; Next with tens
	mov var13,x0
	ldi x0,LOW(10)
	mov var12,x0
	rcall Bin2ToDigit ; Calculate digit
	mov mod1, x0

	mov mod0,var10 ; Remainder are ones
	ret ; and return

;############################################################################################
 
; Bin2ToDigit
; ===========
; converts one decimal digit by continued subraction of a
;   binary coded decimal
; Used by: Bin2ToBcd5, Bin2ToAsc5, Bin2ToAsc
; In: 16-bit-binary in var11:var10, binary coded decimal in
;   var13:var12, Z points to current BCD digit
; Out: Result in Z, Z incremented
; Used registers: var11:var10 (holds remainder of the binary),
;   var13:var12 (unchanged), x0
; Called subroutines: -
;
Bin2ToDigit:
	clr x0 ; digit count is zero
Bin2ToDigita:
	cp var11,var13 ; Number bigger than decimal?
	brcs Bin2ToDigitc ; MSB smaller than decimal
	brne Bin2ToDigitb ; MSB bigger than decimal
	cp var10,var12 ; LSB bigger or equal decimal
	brcs Bin2ToDigitc ; LSB smaller than decimal
Bin2ToDigitb:
	sub var10,var12 ; Subtract LSB decimal
	sbc var11,var13 ; Subtract MSB decimal
	inc x0 ; Increment digit count
	rjmp Bin2ToDigita ; Next loop
Bin2ToDigitc:
	ret ; done





















