#include<mega8535.h>
#asm
.equ __lcd_port=0x15;// use port c else PORTB=18;
#endasm              // see configuration in help LCD.h or in down 
#include<lcd.h>
#include<stdio.h>
#include<delay.h>
char lcd_buffer[33];
void main(void)
 { 
 
    	
while(1)
     	 {      lcd_init(16);
     	 		lcd_clear();
	 			sprintf(lcd_buffer,"100.5 Impact Fm\nMore Than Winer"); //u can write u caracter on "..UP..Max 16 carakter /n.....Down Max 16 caracter"
	       		lcd_puts(lcd_buffer);
	   			delay_ms(1000);
	   			lcd_clear();
         }

} 
/*
The LCD module must be connected to the port bits as follows:
[LCD]	[AVR Port]
RS (pin4) ------ 	bit 0
RD (pin 5) ------	bit 1
EN (pin 6) ------	bit 2
DB4 (pin 11) ---	bit 4
DB5 (pin 12) ---	bit 5
DB6 (pin 13) ---	bit 6
DB7 (pin 14) ---	bit 7

 */



