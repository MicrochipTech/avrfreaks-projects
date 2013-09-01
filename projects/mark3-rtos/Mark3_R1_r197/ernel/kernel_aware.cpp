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

    \file   kernel_Aware.cpp

    \brief  Kernel aware simulation support
*/

#include "kerneltypes.h"
#include "mark3cfg.h"
#include "kernel_aware.h"
#include "threadport.h"

#if KERNEL_AWARE_SIMULATION
//---------------------------------------------------------------------------
typedef void (*ExitHandler_t)(void);

//---------------------------------------------------------------------------
typedef union
{
    volatile K_USHORT ausBuffer[5];
    struct
    {
        volatile const K_CHAR *szName;
    } Profiler;
    struct
    {
        volatile K_USHORT usFile;
        volatile K_USHORT usLine;
        volatile K_USHORT usCode;
        volatile K_USHORT usArg1;
        volatile K_USHORT usArg2;
    } Trace;
    struct
    {
        volatile const K_CHAR *szString;
    } Print;
} KernelAwareData_t;

//---------------------------------------------------------------------------
volatile K_UCHAR        g_ucKACommand;
KernelAwareData_t       g_stKAData;


//---------------------------------------------------------------------------
void Kernel_Aware::Profile_Init(const K_CHAR *szStr_)
{
    CS_ENTER();
    g_stKAData.Profiler.szName = szStr_;
    g_ucKACommand = KA_COMMAND_PROFILE_INIT;
    CS_EXIT();
}

//---------------------------------------------------------------------------
void Kernel_Aware::Profile_Start(void)
{
    g_ucKACommand = KA_COMMAND_PROFILE_START;
}

//---------------------------------------------------------------------------
void Kernel_Aware::Profile_Stop(void)
{
    g_ucKACommand = KA_COMMAND_PROFILE_STOP;
}

//---------------------------------------------------------------------------
void Kernel_Aware::Profile_Report(void)
{
    g_ucKACommand = KA_COMMAND_PROFILE_REPORT;
}

//---------------------------------------------------------------------------
void Kernel_Aware::Exit_Simulator(void)
{
    ExitHandler_t pfHandler = (ExitHandler_t)0;
    pfHandler();
}

//---------------------------------------------------------------------------
void Kernel_Aware::Trace( K_USHORT usFile_,
                          K_USHORT usLine_,
                          K_USHORT usCode_ )
{
    Trace_i( usFile_, usLine_, usCode_, 0, 0, KA_COMMAND_TRACE_0 );
}

//---------------------------------------------------------------------------
void Kernel_Aware::Trace( K_USHORT usFile_,
                          K_USHORT usLine_,
                          K_USHORT usCode_,
                          K_USHORT usArg1_)
{
    Trace_i( usFile_, usLine_, usCode_, usArg1_, 0 ,KA_COMMAND_TRACE_1 );

}
//---------------------------------------------------------------------------
void Kernel_Aware::Trace( K_USHORT usFile_,
                          K_USHORT usLine_,
                          K_USHORT usCode_,
                          K_USHORT usArg1_,
                          K_USHORT usArg2_)
{
    Trace_i( usFile_, usLine_, usCode_, usArg1_, usArg2_, KA_COMMAND_TRACE_2 );
}

//---------------------------------------------------------------------------
void Kernel_Aware::Trace_i( K_USHORT usFile_,
                          K_USHORT usLine_,
                          K_USHORT usCode_,
                          K_USHORT usArg1_,
                          K_USHORT usArg2_,
                          KernelAwareCommand_t eCmd_ )
{
    CS_ENTER();
    g_stKAData.Trace.usFile = usFile_;
    g_stKAData.Trace.usLine = usLine_;
    g_stKAData.Trace.usCode = usCode_;
    g_stKAData.Trace.usArg1 = usArg1_;
    g_stKAData.Trace.usArg2 = usArg2_;
    g_ucKACommand = eCmd_;
    CS_EXIT();
}

//---------------------------------------------------------------------------
void Kernel_Aware::Print(const K_CHAR *szStr_)
{
    CS_ENTER();
    g_stKAData.Print.szString = szStr_;
    g_ucKACommand = KA_COMMAND_PRINT;
    CS_EXIT();
}

#endif
