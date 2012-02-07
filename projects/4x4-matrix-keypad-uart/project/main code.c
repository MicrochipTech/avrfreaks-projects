/*
 *created by Allard Mol
 */

#include <avr/io.h>
#include <avr/delay.h>
#define time 200
#define F_CPU   (1000000UL)

void          USART_vInit (void);
int           USART_iPutChar  (uint8_t c);
void          USART_vPutString (char *p);

int main(void)
{
		USART_vInit();
		DDRA=0xFF;
		DDRC=0x00;  
		while(1)
		{
   
		 PORTA=0x01;
		if ((PINC & (1<<PC7)) != 0)
		 {		
		 USART_vPutString("A\n");
		 _delay_ms(time);
		 }
		 if ((PINC & (1<<PC4)) != 0 )
		 {		
		 USART_vPutString("D\n");
		 _delay_ms(time);
		 }
		 if ((PINC & (1<<PC5)) != 0)
		 {		
		 USART_vPutString("C\n");
		 _delay_ms(time);
		 }
		
		 if ((PINC & (1<<PC6)) != 0)
		 {		
		 USART_vPutString("B\n");
		 _delay_ms(time);
		 }
	
			 PORTA=0x02;
		 if ((PINC & (1<<PC4)) != 0)
		 {		
		 USART_vPutString("#\n");
		 _delay_ms(time);
		 }
		 if ((PINC & (1<<PC5)) != 0)
		 {		
		 USART_vPutString("9\n");
		 _delay_ms(time);
		 }
		 if ((PINC & (1<<PC6)) != 0)
		 {		
		 USART_vPutString("6\n");
		 _delay_ms(time);
		 }
		 if ((PINC & (1<<PC7)) != 0)
		 {		
		 USART_vPutString("3\n");
		 _delay_ms(time);
		 }
		
			 PORTA=0x04;
		 if ((PINC & (1<<PC7)) != 0)
		 {		
		 USART_vPutString("2\n");
		 _delay_ms(time);
		 }
		 if ((PINC & (1<<PC4)) != 0)
		 {		
		 USART_vPutString("0\n");
		 _delay_ms(time);
		}
		 if ((PINC & (1<<PC5)) != 0)
		 {		
		 USART_vPutString("8\n");
		 _delay_ms(time);
		 }
		 if ((PINC & (1<<PC6)) != 0)
		 {		
		 USART_vPutString("5\n");
		 _delay_ms(time);
		 }
	
		PORTA=0x08;
		if ((PINC & (1<<PC7)) != 0)
		 {		
		 USART_vPutString("1\n");
		 _delay_ms(time);
		 }
		 if ((PINC & (1<<PC4)) != 0)
		 {		
		 USART_vPutString("*\n");
		 _delay_ms(time);
		 }
		 if ((PINC & (1<<PC5)) != 0)
		 {		
		 USART_vPutString("7\n");
		 _delay_ms(time);
		 }
		 if ((PINC & (1<<PC6)) != 0)
		 {		
		 USART_vPutString("4\n");
		 _delay_ms(time);
		 }
	 }
}

void USART_vInit (void)
{ 

  UCSRA = (1<<U2X);
  UCSRB = (1<<TXEN);
  UBRRL = (F_CPU / (8 * 9600UL)) - 1;
}
int USART_iPutChar (uint8_t c)
{
  while ((UCSRA & (1<<UDRE)) == 0) {;}
  UDR = c;
  return (c);
}

void USART_vPutString (char *p)
{
  while(*p)
  {
    if(*p == '\b') {USART_iPutChar('\r');}
    USART_iPutChar(*p++);
  }
}
