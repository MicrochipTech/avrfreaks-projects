# uart in atmega 64

Uploaded by foxyforever on 2013-01-07 06:49:00 (rating 0 out of 5)

## Summary

#include   

#include   

#include   

#include 


/* USART0 Control and Status Register C - UCSR0C */  

#define UMSEL0 6  

#define UPM01 5  

#define UPM00 4  

#define USBS0 3  

#define UCSZ01 2  

#define UCSZ00 1  

#define UCPOL0 0


/* USART1 Control and Status Register B - UCSR1B */  

#define RXCIE1 7  

#define TXCIE1 6  

#define UDRIE1 5  

#define RXEN1 4  

#define TXEN1 3  

#define UCSZ12 2  

#define RXB81 1  

#define TXB81 0


/* USART1 Control and Status Register A - UCSR1A */  

#define RXC1 7  

#define TXC1 6  

#define UDRE1 5  

#define FE1 4  

#define DOR1 3  

#define UPE1 2  

#define U2X1 1  

#define MPCM1 0


/* USART1 Control and Status Register C - UCSR1C */  

#define UMSEL1 6  

#define UPM11 5  

#define UPM10 4  

#define USBS1 3  

#define UCSZ11 2  

#define UCSZ10 1  

#define UCPOL1 0


/* USART0 Control and Status Register B - UCSR0B */  

#define RXCIE0 7  

#define TXCIE0 6  

#define UDRIE0 5  

#define RXEN0 4  

#define TXEN0 3  

#define UCSZ02 2  

#define RXB80 1  

#define TXB80 0  

#define UDRE1 5


/* USART0 Control and Status Register A - UCSR0A */  

#define RXC0 7  

#define TXC0 6  

#define UDRE0 5  

#define FE0 4  

#define DOR0 3  

#define UPE0 2  

#define U2X0 1  

#define MPCM0 0


#define UDRE0 5


#define UCSZ01 2  

#define UCSZ00 1


#define TXC0 6


void putchar (char mania);


/*  

void uart1\_init(void)  

{  

 UCSR1B = 0x00; //disable while setting baud rate  

 UCSR1A = 0x00;  

 UCSR1C = 0x06;  

 UBRR1L = 0x19; //set baud rate lo  

 UBRR1H = 0x00; //set baud rate hi  

 UCSR1B = 0xD8;  

}  

*/  

void uart1\_init(void)  

{  

 UCSR0B = 0x00; //disable while setting baud rate  

 UCSR0A = 0x00;  

 UCSR0C = 0x26;  

 UBRR0L = 0x19; //set baud rate lo  

 UBRR0H = 0x00; //set baud rate hi  

 UCSR0B = 0x98;  

}  

void main (void)  

{


UCSR0A=0x00;  

UCSR0B = (1<<RXEN0)|(1<<TXEN0); //|(1<<RXCIE0)|(1<<TXCIE0); // Enable rx/tx, rx/tx ints  

UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // no parity, 1 stop bit, 8 data bits 


UBRR0H=0x00;  

UBRR0L=0x33;


 while (1)  

 {  

 putchar('q');  

 }  

}  

void putchar (char mania)  

{  

 while ( !( UCSR0A & (1<<UDRE0)) );  

 UDR0 = mania;  

}

## Compilers

- IAR Embedded Workbench for AVR

## Tags

- Complete code
- IAR Embedded Workbench for AVR
