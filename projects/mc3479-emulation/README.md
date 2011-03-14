# MC3479 emulation

Uploaded by evacio on 2011-03-14 06:35:00 (rating 0 out of 5)

## Summary

Instead of using two mc3479 we may use just one at90s2313.  

Just half step but the code may be changed for full step.


Please test the code and if possible give some feedback.


;***************************************************************************  

;  

; File Name :'MC3479.asm"  

; Title :  

; Date :  

; Version :


; Target MCU :AT90S82313  

;  

;***************************************************************************;


;  

;  

;A simple routine to emulate two MC3479.  

;  

;Half-Stepping  

;  

;Phase Current  

;  

.equ block =$60


;.include "c:\asmavr\appnotes\2313def.inc"  

.include "C:\Archivos de programa\Atmel\AVR Tools\AvrAssembler\Appnotes\2313def.inc"  

;.include "Equates.inc" ;The basics, naming of registers etc.


;  

; Register usage  

; r17 Holds the status of PORTB  

; r24 ix  

; r25 iy 


;  

;********************************************************************  

;  

;The actual beginning of the code.  

;INIT the machine, start the ISRs, and exit into idle  

.cseg  

 .org 0x00 ;  

 rjmp on\_reset ;Restart vector, points to the beginning of the code  

 .org 0x1  

 rjmp \_int0\_  

 .org 0x2  

 rjmp \_int1\_  

;  

 rjmp halt  

halt:  

; sei  

 rjmp halt  

;  

;********************************************************************  

 .org 0x11  

on\_reset:  

; Perro guardian desactivado....................................  

;  

;Can't call any subroutines till we have a stack!  

 ldi r16,0xDF ;  

 out SPL,r16 ;init Stack Pointer Low  

; ldi r16,0x02 ;  

; out SPH,r16 ;init Stack Pointer High  

;  

; Initialize all CPU hardware peripherals  

 ldi r16, 0xff ;  

 out DDRB, r16 ; PORTB as output  

; PORTD as output  

 ldi r16,0xff  

 out DDRD,r16  

; pull up  

 ldi r16,0xff  

 out PORTD,r16  

; PORTD as input  

 ldi r16,0x00  

 out DDRD,r16  

; 


 rcall Init\_Stepper  

loop\_forever:  

 nop  

 rjmp loop\_forever


;  

;*******************************************************************  

;  

;Initial states for stepper motor control  

;  

Init\_Stepper:  

 ldi ZH,high(Half\_Step\_Table*2)  

 ldi ZL,low(Half\_Step\_Table*2)  

 ldi YH,high(block)  

 ldi YL,low(block)


 ldi r16,8  

flash2ram:  

 lpm ; r0,Z+  

 st Y+,r0  

 adiw ZL,1  

 dec r16  

 brne flash2ram  

;  

 ldi r24,0  

 ldi r25,0  

; Direccion de secuencia en memoria  

 ldi XH,high(block) ; Direccion de memoria de block  

 ldi XL,low(block) ; queda en X


; r17 guarda el registro de PORTB  

 in r17,PORTB ; r17 hods the value of PORTB  

;  

 cli  

 in r16,MCUCR  

 ori r16,0x0f  

 out MCUCR,r16  

;  

 in r16,GIMSK  

 ori r16,0xC0  

 out GIMSK,r16  

;  

 sei  

;  

 ret  

;


\_int0\_:  

 sbic PIND,PD4  

 rjmp \_xClockWise  

\_xContClockW:  

 dec r24 ; ix--  

 rjmp \_xEndIf  

\_xClockWise:  

 inc r24 ; ix++  

\_xEndIf:  

 andi r24,7 ; and r24, 0000 0111 


 mov r30,r24  

 clr r31


 add r30,XL  

 adc r31,XH  

;  

 ld r16,Z  

;there is no need to update r17  

 andi r17,0xf0  

 or r17,r16  

;  

 out PORTB,r17  

 reti


\_int1\_:  

 sbic PIND,PD5  

 rjmp \_yClockWise  

\_yContClockW:  

 dec r25 ; iy--  

 rjmp \_yEndIf  

\_yClockWise:  

 inc r25 ; iy++  

\_yEndIf:  

 andi r25,7 ; and r24, 0000 0111 


 mov r30,r25  

 clr r31  

;  

 add r30,XL  

 adc r31,XH  

;  

 ld r16,Z ; retrieve next sequence  

 swap r16  

; there is no need to update r17  

 andi r17,0x0f  

 or r17,r16  

;  

 out PORTB,r17  

 reti  

;  

;Half\_Step\_Table:  

;.db $08 ;1000 Full current  

;.db $0C ;1100 Half current  

;.db $04 ;0100 Full current  

;.db $06 ;0110 Half current  

;.db $02 ;0010 Full current  

;.db $03 ;0011 Half current  

;.db $01 ;0001 Full current  

;.db $09 ;1001 Half current  

Half\_Step\_Table:  

.db $08,$0c ;1000 Full current  

.db $04,$06 ;0110 Half current  

.db $02,$03 ;0011 Half current  

.db $01,$09 ;1001 Half current  

;  

;

## Compilers

- AVR Studio 3

## Tags

- Complete code
- AVR Studio 3
