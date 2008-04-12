//Includes der nötigen Libarys 
#include <stdlib.h>
#include <avr/io.h>                    
#include <util/delay.h>                
#include <avr/interrupt.h>
#include "lcd-routines.h"

uint8_t A;
uint16_t Zeit;
uint16_t Zufall;
uint64_t Timer1;
uint16_t Timer1a;
char ASCII[50];


void Start_Mess()
{
  Zufall = rand()%900;
  _delay_ms(Zufall);
  PORTB = 254; 
  TCNT1 = 0;
  TCCR1B |= (1<<CS12);
}

void End_Mess()
{
  Timer1= TCNT1;
  Timer1a = (153ul * Timer1) / 100000;
  PORTB = 0xFF;
  lcd_clear();
  lcd_string(ultoa(Timer1a, ASCII, 10));
  lcd_string("ms");
  set_cursor(0,2);
  lcd_string(utoa(Timer1, ASCII, 10));
}

ISR (ADC_vect)
{
  Zeit++;
}


//Hauptschleife
int main(void)
{    
  lcd_init();
  sei();
  DDRB = 0xFF;
  PORTB = 0xFF;
  while(1)
  {
    if(!(PIND & (1<<PD1)) && (A == 0))
	{
	  A = 1;
	  Start_Mess();
	}
	if(!(PIND & (1<<PD0)) && (A == 1))
	{
	  A = 0;
	  End_Mess();
	}
  }
  return(0);
}

//Min = 800
//MAX = 915
//900 = 1ms
//770 = 2ms
