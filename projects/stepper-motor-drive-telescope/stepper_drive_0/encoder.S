; File: encoder.S

; Drive box firmware for ATMega168
;

#include <avr/io.h>

.equ	DECLplus, 	0	; bit numbers used in encoder routines
.equ	RAplus, 	1
.equ	DECLzero,	2
.equ	RAzero,		3
.equ	DECLminus,	4
.equ	RAminus,	5

.equ	DIV1_LO, 48		; TIMER1 divisor OCR1A = 7216 ( =7811 for 1 pulse per 0.499968 sec, 16MHz)
.equ	DIV1_HI, 28		; f = 16MHz/1024/(1+OCR1A)  = 2.165027Hz

.equ	DIV2, 225		; Default TIMER2 divisor for approx sidereal rate 
;ideal(32usteps)=138.560Hz; 225 gives 138.274Hz (16MHz), 224 gives 138.888Hz
;ideal(64usteps)=277.12Hz; 225 gives 276.549Hz, 224 gives 277.777Hz

; Macro: ASCII code for decimal value
.macro decval value
	ldi		r16, lo8(\value)
	ldi		r17, hi8(\value)
	rcall 	decade
	subi	r20, -48
.endm

; Macro: Load immediate Y register with 16-bit data segment address
.macro	ldiy16 address
	ldi YL, lo8(\address)
	ldi YH, hi8(\address)
.endm

; Macro: Load immediate Z register with 16-bit data segment address
.macro ldiz16 address
	ldi ZL, lo8(\address)
	ldi ZH, hi8(\address)
.endm

; Macro: Compare immediate 16-bit constant with 16-bit value in Y-register
.macro	cpiy16 address
	ldi ZL, lo8(\address)
	ldi ZH, hi8(\address)
	cp  ZL, YL				; compare low byte
	cpc ZH, YH				; compare high byte
.endm

.global READ_encoders

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Read Hengstler encoders (model R158-D/5000EF.47KF-F0)
;
; Port C:
;
; Pins 1,3,5,7 from RA encoder (1)
; Pins 0,2,4,6 from Dec encoder (2)
;
;    7      6      5      4       3       2       1       0
;  -----------------------------------------------------
; |  X  |  X  | A1 | A2 | N1 | N2 | B1 | B2 |
;  -----------------------------------------------------
;
; (This is to enable use of SWAP nibbles when determining direction)
;
;	Sequence for +ve rotation of the encoders is
;
;						A		B
;					  -------------
;						0		0   
;						1		0
;						1       	1
;						0		1
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

READ_encoders:

; Read port pins
	in  r16,(PINC-32)		; using in r16,PINC gives wrong address ($26)
	sts new_data, r16

	lds  r16,prev_data
	andi r16,0b00110011		; get prev A1, A2, B1, B2 
	
	lds  r17,new_data
	andi r17,0b00110011		; get new A1, A2, B1, B2 

	swap r17				
	eor  r16,r17			; bits 4,5 will be set for one direction,
							; bits 0,1 will be set for opposite direction
    	mov  r18,r16			; check that bits 0,4 or 1,5 are not equal
	swap r18
	cp   r16,r18
	breq CheckN			; skip if they are - no change in position

; Update current position
	sbrc  r16,RAplus
	adiw  r24,1			; increment RA
	sbrc  r16,RAminus	
	sbiw  r24,1				; decrement RA
	sbrc  r16,DECLplus
	adiw  r26,1			; increment Declination
	sbrc  r16,DECLminus
	sbiw  r26,1				; decrement Declination

; Check for North marks
CheckN:
	lds	r17,new_data
	sbrc 	r17,RAzero
	rcall	ZERORA			; reset RA counter
	sbrc 	r17,DECLzero
	rcall	ZERODEC			; reset Declination counter

; Save new data as previous
	sts	prev_data,r17

	ret
	 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Clear RA or Declination registers
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

ZERORA:
	clr r24
	clr r25
	ret

ZERODEC:
	clr r26  
	clr r27
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; USART intialisation routine ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global USART_Init

USART_Init:

; Set baud rate to 9600 (see Table 17-11, 17-12 of ATMega88 datasheet)
; UBRR0 should be:	103 for 16MHz		(0.2%)
;                   	51 for  8MHz
;			25 for  4MHz
;			12 for  2MHz
; 			6 for  1MHz
	ldi r16, 103
	clr r17
	sts UBRR0L, r16
	sts UBRR0H, r17

; Enable Receiver and Transmitter, enable Rx, Tx complete interrupts
	ldi r16, (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<TXCIE0);
	sts UCSR0B, r16

; Set frame format: 8 data, 1 stop bit, asynchronous mode
	ldi r16, (3<<UCSZ00);
	sts UCSR0C, r16
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; USART transmit routine.
; The function simply waits for the transmit buffer to be empty 
; by checking the UDRE Flag, before loading it with new data 
; to be transmitted. If the data register empty interrupt
; is utilized, the interrupt routine writes the data into the buffer.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global USART_Transmit

USART_Transmit:

; Wait for empty transmit buffer
	lds  r17, UCSR0A
	sbrs r17, UDRE0
	rjmp USART_Transmit

; Put data (r16) into buffer, send the data
	sts UDR0, r16
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; USART RXC Interrupt routine ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Uses r16,r17,r18,r19,r20,ZL,ZH
;

.global USART_RX_vect

USART_RX_vect:
	lds  r21,SREG		; save status register
	push r21
	push r20
	push r19
	push r18
	push r17
	push r16
	push ZL
	push ZH

	lds  r16,UDR0
UP: 
	cpi r16,'P'	        		; "P" ($50)  Return PEC index and zero offset
    	brne UQ
	rcall UPDATE_PECptr
	rjmp UT

UQ:
	cpi  r16,'Q'				; "Q" ($51)  Return RA and Dec encoder counts
	brne UB
	rcall UPDATE_RADec	

UT:
	ldiy16 string+14			; load Y-reg with address of string
	ld  r16,-Y					; decrement Y reg, load first character 
	rcall USART_Transmit 	; send first character
	rjmp UX
	
UB:
	cpi r16,'B'	        		; "B"  Set PEC pointer 
    	brne UZ
U7:	lds  r17,UCSR0A
	sbrs r17,RXC0			; wait for first byte of number
	rjmp U7
	lds  r18,UDR0			; read byte
U8:	lds  r17,UCSR0A
   	sbrs r17,RXC0			; wait for second byte
	rjmp U8
	lds  r19,UDR0			; read byte
	ldi ZL,pm_lo8(PEC_Table)	; load Z reg with byte address of table (in PM)
	ldi ZH,pm_hi8(PEC_Table)	; 
	lsl ZL					; double because PM
	rol ZH
	add r18,ZL				; add to get pointer
	adc r19,ZH
	sts  PEC_ptr, r18		; save
	sts  PEC_ptr+1, r19
	rjmp UX

UZ:
	cpi r16,'Z'	        		; "Z" ($5A)  Set zero offset into PEC table
    	brne Upos
U1:	lds  r17,UCSR0A
	sbrs r17,RXC0			;wait for first byte of number
	rjmp U1
	lds  r16,UDR0			; read byte
	sts  ptrzero, r16
U2:	lds  r17,UCSR0A
   	sbrs r17,RXC0			; wait for second byte
	rjmp U2
	lds  r16,UDR0			; read byte
	sts  ptrzero+1, r16
	rjmp UX

Upos:
	cpi r16,'+'	        		; "+" ($2B)  Increase RA rate 
    	brne Uneg
U3:	lds  r17,UCSR0A
	sbrs r17,RXC0			; wait for byte
	rjmp U3
	lds  r13,UDR0			; set guiding counter
	ldi  r16,-4				; set divisor offset = -4 (rate +1.8%)
	mov  r14,r16 
	rjmp UX

Uneg:
	cpi  r16,'-'	        		; "-" ($2D)  Decrease RA rate
    	brne Uposdec
U4:	lds  r17,UCSR0A
	sbrs r17,RXC0			; wait for byte
	rjmp U4
	lds  r13,UDR0			; set guiding counter
	ldi  r16,4				; set divisor offset = +4 (rate -1.8%)
	mov  r14,r16 
	rjmp UX

Uposdec:
	cpi  r16,'>'	        		; ">" Move north
    	brne Unegdec
U5:	lds  r17,UCSR0A
	sbrs r17,RXC0			; wait for byte (no. of usteps)
	rjmp U5
    	lds r18,UDR0
	lds  r17,drive_flags
	sbrs r17,4				; bit 4 = 0 to suppress Dec guiding
	rjmp UX
	tst r18
	breq UN 				; skip if zero movement in Dec
	clr r19
	clr r20
	clr r21
	ldi r16, 128	
	rjmp UM_Dec
	
Unegdec:
	cpi  r16,'<'	        		; "<" Move south 
    	brne Un
U6:	lds  r17,UCSR0A
	sbrs r17,RXC0			; wait for byte (no. of usteps)
	rjmp U6
    	lds r18,UDR0
	lds r17,drive_flags
	sbrs r17,4				; bit 4 = 0 to suppress Dec guiding
	rjmp UX
	tst r18
	breq Un 				; skip if zero movement in Dec
	clr r19
	clr r20
	clr r21
	clr r16	
	rjmp UM_Dec
	
Un: 	
	cpi r16, 'n'			; Move 5 arcmin north (rate/4)
    	brne Us
    	ldi r18, 203
	ldi r19, 10
	ldi r20, 0
	ldi r21, 0
	ldi r16, 128
	ldi r17,(1<<CS02)|(1<<CS00)	; TIMER0 prescaler = 1024
	sts TCCR0B,r17	
	rjmp UM_Dec

Us:
    	cpi r16, 's'				; Move 5 arcmin south (rate/4)
	brne UN		
    	ldi r18, 203
	ldi r19, 10
	ldi r20, 0
	ldi r21, 0
	ldi r16, 0
	ldi r17,(1<<CS02)|(1<<CS00)	; TIMER0 prescaler = 1024
	sts TCCR0B,r17	
	rjmp UM_Dec

UN:
    	cpi r16, 'N'			
	brne US				; Move 5 arcmin north
   	ldi r18, 203
	ldi r19, 10
	ldi r20, 0
	ldi r21, 0
	ldi r16, 128
	rjmp UM_Dec
	
US:
    	cpi r16, 'S'			
	brne UE				; Move 5 arcmin south
    	ldi r18, 203
	ldi r19, 10
	ldi r20, 0
	ldi r21, 0
	ldi r16, 0
	rjmp UM_Dec

UE:
    	cpi r16, 'E'			
	brne UW				; Move 5 arcmin east
    	ldi r18, 151
	ldi r19, 21
	ldi r20, 0
	ldi r21, 0
	ldi r16, 0
	push YL
	push YH
	lds YL,PEC_ptr			; get pointer into PEC table
	lds YH,PEC_ptr+1
	sbiw YL, 41				; subtract 43 bins - 2 for time taken
	ldi ZL, pm_lo8(PEC_Table)	; address of start of table		
	ldi ZH, pm_hi8(PEC_Table)	
	lsl ZL				; double because PM
	rol ZH		
	cp  YL, ZL			; compare 
	cpc YH, ZH			
	brpl UE_1			; jump if not before start of table
	adiw YL, 6			; add 6
	subi YH, -2			; add 512
UE_1:				
    	sts PEC_ptr,YL			; save new PEC pointer 
	sts PEC_ptr+1,YH
	pop YH
	pop YL
	rjmp UM_RA
	
UW:
    	cpi r16, 'W'			
	brne UM				; Move 5 arcmin west
    	ldi r18, 151
	ldi r19, 21
	ldi r20, 0
	ldi r21, 0
	ldi r16, 128
	push YL
	push YH
	lds YL,PEC_ptr			; get pointer into PEC table
	lds YH,PEC_ptr+1
	adiw YL, 45			; add 43 bins + 2 for time taken
	ldi ZL, pm_lo8(PEC_Table+518)	; address of end of PEC table		
	ldi ZH, pm_hi8(PEC_Table+518)	
	lsl ZL				; double because PM
	rol ZH		
	cp YL, ZL			; compare 
	cpc YH, ZH			
	brmi UW_1			; jump if not beyond end of table
	sbiw YL, 6			; subtract 6
	subi YH, 2			; subtract 512
UW_1:				
    	sts PEC_ptr,YL		; save new PEC pointer 
	sts PEC_ptr+1,YH
	pop YH
	pop YL
	rjmp UM_RA
	
UM:
    	cpi r16, 'M'			
	brne UC				; Move specified distance in RA or Dec
UM1:
	lds r17,UCSR0A
	sbrs r17,RXC0			; wait for first byte of unsigned 32-bit number
	rjmp UM1
	lds r18,UDR0			; read byte 1
UM2:	lds r17,UCSR0A
    	sbrs r17,RXC0			; wait for second byte
	rjmp UM2
	lds r19,UDR0			; read byte 2
UM3:	lds r17,UCSR0A
	sbrs r17,RXC0			; wait for third byte
	rjmp UM3
	lds r20,UDR0			; read byte 3
UM4:	lds r17,UCSR0A
    	sbrs r17,RXC0			; wait for fourth byte
	rjmp UM4
	lds r21,UDR0			; read byte 4
UM5:	lds r17,UCSR0A
    	sbrs r17,RXC0			; wait for sign/axis byte
	rjmp UM5
	lds r16,UDR0			; read sign/axis byte
	sbrc r16, 1				; bit 1 == 0 for RA; bit 1 == 1 for Dec
	rjmp UM_Dec		
UM_RA:					; moving in RA
	sts RA_STEP_max, r18
	sts RA_STEP_max+1, r19
	sts RA_STEP_max+2, r20
	sts RA_STEP_max+3, r21
	cbi (PORTB-32),PB4		; set DIR LO, positive RA
	sbrc r16, 7
	sbi (PORTB-32),PB4		; set DIR HI, negative RA
	rcall RA_SLEW			; initiate slew in RA
	rjmp UX
UM_Dec:					; moving in Dec
	sts DEC_STEP_max, r18
	sts DEC_STEP_max+1, r19
	sts DEC_STEP_max+2, r20
	sts DEC_STEP_max+3, r21
	cbi (PORTD-32),PD4		; set DIR LO, positive Dec
	sbrc r16, 7
	sbi (PORTD-32),PD4		; set DIR HI, negative Dec
	rcall DEC_SLEW			; initiate slew in Dec
	rjmp UX
	
UC:
    	cpi r16, 'C'			
	brne UD			;  Drive correction flag
UC1:
	lds r17,UCSR0A
	sbrs r17,RXC0		; wait for byte
	rjmp UC1
	lds	r16,UDR0	; set drive correction flags
	andi r16,7		; mask off bits 0, 1, 2
	sts drive_flags,r16
	rjmp UX

UD:
    	cpi r16, 'D'			
	brne UX			;  One step in Dec
	rcall PULSE	
	
UX:
    	pop ZH
	pop ZL
	pop r16
	pop r17
	pop r18
	pop r19
	pop r20
	pop r21
	sts SREG,r21		; restore status register
	reti
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; USART TXC Interrupt routine
; To avoid the processor being tied up waiting for the USART
; to send a character.
; NOTE: Y-reg used as dedicated pointer!
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global USART_TX_vect

USART_TX_vect:
	lds	r21,SREG		; save status register
	push r21
	push ZH
	push ZL
	push r17
	push r16

	ld  r16,-Y			; decrement Y reg, load character pointed to
	cpiy16 string-1		; reached end of string?
	breq U9				; jump if true
	rcall USART_Transmit ; send next character

U9:	pop r16
	pop r17
	pop ZL
	pop ZH
	pop r21
	sts SREG,r21		; restore status register
	reti
	
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Update ASCII string containing RA and Dec 
; 		(Trevor Blake's method - 8 times faster!)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Uses r16,r17,r18,r19,r20

UPDATE_RADec:
	ldiz16 string+14	; load Z reg with address of string (data segment)
	mov r18,r24		; load r18,r19 with RA encoder count
	mov r19,r25
	rcall INTDEC		; save RA count in string
	ldi r16, 9		; ASCII TAB
	st  -Z,r16
	mov r18,r26		; load r18,r19 with Dec encoder count
	mov r19,r27	
	rcall INTDEC		; save Dec count in string
	ldi r16, 13		; ASCII CR
	st  -Z,r16
	ret

.global UPDATE_PECptr

UPDATE_PECptr:
	lds r18,PEC_ptr		; load r18,r19 with current PEC pointer (data segment)
	lds r19,PEC_ptr+1
	ldi ZL,pm_lo8(PEC_Table)	; load Z reg with byte address of table (in PM)
	ldi ZH,pm_hi8(PEC_Table)	; 
	lsl ZL				; double because PM
	rol ZH
	sub r18,ZL			; subtract to get bin index
	sbc r19,ZH
	ldiz16 string+14	; load Z reg with address of string
	rcall INTDEC		; save PEC pointer in string
	ldi r16,9		; ASCII TAB
	st  -Z,r16			
	lds r18,ptrzero		; load r18,r19 with zero offset
	lds r19,ptrzero+1
	rcall INTDEC		; save zero offset in string
	ldi r16,13		; ASCII CR
	st  -Z,r16
	ret

INTDEC:
	sbrs r19,7
	rjmp intdec_2
	com r18
	com r19
	subi r18,lo8(-1)
	sbci r19,hi8(-1)
	ldi r20, 45		; ASCII - ($2D)
	st -Z,r20
	rjmp intdec_3
intdec_2:
	ldi r20, 43		; ASCII + ($2B)
	st -Z,r20
intdec_3:
	decval	10000
	st -Z, r20
	decval	1000
	st -Z, r20
	decval	100
	st -Z, r20
	decval	10
	st -Z, r20
	decval	1
	st -Z, r20
	ret

decade:
	ldi r20,0
decade_2:
	sub r18,r16
	sbc r19,r17
	brlo decade_3
	inc r20
	rjmp decade_2
decade_3:
	add r18,r16
	adc r19,r17
	ret	
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TIMER0 initialisation (Dec drive) ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global TIMER0_init

TIMER0_init:
	ldi r16,(1<<CS02)		; Set prescaler to 256 (see Table 12-9, p103) (f=62.5KHz, 16MHz)	
	sts TCCR0B,r16								
	ret		

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TIMER1 initialisation (16-bit timer) ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;using as simple timer in normal mode

.global TIMER1_init

TIMER1_init:
	ldi r16,0			; Normal port operation: OC1A/B disconnected
	sts TCCR1A,r16	
	ldi r16,(1<<WGM12)|(1<<CS12)|(1<<CS10)	; CTC mode, f = 16MHz/1024 = 15.625kHz, see Table 13-5 (p131).
	sts TCCR1B,r16	
	ldi r17,DIV1_HI	
	ldi r16,DIV1_LO	
   	 sts OCR1AH,r17
	sts OCR1AL,r16	
	ldi r16,(1<<OCIE1A)		; enable Timer1 output compare interrupt 
	sts TIMSK1,r16 
	ldi ZL,pm_lo8(PEC_Table)	; load Z reg with address of PEC table
	ldi ZH,pm_hi8(PEC_Table)	
	lsl ZL				; double because program memory
	rol ZH
    	sts PEC_ptr,ZL			; save pointer
	sts PEC_ptr+1,ZH	
	ret
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TIMER2 initialisation (RA drive) ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global TIMER2_init

TIMER2_init:
	ldi r16,(1<<COM2A0)|(1<<WGM21) 	; Toggle on compare match, CTC mode
	sts TCCR2A,r16					
;	ldi r16,(1<<CS22)|(1<<CS21) 	; Set prescaler to 256 (see Table 15-9, p153) f=62.5kHz for 16MHz xtal	
	ldi r16,(1<<CS22)|(1<<CS20)	; Set prescaler to 128 (see Table 15-9, p153) 	
;									  1  0  0 =   64  (250kHz)
;									  1  0  1 =  128  (125kHz)
;									  1  1  0 =  256  (62.5kHz)
;									  1  1  1 = 1024  (15.625kHz)
	sts TCCR2B,r16					
	ldi r16,DIV2
	mov r15,r16
	sts OCR2A,r16
	sbi (PORTB-32),PB4		; set DIR HI (-ve RA) to counteract sidereal drift
	sbi (PORTB-32),PB5		; set ENA HI, enable RA stepper motor	
	
;*******TEST**********
;	lds r16,TIMSK2		; get current Timer2 interrupt mask
;	ori	r16,(1<<OCIE2A)	; enable Timer2 output compare interrupt  
;	sts TIMSK2,r16		; 
;***********************
	
	ret		

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TIMER0 Compare Interrupt routine ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global TIMER0_COMPA_vect

TIMER0_COMPA_vect:
	lds r21,SREG		; save status register
	push r21
	push r0
	clr r0			; r0 == 0 indicates stepping in declination 
	lds r21, DEC_accel
	sbrc r21,0		; break if not accelerating or decelerating
	rcall STEP
	pop r0
	pop r21
	sts SREG,r21
	reti	
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Perform one step in RA or Dec ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
STEP:	
	push r20
	push r19
	push r18
	push r17
	push r16
	push r11
	push r10
	push r9
	push r8
	push r7
	push r6
	push ZL
	push ZH
    	rcall UPDOWN	; increment/decrement step count
	                       			; (step count returned in r18:r21)
	brne S_nz
	rjmp S_finish		; jump if back at zero
S_nz:
	clr r16
	ldi r17, 64			; step count >= 16384? (16MHz)
	cp  r18, r16
	cpc r19, r17
	cpc r20, r16
	cpc r21, r16	
	brlo S_cont
    	breq S_cont	
	rjmp S_end		; break if count >= 16384
S_cont:
	tst r0				; RA or Dec?
	breq S0			; jump if Dec
	lds r6,RA_SLEW_gear
	lds r7,RA_accel	
	rjmp S1
S0:
	lds r6,DEC_SLEW_gear
	lds r7,DEC_accel
S1:	
	clr r16
	ldi r17, 4			; step count = 1024? (16MHz)
	cp  r18, r16
	cpc r19, r17
	cpc r20, r16
	cpc r21, r16	
	breq S_co			; jump if equal - change gear
	rjmp S_get		
	
;  -------- changeover point -----------	
S_co:						
	tst r7					; accelerating or decelerating?
	brmi S_decel			; jump if decelerating	
	
S_accel:
    	eor r6, r6				; change to upper frequency range
	com r6
	tst r0					; RA or Dec?
	breq S_acc_1			; jump if Dec
	
	sts RA_SLEW_gear, r6	
	ldi r17,(1<<CS21)|(1<<CS20) 	; set TIMER2 prescaler to 32 
    	sts TCCR2B, r17	
    	rjmp S_get
    	
S_acc_1:
    	sts DEC_SLEW_gear, r6	
	ldi r17,(1<<CS01)|(1<<CS00)  	; set TIMER0 prescaler to 64 
	sts TCCR0B, r17	
	rjmp S_get
	
S_decel:
    	clr r6					; change to lower frequency range
	tst r0					; RA or Dec?
	breq S_dec_1			; jump if Dec
	
	sts RA_SLEW_gear, r6
	ldi r17,(1<<CS22)|(1<<CS20) 	; set TIMER2 prescaler to 128 	
	sts TCCR2B, r17	
    	rjmp S_get
    	
S_dec_1:
	sts DEC_SLEW_gear, r6
	ldi r17,(1<<CS02)  		; set TIMER0 prescaler to 256 (there's no 128!)
    	sts TCCR0B, r17	
; --------------------------------------

S_get:	
    	tst r6				; lower or upper freq range?
    	breq S_lower		; jump if lower	
S_upper:	
	lsr r21				; divide step count by sixteen if upper 
	ror r20
	ror r19	
	ror r18
	lsr r21	  
	ror r20
	ror r19	
	ror r18
	lsr r21	
	ror r20
	ror r19	
	ror r18
	lsr r21			
	ror r20
	ror r19	
	ror r18
S_lower:
    	ldi ZL, pm_lo8(SLEW_Table)
	ldi ZH, pm_hi8(SLEW_Table)
	lsl ZL				; double because Program Memory
	rol ZH	
	add ZL, r18
	adc ZH, r19	
	lpm r16, Z         		; Load divisor from pm pointed to by Z reg 
	
S_save_Z:
	tst r0				; RA or Dec?
	breq S2			; jump if Dec	
	sts OCR2A,r16	; set RA output frequency on OC2A
    	rjmp S_end
S2:
	sts OCR0A,r16	; set Dec output frequency on OC0A
	rjmp S_end
	
S_finish:
	clr r16
	tst r0				; RA or Dec?
	breq S_f_dec		; jump if Dec	
	
	sts RA_accel,r16	; clear accelerate flag to prevent further steps
	sbi (PORTB-32),PB4	; set DIR HI (-ve RA) to compensate sidereal drift
	rjmp S_end
	
S_f_dec:
	sts TIMSK0,r16		; disable TIMER0 compare interrupt
	sts DEC_accel,r16		; clear accelerate flag
	rcall PULSE			; to stop movement (becoz OCRA still toggling)	
    	rcall TIMER0_init		; reset TIMER0 prescaler to default value	
S_end:
	pop ZH
	pop ZL
	pop r6
	pop r7
	pop r8
	pop r9
	pop r10
	pop r11
	pop r16	
	pop r17
	pop r18	
	pop r19
	pop r20
	ret	
	
;;;;;;;;;;;;;;;;;;;;;;;;
; Up/down step counter ;
;;;;;;;;;;;;;;;;;;;;;;;;

UPDOWN:	
    	tst r0							; RA or Dec?
	breq UD_0						; jump if Dec
	lds r18,RA_STEP_count		; get current RA step count
	lds r19,RA_STEP_count+1
	lds r20,RA_STEP_count+2
	lds r21,RA_STEP_count+3
	lds r16,RA_accel
	rjmp UD_1
UD_0:
	lds r18,DEC_STEP_count		; get current Dec step count
	lds r19,DEC_STEP_count+1
	lds r20,DEC_STEP_count+2
	lds r21,DEC_STEP_count+3
	lds r16,DEC_accel
UD_1:
	tst r16				; accelerating or decelerating?
    	brpl UD_incr			; jump if accelerating
	clr r8
	clr r9
	clr r10
	clr r11
	cp  r8,r18			; is count zero?
	cpc r9,r19
	cpc r10,r20
	cpc r11,r21
	brlo UD_decr			; jump if not
	ret
UD_decr:
	subi r18,1			; decrement step count if decelerating		
	sbci r19,0
	sbci r20,0
	sbci r21,0
	rjmp UD_save
UD_incr:
	subi r18,-1			; increment step count if accelerating
	sbci r19,-1
	sbci r20,-1
	sbci r21,-1	
	tst r0				; RA or Dec?
	breq UD_3			; jump if Dec
	lds r8,RA_STEP_max		; get RA step maximum
	lds r9,RA_STEP_max+1
	lds r10,RA_STEP_max+2
	lds r11,RA_STEP_max+3;
	rjmp UD_4
UD_3:	
	lds r8,DEC_STEP_max		; get Dec step maximum
	lds r9,DEC_STEP_max+1
	lds r10,DEC_STEP_max+2
	lds r11,DEC_STEP_max+3
UD_4:
	cp  r8,r18			; reached max step count?
	cpc r9,r19
	cpc r10,r20
	cpc r11,r21
	brne UD_save			; jump if not at max
	ser r16
	tst r0				; RA or Dec?
	breq UD_5			; jump if Dec
	sts RA_accel,r16		; start decelerating
	clz				; return non-zero result
	ret
UD_5:
	sts DEC_accel,r16		; start decelerating
	clz				; return non-zero result
	ret
UD_save:
	tst r0				; RA or Dec?
	breq UD_6			; jump if Dec
	sts RA_STEP_count,r18		; save new step count
	sts RA_STEP_count+1,r19
	sts RA_STEP_count+2,r20
	sts RA_STEP_count+3,r21
	clz	
	ret	
UD_6:	
	sts DEC_STEP_count,r18		; save new step count
	sts DEC_STEP_count+1,r19
	sts DEC_STEP_count+2,r20
	sts DEC_STEP_count+3,r21
	clz	
	ret	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TIMER1 Compare Interrupt routine ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;uses   r13,			guiding counter
;       r14,			guiding offset
;   	r15,			previous divisor
; 	r16 - r21		temp registers
.global TIMER1_COMPA_vect

TIMER1_COMPA_vect:
	lds r21,SREG		; save status register
	push r21
	push r20
	push r19
	push r18
	push r17
	push r16
;	rcall Update_clock	; advance clock 7217 pulses
	lds r16, RA_accel
	sbrc r16,0	
	rjmp TC1_4			; return if slewing in RA
	push YL
	push YH
	push ZL
	push ZH
	lds ZL,PEC_ptr		; get pointer into PEC table
	lds ZH,PEC_ptr+1
	lpm r16, Z+         		; Load divisor from pm pointed to by Z reg & increment pointer 
	ldi YL, pm_lo8(PEC_Table+518)	; reached end of table?		
	ldi YH, pm_hi8(PEC_Table+518)	
	lsl YL				; double because PM
	rol YH		
	cp YL, ZL			; compare low byte
	cpc YH, ZH			; compare high byte
	brne TC1_1			; jump if not end of table
	ldi ZL,pm_lo8(PEC_Table)	; load Z reg with address of start of table
	ldi ZH,pm_hi8(PEC_Table)	; 
	lsl ZL				; double because PM
	rol ZH	
TC1_1:				
    	sts PEC_ptr,ZL			; save new pointer for next time
	sts PEC_ptr+1,ZH
	ser r17
	sts encoder_mask,r17		; reset encoder_mask
	lds r17,drive_flags		; get drive correction flags
	sbrs r17,2			; (bit2 = 0 to disable PEC)
	ldi r16,225
	lds r18,guiding_counter	
	lds r19,guiding_offset			
	tst r18				; test guiding counter
	breq TC1_2 			; jump if zero - not guiding
	brmi TC1_2			; just in case!
	dec r18				; decrement counter
	sts guiding_counter,r18
	sbrc r17,1			; set bit1 = 0 to disable CCD guiding
	add r16,r19			; add guiding offset
TC1_2:
	clr r19
    	sbrc r17,0			; set bit0 = 0 to disable encoder feedback
	lds r19,encoder_feedback	
	sub r16,r19			; subtract encoder feedback
	brmi TC1_2a			; > 255?
	ldi r16,255			; yes- set = 255
TC1_2a:
    	cpi r16,196			; < 196?
    	brsh TC1_2b
	ldi r16,196			; yes- set = 196
TC1_2b:	
	cp	r16,r15			; compare new divisor with current divisor
	breq TC1_3			; skip if the same
	mov r15,r16			; save new divisor for update
	lds r16,TIMSK2			; get current Timer2 interrupt mask
	ori r16,(1<<OCIE2A)		; enable Timer2 output compare interrupt  
	sts TIMSK2,r16			; (OCR2A will be updated on next compare)
TC1_3:
    	pop ZH
	pop ZL
    	pop YH
	pop YL
TC1_4:
	pop r16
	pop r17
	pop r18
	pop r19
	pop r20
	pop r21
	sts SREG,r21
	reti

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TIMER2 Compare Interrupt routine ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.global TIMER2_COMPA_vect

TIMER2_COMPA_vect:
	lds r21,SREG		; save status register
	push r21
	push r0
	lds r21,RA_accel
	tst r21			; are we slewing in RA?
	breq T2C_1		; jump if not
	clr r0
	com r0			; flag indicates that STEP is called from TIMER2_COMPA
	rcall STEP		; perform one slew step in RA
	rjmp T2C_2
T2C_1:
	sts OCR2A,r15		; set sidereal output frequency on OC2A
	clr r21		
	sts TIMSK2,r21		; disable Timer2 interrupts
	sbi (PORTB-32),PB4	; set DIR HI (-ve RA) to compensate sidereal drift
	sbi (PORTB-32),PB5	; set ENA HI
T2C_2:
    	pop r0
	pop r21
	sts SREG,r21
	reti

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; INT1 intialisation routine ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global INT1_init

INT1_init:
; Enable interrupts on pin PD3
	ldi r16, (1<<INT1)
	sts EIMSK,r16

; Set INT1 to be triggered by rising edge
	ldi r16, (1<<ISC11)|(1<<ISC10)
	sts EICRA,r16
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; INT1 External Interrupt routine, switch connected to PD3 (pin 5) ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global INT1_vect

INT1_vect:
	lds r21,SREG			; save status register
	push r21
	push r17
	push r16
	push ZL
	push ZH
	ldi ZL,pm_lo8(PEC_Table)	; load Z reg with address of start of table
	ldi ZH,pm_hi8(PEC_Table)
	lsl ZL				; double since PM
	rol ZH
	lds r16,ptrzero			; offset to zero point
	lds r17,ptrzero+1
	add ZL,r16			; add to base address
	adc ZH,r17
	sts PEC_ptr,ZL			; reset pointer to zero point of PEC table	
	sts PEC_ptr+1,ZH
    	pop ZH
	pop ZL
	pop r16
	pop r17
	pop r21
	sts SREG,r21
	reti
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Slewing intialisation routines ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global RA_SLEW

RA_SLEW:
	push r16
	clr r16
	sts RA_STEP_count,r16	; clear step count
	sts RA_STEP_count+1,r16
	sts RA_STEP_count+2,r16
	sts RA_STEP_count+3,r16
	sts RA_SLEW_gear,r16
	ldi r16, 1
	sts RA_accel,r16	; set accelerate flag
	ser r16
	sts OCR2A,r16		; set default output frequency on OC2A 
	lds r16,TIMSK2		; get current Timer2 interrupt mask
	ori r16,(1<<OCIE2A)	; enable Timer2 output compare interrupt  
	sts TIMSK2,r16		; (OCR2A will be updated on next compare)
	sbi (PORTB-32),PB5	; set ENA HI, enable RA stepper motor		
	pop r16
	ret
	
.global DEC_SLEW
	
DEC_SLEW:
;	push r16
	clr r16
	sts DEC_STEP_count,r16	; clear step count
	sts DEC_STEP_count+1,r16
	sts DEC_STEP_count+2,r16
	sts DEC_STEP_count+3,r16
	sts DEC_SLEW_gear,r16
	ldi r16, 1
	sts DEC_accel,r16	; set accelerate flag	
	ldi r16,(1<<COM0A0)|(1<<WGM01) 	; Toggle OC0A on compare match, CTC mode
	sts TCCR0A,r16			
	lds r16,TIMSK0		; get current Timer0 interrupt mask
	ori r16,(1<<OCIE0A)	; enable Timer0 output compare interrupt  
	sts TIMSK0,r16		; (OCR0A will be updated on next compare)	
	sbi (PORTD-32),PD5	; set Dec ENA HI	
;	pop r16
	ret

; Wait 5 x 64000 cycles = 20.000ms (16MHz)
	
.global WAIT
WAIT:		
        push ZL
        push ZH	
	clr ZL
	clr ZH
w1:	adiw ZL,1		; 2 cycles
	cpi ZH,250		; 1 cycle
	brne w1		; 2 cycles
	pop ZH
	pop ZL
    	ret  
	
; Single ustep in Dec
.global PULSE

PULSE:
	ldi r16,(1<<COM0A1)|(1<<WGM01) 	; CLEAR OC0A on compare match, CTC mode
	sts TCCR0A,r16			
	lds r16,TCCR0B
    	ori r16,(1<<FOC0A)	; force output compare
	sts TCCR0B,r16		; (to clear PUL)
	
	sbi (PORTD-32),PD5	; set ENA HI, enable Dec stepper motor	
	rcall WAIT		; wait 20ms
	sbi (PORTD-32),PD4	; set DIR HI 	
	rcall WAIT		; wait 20ms
	
	ldi r16,(3<<COM0A0)|(1<<WGM01) 	; SET OC0A on compare match, CTC mode
	sts TCCR0A,r16			
	lds r16,TCCR0B
    	ori r16,(1<<FOC0A)	; force output compare
	sts TCCR0B,r16		; (to set PUL)
	rcall WAIT
	
	ldi r16,(1<<COM0A1)|(1<<WGM01) 	; CLEAR OC0A on compare match, CTC mode
	sts TCCR0A,r16			
	lds r16,TCCR0B
    	ori r16,(1<<FOC0A)	; force output compare
	sts TCCR0B,r16		; (to clear PUL)
	
	ret
	
PEC_Table_2012_04_05:   
;   2012/04/05 (2012/02/18 plus corrections of 2012/04/05)
.word  57825, 57825, 57825, 57825, 57825, 57825, 57825, 57825
.word  58082, 58082, 58338, 58339, 58596, 58852, 58853, 59110
.word  59366, 59367, 59624, 59624, 59881, 59881, 59881, 59881
.word  59625, 59624, 59624, 59367, 59111, 58854, 58853, 58596
.word  58340, 58083, 58082, 57825, 57569, 57568, 57311, 57311
.word  57054, 57054, 56797, 56797, 56541, 56540, 56540, 56283
.word  56283, 56283, 56027, 56026, 56026, 56026, 56026, 56026
.word  56026, 56026, 56026, 56026, 56026, 56026, 56026, 56026
.word  56026, 56026, 56026, 56026, 55770, 55769, 55769, 55513
.word  55512, 55512, 55256, 55255, 55255, 55255, 54998, 54998
.word  54998, 54998, 54742, 54741, 54741, 54741, 54741, 54741
.word  54741, 54741, 54741, 54741, 54741, 54741, 54741, 54998
.word  54998, 55255, 55511, 55768, 56026, 56539, 56796, 57310
.word  57824, 58338, 58852, 59366, 60136, 60651, 61165, 61679
.word  62193, 62451, 62964, 63221, 63222, 63479, 63479, 63479
.word  63479, 63222, 62966, 62709, 62708, 62451, 62194, 61937
.word  61680, 61424, 61167, 61166, 60910, 60909, 60653, 60652
.word  60652, 60396, 60395, 60139, 60138, 59882, 59881, 59624
.word  59368, 59367, 59110, 58853, 58597, 58596, 58339, 58082
.word  57826, 57825, 57568, 57312, 57311, 57054, 56798, 56541
.word  56284, 56027, 55770, 55513, 55256, 54998, 54485, 54228
.word  53971, 53714, 53457, 53456, 53199, 53199, 53199, 53199
.word  53199, 53199, 53456, 53712, 53969, 54226, 54227, 54484
.word  54741, 54997, 54998, 55255, 55255, 55512, 55512, 55512
.word  55512, 55768, 55769, 55769, 56026, 56282, 56283, 56540
.word  57053, 57310, 57824, 58081, 58595, 58853, 59366, 59624
.word  59881, 60138, 60395, 60395, 60652, 60652, 60395, 60395
.word  60138, 59882, 59881, 59624, 59368, 59367, 59367, 59367
.word  59367, 59367, 59367, 59367, 59624, 59624, 59881, 59881
.word  60137, 60138, 60138, 60138, 59882, 59881, 59625, 59624
.word  59368, 59111, 59110, 58853, 58597, 58596, 58339, 58339
.word  58082, 58082, 58082, 58082, 58082, 57826, 57825, 57825
.word  57825, 57825, 57825

PEC_Table:
;PEC_Table_2012_02_18:
.word  58596, 58596, 58596, 58596, 58596, 58852, 58853, 58853
.word  58853, 59110, 59110, 59366, 59367, 59623, 59624, 59880
.word  59881, 60137, 60138, 60138, 60138, 60138, 59882, 59881
.word  59625, 59624, 59367, 59111, 58854, 58597, 58596, 58339
.word  58082, 57826, 57569, 57568, 57312, 57311, 57054, 57054
.word  56798, 56797, 56797, 56541, 56540, 56540, 56540, 56283
.word  56283, 56283, 56283, 56027, 56026, 56026, 56026, 56026
.word  56026, 56026, 56026, 56026, 56026, 56026, 56026, 56026
.word  56026, 56026, 56026, 56026, 55769, 55769, 55769, 55513
.word  55512, 55512, 55512, 55255, 55255, 55255, 55255, 54998
.word  54998, 54998, 54998, 54998, 54998, 54998, 54998, 54998
.word  54998, 54998, 54998, 54998, 54998, 54998, 54998, 55255
.word  55255, 55512, 55768, 56025, 56282, 56540, 57053, 57567
.word  57824, 58338, 58852, 59622, 60137, 60651, 61165, 61679
.word  61937, 62450, 62708, 62965, 63221, 63222, 63222, 63222
.word  63222, 62965, 62709, 62452, 62451, 62194, 61937, 61680
.word  61423, 61167, 61166, 60909, 60653, 60652, 60652, 60396
.word  60395, 60395, 60139, 60138, 59882, 59881, 59625, 59368
.word  59367, 59110, 58854, 58597, 58340, 58339, 58082, 57825
.word  57569, 57568, 57311, 57054, 56798, 56797, 56540, 56283
.word  56026, 55769, 55512, 54999, 54742, 54485, 54227, 53970
.word  53713, 53456, 53199, 52942, 52942, 52686, 52685, 52941
.word  52942, 53198, 53199, 53456, 53713, 53969, 54226, 54483
.word  54740, 54997, 54998, 55254, 55255, 55255, 55512, 55512
.word  55512, 55512, 55768, 55769, 55769, 56026, 56283, 56539
.word  56797, 57310, 57567, 58081, 58339, 58852, 59366, 59623
.word  59881, 60138, 60395, 60651, 60652, 60652, 60652, 60396
.word  60395, 60139, 60138, 59881, 59625, 59624, 59624, 59624
.word  59624, 59624, 59624, 59624, 59624, 59881, 59881, 59881
.word  60137, 60138, 59882, 59881, 59881, 59625, 59624, 59367
.word  59111, 58854, 58853, 58596, 58340, 58339, 58083, 58082
.word  58082, 58082, 58082, 58082, 58082, 58082, 58339, 58339
.word  58339, 58339, 58595

PEC_Table_default:   
;        518 entries
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825, 57568, 57825, 57568, 57825, 57568
.word  57825, 57568, 57825

SLEW_Table:
.word 65535,65535,58576,49589,43681,39571,36232,33664
.word 31607,29810,28268,27239,25955,24928,24157,23386
.word 22871,22100,21329,20816,20046,19532,19275,18761
.word 18247,18245,17731,17219,16705,16705,16448,16448
.word 15677,15677,15163,15163,15161,14649,14649,14135
.word 14135,14135,14133,13621,13621,13621,13618,12850
.word 12850,12850,12850,12336,12336,12336,12336,12336
.word 12333,11565,11565,11565,11565,11565,11565,11565
.word 10794,10794,10794,10794,10794,10794,10794,10794
.word 10794,10023,10023,10023,10023,10023,10023,10023
.word 10023,10023,10023,10023,10023,8995,8995,8995
.word 8995,8995,8995,8995,8995,8995,8995,8995
.word 8995,8995,8995,8995,8995,8995,8224,8224
.word 8224,8224,8224,8224,8224,8224,8224,8224
.word 8224,8224,8224,8224,8224,8224,8224,8224
.word 8224,8224,8224,8224,8224,8224,8224,8224
.word 6939,6939,6939,6939,6939,6939,6939,6939
.word 6939,6939,6939,6939,6939,6939,6939,6939
.word 6939,6939,6939,6939,6939,6939,6939,6939
.word 6939,6939,6939,6939,6939,6939,6939,6939
.word 6939,6939,6939,6939,6939,6939,6939,6939
.word 6939,6939,6934,5654,5654,5654,5654,5654
.word 5654,5654,5654,5654,5654,5654,5654,5654
.word 5654,5654,5654,5654,5654,5654,5654,5654
.word 5654,5654,5654,5654,5654,5654,5654,5654
.word 5654,5654,5654,5654,5654,5654,5654,5654
.word 5654,5654,5654,5654,5654,5654,5654,5654
.word 5654,5654,5654,5654,5654,5654,5654,5654
.word 5654,5654,5654,5654,5654,5654,5654,5654
.word 5654,5654,5654,5654,5654,5654,5654,5654
.word 5654,5654,5654,5654,5654,5654,5654,5654
.word 5654,5654,5654,5654,5654,5654,5654,5654
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112,4112,4112,4112,4112,4112,4112,4112
.word 4112


.end
