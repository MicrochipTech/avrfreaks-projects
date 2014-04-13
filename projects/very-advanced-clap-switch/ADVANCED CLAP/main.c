#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define LOAD1 0
#define LOAD2 1
volatile int clap;
volatile int clapflag;
volatile int clapflag2;

ISR(INT0_vect)
{
	
	clapflag++;
	clap++;
	
	
}



int main(void)
{
    volatile int clap1;
	DDRB|=(1<<LOAD1);
	DDRB|=(1<<LOAD2);
	PORTD|=(1<<2);
MCUCR|=((1<<ISC00)|(1<<ISC01));    
GICR|=(1<<INT0);
    sei();
    clap=0;
	clapflag=0;
	
	while(1)
    {
	
	clapflag2=clapflag;
	
		
		while(clapflag2==1)
		{
			_delay_ms(4000);
			        
		
	clap1=clap;

	if(clap1==2)
	{
		if(PINB&(1<<LOAD1))
		PORTB&=~(1<<LOAD1);
		else
		PORTB|=(1<<LOAD1);
	}
	else if (clap1==3)
	{
		if(PINB&(1<<LOAD2))
		PORTB&=~(1<<LOAD2);
		else
		PORTB|=(1<<LOAD2);
	}
	else if (clap1==4)
	{
		PORTB=0x00;
	}

	clap=0;
	clapflag=0;
	clapflag2=0;
		}
		
		}
		
				
}

