#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

	void ioinit(void);
		
	volatile uint16_t i = 0;
	volatile uint8_t temp;
	volatile uint8_t tone [100] = {1,0,2,0,3,0,4,0,5,5,0,5,5,0,6,0,6,0,6,0,6,0,5,5,5,5,0,4,0,4,0,4,0,4,0,3,3,0,3,3,0,2,0,2,0,2,0,2,0,1,1,1,1,9}; //The melody is written here

ISR(TIMER0_OVF0_vect)			//Regular interrupt to keep the rhythm
{
	temp = tone[i];
	if(temp != 9)
		{
		i += 1;			//Go to the next tone
		}
	else
		{			//9 marks the end of the melody
		}
}

int main(void)
{

	ioinit();
	while(1)
	{
	while(temp == 0)
		{
			PORTB &= 0b11111101;	//Pause
		}
	while(temp == 1)
		{
		PORTB |= 0b00000110;		//C4 tone (1/261.63Hz= ca 3822uS)
		_delay_us(1911);
		PORTB &= 0b11111101;
		_delay_us(1911);
		}
	while(temp == 2)
		{
		PORTB |= 0b00000110;		//D4 tone
		_delay_us(1703);
		PORTB &= 0b11111101;
		_delay_us(1703);
		}
	while(temp == 3)
		{
		PORTB |= 0b00000110;		//E4 tone
		_delay_us(1517);
		PORTB &= 0b11111101;
		_delay_us(1517);
		}
	while(temp == 4)
		{
		PORTB |= 0b00000110;		//F4 tone
		_delay_us(1432);
		PORTB &= 0b11111101;
		_delay_us(1432);
		}
	while(temp == 5)
		{
		PORTB |= 0b00000110;		//G4 tone
		_delay_us(1276);
		PORTB &= 0b11111101;
		_delay_us(1276);
		}
	while(temp == 6)
		{
		PORTB |= 0b00000110;		//A4 tone
		_delay_us(1136);
		PORTB &= 0b11111101;
		_delay_us(1136);
		}
	while(temp == 7)
		{
		PORTB |= 0b00000110;		//B4 tone
		_delay_us(1012);
		PORTB &= 0b11111101;
		_delay_us(1012);
		}
	while(temp == 8)
		{
		PORTB |= 0b00000110;		//A3 tone
		_delay_us(2272);
		PORTB &= 0b11111101;
		_delay_us(2272);
		}
	while(temp == 9)
		{
			PORTB &= 0b11111001;		//End of melody
		}
	}
}
void ioinit(void)
{
	DDRB = 0xfe;
	TIMSK = 0x02;
	TCCR0 = 0x04;	//Prescaler for interrupt timer
	sei();
} 