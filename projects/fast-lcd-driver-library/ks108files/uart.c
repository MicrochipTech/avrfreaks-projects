#include <avr/io.h>
#include "uart.h"

// UART Routinen
#define UART_UBRR_CALC(BAUD_,FREQ_) ((FREQ_)/((BAUD_)*16L)-1)

#define UART_BAUD_RATE 4800

void uart_init()
{

  //  UBRRL = 25;
  
  
  UBRRH = (uint8_t)( UART_UBRR_CALC( UART_BAUD_RATE, F_CPU ) >> 8 );
  UBRRL = (uint8_t)UART_UBRR_CALC( UART_BAUD_RATE, F_CPU );
 

  /* Enable UART receiver and transmitter */
  UCSRB = (1 << RXEN) | (1 << TXEN);
  
  /* 8 data bits, 1 stop bit */

//#if MCU == atmega8 || MCU == atmega32
  UCSRC = (1 << UCSZ1) | (1 << UCSZ0) | (1 << URSEL);
/*#elif MCU == attiny2313
  UCSRC = (1 << UCSZ1) | (1 << UCSZ0);

#else
  error "MCU not defined for uart.c"
#endif*/

    // Flush Receive-Buffer (entfernen evtl. vorhandener ungültiger Werte) 
    do
    {
        UDR;
    }
    while (UCSRA & (1 << RXC));

}


void uart_puts(char *s)
{
  uint8_t c;

  while((c=*s++) != 0)
    uart_putc(c);

}

void uart_putc (const uint8_t c)
{
    // Warten, bis UDR bereit ist für einen neuen Wert 
    while (!(UCSRA & (1 << UDRE)))
        ;

    // UDR Schreiben startet die Übertragung 
    UDR = c;


}

uint8_t uart_getc_wait()
{
    // Warten, bis etwas empfangen wird 
    while (!(UCSRA & (1 << RXC)))
        ;

    // Das empfangene Zeichen zurückliefern 
    return UDR;
}

int uart_getc_nowait()
{
    // Liefer das empfangene Zeichen, falls etwas empfangen wurde; -1 sonst 
    return (UCSRA & (1 << RXC)) ? (int) UDR : -1;
}
void Dump_Char(unsigned char x)
{
	unsigned char c;
	
        c = x;
        c = c >> 4;
		
		if ( c > 0x09 ){
			c -= 0x09;
			c |= 0x40;
			}
		else
			c |= 0x30;
		uart_putc(c);
		
		c = x;
		c &= 0x0f;
		
		if ( c > 0x09 ){
			c -= 0x09;
			c |= 0x40;
		}
		else
			c |= 0x30;
		uart_putc(c);
		uart_putc(0x20);
}

void Dump_Buffer(unsigned char *buf)
{
	unsigned char *bp;
	unsigned int i,b,z;
	
	bp = buf;
	for(i=0,z=0,b=0; i < 1024; i++ ){
 
		 
		Dump_Char(*bp++);
		
		if ( ++z > 15 ){
			uart_puts("\n\r");
				z = 0;
                if ( ++b > 3){
				uart_puts("-----\n\r");
				b=0;
            }
		}
		
	}
}


