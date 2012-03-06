/////////////////////////////////////////////////////////////////////
//
//		RTC.C
//
//		real time clock using asysnchronous counter for AVR ATmega16
//
/////////////////////////////////////////////////////////////////////

#include "rtc.h"
  
/*		GLOBAL DECLARATIONS		*/

time t;


/*		FUNCTION DEFINITIONS	*/

void rtcInit(void) 
{                                

    _delay_ms(1000);	//for crystal to become stable
	
    TIMSK &=~((1<<TOIE2)|(1<<OCIE2));     //Disable TC0 interrupt
    ASSR |= (1<<AS2);           //set Timer/Counter2 to be asynchronous from the CPU clock 
                                //with a second external clock(32,768kHz)driving it.  
    TCNT2 = 0x00;
    TCCR2 = 0x05;                 //prescale the timer to be clock source / 128 to make it
                                //exactly 1 second for every overflow to occur
    while(ASSR&0x07);           //Wait until TC0 is updated
    TIMSK |= (1<<TOIE2);        //set 8-bit Timer/Counter0 Overflow Interrupt Enable                             
    sei();                     //set the Global Interrupt Enable Bit  
                                        
}

ISR(TIMER2_OVF_vect) //Timer0 overflow interrupt vector
{ 
    
    if (++t.second==60)        //keep track of time, date, month, and year
    {
        t.second=0;
        if (++t.minute==60) 
        {
            t.minute=0;
            if (++t.hour==24)
            {
                t.hour=0;
                if (++t.date==32)
                {
                    t.month++;
                    t.date=1;
                }
                else if (t.date==31) 
                {                    
                    if ((t.month==4) || (t.month==6) || (t.month==9) || (t.month==11)) 
                    {
                        t.month++;
                        t.date=1;
                    }
                }
                else if (t.date==30)
                {
                    if(t.month==2)
                    {
                       t.month++;
                       t.date=1;
                    }
                }              
                else if (t.date==29) 
                {
                    if((t.month==2) && (notLeap()))
                    {
                        t.month++;
                        t.date=1;
                    }                
                }                          
                if (t.month==13)
                {
                    t.month=1;
                    t.year++;
                }
            }
        }
    }  

}  
 
char notLeap(void)      //check for leap year
{
    if (!(t.year%100))
        return (char)(t.year%400);
    else
        return (char)(t.year%4);
}         
  
  