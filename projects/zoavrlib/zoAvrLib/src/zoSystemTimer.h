//****************************************************************************************
///	File Name	: \file zoSystemTimer.h 
///	Description	: \brief Watchdog and global timing functionality.
//	Created		: 21/01/2010
//	Target MCU	: Mega 48/88/168/328
//	Author		: Sissakis Giannis
//  email		: info@01mech.com
//
//  Copyright (C) 2010 Zero One Mechatronics LP
//
//  This program is free software: you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation, 
//  version 3 of the License or any later version. This program is distributed in the hope
//  that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public 
//  License for more details: <http://www.gnu.org/licenses/>
//  
/// \ingroup driver_avr 
/// \defgroup zoSystemTimer zoSystemTimer.h : Watchdog and timing functionality.
/// 
/// \par Overview
/// The zoSystemTimer library uses the watchdog timer to provide easy watchdog 
/// functionality, but also to provide a global system timer (with a resolution of 16ms)
/// which is used for implementing time measuring routines.
///   
/// \par Watchdog functionality example 
/// \code
/// #include "zoTypes.h"
/// #include "zoSystemTimer.h"
///
/// int main(void)
/// {
///     zoSystemTimerInit();                //initializes the watchdog and related ISR
///     zoSystemTimerWatchDogInit(100);     //configure the watchdog to cause an MCU reset
///                                         //if it is not serviced every 100 ms.                                
///     zoSystemTimerWatchDogEnable();      //enable the watchdog
///
///     while(1)
///     {
///         zoSystemTimerWatchDogReset();   //service the watchdog. If for some reason 
///                                         //(ie MCU crash) this command is not executed
///                                         //the watchdog will time out and cause a reset
///     }
///
/// }
/// \endcode 
/// \par Timeout functionality example
/// \code
/// #include "zoTypes.h"
/// #include "zoSystemTimer.h"
///
/// int main(void)
/// {
///     u16 t;                                      //variable used for timing
///     zoSystemTimerInit();                        //initialize the system timer
///
///     zoSystemTimerTimeOutInit(&t);               //initialize t for time measurement
///     while( waitForSomethingtoHappen() )         //if MCU waitsForSomethingToHappen
///     {                                           //for more than 500ms, the timer t
///         if(zoSystemTimerTimeOutExpired(&t,500)) //will timeout and break the loop.
///             break;
///     }
/// }
/// \endcode
/// \todo Provide a function handler for an user function just before watchdog reset.
//****************************************************************************************
//@{


#ifndef ZO_SYSTEM_TIMER_H
#define ZO_SYSTEM_TIMER_H

#include "zoTypes.h"

/*! \brief Initializes the system timer. The watchdog 128KHz oscillator is used.  */
void zoSystemTimerInit(void);

//Watchdog functionality__________________________________________________________________ 
/*! \brief Initializes the watchdog timeout period. Since the 
    zoSystemTimerWatchDogEnable() function has been called the watchdog must be serviced
    every timeOutMiliSecond by using the zoSystemTimerWatchDogReset() function.Otherwise
    the watchdog will cause a reset of the MCU.
    \param timeOutMiliSecond The time for servicing the watchdog in miliseconds*/
void zoSystemTimerWatchDogInit(const u16 timeOutMiliSecond);

/*! \brief Enables the watchdog. Call this function if you want to enable the watchdog 
    timer to cause an MCU reset.  */
void zoSystemTimerWatchDogEnable(void);

/*! \brief Disables the watchdog. */
void zoSystemTimerWatchDogDisable(void);

/*! \brief Services (ie resets) the watchdog. */
void zoSystemTimerWatchDogReset(void);

//Timeout functionality___________________________________________________________________ 
/*! \brief Initializes the timeout functionality. The counter variable is used to keep
    track of time.
    \param *counter A local or global variable used for timing. */
void zoSystemTimerTimeOutInit(u16 *counter);

/*! \brief Checks whether timeOutMilisecond amount of time has passed since initializing 
    the timeout with the zoSystemTimerTimeOutInit() function.
    \param *counter A local or global variable used for timing. This must be the same 
    variable used to initialize the timeout.
    \param timeOutMiliSecond Time in miliseconds for the timeout to expire.
    \return True if the elapsed time since initialization is more than timeOutMiliSecond*/
bool zoSystemTimerTimeOutExpired(u16 *counter, const u16 timeOutMiliSecond);

//Time measuring functionality____________________________________________________________
/*! \brief Initializes the time measurement functionality
    \param *counter A local or global variable used for timing. */
void zoSystemTimerMeasureStart(u16 *counter);

/*! \brief Measures time since initialization. 
    \param *counter A local or global variable used for timing. 
    \return The elapsed time in miliseconds since calling the zoSystemTimerMeasureStart() 
    function.*/
u16 zoSystemTimerMeasureGet(u16 *counter);

#endif //ZO_SYSTEM_TIMER_H
//@}