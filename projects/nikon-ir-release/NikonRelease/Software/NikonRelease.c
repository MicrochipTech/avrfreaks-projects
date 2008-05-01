#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>  
#include <avr/pgmspace.h>
#include "Nikon.h"

FUSES = 
{ 

//			Clock source setting
//	0000 	External Clock 
//	0001	High Frequency PLL Clock       16  MC 
//	0010	Calibrated Internal Oscillator 8   MC
//	0011	Calibrated Internal Oscillator 6.4 MC
//	0100	Internal 128 kHz Oscillator 
//	0110	Low-Frequency Crystal Oscillator
//	1000->1111 Crystal Oscillator / Ceramic Resonator 
//	0101->0111 Reserved 

//	FUSE_CKSEL0  =
//	FUSE_CKSEL1  =
//	FUSE_CKSEL2  =
//	FUSE_CKSEL3  =
// 					Start-up time setting
//	00 	6 CK 14CK(1) BOD enabled
//	01 	6 CK 14CK + 4 ms Fast rising power
//	10 	6 CK 14CK + 64 ms Slowly rising power
//	11 	Reserved
//	FUSE_SUT0    =
//	FUSE_SUT1    =

//	FUSE_CKOUT   =	Clock output enabled
//	FUSE_CKDIV8  =	Clock divided by 8
//	LFUSE_DEFAULT

.low = (FUSE_CKDIV8 & FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0), 

//	FUSE_BODLEVEL0	= 1.8 Volt
//	FUSE_BODLEVEL1	= 2.7 Volt
//	FUSE_BODLEVEL0	& FUSE_BODLEVEL1 = 4.2 Volt
//	FUSE_BODLEVEL2	= 
//	FUSE_EESAVE   	= EEPROM preserves chip erase
//	FUSE_WDTON		= Watchdog Timer always on    
//	FUSE_SPIEN    	= Enable Serial Program and Data Downloading
//	FUSE_DWEN     	= DebugWIRE enabled
//	FUSE_RSTDISBL 	= External reset disabled
//	HFUSE_DEFAULT 

.high = (FUSE_SPIEN & FUSE_BODLEVEL0 ), 


// FUSE_SELFPRGEN = Self-programming enabled
.extended = EFUSE_DEFAULT, 
}; 

// LB_MODE_1 = No memory lock features enabled.
// LB_MODE_2 = Further programming of the Flash and EEPROM is disabled
// LB_MODE_3 = Further programming and verification of the Flash and EEPROM is disabled

LOCKBITS = (LB_MODE_1); 

void Wait_Next_Nikon_1sec(void)
{
unsigned char f;
for(f=0;f<16;f++)
	{
		if(PINB&0x01)
			Wait_Next_Nikon_command();
	}
}


void Wait_Next_Nikon_10sec(void)
{
int f;
for(f=0;f<159;f++)
	{
		if(PINB&0x01)
			Wait_Next_Nikon_command();
	}
}

void Wait_Next_Nikon_1Min(void)
{
int f;
for(f=0;f<952;f++)
	{
		if(PINB&0x01)
			Wait_Next_Nikon_command();
	}
}

int main()
{
unsigned char  port;
unsigned int MaxNr;
PORTB	=	0x0F;
DDRB	=	0x10;
MCUCR	= 	( 1 << SE ) | (1 << SM1) ;  // Sleep Enable Power-down
TCCR0B  = 	( 1 << CS00) ; 				// clkI/O/(No prescaling)
GIMSK 	= 	( 1 << PCIE) ;				//	Pin Change Interrupt Enable

for(;;)
	{
		PORTB	=	0x0F;
		DDRB	=	0x10; // Set Pin as output 
		PCMSK 	= 	0x0f; // Pinchage interupt on PB0->PB4
		
	    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		GIFR	=   (1 << PCIF );
		sei();
        sleep_enable();        
        sleep_cpu();
		do
		{
			port=PINB;
			if(!(port&0x01))
				{
				Nikon_Shutter_Release();
				Wait_Next_Nikon_command();
				}
			if(!(port&0x02))
				{
				MaxNr=6000; // max 60 min on
					while( ((PINB&0x01)==0x01) && (MaxNr!=0))
						{
						MaxNr--;
						Nikon_Shutter_Release();
						Wait_Next_Nikon_1sec();
						}
				}
			if(!(port&0x04))
				{				
				MaxNr=600; // max 60 min on
					while( ((PINB&0x01)==0x01) && (MaxNr!=0))
						{
						MaxNr--;
						Nikon_Shutter_Release();
						Wait_Next_Nikon_10sec();
						}
				}
			if(!(port&0x08))
				{
				MaxNr=60; // max 60 min on
					while( ((PINB&0x01)==0x01) && (MaxNr!=0))
						{
						MaxNr--;
						Nikon_Shutter_Release();
						Wait_Next_Nikon_1Min();
						}
				}
		} while((PINB & 0x0f)!= 0xf);

	}
return 0;
}
