#define F_CPU 1000000UL
#include <inttypes.h>
#include <avr/io.h>
#include<compat/deprecated.h>		//HEADER FILE FOR FUNCTIONS LIKE SBI AND CBI
#include<util/delay.h>				//HEADER FILE FOR DELAY


int main(void)
{
	uint16_t recieve, address, cdata;
	uint8_t d;

	DDRB = 0x01;   // USE PORTB as INPUT
	DDRD = 0x00;   // USE PORTC.1 as OUTPUT
	DDRC = 0xFF;
	
	while(1)	
	{
		d = PIND;
	
		if(d == 0x02)
		{
			sbi(PORTB, 0);
			address = PIND;
			address = address << 8;
			PORTC = d;
			while(d != 0x02);
			cdata = PIND;
			recieve = address + cdata;
			PORTC = cdata;
			
		}
		else
		{
			
		//	PORTC = 0x00;

		}

	}



return(0);
}
