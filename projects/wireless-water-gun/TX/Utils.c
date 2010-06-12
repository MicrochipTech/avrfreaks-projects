#include "Utils.h"

//*************INITIALIZE PORTS****************
//Initialize ports
void initialize_PORTS()
{
	DDRB = 0xFF;  //set PORTB to 11111111
	DDRC = 0x00;  //set PORTC to 00000000
	DDRD = 0xFE;  //set PORTD to 11111110

	PORTB = 0x04;  //turn relay on
	PORTC = 0xFF;  //enable pull up resistors
	PORTD = 0x00;  //all outputs low
}


void initialize_PWM()
{
	// set up PB1 and PB2 with 8bit PWM
	TCCR1A = (1 << WGM10);//|(1 << COM1A1);  // 8 bit fast PWM, set OC1A on compare match, clear them at top

	TCCR1B = (1 << CS10)|(1 << WGM12);  // enable timer1 - clkI/O/1 (No prescaling), fast PWM

	OCR1A = 0;  //set wave to 0% duty cycle
}


//****************GET POSITION*****************
//get the position of the key switch (1-4)
char get_position()
{
	if (PORT_IS_OFF(PINC, 4))
	{
		int x = 0;
		for(x = 0; x <= 4; x++)
		{
			TCCR1A &= ~(1<<COM1A1);
			_delay_ms(200);
			TCCR1A |= (1<<COM1A1);
			_delay_ms(200);
		}
		PORT_OFF(PORTB, 2);
	}
	else if(PORT_IS_OFF(PINC, 1))
	{
		return MED;
	}
	else
	{
		PORT_ON(PORTB, 3);
		_delay_ms(1);
		if (PORT_IS_OFF(PINC, 0))
		{
			PORT_OFF(PORTB, 3);
			return HIGH;
		}
	}
	PORT_OFF(PORTB, 3);
	return LOW;
}


//*******************STARTUP*******************
//LED brightens then dims
void startup()
{
	int x = 0;
	TCCR1A |= (1<<COM1A1);

	for(x = 0; x <= 255; x++)
	{
		OCR1A = x;
		_delay_ms(5);
	}
	for(x = 255; x >= 32; x--)
	{
		OCR1A = x;
		_delay_ms(5);
	}

	TCCR1A &= ~(1<<COM1A1);
	_delay_ms(15);
	TCCR1A |= (1<<COM1A1);

	for(x = 0; x <= 20; x++)
	{
		OCR1A = x;
		_delay_ms(5);
	}

	char power = get_position();

	if (power != LOW)
	{
		int brightness_stop = 0;
		if (power == MED)
		{
			brightness_stop = 110;
		}
		else
		{
			brightness_stop = 255;
		}
		for(x = 20; x <= brightness_stop; x++)
		{
			OCR1A = x;
			_delay_ms(5);
		}
	}
}
