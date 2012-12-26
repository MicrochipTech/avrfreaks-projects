/*
 * test1.asm
 *
 *  Created: 12/18/2012 9:32:52 PM
 *   Author: stuart ponder
 *   sjponder@gmail.com
 *   blinks an LED on portA5
 *   work in progress to becoming a template starting point
 */ 

;
; Included header file for target AVR type
.NOLIST
.INCLUDE "tn1634def.inc" ; Header for ATTINY1634
.LIST
;============================================
; INCLUDED FILES
;============================================


;
; ============================================
;   H A R D W A R E   I N F O R M A T I O N   
; ============================================
;
.DEVICE ATTINY1634 ; not sure if I need this, might be redundant
;
; ============================================
;      P O R T S   A N D   P I N S 
; ============================================
;
; [Add names for hardware ports and pins here]
; Format: .EQU Controlportout = PORTA
;         .EQU Controlportin = PINA
;         .EQU LedOutputPin = PORTA2
.EQU ledPin = PORTB3
;
; ============================================
;            C O N S T A N T S   
; ============================================
;
.EQU fosc = 8000000
.EQU baudrate = 9600
;
; ============================================
;      D E R I V E D   C O N S T A N T S 
; ============================================
;
.EQU ubrrno	= (fosc/(baudrate)) - 1
.EQU ubrrh = ubrrno/256
.EQU ubrrl = ubrrno%256
;
; ============================================
;   R E G I S T E R   D E F I N I T I O N S
; ============================================
;
; [Add all register names here, include info on
;  all used registers without specific names]
; Format: .DEF gpReg1 = R16
.DEF gpReg1 = R16 ; general purpose register 1
.DEF gpReg2 = R17 ; general purpose register 2
.DEF scratch = R18 ; register to use for whatever, no need to push/pop
.DEF count  = R19 ; register for counting
; ============================================
;       S R A M   D E F I N I T I O N S
; ============================================
;
.DSEG
.ORG  0X0100
string:		.BYTE 80; communication buffer
; Format: Label: .BYTE N ; reserve N Bytes from Label:
;
; ============================================
;   R E S E T   A N D   I N T   V E C T O R S
; ============================================
;
.CSEG
.ORG $0000
		rjmp	Main ; Reset vector

;
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
;

		jmp		INT0_ISR ; Address 0x0002
		jmp		PCINT0_ISR ; Address 0x0004
		jmp		PCINT1_ISR ; Address 0x0006
		jmp		PCINT2_ISR ; Address 0x0008
		jmp		WDT_ISR ; Address 0x000A
		jmp		TIM1_CAPT_ISR ; Address 0x000C
		jmp		TIM1_COMPA_ISR ; Address 0x000E
		jmp		TIM1_COMPB_ISR ; Address 0x0010
		jmp		TIM1_OVF_ISR ; Address 0x0012
		jmp		TIM0_COMPA_ISR ; Address 0x0014
		jmp		TIM0_COMPB_ISR ; Address 0x0016
		jmp		TIM0_OVF_ISR ; Address 0x0018
		jmp		ANA_COMP_ISR ; Address 0x001A
		jmp		ADC_ISR ; Address 0x001C
		jmp		USART0_RXS_ISR ; Address 0x001E
		jmp		USART0_RXC_ISR ; Address 0x0020
		jmp		USART0_DRE_ISR ; Address 0x0022
		jmp		USART0_TXC_ISR ; Address 0x0024
		jmp		USART1_RXS_ISR ; Address 0x0026
		jmp		USART1_RXC_ISR ; Address 0x0028
		jmp		USART1_DRE_ISR ; Address 0x002A
		jmp		USART1_TXC_ISR ; Address 0x002C
		jmp		USI_START_ISR ; Address 0x002E
		jmp		USI_OVF_ISR ; Address 0x0030
		jmp		TWI_ISR ; Address 0x0032
		jmp		EE_RDY_ISR ; Address 0x0034
		jmp		QTRIP_ISR ; Address 0x0036
;
; ============================================
;     M A I N    P R O G R A M    I N I T
; ============================================
;

Main:
; Init stack
		ldi		gpReg1,		HIGH(RAMEND) ; Init MSB stack
		out		SPH,		gpReg1
		ldi		gpReg1,		LOW(RAMEND) ; Init LSB stack
		out		SPL,		gpReg1
; Init Port A
		ldi		gpReg1,		(0<<DDA0)|(0<<DDA1)|(0<<DDA2)|(0<<DDA3)|(0<<DDA4)|(1<<DDA5)|(0<<DDA6)|(0<<DDA7) ; Direction Port A [1=output]
		out		DDRA,		gpReg1
; Init Port B
		ldi		gpReg1,		(0<<DDB0)|(0<<DDB1)|(0<<DDB2)|(0<<DDB3) ; Direction Port B
		out		DDRB,		gpReg1

; SETUP PORT C -- TIMER 0 OC0A OUTPUT OVERRIDES NORMAL PORT OPERATION ON PIN 17, PORTC0
; setup timer output on PC0 (pin 17)
		ldi		gpReg1,		(0<<DDC0)|(0<<DDC1)|(0<<DDC2)|(0<<DDC3)|(0<<DDC4)|(0<<DDC5) ; Direction Port C 
		out		DDRC,		gpReg1


;
;
; Sleep mode
/*	ldi gpReg1,1<<SE ; enable sleep
	out MCUCR,gpReg1
	sei*/
;
; ============================================
;	MAIN PROGRAM LOOP
; ============================================
;
Loop:
;		sleep										; go to sleep
		nop 	
		;in		gpReg1,		PINA					; Read port A pins
		ldi		gpReg1,		25						; wait about 250ms
		rcall	Delay
		rjmp	TogglePA6
		rjmp	loop								; go back to loop
;
; =============================================
;	SUBROUTINES
; =============================================



TogglePA6:

; toggle a pin
		sbis	PORTA,		PORTA5					; if bit is set, clear it  
		rjmp	setbit								; bit is clear, must be set
		cbi		PORTA,		PORTA5					
		rjmp	loop
setbit:	sbi		PORTA,		PORTA5

		ret

; delay requires delay value to be placed in gpReg1
Delay:
		push	gpReg2
		push	count
		ldi		gpReg2,		78						;timer will count to 78 to make ~ 10ms intervals	
		out		OCR0A,		gpReg2					;load the top value for inner loop


; count down from value,  wait for the inner delay each time, repeat until count is decremented to zero
		mov		count,		gpReg1					; initialize counter to value
outerDelayLoop:
		call	innerDelay
		dec		count								; count down from value to zero
		brne	outerDelayLoop						; loop back if zero flag not set yet
		clz											; clear zero flag							
		pop		count								; restore clobbered registers and return
		pop		gpReg2
		ret
	
innerDelay:
		rcall	startTimer0slow						; start timer using /1024 prescaling
		push	gpReg1

idloop:			
		in		gpReg1,		TIFR
		sbrs	gpReg1,		OCF0A					; loop if timer zero output compare flag is clear 
		rjmp	idloop		
		ori		gpReg1,		(1<<OCF0A)				; clear OCF0A	
		out		TIFR, gpReg1	
		rcall	stopTimer0							; stop timer
		pop		gpReg1
		ret

; setup timer 0 for millisecond operation
startTimer0slow:
		push	gpReg1
		ldi		gpReg1,		0							
		out		TCNT0,		gpReg1					; reset counter
		
;	TCCR0A:				COM0A1	COM0A0	COM0B1	COM0B0	---		---		WGM01	WGM00
;	======================================================================================
;	CTC MODE															1		0
;	SET OC0A			1		1
		ldi		gpReg1,		(0<<COM0A1)|(1<<COM0A0)|(0<<COM0B1)|(0<<COM0B0)|(1<<WGM01)|(0<<WGM00)
		out		TCCR0A,		gpReg1
		
;	TCCR0B:				FOC0A	FOC0B	---		---		WGM02	CS02	CS01	CS00
;	======================================================================================
;	CTC MODE			0		0						0		
;	~1/8 MILLISECONDS												1		0		1
;	STOP TIMER0													0		0		0			
		ldi		gpReg1,		(0<<FOC0A)|(0<<FOC0B)|(0<<WGM02)|(1<<CS02)|(0<<CS01)|(1<<CS00)
		out		TCCR0B,		gpReg1
		pop		gpReg1
		ret

stopTimer0:
		push	gpReg1
		ldi		gpReg1,		0
		out		TCCR0B,		gpReg1
		pop		gpReg1
		ret

;
; ============================================
;		INTERRUPT SERVICE ROUTINES
; ============================================


INT0_ISR:		reti			
PCINT0_ISR:		reti
PCINT1_ISR:		reti
PCINT2_ISR:		reti
WDT_ISR:		reti
TIM1_CAPT_ISR:	reti
TIM1_COMPA_ISR:	reti
TIM1_COMPB_ISR:	reti
TIM1_OVF_ISR:	reti
TIM0_COMPA_ISR: reti
TIM0_COMPB_ISR:	reti
TIM0_OVF_ISR:	reti
ANA_COMP_ISR:	reti
ADC_ISR:		reti
USART0_RXS_ISR:	reti
USART0_RXC_ISR:	reti
USART0_DRE_ISR:	reti
USART0_TXC_ISR:	reti
USART1_RXS_ISR:	reti
USART1_RXC_ISR:	reti
USART1_DRE_ISR:	reti
USART1_TXC_ISR:	reti
USI_START_ISR:	reti
USI_OVF_ISR:	reti
TWI_ISR:		reti
EE_RDY_ISR:		reti
QTRIP_ISR:		reti