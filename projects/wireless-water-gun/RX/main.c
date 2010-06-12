/****************************************************************************
*
*   RX
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

#include "RX.h"
#include "Utils.h"

int main(void)
{
	initialize_PORTS();
	USART_init();
	initialize_PWM();

	signed int cmd = 0;
	signed int cmd_last = 0;

	while(1)
	{
		while (get_rx_byte() != SYNC);
			
		cmd_last = cmd;
		cmd = receive_packet();
		
		if (cmd != cmd_last)
		{
			if (cmd_last == OFF)
			{
				TCCR1A |= (1<<COM1A1);
			}

			if (cmd == LOW)
			{
				OCR1A = 245 ;
			}
			else if (cmd == MED)
			{
				OCR1A = 250;
			}
			else if (cmd == HIGH)
			{
				OCR1A = 255;
			}
			else
			{
				OCR1A = 0;
				TCCR1A &= ~(1<<COM1A1);		
			}
		}
	}
	return 0;
}

