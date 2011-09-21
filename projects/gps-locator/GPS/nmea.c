/*! \file nmea.c \brief NMEA protocol function library. */
//*****************************************************************************
//
// File Name	: 'nmea.c'
// Title		: NMEA protocol function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002.08.27
// Revised		: 2002.08.27
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef WIN32
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "nmea.h"


// Global variables
extern uint8_t status;
extern char USARTBuffer[80];
extern char NMEAPacket[80];
extern char GPGGA[80];
double pi = 3.141592653589793238462643;

uint8_t nmeaProcess(void)//(cBuffer* rxBuffer)
{
	uint8_t foundpacket = NMEA_NODATA;
	

	
		for(int i=0;i<80;i++)
		{
		NMEAPacket[i]=USARTBuffer[i];
		}
		
		
		// check message type and process appropriately
		if(!strncmp(NMEAPacket,"GPGGA", 5))
		{
			// process packet of this type
				
			nmeaProcessGPGGA(NMEAPacket);
			// report packet type
			foundpacket = NMEA_GPGGA;
		}
		else if(!strncmp(NMEAPacket, "GPRMC", 5))
		{
			// process packet of this type
			nmeaProcessGPRMC(NMEAPacket);
			// report packet type
			foundpacket = NMEA_GPRMC;
		}
		
	
	
	return foundpacket;
}

void nmeaProcessGPGGA(char* packet)
{

	uint8_t i;
	uint8_t GPIndex=0;
	

	//start parsing just after "GPGGA,"
	i = 6;
	// attempt to reject empty packets right away
	if((packet[i]==',') & (packet[i+1]==',')) return;
		

	// get UTC time [hhmmss.sss]
	while(packet[i] != ',')				// next field: Time
	{
	GPSTime[GPIndex]=packet[i];
	i++;
	GPIndex++;
	} 
	
	
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field: Latitude
	{
	//do nothing
	i++;
	GPIndex++;
	} 
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field: N/S
	{
	//do nothing
	i++;
	GPIndex++;
	} 
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field: Longitude
	{
	//do nothing
	i++;
	GPIndex++;
	} 
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field: E/W
	{
	//do nothing
	i++;
	GPIndex++;
	} 
	i++;
	GPIndex=0;
	
	while(packet[i] != ',')				// next field:Fix Quality
	{
	FixQuality=packet[i];
	i++;
	GPIndex++;
	} 
	
	
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field:No of satellites
	{
	//bypass no of satellites
	i++;
	GPIndex++;
	} 
	
	
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field:horizontal dilution of position
	{
	//bypass horizontal dilution of position
	i++;
	GPIndex++;
	} 
	
	
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field:Altitude in meters above sea level
	{
	Altitude[GPIndex]=packet[i];
	i++;
	GPIndex++;
	} 
	//GPIndex=0;
	// next field: longitude					//this method can be used to access a variable directly
	//for(int i=25;i<35;i++)
	//{
	//Long[GPIndex]=packet[i];
	//GPIndex++;
	//}
	// get latitude [ddmm.mmmmm]
	//latitude = strtod(&packet[i], &endptr);
	
	//minutesfrac = modf(latitude/100, &degrees);
	//latitude= degrees + (minutesfrac*100)/60;
	//latitude *= (pi/180);
	}
	
	void nmeaProcessGPGSV(char* packet)
	{
	}
	
	void nmeaProcessGPRMC(char* packet)
	{
	uint8_t i;
	uint8_t GPIndex=0;
	

	//start parsing just after "GPRMC,"
	i = 6;
	// attempt to reject empty packets right away
	if(packet[i]==',' & packet[i+1]==',') return;
		
	while(packet[i] != ',')				// next field: Time
	{
	//do nothing
	i++;
	GPIndex++;
	} 
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field: Navigation warning
	{
	//do nothing
	i++;
	GPIndex++;
	} 
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field:Latitude
	{
	Lat[GPIndex]=packet[i];
	i++;
	GPIndex++;
	} 
	
	
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field:N/S
	{
	NS[GPIndex]=packet[i];
	i++;
	GPIndex++;
	} 
	
	
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field:Longitude
	{
	Long[GPIndex]=packet[i];
	i++;
	GPIndex++;
	} 
	
	
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field:E/W
	{
	EW[GPIndex]=packet[i];
	i++;
	GPIndex++;
	} 
	
	
	i++;
	GPIndex=0;
	// get speed
	while(packet[i] != ',')				// next field: speed
	{
	Speed[GPIndex]=packet[i];
	i++;
	GPIndex++;
	} 
	
	
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field:Course made good
	{
	Heading[GPIndex]=packet[i];
	i++;
	GPIndex++;
	} 
	
	i++;
	GPIndex=0;
	while(packet[i] != ',')				// next field:Date
	{
	GPSDate[GPIndex]=packet[i];
	i++;
	GPIndex++;
	} 
	
	
	}