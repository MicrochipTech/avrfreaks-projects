//**********************************************************************//
//FILE: rtc_ds1307.h 
//AUTHOR: Adam Kadolph
//DATE:	12-5-08
//DESCRIPTION: Real Time Clock DS1307 header file to interface a RTC via TWI
//**********************************************************************//

#ifndef RTC_H
#define RTC_H

#define  DS1307_W			0xD0
#define  DS1307_R			0xD1

unsigned char rtc_register[7];
unsigned char time[9];

//RTC register declarations for the DS1307 interfaced to the AVR
#define	SECONDS		rtc_register[0]
#define	MINUTES		rtc_register[1]
#define	HOURS		rtc_register[2]
#define	DAY    		rtc_register[3]
#define	DATE   		rtc_register[4]
#define	MONTH  		rtc_register[5]
#define	YEAR   		rtc_register[6]

//function declarations to use the DS1307 RTC on the AVR
void RTCinit(void);
void RTC_read(void);
void RTC_write(void);
void RTC_getTime(void);
void RTC_getDate(void);
void RTC_displayTime(void);
void RTC_displayDate(void);
void RTC_displayDay(void);
void RTC_updateRegisters(void);
unsigned char RTC_writeTime(void);
unsigned char RTC_writeDate(void);

//define RTC constants and RAM addresses
#define RAM_MAX_SIZE	56
#define RAM_START		0x08
#define RAM_END			0x3F

#endif
