;/*****************************************************************************
;Title  :   Accurate Delay Library
;Author :   SA Develpoment
;Version:   1.02
;*****************************************************************************/

.global DelayCyclesUI
DelayCyclesUI:
  sbiw r24,0x01
  brne DelayCyclesUI
ret

.global DelayCyclesUL
DelayCyclesUL:
mov r1,r24
mov r24,r22
mov r22,r1
mov r1,r25
mov r25,r23
mov r23,r1
clr r1
loop1:
  sbiw r24,0x01
  sbc r22,r1
  sbc r23,r1
  brne loop1
ret
