/////////////////////////////////////////
//
// File : uart.h
//
// v1.1         4/06 stevech@san.rr.com
// v2.0         2012, updated to V2.0 by Kun.io


// Defines for ATmega8.
// These translate the new names to the older names of the ATmega8.
// Only tested for serial communication with 8 databits, no parity, 1 stopbit.
#if defined (__AVR_ATmega8__)
#define UBRR0H UBRRH
#define UBRR0L UBRRL
#define UCSR0A UCSRA
#define UCSR0B UCSRB
#define UCSR0C UCSRC
#define RXCIE0 RXCIE
#define TXEN0 TXEN
#define RXEN0 RXEN
#define UCSZ00 UCSZ0
#define UCSZ01 UCSZ1
#define UCSZ02 UCSZ2
#define UDRIE0 UDRIE
#define UDR0 UDR
#define FE0 FE
#define UDRE0 UDRE
#define U2X0 U2X
#define USART_RX_vect USART_RXC_vect
#endif

// Defines for AT90USB1286 (Teensy Board)
// These translate the names for USART0 to USART1, 
// since the AT90USB1286 doesn't have a USART0.
#if defined (__AVR_AT90USB1286__)
#define UBRR0H UBRR1H
#define UBRR0L UBRR1L
#define UCSR0A UCSR1A
#define UCSR0B UCSR1B
#define UCSR0C UCSR1C
#define RXCIE0 RXCIE1
#define TXEN0 TXEN1
#define RXEN0 RXEN1
#define UCSZ00 UCSZ10
#define UCSZ01 UCSZ11
#define UCSZ02 UCSZ12
#define UDRIE0 UDRIE1
#define UDR0 UDR1
#define FE0 FE1
#define UDRE0 UDRE1
#define U2X0 U2X1
#define USART_TX_vect USART1_TX_vect
#define USART_RX_vect USART1_RX_vect
#define USART_UDRE_vect USART1_UDRE_vect
#endif


// Value for baudrate.
// Using this calculation:
//    UBRR = ( system_clock / (16 * baudrate)) - 1
// Google can be used for the calculation, like this: http://www.google.com/search?q=%2816000000%2F%2816+*+9600%29%29+-1
//
// Use the BAUDRATE and F_CPU (or include a file with F_CPU in uart.c)
// Or set the UBBR_VALUE directly to a value.
#define BAUDRATE 9600
#define UBBR_VALUE ((F_CPU / (16UL * BAUDRATE)) -1)


// Buffersizes must be a power of 2 in size !
//    This is for very simple code for a circular buffer.
// Buffersizes are in 8 bits, set it to 8, 16, 32, 64 or 128.
//    This is for very simple code without the need for atomic programming.
// The buffer is used for speed. Characters are send in an interrupt routine.
// If more characters are send than the buffer could contain, the functions waits for a place in the buffer.
// The receiving of characters does not handle overflow very well, make the buffers large enough.
#define TBUFSIZE 64
#define RBUFSIZE 64


// functions in UART.c

void UART_init(void);               // initialize UART0

void UART_putc(unsigned char c);    // Put ASCII or non-ASCII byte, blocks (waits) if buffer is full
void UART_puts(char *p);            // Print string from RAM
void UART_puts_P(const char *p);    // Print string from FLASH MEMORY

unsigned char UART_getc(void);      // Get ASCII or non-ASCII byte, blocks (waits) if none available. See UART_rbuflen()
int UART_gets(char *p, int max);    // Get a string with autodetect of CR,LF,CRLF, but subject to max chars.

unsigned char UART_tbuflen(void);   // Get number of as yet untransmitted bytes.
unsigned char UART_rbuflen(void);   // Gget number of bytes in the receive buffer or zero.
