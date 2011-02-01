/* WirelessPressureSensor.c 1/26/2011
This program uses a 12bit A/D converter using SPI
to sample absolute pressure and transmits it wirelessly using
an Xbee
*/
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"
#include "uart.h"
#include "adc.h"

#define SAMPLE_RATE 0.5 //Hz
#define BAUD 57600
#define MYUBRR F_CPU/16/BAUD-1

void io_init(void);
void uart_init(unsigned int baud);

int main(void)
/*  Main Program */
{
    double period;
    unsigned int sample;
    period = (1.0/SAMPLE_RATE)*1000.0; //Sampling period in ms

    io_init();

    while (1)
    {
        _delay_ms(period);

        sample = read_sample();

        uart_putchar(126); //Framing Byte
        uart_putchar(sample>>8); //send first 4 bits
        uart_putchar(sample&0xFF); //send rest of bits
        uart_putchar(126); //Framing Byte

    }

    return 1;
}

void io_init(void)
/* Initialize IO ports */
{
    CS_HIGH;
    spi_master_init();  //Initialize SPI
    uart_init(MYUBRR); //Initialize UART

}

void uart_init(unsigned int baud)
{
    
    //Set baud rate
    UBRRH = (unsigned char)(baud>>8);
    UBRRL = (unsigned char)baud;
    //Enable receiver and transmitter
    UCSRB = (1<<RXEN)|(1<<TXEN);

    //Set frame format: 8data, 1stop bit
    UCSRC = (1<<URSEL)|(3<<UCSZ0)|(1<<UCSZ1);
}
