#ifndef _RTC_H_
#define _RTC_H_

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>

#define READ 1
#define WRITE 0

struct rtc_time{
   char hour;
   char minute;
   char second;
   char day;
   char month;
   char weekDay;	
   char year;
   };

void rtc_init(void);
void rtc_comms(unsigned char * data, char address, char rw);
void rtc_get_time(struct rtc_time *);
void rtc_set_time(struct rtc_time *);
#endif 
