#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>
#include <string.h>

#include <uart_util.h>

const char str_READY[] PROGMEM = 
    "\r\n\nAVR U(S)ART library & standard IO example.\r\nReady yo go!\r\n\n";

const char str_ENTER[] PROGMEM =
    "\r\nEnter string (max 10 characters) and press Enter:\r";

const char printf_fmt[] PROGMEM = "\r\n\nReceived %d bytes\n\r%s\n\r";

/* In case when AVR without external clock is used we should calibrate
 * the internal oscillator to obtain accurate transmission speed.
 * You should put here values from the AVR chip you will use for 
 * executing this program. */
#ifdef USE_USART
const uint8_t osc_callibration[] PROGMEM = { 0xA6, 0xA8, 0x9F, 0xA0 };
#endif

/* in case of AVR with 2 USARTs define which one you want to use */
#define UART_DEV 0

/*******************************************************************************
 * Below two functions are needed for compatibility with standard IO facilities
 *******************************************************************************/

static uint8_t stdio_device;

/* this function will wait (block) until there is space in output buffer */
static int u_put(char a_char, FILE *stream)
{
	while (0 == UART_putchar(*(uint8_t*) fdev_get_udata(stream), a_char))
        ;
    return 0;
}

static int u_get(FILE *stream)
{
    UART_sleep_RX(*(uint8_t*) fdev_get_udata(stream));
	return UART_get(*(uint8_t*) fdev_get_udata(stream));
}

static FILE instream = FDEV_SETUP_STREAM(NULL, u_get, _FDEV_SETUP_READ);
static FILE outstream = FDEV_SETUP_STREAM(u_put, NULL, _FDEV_SETUP_WRITE);
static FILE errstream = FDEV_SETUP_STREAM(u_put, NULL, _FDEV_SETUP_WRITE);

/*******************************************************************************/
void ioinit(void)
{
    /* set callibration value for 4MHz */
#ifdef USE_USART
	OSCCAL = pgm_read_byte(&osc_callibration[2]);
#endif
			
	/* set 19200 baud rate for 4Mhz CPU clock */
	UART_set_speed(UART_DEV, 12U);

    /* setup 8N1 serual asynchronous mode for U(S)ART */
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
    int ch;
    uint8_t i;
    uint8_t charbuf[11];

    /* setup USART */
    ioinit();

    /* prepare standard IO facilities */
    stdio_device = UART_DEV;
    stdin = &instream;
    stdout = &outstream;
    stderr = &errstream;
    fdev_set_udata(stdin, &stdio_device);
    fdev_set_udata(stdout, &stdio_device);
    fdev_set_udata(stderr, &stdio_device);

    set_sleep_mode(SLEEP_MODE_IDLE);

    /* output welcome message */
    puts_P(str_READY);

    while (1)
    {
        /* output prompt */
        puts_P(str_ENTER);
        
        /* read a line form the standard input into the charbuf array 
         * this is a similar to standard gets function but with protection against
         * buffer overflow */
        for (i=0; i < (sizeof(charbuf)-1); i++)
        {
            /* read a byte from standard input */
            do {
                ch = getchar();
            } while (EOF == ch);

            /* if end of line found then stop reading */
            if ((uint8_t) ch == '\r')
            {
                break;
            }

            /* echo received character */
            putchar(ch);
            
            /* store just received byte into array */
            charbuf[i] = ch;
        }

        /* end received string with NULL byte to allow use it in 
         * the strlen and printf functions */
        charbuf[i] = '\0';
        
        /* output the whole received line with some information */
        printf_P(printf_fmt, strlen((char*) charbuf), charbuf);

        /* wait until output buffer is empty - transmission is finished */
        //UART_sleep_TX_empty(UART_DEV);
        UART_wait_TX_empty(UART_DEV);
    }

    return (0);
}

