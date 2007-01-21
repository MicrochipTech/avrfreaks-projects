

;******************************************************
;for wiring details see the scematic!!!
;for reference the probability table
;  2    3    4    5    6    7    8    9   10   11   12
;1/36 2/36 3/36 4/36 5/36 6/36 5/36 4/36 3/36 2/36 1/36
; How to calculate:
;calculate a random number  under the 36, depending on
;the number calculate the solution, and the way the led's 
;should be controlled.
;Note:
;* a random number is generated depending on the time you
;  wait with the next buttenpush
;* No intterupts are used, this is a program to practice
;  case situations.
;******************************************************


.include "2313def.inc"
.def Temp = R17
.def dicea = r18
.def diceb = r19
.def tempa = r20

RESET:			
;override alternating functions of pin's
.org 0x0000

ldi temp,0x00
out mcucr,temp	;pullup disable

;erase all old shit	
ldi dicea,0x00	;no numbers inserted
ldi diceb,0x00
ldi temp,0x00
out 0x1b,temp	;is not included in 2313def.inc" this is the direct adress
ldi temp,0xff
out portd,temp
out ddrd,temp
ldi temp,0xfe
out portb,temp
out ddrb,temp
;voor het gebruik van de pointer


rjmp GEN_NUM


GEN_NUM:			;creates the to random numbers (see asm for details) (extreme fast toss gives error) (check if timing is correct)
ldi temp,0x07

;exit if button pressed
sbis pinb,0x00
rjmp BLING_BLING

inc dicea
cpse dicea,temp			;compare skip if equal
rjmp GEN_NUM
inc diceb
ldi dicea,0x01
cpse diceb,temp
rjmp GEN_NUM
ldi dicea,0x01
ldi diceb,0x01
rjmp GEN_NUM




BLING_BLING:
;*****************************************************
;doe iets met een pointer of functie om die wait niet drie maal te schrijven!!!
;*****************************************************



;state1
.org 0x0020
ldi r22,0b10011011
out portb,r22
ldi r22,0b10101101
out portd,r22
ldi temp,low(RAMEND)
out spl,temp
rcall DLY

;state2
.org 0x0030
ldi r22,0b11101111
out portb,r22
ldi r22,0b11110111
out portd,r22
rcall DLY

;state3
.org 0x0040
ldi r22,0b01110101
out portb,r22
ldi r22,0b11011010
out portd,r22
rjmp DLY


;the delay
DLY:
dec r23
;stop hiermee als button niet meer ingedrukt!
sbic pinb,0x00
rjmp WRITE
;ga verder met delay
brne DLY
dec r24
brne DLY
cpi r22,0b11011010
breq BLING_BLING		;kijk of in state3

ret ;return to where func called (mind after state 3 there is no return!!


;overall return to repeat entire cycle (not used!)
rjmp BLING_BLING





WRITE:		;of the first dime
cpi dicea,0x01
breq c01
cpi dicea,0x02
breq c02
cpi dicea,0x03
breq c03
cpi dicea,0x04
breq c04
cpi dicea,0x05
breq c05
cpi dicea,0x06
breq c06
rjmp c00

c00:
ldi temp,0b11111110
rjmp writa
c01:
ldi temp,0b11101110
rjmp writa
c02:
ldi temp,0b10111100
rjmp writa
c03:
ldi temp,0b10101100
rjmp writa
c04:
ldi temp,0b00111000
rjmp writa
c05:
ldi temp,0b00101000
rjmp writa
c06:
ldi temp,0b00010000
rjmp writa

writa:
out portb,temp
rjmp WRITE2

WRITE2:		;of the second dime
cpi diceb,0x01
breq c11
cpi diceb,0x02
breq c12
cpi diceb,0x03
breq c13
cpi diceb,0x04
breq c14
cpi diceb,0x05
breq c15
cpi diceb,0x06
breq c16
rjmp c10

c10:
ldi temp,0b11111111
rjmp writb
c11:
ldi temp,0b11110111
rjmp writb
c12:
ldi temp,0b10111110
rjmp writb
c13:
ldi temp,0b10110110
rjmp writb
c14:
ldi temp,0b10011100
rjmp writb
c15:
ldi temp,0b10010100
rjmp writb
c16:
ldi temp,0b10001000
rjmp writb

writb:
out portd,temp
rjmp GEN_NUM





