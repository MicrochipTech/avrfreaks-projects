;blued7s_drv.asm v0.04
;LiteON Blue LED Dual 7-Segment Display Driver
;(Binary Counter AVRISP Test Code)
;rentzu 20050718 ATmega16L

.include "m16def.inc"	;ATmega16.part def 

.def Temp=R16		   	;general use reg
.def Count1=R17			;counter stage1 reg
.def Count2=R18			;counter stage2 reg
.def Count3=R19			;counter stage3 reg

.org 0x0000				   ;place next opcode at addy 0x0000

	rjmp RESET		   	;jump to RESET
	
RESET:					   ;device init...

	ldi Temp, 0xFF		   ;load 0xFF into Temp
	out DDRB, Temp		   ;output Temp to PORTB data dir reg

LOOP:					      ;main deathloop

	out PORTB, Temp		;output Temp to PORTB (LED 7seg driver)
	
	ldi Count1, 0x01	   ;init timekill bigloop (~.25sec)

WASTE3:					   ;start timekill bigloop

	ldi Count2, 0xFF	   ;init timekill midloop

WASTE2:					   ;start timekill midloop

	ldi Count3, 0xFF	   ;init timekill tinyloop

WASTE1:  				   ;start timekill tinyloop

 	dec Count3			   ;decrement Count3
	brne Waste1			   ;jump to WASTE1 if Count3 != 0

	dec Count2			   ;decrement Count2
	brne Waste2			   ;jump to WASTE2 if Count2 != 0

	dec Count1		   	;decrement Count1
	brne Waste3		   	;jump to WASTE3 if Count3 != 0
	
	dec Temp		      	;decrement Temp reg (binary downcount)
	rjmp LOOP	   		;restart deathloop



   