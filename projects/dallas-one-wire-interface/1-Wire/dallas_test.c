#include "1-wire_bus.h"
#include "pin_numbers.h"
#include "1-Wire.h"
#include <avr\io.h>



#define OWPIN	OMPin5



//uint8_t array[8]; 
//myStruct *p = (myStruct *)array; 
//p->u = uval; 
//etc. 


int main(void)
{
	unsigned char buffer;

	//setup a timer as a simple stopwatch counter: prescaler = 1

		/* reset the timer variables */


	TCNT1 = 0;  

	/* start the timer in CTC mode - no clock prescaler */
	TCCR1A = 0;
	TCCR1B = 0 | (1<<CS10);	//prescaler = 1

 

//do
//{
//	result = 
//	OWReset(OWPIN);
//} while (1);



	do
	{
		OWReadBit(OWPIN);
	} while (1);



//	do
//	{
//		OWWriteBit(OWPIN,0);
//	} while (1);







	return(buffer);
}
