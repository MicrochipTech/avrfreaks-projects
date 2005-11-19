
.include "8515def.inc"

;***************************************************************************
;* Interrupt vectors
;********************	
.cseg
.org 0
	rjmp	Main
;.	INT0addr=$001	External Interrupt0 Vector Address
		reti	; External 0 interrupt  Vector 
;.	INT1addr=$002	;External Interrupt1 Vector Address
		reti			; External 1 interrupt  Vector 
;.	ICP1addr=$003	;Input Capture1 Interrupt Vector Address
		reti
;.	OC1Aaddr=$004	;Output Compare1A Interrupt Vector Address
		reti
;.	OVF1addr=$005	;Overflow1 Interrupt Vector Address
		reti
;.	OVF0addr=$006	;Overflow0 Interrupt Vector Address
		reti
;.	 =$007
;reti
		rjmp HandleT0	; Timer 0 Overflow  Vector 
		
;.	URXCaddr=$008	;UART Receive Complete Interrupt Vector Address
		reti    ; vfd display routine
;.	UDREaddr=$009	;UART Data Register Empty Interrupt Vector Address
		reti
;.	UTXCaddr=$00a	;UART Transmit Complete Interrupt Vector Address
		reti
;.	ADCCaddr =$00b	;ADC Interrupt Vector Address
		reti
;.	ERDYaddr =$00c	;EEPROM Interrupt Vector Address
		reti
;.	ACIaddr  =$00d	;Analog Comparator Interrupt Vector Address
		reti			; Analogue Comparator  Vector 

.def SSREG = r13

.def TL0   = r16 ; Timer 0 reload value
.def idx   = r17 ; for short loop counters
.def idy   = r18 ; for short loop counters
.def CELL  = r19 
.def FRAME = r20
.def TP0   = r21  ;transiant pointer bank 0
.def ARGL  = r22 ; to OS, usually a pointer
.def ARGH  = r23 ; passed arguments
.def ACC   = r24
.def BCC   = r25

; timer 0 interrupt handler, 
; this is the main workhorse of the LCD updater
; Routine is based on EDN design ideas pp 76 20030109
; Here the cells are updated in 7 phases
; r16 holds the phase count FRAME
; r17 holds the cell pointer CELL

; Z register points to next phase to send.
; r0 is used by the lpm instruction
; r12 is used as a scratch area

HandleT0:
	out TCNT0,TL0
	in SSREG,SREG

	; Frame is a bitfield  7  6  5  4  3  2  1  0
	;                      R:ov:BH:BL:ov:PH:PM:PL
	; When bit 5 overflows we are at the start of a Frame
	; Bits 6 and 7 are reserved for future use.
	inc  FRAME
	sbrs FRAME,3
	rjmp T000
	andi FRAME,0xF0 ; reset overflow, save rfu bits
	
;	swap FRAME
;	inc  FRAME
;	swap FRAME
;	sbrs FRAME,6
;	rjmp T000
;	andi FRAME,0x8F ;mask bits
	;ori  FRAME,0x10 ;reset counter (there are 3 fields in this test 4 - 3 = 1
;	ori FRAME,0x30 ; temp for test

	; CELL is a low memory pointer that is used to offset in ram
	; to load the  table. It is a bitfeild
	; 7  6  5  4  3  2  1  0
	; R: R: R:ov:CH:C2:C1:CL
	inc CELL
;	sbrs CELL,4
;	rjmp T000	
;	andi CELL,0xE0
	;use inc logic, there are only 9 chars on first test LCD
	;The bit field can hold 16 bits, So we set the counter to 16 - 9
	;or 7. This will case bit 5 to overlow after 9 clocks
	;ori CELL,0x07
	; for the first test we only have the first 3 cells connected so
	; 16 - 3 = 13
   ;ori CELL,0x0D

;	ori CELL, 0x0F ; temp for test
	; Cell now points to memory value

	;
	ldi YH,High(RTABLE)	
	ldi YL,low(RTABLE)	

	; Get Z base
	ldi ZL,low(SYM0 * 2) 
	ldi ZH,high(SYM0 * 2)

T000: ; continue processing;
	lpm  ; next pattern
	out PORTD,r0
	adiw ZL,1
	lpm
	out DDRD,r0
	adiw ZL,1
	lpm 
	out PORTD,r0
	adiw ZL,2
	;lpm               ; keep data aligned

	ld ACC,Y+
	out PORTB,ACC
	ld ACC,Y+
	out PORTC,ACC
	ld ACC,Y+
	out PORTA,ACC
	ld ACC,Y+        ; shift byte

	out SREG,SSREG
	reti

.org 0x0080
Main:

	CLI
	ldi	ACC,0x00	;Disable all ints. (3-26)
	out	GIMSK,ACC	;

    ldi    ACC, high(RAMEND)
    out    SPH, ACC
    ldi    ACC, low(RAMEND)
    out    SPL, ACC

    WDR
	ldi	ACC,0x1F	
	out	WDTCR,ACC	;load WDTCR with value
	ldi	ACC,0x17	;00010111 Wrute 0 to WDTCR.3 to disable WDT
	out	WDTCR,ACC	;
	
	;port A setup

	ldi	ACC,0xFF	
	out	DDRA,ACC	; Set port A data direction 
    ldi   ACC,0x00
	out	PORTA,ACC 	

	;port B setup

	ldi	ACC,0xFF	
	out	DDRB,ACC	; Set port B data direction 
    ldi   ACC,0x00
	out	PORTB,ACC 	

	;port C setup
	ldi ACC,0xFF 
	out   DDRC,ACC ; Set port C data direction 
	ldi	TP0,0x00   
	out	PORTC,TP0	

	;Port D setup
	ldi	TP0,0xF2	; 999ccctr
	out	DDRD,TP0	; Set port D data direction
	ldi	TP0,0x00   
	out	PORTD,TP0	
	
; set all SRAM high.

   ;safe harbor all registers
   ldi r16,0x00 ;TINT TIMER
   mov r0,r16   ;Bank0 X/ lpm
   mov r1,r16   ;Bank0 Y
   mov r2,r16   ;task time
   mov r3,r16   ;R0@ bank1 code access
   mov r4,r16   ;ZL Bank1 code access
   mov r5,r16   ;ZH Bank1 code access
   mov r6,r16   ;IR local
   mov r7,r16   ;VF data
   mov r8,r16   ;SIGS     ; bit signals
   mov r9,r16   ;SIGW     ; waiting flags
   mov r10,r16  ;Bank1 X  ; points to stack table
   mov r11,r16  ;Bank1 Y  ; address of task in READYQ for task
   mov r12,r16  ;temp@interrupt
   mov r13,r16  ;ssreg
   mov r14,r16  ;newsc
   mov r15,r16  ;newsc
   
   ldi r17,0x00 ;passed arguments
   ldi r18,0x00 ;interrupt scratch
   ldi r19,0x00 ;timer reload count TL0
   ldi r20,0x00 ;ACC scratch
   ldi r21,0x00 ;temp for keyscanner msgQ ptr
   ldi r22,0x00 ;TP0
   ldi r23,0x00 ; task flags 0x80 used by timer
   ldi r24,0x00 ;RUNNING
   ldi r25,0x00 ;READYQTOP
   ldi r26,0x00 ;XL
   ldi r27,0x00 ;XH
   ldi r28,0x00 ;YL
   ldi r29,0x00 ;YH
   ldi r30,0x00 ;ZL
   ldi r31,0x00 ;ZH


	ldi ACC,0xFF 
	ldi XH,0
   ldi XL,low(RAMEND)
   
clrBase:
   st X,ACC
   dec R26
   cpi R26,0x60
   brsh clrBase

   
;====================================================================
;This section allows ints which are apparently 
;caused by reset, to occur harmlessly.
;Databook 3-21, 3-26

	ldi	ACC,0x00	;
	out	GIMSK,ACC	;Enable int 0
	
	ldi	ACC,0x00	;
	out	TIMSK,ACC	;Disable  all timer ints
	
	SEI			;Global int enable
	nop
	nop
	nop	;Let the ints happen, so they
	nop	;clear out
	nop
	nop
	CLI	;Turn off all ints (3-21) Clear SREG.7

;
;*******************************************************************
;Set EEProm Inactive (3-41)
;
	ldi	ACC,0x00	;00000000
 	out	EECR,ACC	;
;
;*******************************************************************
;

;set-up interrupt timer and inital phases

	ldi FRAME,0b00110111 ; set so will overflow on start
	ldi CELL, 0


	; copy bitpattern to ram

	ldi ZL,low(SYM1 * 2) 
	ldi ZH,high(SYM1 * 2)


	ldi YH,High(RTABLE)	
	ldi YL,low(RTABLE)	

	ldi idx,32

LOADRAMTABLE:
	lpm
	adiw ZL,1
	st Y+,r0
	dec idx
	brne LOADRAMTABLE

	ldi ACC,0x04 ; div 64 == 64uS
	out TCCR0,ACC

	ldi TL0,0x64  ;100Hz ?

	out TCNT0,TL0

	ldi ACC,0x02
	out TIMSK,ACC ;enable timer
	SEI           ;Start system

Nothing:
	WDR
	sbrs CELL,4
 	rjmp Nothing
	clr CELL
	ldi XH,High(RTABLE)
	ldi XL,low(RTABLE)

	ldi  idy,8
PHASESHIFT8:

	ldi  idx,3
PHASESHIFT3:
	ld TP0,X ; portb
	      
	adiw XL,4 ; start froom the end

	rol TP0  ; get cary from port b

	ld ACC,-X ; shift
	rol ACC
	st X,ACC

	ld ACC,-X ; port a
	rol ACC
	st X,ACC

	ld ACC,-X ; port c
	rol ACC
	st X,ACC

	ld ACC,-X ; port b
	rol ACC
	st X,ACC

	dec idx
	brne PHASESHIFT3

	; adjust the shift
	adiw XL,3 ; start froom the end
	ld ACC,X ; port b
	
	ror ACC  ; right shift by three the blank digit
	ror TP0
	ror ACC
	ror TP0
	ror ACC
	ror TP0

	ror ACC ; shift out the two null bits
	ror ACC
	
	rol TP0 ; shift the three bits back  into the shift variable
	rol ACC
	rol TP0  
	rol ACC
	rol TP0  
	rol ACC

	lsl ACC
	lsl ACC

	st X+,ACC ;save  it back and prepare to loop

	dec idy
	brne PHASESHIFT8

 	rjmp Nothing

SYM0:
;   portd      ddrd       portd      
;     999ccc     999ccc     999ccc     
;phase 0 
.db 0b11110000,0b11110010,0b11110000,0
;phase 1 ; 
.db 0b11110100,0b11100110,0b11100100,0
;phase 2
.db 0b11101100,0b11101010,0b11101000,0
;phase 3
.db 0b11101000,0b11100010,0b11100000,0
;phase 4
.db 0b00000000,0b11110010,0b00000000,0
;phase 5
.db 0b00000000,0b11100110,0b00000000,0
;phase 6
.db 0b00000000,0b11101010,0b00000000,0
;phase 7
.db 0b00000000,0b11100010,0b00000000,0

SYM1:
;   port b      port c     port a    shift
;     11122233   34445556   66777888   999AAAXX
;phase 0 
.db 0b10011110,0b11011111,0b01101111,0b10111111
;phase 1 ; 
.db 0b01011000,0b11001001,0b00000110,0b00010011
;phase 2
.db 0b00011010,0b01000100,0b01001100,0b00000011
;phase 3
.db 0b11111111,0b11111111,0b11111111,0b11111111
;phase 4
.db 0b01100001,0b00100000,0b10010000,0b01000000
;phase 5
.db 0b10100111,0b00110110,0b11111001,0b11101100
;phase 6
.db 0b11100101,0b10111011,0b10110011,0b11111100
;phase 7
.db 0b00000000,0b00000000,0b00000000,0b00000000

DIGIBIT:

; zero
;   01234567
.db 0b10010110,0b011 ; fe
.db 0b01011010,0b101 ; ad
.db 0b00011110,0b111 ; pbc
; one
.db 0b11110000,0b000 ;
.db 0b11110000,0b000 ;
.db 0b10010110,0b011 ; bc
; two
.db 0b10110100,0b010 ; e
.db 0b00011110,0b111 ; agd
.db 0b11010010,0b001 ; b
;three
.db 0b11110000,0b000 ;
.db 0b00001111,0b111 ; agd
.db 0b10010110,0b011 ; bc
;four
.db 0b11010010,0b001 ; f
.db 0b10110100,0b010 ; g
.db 0b10010110,0b011 ; bc
;five
.db 0b11010010,0b001 ; f
.db 0b00001111,0b111 ; agd
.db 0b10110100,0b010 ; c
;six
.db 0b10010110,0b011 ; fe
.db 0b00011110,0b111 ; agd
.db 0b10110100,0b010 ; c
;seven
.db 0b11110000,0b000 ;
.db 0b11010010,0b001 ; a
.db 0b10010110,0b011 ; bc
;eight
.db 0b10010110,0b011 ; fe
.db 0b00011110,0b111 ; agd
.db 0b10010110,0b011 ; bc
;nine
.db 0b11010010,0b001 ; f
.db 0b10010110,0b011 ; ag
.db 0b10010110,0b011 ; bc

.dseg 
RTABLE: .byte 32