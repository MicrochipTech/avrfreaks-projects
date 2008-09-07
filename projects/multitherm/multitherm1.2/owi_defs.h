// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\owi_defs.h,v 1.1 2007-10-28 19:36:37+08 tblough Exp tblough $

#ifndef _OWI_DEFS_
#define _OWI_DEFS_

// 1-Wire Hardware connection - change depending on setup
#define OWI_PIN PD6
#define OWI_IN PIND
#define OWI_OUT PORTD
#define OWI_DDR DDRD

// Bit timing delays from Maxim-IC Application Note 126
#define OWI_DELAY_A 6
#define OWI_DELAY_B 64
#define OWI_DELAY_C 60
#define OWI_DELAY_D 10
#define OWI_DELAY_E 9
#define OWI_DELAY_F 55
#define OWI_DELAY_G 0
#define OWI_DELAY_H 480
#define OWI_DELAY_I 70
#define OWI_DELAY_J 410

#define OWI_READ_ROM 0x33
#define OWI_MATCH_ROM 0x55
#define OWI_SKIP_ROM 0xCC
#define	OWI_SEARCH_ROM 0xF0
#define	OWI_ALARM_SEARCH 0xEC

// rom-code size including CRC
#define OWI_ROMCODE_SIZE 8

// error codes
#define OWI_BUS_OK 0x00
#define OWI_ROM_CRC_OK 0x00 
#define OWI_BUS_ERROR 0xFF
#define OWI_PRESENCE_ERROR	0xFE
#define	OWI_DATA_ERR 0xFD
#define OWI_ROM_CRC_ERROR 0xFC


#endif // _OWI_DEFS_
