#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>       
#include <avr/sleep.h>

#define LOOPDELAY 60

#include <newwavdata1.c>
#include <newwavdata2.c>
#include <newwavdata3.c>
#include <newwavdata4.c>


volatile char alertlevel = 0;
ISR(TIMER1_COMPA_vect){
	/* Occurs if the the time after the last interrupt
	has elapsed 4 seconds */
	alertlevel = 0; //Select initial response
	TIMSK &= ~(1<<OCIE1A); //disable compare interrupts 
						   // to avoid unncecessary interrupts
}

ISR (INT0_vect)
{
 	PORTC=0x00;	
		int i=0;
		unsigned int rd;
		//GIFR &= ~(1<<INTF1);
		if (alertlevel == 0)  //sajid
		{
			alertlevel = 1;
			while(i<WAVDATALENGTH1) {
				rd = pgm_read_byte(&(wavdata1[i])); 
				i=i+1;
				_delay_loop_2(LOOPDELAY); 
				PORTA=rd;
			}
		}
		else if (alertlevel == 1)   //atif
		{
			alertlevel = 2;
			while(i<WAVDATALENGTH2) {
				rd = pgm_read_byte(&(wavdata2[i])); 
				i=i+1;
				_delay_loop_2(LOOPDELAY);	
				PORTA=rd;
			}
		}		
		else if (alertlevel == 2)    //omar
		{
			alertlevel = 3;
			while(i<WAVDATALENGTH3) {
				rd = pgm_read_byte(&(wavdata3[i])); 
				i=i+1;
				_delay_loop_2(LOOPDELAY);	
				PORTA=rd;
			}
		}
		else if (alertlevel == 3)   //asif get out!!!
		{
			//keep alert level 3 unless system is relaxed
			while(i<WAVDATALENGTH4) {
				rd = pgm_read_byte(&(wavdata4[i])); 
				i=i+1;
				_delay_loop_2(LOOPDELAY);		
				PORTA=rd;
			}
		}
		PORTC= 0xFF;
		
		GIFR &= ~(1<<INTF0); //disable debouncing
		TIFR = 0; //disable any pending timer interrupts
		
		TCNT1 = 0; //reset count of timer
		TIMSK |= (1<<OCIE1A); //enable output capture interrupt
}

int main (void) {
	DDRA = 0xFF;
	DDRC = 0xFF;
	PORTC = 0xFF;
	
	//INT 0
	MCUCR |= (1<<ISC01) | (1<<ISC00); // INT0 to rising edge
	GICR |= (1<<INT0);
	

	//timer interrupts
	TCCR1A = 0x00; 
    TCCR1B= 0x00; //mode 0
	
	TCCR1B&= ~(1<<CS11); 			 //clock select clk /1024. 
	TCCR1B|= ((1<<CS12)|(1<<CS10)); //so 1000 counts = 1sec at 1MHz, 4000 counts = 4secs
	OCR1A = 0x0FA0; //4000
	
	TIMSK &= ~(1<<OCIE1A); //output capture interrupt disable
	
	
	set_sleep_mode(SLEEP_MODE_IDLE);
	sei(); //Set Interrupt Enable*/
	
	while(1) {
		sleep_mode();
	}
}
