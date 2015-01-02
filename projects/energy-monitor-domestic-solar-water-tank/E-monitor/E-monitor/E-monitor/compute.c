
#include "C:\Users\Joachim Beyer\Documents\Atmel Studio\E-monitor\E-monitor\compute.h"

//************************************************************************************
// compute.c
// compute Energy, Power and Trend for display 2 x 16 char
// display routines for TWI are in display.c
//*************************************************************************************

signed long FIFO[12];      // for accumulation of power values over time
signed long trend_p;
signed long trend_n;
extern char sample_rate;      // default 5 min sample interval
signed long energy=0;
signed long prev_energy=0;
signed long power=0;

//----------------------------------------------------------------------------
// prototypes
//----------------------------------------------------------------------------
void compute_E(void);
void compute_P(void);
void compute_Trend (void);
void displ_energy(void);
void displ_Trend (void);
void shift_FIFO (void);
void showE (unsigned long,unsigned int,unsigned int);
void convert1(void);
void convert2(void);

//**************************************************************************
// look up table for energy storage capacity c in Joule (range 0-100C°C)
//**************************************************************************

const unsigned int capacity []
PROGMEM ={4218,4214,4211,4208,4205,4202,4200,4198,4196,4194,4192,4191,4189,4188,4187,4186,4185,4184,4183,
4183,4182,4181,4181,4180,4180,4180,4179,4179,4179,4179,4179,4178,4178,4178,4178,4178,4178,4178,4178,
4179,4179,4179,4179,4179,4179,4180,4180,4180,4180,4180,4181,4181,4181,4182,4182,4182,4183,4183,4184,
4184,4184,4185,4185,4186,4186,4187,4187,4188,4189,4189,4190,4190,4191,4192,4192,4193,4194,4194,4195,
4196,4196,4197,4198,4199,4200,4201,4201,4202,4203,4204,4205,4206,4207,4208,4209,4210,4211,4213,4214,
4215,4216};


//-------------------------------------------------------------------------------------
// equation: E= T/2 * c * LTR * 2.778 / 10 000  (Wh)
// t is T*2  bit0 represents 0.5°C
// compute E of each layer of 25 or 50 LTR
// c is the storage capacity of water in Joule
// 2.778 is the conversion constant of Joule to Wh
// devices 0-16 only are attached to the tank
//--------------------------------------------------------------------------------------

void compute_E(void)
{
    unsigned int d=0;
    unsigned int t;                 // temperature
    unsigned int c;       
    signed long acc=0;
    signed long e;					// e= energy
             	
    cli();
	prev_energy = energy;
	
	// top and bottom device 0 and 16 have 25ltr
	
    t= (unsigned int) store[d];             // device0
    t>>=1;
    if (t==0) goto skip1;			// skip if device is faulty
    c= pgm_read_word_near(&capacity[t]);
    e= (signed long)t * c *25*2.778/10000;   //25ltr
    acc += e;
	//showE (e,t,d);
	
	
	// check sensor 16
skip1:	
	d=16;                    
    t= (unsigned int) store[d];           
    t>>=1;    
    if (t==0) goto skip2;
    c= pgm_read_word_near(&capacity[t]);
    e= (signed long)t * c * 25* 2.778/10000;   //25ltr
    acc += e;            // accumulate
	//showE (e,t,d);
	
//***********************************************************	
	// devices 1 and 15 are attached to 50 l tank section
//***********************************************************	  
skip2:   
    for (d=1;d<16;d++)      // devices 1-15
    {
        t=(unsigned int) store[d];
        t>>=1;        
        if(t==0) continue;
        c= pgm_read_word_near(&capacity[t]);
        e= (signed long)t * c * 50 *2.778/10000;  
        acc += e;        // accumulate
		//showE (e,t,d);
    }
//**************************************************************
// below 15C, energy is not taken into account therefore deducted
// due to the fact that the energy of cold water is not usable 
//**************************************************************
    if (acc < offset) acc = 0;
    else acc -= offset;			//
    energy = acc;
    if (entries==0)prev_energy=energy;  
	sei();
}
//***************************************************************** 
// display of energy and power
// E (int) is converted to float and displayed as nn.nnnn kWh    
// P (int) = (Enow - Eprevious) * 60min / sample rate     
// displayed as float +- nn.nnn kW
// called by main 
// operated by displmod and update flag set by RTC  
//*****************************************************************

void displ_energy(void)//LCD Port
{
	
    sprintf(display1, "\1 E                 ");
    sprintf(display2, "\2 P                 ");
	convert1();
	convert2();
	while (LCD());
	update = false;
	displmod++;
	waitsec=4;
	while (waitsec!=0);
}

//************************************************************************** 
// computation of power in Watts input(+) or output(-)
// store in shift register
// called by main
// display routine changes format to KW
//**************************************************************************
     
void compute_P(void)
{
    cli();
    power = (energy - prev_energy)* sample_rate; //W
	shift_FIFO();
    FIFO[11]= power;                // store in FIFO top   
	sei(); 
}


//*******************************************************************************
// computation of power trend
// compare power input and output  
// positive and negative values are separated and averaged
// FIFO is maintained by compute_power  
//*********************************************************************** 

void compute_Trend (void)
{
    signed int t;
    unsigned int r;         //index
	char count;
    char cp=0;      //pos counter
    char cn=0;      //neg counter
    
    trend_p=0;      //pos numbers
    trend_n=0;      //neg numbers
    cli();
    for (r=11,count= sample_rate;count>0;count--,r--)       //take samples of the last hour
        {
            t = FIFO[r];
            if (t>0)        //test for positive numbers
            {
                trend_p+=t;
                cp++;
            }
            else 
            {
                trend_n+=t;          //neg numbers
                cn++;
            }
        }
	if (cp!=0)trend_p/=cp;                   //average
    if (cn!=0)trend_n/=cn;
     
	sei();
	//waitsec=2;
	//while (waitsec!=0);
}
//***************************************************************************
// displays energy trend
// called by main
//***************************************************************************

void displ_Trend(void)
{  
    sprintf(display1,"\1  IN   %5ld W  ",trend_p);
    sprintf(display2,"\2  OUT  %5ld W  ",trend_n);
	while(LCD());
	update = false;
	displmod++;
}


//*****************************************************************************     
// maintain FIFO after new entry
// the FIFO stores the last 12 power values
// the FIFO is shifted down before a new entry on top is made
//*****************************************************************************

void shift_FIFO (void)
{
    unsigned int bottom,top;        //pointer
    
    for (bottom=0,top=1;top<12;top++,bottom++)
    {
        FIFO[bottom]= FIFO[top];
    }
}
    
//*****************************************************************************
    
// show energy, temperature and device number of each 
// not activated. 
 
 void showE (unsigned long E,unsigned int T,unsigned int D)
 {
	 sprintf (display1,"\1   E = %8lu    ",E);
	 sprintf (display2,"\2  Temp %2u    %2u   ",T,D);			
	 while(LCD());
	 sei();
	 waitsec=3;
	 while(waitsec!=0);
	 cli();
 }
 //*************************************************************
 // convert long values (Watt) into ascii with dP and 3 decimals (KW)
 //*************************************************************
 void convert1(void)
 {
	 
	 display1[5]= (char)(energy/10000 + 48);
	 display1[6]= (char)(energy%10000/1000 +48);
	 display1[7]= '.';
	 display1[8]= (char)(energy%10000%1000/100 +48);
	 display1[9]= (char)(energy%10000%1000%100/10 +48);
	 display1[10]= (char)(energy%10000%1000%100%10 +48);
	 display1[11]= ' ';
	 display1[12]= 'K';
	 display1[13]= 'W';
	 display1[14]= 'h';
 }
 
 void convert2(void)
 {
	 display2[5]= '+';
	 if (power<0) 
	 {
		 display2[5]= '-';
		 power*= -1;
	 }
	 display2[7]= (char)(power/10000 + 48);
	 display2[8]= (char)(power%10000/1000 +48);
	 display2[9]= '.';
	 display2[10]= (char)(power%10000%1000/100 +48);
	 display2[11]= (char)(power%10000%1000%100/10 +48);
	 display2[12]= (char)(power%10000%1000%100%10 +48);
	 display2[13]= ' ';
	 display2[14]= 'K';
	 display2[15]= 'W';
 }