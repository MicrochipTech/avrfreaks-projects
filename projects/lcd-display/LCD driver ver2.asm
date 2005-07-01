
/* 	ATmega8535 is configured for LCD-driver by Mythos

PORTC=LCD output.

PORTC.0=RS (LCD pin4)
PORTC.1=RW (LCD pin5)
PORTC.2=EN (LCD pin6)
PORTC.3=free
PORTC.4=DB4 (LCD pin11)
PORTC.5=DB5 (LCD pin12)
PORTC.6=DB6 (LCD pin13)
PORTC.7=DB7 (LCD PIN14)
*/

.device ATmega8535
.include "m8535def.inc"

.org $100

.DEF temp=r16
.DEF mask=r17
.DEF data_low=r18
.DEF data_high=r19
.DEF data =r20
.DEF command=r21
.DEF command_low=r22
.DEF command_high=r23
.DEF char=r24				;sent your ascii to char register to display at the LCD 2x16
.DEF count=r25
.DEF result=r26
.DEF value=r27
.DEF message=r28

.equ LCD_RS =0
.equ LCD_RW =1
.equ LCD_E =2
.equ MASK_HIGH =0b11110000
.equ MASK_LOW  =0b00001111


text1:	.db "LCD display 2x16",0,1


.include "initialization.asm"
.include "initialize_LCD.asm"	
.include "main_2.asm"		

;-------------subroutines----------------------------------------

	LCD_init:  .include "LCD_init.asm" 

	LCD_command: .include "LCD_command.asm" 

	LCD_strobe:	.include "LCD_strobe.asm"

	LCD_delay:	.include "LCD_delay.asm"			

	LCD_data:	.include "LCD_data.asm"			

	LCD_strobe_char:  .include "LCD_strobe_char.asm"







