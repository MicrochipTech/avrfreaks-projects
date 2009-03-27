#include<mega32.h> 
unsigned int B,cacah; 
void io_init(); 
void timer_init();
void main(void)
 {   
       			io_init();
	       		timer_init();	
// aktif timer 0 
TCNT0=0x28;
TCCR0=0x05;
TIMSK=0x01;
TIFR=0x00;
#asm("sei");		
while(1)  //waiting timer intrupsion,
	{
       	}

}

void timer_init()
{    
  	TCCR1A=0xA3;
	TCCR1B=0x0C;  //A=8 B=64,c=256(43hz) d=1024 42 hz
	TCNT1=0x0000;
} 
void io_init()
{      
	DDRB=0xff;// PORTB to output
}  

interrupt[TIM0_OVF]void timer0_overflow(void)
{ TCNT0=0x28;
	cacah++; // like 0.02 sekon 
	
	if(cacah==50)// like 1 sekon
	   {    cacah=0; 
	        //just exampel program
	   	B=0xff;  
	   	PORTB=~B;         	  	    	   	      	        	       
             // add u program here this timer is very presisition
             
       	   }
}
        	 
