#include <avr/interrupt.h>
#include <util/delay.h>

#include <uart_util.h>

#define UART_DEV 0
#define PACKET_LEN 16
#define UBRR_VAL 0
#define OK_IND_PORT PORTC
#define OK_IND_PORTD DDRC
#ifdef USE_USART
#define OK_IND_BIT PC5
#else
#define OK_IND_BIT PC0
#endif


#ifdef USE_USART
const uint8_t osc_callibration[] PROGMEM = { 0xA6, 0xA8, 0x9F, 0xA0 };
#endif

uint8_t charbuf[PACKET_LEN];

static void blink_forever(uint8_t blinks);
static void delay_ots(uint8_t oths);
static int send_receive(void);


/*******************************************************************************\
 *
\*******************************************************************************/
static void ioinit()
{
    /* set callibration value for 4MHz */
#ifdef USE_USART
	OSCCAL = pgm_read_byte(&osc_callibration[2]);
#endif
			
	/* try device limit e.g. 
	 * for 4Mhz ATmega8 it is 1 and for 4MHz AT90S8515 0 */
	UART_set_speed(UART_DEV, UBRR_VAL);

	/*
	 * Add here specific UART mode initialization eg. 8N1
	 */
#ifdef USE_USART
	UCSRB = 0;
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
#else
	UCR = 0;
#endif

	UART_init_RX(UART_DEV);
	UART_init_TX(UART_DEV);
	
	sei();
}

/*******************************************************************************\
 *
\*******************************************************************************/
int main(void)
{
	uint8_t i;

	ioinit();

	OK_IND_PORTD |= _BV(OK_IND_BIT);
	OK_IND_PORT &= ~_BV(OK_IND_BIT);

	/* patttern with zeros */
	for (i=0; i < PACKET_LEN; i++)
	{
		charbuf[i] = 0;
	}
	
	i = send_receive();
	if (i)
	{
		blink_forever(i+0);
	}

	/* patttern with ones */
	for (i=0; i < PACKET_LEN; i++)
	{
		charbuf[i] = 0xff;
	}
	
	i = send_receive();
	if (i)
	{
		blink_forever(i+10);
	}

	/* "checkboard" patttern */
	for (i=0; i < PACKET_LEN; i++)
	{
		charbuf[i] = 0xaa;
	}
	
	i = send_receive();
	if (i)
	{
		blink_forever(i+20);
	}

	/* different characters at each position */
	for (i=0; i < PACKET_LEN; i++)
	{
		charbuf[i] = 0xfe - i;
	}
	
	i = send_receive();
	if (i)
	{
		blink_forever(i+30);
	}

	/* All test passed - set 0V at indicator pin */
	blink_forever(0);

	return 0;
}

/*******************************************************************************\
 *
\*******************************************************************************/
static int send_receive(void)
{
	uint8_t i;
	uint8_t a_char;

	for (i=0; i < PACKET_LEN; i++)
	{
		if (!UART_putchar(UART_DEV, charbuf[i]))
		{
			return 1;
		}
	}

	while (UART_get_TX_len(UART_DEV) != 0)
	{
		_delay_loop_2(5000);
		i--;
		if (0 == i)
		{
			return 2;
		}
	}

	if (UART_get_RX_len(UART_DEV) != PACKET_LEN)
	{
		return 3;
	}
	
	for (i=0; i < PACKET_LEN; i++)
	{
		if (!UART_getchar(UART_DEV, &a_char))
		{
			return 4;
		}
		if (a_char != charbuf[i])
		{
			return 5;
		}
	}

	return 0;
}

/*******************************************************************************\
 *
\*******************************************************************************/
static void delay_ots(uint8_t oths)
{
	uint8_t i;

	for ( ; oths; oths--)
	{
		for (i=0; i < 100; i++)
		{
			_delay_loop_2(1000);
		}
	}
}

/*******************************************************************************\
 *
\*******************************************************************************/
static void blink_forever(uint8_t blinks)
{
	uint8_t i;

	/* wait a second before first blink */
	OK_IND_PORT &= ~_BV(OK_IND_BIT);
	delay_ots(10);

	for ( ;; )
	{
		for (i=0; i < blinks; i++)
		{
			OK_IND_PORT |= _BV(OK_IND_BIT);
			delay_ots(6);
			OK_IND_PORT &= ~_BV(OK_IND_BIT);
			delay_ots(3);
		}
		delay_ots(15);
	}
}

