/*
 * usart.c
 * libavrutil
 *
 * Created by Árpád Goretity on 18/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <stdarg.h>
#include <stdio.h>
#include "usart.h"


void avr_usart_init(uint8_t channel, uint32_t baud)
{
	/* Calculate the baudrate */
	uint16_t baud_prscl = F_CPU / (16 * baud) - 1;
	
#if defined(UCSRB)
	if (channel == AVR_USART_CH0)
	{
		/* Set the prescaler properly */
		UBRRH = (uint8_t)((baud_prscl >> 8) & 0xff);
		UBRRL = (uint8_t)(baud_prscl & 0xff);
		/* Enable the receiver and transmitter circuits */
		UCSRB = _BV(RXEN) | _BV(TXEN);
		/* 8 bit data frames, 1 stop bit, no parity bit, asynchronous mode */
		UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
	}
#else /* UCSRB */
#if defined(UCSR0B)
	if (channel == AVR_USART_CH0)
	{
		/* Set the prescaler properly */
		UBRR0H = (uint8_t)((baud_prscl >> 8) & 0xff);
		UBRR0L = (uint8_t)(baud_prscl & 0xff);
		/* Enable the receiver and transmitter circuits */
		UCSR0B = _BV(RXEN0) | _BV(TXEN0);
		/* 8 bit data frames, 1 stop bit, no parity bit, asynchronous mode */
		UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	}
#endif /* UCSR0B */
#if defined(UCSR1B)
	if (channel == AVR_USART_CH1)
	{
		/* Set the prescaler properly */
		UBRR1H = (uint8_t)((baud_prscl >> 8) & 0xff);
		UBRR1L = (uint8_t)(baud_prscl & 0xff);
		/* Enable the receiver and transmitter circuits */
		UCSR1B = _BV(RXEN1) | _BV(TXEN1);
		/* 8 bit data frames, 1 stop bit, no parity bit, asynchronous mode */
		UCSR1C = _BV(UCSZ11) | _BV(UCSZ10);
	}
#endif /* UCSR1B */
#if defined(UCSR2B)
	if (channel == AVR_USART_CH2)
	{
		/* Set the prescaler properly */
		UBRR2H = (uint8_t)((baud_prscl >> 8) & 0xff);
		UBRR2L = (uint8_t)(baud_prscl & 0xff);
		/* Enable the receiver and transmitter circuits */
		UCSR2B = _BV(RXEN2) | _BV(TXEN2);
		/* 8 bit data frames, 1 stop bit, no parity bit, asynchronous mode */
		UCSR2C = _BV(UCSZ21) | _BV(UCSZ20);
	}
#endif /* UCSRB */
#if defined(UCSR3B)
	if (channel == AVR_USART_CH3)
	{
		/* Set the prescaler properly */
		UBRR3H = (uint8_t)((baud_prscl >> 8) & 0xff);
		UBRR3L = (uint8_t)(baud_prscl & 0xff);
		/* Enable the receiver and transmitter circuits */
		UCSR3B = _BV(RXEN3) | _BV(TXEN3);
		/* 8 bit data frames, 1 stop bit, no parity bit, asynchronous mode */
		UCSR3C = _BV(UCSZ31) | _BV(UCSZ30);
	}
#endif /* UCSR3B */
#endif /* UCSRB */
}

void avr_usart_close(uint8_t channel)
{
#if defined(UCSRB)
	if (channel == AVR_USART_CH0)
	{
		UCSRB &= ~(_BV(RXEN) | _BV(TXEN));
	}
#else /* UCSRB */
#if defined(UCSR0B)
	if (channel == AVR_USART_CH0)
	{
		UCSR0B &= ~(_BV(RXEN0) | _BV(TXEN0));
	}
#endif /* UCSR0B */
#if defined(UCSR1B)
	if (channel == AVR_USART_CH1)
	{
		UCSR1B &= ~(_BV(RXEN1) | _BV(TXEN1));
	}
#endif /* UCSR1B */
#if defined(UCSR2B)
	if (channel == AVR_USART_CH2)
	{
		UCSR2B &= ~(_BV(RXEN2) | _BV(TXEN2));
	}
#endif /* UCSR2B */
#if defined(UCSR3B)
	if (channel == AVR_USART_CH3)
	{
		UCSR3B &= ~(_BV(RXEN3) | _BV(TXEN3));
	}
#endif /* UCSR3B */
#endif /* UCSRB */
}

uint8_t avr_usart_recv(uint8_t channel)
{
#if defined(UCSRA)
	if (channel == AVR_USART_CH0)
	{
		loop_until_bit_is_set(UCSRA, RXC);
		return UDR;
	}
#else /* UCSRA */
#if defined(UCSR0A)
	if (channel == AVR_USART_CH0)
	{
		loop_until_bit_is_set(UCSR0A, RXC0);
		return UDR0;
	}
#endif /* UCSR0A */
#if defined(UCSR1A)
	if (channel == AVR_USART_CH1)
	{
		loop_until_bit_is_set(UCSR1A, RXC1);
		return UDR1;
	}
#endif /* UCSR1A */
#if defined(UCSR2A)
	if (channel == AVR_USART_CH2)
	{
		loop_until_bit_is_set(UCSR2A, RXC2);
		return UDR2;
	}
#endif /* UCSR2A */
#if defined(UCSR3A)
	if (channel == AVR_USART_CH3)
	{
		loop_until_bit_is_set(UCSR3A, RXC3);
		return UDR3;
	}
#endif /* UCSR3A */
#endif /* UCSRA */
	return 0;
}

void avr_usart_send(uint8_t channel, uint8_t byte)
{
#if defined(UCSRA)
	if (channel == AVR_USART_CH0)
	{
		loop_until_bit_is_set(UCSRA, UDRE);
		UDR = byte;
	}
#else /* UCSRA */
#if defined(UCSR0A)
	if (channel == AVR_USART_CH0)
	{
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = byte;
	}
#endif /* UCSR0A */
#if defined(UCSR1A)
	if (channel == AVR_USART_CH1)
	{
		loop_until_bit_is_set(UCSR1A, UDRE1);
		UDR1 = byte;
	}
#endif /* UCSR1A */
#if defined(UCSR2A)
	if (channel == AVR_USART_CH2)
	{
		loop_until_bit_is_set(UCSR2A, UDRE2);
		UDR2 = byte;
	}
#endif /* UCSR2A */
#if defined(UCSR3A)
	if (channel == AVR_USART_CH3)
	{
		loop_until_bit_is_set(UCSR3A, UDRE3);
		UDR3 = byte;
	}
#endif /* UCSR3A */
#endif /* UCSRA */
}

void avr_usart_read(uint8_t channel, uint8_t *buf, size_t length)
{
	while (length--)
	{
		*buf++ = avr_usart_recv(channel);
	}
}

void avr_usart_write(uint8_t channel, const uint8_t *buf, size_t length)
{
	while (length--)
	{
		avr_usart_send(channel, *buf++);
	}
}

size_t avr_usart_printf(uint8_t channel, const char *str, ...)
{
	char buf[256];
	va_list args;
	va_start(args, str);
	size_t len = vsnprintf(buf, 256, str, args);
	va_end(args);
	
	char *ptr = buf;
	while (*ptr)
	{
		avr_usart_send(channel, *ptr++);
	}
	
	return len;
}

void avr_usart_intr_enable(uint8_t channel, uint8_t mode)
{
#if defined(UCSRB)
	if (channel == AVR_USART_CH0)
	{
		if (mode & AVR_USART_RX)
		{
			UCSRB |= _BV(RXCIE);
		}
		if (mode & AVR_USART_TX)
		{
			UCSRB |= _BV(TXCIE);
		}
	}
#else /* UCSRB */
#if defined(UCSR0B)
	if (channel == AVR_USART_CH0)
	{
		if (mode & AVR_USART_RX)
		{
			UCSR0B |= _BV(RXCIE0);
		}
		if (mode & AVR_USART_TX)
		{
			UCSR0B |= _BV(TXCIE0);
		}
	}
#endif /* UCSR0B */
#if defined(UCSR1B)
	if (channel == AVR_USART_CH1)
	{
		if (mode & AVR_USART_RX)
		{
			UCSR1B |= _BV(RXCIE1);
		}
		if (mode & AVR_USART_TX)
		{
			UCSR1B |= _BV(TXCIE1);
		}
	}
#endif /* UCSR1B */
#if defined(UCSR2B)
	if (channel == AVR_USART_CH2)
	{
		if (mode & AVR_USART_RX)
		{
			UCSR2B |= _BV(RXCIE2);
		}
		if (mode & AVR_USART_TX)
		{
			UCSR2B |= _BV(TXCIE2);
		}
	}
#endif /* UCSR2B */
#if defined(UCSR3B)
	if (channel == AVR_USART_CH3)
	{
		if (mode & AVR_USART_RX)
		{
			UCSR3B |= _BV(RXCIE3);
		}
		if (mode & AVR_USART_TX)
		{
			UCSR3B |= _BV(TXCIE3);
		}
	}
#endif /* UCSR3B */
#endif /* UCSRB */
}

void avr_usart_intr_disable(uint8_t channel)
{
#if defined(UCSRB)
	if (channel == AVR_USART_CH0)
	{
		UCSRB &= ~(_BV(RXCIE) | _BV(TXCIE));
	}
#else /* UCSRB */
#if defined(UCSR0B)
	if (channel == AVR_USART_CH0)
	{
		UCSR0B &= ~(_BV(RXCIE0) | _BV(TXCIE0));
	}
#endif /* UCSR0B */
#if defined(UCSR1B)
	if (channel == AVR_USART_CH1)
	{
		UCSR1B &= ~(_BV(RXCIE1) | _BV(TXCIE1));
	}
#endif /* UCSR1B */
#if defined(UCSR2B)
	if (channel == AVR_USART_CH2)
	{
		UCSR2B &= ~(_BV(RXCIE2) | _BV(TXCIE2));
	}
#endif /* UCSR2B */
#if defined(UCSR3B)
	if (channel == AVR_USART_CH3)
	{
		UCSR3B &= ~(_BV(RXCIE3) | _BV(TXCIE3));
	}
#endif /* UCSR3B */
#endif /* UCSRB */
}

