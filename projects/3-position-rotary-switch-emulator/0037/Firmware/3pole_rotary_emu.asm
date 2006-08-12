
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
.include "tn2313def.inc"				;Includes the tn2313def def. file

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
RETI
RETI
RETI
RETI
RETI
.ORG 0x0006 							;Inerrupt service sub
RJMP ISR 								;
RETI

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
		LDI 	TempL,0b11100111 		;Setup PORTB I/O pin
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
		LDI 	TempL,0x02 				;Setup Mask register
		OUT 	TIMSK,TempL 			;
		LDI 	TempL,0x00 				;Setup Timer0 properties
		OUT 	TCCR0A,TempL 			;
		LDI 	TempL,0x05 				;Setup Timer0 prescaler
		OUT 	TCCR0B,TempL 			;
		LDI		Timer0,100				;Reload timer0
		OUT		TCNT0,Timer0
		SEI								;Enable Global interrupts
		LDI		TempL,0b00000000		;Reload TempL with 0b00000000
		MOV		LMap,TempL				;Copy TempL to LMap
		RCALL	ST_RAM					;Store LMap to RAM
		RJMP	Main					;Go to Main Task


;********************************************************************
;*																	*
;*																	*
;*				Kernel												*
;*																	*
;*																	*
;********************************************************************
Main:	MOV		TempL,SMap				;Copy SMap to TempL 				
		ANDI	TempL,0b00011000		;Filter only switches		
		CPI		TempL,0b00011000		;Button(1) pressed?
		BREQ	POS1					;If yes then go to POS1
		CPI		TempL,0b00010000		;Button(2) pressed?
		BREQ	POS2					;If yes then go to POS2
		CPI		TempL,0b00001000		;Button(3) pressed?
		BREQ	POS3					;If yes then go to POS3
		RJMP	Main					;Jump back to Main

POS1:	LDI		TempL,0b00000100		;Reload TempL with 0b00000100
		MOV		LMap,TempL				;Copy TempL to LMap
		RCALL	ST_RAM					;Store LMap to RAM
		RJMP	SHLD					;Jump back to Main

POS2:	LDI		TempL,0b00000010		;Reload TempL with 0b00000010
		MOV		LMap,TempL				;Copy TempL to LMap
		RCALL	ST_RAM					;Store LMap to RAM
		RJMP	SHLD					;Jump back to Main

POS3:	LDI		TempL,0b00000001		;Reload TempL with 0b00000001
		MOV		LMap,TempL				;Copy TempL to LMap
		RCALL	ST_RAM					;Store LMap to RAM
		RJMP	SHLD					;Jump to SHLD
						
SHLD:	RCALL	LD_RAM					;Load LMap from RAM
		MOV		TempL,LMap				;Copy LMap to TempL
		ANDI	TempL,0b00000111		;Filter only leds
		OUT		PORTB,TempL				;Output leds
		RJMP	Main					;Jump back to Main	

;****************************
;*		I.S.R				*
;*		5ms intervals		*
;*		750Hz carrier		*
;*		1,3msec cycle		*
;****************************
ISR:	IN		STATUS,SREG				;Store machine status
		LDI		Timer0,100				;Reload timer0
		OUT		TCNT0,Timer0
		INC		Tick					;+1 to xms register
		IN		SMap,PINB				;Import switches(raw data)
		OUT		SREG,STATUS				;Restore machine status
		RETI

			
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
;****************************
;*	25ms delay				*
;****************************
D25ms:  CLR		Tick					;Re-init 5ms register
DA0:	CPI		Tick,5					;Delay timeout?
		BRNE	DA0						;(Yes)
		RET

;****************************
;*	500ms delay				*
;****************************
D500ms:	CLR		Tick					;Re-init 5ms register
DA1:	CPI		Tick,100				;Delay timeout?
		BRNE	DA1						;(Yes)
		RET

;****************************
;*	1s delay				*
;****************************
D1s:	CLR		Tick					;Re-init 5ms register
DA4:	CPI		Tick,200				;Delay timeout?
		BRNE	DA4						;(Yes)
		RET

;********************************************************************
;*																	*
;*																	*
;*				RAM handlers										*
;*																	*
;*																	*
;********************************************************************
;****************************
;*	1Byte write to RAM		*
;****************************
ST_RAM:	LDI		YH,high(Table)			;Re-Initialize Y pointer 
		LDI		YL,low(Table)			;start address
		ST		Y+,LMap					;Store 1 data byte to RAM
		RET

;****************************
;*	1Byte read from RAM		*
;****************************
LD_RAM:	LDI		YH,high(Table)			;Re-Initialize Y pointer 
		LDI		YL,low(Table)			;start address
		LD		LMap,Y+					;Load 1 data byte from RAM
		RET