;****************************************************************
;dc motor controller
;circuit: motdriv.tif
;Author Joachim Beyer
;achim@itmin.com
;the motor is activated by one pushbutton in either direction.
;the motor is pulsed at 81 Hz to stabilize speed.
;Mount the limit switches with springs to avoid damage!
;code was simulated and tested okay
;****************************************************************

;motor direction
.equ frwrd= 3 ;open
.equ revs= 4 ;close

;pushbutton
.equ pb= int0

;limit switches
.equ limop= 0 ;open
.equ limclo= 2 ;closed

.def t1= r16     ;temp register
.def cnt= r17    ;counter
.def flag= r18



.include "D:\avrtools\appnotes\2343def.inc"

.cseg

         rjmp init
         rjmp activate        ;int0
         rjmp timebase        ;t0 ovfl
         reti


init:    
         clr flag
         clr t1
         out mcusr,t1
         ldi t1,ramend
         out spl,t1
         ldi t1,0b011000
         out ddrb,t1
         out portb,t1

;timer0
         ldi t1,0b010
         out timsk,t1         ;t0 intr en

         ldi t1,0b01000000
         out gimsk,t1         ;intr0 push button
         ldi t1,2
         out mcucr,t1         ;intr0 low level active

         ldi cnt,3            ;counter
         SEI

;***************************************************
;This loop decides whether to start testing
;the limit switches
;input: flag 3 or 4
;***************************************************

loop:    

         sbrc flag,frwrd
         rjmp testopen
         sbrc flag,revs
         rjmp testclosed
         rjmp loop

;*******************************************
; test the limit switches and stop motor
;*******************************************

testopen:
         sbic pinb,limop
         rjmp testopen
         sbi portb,frwrd    ;stop motor
         clr t1
         out TCCR0,t1       ;stop timer
         clr flag
         rjmp loop

testclosed:
         sbic pinb,limclo
         rjmp testclosed
         sbi portb,revs    ;stop motor
         clr t1
         out tccr0,t1      ;stop timer
         clr flag
         rjmp loop

;*************************************************
; activate motor
; int0 routine
; determine direction by testing limit switches
;*************************************************

activate:

;start timer0
         ldi t1,2
         out tccr0,t1         ;prescale 8x256 f= 488Hz
         ldi cnt,3
         clr t1
         out gimsk,t1         ;inhibit push button (debouncing)

;test limit switches
         SBIS pinb,limop
         rjmp close             ;default if both are high
         sbis pinb,limclo
         rjmp open


open:    
         cbi portb,frwrd
         sbr flag,(1 <<frwrd)     ;set forwrd flag 3
         reti

close:   
         cbi portb,revs
         sbr flag,(1 <<revs)      ;set reverse flag 4
         reti

;*****************************************************
; intr routine timer0
; divider by 3= 488Hz/3=162 Hz = 81Hz port frequency
; toggles the port pins to pulse the motor
; input: flag 3 or 4
;*****************************************************

timebase:
         dec cnt
         brne hjk
         ldi cnt,3
;....................................................
; if this instruction is shifted to "loop" it
; would inhibit the PB until the motor stops again.
; In this position, the int0 is enabled after 6.144 ms
; (3x8x256).

         ldi t1,0b01000000
         out gimsk,t1         ;en pushbutton (debouncing)
;.....................................................
         rjmp toggle
hjk:     reti


;***************************************************
; Function: pulses the motor driver
; input: flag 3 or 4
; part of timer intr routine
;***************************************************

toggle:  
         SBRC flag,frwrd
         rjmp toggle3
         sbrc flag,revs
         rjmp toggle4
         reti

toggle3: 
         SBIC pinb,frwrd
         cbi portb,frwrd
         sbi portb,frwrd
         reti

toggle4: 
         sbic pinb,revs
         cbi portb,revs
         sbi portb,revs
         reti







