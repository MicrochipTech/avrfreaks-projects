
#include "C:\Users\Joachim Beyer\Documents\Atmel Studio\E-monitor\E-monitor\keyp.h"

volatile char modus= 0;          // menu modus  

void keyp_exe (void);
void rateset(signed char);

//soft key values
#define next 2
#define yes 1
#define ret 4
#define inc 1
#define dec 4


// definitions for sample rate
#define fivemin 12
#define tenmin 6
#define fifteenmin 4
#define twentymin 3
char rate = 0;
//***************************************************************************
// state machine soft keys

// soft keys are displayed in line2 of LCD
// keys should be placed below LCD
// key order:   2   1    4
// the key input is handled by RTC 20Hz intr and saved globally in "keys"
// keys others than 1,2,4, are rejected (like double pressed keys)
// called by main on input of flag keyin
// any key can be pressed to start the menu
// the state machine uses 8 states of variable modus 

//***************************************************************************


void keyp_exe (void)
{ 
    bool changed=false;                 //flag
    signed char t;                      // inc and decr
       
    
    switch (modus)
    {
        case 0:                         // start with any key
            sprintf (display1,"\1 remove stick? ");
			while(LCD());
			waitsec = 1;
			while (waitsec!=0);  
            sprintf (display2,"\2nxt   yes    ret");
			while(LCD());
            modus++;  
        break;
        case 1:                         //set year:
             if (keys==next)
            {
                sprintf (display1,"\1   Set year     ");
				while(LCD());
                sprintf (display2,"\2nxt    +      - ");
				while(LCD());
                modus++;
            }
            else if (keys==yes) 
            {
                PORTD &= ~(1<<5);	//reset close file
				_delay_ms(16);
                PORTD |= (1<<5);	// set
				_delay_ms(2000);
                sprintf (display1,"\1  remove stick   ");
				sprintf (display2,"\2                 ");
				while(LCD());
				waitsec=5;
				while (waitsec!=0);
				
                modus=0;                 
                update=true;
                displmod=0;
                entries=0;
            }
            else if (keys==ret)
            {
                modus=0;                   
                update=true;
                displmod=0;
            }   
        break;
        case 2:                         //set month:
            if (keys==next)
            {
                sprintf (display1,"\1   Set month    ");
                sprintf (display2,"\2nxt    +      - ");
				while(LCD());
                if (changed) setFebruary();
                changed = false;                        
                modus++;
            }
            else if (keys == inc)
            {   
                if (++year>49)year=10;
                sprintf (display1,"\1 year %4d      ",(unsigned int)year+2000);
                sprintf (display2,"\2nxt    +      - ");
				while(LCD());
                changed=true;               
            }
            else if (keys== dec)
            {
                if (--year<10)year=49;
                sprintf (display1,"\1 year %4d      ",(unsigned int)year+2000);
                sprintf (display2,"\2nxt    +      - "); 
				while(LCD());               
                changed=true;                               
            }
        break;                
        case 3:                             // set date:
            if (keys ==next)
            {
                sprintf (display1,"\1   Set date     ");
                sprintf (display2,"\2nxt    +      - ");
				while(LCD());
                modus++;
                day=1;
            }
            else if (keys==inc)
            {
                if (++month>12) month=1;
                sprintf (display1,"\1 month %2d       ", month);
                sprintf (display2,"\2 nxt    +       - ");
				while(LCD()); 
                changed=true;           
            }
            else if (keys==dec)
            {
                if (--month==0) month=12;
                sprintf (display1,"\1 month %2d       ",month);
                sprintf (display2,"\2nxt    +      - "); 
				while(LCD());
                changed=true;               
            }                
         break;        
         case 4:                        //set hours:
            if (keys ==next)
            {
                sprintf (display1,"\1   Set hours    ");
                sprintf (display2,"\2nxt   +       - ");
				while(LCD());
                modus++;  
            }
            else if (keys==inc)
            {
                if (++day>mdays[month])day=1;
                sprintf (display1,"\1 day %2d        ", day);
                sprintf (display2,"\2nxt    +      - "); 
				while(LCD());                              
            }
            else if (keys==dec)
            {
                if (--day>mdays[month])day=1;
                sprintf (display1,"\1 day %2d         ",day);
                sprintf (display2,"\2nxt    +       - "); 
				while(LCD());                               
            }                
        break;       
        case 5:                     //set minutes:
            if (keys ==next)
            {
                sprintf (display1,"\1  Set minutes   ");
                sprintf (display2,"\2nxt    +      - ");
				while(LCD());
                modus++;              
            }
            else if (keys == inc)
            {   
                if (++hour>=24) hour = 0;
                sprintf (display1,"\1 %2d hours        ",hour);
                sprintf (display2,"\2nxt    +      - ");
				while(LCD());
            }
            else if (keys== dec)
            {
                if (--hour>24) hour = 24;
                sprintf (display1,"\1 %2d hours       ",hour);
                sprintf (display2,"\2nxt    +     - "); 
				while(LCD());
            }
        break;      
        case 6:                     //set rate?
            if (keys ==next)
            {
                sprintf (display1,"\1Set sample rate?");
                sprintf (display2,"\2nxt     +     - ");
				while(LCD());
                modus++;
            }
            else if (keys == inc)
            {   
                if(++min>=60)min=0;
                sprintf (display1,"\1 %2d  min       ",min);
                sprintf (display2,"\2nxt    +      - "); 
				while(LCD());                               
            }
            else if (keys== dec)
            {
                if (--min>=60)min=0;
                sprintf (display1,"\1 %2d min        ",min);
                sprintf (display2,"\2nxt    +      - ");
				while(LCD());                                
            }
         break;
         case 7:                        
              if (keys==next)
              {
                 modus=0;               
                 update=1;
                 displmod=0;
                 displ_calendar();
    
                 waitsec = 4;			
                 while (waitsec!=0); 
              }              
             
             else if (keys==dec)
             {
                 t= -1;
                 rateset(t);
                 sprintf (display1,"\1     %d Sph     ",sample_rate);
                 sprintf (display2,"\2nxt    +      - ");
				 while(LCD());                                
             }
             else if (keys==inc)
             {
                 t= 1; 
                 rateset(t);
                 sprintf (display1,"\1     %d Sph     ",sample_rate); 
                 sprintf (display2,"\2nxt    +      - ");
				 while(LCD());                                                                                
             }
          break;
       
        }
        keyin=false;                     
}        

    
//-----------------------------------------------------------------------
// set sample rate per hour
// parameter passed are either +1 or -1 to determine the next sample rate
// decr or incr the rate
// sample_rate is not linear and cannot be incremented
//------------------------------------------------------------------------


void rateset(signed char x)
{  
	
    rate+=x;                       // add +1 or -1
    if (rate==255)rate = 3;			// if decremented from zero
    else if (rate>3)rate = 0;
    
    switch (rate)
    {
        case 0: sample_rate=fivemin;    
        break;
        case 1: sample_rate=tenmin; 
        break;
        case 2: sample_rate=fifteenmin; 
        break;
        case 3: sample_rate=twentymin;         
    }
}     
