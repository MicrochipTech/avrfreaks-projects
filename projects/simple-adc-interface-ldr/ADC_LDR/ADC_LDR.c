//****************************************************
// Target: ATMEGA48
// Code by virgoptrex (c) 2008
// Read ADC from ATMEGA48 datasheet for more understanding on ADC operation
// Note: while porting this code to atmega128 you may need to take into account ADFR (free running) in ADCSRA.
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "lcd_primary.c" // ONLY required for LCD
int main (void)
{
DDRB |= (1 << 1); // Set LED1 as output
DDRB |= (1 << 0); // Set LED2 as output
// LCD_init(); // ONLY required for LCD
// LCD_cursor_blink(); // ONLY required for LCD
// LCD_string("LDR value",1,1); // string, line position // ONLY required for
LCD
_delay_ms(100);
ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC
prescaler to 128 - 125KHz sample rate @ 16MHz
ADMUX |= (1 << REFS0)| (0 << REFS1); // Set ADC reference to AVCC
ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit
reading
while(1)
{
ADMUX |= (1 << MUX1)|(1 << MUX0); // using ADC3 pin
// No MUX values needed to be changed to use ADC0
ADCSRA |= (1 << ADEN); // Enable ADC
ADCSRA |= (1 << ADIE); // Enable ADC Interrupt
sei(); // Enable Global Interrupts
ADCSRA |= (1 << ADSC); // Start A2D Conversions
}
}
ISR(ADC_vect)
{
uint16_t ADCtemp;
ADCtemp = ADCL;
/*shift from low level to high level ADC, from 8bit to 10bit*/
ADCtemp += (ADCH<<8);
//LCD_string_Int(ADCtemp,2,1); //sent to LCD display panel if (ADCH < 128) 
// max ADCH value is 2^8 = 256 hence half of it. I am setting LDAR to one. Hence in comparison using ADCH else if LDAR is zero use lower byte ADCL
{
PORTB |= (1 << 1); // Turn on LED1
PORTB &= ~(1 << 0); // Turn off LED2
}
else
{
PORTB &= ~(1 << 1); // Turn off LED1
PORTB |= (1 << 0); // Turn on LED2
}
}
//***************************************************************
//---------------------------------------------------------------