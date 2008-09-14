//------------------------------------------------------------------------------------------------
// Program by virgoptrex (c) 2008
//target: Atmega 48/168
// Sharp's GP2D02 are hard to find . But you may experiment with those lying in your stock. You may use this code freely but kindly retain the credits!!!
// Yet to be linearized

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "lcd_primary.c" //ONLY for LCD functions - any LCD library on the internet may be selected instead of this one.
int main()
{
char reading = 0;
LCD_init();
LCD_cursor_blink();
LCD_string("GP2D02 Digit IR",1,1); // string, line position /
while(1){
DDRB |= (1 << 2); // THIS IS "VIN". O/P (one with diode)
DDRB |= (0 << 3); // THIS IS "VOUT". I/P
PORTB &= ~_BV(2); //bringing Vin (pin2) low
asm volatile("nop\n\t"
"nop\n\t"
"nop\n\t"
::);
//while((PINB & _BV(3)) == 0); //wait until Vout pin is high
loop_until_bit_is_set(PINB, 3);
PORTB|=_BV(2);// ON
asm volatile("nop\n\t"
"nop\n\t"
"nop\n\t"
::);
for (uint8_t x = 0; x < 8; x++) //start loop(runs 8 times)
{
PORTB &= ~_BV(2); //bringing Vin (pin2) low
asm volatile("nop\n\t"
"nop\n\t"
::);
if (bit_is_set(PINB,3))
{reading |=(1<
PORTB |= _BV(2); //bringing Vin (pin2) high
asm volatile("nop\n\t"
"nop\n\t"
::);
}
//Fcm = 1590/(reading-65);
LCD_string_Int(reading,2,1); //sent to LCD display panel
reading = 0;
_delay_ms(50);
}
}
//------------------------------------------------------------
