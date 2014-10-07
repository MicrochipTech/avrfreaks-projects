/*
 * gps.h
 *
 * Created: 23.3.2014 15:24:20
 *  Author: PjV
 *
 * Version: 1.0  
 */ 

#ifndef GPS_H_
#define GPS_H_

/***********************************************************************//**
* GPS Configuration Commands.
* UBX Packet Structure:

|header     |ID      |LEN      |Payload|Checksum |
|Sync1|Sync2|Class|ID|Len1|Len0|Payload|CK_A|CK_B|

  fields 		size		byte	
 -------------------------------------------------
  Header		1			Sync char1
				1			Sync char2
  ID			1			Message Class
				1			Message ID
				1			Payload's Length, low byte
				1			Payload's Length, high byte
  Payload		(Variable length)
  Checksum		1			CK_A
				1			CK_B

* more information u-blox6_ReceiverDescriptionProtocolSpec.pdf
* www.u-blox.com
***************************************************************************/
const unsigned char cmd_header[2];// PROGMEM = { 0xB5/*Sync1*/, 0x62/*Sync2*/};

typedef struct /*PROGMEM*/
{
	const char cfg_RATE_period[4];	/*Set Measurement Rate Settings*/
	const char cfg_MSG_rate[4];		/*Set Message Rate(s)*/
	const char cfg_RXM_power[4];	/*RXM configuration, Power management*/
}CMD_ID_t;

typedef struct /*PROGMEM*/
{
	const char RATE_set_period_7s[6];
	const char RATE_set_period_max[6];
	const char MSG_disable_GGA[8];
	const char MSG_disable_GLL[8];
	const char MSG_disable_GSA[8];
	const char MSG_disable_GSV[8];
	const char MSG_enable_RMC[8];
	const char MSG_disable_RMC[8];
	const char MSG_disable_VTG[8];
	const char RXM_power_down[4];
	const char RXM_power_eco[4];
}CMD_PAYLOAD_t;

/***********************************************************************//**
*
***************************************************************************/
void GPS_UartInit(void);
void GPS_config(void);
void GPS_LowerPower(void);
void GPS_EcoPower(void);
void GPS_UpperPower(void);
unsigned char GPS_GetTimeAndDate(void);
void GPS_ProcessData( unsigned char* nmea_rmc );

#endif /* GPS_H_ */