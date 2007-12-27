;****************************************************************
;* Titel : One I/O Pin 3x7-Segment Driver with 74HCT164´s       *
;*                                                              *	
;* Version: 1.2                                                 *
;*																*
;* Autors:														*
;*        Hardware Basic Concept:Rex Niven,Australia			*
;*		  His Article is on EDN,June 21,2007					*
;*																*
;*		  Sven de Marothy has developed a three I/O Pin			*
;*        Hardware for 2x7-Segment Displays.					*
;*        See his Project on www.avrfreaks.net					*
;*		  														*
;*		  I have combined this two ideas to a single			*
;*		  I/O-Pin 3x7-Segment Display							*
;*																*																 
;*																*
;* Date: 26/12/2007                                             *
;* 							                                    *	
;* Description:                                                 *
;*																*
;* Will work only with additional Hardware (see attached		*
;* Circuit-Diagram)												*
;*																*
;* One general I/O-Pin connected to an RC-Lowpass Filter        *
;* controls three 8-Bit Serial-to-Parallel Converter 74HC(T)164 *
;* which will activate the LED-Segments.						*
;*																*
;* Decimal-Point can be displayed as defined by Constant "DP"	*
;*																*
;* This Code-Example will run on every ATMEL-Processor with		*
;* a Clock-Frequency from 500KHz up to 8MHz with no change.		*
;* Faster (or slower) clocks requires changes in the			*
;* Delay-Routine and possibly changes on the RC-Lowpass-Filter	*
;*																*
;* Set your Clock-Speed in constant "xtal"						*
;*																*
;* In this Example PB0 is the general I/O-Pin, but this can be	*
;* easily changed (see Constants).								*			
;*                                                              *
;****************************************************************
.nolist
.include "tn2313def.inc" 	;All ATMEL´s supported
.list
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                         Name Constants
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

; define Output-Pin (change to your needs)
 
.equ SegDDR=DDRB	;DDR for 7-Segment Hardware
.equ SegPort=PORTB	;Port for 7-Segment Hardware
.equ SegPin=PB0		;Output-Pin for 7-Segment Hardware

.equ DP = 0			;set DP to 0 to supress Dezimalpoint at all
					;set DP to 1 to show Decimalpoint on left 7-Segment
					;set DP to 2 to show Decimalpoint on middle 7-Segment
					;Dezimalpoint on right 7-Segment is not implemented but
					;can be done

.equ DP_Bit = 0x7F	;Bit-Position of Decimal-Point on Shift-Register
					;-->     can NOT be changed !   <--
					;--> fixed Position by Hardware <--

.equ xtal = 1000000	;Your Clock in Hz (500KHz to 8MHz)
.equ Del_Val = xtal/100000

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                         Name Registers
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.def Reserved	= r0	;needed for LPM
.def Byte_Low 	= r1	;used in "3x7Segment.inc"
.def Byte_Med	= r2	;used in "3x7Segment.inc"
.def Byte_High	= r3	;used in "3x7Segment.inc"

.def Data		= r16	;Data to 3x7-Segment-Display
.def Temp 		= r17	;general Purpose
.def TempA 		= r18	;general Purpose
.def TempB 		= r19	;general Purpose
.def Byte_Out 	= r20	;used in "3x7Segment.inc"

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                        Interrupt-Vectors
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.CSEG
.ORG 0x0000			

	rjmp RESET 	;Reset Handler
	reti		;External Int. 0 Handler
	reti		;External Int. 1 Handler
	reti		;Timer1 Capture Handler
	reti		;Timer1 Compare A Handler
	reti		;Timer1 Overflow Handler
	reti		;Timer0 Overflow Handler
	reti		;USART RX complete Handler
	reti		;USART UDR empty Handler
	reti		;USART TX complete Handler
	reti		;Analog Comparator Handler
	reti		;Pin change Interrupt
	reti		;Timer1 Compare B Handler
	reti		;Timer0 Compare A Handler
	reti		;Timer0 Compare B Handler
	reti		;USI Start Handler
	reti		;USI Overflow Handler
	reti		;EEPROM Ready Handler
	reti		;Watchdog Overflow Handler

Reset:
;	ldi r16,0xXX			;Load OSCCAL-Value
;	out OSCCAL,r16
	ldi r16,RAMEND			;init Stack
	out spl,r16
	sbi	SegDDR,SegPin		; define Output-Pin

;-------- Test-Program for 7-Segment Driver+Hardware -------
	clr Data			;Data counts from 0 to 255
Test_Loop:
	rcall hex2dec		;display Data
	inc Data
	rcall Delay
;now display some Special Characters (Hex-Values see SegTable in 3x7Segment.inc)
	ldi Byte_Out,0x91	;"Y" on right Display
	rcall OutByte
	ldi Byte_Out,0xC7	;"L" on middle Display
	rcall OutByte
	ldi Byte_Out,0x89	;"H" on left Display
	rcall OutByte
	rcall Delay
	rjmp Test_Loop

;********** Delay, so you can see your successfully work ************
Delay:
	clr R17
Delay0:
	ldi  R18, $04
Delay1:
	ldi  R19, $C4
Delay2:
	dec  R19
    brne Delay2
    dec  R18
    brne Delay1
    dec  R17
    brne Delay0
	ret

.include "3x7Segment.inc"

;----------- end of program ------------------
