#include <util/delay.h>
#include "delay.h"


void delay_us(unsigned int d)
{

	for( ; d>0;d--)
		_delay_us(1);

}


void delay_ms(unsigned int d)
{
	for( ; d>0;d--)
		_delay_ms(1);
	
}

void delay_s(unsigned int d)
{
	
	for( ; d>0;d--)
	{
		for(int q=1000 ; q>0;q--)
			_delay_ms(1);
	}
}

