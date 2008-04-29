#include "main.h"
//global(s)
extern gps_type gps;
 
// Interrupt driven NMEA parser for Atmel AVR
 
ISR(USART_RX_vect)				//UART interrupt on mega xx8 series
{
	static char buffer[6];
	static u08 GGA;
	static u08 RMC;
	static u08 stage;
	static u08 commacount;
	static u08 bufferindex;
	static u08 pointcount;
	char c=UDR0;
	switch(c)
	{
		case '$':			//start of a packet
		  commacount=0;
		  GGA=FALSE;
		  RMC=FALSE;			//we dont know what sort of packet it is yet
		case ',':			//we dont break as the code is shared
		  commacount++;			//note this means packet header is 1
		  bufferindex=0;		//wipe all these so they can be reused
		  pointcount=0;
		  stage=FALSE;
		  memset(buffer,' ',6);
		break;
		case '.':
		  pointcount++;			//we need to be able to detect number of points in the case of altitude
		break;                      
		default:			//we have some of the CSV data
		  if(bufferindex<6)		//dont mess up ! Dont overflow
		  {									
		    	buffer[bufferindex]=c;	//stick the character in our buffer		
		  }
		  if(GGA)
		  {
			switch(commacount)
			{			
				case 3:		//the latitude from the GGA
				  if( (bufferindex<1  && !stage) || bufferindex<5)
				  {
					bufferindex++;
				  }
				  else
				  {
					if(!stage)
					{				
						gps.latitude=(float)atoi(buffer);		// degrees
						stage=TRUE;
					}
					else
					{
						gps.latitude+=minutes*(float)atoi(buffer);	// minutes
					}
				  }
				break;
				case 4:
				  if(c=='S')
				  {
					gps.latitude=-gps.latitude;
				  }
				break;
				case 5:
				  if( (bufferindex<1 && !stage) || bufferindex<5)
				  {
					bufferindex++;
				  }
				  else
				  {
					if(!stage)
					{				
						gps.longitude=(float)atoi(buffer);		// degrees
						stage=TRUE;
					}
					else
					{
						gps.longitude+=minutes*(float)atoi(buffer);	// minutes
					}
				  }
				break;
				case 6:
				  if(c=='W')
				  {
					gps.longitude=-gps.longitude;
				  }
				break;
				case 7:
				  gps.status=atoi(&c);
				break;
				case 10:
				  if(!pointcount)				//wait until we get to a decimal point		
				  {
					bufferindex++;
				  }
				  else
				  {
					gps.altitude=(float)atoi(buffer)*0.1;	//last char in buffer will be after dp
				  }
			}
		  }
		  else if(RMC)
		  {
			if(commacount==8)		//speed in knots
			{		
				if(!pointcount)
				{
					bufferindex++;
				}
				else
				{
					gps.speed=(float)atoi(buffer)*0.1;
				}
			}	
			if(commacount==9)		//the heading
			{
 
				if(!pointcount)
				{
					bufferindex++;
				}
				else
				{
					gps.heading=(float)atoi(buffer)*0.1;
					toggle_pin;				//toggles pin D5 - flashing LED
					gps.packetflag=TRUE;	//this is usually the last interesting part of the fix info to come through
				}
			}
		  }
		  else if(!commacount)			//the header
		  {		
			if(bufferindex<4)
			{
				bufferindex++;		//increase the position in the buffer
			}
			else
			{
				if(buffer=="GPGGA ")	//the last character will be a space
				{
					GGA=TRUE;
				}
				if(buffer=="GPRMC ")
				{
					RMC=TRUE;
				}
			}
		 }	
	}
}
