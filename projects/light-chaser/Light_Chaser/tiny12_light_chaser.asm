;*********************************************************************************
;
;	Title: 	tinyAVR 12 light chaser program
;   Author: Josh Wolf, engg100@yahoo.com
;
;	Target:	tinyAVR12 or similar 8 pin device
;
;	Description: 	You've probably seen them before, is cars, those LED light
;					chasers. Well this is my attempt at making one. It is a very
;					simple program, but gimmie a break it is my first one. You 
;					can set the delay of the chaser by setting the value of the
;					setDelay variable. The higher you set the value of setDelay,
;					the slower the lights will chase eachother. 
;
;**********************************************************************************
.include "tn12def.inc"

.def Temp	 =r16		;temp variable for output
.def Delay   =r17		;delay variable 1
.def Delay2  =r18		;delay variable 2

.equ setDelay =0x2D		;set length of delay light chaser

RESET:
  ldi  Temp, 0xFF 
  out  DDRB, Temp		;set PORTB pins to output

LOOP:
  ldi  Temp, 0xFE		
  out  PORTB, Temp		;LED0 on
  rcall DLY				;delay for specified amount

  ldi  Temp, 0xFD		
  out  PORTB, Temp		;LED1 on
  rcall DLY				;delay for specified amount
  
  ldi  Temp, 0xFB		
  out  PORTB, Temp		;LED2 on
  rcall DLY				;delay for specified amount  

  ldi  Temp, 0xF7		
  out  PORTB, Temp		;LED3 on
  rcall DLY				;delay for specified amount

  ldi  Temp, 0xEF
  out  PORTB, Temp		;LED4 on
  rcall DLY				;delay for specified amount

  ldi  Temp, 0xF7
  out  PORTB, Temp		;LED3 on
  rcall DLY				;delay for specified amount

  ldi  Temp, 0xFB
  out  PORTB, Temp		;LED2 on
  rcall DLY				;delay for specified amount
  
  ldi  Temp, 0xFD
  out  PORTB, Temp		;LED1 on
  rcall DLY				;delay for specified amount  

  rjmp LOOP				;Loop forever

;Delay subroutine
;setDelay variable sets the delay
DLY:
  ldi  Delay2, setDelay	;store delay in Delay2 register
DLY1:  
  DLY2:
    dec  Delay
    brne DLY2
  dec  Delay2
  brne DLY1
  ret
