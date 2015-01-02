
#include "C:\Users\Joachim Beyer\Documents\Atmel Studio\E-monitor\E-monitor\storage.h"
//------------------------------------------------------------------------------------------
// storage.c
// data are written to USB stick 
//------------------------------------------------------------------------------------------

char USBbuffer [20];			//for outp to logger
//***************************************************************
void USB(unsigned int);			// parameter = number of char to be transmitted
char base  = 50 ;				// temperature starting point (dummies)
bool up_down = true;			// rising
void transfer(void);
void generate(void);			//  dummy generation for NBM	
unsigned int i;


//****************************************************
// main transfer routine
// transfers data to USB Stick
// storage is done after every reading and computation
// in NBM every minute for testing purpose
//****************************************************
  
void transfer(void) 
{   
	unsigned int i,z;
    char a8;
  
	sprintf (display1,"\1Writing USBStick");
	sprintf (display2,"\2                ");
	while(LCD());
	waitsec=2;
	while (waitsec!=0);

      //UART USB
        i=sprintf (USBbuffer,"%4d.",2000+year);
		USB (i);
        // month, day
        i=sprintf (USBbuffer,"%2d.%2d.",month,day);
        USB (i);       
        // hour min
        i=sprintf (USBbuffer,"%2d:%2d\t",hour,min);
        USB (i);      
        //energy
    
        i=sprintf (USBbuffer,"%ld\t",energy);
        USB (i);     
        // temperature values
        for (z=0;z<20;z++) 
        {
            a8 = store[z];
			a8<<=1;
            i=sprintf (USBbuffer,"%2d\t",a8);
			USB (i);
        }
		i=sprintf (USBbuffer,"\n");
		USB(i);
	sei();	
	sprintf (display1,"\1    Entries     ");
	sprintf (display2,"\2     %6lu     ",entries);
	
	while(LCD ());
    entries++;
    read = false;
    displmod = 0;
}

//*********************************************************************
// dummy generation in NBM
// generates one temperature dummy stair case spaced 1C°rising
// keep top temperature below max= 95C°
// keep base above 20°C
// staircase rises to top and then falls by 2° each time to lower limit
// input = base, up-down
//**********************************************************************

void generate(void)
{
	unsigned int dev;
    char dummy,stair;
	
	cli();
    for(dev=0,stair=base;dev<maxdevices;dev++)
    {
        if ((stair+=2)>190) dummy = 190;   //incr by 1°C
        else dummy = stair;
        store[dev]=dummy;
    }
    if (base>=190)up_down=false;       // upper limit 95
    else if (base<=40)up_down=true;    // lower limit 20
    if (up_down)base+= 4;            // inc or dec for next series
    else base-= 4;  
	sei(); 
	
}

//************************************************************
 // USART transfer to USB controller 9600 Baud
 // the controller handles FAT and writing to USB stick
 // parameter = number of char to be written
 //*********************************************************** 
 void USB( unsigned int t)
 {
	 unsigned int i;
	 
	 for (i=0;i<t;i++)
	 {
		 while ( !( UCSRA & (1<<UDRE)) );
		 UDR = USBbuffer[i];
	 }
	 
 }                          
                 
                
                  
         
           
   
                  
  

            

