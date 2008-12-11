#include"definitions.h"
void InitDDRs(void)
{
	DDRB =  0;
	DDRC =  0;
	DDRD =  0; 
    DDRB = DDRB | _BV(PB1) | _BV(PB6);
	PORTB = 0;
	DDRD =  DDRD | _BV(PD0) | _BV(PD2) | _BV(PD3);
	PORTD = 0;
	PORTC = 0;
}
void TurnMotors(int count)
{
	if (count == 15) // move right 
		{
			PORTB = PORTB | _BV(PB6) ;
			// Motor 1 is Off
		}
            
	else if(count == 5)         // move forward
		{
			PORTD = PORTD | _BV(PD3);
			PORTB = PORTB | _BV(PB6);
			// Both Motors should be On.
		}
	else if(count == 2)	// move backward
    	{
		    PORTD = PORTD | _BV(PD0) | _BV(PD2) ;  
			PORTB = PORTB | _BV(PB1);
			// Both Motors should rotate reverse
		}		
           
	else if (count == 12) //  move left
        {
            PORTD = PORTD | _BV(PD3) ;	
			// Motor 2 is Off
		}             
	return ;
}
int FindFirst(void)
{
    if(TSOP0)
        return 1;
    if(TSOP1)
        return 2;
    if(TSOP2)
        return 3;
    if(TSOP3)
        return 4;
    if(TSOP4)
        return 5;
    if(TSOP5)
        return 6;
    if(TSOP6)
        return 7;
    if(TSOP7)
        return 8;
    else return 0;
}	
