// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\ds18x20.h,v 1.1 2007-10-28 19:36:30+08 tblough Exp tblough $

#ifndef _DS18X20_H_
#define _DS18X20_H_

#include <inttypes.h>

// device information
#define DS18S20_FAMILY_CODE 0x10
#define DS18B20_FAMILY_CODE 0x28

#define DS18X20_CONVERT_T	0x44
#define DS18X20_READ		0xBE
#define DS18X20_READ_PWR_SUPPLY 0xB4

#define DS18B20_CONF_REG    4
#define DS18B20_9_BIT       0
#define DS18B20_10_BIT      (1<<5)
#define DS18B20_11_BIT      (1<<6)
#define DS18B20_12_BIT      ((1<<6)|(1<<5))

// indeterminate bits in LSB if 18B20 != 12bit
#define DS18B20_9_BIT_UNDF       ((1<<0)|(1<<1)|(1<<2))
#define DS18B20_10_BIT_UNDF      ((1<<0)|(1<<1))
#define DS18B20_11_BIT_UNDF      ((1<<0))
#define DS18B20_12_BIT_UNDF      0

// conversion times in ms
#define DS18B20_TCONV_12BIT      750
#define DS18B20_TCONV_11BIT      DS18B20_TCONV_12_BIT/2
#define DS18B20_TCONV_10BIT      DS18B20_TCONV_12_BIT/4
#define DS18B20_TCONV_9BIT       DS18B20_TCONV_12_BIT/8
#define DS18S20_TCONV            DS18B20_TCONV_12_BIT

// constant to convert the fraction bits to cel*(10^-4)
#define DS18X20_FRACCONV         625

// size of the scratchpad in bytes
#define DS18X20_SP_SIZE  9
#define DS18X20_SPDATA_OK 0x01
#define DS18X20_SPDATA_ERROR 0x00

#define DS18X20_BAD_TEMP 9999


extern uint8_t DS18X20_get_power_type( uint8_t id[]);
extern uint8_t DS18X20_start_meas( uint8_t id[]);
extern int DS18X20_get_temp( uint8_t id[]);
extern int DS18X20_get_meas( uint8_t id[]);

#endif
