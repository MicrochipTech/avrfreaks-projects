#include "C:\WinAVR-20080610\avr\include\avr\io.h"


/* Connect PORTA to LEDs and PORTD to SWITCHES on STK500 demo board.  
When any switch is pressed all LEDs are toggled on/off.  Make sure
your compiler options are set to point to the external makefile.*/


unsigned int count;			//Define count as unsigned short


int main(void)					//Main Loop
{

	PORTA = 0xFF;				//Set PORTA - tunrs off LED's (MCU pin sinks current)
	DDRA = 0xFF;				//Set PORTA as output
	PORTD = 0xFF;				//Clear PORTD
	DDRD = 0;					//Set PORTD as input

	for(;;)						//Main loop
	{
		if(PIND != 0xFF)		//Check state of PORTD Pins
		{
			PORTA = ~PORTA;	//If button pressed (input pulled low), toggle bits on PORTA
		}
	
		for(count = 0; count < 65535; count++)	//Delay so flashing LED's are visible
		{
			//Loop for 0 < count < 60000
			count = count + 0;//Do nothing
		}
	}
	return 0;					//Return nothing
}
