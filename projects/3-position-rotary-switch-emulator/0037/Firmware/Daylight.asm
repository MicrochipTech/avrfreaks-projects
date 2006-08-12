

;****************************************************************************
;*																			*
;*			3position rotary switch emulator v1.0					        *
;*			Features:	     												*
;*			1) 	8MHZ internal Oscilator    									*
;*			2)	Status leds for switch position								*
;*			3) 	Interrupt driven(5ms ticks)									*
;*																			*
;*			BUG Reports   													*
;*			> None yet			  	      						  (xx/07/06)*
;****************************************************************************
.include "m8def.inc"					;Includes the ATmega8 definitions file

;****************************
;*	GP registers            *
;****************************
.DEF STATUS = R10 						;Machine status register
.DEF TempL 	= R16 						;Reload storage register

;****************************
;*	Timer registers         *
;****************************
.DEF Timer0 = R17 						;Interrupt reload register
.DEF Tick	= R18 						;Milisecond register

;****************************
;*	Switch registers        *
;****************************
.DEF SMap	= R11 						;Switch map register
.DEF LMap	= R12 						;LED map register

;****************************
;*	RAM AREA setup          *
;****************************
.EQU Table  =0x060						;RAM Table start address

;****************************
;*	ROM AREA setup          *
;****************************
.CSEG
.ORG 0x0000 							;The code starts from 0x0000
RJMP RESET 								;Relative Jump to the RESET 

RESET:
;****************************
;*	Stack setup				*
;****************************
		CLI								;Disable Global interrupts
		LDI		TempL,low(RAMEND)		;STACK pointer location 
		OUT		SPL,TempL				;started from the end of SRAM
;****************************
;*	PORTS setup				*
;****************************
;************
;*	PORTB	*
;************
		LDI 	TempL,0b11111111 		;Setup PORTB I/O pin
		OUT 	DDRB,TempL
		LDI 	TempL,0b00000000 		;Setup PORTB I/O pin behaviour
		OUT 	PORTB,TempL
;************
;*	PORTD	*
;************
		LDI 	TempL,0b11111111 		;Setup PORTD I/O pin
		OUT 	DDRD,TempL
		LDI 	TempL,0b00000000 		;Setup PORTD I/O pin behaviour
		OUT 	PORTD,TempL
;****************************
;*	Timers setup			*
;****************************
;
		RJMP	Main					;Go to Main Task


;********************************************************************
;*																	*
;*																	*
;*				Kernel												*
;*																	*
;*																	*
;********************************************************************
Main:	LDI		TempL,0b11111111
		OUT		PORTB,TempL
		RJMP	Main					;Jump back to Main	


			
;********************************************************************
;*																	*
;*																	*
;*				Delay handlers										*
;*																	*
;*																	*
;********************************************************************
;****************************
;*	25us delay				*
;****************************
D25us:  LDI		TempL,$42				;Re-init TempL register
DB0:	DEC		TempL					;(-1) to TempL register
		CPI		TempL,0					;Delay timeout?
		BRNE	DB0						;(Yes)
		RET