;***********************************************************
;1-wire bus, microLan routines for DS1820 temperature sensor
;Author: Joachim Beyer, achim@itmin.com
;Circuit diagramme available
;
;portb0 serial transmission micro lan
;ds1850 protocoll: reset, convert temp, wait 500ms,
;reset,recall,reset.
;see datasheet www. dalsemi.com
;if more devices are on the bus the protocoll is different.
;device connection: VCC, serial bus, GND
;
;routines were tested and timed with logic controller
;clock rate: 1 MHz
;device AT90S2343-1, internal RC oscillator
;************************************************************

.equ convert_t= $44
.equ tx= $be
.equ led=3

.def temp= r16
.def shift= r17
.def cnt= r18
.def cnt1= r19
.def reading= r21
.def fail=r22
.def bits= r23 ;bit counter

.include "D:\avrtools\appnotes\2343def.inc"
; code can be used for any device

.cseg


;**********************************************
;main protocoll routines
;output: reading= temperature in 8bit format
;positive number
;if neg temp reading is required read 16 bit
;with the sign bit (15)
;see data sheet
;*************************************************

microbus:
         cli
         rcall reset
         rcall convert
         sei
         rcall break
         cli
         rcall reset
         clr reading
         cbi portb,led

         rcall recall
         rcall debug
         rcall reset
         sei
         ret



;......................................................
;convert command to ds1820 to convert chip temperature
;into 9 bit word
;........................................................

convert: 
         ldi shift,convert_t
         rcall transmit
         ret

;===================================
;initiates reading of DS1820
;if reading fails T-flag is set,
;repeats then the entire protocol
;and reads again. If this fails the
;program goes to error
;===================================

recall:  
         CLT
         ldi fail,3
zxc:     ldi shift,tx
         rcall transmit

         ldi cnt,10
yt:      dec cnt
         brne yt

         rcall read
         brtc ut
         dec fail
         breq error
         rjmp zxc
ut:      ret

;=======================================
;routine to send commands to ds1820
;=======================================

transmit:
         rcall outmod
         ldi bits,8

next:    cbi portb,0          ;startbit

         ldi cnt,2
stbit:   dec cnt
         brne stbit
         lsr shift
         brcs send1           ;9
         nop
         nop
         rjmp send0
send1:   sbi portb,0

send0:   
         ldi cnt,16           ;x3
ssd:     dec cnt
         BRNE ssd
         sbi portb,0
recover: 
         nop
         nop
         nop
         dec bits
         brne next            ;62
         ret


;===============================================
;loop after sensor failure, toggles LED, F=5Hz
;================================================
error:   
         cbi portb,led
         rcall sbreak
         sbi portb,led
         rcall sbreak
         rjmp error

;**************************************************
;temperature reading (input)
;serial data, PB0,
;input: 8bit
;bit0 is eliminated since it denotes .5 centigrades
;if .5 centigrades accuracy is required keep bit0 and
;use the value as temperature x 2 (see data sheet)
;****************************************************

read:    
         clc
         ldi bits,8

startbit:
         cbi portb,0
         nop
         nop
         rcall inmod
         nop
         nop
         nop
         nop
         SBIC pinb,0          ;14us read bit
         sec
         ror reading          ;shift in
         dec bits
         BREQ complete
         CLC                  ;20us
         ldi cnt1,12
rst:     dec cnt1
         brne rst
         rcall outmod
         rjmp startbit        ;62us

complete:
         cpi reading,$ff
         breq xdc
         lsr reading          ;eliminate bit0
         clt
         ret
xdc:     
         set                  ;error
         ret


;''''''''''''''''''''''''''''''''''''''''
;debug, serial outp of temperature
;5us low=1 2us low=0
;pb1 outp visible only on logic analyser
;for debugging
;........................................

debug:   
         ldi cnt,8
         clc
bit:     rol reading
         BRCC szero
         cbi portb,1
         nop
         nop
         nop
         nop
         nop
         nop
         sbi portb,1
         rjmp ccb
szero:   cbi portb,1
         nop
         nop
         sbi portb,1

ccb:     dec cnt
         brne bit
         ret


;*************************************
;master reset routine of DS1820 protocol
;required to initiate a commnd writing to device
;1. master reset pulse 500us low,
;2. acknowledge
;3. command: skip rom
;***************************************

reset:   
         ldi fail,3
fff:     
         rcall outmod
         cbi portb,0
         ldi cnt,75
         ldi cnt1,1
         rcall wait

;recovery time 50us
         rcall inmod

recov:   sbis pinb,0          ;500us
         rjmp recov
         ldi cnt,30

;test for acknowledgement
ackn:    nop
         nop
         dec cnt
         BREQ ackfail
         SBIc pinb,0          ;600us
         rjmp ackn
         ldi cnt,100
         ldi cnt1,1
         rcall wait

;skip rom
         ldi shift,$cc
         rcall transmit
         nop
         ret
ackfail: 
         dec fail
         brne fff
         rjmp error


;*******************************************
;subroutine 500ms break for temperature
;conversion time required by ds1850
;*******************************************

break:   
         ldi cnt1,250
         ldi cnt,250
         rcall wait
         ret


;*******************************************
;subroutine 125ms break for led timing

sbreak:  
         ldi cnt1,60
         ldi cnt,250
         rcall wait
         ret
;*********************************************
;subroutines: port change to outp and inp mode
;**********************************************

outmod:  
         sbi portb,0
         sbi ddrb,0
         ret


inmod:   
         cbi ddrb,0
         cbi portb,0
         ret
;******************************************
;delay sub routine
;inp: cnt, cnt1
;******************************************

wait:    sei
         nop
         nop
         nop
         nop
         dec cnt
         BRNE wait
         dec cnt1
         brne wait
         cli
         ret
;*****************************************


