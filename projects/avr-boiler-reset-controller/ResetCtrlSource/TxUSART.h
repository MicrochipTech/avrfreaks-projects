/* ========================================================================== */
/*   TxUSART.h                                                                */
/*   Description                                                              */
/*    Transmit Only Serial Communication via USART                            */
/* ========================================================================== */
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 8000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

// Buffer sizes must be 2^n
//
#define TBUFSIZE	64
#define TMASK		(TBUFSIZE-1)

// Function prototypes
//
void USART_Init(void);
int USART_putchar(char c);
char tbuflen(void);

// Static variables
//
volatile unsigned char tbuf[TBUFSIZE];  // TX buffer

volatile unsigned char t_in;            // TX buffer in index
volatile unsigned char t_out;           // TX buffer out index

