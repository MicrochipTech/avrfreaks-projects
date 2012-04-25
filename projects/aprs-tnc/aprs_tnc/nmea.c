//************************************************************************
// nmea.c	Processes GPS nmea sentences and extracts position information
//			and computes/processes the data into Mic-E format
//
//			(C) 2007 George Scolaro
//************************************************************************


#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>

#include "main.h"
#include "avrio.h"
#include "ax25.h"
#include "nmea.h"

uint8_t chk_nmea_crc(void);
uint8_t gps_gpgga(void);
uint8_t gps_gpgsa(void);
uint8_t gps_gprmc(void);
uint8_t gps_field(char *ptr);
uint8_t myatoi(char data);
void getfloat(char *ptr);

uint8_t fpnum[3];
uint8_t alt_valid = 0;
uint8_t nmea_seq = 0;

#define NO_PRO	0		// unknown sentence
#define GPRMC	1		// minimum navigation sentence
#define GPGGA	2		// some navigation AND ALTITUDE
#define GPGSA	3		// status AND 2D/3D info

uint8_t sentence;		// nmea sentence type

//*******************************************************************
// This function captures a "snapshot" of the current NMEA sentence and
// makes a local copy to parse and process.
// Converts altitude to feet.

extern uint8_t gps_grab_nmea(void)
{
	uint8_t nchar, toss_packet = 0;

	if (gps_eol) {			// got full nmea packet
		if (i_buf0[0] != '$')	// something wrong, throw it away
			toss_packet = 1;
		else {				// got start of an nmea packet
			i_rdptr0 = 0;
			if (nmea_seq == 0 && strncmp((char *)&i_buf0[1], "GPRMC", 5) == 0)
				sentence = GPRMC, nmea_seq++;
			else if (nmea_seq == 1 && strncmp((char *)&i_buf0[1], "GPGGA", 5) == 0)
				sentence = GPGGA, nmea_seq++;
			else if (nmea_seq == 2 && strncmp((char *)&i_buf0[1], "GPGSA", 5) == 0)
				sentence = GPGSA, nmea_seq++;
			else
				sentence = NO_PRO;		// sentence type we don't know about

			if (nmea_seq > 2)
				nmea_seq = 0;

			if (sentence != NO_PRO) {
				if (chk_nmea_crc())
					toss_packet = 1;	// bad crc, try again
				else {					// ok, GPRMC and good CRC
					if (sentence == GPRMC && gps_gprmc())	// parse for navi data
						toss_packet = 1;	// bad fix
					else if (sentence == GPGGA && gps_gpgga())
						toss_packet = 1;	// bad fix
					else if (sentence == GPGSA && gps_gpgsa())
						toss_packet = 1;	// bad fix
					else {
						gps_eol = 0;		// processed the packet
						if (sentence == GPRMC) {// if RMC then we have a new
							new_position = 1;	//  Position & can send it
							PORTC &= ~(_BV(LED0));	// turn on Green LED
						}
					}
				}
			}
			else
				toss_packet = 1;
			nchar = i_buf0[i_rdptr0];
		}
		if (toss_packet) {
			cli();
			i_wrptr0 = 0;
			gps_start = 1;		// look for nmea '$' - start of packet
			gps_eol = 0;		// and try again
			nchar = UDR0;		// read character in
			sei();
			return (1);				// done
		}
		return(0);
	}
	return(0);
}

//*******************************************************************
// This function checks the nmea string for valid CRC.
//	NMEA crc is computed between the '$' and the '*' (exclusive)
//	CRC is exclusive-or of each character.
//

uint8_t chk_nmea_crc(void)
{
	uint8_t crc = 0, match;
	uint8_t *ptr;

	ptr = &i_buf0[1];		// skip '$', not part of the CRC

	while (*ptr != '*' && *ptr != CR_CHAR)
		crc ^= *ptr++;		// compute CRC unit until done

	ptr++;					// skip '*' go point to attached CRC

	match = 0;

	if (*ptr != CR_CHAR)	// extra the CRC from the nmea string
		match = myatoi(*ptr++) << 4;
	if (*ptr != CR_CHAR)
		match |= myatoi(*ptr++);

	if (crc == match)		// compare computed CRC to attached CRC
		return(0);			// crc is good!
	else
		return(1);			// crc is bad
}

#define FIELD_BUFF	12

//*******************************************************************
// This function parses the GPGSA nmea string and extracts the 2D/3D
//	information to determine if the altitude data is valid in the GPGGA
//  sentence.
// Returns 1 if something is wrong/bad with the GPS fix
//

uint8_t gps_gpgsa(void)
{
	uint8_t i;
	char field_buff[FIELD_BUFF];

	i_rdptr0 = 1;					// skip '$'

	for (i = 0; i < 3; i++)			// skip fields till we get 2D/3D
		if (gps_field(field_buff))
			return(1);

	if (field_buff[0] == '3')		// 3D fix
		alt_valid = 1;				// altitude is valid
	else
		alt_valid = 0;				// no it's not
	return(0);
}


//*******************************************************************
// This function parses the GPGGA nmea string and extracts the altitude
//	information and stores the binary converted data into the appropriate
//  buffer location.
// Returns 1 if something is wrong/bad with the GPS fix
//

uint8_t gps_gpgga(void)
{
	uint8_t i;
	char field_buff[FIELD_BUFF];
	uint16_t alt, temp;

	i_rdptr0 = 1;					// skip '$'

	for (i = 0; i < 7; i++)			// skip fields till we get to valid flag
		if (gps_field(field_buff))
			return(1);

	if (field_buff[0] == '0')		// GPS fix not valid, discard
		return(1);

	for (i = 0; i < 3; i++)			// skip fields till we get altitude
		if (gps_field(field_buff))
			return(1);

	// we have the altitude in metres in field_buff

	alt = 0;
	for (i = 0; i < sizeof(field_buff); i++) {
		if (field_buff[i] == '.' || field_buff[i] == '\0')
			break;
		alt = (alt * 10) + field_buff[i] - '0';	// convert ascii to integer
	}

	// encode altitude in metres in mic-e format

	alt += 10000;

	temp = alt % (91 * 91);
	mic_altitude[0] = (alt / (91 * 91)) + 33;
	mic_altitude[1] = (temp / 91) + 33;
	mic_altitude[2] = (temp % 91) + 33;

	return(0);
}

//*******************************************************************
// This function parses the GPRMC nmea string and extracts the navigation
//	information and stores the binary converted data into the appropriate
//  buffer locations.
// Returns 1 if something is wrong/bad with the GPS fix
//

uint8_t config = 5;

uint8_t gps_gprmc(void)
{
	char field_buff[FIELD_BUFF];
	uint8_t tens, hunds;

	i_rdptr0 = 1;					// skip '$'

	if (gps_field(field_buff))
		return(1);					// GPS Sentence, type, discard

	if (gps_field(field_buff))		// time field
		return(1);

	if (gps_field(field_buff))		// time field
		return(1);

	if (field_buff[0] != 'A')		// 'A' = good fix, 'V' = bad fix
		return(1);
		
	// Process and build mic-e encoded latitude field

	if (gps_field(field_buff))		// latitude field
		return(1);
	
	mic_dst_addr[0] = (field_buff[0] & 0xf) + 0x30;
	if (config & 0x4)
		mic_dst_addr[0] += 0x20;

	mic_dst_addr[1] = (field_buff[1] & 0xf)	+ 0x30;
	if (config & 0x2)
		mic_dst_addr[1] += 0x20;

	mic_dst_addr[2] = (field_buff[2] & 0xf)	+ 0x30;
	if (config & 0x1)
		mic_dst_addr[2] += 0x20;
		
	mic_dst_addr[3] = (field_buff[3] & 0xf)	+ 0x30;
	mic_dst_addr[4] = (field_buff[5] & 0xf)	+ 0x30;	// skip '.' in lat
	mic_dst_addr[5] = (field_buff[6] & 0xf)	+ 0x30;

	if (gps_field(field_buff))		// 'N' or 'S'
		return(1);
		
	if (field_buff[0] == 'N')		// if 'N' offset to characters
		mic_dst_addr[3] += 0x20;

	if (gps_field(field_buff))		// longitude field
		return(1);

	if (field_buff[0] == '1')		// 100's for longitude
		mic_dst_addr[4] += 0x20;

	mic_long[1] = tens = ((field_buff[1]&0xf) * 10) + (field_buff[2] & 0xf);

	if (tens < 10) {
		if (field_buff[0] == '0') {
			mic_dst_addr[4] += 0x20;
			mic_long[1] = 'v' - 28 + tens;
		}
		else
			mic_long[1] = 'l' - 28 + tens;
	}

	mic_long[1] += 28;				// D+28 field

	tens = ((field_buff[3]&0xf) * 10) + (field_buff[4] & 0xf);

	if (tens < 10)					// minutes < 10
		mic_long[2] = 'X' + (field_buff[4] & 0xf);
	else
		mic_long[2] = '&' - 10 + tens;

	tens = ((field_buff[6]&0xf) * 10) + (field_buff[7] & 0xf);	// skip '.'
	mic_long[3] = 28 + tens;
	
	// mic-e latitude processed EXCEPT for W/E bit, process the longitiude
	// first and then get the W/E field.

	if (gps_field(field_buff))		// 'W' or 'E'
		return(1);			
					
	if (field_buff[0] == 'W')		// if 'W' offset to characters
		mic_dst_addr[5] += 0x20;

	// mic-e latitude and longitude fields are all complete

	// Process Speed over Ground

	if (gps_field(field_buff))		// speed over ground (knots)
		return(1);

	getfloat(field_buff);			// get speed FP number

	gps_speed = fpnum[0] * 100 + fpnum[1] * 10 + fpnum[2];
	if (gps_speed > 550)
		gps_speed = 550;			// clip to prevent overflow
	gps_speed = (gps_speed * 115)/100; // convert to mph

	hunds = fpnum[0];
	mic_sp_crs[0] = (fpnum[0]*10) + fpnum[1];
	mic_sp_crs[1] = (fpnum[2]*10);

	if (gps_field(field_buff))		// course made good (true north)
		return(1);

	getfloat(field_buff);			// get course FP number

	gps_course = fpnum[0] * 100 + fpnum[1] * 10 + fpnum[2];

	mic_sp_crs[1] += fpnum[0];		// add in course hundreds
	mic_sp_crs[2] = (fpnum[1]*10) + fpnum[2];

	if (hunds < 2)
		mic_sp_crs[0] += 108;		// 0 to 199 knots
	else
		mic_sp_crs[0] += '0' - 20;	// 200+ knots
	mic_sp_crs[1] += 32;
	mic_sp_crs[2] += 28;

	if (gps_field(field_buff))		// date of fix
		return(1);						

	return(0);						// all is good!
}

//*******************************************************************
// This function scans for an nmea field within the buffer.
// Fields should end with a ',' or '*'
//

uint8_t gps_field(char *ptr)
{
	uint8_t i, *sptr;

	sptr = &i_buf0[i_rdptr0];			// current field

	if (*sptr == ',' || *sptr == '*')	// null field
		return(1);						// nothing here
	
	for (i = 0; i < FIELD_BUFF; i++) {
		if (*sptr == ',' || *sptr == '*') {
			*ptr = '\0';				// terminate copied field
			i_rdptr0++;					// move along too
			return(0);					// all good
		}
		i_rdptr0++;						// move along too
		*ptr++ = *sptr++;				// copy field
	}
	return(1);							// field too long, something is wrong
}

//*******************************************************************
// This function extracts the speed/course FP numbers from the input
//  buffer. Returns them right justified integer part so the calling
//  function can compute the overall integer result.
//	e.g.
//		  0.1 would return 000
//		  1.2 would return 001
//		 12.3 would return 012
//		123.4 would return 123
//

void getfloat(char *ptr)
{

	// get stuff to left of decimal (if any)

	fpnum[0] = fpnum[1] = fpnum[2]=0;   // zero out fp first
	if (ptr[3] == '.') {     	//if 3 digits before the decimal
		fpnum[0] = ptr[0];
		fpnum[1] = ptr[1];
		fpnum[2] = ptr[2];
	}
	if (ptr[2] == '.') {    	//if 2 digits before the decimal
		fpnum[1] = ptr[0];
		fpnum[2] = ptr[1];
	}
	if (ptr[1] == '.')
		(fpnum[2] = ptr[0] );  	//if 1 digit before the decimal
	fpnum[0] = (fpnum[0] & 0xf);// leave just the lsb's
	fpnum[1] = (fpnum[1] & 0xf);
	fpnum[2] = (fpnum[2] & 0xf);
}

//*******************************************************************
// Simple function to return an integer 0-15 from an ascii Hex number
// 0-9, A-F
//

uint8_t myatoi(char data)
{
	if (data < '9'+1)
		return(data - '0');				// to binary
	else
		return(data - 'A'+10);			// Hex, to binary
}
