#include <avr/io.h>    	// Standard AVR header
#include <avr/delay.h> 	// Delay loop functions

int main(void) 
 {
	DDRA = 0xFF; // Set PORTA as output
	DDRB = 0x00; // Set PORTB as input
	PORTB = 0xFF;
	ADCSRA = 0x80;

	unsigned int x;

	static void delay_clk (uint16_t clk)
	{
		for (;clk>0;clk--)
			_delay_ms(1);
	}

	while(1)
	{
		ADCSRA = 0xC0;
		x=PINB;
		if (x==0xFF);
		PORTA=0x00;
		//save info in memory
		while (x==0xFE)
		{	
			x=PINB;
			PORTA=0x8F;
			delay_clk(40);
			x=PINB;
			if (x==0xFE)
			{
				PORTA=0x80;
				delay_clk(40);
			}
		}
		while (x==0xFC)
		{	
			x=PINB;
			PORTA=0x8F;
			delay_clk(40);
			x=PINB;
			if (x==0xFC)
			{
				PORTA=0x80;
				delay_clk(40);
			}
		}
		while (x==0xFD)
		{
			x=PINB;
			PORTA=0x87;
			delay_clk(80);
			x=PINB;
			if (x==0xFD)
			{
				PORTA=0x80;
				delay_clk(80);
			}
		}
		while (x==0xF9)
		{	
			x=PINB;
			PORTA=0x87;
			delay_clk(80);
			x=PINB;
			if (x==0xF9)
			{
				PORTA=0x80;
				delay_clk(80);
			}
		}
		while (x==0xFB)
		{	
			x=PINB;
			PORTA=0x03;
			delay_clk(120);
			x=PINB;
			if (x==0xFB)
			{
				PORTA=0x00;
				delay_clk(120);
			}
		}
		while (x==0xF3)
		{	
			x=PINB;
			PORTA=0x03;
			delay_clk(120);
			x=PINB;
			if (x==0xF3)
			{
				PORTA=0x00;
				delay_clk(120);
			}
		}
		while (x==0xF7)
		{	
			x=PINB;
			PORTA=0x01;
			delay_clk(180);
			x=PINB;
			if (x==0xF7)
			{
				PORTA=0x00;
				delay_clk(180);
			}
		}

	}
	return 1;
}
