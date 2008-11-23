#include "definitions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "nmea.h"

int nmeaProcessGPGGA(char packet[], struct currentPositionStruct *pos) // return 1 if successful, 0 if not
{
	unsigned int i;
	char* endptr;
	double degrees, minutesfrac;

	// start parsing just after "GPGGA,"
	i = 6;
	// attempt to reject empty packets right away
	if(packet[i]==',' && packet[i+1]==',')
		return 0;

	// get UTC time [hhmmss.sss]
	pos->timeStamp = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: latitude
	
	// get latitude [ddmm.mmmmm]
	pos->latitude = strtod(&packet[i], &endptr);
	// convert to pure degrees [dd.dddd] format
	minutesfrac = modf(pos->latitude/100, &degrees);
	pos->latitude = degrees + (minutesfrac*100)/60;
	
	while(packet[i++] != ',');				// next field: N/S indicator
	
	// correct latitute for N/S
	if(packet[i] == 'S') pos->latitude = -pos->latitude;
	
	while(packet[i++] != ',');				// next field: longitude
	
	// get longitude [ddmm.mmmmm]
	pos->longitude = strtod(&packet[i], &endptr);
	// convert to pure degrees [dd.dddd] format
	minutesfrac = modf(pos->longitude/100, &degrees);
	pos->longitude = degrees + (minutesfrac*100)/60;
	
	while(packet[i++] != ',');				// next field: E/W indicator

	// correct latitute for E/W
	if(packet[i] == 'W') pos->longitude = -pos->longitude;
	
	while(packet[i++] != ',');				// next field: position fix status

	// position fix status
	// 0 = Invalid, 1 = Valid SPS, 2 = Valid DGPS, 6 = Dead Reckoning
	// check for good position fix
	pos->fixIndicator = atoi(&packet[i]);
	
	while(packet[i++] != ',');				// next field: satellites used
	
	// get number of satellites used in GPS solution
	pos->numberOfSatellites = atoi(&packet[i]);
	while(packet[i++] != ',');				// next field: HDOP (horizontal dilution of precision)
	while(packet[i++] != ',');				// next field: altitude
	
	// get altitude (in meters)
	pos->altitude = strtod(&packet[i], &endptr);

	return 1;

	while(packet[i++] != ',');				// next field: altitude units, always 'M'
	while(packet[i++] != ',');				// next field: geoid seperation
	while(packet[i++] != ',');				// next field: seperation units
	while(packet[i++] != ',');				// next field: DGPS age
	while(packet[i++] != ',');				// next field: DGPS station ID
	while(packet[i++] != '*');				// next field: checksum
}

int nmeaProcessGPVTG(char packet[], struct currentPositionStruct *pos)	// return 1 if successful, 0 if not
{
	unsigned int i;
	char* endptr;

	// start parsing just after "GPVTG,"
	i = 6;
	// attempt to reject empty packets right away
	if(packet[i]==',' && packet[i+1]==',')
		return 0;

	// get course (true north ref) in degrees [ddd.dd]
	pos->heading = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'T'
	while(packet[i++] != ',');				// next field: course (magnetic north)

	// get course (magnetic north ref) in degrees [ddd.dd]
	//GpsInfo.VelHS.heading.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'M'
	while(packet[i++] != ',');				// next field: speed (knots)

	// get speed in knots
	//GpsInfo.VelHS.speed.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'N'
	while(packet[i++] != ',');				// next field: speed (km/h)

	// get speed in km/h
	pos->speed = strtod(&packet[i], &endptr);
	return 1;
	while(packet[i++] != ',');				// next field: 'K'
	while(packet[i++] != '*');				// next field: checksum
}

