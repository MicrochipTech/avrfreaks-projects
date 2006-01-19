;****************************************************************************************************************
;* 																												*
;*					    					LIGHT PATTERN PROGRAM												*
;*																												*
;*				    							by Joel Mills													*
;*			    									 and														*
;*		    									   KC Cook														*
;*	                                                                                                            *
;*                          Used a STK300 with ATMEGA128 with 8 LED's and switches                              *                                                                         *              
;*      Each pattern uses a delay inside a delay to slow the speed down enough for the pattern to be seen       *
;*                                                                                                              *
;*																											    *
;****************************************************************************************************************


.include "m128def.inc"		;include the appropriate header file for the processor
.def	a=R16				;a-c are variables for general use
.def	b=R17
.def	c=R18
.def	Temp=R19			;Temp is a variable to define which lights are on
.def	Delay=R20			;Delay and Delay2 are used to slow the light patterns down
.def	Delay2=R21

.org 0x0000					;tell program to start at beginning of flash

RESET:
	ldi a,$FF				;this uses the first button as a reset
	ldi b,$00	
	ldi Temp, $FF			
	out SPH,a				;set stack pointer high
	out SPL,b				;set stack pointer low

	out PORTD,b				;clear PORTD
	out DDRD,b				;clear data direction register for D
	out DDRB,a				;set data direction register for B
	out PORTB,b

SELECT:	
	
   	in c,PIND       		;read pind
   	com c					;take one's complement of PIN D value
   	cpi c,0x02				;identify which button was pressed using the cpi command,
	;button 2 					
	breq RIDER				;and display the appropriate pattern  	
	cpi c,0x04					;by branching to its subroutine
	;button 3
	breq CD				
	cpi c,0x08
	;button 4
	breq CN
	cpi c,0x10
	;button 5
	breq CH
	cpi c,0x20
	;button 6
	breq PU
	cpi c,0x40
	;button 7
	breq BINARY
	cpi c,0x80
	;button 8
	breq FL
   	rjmp SELECT

CD:							;these are temporary branches used to jump to the correct subroutine
	rjmp COUNTDOWN				;they are necessary because the breq command doesn't branch far enough
CN:
	rjmp COUNTER
CH:
	rjmp CHASER
PU:
	rjmp PULSE
FL:
	rjmp FLASH
	

;******************************************************************************************************
BINARY:						;lights flash in a pattern that counts in binary from 0-255
;a=Light
;b=Delay
;c=Counter

	ldi a,$FF
	out DDRB, a

LOOP:
	ldi b,$FF				;set delay value to max
	ldi c, $80				;set counter value to 128
	DEL:
		in Temp,PIND		;read PIND
		cpi Temp,0xFE		;if PIND0 is pushed, then
		breq RESET				;reset lights

		OUT PORTB,a			;turn on lights
		dec b				;decrement delay value by one
		cpi b,$00			;if delay value is not zero, then
		brne DEL				;return to DEL
		dec c				;decrement counter value by one
		cpi c,$00			;if counter value is not zero, then
		brne DEL				;return to DEL
	dec a					;decrement light value to change pattern
rjmp LOOP


;******************************************************************************************************
RIDER:						;single light moves back and forth

.equ 	setDelay1 =0x50		;set length of delay light chaser

RESET1:
  ldi  Temp, 0xFF 
  out  DDRB, Temp			;set PORTB pins to output

  rjmp ALOOP9				;starts the program from AlOOP9

ALOOP2:
	in a,PIND				;read PIND
	cpi a,0xFE				;if PIND0 is pushed, then
	breq RESET  			;reset the lights
	
  ldi  Temp, 0xFD		
  out  PORTB, Temp			
  	ADLY3:
 		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLY4:  
 	ADLY5:
  		 dec  Delay			;Since one timer will not slow
  		 brne ADLY5			;the patterns down enough
	  	 dec  Delay2		;to see what they do	
	     brne ADLY4			;we use a delay inside another delay
  rjmp ALOOP3 


ALOOP3:
  ldi  Temp, 0xFB		
  out  PORTB, Temp			
  	ADLY6:
         ldi  Delay2, setDelay1	;store delay in Delay2 register
    ADLY7:  
 	ADLY8:
    	 dec  Delay			
    	 brne ADLY8
  	     dec  Delay2
         brne ADLY7
  rjmp ALOOP4


ALOOP4:
  ldi  Temp, 0xF7		
  out  PORTB, Temp			
  	ADLYa:
  		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLYb:  
    ADLYc:
    	 dec  Delay		
     	 brne ADLYc
  		 dec  Delay2
 		 brne ADLYb
  rjmp ALOOP5


ALOOP5:
  ldi  Temp, 0xEF
  out  PORTB, Temp		
  	ADLYd:
  		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLYe:  
  	ADLYf:
    	 dec  Delay		
    	 brne ADLYf
  		 dec  Delay2
  		 brne ADLYe
  rjmp ALOOP6


ALOOP6:
  ldi  Temp, 0xDF
  out  PORTB, Temp		
  	ADLYg:
  		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLYh:  
  	ADLYi:
    	 dec  Delay		
  	     brne ADLYi
  		 dec  Delay2
 		 brne ADLYh
  rjmp ALOOP7


ALOOP7:
  ldi  Temp, 0xBF
  out  PORTB, Temp		
  	ADLYj:
  		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLYk:  
  	ADLYl:
    	 dec  Delay		
  	     brne ADLYl
  		 dec  Delay2
 		 brne ADLYk 
  rjmp ALOOP8


ALOOP8:
  ldi  Temp, 0x7F
  out  PORTB, Temp	
  	ADLYm:
  		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLYn:  
  	ADLYo:
    	 dec  Delay		
  	     brne ADLYo
  		 dec  Delay2
 		 brne ADLYn
 
  rjmp ALOOP15				;jumps to ALOOP15 where the program starts lighting up the LED's backwards


ALOOP9:
  ldi  Temp, 0xFE		
  out  PORTB, Temp		
 	ADLY9:
  		ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLY10:  
  	ADLY11:
   		 dec  Delay		
   		 brne ADLY11
 		 dec  Delay2
 		 brne ADLY10
  rjmp ALOOP2				;From here the pattern is reset to the begining

ALOOP10:
  ldi  Temp, 0xFD		
  out  PORTB, Temp		
  	ADLY12:
 		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLY13:  
 	ADLY14:
  		 dec  Delay		
  		 brne ADLY14
	  	 dec  Delay2
	     brne ADLY13
  rjmp ALOOP9 


ALOOP11:
  ldi  Temp, 0xFB		
  out  PORTB, Temp		
  	ADLY15:
         ldi  Delay2, setDelay1	;store delay in Delay2 register
    ADLY16:  
 	ADLY17:
    	 dec  Delay		
    	 brne ADLY17
  	     dec  Delay2
         brne ADLY16
  rjmp ALOOP10


ALOOP12:
  ldi  Temp, 0xF7		
  out  PORTB, Temp	
  	ADLY1a:
  		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLY1b:  
    ADLY1c:
    	 dec  Delay		
     	 brne ADLY1c
  		 dec  Delay2
 		 brne ADLY1b
  rjmp ALOOP11


ALOOP13:
  ldi  Temp, 0xEF
  out  PORTB, Temp	
  	ADLY1d:
  		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLY1e:  
  	ADLY1f:
    	 dec  Delay		
    	 brne ADLY1f
  		 dec  Delay2
  		 brne ADLY1e
  rjmp ALOOP12


ALOOP14:
  ldi  Temp, 0xDF
  out  PORTB, Temp		
  	ADLY1g:
  		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLY1h:  
  	ADLY1i:
    	 dec  Delay		
  	     brne ADLY1i
  		 dec  Delay2
 		 brne ADLY1h
  rjmp ALOOP13


ALOOP15:
  ldi  Temp, 0xBF
  out  PORTB, Temp	
  	ADLY1j:
  		 ldi  Delay2, setDelay1	;store delay in Delay2 register
	ADLY1k:  
  	ADLY1l:
    	 dec  Delay		
  	     brne ADLY1l
  		 dec  Delay2
 		 brne ADLY1k 
  rjmp ALOOP14			;LEDs now flash in reverse order

;******************************************************************************************************
Res1:
	jmp RESET

COUNTDOWN:					;multiple lights scroll to the left

.equ 	setDelay =0x50		;set length of delay for countdown

RESET2:
  ldi  Temp, 0xFF 
  out  DDRB, Temp		;set PORTB pins to output

  rjmp BLOOP1

BLOOP1:
  	in a,PIND				;read PIND
	cpi a,0xFE				;if PIND0 is pushed, then
	breq Res1					;reset lights
  
  ldi  Temp, 0xFE		
  out  PORTB, Temp		
  	BDLY:
 		 ldi  Delay2, setDelay	
	BDLY1:  
 	BDLY2:
  		 dec  Delay		;two delays are used to slow the pattern down enough to see
  		 brne BDLY2
	  	 dec  Delay2
	     brne BDLY1
  rjmp BLOOP2

BLOOP2:
  ldi  Temp, 0xFC		
  out  PORTB, Temp	
  	BDLY3:
 		 ldi  Delay2, setDelay	
	BDLY4:  
 	BDLY5:
  		 dec  Delay		
  		 brne BDLY5
	  	 dec  Delay2
	     brne BDLY4
  rjmp BLOOP3

BLOOP3:
   ldi  Temp, 0xF8		
   out  PORTB, Temp		
  	BDLY6:
 		 ldi  Delay2, setDelay	
	BDLY7:  
 	BDLY8:
  		 dec  Delay		
  		 brne BDLY8
	  	 dec  Delay2
	     brne BDLY7
  rjmp BLOOP4

BLOOP4:
   ldi  Temp, 0xF0		
   out  PORTB, Temp	
  	BDLY9:
 		 ldi  Delay2, setDelay	
	BDLY10:  
 	BDLY11:
  		 dec  Delay		
  		 brne BDLY11
	  	 dec  Delay2
	     brne BDLY10
  rjmp BLOOP5

BLOOP5:
   ldi  Temp, 0xE0		
   out  PORTB, Temp		
  	BDLYa:
 		 ldi  Delay2, setDelay	
	BDLYb:  
 	BDLYc:
  		 dec  Delay	
  		 brne BDLYc
	  	 dec  Delay2
	     brne BDLYb
  rjmp BLOOP6

BLOOP6:
   ldi  Temp, 0xC0		
   out  PORTB, Temp		
  	BDLYd:
 		 ldi  Delay2, setDelay	
	BDLYe:  
 	BDLYf:
  		 dec  Delay		
  		 brne BDLYf
	  	 dec  Delay2
	     brne BDLYe
  rjmp BLOOP7

BLOOP7:
   ldi  Temp, 0x80		
   out  PORTB, Temp		
  	BDLYg:
 		 ldi  Delay2, setDelay	
	BDLYh:  
 	BDLYi:
  		 dec  Delay
  		 brne BDLYi		
	  	 dec  Delay2
	     brne BDLYh
  rjmp BLOOP8

BLOOP8:
   ldi  Temp, 0x00		
   out  PORTB, Temp	
  	BDLYj:
 		 ldi  Delay2, setDelay	
	BDLYk:  
 	BDLYl:
  		 dec  Delay		
  		 brne BDLYl
	  	 dec  Delay2
	     brne BDLYk
  rjmp BLOOP9

BLOOP9:
   ldi  Temp, 0x01		
   out  PORTB, Temp		
  	BDLYm:
 		 ldi  Delay2, setDelay	
	BDLYn:  
 	BDLYo:
  		 dec  Delay		
  		 brne BDLYo
	  	 dec  Delay2
	     brne BDLYn
  rjmp BLOOP10

BLOOP10:
   ldi  Temp, 0x03		
   out  PORTB, Temp		
  	BDLYp:
 		 ldi  Delay2, setDelay	
	BDLYq:  
 	BDLYr:
  		 dec  Delay	
  		 brne BDLYr
	  	 dec  Delay2
	     brne BDLYq
  rjmp BLOOP11

BLOOP11:
   ldi  Temp, 0x07		
   out  PORTB, Temp		
  	BDLYs:
 		 ldi  Delay2, setDelay	
	BDLYt:  
 	BDLYu:
  		 dec  Delay		
  		 brne BDLYu
	  	 dec  Delay2
	     brne BDLYt
  rjmp BLOOP12

BLOOP12:
   ldi  Temp, 0x0F		
   out  PORTB, Temp		
  	BDLYv:
 		 ldi  Delay2, setDelay	
	BDLYw:  
 	BDLYx:
  		 dec  Delay		
  		 brne BDLYx
	  	 dec  Delay2
	     brne BDLYw
  rjmp BLOOP13

BLOOP13:
   ldi  Temp, 0x1F		
   out  PORTB, Temp		
  	BDLY1a:
 		 ldi  Delay2, setDelay	
	BDLY1b:  
 	BDLY1c:
  		 dec  Delay		
  		 brne BDLY1c
	  	 dec  Delay2
	     brne BDLY1b
  rjmp BLOOP14

BLOOP14:
   ldi  Temp, 0x3F		
   out  PORTB, Temp	
  	BDLY1d:
 		 ldi  Delay2, setDelay	
	BDLY1e:  
 	BDLY1f:
  		 dec  Delay	
  		 brne BDLY1f
	  	 dec  Delay2
	     brne BDLY1e
  rjmp BLOOP15

BLOOP15:
   ldi  Temp, 0x7F		
   out  PORTB, Temp		
  	BDLY1g:
 		 ldi  Delay2, setDelay	
	BDLY1h:  
 	BDLY1i:
  		 dec  Delay	
  		 brne BDLY1i
	  	 dec  Delay2
	     brne BDLY1h
  rjmp BLOOP16

BLOOP16:
   ldi  Temp, 0xFF		
  out  PORTB, Temp		
  	BDLY1j:
 		 ldi  Delay2, setDelay
	BDLY1k:  
 	BDLY1l:
  		 dec  Delay		
  		 brne BDLY1l
	  	 dec  Delay2
	     brne BDLY1k
  rjmp BLOOP1				;this starts the pattern over
   	

;******************************************************************************************************
Res2:
	jmp RESET

COUNTER:					;starts with no lights on, then adds one at a time
.equ setDelay2 = 0xA0		;sets length of delay for counter

.def avr    = R22

RESET3:
  ldi  avr, 0xFF 
  out  DDRB, avr

 rjmp LIGHT1
 

LIGHT1:
	in a,PIND				;read PIND
	cpi a,0xFE				;if PIND0 is pushed, then
	breq Res2					;reset lights
	
	ldi avr, 0xFE
	out PORTB, avr
	DELAYA:
	ldi delay2, setDelay2
	 CDLY:
	 CDLY1:
	 	dec  delay			;two delays used to slow pattern down 
		brne CDLY
	    dec  delay2
	    brne CDLY
 rjmp LIGHT2


LIGHT2:
	ldi avr, 0xFC
	out PORTB, avr
	DELAY1:
	ldi delay2, setDelay2
	 CDLY2:
	 CDLY3:
        dec  delay	
  	    brne CDLY3
	    dec  delay2
	    brne CDLY2
 rjmp LIGHT3


LIGHT3:
	ldi avr, 0xF8
	out PORTB, avr
	DELAYB:
	ldi delay2, setDelay2
	 CDLY4:
	 CDLY5:
        dec  delay		
  	    brne CDLY5
	    dec  delay2
	    brne CDLY4
 rjmp LIGHT4


LIGHT4:
	ldi avr, 0xF0
	out PORTB, avr
	DELAY3:
	ldi delay2, setDelay2
	 CDLY6:
	 CDLY7:
        dec  delay	
  	    brne CDLY7
	    dec  delay2
	    brne CDLY6
 rjmp LIGHT5 


LIGHT5:
	ldi avr, 0xE0
	out PORTB, avr
	DELAY4:
	ldi delay2, setDelay2
	 CDLY8:
	 CDLY9:
        dec  delay		
  	    brne CDLY9
	    dec  delay2
	    brne CDLY8
 rjmp LIGHT6


LIGHT6:
	ldi avr, 0xC0
	out PORTB, avr
	DELAY5:
	ldi delay2, setDelay2
	 CDLY10:
	 CDLY11:
        dec  delay	
  	    brne CDLY11
	    dec  delay2
	    brne CDLY10
 rjmp LIGHT7


LIGHT7:
	ldi avr, 0x80
	out PORTB, avr
	DELAY6:
	ldi delay2, setDelay2
	 CDLY12:
	 CDLY13:
        dec  delay		
  	    brne CDLY13
	    dec  delay2
	    brne CDLY12
 rjmp LIGHT8


LIGHT8:
	ldi avr, 0x00		;this turns on all the lights
	out PORTB, avr
	DELAY7:
	ldi delay2, setDelay2
	 CDLY14:
	 CDLY15:
        dec  delay	
  	    brne CDLY15
	    dec  delay2
	    brne CDLY14
 rjmp LIGHT1			;restarts the pattern				

;******************************************************************************************************
Res3:
	jmp RESET

CHASER:						;single light scrolls to the left, jumping from the end back to the beginning

.equ setDelay3 =0x50		;set length of delay for chaser

RESET4:
  ldi  Temp, 0xFF 
  out  DDRB, Temp			;set PORTB pins to output

  rjmp DLOOPA

DLOOPA:
  	in a,PIND				;read PIND
	cpi a,0xFE				;if PIND0 is pushed, then
	breq Res3			 		;reset lights	

  ldi  Temp, 0xFE
  out  PORTB, Temp
        DDLY200:
 		 ldi  Delay2, setDelay3	;store delay in Delay2 register
	DDLY201:  
 	DDLY202:
  		 dec  Delay			;uses a delay inside a delay to slow speed of pattern
  		 brne DDLY202
	  	 dec  Delay2
	     brne DDLY201
  rjmp DLOOP1 
       

DLOOP1:
  ldi  Temp, 0xFD		
  out  PORTB, Temp			
  	DDLY:
 		 ldi  Delay2, setDelay3	;store delay in Delay2 register
	DDLY2:  
 	DDLY3:
  		 dec  Delay	
  		 brne DDLY3
	  	 dec  Delay2
	     brne DDLY2
  rjmp DLOOP2 

DLOOP2:
  ldi  Temp, 0xFD		
  out  PORTB, Temp			
  	DDLY100:
 		 ldi  Delay2, setDelay3	;store delay in Delay2 register
	DDLY4:  
 	DDLY5:
  		 dec  Delay		
  		 brne DDLY5
	  	 dec  Delay2
	     brne DDLY4
  rjmp DLOOP3 


DLOOP3:
  ldi  Temp, 0xFB		
  out  PORTB, Temp			
  	DDLY6:
         ldi  Delay2, setDelay3	;store delay in Delay2 register
    DDLY7:  
 	DDLY8:
    	 dec  Delay		
    	 brne DDLY8
  	     dec  Delay2
         brne DDLY7
  rjmp DLOOP4


DLOOP4:
  ldi  Temp, 0xF7		
  out  PORTB, Temp		
  	DDLYa:
  		 ldi  Delay2, setDelay3	;store delay in Delay2 register
	DDLYb:  
    DDLYc:
    	 dec  Delay		
     	 brne DDLYc
  		 dec  Delay2
 		 brne DDLYb
  rjmp DLOOP5


DLOOP5:
  ldi  Temp, 0xEF	
  out  PORTB, Temp			
  	DDLYd:
  		 ldi  Delay2, setDelay3	;store delay in Delay2 register
	DDLYe:  
  	DDLYf:
    	 dec  Delay	
    	 brne DDLYf
  		 dec  Delay2
  		 brne DDLYe
  rjmp DLOOP6


DLOOP6:
  ldi  Temp, 0xDF
  out  PORTB, Temp			
  	DDLYg:
  		 ldi  Delay2, setDelay3	;store delay in Delay2 register
	DDLYh:  
  	DDLYi:
    	 dec  Delay		
  	     brne DDLYi
  		 dec  Delay2
 		 brne DDLYh
  rjmp DLOOP7


DLOOP7:
  ldi  Temp, 0xBF
  out  PORTB, Temp			
  	DDLYj:
  		 ldi  Delay2, setDelay3	;store delay in Delay2 register
	DDLYk:  
  	DDLYl:
    	 dec  Delay	
  	     brne DDLYl
  		 dec  Delay2
 		 brne DDLYk 
  rjmp DLOOP8


DLOOP8:
  ldi  Temp, 0x7F
  out  PORTB, Temp		
  	DDLYm:
  		 ldi  Delay2, setDelay3	;store delay in Delay2 register
	DDLYn:  
  	DDLYo:
    	 dec  Delay	
  	     brne DDLYo
  		 dec  Delay2
 		 brne DDLYn
 
  rjmp DLOOPA				;returns to begining and restarts


;******************************************************************************************************
Res4:
	jmp RESET

PULSE:						;lights move in to center from both sides then back out again

.equ setDelay4 =0x80		;set length of delay for Pulse

RESET5:
  ldi  Temp, 0xFF 
  out  DDRB, Temp			;set PORTB pins to output

  rjmp ELOOP1

ELOOP1:
  ldi  Temp, 0x81		
  out  PORTB, Temp		
  	EDLY:
 		 ldi  Delay2, setDelay4	
	EDLY1:  
 	EDLY2:
  		 dec  Delay			;once again two delays are used
  		 brne EDLY2
	  	 dec  Delay2
	     brne EDLY1
  rjmp ELOOP2

ELOOP2:
  	in a,PIND				;read PIND
	cpi a,0xFE				;if PIND0 is pushed, then
	breq Res4					;reset lights

  ldi  Temp, 0x42		
  out  PORTB, Temp	
  	EDLY3:
 		 ldi  Delay2, setDelay4	
	EDLY4:  
 	EDLY5:
  		 dec  Delay		
  		 brne EDLY5
	  	 dec  Delay2
	     brne EDLY4
  rjmp ELOOP3

ELOOP3:
   ldi  Temp, 0x24		
   out  PORTB, Temp		
  	EDLY6:
 		 ldi  Delay2, setDelay4	
	EDLY7:  
 	EDLY8:
  		 dec  Delay		
  		 brne EDLY8
	  	 dec  Delay2
	     brne EDLY7
  rjmp ELOOP4

ELOOP4:
   ldi  Temp, 0x18		
   out  PORTB, Temp	
  	EDLY9:
 		 ldi  Delay2, setDelay4	
	EDLY10:  
 	EDLY11:
  		 dec  Delay	
  		 brne EDLY11
	  	 dec  Delay2
	     brne EDLY10
  rjmp ELOOP5

ELOOP5:
  ldi  Temp, 0x24		
  out  PORTB, Temp		
  	EDLYa:
 		 ldi  Delay2, setDelay4	
	EDLYb:  
 	EDLYc:
  		 dec  Delay		
  		 brne EDLYc
	  	 dec  Delay2
	     brne EDLYb
  rjmp ELOOP6

ELOOP6:
  ldi  Temp, 0x42		
  out  PORTB, Temp	
  	EDLYd:
 		 ldi  Delay2, setDelay4	
	EDLYe:  
 	EDLYf:
  		 dec  Delay	
  		 brne EDLYf
	  	 dec  Delay2
	     brne EDLYe
  rjmp ELOOP7

ELOOP7:
   ldi  Temp, 0x81		
   out  PORTB, Temp		
  	EDLYg:
 		 ldi  Delay2, setDelay4	
	EDLYh:  
 	EDLYi:
  		 dec  Delay		
  		 brne EDLYi
	  	 dec  Delay2
	     brne EDLYh
  rjmp ELOOP2			;resets pattern at ELOOP2
  						;ELOOP1 is only used once in the begining


;**********************************************************************************************************
FLASH:						;lights start in center and move out

.equ setDelayA = 0xFF		;controls delay of LEDs 0 and 7
.equ setDelayB = 0xA0		;controls delay of LEDs 1 and 6
.equ setDelayC = 0x5A		;controls delay of LEDs 2 and 5
.equ setDelayD = 0x2D		;controls delay of LEDs 3 and 4

RESET7:
  ldi  Temp, 0xFF 
  out  DDRB, Temp


	rjmp FLASH1

FLASH1:		
  	in a,PIND				;read PIND
	cpi a,0xFE				;if PIND0 is pushed, then
	breq Res6					;reset lights

  ldi  Temp, 0x18		
  out  PORTB, Temp		
  	FLH:
 		 ldi  Delay2, setDelayA	
	FLH1:  
 	FLH2:
  		 dec  Delay			;two delays used
  		 brne FLH2
	  	 dec  Delay2
	     brne FLH1
  rjmp FLASH2

FLASH2:
  ldi  Temp, 0x24
  out PORTB, Temp
    FLH3:
	     ldi  Delay2, setDelayB
    FLH4:
	FLH5:
	     dec Delay		
		 brne FLH5
		 dec Delay2
		 brne FLH4
  rjmp FLASH3

FLASH3:
  ldi  Temp, 0x42
  out PORTB, Temp
    FLH6:
	     ldi  Delay2, setDelayC
    FLH7:
	FLH8:
	     dec Delay	
		 brne FLH8
		 dec Delay2
		 brne FLH7
  rjmp FLASH4

FLASH4:
  ldi  Temp, 0x81
  out PORTB, Temp
    FLH9:
	     ldi  Delay2, setDelayD
    FLH10:
	FLH11:
	     dec Delay	
		 brne FLH11
		 dec Delay2
		 brne FLH10
  rjmp FLASH1			;restarts pattern

Res6:
	jmp RESET			
