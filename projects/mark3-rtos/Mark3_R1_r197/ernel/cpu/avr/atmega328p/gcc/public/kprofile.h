/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file kprofile.h
    
    \brief Profiling timer hardware interface
*/

#include "kerneltypes.h"
#include "mark3cfg.h"
#include "ll.h"

#ifndef __KPROFILE_H__
#define __KPROFILE_H__

#if KERNEL_USE_PROFILER

//---------------------------------------------------------------------------
#define TICKS_PER_OVERFLOW              (256)
#define CLOCK_DIVIDE                    (8)

//---------------------------------------------------------------------------
/*!
    System profiling timer interface
*/
class Profiler
{
public:
    /*!
        \fn void Init()
        
        Initialize the global system profiler.  Must be 
        called prior to use.
    */
    static void Init();
    
    /*!
        \fn void Start()
        
        Start the global profiling timer service.
    */
    static void Start();
    
    /*!
        \fn void Stop()
        
        Stop the global profiling timer service
    */
    static void Stop();
    
    /*!
        \fn K_USHORT Read()
        
        Read the current tick count in the timer.  
    */
    static K_USHORT Read();
    
    /*!
        Process the profiling counters from ISR.
    */
    static void Process();
    
    /*!
        Return the current timer epoch    
    */
    static K_ULONG GetEpoch(){ return m_ulEpoch; }
private:

    static K_ULONG m_ulEpoch;
};

#endif //KERNEL_USE_PROFILER

#endif

