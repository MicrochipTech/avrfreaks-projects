;***** PWM demonstration with LED ****
; this program demonstrates how to set the PWM mde of the Timer/Counter1  
; Target	: AT2313 
; Hardware	: LED at PORTB3
;*************************************
.include "2313def.inc"
.def Temp =r16 ; Temporary register
.def pw =r19

;***** Initialization
INIT_SP:

			ldi	temp,RAMEND
			out	spl,temp

INIT_PORTB:
			ser	Temp
			out	DDRB,Temp			; Set PORTB to output
			clr temp
			out PORTB,temp			; Set PORTB to 0

INIT_TIMER:
			ldi	temp,0				; Set Output Comp Reg H to 0
			out	OCR1AH,temp
			ldi temp,0				; Set pulse width
			out OCR1AL,temp
			ldi	temp, 0b10000001	; Set Timer/Counter1 as an 8-bit PWM     
			out	TCCR1A, temp		;
			ldi temp, 0b00001001	; Start Timer/Counter1 , set PWM mode to clear OC1(PB3) at upcounting
			out TCCR1B, temp
	
			
LOOP:
	
UP:			out OCR1AL,pw			; Output to CompareRegister1 which will set the puls width  
			inc pw					; Increase the pulse width
			rcall delay				; Delay of 0.01s
			cpi pw,0xFF				; Check if pulse width is max
			brne UP					; 

DOWN:		out OCR1AL,pw
			dec pw
			rcall delay
			cpi pw,0x00
			brne DOWN
			rjmp LOOP


DELAY:								; delay of 0.01s @ 4Mhz
; ============================= 
;    delay loop generator 
          ldi  R17, $43
WGLOOP0:  ldi  R18, $C6
WGLOOP1:  dec  R18
          brne WGLOOP1
          dec  R17
          brne WGLOOP0
; ----------------------------- 
; delaying 1 cycle:
          nop
; =============================

 
			ret
