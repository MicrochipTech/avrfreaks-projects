//****************************************************
//   RTC.c
//   RTC   and calendar functions
//-----------------------------------------------------  
#include "C:\Users\Joachim Beyer\Documents\Atmel Studio\E-monitor\E-monitor\RTC.h"

// number of days for each month 2011
 volatile char mdays[]={0,31,28,31,30,31,30,31,31,30,31,30,31}; 
//--------------------------------------------------------------------------------------------
volatile char sec = 0;
volatile char min = 4;
volatile char hour = 12;
volatile char day = 1;      
volatile unsigned int month = 10;
volatile char year = 14;
volatile bool read = false;         //triggers ds1820 reading
volatile char debounce= 1;
volatile char keys;

char sample_rate= fivemin;			// at start up 5 min sample interval

volatile bool keyin = false;
volatile bool update = false;       // update display


bool NBM = false;					//NoBusMode
void calendar (void);
void setFebruary (void);
void displ_calendar(void);

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// outp: read flag to trigger temperature reading
//       keyin flag and keys input
//       update flag for display state machine
//       waitsec timer
//		 RTC + calendar
//***********************************************************************

//TIMER1_COMPA Timer/Counter1 Compare Match A

ISR (TIMER1_COMPA_vect)		//40Hz
{ 
    static char d40;
	char mask=16;			
	PORTB ^=mask;		//intr indicator pin5
	
    if (--debounce==0)
    {
        debounce=1;
        keys =~PINA;					// input and test keys pin 0-2
        keys &= 0x07;					// mask pin 3-7
        if (keys!=0)                     
        {
            keyin = true;                  // inp flag if key pressed
            debounce=20;					// 2 inputs per sec permitted
            update=false;                   // stop display state machine
        }
    }        
        
    if (d40==5) PORTD &= ~(1<<6);		//LED off, 12.5% on-time
    if (++d40  >= 40)					// div by 40 = 1 sec 
    {
        d40=0;
        PORTD |= (1<<6);                // LED on sec indicator
		
		if (waitsec!=0)	waitsec--;		// sec timer for wait delay
		
        if (++sec>= 60)					// sec counter
        {
            sec=0;
            if (++min >= 60)			// min counter
            {
                min = 0;
                if (++hour >=24)		// hour counter
                { 
                    hour = 0;
                    calendar();
                }
            }                            
            if (!singlemode && !NBM && ( ( min % (60/sample_rate) ==0) ||min==0) )
            {
                read=true; // trigger temperature reading according to sample rate
                update=false;
            }
        }
        if (!read && waitsec== 0 &&(sec%3==0||sec==0)) update = true;      // trigger next display 3sec interval
    }
       
}
                 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// calendar routine
// called every new day
// updates calendar in RAM
//----------------------------------------------------------------------

void calendar(void)
{ 
    if (++day > mdays [month])	//last day of month?
    {
        day = 1;   
        if (++month > 12)
        {
            month = 1; 
            if (++year >= 50) year = 11; // max year = 2049!!!
            setFebruary();  
        } 
           
    }
}
        
//-------------------------------------------------------------------      
// leap year?
// this routine checks for the leap year on new year eve
// by setting the number of days for February in the RAM array
//-------------------------------------------------------------------

void setFebruary (void)
{
 mdays[2] = ((2000+year) % 4) != 0? (char)28 : (char)29;
}


//-----------------------------------------------------------------
// display calendar 
// called by main and keypad
//-----------------------------------------------------------------

void displ_calendar(void)
{  
    sprintf (display1,"\1 %4d %2d %2d     ",(unsigned int)year+2000,month,day);
    sprintf (display2,"\2 %2d HRS %2d MIN  ",hour,min);
	while(LCD());
}  





       

