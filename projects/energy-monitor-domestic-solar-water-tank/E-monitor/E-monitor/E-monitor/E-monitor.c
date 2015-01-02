/*
 * e-monitor.c
 *
 * Created: 27.06.2012 10:30:44
 *  Author: Joachim Beyer	achimbey@kabelbw.de
 
 *  This project serves as a the monitor for a domestic solar water tank to see whether sufficient
 *  energy from the solar panel is available 
 *  17 nos DS1820 are attached to measure the temperatures which are distinct
 *  due to intakes from solar panel or main heating, and outlets for heating and domestic water use
 *  calculated is the total energy (KWh) as well as the intake and output of power (KW) over time
 *  and stored in a memory stick for later analysis
 */ 

// 1-wire bus on PD7
// keypad on PA 3 keys neg logic
// LCD on  TWI
// USB stick on USART


// PD6 LED sec indicator
 
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "C:\Users\Joachim Beyer\Documents\Atmel Studio\E-monitor\E-monitor\monit.h"

//******************************************************************
// main routine
//******************************************************************
// Features
 
// The standard display is a 2x16 LCD I2C module
 
// if there is no device on bus NoBusMode is entered
//	and dummy data are generated, displayed as well as stored

// if a single unregistered device on the bus
//	SingleDeviceMode is entered, the device is identified, addressed
//	read and displayed; all read data are taken from this device, incremented
//	and displayed to facilitate testing

// if all 20 devices are on the bus the program runs in Bus Mode

// if 3 registered devices are on the bus, Bus Mode is run 

// If there is any key pressed at power up the single device on bus is identified 
//    and its address is displayed to facilitate manually entry in the look up table.
    
// The display is operated by a state machine and 
//  displays all temperatures as well as energy data in all modes;
//  reading is triggered by RTC according to the sample rate in BUS MODE
//  else after each display of all data

// the sample rate can be selected with the soft keys.

// date and time can be set with the soft keys to set the RTC.

// data are logged by the attached stick controller( Elektor USB controller kit for memory sticks)
//    format: date, energy(KWh),temperatures...



//*******************************************************************
# define PB PINA
unsigned long entries=1;            //Data Entries    
bool singlemode=false;
char volatile waitsec = 0;

//*******************************************************************************
int main(void)
{
	
    Init();
	
    if (~PB & 7) ds_read_id();        // read ID of single device after reset and PB pressed
    
	
    displ_calendar();           //start off with displaying calendar
    
    waitsec=2;
    while (waitsec!=0);    
	base= 50;
	    
    if (!resetL())              // Ack response?
    {
        NBM=true;
		sprintf(display1,"\1 No Bus Mode    " );	    
		
        sprintf(display2,"\2                ");
		while(LCD());
		
        waitsec=2;
        while (waitsec!=0); 
    }
    else read=true;
    
    for(;;)
    {
		if (keyin) keyp_exe();
        if (modus==0)           // menu mode keypad
        {            
            if (read)
            {
                if (resetL())           // repeat checking bus          
                {
                    NBM=false;
					if (singlemode) readSdeviceT();
                    else read_deviceT();      
                    //waitsec=2;
                    //while (waitsec!=0);
				}					
                else  
                {   
					NBM=true;
				
                    sprintf (display1,"\1   generating   ");
					
                    sprintf (display2,"\2    dummies     ");
                    while(LCD());
					
					generate();                        // dummies (in file storage)
                    //waitsec=2;
                    //while (waitsec!=0);           
                }
                compute_E();                                // computation
				
                compute_P();                                // computation

                if (entries>=sample_rate)compute_Trend();   // computation
                
                transfer();    
                update = true; 
				//waitsec=2;
				//while (waitsec!=0);               
            }
             
            if (update) display();    
        }                    
                   
     }                   //for
}

