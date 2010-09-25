#include <avr/io.h>
#define F_CPU 64000000L  // !!!! wtf
#include <util/delay.h>
#include <stdint.h>
#include "flash.h"

void latch(uint8_t c)
{
	PORTC=c;
	PORTB|=0x04;
	PORTB&=~0x04;
}


void set_adr(uint32_t adr)
{
	uint8_t l,m,h;
	l=(adr & 0x0000ff);
	m=(adr >> 8) & 0x0ff;
	h=(adr >> 16);

	latch(m);
	PORTC=l;
	PORTB&=~0x03;
	PORTB|=(h & 0x03);
}

void set_data(uint8_t c)
{
	DDRA=0xff;
	PORTA=c;
}

uint8_t get_data(void)
{
	DDRA=0x00;
	return PINA;
}


uint8_t read_byte(uint32_t adr)
{
	uint8_t c;
	get_data();
	we_up;
	set_adr(adr);
	oe_down;
	c=get_data();
	oe_up;
	return c;
}


void write_byte(uint32_t adr, uint8_t c)
{	
	get_data();
	oe_up;
	set_adr(adr);
	we_down;
	set_data(c);
	we_up;
}

void wait(void)
{
	uint8_t b,b2;
	do
	{
		b=read_byte(0);
		b2=read_byte(0);
	} while ((b & 0x40) != (b2 & 0x40));
}

void chip_erase(void)
{
	led_on;
	write_byte(0x555,0xaa);
	write_byte(0xaaa,0x55);
	write_byte(0x555,0x80);
	write_byte(0x555,0xaa);
	write_byte(0xaaa,0x55);
	write_byte(0x555,0x10);
	wait();
	led_off;
}

void program_byte(uint32_t adr, uint8_t c)
{
	write_byte(0x555,0xaa);
	write_byte(0xaaa,0x55);
	write_byte(0x555,0xa0);
	write_byte(adr,c);
	wait();
}


uint8_t programm_block(uint32_t adr, uint16_t len, uint8_t *block)
{
	uint32_t i;
	ce_down;
	led_on;
	for(i=0;i<len;i++)
	{
		program_byte(adr+i,block[i]);
		if (read_byte(adr+i)!=block[i])
		{
			ce_up;
			return 1;
		}
	}
	ce_up;
	led_off;
	return 0;
}



void test_latch(void)
{
	while(1)
	{
		latch(0x00);
		PORTC=0x01;
		_delay_ms(500);
		latch(0x01);
		PORTC=0x00;
		_delay_ms(500);
	}
}
