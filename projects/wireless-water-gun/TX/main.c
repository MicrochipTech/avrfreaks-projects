/****************************************************************************
*
*   TX
*   Austin Shaf 2010
*
*	Atmega8 16PU Student Soaker
*   
*   Description: The ultimate school prank. A user will have the capability
*				 to spray students walking throught the halls with water 
*				 without them knowing the source of the water. The student 
*				 soaker will be placed above a ceiling board above the school's 
*				 hallway. The device will be connected to a pump that sprays 
*				 water. The user activates the pump using a one channel 
*				 transmitter. A receiver on the module's end outputs a analog 
*				 voltage that the ATmega8 reads. The speed of the pump is 
*				 based on the analog voltage.       
*
****************************************************************************/

#include "TX.h"
#include "Utils.h"

int main(void)
{
	initialize_PORTS();
	USART_init();
	initialize_PWM();
	startup();

	char data = 0;
	char data_last = 0;
	char power = 0;
	
	//endless transmission
	while(1)
	{
		data_last = data;
		data = get_position();
		
		if (data != data_last)
		{
			if (data == LOW)
			{
				power = 20;
			}
			else if (data == MED)
			{
				power = 105;
			}
			else
			{
				power = 255;
			}
	
			OCR1A = power;
		}
		
		if (PORT_IS_OFF(PINC, 5))
		{	
			send_packet(RADDR, data);

			while (PORT_IS_OFF(PINC, 5))
			{
				TCCR1A ^= (1<<COM1A1);	
				_delay_ms(40);
			}
			if (!(TCCR1A & (1<<COM1A1)))
			{
				TCCR1A |= (1<<COM1A1);
			}
		}
		else
		{
			send_packet(RADDR, OFF);
		}
		_delay_ms(1);
	}
	return 0;
}
