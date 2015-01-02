
#include "C:\Users\Joachim Beyer\Documents\Atmel Studio\E-monitor\E-monitor\display.h"

///**************************************************************
// display.c
// displays each temperature in pairs
// called by main after entries and by keypad menu

//**************************************************************

char displayT(unsigned int);			// special displ routine for temperatures
unsigned int volatile displmod;							// of all sensors
void display(void);
char display1[25];				//LCD outp buffer
char display2[25];
bool LCD (void);
char status = 0;
bool cursor= true;

//************************************************************
// displays two temperatures in pairs of buffer displ mode 0-19
// called by display function
//*****************************************************************
char displayT(unsigned int i)
{
	char t,dP;
	t= store[i];
	if (t&1) dP = 5;
	else dP=0;
	t>>=1;
	sprintf(display1,"\1 T%2d = %2d.%1d C   ",i,t,dP);
	i++;
	t= store[i];
	if (t&1) dP = 5;
	else dP=0;
	t>>=1;
	sprintf(display2,"\2 T%2d = %2d.%1d C   ",i,t,dP);
	i++;
	while(LCD());
	waitsec=2;
	while (waitsec!=0);
	return (char)i;
}
//---------------------------------------------------------------------
// display state machine
// called by main
// this handles the entire display of data changing every 3 sec
// the display runs through all temperatures, energy, power, trend 
// triggered by RTC changing in 3 sec interval
// there is no difference in the display between normal mode and NoBusMode 
// as well as singlemode (to facilitate
// testing without 1-wire bus) except that in NBM dummies are generated 
// every display cycle or the single device is read in singlemode. 
//---------------------------------------------------------------------

void display(void)
{
        if (displmod<20) displmod = displayT(displmod);		
		
        else if (displmod==20) displ_energy();
				 
        else if (displmod==21) 
		{
            if (entries>=sample_rate) displ_Trend();
			else displmod++;
		}
		else if (displmod >= 22)
		{
			if (NBM || singlemode)
			{
				read = true;		// start with new dummies
				update = false;
			}
			displmod = 0;
		}										
}
//**************************************************************
// TWI handler 
// Master write mode
//**************************************************************

#define SLA_W  2		// LCD adr = 1; slave adr + Write = 0

bool LCD (void)			// outputs 1 line of 16 characters from Buffer
{
				
	unsigned int i;		// buffer index
	bool err = false;
	

	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);		// start transmission
	
	while (!(TWCR & (1<<TWINT)));
	
	if ((TWSR & 0xF8) == 0x10)	
	{
		PORTB= 1;		//repeated start LED0	
	}
	
	
	if ((TWSR & 0xF8) == 0x08 || 0x10)			// status = START 
		{
			TWDR = SLA_W;						// send slave adr
			TWCR = (1<<TWINT) | (1<<TWEN);		// start transmission
		}
	
	while (!(TWCR & (1<<TWINT)));	// wait for TWINT flag
	
	
	//*******************************************************************************
		
	if ((TWSR & 0xF8) == 0x20)
		{
			PORTB |= (1<<3);			//adr not acknowledged
			err = true;
			goto newstart;					//LED3 
		}
	if ((TWSR & 0xF8) == 0x38)
		{
			PORTB |= (1<<3);			// arbitration lost
			err = true;					// LED3
			goto newstart;
		}
		//***************************************************************************

	if ((TWSR & 0xF8) == 0x18)			// status = start data LED 1
		{
			if (cursor)
			{
				TWDR = 0x1b;
				TWCR = (1<<TWINT) | (1<<TWEN);		//send
				while (!(TWCR & (1<<TWINT)));
				
				TWDR = 0x73;
				TWCR = (1<<TWINT) | (1<<TWEN);		//send
				while (!(TWCR & (1<<TWINT)));
			
				cursor = false;
				err= false;
				goto newstart;
			}
			TWDR = display1[0];
			PORTB = 2;								
			TWCR = (1<<TWINT) | (1<<TWEN);		// clear intr flag
		}
		//****************************************************************************
		
	for (i=1;i<17;)
		{
			while (!(TWCR & (1<<TWINT)));
		
			if ((TWSR & 0xF8) == 0x28)			// status acknowledged data
				
				{
					TWDR = display1[i++];
					TWCR = (1<<TWINT) | (1<<TWEN);			// clear intr flag
				}
			else
				{
					PORTB = (1<<2);						// data not acknowledged
					err = true;									// LED2
					goto newstart;
				}
		}
		
			
		for (i=0;i<17;)
		{
			while (!(TWCR & (1<<TWINT)));
			
			if ((TWSR & 0xF8) == 0x28)						// status acknowledged data
			
			{
				TWDR = display2[i++];
				TWCR = (1<<TWINT) | (1<<TWEN);			// clear intr flag
			}
			else
			{
				PORTB = (1<<2);						// data not acknowledged
				err = true;									// LED2
				goto newstart;
			}
		}
newstart:

	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);				// stop transmission
	_delay_ms(10);
	TWCR = 0;
	PORTB = 0;		// turn of status LEDs
	return err;

}	

		
     
            
        
        


