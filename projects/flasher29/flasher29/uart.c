#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000L
#define BAUD 115200
#include <stdint.h>
#include <stdio.h>
#include "flash.h"

uint8_t in_buf[257];
volatile uint8_t pos;
volatile uint8_t terminator;
void dummy(uint16_t len,uint8_t *buf);
void (*uart_cb)(uint16_t len,uint8_t *buf)=dummy;
uint16_t limit=256;
void putch(char c);
static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL,_FDEV_SETUP_WRITE);

static int uart_putchar(char c, FILE *stream)
{
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
	return 0;
}



ISR(USART_RXC_vect)
{
	uint16_t c;
//	led_on;
	c=UDR;
	in_buf[pos++]=c;
	if((c==terminator) && (c!=255)){ uart_cb(pos,in_buf); pos=0; }
	else
	if(pos==256) { uart_cb(pos,in_buf);  pos=0; }
	else
	if (pos==limit) { uart_cb(pos,in_buf);  pos=0; }
//	led_off;
	
}

void set_limit(uint8_t lim, char term)
{
		limit=lim;
		terminator=term;
}

void print_adr(uint32_t adr)
{
	uint8_t digit[16],digits;
	uint32_t rest,tmp;

	rest=adr;
	digits=0;
	while (rest>0)
	{
		tmp=rest;
		rest=rest/10;
		digit[digits++]=tmp % 10;
	}
	if (digits==0)
	{
		putch('0');
		return;
	}
	while(digits)
	{
		putch('0'+digit[--digits]);
	}
}


void uart_init(void (*uart_callback)(uint16_t len, uint8_t *buf))
{
	pos=0;
	terminator='\n';
	uart_cb=uart_callback;
	limit=256;
    UCSRB=0x98;
    UCSRC=0x06; //8-бит| 1 стоп| без паритета
#include <util/setbaud.h>
   UBRRH = UBRRH_VALUE;
   UBRRL = UBRRL_VALUE;
#if USE_2X
   UCSRA |= (1 << U2X);
#else
   UCSRA &= ~(1 << U2X);
#endif
	stdout=&mystdout;
}

void putch(char c)
{
     loop_until_bit_is_set(UCSRA, UDRE);
     UDR = c;
}

void dummy(uint16_t len, uint8_t *buf) // len - кол-во принятых байт
{
}


