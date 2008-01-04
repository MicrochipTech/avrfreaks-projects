#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd_lib.h"
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

char CopyIntegertoLCD(int number, unsigned char x, unsigned char y);
char CopyDoubletoLCD(double number, unsigned char x, unsigned char y);
void LCDclrSection(unsigned char x, unsigned char y, unsigned char length);

const uint8_t LCDwelcomeln1[] PROGMEM="Temperature:\0";
char lastoff = 0;
const char AVGLEN = 25;
unsigned char avg[25];
char units = 0;
char btndown = 0;

int main (void)
{
   char i;
   for(i = 0; i < AVGLEN; i++)
      avg[i] = 0;

   DDRB = 0;
   ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); 
   ADMUX |= (1 << REFS0); 
   ADMUX |= (1 << ADLAR); 
   ADCSRA |= (1 << ADFR); 
   ADCSRA |= (1 << ADEN); 
   ADCSRA |= (1 << ADSC); 

   LCDinit();
   LCDclr();
   LCDcursorOFF();
   CopyStringtoLCD(LCDwelcomeln1, 0, 0);

   for(;;)
   {

	  if(~PINB & (1<<5))
	  {
	     if(btndown == 0)
		 {
	        units++;
		    if(units == 3)
		       units = 0;
		 }
         btndown = 1;
	  }
	  else
	     btndown = 0;
	  int i;
	  for(i = AVGLEN-1; i > 0; i--)
	     avg[i] = avg[i-1];
	  avg[0] = ADCH;
	  double total = 0;
	  for(i = 0; i < AVGLEN; i++)
	     total += avg[i];
      total *= .04;
	  double number;
	  if(units == 0)
	     number = 0.723*total-24.846;
	  if(units == 1)
	     number = 0.401*total-31.549;
	  if(units == 2)
	     number = 0.401*total+241.65;
	  number = 0.1 * floor( number * 10.0 );
	  char offset = CopyDoubletoLCD(number, 0, 1);
	  if(offset < lastoff)
	     LCDclrSection(offset, 1, lastoff);
	  lastoff = offset;
	  if(units != 2)
	  {
	     LCDGotoXY(offset, 1);
	     LCDsendChar(223);
	  }
	  if(units == 0)
	  {
	     LCDGotoXY(offset+1, 1);
	     LCDsendChar(70);
	  }
	  if(units == 1)
	  {
	     LCDGotoXY(offset+1, 1);
	     LCDsendChar(67);
	  }
	  if(units == 2)
	  {
	     LCDGotoXY(offset, 1);
	     LCDsendChar(75);
	  }
   }

} 

char CopyIntegertoLCD(int number, unsigned char x, unsigned char y)
{
    char len = 0;
	char buffer [7];
	char* p = &buffer[0];
	itoa (number,buffer,10);  // itoa(number,p,10) is okay, too
	char c; 
	while ((c = *p))
	{
		LCDGotoXY(x++, y);
		LCDsendChar(c);
		p++;
		len++;
	}
	return len;
}
char CopyDoubletoLCD(double number, unsigned char x, unsigned char y)
{
	char len = 0;
	char buffer [10];
	char* p = &buffer[0];
	sprintf (buffer, "%.3g", number);
	//dtostrf(number, 4, 2, buffer);
	char c; 
	while ((c = *p))
	{
		LCDGotoXY(x++, y);
		LCDsendChar(c);
		p++;
		len++;
	}
	return len;
 }

void LCDclrSection(unsigned char x, unsigned char y, unsigned char length)
{
	int i;
	for(i = 0; i < length; i++)
	{
		LCDGotoXY(x + i, y);
		LCDsendChar(32);
	}
}
