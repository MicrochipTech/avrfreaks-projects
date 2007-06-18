/*
**	Orangutan (ATmega168) Devantech SRF08 Access Library
**
**	srf08.h	'C' Include file
**
**	Steven G. Judd, Tentacle Robotics
**	sgjudd@gmail.com
**
**	V0-00	Feb-1-2007

 This work is licensed under the Creative Commons Attribution-Noncommercial-Share Alike 2.5 License.
 To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/2.5/ or 
 send a letter to:
 
 	 Creative Commons, 
	 543 Howard Street, 5th Floor, 
	 San Francisco, California, 94105, USA.

**
**	These routines use the simple TWI master implementation by Peter Fleury and available for download
**  at http://www.uoxray.uoregon.edu/orangutan/#Code_examples.  The I2c code is included with this 
**  distribution.
*/

#ifndef __SRF08_H__
#define __SRF08_H__

#include	"i2cmaster.h"

#define SRF08_MIN_GAIN        0      /* sets gain to 94   		*/
#define SRF08_MAX_GAIN        31     /* sets gain to 1025 		*/
#define SRF08_MIN_RANGE       0      /* in millimeters    		*/
#define SRF08_MAX_RANGE       10965  /* in millimeters 43*255   */

#define	SRF08_UNIT_1	0xE0
#define	SRF08_UNIT_2	0xE2
#define	SRF08_UNIT_3	0xE4
#define	SRF08_UNIT_4	0xE6
#define	SRF08_UNIT_5	0xE8
#define	SRF08_UNIT_6	0xEA
#define	SRF08_UNIT_7	0xEC
#define	SRF08_UNIT_8	0xEE

#define	SRF08_UNIT_9	0xF0
#define	SRF08_UNIT_10	0xF2
#define	SRF08_UNIT_11	0xF4
#define	SRF08_UNIT_12	0xF6
#define	SRF08_UNIT_13	0xF8
#define	SRF08_UNIT_14	0xFA
#define	SRF08_UNIT_15	0xFC
#define	SRF08_UNIT_16	0xFE

// Registers

#define SRF08_COMMAND         0
#define SRF08_LIGHT           1
#define SRF08_ECHO_1          2
#define SRF08_ECHO_2          4
#define SRF08_ECHO_3          6
#define SRF08_ECHO_4          8
#define SRF08_ECHO_5          10
#define SRF08_ECHO_6          12
#define SRF08_ECHO_7          14
#define SRF08_ECHO_8          16
#define SRF08_ECHO_9          18
#define SRF08_ECHO_10         20
#define SRF08_ECHO_11         22
#define SRF08_ECHO_12         24
#define SRF08_ECHO_13         26
#define SRF08_ECHO_14         28
#define SRF08_ECHO_15         30
#define SRF08_ECHO_16         32
#define SRF08_ECHO_17         34

// Commands

#define RANGE_INCH 	0x50
#define RANGE_CM	0x51
#define RANGE_USEC	0x52

//	Send a ping.
//
//	status = srf08_ping(SRF_UNIT_1, I2C_rangeIN, nSamples, &ucLight, ucResultArray);
//
	unsigned char srf08_ping(unsigned char, unsigned char);

//  Read the results. The SRF08 can return up to 17 integer results from a single
//  ping.
// 
//	usage:
//
//  int iResultArray[17];
//  unsigned char nSamples = sizeof(iResultArray)/2;  // size in 16-bit ints
//
//	status = srf08_echo(SRF_UNIT_1,nSamples, iResultArray);

	unsigned char srf08_echo(unsigned char, unsigned char, int *);

//	Get the light sensor value
//
//  usage:
//
//	unsigned char ucLight;
//
//	status = srf08_light(SRF_UNIT_1,&ucLight);
//
	unsigned char srf08_light(unsigned char, unsigned char *);

//  set the range cut-off
//
//  usage:
//
//	int iMaxRange=2000;		// range cut-off, in MM, max 6000
//
//  status = srf08_setRange(SRF_UNIT-`, iMaxRange);

	unsigned char srf08_setRange(unsigned char, int);

//  set the analog gain: see the Devantech documentation for an explanation.  This is best 
//  left alone for most applications.
//
//  usage:
//
//	unsigned char ucGain=31;	// default
//	
//	status = srf08_setGain(SRF_UNIT-`, ucGain);

	unsigned char srf08_setGain(unsigned char, unsigned char);

//  set the I2C address for a given SRF08 unit. If you need to connect more than one
//  SRF08 to the same bus, you will need to give each a unique address.  The device being
//  set must be the ONLY device on the bus when this routine is executed.
//
//	usage
//
//	status = srf08_setAddr(Current_I2C_Addr, Desired_I2C_Addr);

	unsigned char srf08_setAddr(unsigned char, unsigned char);

	typedef enum{ 	I2C_OK,
					I2C_NO_ACCESS,
					SRF_BADPARAM
				 }  SRF_ERR;


#endif
