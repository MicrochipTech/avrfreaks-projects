//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
	\file rtc.c

	Description:
		Software real-time-clock implementation 
*/
//---------------------------------------------------------------------------
#include "rtc.h"
#include "types.h"

//---------------------------------------------------------------------------
// Create a list of all of the days in each month, used for determining when
// to rollover from one month to the next
//---------------------------------------------------------------------------
const static USHORT ausDaysPerMonth[MONTH_COUNT] = 
{
	31, // Jan
	28, // Feb
	31, // March
	30, // April
	31, // May
	30, // June
	31, // July
	31, // August
	30, // September
	31, // October
	30, // November
	31	// December
};

//---------------------------------------------------------------------------
// Private declarations
//---------------------------------------------------------------------------
static BOOL RTC_IsLeapYear(unsigned int uiYear_);

//---------------------------------------------------------------------------
// Define this to build test code for static validation
//---------------------------------------------------------------------------
//#define RTC_TEST

#ifdef RTC_TEST
//---------------------------------------------------------------------------
// Private declarations
//---------------------------------------------------------------------------
void RTC_PrintTime(RTC_STRUCT *pstRTC_);

int main(void)
{
	RTC_STRUCT stClock;
	RTC_Init(&stClock);
	printf("---Default Time---\n");
	RTC_PrintTime(&stClock);
	
	// Test boundary conditions (using leap years)
	// 1) Normal, non leap-year
	printf("---No leap year, Feb-March rollover ---\n");
	RTC_SetMonth(&stClock, MONTH_FEBRUARY);
	RTC_SetDay(&stClock, 28);
	RTC_SetHour(&stClock, 23);
	RTC_SetMinute(&stClock, 59);
	RTC_SetSecond(&stClock, 59);
	RTC_PrintTime(&stClock);
	
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
	
	// 2) Leap year, 4/year modulo
	printf("---Leap year, Feb 29 test (4 yr)---\n");
	RTC_SetMonth(&stClock, MONTH_FEBRUARY);
	RTC_SetYear(&stClock, 2008);
	RTC_SetDay(&stClock, 28);
	RTC_SetHour(&stClock, 23);
	RTC_SetMinute(&stClock, 59);
	RTC_SetSecond(&stClock, 59);
	RTC_PrintTime(&stClock);
	
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
	
	// Verify leap-year month rollover
	printf("---leap year, month rollover ---\n");
	RTC_SetHour(&stClock, 23);
	RTC_SetMinute(&stClock, 59);
	RTC_SetSecond(&stClock, 59);
	RTC_PrintTime(&stClock);
	
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
	
	// 3) No leap year, 100/year modulo
	printf("---No Leap year, rollover test (100 yr)---\n");
	RTC_SetMonth(&stClock, MONTH_FEBRUARY);
	RTC_SetYear(&stClock, 2100);
	RTC_SetDay(&stClock, 28);
	RTC_SetHour(&stClock, 23);
	RTC_SetMinute(&stClock, 59);
	RTC_SetSecond(&stClock, 59);
	RTC_PrintTime(&stClock);
	
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
		
	// 4) Leap year, 400/year modulo
	printf("---Leap year, Feb 29 test (400 yr)---\n");
	RTC_SetMonth(&stClock, MONTH_FEBRUARY);
	RTC_SetYear(&stClock, 2000);
	RTC_SetDay(&stClock, 28);
	RTC_SetHour(&stClock, 23);
	RTC_SetMinute(&stClock, 59);
	RTC_SetSecond(&stClock, 59);
	RTC_PrintTime(&stClock);
	
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
	
	// 4) End of year test
	printf("---End of year---\n");
	RTC_SetMonth(&stClock, MONTH_DECEMBER);
	RTC_SetYear(&stClock, 2009);
	RTC_SetDay(&stClock, 31);
	RTC_SetHour(&stClock, 23);
	RTC_SetMinute(&stClock, 59);
	RTC_SetSecond(&stClock, 59);
	RTC_PrintTime(&stClock);
	
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
	RTC_AddTime(&stClock, 750);
	RTC_PrintTime(&stClock);
		
	return 0;
}
//---------------------------------------------------------------------------
/*!
	Displays the RTC's current time in an HH:MM:SS.Tick, MM/DD/YYYY format
	\fn void RTC_PrintTime(RTC_STRUCT *pstRTC_)
	\param pstRTC_ - pointer to the realtime clock object
*/
void RTC_PrintTime(RTC_STRUCT *pstRTC_)
{
	printf("%02d:%02d:%02d.%d, %02d/%02d/%04d\n",
		pstRTC_->uiHour,
		pstRTC_->uiMinute,
		pstRTC_->uiSecond,
		pstRTC_->uiTicks,
		pstRTC_->uiMonth + 1,
		pstRTC_->uiDay + 1,
		pstRTC_->uiYear);
}
#endif

//---------------------------------------------------------------------------
/*!
	Initialize the RTC object to the default time specified in rtc.h
	\fn void RTC_Init(RTC_STRUCT *pstRTC_)
	\param pstRTC_ - pointer to the realtime clock object
*/
void RTC_Init(RTC_STRUCT *pstRTC_)
{
	pstRTC_->uiYear = 	DEFAULT_YEAR;
	pstRTC_->uiMonth =	DEFAULT_MONTH;
	pstRTC_->uiDay = 	DEFAULT_DAY;
	pstRTC_->uiHour	=	DEFAULT_HOUR;
	pstRTC_->uiMinute =	DEFAULT_MIN;
	pstRTC_->uiSecond =	DEFAULT_SEC;
	pstRTC_->uiTicks = 	DEFAULT_TICKS;
}

//---------------------------------------------------------------------------
/*!
	This function drives the real-time clock by adding a specified number of 
	system ticks to the clock.  As a result, you have to make sure this is 
	called regularly from a semi-fixed, high-priority context. calls to this 
	function require that no rollovers in the tick count can occur inbetween
	calls.  Ensure that no object can access the RTC object while this function
	is running.  Make sure you use a Mutex to protect the resource if necessary.
	\fn void RTC_AddTime(RTC_STRUCT *pstRTC_, USHORT usTicks_)
	\param pstRTC_ - pointer to the realtime clock object
	\param usTicks_ - number of system ticks to add
*/
void RTC_AddTime(RTC_STRUCT *pstRTC_, USHORT usTicks_)
{
	// Increment the total ticks for the clock object
	pstRTC_->uiTicks += usTicks_;
	
	// Test ticks / second
	while (pstRTC_->uiTicks >= RTC_TICK_PER_SECOND)
	{	
		pstRTC_->uiTicks-= RTC_TICK_PER_SECOND;
	
		// Test seconds/minute
		pstRTC_->uiSecond++;
		if (pstRTC_->uiSecond < RTC_SEC_PER_MIN)
		{
			continue;
		}
		pstRTC_->uiSecond = 0;
		
		// Test mintues/hour
		pstRTC_->uiMinute++;
		if (pstRTC_->uiMinute < RTC_MIN_PER_HOUR)
		{
			continue;
		}
		pstRTC_->uiMinute = 0;
		
		// Test hours/day
		pstRTC_->uiHour++;
		if (pstRTC_->uiHour < RTC_HOUR_PER_DAY)
		{
			continue;
		}
		pstRTC_->uiHour = 0;
		
		// Test days/month		
		pstRTC_->uiDay++;
		if (pstRTC_->uiDay < ausDaysPerMonth[pstRTC_->uiMonth])
		{
			continue;
		}
		
		// Check for special case - leap year
		if (pstRTC_->uiMonth == MONTH_FEBRUARY)
		{
			if (RTC_IsLeapYear(pstRTC_->uiYear))
			{
				if (pstRTC_->uiDay < (ausDaysPerMonth[pstRTC_->uiMonth] + 1))
				{
					continue;
				}
			}
		}
		pstRTC_->uiDay = 0;
		
		// Test Months/Year
		pstRTC_->uiMonth++;
		if (pstRTC_->uiMonth < RTC_MONTH_PER_YEAR)
		{
			continue;
		}
		pstRTC_->uiMonth = 0;
		
		// Increment counter
		pstRTC_->uiYear++;

		// The world ends in 2012 anyways, so don't bother checking further.
	}
}
//---------------------------------------------------------------------------
/*!
	\fn void RTC_SetYear(RTC_STRUCT *pstRTC_, USHORT usYear_)
	\param pstRTC_ - pointer to the realtime clock object
	\param usYear_ - The new year value to set
*/
void RTC_SetYear(RTC_STRUCT *pstRTC_, USHORT usYear_)
{
	pstRTC_->uiYear = usYear_;
}

//---------------------------------------------------------------------------
/*!
	Sets the new month of the year.  We index the months 0-11, not 
	1-12 as in digital calendar displays.  Use the enumerate MONTHS type to 
	set the month properly.
	\fn void RTC_SetMonth(RTC_STRUCT *pstRTC_, UCHAR ucMonth_)
	\param pstRTC_ - pointer to the realtime clock object
	\param ucMonth_ - The new month value to set
*/
void RTC_SetMonth(RTC_STRUCT *pstRTC_, UCHAR ucMonth_)
{
	pstRTC_->uiMonth = ucMonth_;
}
//---------------------------------------------------------------------------
/*!
	Set the day of the month.  The RTC struct stores the time as a value starting
	at 0 for the first day of the month, but the input to this function is the
	calendar day to be more intuitive.
	\fn void RTC_SetDay(RTC_STRUCT *pstRTC_, UCHAR ucDay_)
	\param pstRTC_ - pointer to the realtime clock object
	\param ucDay_ - The new day value to set (calendar date)
*/
void RTC_SetDay(RTC_STRUCT *pstRTC_, UCHAR ucDay_)
{
	//!< we index from day 0, not day 1 of the month
	pstRTC_->uiDay = ucDay_ - 1;
}
//---------------------------------------------------------------------------
/*!
	\fn void RTC_SetHour(RTC_STRUCT *pstRTC_, UCHAR ucHour_)
	\param pstRTC_ - pointer to the realtime clock object
	\param ucHour_ - The new hour value to set
*/
void RTC_SetHour(RTC_STRUCT *pstRTC_, UCHAR ucHour_)
{
	pstRTC_->uiHour = ucHour_;
}
//---------------------------------------------------------------------------
/*!
	\fn void RTC_SetMinute(RTC_STRUCT *pstRTC_, UCHAR ucMinute_)
	\param pstRTC_ - pointer to the realtime clock object
	\param ucMinute_ - The new minute value to set
*/
void RTC_SetMinute(RTC_STRUCT *pstRTC_, UCHAR ucMinute_)
{
	pstRTC_->uiMinute = ucMinute_;
}
//---------------------------------------------------------------------------
/*!
	\fn void RTC_SetSecond(RTC_STRUCT *pstRTC_, UCHAR ucSecond_)
	\param pstRTC_ - pointer to the realtime clock object
	\param ucSecond_ - The new second value to set
*/
void RTC_SetSecond(RTC_STRUCT *pstRTC_, UCHAR ucSecond_)
{
	pstRTC_->uiSecond = ucSecond_;
}
//---------------------------------------------------------------------------
/*!
	\fn void RTC_SetTicks(RTC_STRUCT *pstRTC_, USHORT usTicks_)
	\param pstRTC_ - pointer to the realtime clock object
	\param usTicks_ - The tick value to set
*/
void RTC_SetTicks(RTC_STRUCT *pstRTC_, USHORT usTicks_)
{
	pstRTC_->uiTicks = usTicks_;
}
//---------------------------------------------------------------------------
/*!
	Check to see if the specified year is a leap year
	\fn static BOOL RTC_IsLeapYear(unsigned int uiYear_)
	\param uiYear_ - The year to check
	\return TRUE if the year is leap year, FALSE otherwise
*/
static BOOL RTC_IsLeapYear(unsigned int uiYear_)
{
	BOOL bLeapYear = FALSE;
	
	// Leap year every 400 years, but not other 100 year intervals
	// And every 4 years otherwise
	if ((uiYear_ % 400) == 0)
	{
		bLeapYear = TRUE;
	}
	else if ((uiYear_ % 100) == 0)
	{
		bLeapYear = FALSE;
	}
	else if ((uiYear_ % 4) == 0)
	{
		bLeapYear = TRUE;
	}
	return bLeapYear;
}
