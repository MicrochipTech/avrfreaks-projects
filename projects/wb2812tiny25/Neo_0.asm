;*******************************************
; Neo.asm
;


;      Tiny25 fuse settings ::
; Value    high byte                     Value  low byte               extended
;   1        7  - ExtResetDisable         1    7  - CKDIV8		-
;   1        6  - DebugWireEnb            1    6  - CKOUT		-
;   0        5  - SPIenable               1    5  - SUT1		-
;   1 (dft)  4  - WatchDog ON             0    4  - SUT0		-
;   1 (dft)  3  - EESAVE                  0    3  - CKSEL3		-
;   1 (dft)  2  - BODlev2                 0    2  - CKSEL2		-
;   1 (dft)  1  - BODlev	          1    1  - CKSEL1		-
;   1 (dft)  0  - BODlev0                 0    0  - CKSEL0    SELFPRGEN self-prg enabled
;      111 BOD disabled


; Atmel AVR Tiny25 microcontroller
;             _____
;     reset\ |1   8| Vcc
;         PB3|2   7| pb2 SCK
;NeoPixel PB4|3	  6| pb1 MISO
;         Gnd|4   5| pb0 MOSI
;	     |_____|

.nolist
.include "tn25def.inc"
.list

.equ	Clock			= 8000000
.equ	HeartBeatsPerSecond 	= 100
.equ	Timer1Reload		= ~((Clock/16384) / HeartBeatsPerSecond)   ;488.28 for Tiny25


.equ	NEOPIXELBIT		= 4	; Port B4 [pin 3]
.equ	HEARTBEATS_OFF_FRAME	= 30 ; speed at which LEDs change  units are 1/100ths of a second 
.equ	HEARTBEATS_ON_FRAME	= 18 ; speed at which LEDs change  units are 1/100ths of a second
;5 frames per second [a frame is a snapshot of the GRB values of each Neo in


.def	SREGtemp		= r0
.def	oneReg			= r1
.def	zeroReg			= r2
.def	NumberOfNeoPixelsReg	= r3
.def	GreenIntensityReg	= r4
.def	RedIntensityReg		= r5
.def	BlueIntensityReg	= r6
.def	LEDvalueReg		= r7
.def	count			= r8
.def	togglePB4maskReg 	= r9
.def	ADCstore                = r10	; most sig 8-bits of 10-bit ADC conversion
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.def	temp			= r16
.def	temp2			= r17
.def	irqtemp			= r18


;***************************************************
;  Data Segment
;
;  Static RAM Arrays, tables, stacks, and queues
;
;***************************************************
.dseg
isNeoPixelOn:		.byte   1

heartBeat_Low:  	.byte   1
heartBeat_Med:  	.byte   1
heartBeat_High: 	.byte   1
heartBeat_Count:	.byte   1

; color data for current NeoPixel frame.  Three bytes [GRB] for each neoPixel device.
Green0:			.byte	1    ; a 3-byte Gn-Rd-Bl struct for each NeoPixel
Red0:			.byte	1
Blue0:			.byte	1
Green1:			.byte	1
Red1:			.byte	1
Blue1:			.byte	1
Green2:			.byte	1
Red2:			.byte	1
Blue2:			.byte	1
Green3:			.byte	1
Red3:			.byte	1
Blue3:			.byte	1
Green4:			.byte	1
Red4:			.byte	1
Blue4:			.byte	1



;***************************************************
;  Data Segment
;
;  Static RAM Arrays, tables, stacks, and queues
;
;***************************************************
.dseg
heartBeat:	.byte	1


;*********************************
;	Code Segment
;*********************************
.cseg
.org	$0000
	rjmp reset	; Reset handler $000
	reti		; 0x0001 External Interrupt_0
	reti		; 0x0002 Pin change Interrupt Request_0
	reti		; 0x0003 Timer/Counter1 Compare Match 1A
	rjmp T1CntOvflw	; 0x0004 Timer/Counter1 Overflow   // section 12 PDF june10
	reti	;rjmp T0CntOvflw	; 0x0005 Timer/Counter0 Overflow
	reti		; 0x0006 EEPROM Ready
	reti		; 0x0007 Analog comparator
	reti		; 0x0008 ADC Conversion ready
	reti		; 0x0009 Timer/Counter1 Compare Match B
	reti		; 0x000a Timer/Counter0 Compare Match A
	reti		; 0x000b Timer/Counter0 Compare Match B
	reti		; 0x000c Watchdog Time-out
	reti		; 0x000d USI START
	reti		; 0x000e USI Overflow


reset:
; initialize Timers in the Tiny25
	ldi	temp, (1 << TOIE1) ;| (1<< TOIE0)  ; enable timer overflow interrupts
	out	TIMSK, temp	; 12.3.7
	ldi	temp, Timer1Reload ; 488 increments/sec  488*16=7808
	out	TCNT1, temp	; 0x2f_sfr       13.3.3
	ldi	temp, (1<<CS13) | (1<<CS12) | (1<<CS11) | (1<<CS10) ;  CLK/16384  table 12.5
	out	TCCR1 , temp	;
;  compare to the Timer1 in the Tiny1634
;   t1634 has CLK/1024 but not CLK/16384  488*16=

	ldi	temp, (1<<NEOPIXELBIT)	; 76543210 PortB bit 4 is the NeoPixel data pin
	mov	oneReg, temp		; 00010000
	ldi	temp, 0
	mov	zeroReg, temp

	ldi	temp, (1<<NEOPIXELBIT)
	out	DDRB, temp

 	clr	temp
	sts	heartBeat_Low, temp
	sts	heartBeat_Med, temp
	sts	heartBeat_High, temp
	sts	heartBeat_Count, temp

	sei	; interrupts needed by heartBeat counter

;  initialize the NeoPixel LED color display and sequence
	ser	temp
	sts	isNeoPixelOn, temp

	clr	zl     ; ZH:ZL is a index into the flash program bytes. Values are used as random
	clr	zh     ; which are used to light the NeoPixels in random colors.

	lpm	temp, z+   ; color on neo2 is from flash, color on neo1 and neo3 is prev neo2 colo
	sts	Green0, temp   ; color on neo0 and neo4 is prev neo1:neo3 color
	sts	Green4, temp
	lpm	temp, z+
	sts	Red0, temp
  	sts	Red4, temp
	lpm	temp, z+
	sts	Blue0, temp
  	sts	Blue4, temp
	lpm	temp, z+   ; color on neo2 is from flash, color on neo1 and neo3 is prev neo2 colo
	sts	Green1, temp   ; color on neo0 and neo4 is prev neo1:neo3 color
	sts	Green3, temp
	lpm	temp, z+
	sts	Red1, temp
  	sts	Red3, temp
	lpm	temp, z+
	sts	Blue1, temp
  	sts	Blue3, temp
;__________________________________________________________________________



;********************************************
;********************************************
Main:

m_nx0:
	lds	temp, heartBeat_Count
	cpi	temp, HEARTBEATS_ON_FRAME
	breq	m_1
	rjmp	m_nx0
	
m_1:	clr	temp
	sts	heartBeat_Count, temp
	rcall	NeoFrame

m_nx1:
	lds	temp, heartBeat_Count
	cpi	temp, HEARTBEATS_OFF_FRAME
	breq	m_2
	rjmp	m_nx1
	
m_2:	clr	temp
	sts	heartBeat_Count, temp
	rcall	TurnOffAllNeoPixels
	
m_nx2:	rjmp	Main



;********************************************
;********************************************




;********************************************
;	Subroutines
;********************************************
NeoFrame:   ; uses ZH:ZL as a pointer in flash code to make pseudo-random #

;  main code checks if   HBcount == HEARTBEATSPERFRAME.
;  If not, then NF not called.
;  if yes, then read 3 bytes from flash, and use them as new GBR
;  for neopixel color on center LED. copy other LEDs.


; check if at pseudo-random pointer is at the end of the flash program data.
; 0x160 is the size of the block of flash program data used to make pseudo-random colors.
	cpi	zh,1
	brne	NeoFrameOn
	cpi	zl, 0x60
	brlo	NeoFrameOn
	ldi	zl, 0x10
	clr	zh

; Update the SRAM array of the LED brightness values for each NeoPixel device.
; copy inside NeoLED colors to the outside, 
;  -- put the previous color from neoPxl#1 [and #3] on neoPxl#0 and #4 [the outer neoPixels].  
;  -- put the previous center NeoPxl's color on neoPxl#1 and #3 (the ones next to the center)  
; get three bytes from the flash program listing and
; use them to make a random GRB color on the center NeoPx [#2, of 0-4].
NeoFrameOn:
	lds	temp, Green1    ; copy the color of the middle pixels to the edge pixels
	sts	Green0, temp    ;   x C x C x  ===> C C x C C
	sts	Green4, temp    ;   0 1 2 3 4       0 1 2 3 4
	lds	temp, Red1
	sts	Red0, temp
	sts	Red4, temp
	lds	temp, Blue1
	sts	Blue0, temp
	sts	Blue4, temp

	lds	temp, Green2    ;  copy the color of the center pixel to the middle pixels
	sts	Green1, temp    ;   x x C x x  ==>  x C C C x
	sts	Green3, temp    ;   0 1 2 3 4       0 1 2 3 4
	lds	temp, Red2
	sts	Red1, temp
	sts	Red3, temp
	lds	temp, Blue2
	sts	Blue1, temp
	sts	Blue3, temp

	lpm	temp, z+	; put a new color [GRB:3 bytes read from flash] on the center pixel
	sts	Green2, temp
	lpm	temp, z+
	sts	Red2, temp
	lpm	temp, z+
	sts	Blue2, temp

NeoF_loop:    ;  change NeoPixel values on a 5-device group.
	ldi	temp, 5
	mov	NumberOfNeoPixelsReg, temp
	ldi	YH, high (Green0)
	ldi	YL, low  (Green0)
NeoF_lp2:
	ld	GreenIntensityReg, y+
	ld	RedIntensityReg, y+
	ld	BlueIntensityReg, y+
	rcall	LightOneNeoPixel
	dec	NumberOfNeoPixelsReg
	brne	NeoF_lp2

NeoFrameExit:
	ret


;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TurnOffAllNeoPixels:

	clr	temp
	sts	heartBeat_Count, temp
	
	ldi	temp, 5                                                
	ldi	temp2, 0x00                                                        
Toff_lp:                                             
	mov	GreenIntensityReg, temp2
	mov	RedIntensityReg, temp2                     
	mov	BlueIntensityReg, temp2                        
	rcall	LightOneNeoPixel                             
	dec	temp                       
	brne	Toff_lp                                    
                         
TnOffExit:
	ret      
                         
                         
                         
;=================================================================
;  LightOne_NeoPixel    8MHz system clock :: 800KHz neos
;   Receives three 8-bit values in low registers:
;     RedIntensityReg, GreenIntensityReg, BlueIntensityReg.
;
;   Uses four additional registers: LEDvalueReg and temp as temporary storage.
;   and has two registers with unchanging pre-defined values:
;      	oneReg  = PORTB |  pinMask
;      	zeroReg = PORTB & ~pinMask
;
;   1st period write PortB3 high 0.4uS
;   2nd period write PortB3 either high or low depending on bit value
;   3rd period_write PortB3 low for 0.4uS
;
;
;  code doesn't loop after testing and sending each bit. All 24 data bits
;  sent in a long code sequence, most sig bit first. Green:Red:Blue
;-----------------------------------------------------------------------
LightOneNeoPixel:
	cli

	mov	LEDvalueReg, GreenIntensityReg
 	mov	temp,	zeroReg
	sbrc	LEDvalueReg, 7
	mov	temp,	oneReg
GreenLED:		; three periods per bit: High:0.4uS  data:0.45uS  Low:0.40uS
	out	PORTB,	oneReg		;Grn7H    1 cycles    0.000     x
	mov	temp2,	zeroReg		;         1 cycle     0.125     H
	out	PORTB,	temp		;Grn7D    1 cycle     0.250     H
	nop				;         1 cycle     0.375     D
	nop				;         1 cycle     0.500     D
	sbrc	LEDvalueReg, 6		;       1_2 cycle     0.625     D
	mov	temp2,	oneReg		;         1 cycle     0.870     D
	out	PORTB ,	zeroReg		;Grn7L    1 cycle     1.000     D
	nop				;         1 cycle     1.125     L
	nop				;         1 cycle     1.250     L

	out	PORTB,	oneReg		;Grn6H    1 cycle     1.375     L
	mov	temp,	zeroReg		;         1 cycle     1.500     H
	out	PORTB,	temp2		;Grn6D    1 cycle     1.625     H
	nop				;         1 cycle     1.750     L
	nop				;         1 cycle     1.870     L
	sbrc	LEDvalueReg, 5		;       1_2 cycle     2.125     D
	mov	temp,	oneReg 		;         1 cycle     2.250     D
	out	PORTB,	zeroReg		;Grn6L    1 cycle     2.375     D
	nop				;         1 cycle     2.500     L
	nop				;         1 cycle     2.625     L

	out	PORTB,	oneReg 		;Grn5H    1 cycle     2.750     L
	mov	temp2,	zeroReg		;         1 cycle     3.000     H
	out	PORTB,	temp		;Grn5D    1 cycle     3.125     H
	nop				;         1 cycle     3.250     L
	nop				;         1 cycle     3.375     L
	sbrc	LEDvalueReg, 4		;       1/2 cycle     3.50      D
	mov	temp2,	oneReg		;         1 cycle     3.75      D
	out	PORTB,	zeroReg		;Grn5L    1 cycle     3.875     D
	nop				;         1 cycle     4.0       L
	nop				;         1 cycle     4.125     L

	out	PORTB,	oneReg 		;Grn4H    1 cycle     4.250     L
	mov	temp,	zeroReg		;         1 cycle     4.375     H
	out	PORTB,	temp2  		;Grn4D    1 cycle     4.500
	nop				;         1 cycle     4.625     L
	nop				;         1 cycle     4.750     L
	sbrc	LEDvalueReg, 3		;       1/2 cycle     5.000     D
	mov	temp, oneReg 		;         1 cycle     5.125     D
	out	PORTB, zeroReg		;Grn4L    1 cycle     5.250     D
	nop				;         1 cycle     5.375     L
	nop				;         1 cycle     5.500     L

	out	PORTB,oneReg 		;Grn3H    1 cycle     5.625     L
	mov	temp2,zeroReg		;         1 cycle     5.750     H
	out	PORTB,temp		;Grn3D    1 cycle     5.875
	nop				;         1 cycle     6.000     L
	nop				;         1 cycle     6.125     L
	sbrc	LEDvalueReg, 2		;       1/2 cycle     6.250     D
	mov	temp2,	oneReg		;         1 cycle     6.375     D
	out	PORTB,	zeroReg		;Grn3L    1 cycle     6.500     D
	nop				;         1 cycle     6.625     L
	nop				;         1 cycle     6.750     L

	out	PORTB, oneReg 		;Grn2H    1 cycle     6.875     L
	mov	temp, zeroReg		;         1 cycle     7.000     H
	out	PORTB, temp2		;Grn2D    1 cycle     7.125     D
	nop				;         1 cycle     7.250     D
	nop				;         1 cycle     7.375     D
	sbrc	LEDvalueReg, 1		;       1/2 cycle     7.500     D
	mov	temp, oneReg 		;         1 cycle     7.750     D
	out	PORTB, zeroReg		;Grn2L    1 cycle     7.875     D
	nop				;         1 cycle     8.000     L
	nop				;         1 cycle     8.125

	out	PORTB, oneReg		;Grn1H    1 cycle     8.250     H
	mov	temp2, zeroReg		;         1 cycle     8.375     H
	out	PORTB, temp		;Grn1D    1 cycle     8.500     D
	nop				;         1 cycle     8.625     D
	nop				;         1 cycle     8.875     D
	sbrc	LEDvalueReg, 0		;       1/2 cycle     9.000     L
	mov	temp2, oneReg		;         1 cycle     9.125     L
	out	PORTB, zeroReg		;Grn1L    1 cycle     9.250
	mov	LEDvalueReg,RedIntensityReg	;  <<<<<< 2 cycle     9.375   L
	out	PORTB, oneReg		;Grn0H    1 cycle     9.50      L
	mov	temp, zeroReg		;         1 cycle     9.625     H
	out	PORTB, temp2		;Grn0D    1 cycle     9.750     H
	sbrc	LEDvalueReg,  7		;	1/2 cycle     9.875     D
	mov	temp, oneReg		;	  1 cycle    10.000     D
	out	PORTB, zeroReg		;Grn0L	  1 cycle    10.125     D

	mov	temp,	zeroReg
	sbrc	LEDvalueReg, 7
	mov	temp,	oneReg
RedLED:		; three periods per bit: High:0.4uS  data:0.45uS  Low:0.40uS
	out	PORTB, oneReg		;Red7H    1 cycles       0     x
	mov	temp2, zeroReg		;Red      1 cycle      .125    H
	out	PORTB, temp		;Red7D    1 cycle      .25     H
	nop				;Red      1 cycle      .375    D
	nop				;Red      1 cycle      .50     D
	sbrc	LEDvalueReg, 6		;Red    1_2 cycle      .625    D
	mov	temp2,	oneReg		;Red      1 cycle      .87     D
	out	PORTB ,	zeroReg		;Red7L    1 cycle     1.00     D
	nop				;Red      1 cycle     1.125    L
	nop				;Red      1 cycle     1.25     L

	out	PORTB, oneReg		;Red6H    1 cycle     1.375    L
	mov	temp,  zeroReg		;Red      1 cycle     1.50     H
	out	PORTB, temp2		;Red6D    1 cycle     1.625    H
	nop				;Red      1 cycle     1.75     L
	nop				;Red      1 cycle     1.87     L
	sbrc	LEDvalueReg, 5  	;Red    1/2 cycle     2.125    D
	mov	temp,	oneReg 		;Red      1 cycle     2.25     D
	out	PORTB,	zeroReg		;Red6L    1 cycle     2.375    D
	nop				;Red      1 cycle     2.50     L
	nop				;Red      1 cycle     2.625    L

	out	PORTB, oneReg 		;Red5H    1 cycle     2.75     L
	mov	temp2, zeroReg		;Red      1 cycle     3.0      H
	out	PORTB, temp		;Red5D    1 cycle     3.125    H
	nop				;Red      1 cycle     3.25     L
	nop				;Red      1 cycle     3.375    L
	sbrc	LEDvalueReg, 4		;Red    1/2 cycle     3.50     D
	mov	temp2,	oneReg		;Red      1 cycle     3.75     D
	out	PORTB,	zeroReg		;Red5L    1 cycle     3.875    D
	nop				;Red      1 cycle     4.0      L
	nop				;Red      1 cycle     4.125    L

	out	PORTB,oneReg 		;Red4H    1 cycle     4.250    L
	mov	temp, zeroReg		;Red      1 cycle     4.375    H
	out	PORTB,temp2  		;Red4D    1 cycle     4.50     H
	nop				;Red      1 cycle     4.625    D
	nop				;Red      1 cycle     4.75     D
	sbrc	LEDvalueReg, 3 		;Red    1/2 cycle     5.0      D
	mov	temp, oneReg 		;Red      1 cycle     5.125    D
	out	PORTB, zeroReg		;Red4L    1 cycle     5.25     D
	nop				;Red      1 cycle     5.375    L
	nop				;Red      1 cycle     5.50     L

	out	PORTB, oneReg 		;Red3H    1 cycle     5.6250   L
	mov	temp2, zeroReg		;Red      1 cycle     5.75     H
	out	PORTB, temp		;Red3D    1 cycle     5.8750
	nop				;Red      1 cycle     6.00     L
	nop				;Red      1 cycle     6.125    L
	sbrc	LEDvalueReg, 2		;Red    1/2 cycle     6.25     D
	mov	temp2, oneReg  		;Red      1 cycle     6.375    D
	out	PORTB, zeroReg		;Red3L    1 cycle     6.50     D
	nop				;Red      1 cycle     6.625    L
	nop				;Red      1 cycle     6.750    L

	out	PORTB, oneReg 		;Red2H    1 cycle     6.8750   L
	mov	temp , zeroReg		;Red      1 cycle     7.000    H
	out	PORTB, temp2 		;Red2D    1 cycle     7.125    D
	nop				;Red      1 cycle     7.250    D
	nop				;Red      1 cycle     7.375    D
	sbrc	LEDvalueReg, 1		;Red    1/2 cycle     7.500    D
	mov	temp, oneReg 		;Red      1 cycle     7.750    D
	out	PORTB, zeroReg		;Red2L    1 cycle     7.875    D
	nop				;Red      1 cycle     8.000    L
	nop				;Red      1 cycle     8.125

	out	PORTB, oneReg		;Red1H    1 cycle     8.250    H
	mov	temp2, zeroReg		;Red      1 cycle     8.375    H
	out	PORTB, temp		;Red1D    1 cycle     8.500    D
	nop				;Red      1 cycle     8.625    D
	nop				;Red      1 cycle     8.875    D
	sbrc	LEDvalueReg, 0		;Red    1/2 cycle     9.000    L
	mov	temp2, oneReg		;Red      1 cycle     9.125    L
	out	PORTB, zeroReg 		;Red1L    1 cycle     9.250
	mov	LEDvalueReg, BlueIntensityReg	;Red      2 cycle     9.375   L

	out	PORTB, oneReg		;Red0H    1 cycle     9.50    L
	mov	temp , zeroReg		;Red      1 cycle     9.625   H
	out	PORTB, temp2		;Red0D    1 cycle     9.750   H
	sbrc	LEDvalueReg, 7		;Red    1/2 cycle     9.875   D
	mov	temp, oneReg		;Red      1 cycle    10.000   D
	out	PORTB, zeroReg		;Red0L	  1 cycle    10.125   D

	mov	temp,	zeroReg
	sbrc	LEDvalueReg, 7
	mov	temp, oneReg
BlueLED:		; three periods per bit: High:0.4uS  data:0.45uS  Low:0.40uS
	out	PORTB, oneReg		;Blue7H    1 cycles       0     x
	mov	temp2, zeroReg		;Blue      1 cycle      .125    H
	out	PORTB, temp		;Blue7D    1 cycle      .25     H
	nop				;Blue      1 cycle      .375    D
	nop				;Blue      1 cycle      .50     D
	sbrc	LEDvalueReg, 6		;Blue    1_2 cycle      .625    D
	mov	temp2, oneReg		;Blue      1 cycle      .87     D
	out	PORTB, zeroReg		;Blue7L    1 cycle     1.00     D
	nop				;Blue      1 cycle     1.125    L
	nop				;Blue      1 cycle     1.25     L

	out	PORTB,	oneReg		;Blue6H    1 cycle     1.375    L
	mov	temp,	zeroReg		;Blue      1 cycle     1.50     H
	out	PORTB,	temp2		;Blue6D    1 cycle     1.625    H
	nop				;Blue      1 cycle     1.75     L
	nop				;Blue      1 cycle     1.87     L
	sbrc	LEDvalueReg, 5  	;Blue    1_2 cycle     2.125    D
	mov	temp,	oneReg 		;Blue      1 cycle     2.25     D
	out	PORTB,	zeroReg		;Blue6L    1 cycle     2.375    D
	nop				;Blue      1 cycle     2.50     L
	nop				;Blue      1 cycle     2.625    L

	out	PORTB,	oneReg 		;Blue5H    1 cycle     2.75     L
	mov	temp2, zeroReg		;Blue      1 cycle     3.0      H
	out	PORTB, temp		;Blue5D    1 cycle     3.125    H
	nop				;Blue      1 cycle     3.25     L
	nop				;Blue      1 cycle     3.375    L
	sbrc	LEDvalueReg, 4		;Blue    1_2 cycle     3.50     D
	mov	temp2,	oneReg		;Blue      1 cycle     3.75     D
	out	PORTB,	zeroReg		;Blue5L    1 cycle     3.875    D
	nop				;Blue      1 cycle     4.0      L
	nop				;Blue      1 cycle     4.125    L

	out	PORTB, oneReg		;Blue4H    1 cycle     4.250    L
	mov	temp, zeroReg		;Blue      1 cycle     4.375    H
	out	PORTB, temp2		;Blue4D    1 cycle     4.50
	nop				;Blue      1 cycle     4.625    L
	nop				;Blue      1 cycle     4.75     L
	sbrc	LEDvalueReg, 3		;Blue    1_2 cycle     5.0      D
	mov	temp, oneReg 		;Blue      1 cycle     5.125    D
	out	PORTB, zeroReg		;Blue4L    1 cycle     5.25     D
	nop				;Blue      1 cycle     5.375    L
	nop				;Blue      1 cycle     5.50     L

	out	PORTB, oneReg 		;Blue3H    1 cycle     5.6250   L
	mov	temp2, zeroReg		;Blue      1 cycle     5.75     H
	out	PORTB, temp		;Blue3D    1 cycle     5.8750
	nop				;Blue      1 cycle     6.00     L
	nop				;Blue      1 cycle     6.125    L
	sbrc	LEDvalueReg, 2		;Blue    1_2 cycle     6.25     D
	mov	temp2, oneReg  		;Blue      1 cycle     6.375    D
	out	PORTB, zeroReg		;Blue3L    1 cycle     6.50     D
	nop				;Blue      1 cycle     6.625    L
	nop				;Blue      1 cycle     6.750    L

	out	PORTB, oneReg 		;Blue2H    1 cycle     6.8750   L
	mov	temp , zeroReg		;Blue      1 cycle     7.000    H
	out	PORTB, temp2 		;Blue2D    1 cycle     7.125    D
	nop				;Blue      1 cycle     7.250    D
	nop				;Blue      1 cycle     7.375    D
	sbrc	LEDvalueReg, 1		;Blue    1_2 cycle     7.500    D
	mov	temp, oneReg		;Blue      1 cycle     7.750    D
	out	PORTB, zeroReG		;Blue2L    1 cycle     7.875    D
	nop				;Blue      1 cycle     8.000    L
	nop				;Blue      1 cycle     8.125

	out	PORTB, oneReg		;Blue1H    1 cycle     8.250    H
	mov	temp2, zeroReg		;Blue      1 cycle     8.375    H
	out	PORTB, temp		;Blue1D    1 cycle     8.500    D
	nop				;Blue      1 cycle     8.625    D
	nop				;Blue      1 cycle     8.875    D
	sbrc	LEDvalueReg, 0		;Blue    1_2 cycle     9.000    L
	mov	temp2, oneReg		;Blue      1 cycle     9.125    L
	out	PORTB, zeroReg 		;Blue1L    1 cycle     9.250
	mov	LEDvalueReg, GreenIntensityReg	;Blue      2 cycle     9.375    L

	out	PORTB, oneReg		;Blue0H    1 cycle     9.500    L
	mov	temp, zeroReg		;Blue      1 cycle     9.625    H
	out	PORTB, temp2		;Blue0D    1 cycle     9.750    H
	sbrc	LEDvalueReg, 7		;Blue    1_2 cycle     9.875    D
	mov	temp, oneReg		;Blue      1 cycle    10.000    D
	out	PORTB, zeroReg		;Blue0L    1 cycle    10.125    D
	sei
	ret
;~~~~~~~~~~~~~~~~~~~~~ end of LightOneNeoPixel ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



;*********************************************************************************                                                                                                                                                                                                         
;   doADC   - set up, run analog-to-digital convertor, then return high bits (9-2) of result                                                                                                                                                                                               
;                                                                                                                                                                                                                                                                                          ;***************************************************************************
;   send:	The ADC channel in Temp register bits 1 and 0                                                                                                                                                                                                                                ;   Timer 1 overflows and is reloaded
;   return:	left aligned upper 8 bits of result in ADCstore register                                                                                                                                                                                                                     ; heartBeat Low increments 100 times per second.
;**********************************************************************************                                                                                                                                                                                                          ; heartBeat Medium byte increments every 2.56 seconds
doADC:	; Tiny25 version                                                                                                                                                                                                                                                                     ; hb High increments every 655.36 sec[10.92 min]
	andi	temp, 0b00000011 ; preserve MUX bits                                                                                                                                                                                                                                         ; all turnover to zero after: 46.6 hrs
	ori	temp, (1<<ADLAR); return bits 9-2 in ADCH; REFS0 cleared for VCC reference                                                                                                                                                                                                   ;  an 8-bit variable called heartBeat_Count is also incremented.
	out	ADMUX, temp; VCC is reference; ADCh=bits 9-2                                                                                                                                                                                                                               T1CntOvflw:
; ADEN = ADC enable;  ADIE = irq enable; ADPS2 set=clock/32 for ADCclk for 200K ADC clock 8MHz sysclk                                                                                                                                                                                      	push	temp
 	ldi	temp, (1 << ADEN) | (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0)                                                                                                                                                                                                              	in	SREGtemp, SREG
	out	ADCSRA, temp                                                                                                                                                                                                                                                               	ldi	irqtemp, Timer1Reload  ; 0xdf
	sbi	ADCSRA, ADSC ; start conversion                                                                                                                                                                                                                                            	out	TCNT1, irqtemp
doADC0:	sbic	ADCSRA, ADSC; set means ADC conversion still in progress                                                                                                                                                                                                                   
	rjmp	doADC0                                                                                                                                                                                                                                                                     	lds	irqtemp, heartBeat_Count
	in	ADCstore, ADCH                                                                                                                                                                                                                                                             	inc	irqtemp
	ret                                                                                                                                                                                                                                                                                	sts	heartBeat_Count, irqtemp
                                                                                                                                                                                                                                                                                           
    
                                                                                                            
;***************************************************************************                                
  ;   Timer 1 overflows and is reloaded                                                                     
  ; heartBeat Low increments 100 times per second.                                                          
  ; heartBeat Medium byte increments every 2.56 seconds                                                     
  ; hb High increments every 655.36 sec[10.92 min]                                                          
  ; all turnover to zero after: 46.6 hrs                                                                    
  ;  an 8-bit variable called heartBeat_Count is also incremented.                                          
T1CntOvflw:                                                                                                 
	push	temp                                                                                        
 	in	SREGtemp, SREG                                                                              
	ldi	irqtemp, Timer1Reload  ; 0xdf                                                               
	out	TCNT1, irqtemp                                                                              
                                                                                                            
	lds	irqtemp, heartBeat_Count                                                                    
	inc	irqtemp                                                                                     
	sts	heartBeat_Count, irqtemp                                                                    
                                                                                                            
	ldi	irqtemp, 1                                                                                  
	lds	temp, heartBeat_Low                                                                         
	add	temp, irqtemp                                                                               
	sts	heartBeat_Low, temp                                                                         
	ldi	irqtemp, 0                                                                                  
	lds	temp, heartBeat_Med                                                                         
	adc	temp, irqtemp                                                                               
	sts	heartBeat_Med, temp                                                                         
	lds	temp, heartBeat_High                                                                        
	adc	temp, irqtemp                                                                               
	sts	heartBeat_High, temp                                                                        
                                                                                                            
	pop	temp                                                                                        
	reti                                                                                                
                                                                                                            
  
    
    


;===========================================================================

.db	"**Neo.asm  ver 1.4    Oct 16, 2017"





;;*****************************************************************************
;;
;;  Main Loop for Timer1 Overflow blinker
;;
;;*****************************************************************************
;
;Main:
;	lds	temp, heartBeat
;	cpi	temp, 100        ; one second
;	brge	Mn1
;
;;  other code for main() goes here
;
;       	rjmp	Main
;
;
;;  toggle LED on pin 3 [PB4] every second using Timer 1 overflow
;Mn1:	clr	temp
;	sts	heartBeat, temp
;	in	temp, portb
;	eor	temp, togglePB4maskReg
;	out	portb, temp
;
;	rcall	doADC
;
;	rjmp	Main
;
;
;
;; see 11.9.3 pdf page 79 for Timer registers
; ; see 11.7.2 pdf page 79 for Clear_Timer_On_Compare_Match  WGM2:0=010

;*************************************
;	Interrupt Routines
;*************************************
;
;;**************************************************************************
;;  Timer Zero Overflow interrupt routine happens every 104 micro seconds.
;;  This is the 9600 baud period for the MP3 DFplayer module board.
;;  PinOut on Tiny25: TIMER0_RELOAD,  txPort:txpin ;  PINB:txpin_monitor 
;;  dseg:  SoftTXOutData,	SoftTXControlReg
;;  STxCR {Out_Active=bit7} set to 0x1xxx1010 by main code to transmit 
;;**************************************************************************
;T0CntOvflw:  ; every baud 104 microseconds  9600/second
;	in	SREGtemp, SREG			;1
;	ldi	irqtemp, TIMER0_RELOAD	;2   151
;	out	TCNT0, irqtemp			;1
;;	sbi	PINB, txpin_monitor			;0    debugging toggle
; 	sbrc	SoftTXControlReg, Out_Active	;1/2  check for processing an output
;	rjmp	SoftUART					;2    yes, jump to output handler
;T0exit:	out	SREG, SREGtemp		;1    -- 15 cycles/interrupt
;	reti							;4    -- @38 cycles/int  during Tx
;SoftUART:
;	mov	irqtemp, SoftTXControlReg 	;1
;	andi	irqtemp, 0b00001111		;1    separate the bitcount from the
;	breq	TXoutByteFinished		;1/2  Tx is completely finished, so
;	cpi	irqtemp, 10			;1    Tx is at beginning, so make a
;	breq	MakeTxZeroBit			;1/2
;	cpi	irqtemp, 1				;1    Tx  complete, do STOP bit
;	breq	MakeTxOneBit			;1/2
;	lsr	SoftTXOutData			;1    least sig bit sent first
;	brcc	MakeTxZeroBit			;1/2
;MakeTxOneBit:
;	sbi	txPort, txpin 			;2
;	rjmp	DoOutBit1 			;2
;MakeTxZeroBit :
;	cbi	txPort, txpin			;2
;DoOutBit1:
;	dec	irqtemp				;1
;	andi	SoftTXControlReg, 0b11110000	;1
;	or	SoftTXControlReg, irqtemp 	;1
;	rjmp	T0exit				;2
;TXoutByteFinished:
;	ldi	SoftTXControlReg, 0x0a	;1    set up new softTx bit count
;	rjmp	T0exit   				;2
;;                                                                                  