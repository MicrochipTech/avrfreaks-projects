/*
 * usispi.c
 *
 * Created: 9/6/2014 5:34:56 PM
 *  Author: Mike
 */ 
#include "avr/io.h"
#define F_CPU 8000000UL
#include "usispi.h"
#include "util/delay.h"
#include <stdlib.h>

/* Old code not using timer going to straight control of spi



void usi_spi_out(uint8_t usi_spi_cs, uint8_t usi_spi_out)
{
	USIDR = usi_spi_out;
	USISR |= (1<<USIOIF);
	USI_PORT |= (1<<usi_spi_cs);
	timer_enable();
}

ISR(TIMER1_OVF_vect)
{
	if ((USISR & (1<<USIOIF)) == (1<<USIOIF))
	{
		timer_disable();
		USI_PORT &= ~((1<<PORTB0) | (1<<PORTB2) | (1<<USI_SPI_CS1) | (USI_SPI_CS2));
	}
	else
	{
		USICR |= (1<<USITC);
	}
}

void timer_init(void)
{
	TCCR1B &= ~(0x0f);
	TCNT1 = 0;
	OCR1C = USI_TIMER_COUNT;
	TIMSK = (1<<TOIE1);	
}

void timer_enable(void)
{
	TCCR1B |= USI_TIMER_CS;
	TCNT1 = 0;
	
}

void timer_disable(void)
{
	TCCR1B &= ~(0x0f);	
}
*/

void usi_spi_init(void)
{
	USI_PORT |= (1<<USI_DI) | (1<<USI_SPI_CS1) | (1<<USI_SPI_CS2);
	USI_PORT &= ~((1<<USI_DO) | (1<<USI_CLK));
	USI_DDR |= (1<<USI_DO) | (1<<USI_CLK) | (1<<USI_SPI_CS1) | (1<<USI_SPI_CS2);
	USI_DDR &= ~(1<<USI_DI);
	USICR |= (1<<USIWM0) | (1<<USICS1) | (1<<USICLK);
	_delay_ms(30);
	
}

void usi_spi_out(uint8_t usi_spi_cs, uint8_t usi_spi_out)
{
	USIDR = usi_spi_out;
	USISR |= (1<<USIOIF);
	USI_PORT &= ~(1<<usi_spi_cs);
	_delay_us(USI_CLK_Delay);
	while((	USISR & (1<<USIOIF)) != (1<<USIOIF))
	{
		USICR |= (1<<USITC);
		_delay_us(USI_CLK_Delay);		
	}
	USI_PORT &= ~((1<<USI_DO) | (1<<USI_CLK)); 
	USI_PORT |=(1<<USI_SPI_CS1) | (1<<USI_SPI_CS2);

}
char data[10];
void usi_putv(uint16_t lcd_varible, uint8_t lcd_numtype, uint8_t lcd_digits )
{
	uint8_t x=lcd_digits;
	
	itoa(lcd_varible, data,lcd_numtype);
	for (uint8_t i=0; data[i] != 0x00 ; i++)
	{
		lcd_digits--;
	}
	
	if (lcd_digits > x)
	{
		lcd_digits=0;
	}
	
	for (uint8_t i=0; i<lcd_digits; i++)
	{
//		usi_spi_out(USI_SPI_CS2,' ');
		usi_putc(' ');
	}
	for (uint8_t i=0; data[i] != 0x00; i++)
	{
		usi_putc(data[i]);
	}

}

