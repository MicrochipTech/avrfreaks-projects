;*****************************************************************************
;DC Motor driver bridge with pulswidth modulated speed control
;Schematic pwm.tif
;Author Joachim Beyer, achim@itmin.com

;Clock 1MHz AT1200
;Speed control with 10K Poti
;the analog comp measures the time constant of a rc network, 63% voltage drop.

;Direction control with two push buttons Forw and Reverse, high active
;outp: line cable drivers for remote control
;receiver: 4 opto couplers, 2 nos in series for each direction.
;Pulse frequency aprox 50 Hz, 20ms width
;Speed: 1..19ms pulswidth
;*****************************************************************************



;Definitions
;PortB

.equ frw= 6 ;PB
.equ rev= 7 ;PB
.equ rc= 2 ;rc network active low
.equ fdr= 3 ;forw driver
.equ rdr= 4 ;reverse driver
.equ max= 19 ;pulswidth


.def tmp= r16
.def speed= r17 ;reading from analog comparator
.def pulsw= r18 ;pulsw down counter
.def div20= r19 ;total= mark + space width
.def div4= r22 ;timer divider
.def flag= r23 ;PB flag forw or reverse


.def mask= r2 ;mask off all pins except pushbuttons 0+1



.include "D:\avrtools\appnotes\1200def.inc"

.cseg

         rjmp init
         reti                 ;int0
         rjmp timebase        ;t0 ovfl
         reti                 ;an comp


init:    
         ldi tmp,0b00111100
         out ddrb,tmp
         ldi tmp,0b00000100
         out portb,tmp

         ldi tmp,0b11000000
         mov mask,tmp

;start timer0
         ldi tmp,1            ;1MHz / 256 = 3.9 KHz
         out tccr0,tmp
;comparator
         clr tmp
         out acsr,tmp         ;no intr, trigger on either edge

         clr flag
         clr pulsw

         ldi tmp,0b010
         out timsk,tmp        ;t0 intr en
         ldi div4,4
         ldi div20,20
         ldi speed,10
         SEI
         rcall input
wait:    
         rjmp wait

;*******************************************************************
;function: generates timebase for output, pulse start and pulswidth
;intr 0.256 ms
;timebase 0.256 ms * 4 = 1ms
;outp pulsetime : 20ms = 50Hz
;*******************************************************************

timebase:
         dec div4
         brne no_change
         ldi div4,4           ;1ms


         dec div20
         breq restart         ;20ms

;test Push button change
         tst flag
         BREQ no_change
;test for pulsewidth coundown end
         cpi pulsw,1
;reset pulse
         breq reset
         BRLO no_change
         dec pulsw
no_change:
         reti


;**********************************
;Function: resets the outp pulse
;**********************************

reset:   
         cbi portb,fdr
         cbi portb,rdr
         clr pulsw
         ret


;*******************************************
;reads push buttons forw and reverse
;and sets flags. If both are pressed
;stop the motor
;*******************************************

input:   
         in tmp,pinb
         and tmp,mask
         cp tmp,flag          ;compare with previous flags
         brne change          ;push button change?
         ret

change:  
         cpi tmp,0b10000000   ;determine new setting
         breq forward
         cpi tmp,0b01000000
         breq reverse
         rjmp stop

forward: 
         cbi portb,rdr
         sbi portb,fdr
xcv:     
         mov pulsw,speed      ;reload pulswidth
         mov flag,tmp         ;set valid flag
         ldi div20,20         ;reload total width
         ret

reverse: 
         cbi portb,fdr
         sbi portb,rdr
         rjmp xcv

stop:                         ;stop motor
         clr flag
         cbi portb,fdr
         cbi portb,rdr
         clr pulsw


;*******************************************
;Function: starts the outp pulse
;part of time base
;according to PB settings and speed reading
;each outp cycle starts with pushbutton check
;and speed check 50Hz
;*******************************************

restart: 
         rcall input
         rcall comparator
         cpi flag,0b10000000
         brne jki
         sbi portb,fdr
erf:     mov pulsw,speed
         ldi div20,20
klo:     reti
jki:     
         cpi flag,0b01000000
         brne klo
         sbi portb,rdr
         rjmp erf

;**********************************************************
;reads the speed from the poti setting, scale 1...19
;reads the rc time constant as a measure of the pulswidth
;time= 3x20us = 60us ; the program reads every 3 us the flag
;19 is the max speed; time constant max 60us= 10k x 6nF
;min speed: 3us= 500 x 6nF ;
;the reading 1...19 serves as a counter of the pulswidth 1..19ms
;************************************************************

comparator:
         clr speed
         cbi portb,rc
fgb:     inc speed
         sbis acsr,aco        ;tests every 3 usec !!
         rjmp fgb
;reading finished
         sbi portb,rc
         cpi speed,20         ;exeeds maximum?
         BRSH cbn
         ret
cbn:     ldi speed,max
         ret



