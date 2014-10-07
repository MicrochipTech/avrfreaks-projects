/*
 * gps.c
 *
 * Created: 23.3.2014 15:20:15
 *  Author: PjV
 *
 * Version: 1.0  
 */ 

#include <avr/io.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "macros.h"
#include <util/delay.h>
#include "tracker.h"
#include "gps.h"
#include "db_uart.h"


/***********************************************************************//**
* GPS Configuration Commands.
* Command Packet Structure :

|header     |ids               |Payloads|checksum |
|Sync1|Sync2|Class|ID|Len0|Len1|Payloads|CK_A|CK_B|

* more information u-blox6_ReceiverDescriptionProtocolSpec.pdf
* www.u-blox.com
***************************************************************************/
const unsigned char cmd_header[2] PROGMEM = { 0xB5/*Sync1*/, 0x62/*Sync2*/};

const CMD_ID_t ids PROGMEM = 
{		/*cfg_RATE_period*/	{0x06,		/* Class - Configuration Input Message*/ 
							 0x08,		/* ID - Measurement Rate*/ 
							 0x06, 0x00	/* Len0, Len1 - payload's length=6 bytes*/}, 
		/*cfg_MSG_rate*/	{0x06,		/* Class - Configuration Input Message*/
							 0x01,		/* ID - Message Rate(s)*/  
							 0x08, 0x00,/* payload's length=8*/}, 
		/*cfg_RXM_power*/	{0x06,		/* Class - Configuration Input Message*/
							 0x11,		/* ID - Receiver Manager, Power management*/ 
							 0x02, 0x00,/* payload's length=2*/} 
};

const CMD_PAYLOAD_t payloads PROGMEM = 
{
/*set_period_7s*/	{0x58, 0x1B,/*0x1B58==7000mS=>period 7sec*/ 0x01, 0x00, 0x01, 0x00 },
/*set_period_max*/	{0x60, 0xEA, 0x01, 0x00, 0x01, 0x00 },
	/*disable_GGA*/	{0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	/*disable_GLL*/	{0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	/*disable_GSA*/	{0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	/*disable_GSV*/	{0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	/*enable_RMC*/	{0xF0, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 },/*Enable RMC only in module-uart1*/
	/*disable_RMC*/	{0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	/*disable_VTG*/ {0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	/*power_down*/	{0x08, /*Always set to 8*/ 0x01, /*	0: Max. performance mode
														1: Power Save Mode (>= FW 6.00 only)
														4: Eco mode */},
	/*power_eco*/	{0x08, /*Always set to 8*/ 0x04, /*	0: Max. performance mode
														1: Power Save Mode (>= FW 6.00 only)
														4: Eco mode */}
};


/*************************************************************************//**
* static functions
*****************************************************************************/
static void _calc_checksum_p(PGM_P id, PGM_P payload, unsigned char* checksum);
static void _convert_minutes(unsigned char* mins);
static void _send_cmd_p(PGM_P id, PGM_P payload);

/* RXC-Interrupt handler receives NMEA-RMC string, index calculates chars */
static volatile unsigned char _nmea_rx_ind;



/*************************************************************************//**
* Description: Initialize GPS communications USART1
*****************************************************************************/
void GPS_UartInit(void)
{
	/* set baud rate */
	UBRR1L = GSM_BAUDRATE;		//low byte
	UBRR1H = (GSM_BAUDRATE>>8);	//high byte

	UCSR1B |= (1 << RXEN1);	// enable reception circuitry
	/* default frame format: 8 bit, no parity, 1 bit */
	//UCSR1C = (1 << URSEL) | (1 << UCSZ00) | (1 << UCSZ01);
	UCSR1B |= (1 << RXCIE1);// Enable the USART Receive Complete interrupt (USART1_RXC_vect)
	
	_nmea_rx_ind = 0;
	rmc_ready_flag = 0;
	/* these RMC-chars are what we looking for, first characters are always same */
	rmc_rx_data_g[0] = '$';
	rmc_rx_data_g[1] = 'G';
	rmc_rx_data_g[2] = 'P';
	rmc_rx_data_g[3] = 'R';
	rmc_rx_data_g[4] = 'M';
	rmc_rx_data_g[5] = 'C';	
}

/*************************************************************************//**
 * Description: Set only Recommended Minimum data (NMEA-RMC), n sec period.
				More information u-blox6_ReceiverDescriptionProtocolSpec.pdf
				www.u-blox.com
 * \param
 * \returns  
*****************************************************************************/
void GPS_config(void)
{
#if USE_NEO6_GPS_MODULE
	UCSR1B |= (1 << TXEN1);		// Turn on the transmission
	UCSR1B &= ~(1 << RXEN1);	// Turn off the receiving, we dont expect any acks

	_send_cmd_p( ids.cfg_RATE_period, payloads.RATE_set_period_7s );
	_send_cmd_p( ids.cfg_MSG_rate, payloads.MSG_disable_GGA );
	_send_cmd_p( ids.cfg_MSG_rate, payloads.MSG_disable_GLL );
	_send_cmd_p( ids.cfg_MSG_rate, payloads.MSG_disable_GSA );
	_send_cmd_p( ids.cfg_MSG_rate, payloads.MSG_disable_GSV );
	_send_cmd_p( ids.cfg_MSG_rate, payloads.MSG_enable_RMC );/*Enable only RMC, GPSmodule:uart1*/
	_send_cmd_p( ids.cfg_MSG_rate, payloads.MSG_disable_VTG );
	
	UCSR1B &= ~(1 << TXEN1);	// Turn off the transmission, we don't need it anymore
	UCSR1B |= (1 << RXEN1);		// Now we are ready receiving data from GPS again
#endif	
}

/*************************************************************************//**
* Description:	Send configuration command to the GPS chip.
				Send command: header - ID - Payload - Checksum.
				Calculates checksums
* Param[in]:	Command's ID (4 bytes)
				Payload (Variable length field)
*****************************************************************************/
static void _send_cmd_p(PGM_P id, PGM_P payload)
{
	unsigned char chksmbts[2] = {0x00, 0x00};	// checksum bytes, calculate
	const unsigned char *ptr_to_header = cmd_header;
	unsigned char pl_len = pgm_read_byte(id+2);//payload's length, third byte in ID
	unsigned char ind;
	
	_calc_checksum_p( id, payload, chksmbts );
	
	/* send start bytes, sync */
	for ( ind=0; ind<2; ind++ ){
		while(!(UCSR1A & (1<<UDRE1)));
		UDR1 = pgm_read_byte(ptr_to_header++);
	}	
	/*send command ID (& payload's length)*/
	for ( ind=0; ind<4; ind++ ){
		while(!(UCSR1A & (1 << UDRE1)));
		UDR1 = pgm_read_byte( id++ );	
	}	
	/*send payload*/
	for ( ind=0; ind<pl_len; ind++ ){
		while(!(UCSR1A & (1 << UDRE1)));
		UDR1 = pgm_read_byte( payload++ );
	}
	/*send checksums*/
	for ( ind=0; ind<2; ind++ ){
		while(!(UCSR1A & (1<<UDRE1)));
		UDR1 = chksmbts[ind];
	}	
	while(!(UCSR1A & (1<<UDRE1)));
}

/*************************************************************************//**
* Description: Calculates checksums for GPS commands.
* Param[in]:		id - Command ID (&payload's length), 4 bytes
* Param[in]:		payload - Commands payload (Variable length field)
* Param[in,out]:	checksum - calculated checksum
*****************************************************************************/
static void _calc_checksum_p(PGM_P id, PGM_P payload, unsigned char* checksum )
{
	unsigned char temp = 0;
	unsigned char pl_len = pgm_read_byte(id+2);//payload's length, third byte in ID

	for( unsigned char i=0; i<4; i++ )
	{
		temp = pgm_read_byte( id++ );
		checksum[0] = checksum[0] + temp;
		checksum[1] = checksum[1] + checksum[0];
	}
	for( unsigned char i=0; i<pl_len; i++ )
	{
		temp = pgm_read_byte( payload++ );
		checksum[0] = checksum[0] + temp;
		checksum[1] = checksum[1] + checksum[0];
	}
}

/*************************************************************************//**
* more information u-blox6_ReceiverDescriptionProtocolSpec.pdf
* www.u-blox.com
*****************************************************************************/
void GPS_LowerPower(void)
{
#if USE_NEO6_GPS_MODULE
	//TODO: Save Power, GPS power down
#endif	
}
/*************************************************************************//**
* more information u-blox6_ReceiverDescriptionProtocolSpec.pdf
* www.u-blox.com
*****************************************************************************/
void GPS_UpperPower(void)
{
#if USE_NEO6_GPS_MODULE
	//TODO: GPS Set Power mode 
#endif
}
/*************************************************************************//**
* more information u-blox6_ReceiverDescriptionProtocolSpec.pdf
* www.u-blox.com
*****************************************************************************/
void GPS_EcoPower(void)
{
#if USE_NEO6_GPS_MODULE
	UCSR1B &= ~(1 << RXCIE1);	/* disable GPS_uart RXC-interrupt */
	UCSR1B &= ~(1 << RXEN1);	// Turn off the receiving, we dont expect any acks
	UCSR1B |= (1 << TXEN1);		// Turn on the transmission

	/*Set Power Mode*/
	_send_cmd_p( ids.cfg_RXM_power, payloads.RXM_power_eco );
	
	UCSR1B &= ~(1 << TXEN1);	// Turn off the transmission, we don't need it anymore
	UCSR1B |= (1 << RXCIE1);	// Enable RX complete interrupt.
	UCSR1B |= (1 << RXEN1);		// Enable RX. Now we are ready receiving data from GPS again	
	_delay_ms(1);
#endif	
}

/********************************************************************
 * Descriptions: Parses the GPS time and date from GPS_RMC sentence.
 * RMC sentence if (status==Valid=='A'): 
 *		$GPRMC,hhmmss,status,latitude,N,longitude,E,spd,cog,ddmmyy,mv,mvE,mode*cs<CR><LF>
 * Empty sentence after reset if (status==Invalid=='V'):
 *		$GPRMC,,V,,,,,,,,,,N*53
 *
 * \param[in] RMC string for which to parse the timestamp and date.
 * \param[in,out] global rmc-struct.
 * \returns 0 on failure, 
 *			1 on success - if both, timestamp and date is copied to rmc-struct.
 ********************************************************************/
unsigned char GPS_GetTimeAndDate(void)
{
	unsigned char nmea_rmc[80];
	char* ptr_to_comma = 0x0000;/*we search commas from nmea_rmc-string*/
	unsigned char rc = 0;	/*return code*/

	rmc_ready_flag = 0;
	while( !rmc_ready_flag ){}

	/*jump over 7 first chars [$GPRMC,] - 8th should be first character of timestamp*/
	strcpy( (char*) nmea_rmc, (const char*) rmc_rx_data_g+7 );

#if 0 /*DEBUG*/
	db_uart_puts_p(PSTR("$GPRMC,"));
	db_uart_puts((const char*)nmea_rmc);
#endif

	/*Check for a digit (0 through 9)*/
	if ( isdigit(nmea_rmc[0]) )
	{
		/*get the timestamp and copy it to the rmc-struct*/
		strlcpy ((char*)rmc.hhmmss, (const char*)nmea_rmc, 7 );/*copies most size-1, null terminated*/
		
		/* search next ',' */
		ptr_to_comma = strchr( (const char*) nmea_rmc, (int) ',');
		ptr_to_comma++;	
		ptr_to_comma++; /*step over status char*/ 
		ptr_to_comma++;	/*step over comma*/
		
		/* now pointer points to first of latitude chars (if any) */

		unsigned char comma_cntr = 0;
		/* there is still six commas on the way towards the date chars */		
		while (1)
		{
			/* next ',' */
			ptr_to_comma = strchr( (const char*) ptr_to_comma, (int) ',');
			
			if (ptr_to_comma)
			{
				ptr_to_comma++;	/*jump over ',' */
				comma_cntr++;
				if (comma_cntr>5)
				{
					if (isdigit(*ptr_to_comma))/*(0 through 9)*/
					{
						strlcpy ((char*)rmc.ddmmyy, (const char*)ptr_to_comma, 7 );
						rc = 1;/*return code success*/
						break;
					}else break;//didn't get it
				}
				else if (comma_cntr>9)//too much
					break;
			}else break;	// ',' not found (something wrong)
		}//while
	}//if
	return(rc);
}

/********************************************************************
 * Descriptions: Parses the NMEA longitude and latitude.
 * \param[in] latest valid RMC string which to parse.
 * \param[in,out] global rmc-struct, longitude & latitude.
 * \returns 
 
 * NOTE: Check GPS-module default latitude and longitude presentation.
		 u-blox NEO-6-module default value is ddmm.mmmmm	 
 ********************************************************************/
void GPS_ProcessData(unsigned char* nmea_rmc ){
	
	unsigned char lat[11+1];	/*N-S*/
	unsigned char lon[12+1];	/*W-E*/
	unsigned char sign = '0';
	
	//db_uart_puts_p(PSTR("NMEARMC: "));
	//db_uart_puts((const char*)nmea_rmc);	
	
	
	unsigned char* ptr_to_comma = 0x0000;/*we search commas from nmea_rmc-string*/
	
	/*jump over 7 first chars [$GPRMC,]*/
	ptr_to_comma = nmea_rmc+7;//pointer is just behind in first comma

	/* jump over Time field to the next comma */
	ptr_to_comma = (unsigned char*) strchr( (const char*) ptr_to_comma, (int) ',');
	ptr_to_comma++;	/*step over comma*/
	ptr_to_comma++; /*step over status char*/
	ptr_to_comma++;	/*step over comma to the latitude field */

	/* now we are in first char of latitude */

	/* copy latitude chars */
	strlcpy ((char*)lat, (const char*)ptr_to_comma, 11 );/*copies most size-1, null terminated*/
	
	/* default latitude presentation is ddmm.mmmmm 
		we need to change it to dd.mmmmmmm 
	*/
	for (unsigned char ind=0; ind<10; ind++)
	{
		if (*ptr_to_comma == '.')
		{
			ptr_to_comma++;//step over '.'
		}
		if ( ind==2 )
		{
			lat[ind++]='.';
		}
		lat[ind] = *ptr_to_comma;
		ptr_to_comma++;
	}
	lat[11] = '\0'; //terminate
	/* now lat == dd.mmmmmmm */
	
	ptr_to_comma++;	/*step over comma to the NS-indicator*/

	/*catch NorthSouth-indicator and step*/
	if ( *ptr_to_comma == 'N'){ /*if we are in the North*/
		sign = '+';
		rmc.ns_indicator = 'N';
	} else if ( *ptr_to_comma == 'S'){ /*else we are in the South*/
		sign = '-';
		rmc.ns_indicator = 'S';
	}
	ptr_to_comma++;//step over NS-indicator
	ptr_to_comma++;//step over comma to the longitude field
	
	/* dd.mmmmmmm to dd.ddddddd */
#if 0//DEBUG
	db_uart_putc('\r');
	db_uart_puts((const char*)lat);
	db_uart_putc('\r');
#endif	
	_convert_minutes( (unsigned char*) lat+3 );
	
	/* copy latitude with sign to the rmc-struct */
	rmc.latitude[0] = sign;
	strcpy ( (char*)rmc.latitude+1, (const char*)lat); 
	rmc.latitude[11]='\0';

#if 0//DEBUG
	db_uart_puts((const char*)rmc.latitude);
	db_uart_putc('\r');
#endif
	
	/** latitude is ready, longitude still waits **********************************************/
	/* now we are in first char of longitude */

	/* copy longitude chars */
	strlcpy ((char*)lon, (const char*)ptr_to_comma, 12 );/*copies most size-1, null terminated*/

	/* move the point from dddmm.mmmmm to ddd.mmmmmmm */	
	for (unsigned char ind=0; ind<11; ind++)
	{
		if (*ptr_to_comma == '.')
		{
			ptr_to_comma++;//step over '.'
		}
		if ( ind==3)
		{
			lon[ind++]='.';
		}
		lon[ind] = *ptr_to_comma;
		ptr_to_comma++;
	}
	lon[12] = '\0';

	/* now lon == ddd.mmmmmmm */
#if 0//DEBUG
	db_uart_putc('\r');
	db_uart_puts((const char*)lon);
	db_uart_putc('\r');
#endif	
	_convert_minutes( (unsigned char*) lon+4 );
	/* and now lon == ddd.ddddddd */
	
	ptr_to_comma++;//step over comma to the EW-indicator

	if ( *ptr_to_comma == 'E'){
		sign = '+';
		rmc.ew_indicator = 'E';
	} else if ( *ptr_to_comma == 'W'){
		sign = '-';
		rmc.ew_indicator = 'W';
	}
		
	/*get the longitude and copy it with sign to the rmc-struct */
	rmc.longitude[0] = sign;
	strcpy ( (char*)rmc.longitude+1, (const char*)lon );
	rmc.longitude[12] = '\0';
	
#if 0//DEBUG
	db_uart_puts((const char*)rmc.longitude);
	db_uart_putc('\r');
#endif
}

/**************************************************************//**
 * Descriptions: Convert coordinate minutes to decimal degree presentation
 * \param [in]	minutes
 *		  [out]	decimals
 *****************************************************************/
static void _convert_minutes( unsigned char* mins ) 
{
	unsigned long int decimal = 0;
	float temp = 0.0;

	//Convert a string to a long integer.
	decimal = atol((const char*)mins);	 
	
	if (decimal>0 && decimal<6000000UL){
		temp = (float)decimal/6000000UL;
		decimal = temp*1000000;		
	} else if (decimal>=6000000UL){
		decimal = 5999999UL;
	} else decimal=0;
	
	//Convert an unsigned long integer to a string.
	mins =  (unsigned char*) ultoa( decimal, (char*)mins, 10 );
}

/**************************************************************//**
 * Description: Search RMC-sentence from NMEA data and collect data.
 *****************************************************************/	
ISR(GPS_NMEA_RX_Complete)
{
	unsigned char received_byte = 0; 

	received_byte = UDR1;
	
	if (received_byte == '$')//if '$', Always start at the beginning
	{
		_nmea_rx_ind = 0;
	}

	if ( _nmea_rx_ind<6 )// search RMC string
	{
		switch (received_byte)
		{
			case '$':	_nmea_rx_ind=1;
						break;
			
			case 'G':	if (_nmea_rx_ind==1){
							_nmea_rx_ind++;
						}else _nmea_rx_ind=0;
						break;
			
			case 'P':	if (_nmea_rx_ind==2){
							_nmea_rx_ind++;
						}else _nmea_rx_ind=0;
						break;
			
			case 'R':	if (_nmea_rx_ind==3){
							_nmea_rx_ind++;
						}else _nmea_rx_ind=0;
						break;
			
			case 'M':	if (_nmea_rx_ind==4){
							_nmea_rx_ind++;
						}else _nmea_rx_ind=0;
						break;
			
			case 'C':	if (_nmea_rx_ind==5){
							_nmea_rx_ind++;
						}else _nmea_rx_ind=0;
						break;
						
			default:	_nmea_rx_ind=0;
						break;
		}
	}
	else if(_nmea_rx_ind>5 && _nmea_rx_ind<82 /*max len*/) //collect data 
	{
			rmc_rx_data_g[_nmea_rx_ind++] = received_byte;
		
			if (received_byte == 0x0a)//end of nmea_rmc-string
			{
				rmc_rx_data_g[_nmea_rx_ind] = '\0';//terminate
				_nmea_rx_ind = 0;
				rmc_ready_flag = 1;
			}
	}
	else /* if(_nmea_rx_ind>81) didn't found 0x0a(end of), discard */
	{	
		rmc_rx_data_g[82] = '\0';//terminate
		_nmea_rx_ind = 0;
	}
}//isr