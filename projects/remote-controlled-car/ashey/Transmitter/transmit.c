#include<avr/io.h>
#include<avr/interrupt.h>
#include<avr/signal.h>
#include"test.c"

int main(void)
{
    initDDRs();
	setupTimers(105);
	while(1)
	{
	    StartTransmit(10);
	
	} 
 


}

void StartTransmit(unsigned int transmit )
{
   
    while(transmit!=1)
	{
	  if(transmit%2)
	   {
	    setupTimers(105);
		setIR(50);
	   }
	  else 
	    delay(100,105);
	    transmit = transmit/2;
	}
}
