;****************************************************************
;* Titel : 1-Pin I/O Expander with 74HCT164                     *
;*                                                              *	
;* Version: 1                                                   *
;*																*
;* Autor: Basic Concept:Rex Niven,Australia						*
;*		  His Article is on EDN,June 21,2007					*
;*        Code adaptet for ATMEL by Robert Ibener               *
;*																*
;* Date: 27/10/2007                                             *
;* 							                                    *	
;* Description:                                                 *
;* One general I/O-Pin connected to an RC Lowpass Filter        *
;* controls an 8-Bit Serial-to-Parallel Converter 74HC(T)164.	*
;* This Circuit can control slow-reacting Devices (such as		*
;* Relays).LED´s connected to the Output of the 74HC(T)164		*
;* will show some flickering during the shift-process.			*
;*																*
;* This Code-Example will run on every ATMEL-Processor with		*
;* a Clock-Frequency up to 8MHz with no change.					*
;* Faster Clocks requires changes in the Delay-Routine.			*
;*																*
;* In this Example PB0 is the general I/O-Pin.					*			
;*                                                              *
;****************************************************************
.nolist
.include "tn45def.inc"
.list
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                         Name Registers
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.def Temp = r17
.def TempA = r18
.def TempB = r19

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                        Interrupt-Vectors
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.CSEG
.ORG 0x0000			;Start of Flash-RAM
	rjmp	RESET	;Reset Handler
	reti			;INT0 (External Interrupt Request_0) Handler
	reti			;PCINT0 (Pin Change Interrupt Request_0) Handler
	reti			;Timer1_Comp_A (Timer1 Compare Match A) Handler
	reti			;Timer1_OVF (Timer1 Overflow) Handler
	reti			;Timer0_OVF (Timer0 Overflow) handler
	reti			;EE_RDY (EEPROM Ready) Handler
	reti			;ANA_Comp (Analog Comparator) Handler
	reti			;ADC (ADC Conversion complete) Handler 
	reti			;Timer1_Comp_B (Timer1 Compare Match B) Handler
	reti			;Timer0_Comp_A (Timer0 Compare Match A) Handler
	reti			;Timer0_Comp_B (Timer0 Compare Match B) Handler
	reti			;WDT (Watchdog Time-out) Handler
	reti			;USI-Start (USI Start) Handler
	reti			;USI_OVF (USI Overflow) Handler
		
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Start Program
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

Reset:						; Init Stack here for Devices other than Tiny25/45/85!!
	sbi	DDRB,DDB0			; define PB0 as Output
;---
Loop:						; Infinite Test-Loop
	ldi	Temp,0x80			; load Test Value
	rcall OutByte
	rcall Long_Delay		; for test only,remove afterwards
	rjmp Loop

OutByte:					; 260usec/Byte at 8Mhz,2msec/Byte at 1MHz 					
;------
; used registers : Temp,TempA,TempB
; input : Byte to be sent in Temp
; return: Temp,TempA,TempB destroyed

	ldi	TempA,8				; load Bit Counter
	sbi	PORTB,0				; set PB0 High
Next_Bit:
	lsl	Temp				; Shift next bit into carry
	brcs	Write_One
Write_Zero:
	cbi	PortB,0				; if a zero, pin is low for 30 usec.
Write_One:
	rcall Delay				; if a one, pin stays high for 30 usec.
	cbi	PORTB,0				; edge falls here
	sbi	PORTB,0				; rising edge clocks data into 74HC164
	dec	TempA				; Decrement BitCount
	brne Next_Bit			; all Bits done ?
	ret						; yes

Delay:						; 30 usec delay at 8MHz
	ldi TempB,80			; must be adapted if Clock > 8MHz
Delay_Z:
  	dec	TempB
    brne 	Delay_Z
	ret

;********** ;for Test only.remove afterwards ! ************
Long_Delay:				
	ldi Temp,10
Long_:
	rcall Delay
	dec Temp
	brne Long_
	ret
