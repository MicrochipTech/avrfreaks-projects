// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\ds2415.h,v 1.1 2007-10-28 19:36:32+08 tblough Exp tblough $

#ifndef _DS2415_H_
#define _DS2415_H_

#include <stdlib.h>
#include <inttypes.h>
#include "owi_highlevel.h"

// device information
#define DS2415_FAMILY_CODE 0x24

#define DS2415_CLOCK_OK 0x00
#define DS2415_CLOCK_ERROR 0xFF

#define DS2415_READ_CLOCK 0x66
#define DS2415_WRITE_CLOCK 0x99

#define DS2415_OSC_ON( ctrl_byte) (ctrl_byte | 0x0C)	// set bits 3 and 4
#define DS2415_OSC_OFF( ctrl_byte) (ctrl_byte & ~0x0C)	// clear bits 3 and 4
#define DS2415_OSC_STATUS( ctrl_byte) ((ctrl_byte & 0x08) >> 3)	// read master bit 4

typedef struct
{
	uint8_t year;	/* years since 1900 */
	uint8_t month;	/* month - [0,11] */
	uint8_t day;	/* day - [1,31] */
	uint8_t hour;	/* hours - [0,23] */
	uint8_t min;	/* minutes - [0,59] */
	uint8_t sec;	/* seconds - [0,59] */
} DS2415_date_t;

extern uint8_t DS2415_write_clock( uint8_t ctrl, uint32_t rtc, uint8_t id[]);
extern uint8_t DS2415_read_clock( uint8_t *ctrl, uint32_t *rtc, uint8_t id[]);
extern uint32_t DS2415_gregorian_to_epoch( DS2415_date_t *dt);
extern void DS2415_epoch_to_gregorian( uint32_t seconds, DS2415_date_t *dt);

// not really needed but make life easier.
// comment out DS2415_FULL_PKG if not needed to reduce footprint
//#define DS2415_FULL_PKG
#ifdef DS2415_FULL_PKG
extern uint8_t DS2415_set_osc_status( uint8_t enable, uint8_t id[]);
extern uint8_t DS2415_get_osc_status( uint8_t id[]);
extern uint8_t DS2415_get_time( DS2415_date_t *dt, uint8_t id[]);
extern uint8_t DS2415_set_time( DS2415_date_t *dt, uint8_t id[]);
#endif

#endif	// _DS2415_H_
