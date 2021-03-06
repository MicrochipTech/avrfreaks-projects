#include "usart.h"

void inisialisasiUSART()
{
    // USART0 initialization
    // Communication Parameters: 8 Data, 2 Stop, No Parity
    // USART0 Receiver: On
    // USART0 Transmitter: On
    // USART0 Mode: Asynchronous
    // USART0 Baud Rate: 2400
    UCSR0A=0x00;
    UCSR0B=0x98;
    UCSR0C=0x8E;
    UBRR0H=0x01;
    UBRR0L=0x1F;
    
    // USART1 initialization
    // Communication Parameters: 8 Data, 2 Stop, No Parity
    // USART1 Receiver: Off
    // USART1 Transmitter: On
    // USART1 Mode: Asynchronous
    // USART1 Baud Rate: 2400
    UCSR1A=0x00;
    UCSR1B=0x08;
    UCSR1C=0x8E;
    UBRR1H=0x01;
    UBRR1L=0x1F;
}

void kirimDataSerialIR(unsigned char data)
{
    //Wait for empty transmit buffer
    while (!(UCSR0A & (1<<UDRE)));
    UDR0=data;
} 

//Pengiriman data serial untuk debugging program
void kirimDataSerial(unsigned char data)
{
    //Wait for empty transmit buffer
    while (!(UCSR1A & (1<<UDRE)));
    UDR1=data;
} 
