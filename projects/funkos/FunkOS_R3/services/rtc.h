//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file rtc.h

	Description:
		Software real-time-clock implementation 
*/
//---------------------------------------------------------------------------
#ifndef __RTC_H_
#define __RTC_H_

#include "types.h"

//---------------------------------------------------------------------------
#define DEFAULT_YEAR			(2009)
#define DEFAULT_MONTH			(0)
#define DEFAULT_DAY				(0)
#define DEFAULT_HOUR			(0)
#define DEFAULT_MIN				(0)
#define DEFAULT_SEC				(0)
#define DEFAULT_TICKS			(0)

#define RTC_TICK_PER_SECOND		(1000)

#define RTC_SEC_PER_MIN			(60)
#define RTC_MIN_PER_HOUR		(60)
#define RTC_HOUR_PER_DAY		(24)
#define RTC_MONTH_PER_YEAR		(12)

//---------------------------------------------------------------------------
typedef enum
{
	MONTH_JANUARY = 0,
	MONTH_FEBRUARY,
	MONTH_MARCH,
	MONTH_APRIL,
	MONTH_MAY,
	MONTH_JUNE,
	MONTH_JULY,
	MONTH_AUGUST,
	MONTH_SEPTEMBER,
	MONTH_OCTOBER,
	MONTH_NOVEMBER,
	MONTH_DECEMBER,
	// -- Counter
	MONTH_COUNT
} MONTHS;

//---------------------------------------------------------------------------
typedef struct
{
	// Bitfields for added tinyness
	unsigned int	uiYear:12;		//!< Years 0-4095
	unsigned int	uiMonth:4;		//!< Month 0-11	(use the enumerated values)
	
	unsigned int	uiDay:5;		//!< Day 0-30 (0 is the first day of the month)
	unsigned int	uiHour:5;		//!< Hour 0-23
	unsigned int	uiMinute:6;		//!< Minute 0-59
	
	unsigned int	uiSecond:6;		//!< Second 0-59
	unsigned int	uiTicks:16;		//!< Ticks 0-65535
} RTC_STRUCT;

//---------------------------------------------------------------------------
void RTC_Init(RTC_STRUCT *pstRTC_);
void RTC_AddTime(RTC_STRUCT *pstRTC_, USHORT usTicks_);
void RTC_SetYear(RTC_STRUCT *pstRTC_, USHORT usYear_);
void RTC_SetMonth(RTC_STRUCT *pstRTC_, UCHAR ucMonth_);
void RTC_SetDay(RTC_STRUCT *pstRTC_, UCHAR ucDay_);
void RTC_SetHour(RTC_STRUCT *pstRTC_, UCHAR ucHour_);
void RTC_SetMinute(RTC_STRUCT *pstRTC_, UCHAR ucMinute_);
void RTC_SetSecond(RTC_STRUCT *pstRTC_, UCHAR ucSecond_);
void RTC_SetTicks(RTC_STRUCT *pstRTC_, USHORT usTicks_);

#endif 
