#include <avr/interrupt.h>

#include <uart_util.h>

const char str_READY[] PROGMEM = "\r\n\nAVR U(S)ART library \"echo\" example.\r\nReady yo go!\r\n\n";

/* In case when AVR without external clock is used we should calibrate
 * the internal oscillator to obtain accurate transmission speed.
 * You should put here values from the AVR chip you will use for 
 * executing this program. */
#ifdef USE_USART
const uint8_t osc_callibration[] PROGMEM = { 0xA6, 0xA8, 0x9F, 0xA0 };
#endif

/* in case of AVR with 2 USARTs define which one you want to use */
#define UART_DEV 0

/*******************************************************************************/
void ioinit(void)
{
    /* set callibration value for 4MHz */
#ifdef USE_USART
	OSCCAL = pgm_read_byte(&osc_callibration[2]);
#endif
			
	/* set 19200 baud rate for 4Mhz CPU clock */
	UART_set_speed(UART_DEV, 12U);

    /* setup 8N1 asynchronous mode for U(S)ART */
#ifdef USE_USART
	UCSRB = 0;
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
#else
	UCR = 0;
#endif

    /* Initialize U(S)ART library */
    UART_init_RX(UART_DEV);
	UART_init_TX(UART_DEV);
	
    /* enable interrupts to allow the U(S)ART library processing 
     * incoming and outgoing transmission */    
	sei();
}

/*******************************************************************************/
int main(void)
{
    uint8_t ch;

    /* setup USART */
    ioinit();

    /* output welcome message */
    UART_puts_P(UART_DEV, str_READY);

    /* Just echo received bytes */
    while (1)
    {
        /* wait for a character */
        while (0 == UART_getchar(UART_DEV, &ch))
            ;

        /* send back received character */
        UART_putchar(UART_DEV, ch);
        
        /* if end of line (Enter) detected then output also a CR character */
        if ('\r' == ch)
        {
            UART_putchar(UART_DEV, '\n');
        }
    }

    return (0);
}

