;****************************************************************************
;File name:		Timer0.asm
;Description:	Every 32nd clock cycle of the Timer0 shift left through carry
;				the one on PORTC. The value of the counter is on PORTA. 
;				Timer0 is connected to the external 32,768 kHz crystal of the 
;				Crumb128 board.	Connect some LEDs to the ports and you'll see 
;				it too.
;
;Name:			Jan Tomko
;Company:		Helmholtz Institute, University Clinic Aachen
;				www.hia.rwth-aachen.de
;
;Written in:	AVR Studio 4
;Controller:	ATmega128 on the Crumb128 board by www.chip45.com
;****************************************************************************

.include "m128def.inc"  	;Includes the Atmega128 definitions file

.org 0x0000					;Places the following code from address 0x0000

;Ports initialization
	ldi r16, 0x00			;r16 = 0x00
	out DDRA, r16			;DDRA = 0x00; Port A is output now
	out DDRC, r16			;DDRC = 0x00; Port C is output now

	out PORTA, r16			;Port A = 0x00
	out PORTC, r16			;Port C = 0x00

;Timer0 initialization
	in r16, SFIOR			;set bit7(Timer/Counter Synchronization Mode)
	cbr r16, TSM			;in SFIOR to 0 => disabled
	out SFIOR, r16			;
	
	ldi r16, 0x00			;r16 = 0000 0000
	ldi r17, 0x20			;r17 = 0x20 = 32 (decimal)
	out TCNT0, r16			;clear TCNT0
	out OCR0, r17			;OCR0 = 32 (decimal)
	out TIFR, r16			;clear TIFR
	out TIMSK, r16			;disable clock compare and overflow interrupts

	ldi r16, 0x0F			;r16 = 0000 1111
	out TCCR0, r16			;CTC mode, prescaling CLKtos/1024

	ldi r16, 0x08			;set bit3(Asynchronous Timer/Counter) in ASSR to 1
	out ASSR, r16			;external 32.768 kHz clock selected(pins TOSC1 and TOSC2)

	ldi r17, 0x01			;r17 = 0x0F
	ldi r18, 0x02			;r18 = 0000 0010
	
;*** Initialization complete ***

Clock_wait:
	in r16, TCNT0			;read TCNT0 into r16
	out PORTA, r16			;write TCNT0 to PORTA

	in r16, TIFR			;r16 = TIFR
	sbrs r16, OCF0			;wait until output compare flag is set
	rjmp Clock_wait			;remote jump to Clock_wait lable

	out TIFR, r18			;reset output compare flag
	rol r17					;shift left through carry
	out PORTC, r17			;write r17 to PORTC

	rjmp Clock_wait			;remote jump to Clock_wait label