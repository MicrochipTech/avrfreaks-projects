# Uni-polar stepper motor

Uploaded by mo7mad on 2008-03-18 23:36:00 (rating 0 out of 5)

## Summary

.nolist ; Don't show list directives  

.include "m32def.inc"  

.def temp=r16  

.def contd1=r10  

.def contd2=r11  

.def contd3=r12  

.list  

.cseg ; Code Segment (Flash storage)  

.org $000  

rjmp start


start:  

cli


;---------------Stack initialization-----------------------------  

ldi temp,$08  

out sph,temp  

ldi temp,$5f  

out spl,temp  

;-----------------Port & Pointers initialization-------------  

ser temp  

out ddra,temp ;Output Port  

sei


ii:ldi zh,high(2*table)  

 ldi zl,low(2*table)


main\_loop:  

rcall delay  

adiw z,$0002  

lpm  

out porta,r0  

ldi temp,$09  

cpse r0,temp  

rjmp main\_loop  

rjmp ii


delay:  

clr temp  

mov contd1,temp  

xx:inc contd1


clr temp  

mov contd2,temp  

yy:inc contd2


clr temp  

mov contd3,temp  

zz:inc contd3


mov temp,contd3  

cpi temp,$ff  

brne zz


mov temp,contd2  

cpi temp,$ff  

brne yy


mov temp,contd1  

cpi temp,$1f  

brne xx


ret


table:  

 .db $00,$01,$03,$02,$06,$04,$0c,$08,$09

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
