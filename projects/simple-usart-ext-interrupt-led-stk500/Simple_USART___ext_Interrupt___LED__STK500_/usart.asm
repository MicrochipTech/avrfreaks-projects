; ************************************************************************************
; Author Nay Oo
; Email : nayoo1979@yahoo.com.sg
; Date  : 12 Nov 2003
;
; DESCRIPTION
;
; This program copy the 20 bytes of character data from the program memory to the SRAM
; and send to the PC by means of USART and display on the Hyperterminal screen
; I uses two external interrupts  to control
; SW0 : to display the text on the hypher terminal
; SW1 : to light on and off eight leds
; 
; Microcontroller : AT90S8515
; Development Board : STK500
;						1. Connect PB0-8 to LED0-8 using 10 pins connector
;						2. Connect SW0 to PD2
;						3. Connect SW1 to PD3															
; 						4. Connect TXD to PD1
;						5. Connect RXD to PD0
; 						6. After downloading, connect RS232 cable to STK500
;						   RS232 'SPARE' connector'	
; *************************************************************************************

.include "8515def.inc"
.org	$0000
	rjmp	RESET					; reset handle
.org	INT0addr
	rjmp	IntV0					; Int0 vector (ext. Interrupt from pin D2)
.org	INT1addr
	rjmp	IntV1					; Int1 vector (ext. Interrupt from pin D3)

.equ	BLOCK1 = $60

; ****************** Main Program Register variables
.def 	temp = r16					; temporary storage variable
.def	flashsize=r17
.def	Delay =r18; Delay variable 1
.def	Delay2 =r19; Delay variable 2
.def	LED=r20

flash2ram:
		lpm
		st		Y+,r0			; store in SRAM and increment Y
		adiw	ZL,1
		dec		flashsize
		brne	flash2ram
		ret
usart2pc:
		ldi		temp,23			; Set Baurate register to 23
		out		UBRR,temp		; 9600 baud

		sbi		UCR,TXEN	
		
		ld		temp,Y+

		;transmit character
		out		UDR,temp

wait:
		sbis	USR,TXC
		rjmp	wait

		ldi		Delay2,10
DLY:
		dec		Delay
		brne	DLY
		dec		Delay2
		brne	DLY
		
		dec		flashsize
		brne	usart2pc
		ret
		
;**** Code
RESET:
		ldi		temp,low(RAMEND)
		out 	SPL,temp				; init stack pointer
		ldi		temp,high(RAMEND)
		out		SPH,temp

		; set up interrupt 0 and 1
		ldi		TEMP,(1<<INT0)+(1<<INT1)
		out		GIMSK,TEMP
		ldi		TEMP,$0F
		out		MCUCR,TEMP
	
		ser		temp
		out		DDRB,temp
		
		ser		LED
		out		PORTB,LED
		sei

forever:rjmp	forever

IntV0:
; ****************** Copy 20 bytes ROM -> RAM
		ldi		ZH, high(F_TABLE*2)
		ldi		ZL, low(F_TABLE*2)
		ldi		YH,	high(BLOCK1)
		ldi		YL,	low(BLOCK1)
		ldi		flashsize,20
		
		rcall	flash2ram
		
	
		ldi		YH,	high(BLOCK1)
		ldi		YL,	low(BLOCK1)
		ldi		flashsize,20
	
		rcall	usart2pc
		reti
		

IntV1:
		com		LED
		out		PORTB,LED
		reti


F_TABLE:
		.db		"Embedded Computing"
		.db		10,13				; line feed and carriage return
				;123456789012345678
