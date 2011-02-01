#include <avr/io.h>
#include "stdio.h"

void uart_putchar(char c);
char uart_getchar(void);
void uart_getstring(char string[]);
void uart_putstring(char string[]);


void uart_putchar(char c)
{
    //Puts a character on the serial stream
    
    while (!(UCSRA&(1<<UDRE)))
        ;
    UDR = c;
}

char uart_getchar(void)
{
    //Gets a character on the serial stream
    
    while (!(UCSRA&(1<<RXC)))
        ;
    return UDR;
}

void uart_getstring(char string[])
{
    int i = 0;
    char c;
    
    do
    {
        c = uart_getchar();
        string[i] = c;
        i++;
    } while (c != '\n');
}

void uart_putstring(char string[])
{
    int i = 0;
    char c;
    
    while (string[i] != 0)
    {
        c = string[i];
        uart_putchar(c);
        i++;
    }
}
