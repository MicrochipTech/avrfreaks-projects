#include "uart.h"
#include <avr/io.h>
#include <avr/pgmspace.h>

void inicializar_uart(void)
{
    UCSR1B = (1<<RXEN1)|(1<<TXEN1); // enable rx, tx
    UCSR1C = (1<<UCSZ11)|(1<<UCSZ10); // frame 8, N, 1 
    UBRR1H = (UART_UBRR >> 8); 
    UBRR1L = (UART_UBRR & 0xff); 
}

void escribir_uart(char *s)
{
    while (*s)
    {
        caracter_uart(*s++);
    }
    caracter_uart('\r'); 
}

void escribir_uart_P(const char *s) 
{
    while (pgm_read_byte(s) != 0x00) 
    {   
        caracter_uart(pgm_read_byte(s++));
    }   
    caracter_uart('\r');
}

void caracter_uart(char car)
{
    while ((UCSR1A & (1 << UDRE1)) == 0) {}
    UDR1 = (car);
}

void leer_uart(char *serial)
{
    int cont, fin;
    for (cont=0, fin=0; fin<2; cont++)
    {
        while ((UCSR1A & (1 << RXC1)) == 0) {}
        serial[cont] = UDR1;
        if (serial[cont] == '\n')
            fin++;
    }
}

void leer_socket(char *accion, char *velocidad)
{
    int cont;
    for (cont=0; cont<3; cont++)
    {
        while ((UCSR1A & (1 << RXC1)) == 0) {}
        if (cont == 0)
            *accion = UDR1;
        else
            velocidad[cont-1] = UDR1;
    }
}
