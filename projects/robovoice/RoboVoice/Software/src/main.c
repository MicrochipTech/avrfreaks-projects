/*!\file main.c
 *  Contains the main infinite loop.  It does nothing special, because the 
 *	design is interrupt based.
 */

#include "define.h"

//!Main function
int main(void){

	cli();

	timer1_init();
	init_ADC();
	#ifdef SAWTOOTHVIBRATO
		initSawtoothVibrato();
	#endif

	//set portD as output
	DDRD=0xFF;
	//set portB as output
	DDRB=0xFF;
	
 	sei();

	for(;;){
	}
	
	return 0;
}//main