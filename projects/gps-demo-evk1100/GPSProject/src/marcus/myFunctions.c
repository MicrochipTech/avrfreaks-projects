#include "myFunctions.h"
#include "definitions.h"
#include "m_interrupts.h"
#include "cycle_counter.h"
#include "nmea.h"
#include "board.h"
#include "dip204.h"
#include <stdio.h>
#include <string.h>

/*!
 * \brief Software delay
 */
void delay_ms(unsigned short time_ms)
{
  unsigned long u32CountVal,u32CompareVal;

  TimeOut = 0;
  u32CountVal = Get_sys_count();

  u32CompareVal = u32CountVal  + ((unsigned long)time_ms * (FOSC0 / 1000)); // WARNING: MUST FIT IN 32bits.

  Set_sys_compare(u32CompareVal); // GO

  // The previous COMPARE write succeeded.
  // Loop until the COUNT&COMPARE match triggers.
  while (!TimeOut);
}

void parseGPSBuff()
{
	char NmeaPacket[gps_buff_size];
	int i = 0;
	
	// copy GPS buffer to local storage in case of interrupt before comletion of parsing
	for(i = 0; i<gps_buff_size; i++)
	{
		NmeaPacket[i] = CurrentPosition.gpsBuff[i];
	}
	
	enable_usart_interrupts(EXAMPLE_USART, 0, 1);	// enable gps interrupt, now it is safe to overwrite the buffer

	if(!strncmp(NmeaPacket, "GPGGA", 5))	// GPGGA
		{
			gps.gotGGA = nmeaProcessGPGGA(NmeaPacket, &CurrentPosition);
		}
	else if(!strncmp(NmeaPacket, "GPVTG", 5))
		{
			gps.gotVTG = nmeaProcessGPVTG(NmeaPacket, &CurrentPosition);
			
		}
}

void displayGPS(void)
{
	int i;
	char myBuff[20];

	// clear myBuff
	for (i=0;i<20;i++){myBuff[i] = 0;}
	
	dip204_set_cursor_position(1,1);	
	sprintf(myBuff, "%06.0f", CurrentPosition.timeStamp);	
	dip204_write_string("LOCK: ");
	dip204_write_string(myBuff);
	
	dip204_set_cursor_position(1,2);	
	sprintf(myBuff, "%09.5f", CurrentPosition.latitude);	
	dip204_write_string("LAT:  ");
	dip204_write_string(myBuff);
	
	dip204_set_cursor_position(1,3);
	sprintf(myBuff, "%010.5f", CurrentPosition.longitude);
	dip204_write_string("LON:  ");
	dip204_write_string(myBuff);
	
	dip204_set_cursor_position(1,4);
	sprintf(myBuff, "%05.1f", CurrentPosition.altitude);
	dip204_write_string("ALT:  ");
	dip204_write_string(myBuff);
	dip204_write_string(" meters");
}
