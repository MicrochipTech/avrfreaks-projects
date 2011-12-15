/******************************************************************************
 * Project      : Software UART ATtiny13
 * File         : Software UART driver.c
 * Author      	: Jeroen Lodder
 ******************************************************************************
  Change History:

    Version 1.0 - June 2010
    > Created
	Version 2.0 - November 2011
	> Revisioned
	> Optimised to Flash 552 bytes
	> Removed ascii lookup tables
******************************************************************************/
#include "SoftwareUART.h"

//Variables:
volatile byte sTX_state 		= 0;		//Volatile for interrupt
volatile byte sTX_bitcounter 	= 0;
volatile byte sTX_data 			= 5;

//Functions:

void init_suart(void){
	/* PORT is set here */	
	DIRREG	|=	(1<<TXport);	//As output
	PORTREG	|=	(1<<TXport);	//With pullup

	/* Timer is set here, CTC mode, and your prescaler 8*/
	//Timer 0
	TCCR0A = (1<<WGM01);	//CTC mode
	TCCR0B = (1<<CS01);		//Prescaler 8
 	OCR0A  = TIMERVALUE; 	//Set Output Compare A 		(See header file)
 	TIMSK0 = (1<<OCIE0A);	//Enable Output Compare A interrupt
	sei();					//Enable interrupt
}

ISR(TIM0_COMPA_vect){		//Output Compare A interrupt
	switch(sTX_state){
		//##########
		case IDLE:											//IDLE mode, wait for startbit
			TIMSK0 &= ~(1<<OCIE0A);							//Disable interrupt, waste of time when nothing has to be send
	break; 	//Wait 1 bit length
		//##########
		case STARTBIT:										//STARTBIT MODE (1 bit)
			PORTB  &= ~(1<<TXport);							//Pull down, low level
			sTX_state 	= TRANSMIT;							//Next state is transmit
	break;	//Wait 1 bit length
		//##########
		case TRANSMIT:
			//Fase 1: Shift 1 bit
				if( (sTX_data & 0x01) == 0x01 ){			//1 in lsb sTX_data					
					PORTB |= (1<<TXport);					//Pull up, high level
				}else{										//0 in lsb sTX_data					
					PORTB  &= ~(1<<TXport);					//Pull down, low level
				}
				sTX_data >>= 1;								//Shift away sent bit
			//Fase 2: Incement Bit Counter and Compare
				sTX_bitcounter += 1;						//Increment
				if(sTX_bitcounter < 8){						//Not done yet
					sTX_state = TRANSMIT;					//Repeat transmit state next bit
				}else{										//Done with data
					sTX_state = STOPBIT;					//Stopbit is the last bit
				}								
	break;	//Wait 1 bit length
		//##########
		case STOPBIT:
			//Fase 3: Stopbit, put line to high level again									
			PORTB |= (1<<TXport);							//Pull up, High level
			//Prepare for next time
			sTX_bitcounter  = 0;							//Reset bitcounter
			sTX_state		= IDLE;							//Change mode to idle
	break;
		//##########
		default:	//Any exceptions or whatever
			sTX_state 	=	STOPBIT;
	break;
	}
	return;
}

void sTX_putchar(byte data){
	while(sTX_state != IDLE);						//When routine is not idle, wait for it to become idle
		sTX_data   = data;							//Put data ready
		sTX_state  = STARTBIT;						//Set state to startbit
		TIMSK0 	|= (1<<OCIE0A);						//Enable timer interrupt
}

#ifdef putbyte_ascii 
void sTX_putbyte_ascii(byte data){
	unsigned int i;
    for(i=100;i>=1;i=i/10){			
        sTX_putchar ( ((data/i)%10)+48 );			//digit = (input / divisor) Mod 10
    }   
}
#endif

#ifdef putint
void sTX_putint(int data){
	sTX_putchar(data& 0xFF);	//Low
	sTX_putchar(data>>8);		//High 
}
#endif

#ifdef putint_ascii
void sTX_putint_ascii(int data){
	unsigned int i;
    for(i=10000;i>=1;i=i/10){			
        sTX_putchar ( ((data/i)%10)+48 );			//digit = (input / divisor) Mod 10
    }  
}
#endif

#ifdef putstring
// This function uses lots of memory because the strings in the argument are inside the flash memory, and reseved in data*
void sTX_putstring(char *p){
	byte i_putstring = 0; 
 
 	while (p[i_putstring] != '\0') 	
 	{
		sTX_putchar(p[i_putstring]);
  		i_putstring++;
 	}
}
#endif

