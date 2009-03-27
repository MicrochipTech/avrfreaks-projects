#include<mega32.h> 
void io_init(); 
void main(void)
 {   
 io_init();	       		
// aktifkan intrup external 
GICR|=0x80;
MCUCR=0x0C;
GIFR=0x50;           
#asm("sei");		
while(1)
	{ 
	 PINB.0=1;  //waiting input on
      	}

}
void io_init()
{  	DDRB=0x00;// PORTB sebagai input
//--------------- output 	
	DDRD=0xff;     
}  

interrupt[EXT_INT1]void ext_int1_isr(void)
{ 	PORTD=0xff;
        // add u program here
}
        	 
