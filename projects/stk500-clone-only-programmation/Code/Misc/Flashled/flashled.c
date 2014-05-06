/******************************************************************************
Flashled
*
* Jidan Al-Eryani
*******************************************************************************/
#include <avr/io.h>


#define FREQ		  8000000L
#define OC1A_PIN PB5              // OC1A pin (Timer/counter)
#define OC1A_DDR DDRB             


#define LED_FREQ     5         // flashi frequency of LED in Hz

                                   

int main(void)
{    
    
    // set the T/C output pin
    OC1A_DDR |= (1<<OC1A_PIN);

    // set prescalar and clear tT/C on compareA match                               
    TCCR1B = (1<<CS10) | (1<<CS11)  | (1<<WGM12);
	
    // preset timer1 high/low byte
    OCR1A = (FREQ/2/64/LED_FREQ) - 1; 
	
    // enable T/C
    TCCR1A = (1<<COM1A0);

    
	//LED will flash wihtout any further code -> Interrupts!
    while(1);
}
