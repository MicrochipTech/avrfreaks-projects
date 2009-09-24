; Twinkle controller for front door wreath
; Programmed for ATtiny 24/44/84 microcontroller

; Controls 24 LEDs.
; Each LED twinkles inedpendently of other 23 LEDs.
; There are 4 multiplexed groups of 6 LEDs each.
; Ports PA0:PA5 control the individual 6 LEDs in each group (High is on).
; Ports PA6, PA7, PB0 and PB1 control which of the 4 phases is active (Low is on).
; Each phase is active for about 1 msec.
; The hardware is designed to pass about 40ma through the LED while it is on.
; Each phase is on with a max 25% duty cycle, so the average LED current is a safe 10ma max.
; PA0:PA5 are individually pulse width modulated to control intensity of each LED

; The Attiny44 clock should be set to 8 MHz by unprogramming the CKDIV8 Fuse 

.include "tn44def.inc"
.DEVICE ATtiny44; Use the ATtiny44
 
;.undef	XH ; undefine these symbols from tn44def.inc to avoid error message from assembler	
;.undef	XL	; X = R27:R26
;.undef	YH	
;.undef	YL  ; Y = R29:R28 ; Z = R31:R30

; port control variables
.equ PointerPhase0 = 0x0060 ; 24 bytes in SRAM to control duty cycle of each of the 24 LEDs
.equ PointerPhase1 = 0x0066 
.equ PointerPhase2 = 0x006C 
.equ PointerPhase3 = 0x0072 
 
.def RandLo  = R16 ; RandHi:RandLo is the 16 bit random number 
.def RandHi  = R17
 
.def X10 = R18 ; Bits taken from rand (numbered 0 - 15) ; these 5 can also used for other purposes
.def X12 = R19
.def X13 = R20
.def X15 = R21
.def Xor = R22 ; Bit 15 XOR Bit 13 XOR Bit 12 XOR Bit 10

.def counterMPX = R23
.def PointerPhaseX = R24 

.equ offmax = 60 ; max allowed off count (controls minimum intensity)
.equ offmaxneg = -offmax ; negative of offmax
.equ offMaxMinusOne = offmax - 1
 
.CSEG
.org 0000
rjmp START
START:

; initialize port control variables in SRAM
ldi counterMPX, 24
LDI ZH, 0
LDI ZL, PointerPhase0
ldi Xor, offmax
initloop:
  ST Z+, Xor ; store a value of offmax in all 24
  dec counterMPX
brne initloop

ldi Xor, 0b00001000 ; Enable watchdog timer; automatic reset if wdr is not called every 16 msec.
out WDTCSR, Xor ; to WDTCR

ldi Xor, 0b11111111 ; Set all port A pins for output
out DDRA,Xor

ldi Xor, 0b11000000 ; Set port A bits 0:5 low (off); bits 6:7 high (off)
out PORTA,Xor

ldi Xor, 0b00000011 ; Set Ports PB0 & PB1 to enable output
out DDRB,Xor

sbi PORTB,0 ; Set PB0 HIGH (off) (2 clock cycles)
sbi PORTB,1 ; Set PB1 HIGH (off) (2 clock cycles)

ldi RandLo, 0b10100111 ; Initial (seed) value for the random number generator
ldi RandHi, 0b11101100     ; Use any value except all zeroes.


; Set stack pointer to end of SRAM for subroutine call
LDI Xor, HIGH(RAMEND) ; Upper byte
OUT SPH,Xor ; to stack pointer
LDI Xor, LOW(RAMEND) ; Lower byte
OUT SPL,Xor ; to stack pointer


MainLOOP: ; this loop repeats forever, about every 4 msec
; In this loop:
;  1) A new 16 bit random variable (RV) is computed
;  2) The low byte of the RV is used to (possibly) select one of LED's to twinkle
;  5) The selected port control variable is updated and saved
;  6) The 4 phases are multiplexed, 1 msec per phase, for about 100 msec.
;  7) The loop is repeated. 

; Generate a 16 bit random number, RandHi:RandLo, using Fibonacci LFSR method
; This sequence repeats after 65536 numbers

wdr ; reset watchdog timer

ldi X10, 0 ; reset to all zeroes
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

cpi RandLo, 24 ; probablitity 24/256 one LED will be picked to twinkle on this iteration 
brsh TimerLoop

ldi Xor, PointerPhase0 ; compute pointer to the port control variable selected
add Xor, RandLo
ldi ZH, 0
mov ZL, Xor

LD Xor, Z ; load timer variable into Xor (2 cycles)
cpi Xor, offmax
brne TimerLoop ; don't initiate a twinkle if one is already in progress  
ldi Xor, offMaxMinusOne
ST Z, Xor ; Initiate a twinkle by setting the selected port control variable to offmax - 1

TimerLoop:

; Phase 0 timing loop. Approx 8000 cycles (1 msec). PA6 active
ldi PointerPhaseX , PointerPhase0 
cbi PORTA,6 ; Set PA6 low to enable Phase 0 (2 clock cycles)
rcall PhaseSub
sbi PORTA,6 ; Set PA6 high to disable Phase 0 (2 clock cycles)

; Phase 1 timing loop. Approx 8000 cycles (1 msec). PA7 active
ldi PointerPhaseX , PointerPhase1 
cbi PORTA,7 ; Set PA6 low to enable Phase 0 (2 clock cycles)
rcall PhaseSub
sbi PORTA,7 ; Set PA6 high to disable Phase 0 (2 clock cycles)

; Phase 2 timing loop. Approx 8000 cycles (1 msec). PB1 active
ldi PointerPhaseX , PointerPhase2 
cbi PORTB,1 ; Set PB1 low to enable Phase 2 (2 clock cycles)
rcall PhaseSub
sbi PORTB,1 ; Set PB1 low to disable Phase 2 (2 clock cycles)

; Phase 3 timing loop. Approx 8000 cycles (1 msec). PB0 active
ldi PointerPhaseX , PointerPhase3 
cbi PORTB,0 ; Set PB0 low to enable Phase 2 (2 clock cycles)
rcall PhaseSub
sbi PORTB,0 ; Set PB0 low to disable Phase 2 (2 clock cycles)

rjmp MainLOOP ; Repeat the loop. (2 clock cycles if taken)

;----------------------------------------------------------------------------------
PortSub: ; subroutine for processing each port, PA0:PA5

; On each call:

  ; If the port control variable (controls off time) is = offmax, do nothing
  ; else if the port variable = -offmax, set it to offmax
  ; else decrement the port variable 

; once a twinkle is initiated by setting the port control variable to offmax - 1, then
; the port control variable is decremented on each call until it reaches -offmax, at which
; time it is set to offmax. The actual off time for the LED is proportional to the absolute
; value of the port control variable.

; Varaible Xor contains the absolute value when this subroutine returns.

; cycles
;  22 if port control variable = offmax  
;  22 if port control variable = -offmax
;  22 decrement
;  22 skip  

  LD Xor, Z ; load timer variable into Xor (2 cycles)

  cpi counterMPX, 0 ; skip changes to port control variable except on first pass thru each phase
  breq sub1
  adiw ZH:ZL,1 ; Increment the Z-pointer(r31:r30) (2 cycles)
  nop
  nop
  nop
  nop
  nop
  nop
  rjmp sub5 ; (2 cycles)

  
  sub1:
  cpi Xor, offmax ; check if off time at max
  brne sub2 ; do nothing (2 cycles if taken)
  adiw ZH:ZL,1 ; Increment the Z-pointer(r31:r30) (2 cycles)
  nop
  nop
  nop
  rjmp sub5 ; (2 cycles)

  
  sub2:
  cpi Xor, offmaxneg ; check if off time at negative of max
  brne sub3  ; (2 cycles if taken)
  ldi Xor, offmax ; change to offmax
  rjmp sub4 ;(2 cycles)
  
  sub3:
  dec Xor

  sub4:
  ST Z+, Xor ; store Xor in timer variable & increment Z register (2 cycles)

  sub5:
  cpi Xor, 0   ; take |Xor|
  brge sub6 ; (2 cycles if taken)
  neg Xor
  
  sub6:
  lsr Xor ; divide by two & double offmax to slow down rate of twinkle
ret ; (4 cycles)
;----------------------------------------------------------------------------------

;----------------------------------------------------------------------------------
PhaseSub: ; Subroutine for processing each of the 4 phases. 7845 cycles (980 usec @ 8 MHz clock)

  
  cbi PORTA,0 ; set port low to turn off LED  
  cbi PORTA,1 ; set port low to turn off LED  
  cbi PORTA,2 ; set port low to turn off LED  
  cbi PORTA,3 ; set port low to turn off LED  
  cbi PORTA,4 ; set port low to turn off LED  
  cbi PORTA,5 ; set port low to turn off LED  
 
  ldi counterMPX, 0

  PhaseSubLoop: ; repeats 45 times - 174 cycles each iteration

  ldi ZH, 0 ; reset the pointer
  mov ZL, PointerPhaseX

  ; PA0: 28 cycles for this section 
  rcall PortSub ; call routine to process port variable (3 + 22 for subroutine = 25 cycles)
  cp counterMPX, Xor  
  brne step00
  sbi PORTA,0 ; set port High to turn turn on LED  
  step00:

  ; PA1: 28 cycles for this section 
  rcall PortSub ; call routine to process port variable (3 + 22 for subroutine = 25 cycles)
  cp counterMPX, Xor  
  brne step01
  sbi PORTA,1 ; set port High to turn turn on LED  
  step01:

  ; PA2: 28 cycles for this section 
  rcall PortSub ; call routine to process port variable (3 + 22 for subroutine = 25 cycles)
  cp counterMPX, Xor  
  brne step02
  sbi PORTA,2 ; set port High to turn turn on LED  
  step02:

  ; PA3: 28 cycles for this section 
  rcall PortSub ; call routine to process port variable (3 + 22 for subroutine = 25 cycles)
  cp counterMPX, Xor  
  brne step03
  sbi PORTA,3 ; set port High to turn turn on LED  
  step03:

  ; PA4: 28 cycles for this section 
  rcall PortSub ; call routine to process port variable (3 + 22 for subroutine = 25 cycles)
  cp counterMPX, Xor  
  brne step04
  sbi PORTA,4 ; set port High to turn turn on LED  
  step04:

  ; PA5: 28 cycles for this section 
  rcall PortSub ; call routine to process port variable (3 + 22 for subroutine = 25 cycles)
  cp counterMPX, Xor  
  brne step05
  sbi PORTA,5 ; set port High to turn turn on LED  
  step05:

  inc counterMPX
  cpi counterMPX, 45
  brlo PhaseSubLoop

ret ;(4 cycles)
;----------------------------------------------------------------------------------


