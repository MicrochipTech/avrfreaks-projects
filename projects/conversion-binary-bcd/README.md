# Conversion of binary to BCD

Uploaded by sun on 2006-12-20 10:52:00 (rating 0 out of 5)

## Summary

.include "m32def.inc"  

.cseg  

.org 0x0000  

rjmp main  

main:  

ldi r16,$00  

ldi r17,$ff  

out ddra,r16  

out ddrb,r17  

ldi r16,$0f  

out ddrc,r16  

ldi r26,$00  

ldi r21,$00  

in r22,pina  

cpi r22,$0a  

brlo less  

brsh more  

rjmp main


less:  

out portb,r22


; delay loop generator  

; 1000000 cycles:  

; -----------------------------  

; delaying 999999 cycles:  

 ldi R17, $09  

WGLOOP0: ldi R18, $BC  

WGLOOP1: ldi R19, $C4  

WGLOOP2: dec R19  

 brne WGLOOP2  

 dec R18  

 brne WGLOOP1  

 dec R17  

 brne WGLOOP0  

; -----------------------------  

; delaying 1 cycle:  

 nop  

; ============================= 


ldi r16,$00  

out portb,r16  

rjmp main


more:  

sbci r22,$0a  

inc r26  

cpi r22,$0a  

brlo output


rjmp more


output:  

cpi r26,$0a  

brsh most  

swap r26  

or r26,r22  

out portb,r26


; delay loop generator  

; 1000000 cycles:  

; -----------------------------  

; delaying 999999 cycles:  

 ldi R17, $09  

WGLOOP01: ldi R18, $BC  

WGLOOP11: ldi R19, $C4  

WGLOOP21: dec R19  

 brne WGLOOP21  

 dec R18  

 brne WGLOOP11  

 dec R17  

 brne WGLOOP01  

; -----------------------------  

; delaying 1 cycle:  

 nop  

; ============================= 


ldi r16,$00  

out portb,r16  

rjmp main


most:  

sbci r26,$0a  

inc r21  

cpi r26,$0a  

brlo final  

rjmp most


final:  

swap r26  

or r26,r22  

ldi r16,$00  

out portc,r21  

out portb,r26


; delay loop generator  

; 1000000 cycles:  

; -----------------------------  

; delaying 999999 cycles:  

 ldi R17, $09  

WGLOOP02: ldi R18, $BC  

WGLOOP12: ldi R19, $C4  

WGLOOP22: dec R19  

 brne WGLOOP22  

 dec R18  

 brne WGLOOP12  

 dec R17  

 brne WGLOOP02  

; -----------------------------  

; delaying 1 cycle:  

 nop  

; ============================= 


out portb,r16  

out portc,r16  

rjmp main
