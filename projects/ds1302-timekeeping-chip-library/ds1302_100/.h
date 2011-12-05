#ifndef DS1302_H
#define DS1302_H

#include "ds1302_settings.h"

struct ymdhms_type
  {
    unsigned char year,month,day,hour,minute,second;                   //year is 0-99 which represents 2000-2099
  };

unsigned char ds1302_getbyte(unsigned char AAddress);                  //these low level functions are not mentioned in the documentation
void ds1302_setbyte(unsigned char AAddress, unsigned char AValue);     //but allow you to read or write to the ds1302 using the datasheet
                                                                       //addresses on the datasheet page 9, table 3

unsigned char ds1302_init();                                           //called before all other functions, returns 1 if ds1302 is present
                                                                       //write signature if not found

void ds1302_clear();                                                   //clears all ds1302 registers

void ds1302_reset_time();                                              //sets clock to clock is not set

unsigned char ds1302_get_time(struct ymdhms_type *ATime);              //gets the time, returns 0 if clock is not set
void ds1302_set_time(struct ymdhms_type *ATime);                       //sets the time

unsigned char ds1302_get_ram(unsigned char AAddress);                  //gets a byte from user ram
void ds1302_set_ram(unsigned char AAddress, unsigned char AValue);     //sets a byte to user ram

#endif

