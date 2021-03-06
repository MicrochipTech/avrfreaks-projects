; Flicker controller for blow mold candles
; Programmed for ATtiny25 microcontroller
; Only 124 bytes of flash memory are used, so this should work with any of the ATtiny microcontrollers

; Output signal is pulse width modulated to control LED brightness.
; Timing is set for about 70 samples/sec output with default internal clock of 1.0 MHz

; Output is to PB0 (pin 5) or PB1 (pin 6).
; PB0 is low when LED is on; PB1 is high when LED is on


.include "tn25def.inc"
.DEVICE ATtiny25; Use the ATtiny25
  
.def RandLo  = R16 ; RandHi:RandLo is the 16 bit random number 
.def RandHi  = R17 
 
.def X10 = R18 ; Bits taken from rand (numbered 0 - 15)
.def X12 = R19
.def X13 = R20
.def X15 = R21
 
.def Xor = R22 ; Bit 15 XOR Bit 13 XOR Bit 12 XOR Bit 10
.def Xsum = R23 ; variable that controls pulse width modulation
.def Xled = R24

.def Counter = R25
 
.CSEG
.org 0000
rjmp START
START:

ldi Xor, 0b00001110 ; Enable watchdog timer; automatic reset if wdr is not called.
out WDTCR, Xor ; to WDTCR

ldi Xor, 0b00000011 ; Set Ports PB0 & PB1 to enable output
out DDRB,Xor  

ldi RandLo, 0b10100111 ; Initial (seed) value for the random number generator
ldi RandHi, 0b11101100     ; Use any value except all zeroes.

ldi Xsum, 96 ; initialize to 96

LOOP: ; this loop repeats forever, 70 times per second

; Turn LEDs on
cbi PORTB,0 ; Set PB0 LOW (2 clock cycles)
sbi PORTB,1 ; Set PB1 HIGH (2 clock cycles)

wdr ; reset watchdog timer

; Generate a 16 bit random number, RandHi:RandLo, using Fibonacci LFSR method
; This sequence repeats after 65536 numbers (15.6 minutes @ 70 samples/sec)

ldi X10, 0 ; just to be sure bits 7:1 are still all zeroes
ldi X12, 0
ldi X13, 0
ldi X15, 0

bst RandHi,2 ; Get bit 10 of RandHi:RandLo (bit 2 of RandHi). Store in T Flag
bld X10,0 ; Load T into bit 0 of X10

bst RandHi,4 ; repeat for bits 12, 13 & 15
bld X12,0 

bst RandHi,5 
bld X13,0 

bst RandHi,7 
bld X15,0

mov Xor,X15 ; copy bit 15 to Xor
eor Xor,X13 ; XOR with bit 13
eor Xor,X12 ; XOR with bit 12 
eor Xor,X10 ; XOR with bit 10  

lsl RandLo ; left shift RandLo & leave bit 7 in Carry Flag
rol RandHi ; left shift RandHi & move Carry Flag into bit 0 - This makes a complete left shift of RandHi:RandLo

or RandLo, Xor ; OR to copy Xor into bit 0 of RandHi:RandLo. The result is the new 16 bit random number   

mov Xor, RandLo ; copy low byte to Xor
andi Xor, 0b00011111 ; take least significant 5 bits to yield a random variable in the range of (0,31)

; Integrator to slow flicker rate.
add Xsum, Xor
subi Xsum, 16 ; Subtract 16 from Xsum to (equivalently) make X10 a symmetric random variable in range (-16,15)

; set lower limit of 35 on Xsum - controls minimum brightness of LEDs. Can set in range 10 to 127
cpi Xsum,35 
brsh step2 ; Skip next instruction if Xsum >= 35
ldi Xsum, 35 ; Set Xsum = 35
step2:

; set upper limit of 160 on Xsum - controls amount of time LEDs are at full brightness. Do not set > 224 or < 127.
cpi Xsum,161 
brlo step3 ; Skip next instruction if Xsum <= 160
ldi Xsum, 160 ; Set Xsum = 160
step3:

; copy Xsum into Xled - The brightness of the LEDs is proportional to Xled
mov Xled, Xsum

; set upper limit of 127 on Xled - Do not change this number 
cpi Xled,128 
brlo step4 ; Skip next instruction if Xsum <= 127
ldi Xled, 127 ; Set Xled = 127
step4:
; verified Xsum & Xled same as fortran simulation

mov Counter, Xled ; Copy Xled into Counter

OnDelay: ; delay 110 X Counter cycles 

  ldi Xor, 36  ; 107 cycles for inner loop
  TimeLoopOn:
    dec Xor
  brne TimeLoopOn

  dec Counter
brne OnDelay ; (2 clock cycles)

; Turn Leds off

ldi Counter, 127 ; value for 100% duty cycle
sub Counter, Xled ; Subtract Xled to get value for OFF time
cpi Counter, 1 ; if the value < 1, skip turning off the LEDs
brlo LOOP 

sbi PORTB,0 ; Set PB0 HIGH (2 clock cycles)
cbi PORTB,1 ; Set PB1 LOW (2 clock cycles)

OffDelay: ; delay 110 X Counter cycles 

  ldi Xor, 36  ; 107 cycles for inner loop
  TimeLoopOff:
    dec Xor
  brne TimeLoopOff

  dec Counter
brne OffDelay ; (2 clock cycles)

rjmp LOOP ; Repeat the loop. (rjmp = 2 clock cycles)

