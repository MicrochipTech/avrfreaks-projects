;My Very First AVR Project

.include "tn15def.inc"  ;Includes the 8515 definitions file
;.include "tn2313def.inc"  ;Includes the 8515 definitions file

.def Input = R16          ;Gives "Defines" Register R16 the name Temp
.def Displayer = R17
.def Const1 = R18		
.def Delay1 = R19
.def Delay2 = R20

.org 0x0000             ;Places the following code from address 0x0000
   rjmp  RESET          ;Take a Relative Jump to the RESET Label
	
RESET:                  ;Reset Label
   ldi Input, 0x03      ;Store 255 in R16 (Since we have defined R16 = Temp)
   out DDRB, Input      ;Setting the value of DDRD to WRITE.
   clr Displayer		;Initialized to zeros
   ser Const1			;Set to high
   ser Delay1
   ldi Delay2, 0xcf
   out PORTB,Delay1		;Turn the led off

Loop:                   ;Loop Label
   in   Input,PINB       ;Normally open push-button.
   eor  Input,Const1
   lsr  Input
   lsr  Input
   andi Input,0x03		;Took Input bit as high bit
   cpi  Input,0x00
   breq Loop
   eor  Displayer,Input
Loop1:					;Delay so you had time to get ur finger off
   dec  Delay1
   brne Loop1
   dec  Delay2
   brne Loop1
   com  Displayer
   out  PORTB,Displayer
   com  Displayer		;return the value again
   ldi  Delay1,0xFF
   ldi  Delay2,0xcf
   rjmp Loop            ;Take a relative jump to the Loop label
