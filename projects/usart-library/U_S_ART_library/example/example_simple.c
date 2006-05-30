#include <avr/interrupt.h>

#include <uart_util.h>

const char str_OK[] PROGMEM = "\r\nOK\r\n";
const char str_READY[] PROGMEM = "\r\n\nAVR U(S)ART library example.\r\nReady yo go!\r\n\n";
const char str_ENTER[] PROGMEM = "\r\nEnter string";
const char str_ENDL[] PROGMEM = " and press Enter\r\n";
const char str_LENGTH[] PROGMEM = " up to 20 characters long\r\n";

const char str_TEST1[] PROGMEM = "\r\nTest 1: UART_getchar and UART_putchar";
const char str_TEST2[] PROGMEM = "\r\nTest 2: UART_gets_delim and UART_puts";
const char str_TEST3[] PROGMEM = "\r\nTest 3: UART_gets and UART_puts";

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
    uint8_t i;
    uint8_t charbuf[21];

    /* setup USART */
    ioinit();

    /* output welcome message */
    UART_puts_P(UART_DEV, str_READY);

    while (1)
    {
        UART_puts_P(UART_DEV, str_TEST1);
        UART_puts_P(UART_DEV, str_ENTER);
        UART_puts_P(UART_DEV, str_ENDL);

        UART_clear_RX_buf(UART_DEV);

        /* read characters from UART untill charbuf is full or end of line (Enter) */
        for (i=0; i < sizeof(charbuf); i++)
        {
            while (0 == UART_get_RX_len(UART_DEV))
                ;
            if (UART_getchar(UART_DEV, &charbuf[i]))
            {
                if ( charbuf[i] == '\r' )
                    break;
                UART_putchar(UART_DEV, charbuf[i]);
            }
        }
        /* send back received string */
        UART_puts_P(UART_DEV, str_OK);

        UART_puts_P(UART_DEV, str_TEST2);
        UART_puts_P(UART_DEV, str_ENTER);
        UART_puts_P(UART_DEV, str_ENDL);

        UART_clear_RX_buf(UART_DEV);

        /* the same functionality like above but using the library function */
        i = UART_gets_delim(UART_DEV, charbuf, '\r', sizeof(charbuf)-1);
        charbuf[i] = '\0';
        UART_puts(UART_DEV, charbuf);
        UART_puts_P(UART_DEV, str_OK);

        UART_puts_P(UART_DEV, str_TEST3);
        UART_puts_P(UART_DEV, str_ENTER);
        UART_puts_P(UART_DEV, str_LENGTH);

        UART_clear_RX_buf(UART_DEV);

        /* fill the charbuf array using characters received through UART */
        UART_gets(UART_DEV, charbuf, sizeof(charbuf)-1);
        charbuf[sizeof(charbuf)-1] = '\0';
        UART_puts(UART_DEV, charbuf);
        UART_puts_P(UART_DEV ,str_OK);
    }

    return (0);
}

