// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\owi_highlevel.h,v 1.1 2007-10-28 19:36:39+08 tblough Exp tblough $

#ifndef _OWI_HIGHLEVEL_
#define _OWI_HIGHLEVEL_

#include <avr/io.h>

#include "owi_defs.h"
#include "owi_lowlevel.h"

#define OWI_FAMILY( c) (c & 0x7F)	// oem/custom devices have high bit set

#define	OWI_SEARCH_FIRST 0xFF	// start new search
#define OWI_LAST_DEVICE 0x00	// last device found
// 0x01 ... 0x40: continue searching

// function declarations
extern uint8_t owi_byte_io( uint8_t b);
extern uint8_t owi_device_command( uint8_t command, uint8_t *id);
extern uint8_t owi_search( uint8_t diff, uint8_t *id, uint8_t cmd);

#endif  // _OWI_HIGHLEVEL_

