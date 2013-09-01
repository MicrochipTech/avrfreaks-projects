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

    \file   kernel_Aware.h

    \brief  Kernel aware simulation support
*/

#ifndef __KERNEL_AWARE_H__
#define __KERNEL_AWARE_H__

#include "kerneltypes.h"
#include "mark3cfg.h"

#if KERNEL_AWARE_SIMULATION
//---------------------------------------------------------------------------
typedef enum
{
    KA_COMMAND_IDLE = 0,
    KA_COMMAND_PROFILE_INIT,
    KA_COMMAND_PROFILE_START,
    KA_COMMAND_PROFILE_STOP,
    KA_COMMAND_PROFILE_REPORT,
    KA_COMMAND_EXIT_SIMULATOR,
    KA_COMMAND_TRACE_0,
    KA_COMMAND_TRACE_1,
    KA_COMMAND_TRACE_2,
    KA_COMMAND_PRINT
} KernelAwareCommand_t;

//---------------------------------------------------------------------------
/*!
 * \brief The Kernel_Aware class
 *
 * This class contains functions that are used to trigger kernel-aware
 * functionality within a supported simulation environment (i.e. flAVR).
 *
 * These static methods operate on a singleton set of global variables,
 * which are monitored for changes from within the simulator.  The simulator
 * hooks into these variables by looking for the correctly-named symbols in
 * an elf-formatted binary being run and registering callbacks that are called
 * whenever the variables are changed.  On each change of the command variable,
 * the kernel-aware data is analyzed and interpreted appropriately.
 *
 * If these methods are run in an unsupported simulator or on actual hardware
 * the commands generally have no effect (except for the exit-on-reset command,
 * which will result in a jump-to-0 reset).
 */
class Kernel_Aware
{
public:
    //---------------------------------------------------------------------------
    /*!
     * \brief Profile_Init
     *
     * Initializes the kernel-aware profiler.  This function instructs the
     * kernel-aware simulator to reset its accounting variables, and prepare to
     * start counting profiling data tagged to the given string.  How this is
     * handled is the responsibility of the simulator.
     *
     * \param szStr_ String to use as a tag for the profilng session.
     */
    static void Profile_Init( const K_CHAR *szStr_ );

    //---------------------------------------------------------------------------
    /*!
     * \brief Profile_Start
     *
     * Instruct the kernel-aware simulator to begin counting cycles towards the
     * current profiling counter.
     *
     */
    static void Profile_Start( void );

    //---------------------------------------------------------------------------
    /*!
     * \brief Profile_Stop
     *
     * Instruct the kernel-aware simulator to end counting cycles relative to the
     * current profiling counter's iteration.
     */
    static void Profile_Stop( void );

    //---------------------------------------------------------------------------
    /*!
     * \brief Profile_Report
     *
     * Instruct the kernel-aware simulator to print a report for its current
     * profiling data.
     *
     */
    static void Profile_Report( void );

    //---------------------------------------------------------------------------
    /*!
     * \brief Exit_Simulator
     *
     * Instruct the kernel-aware simulator to terminate (destroying the virtual
     * CPU).
     *
     */
    static void Exit_Simulator( void );

    //---------------------------------------------------------------------------
    /*!
     * \brief Print
     *
     * Instruct the kernel-aware simulator to print a char string
     *
     * \param szStr_
     */
    static void Print( const K_CHAR *szStr_ );

    //---------------------------------------------------------------------------
    /*!
     * \brief Trace
     *
     * Insert a kernel trace statement into the kernel-aware simulator's debug
     * data stream.
     *
     * \param usFile_   16-bit code representing the file
     * \param usLine_   16-bit code representing the line in the file
     * \param usCode_   16-bit data code, which indicates the line's format.
     */
    static void Trace( K_USHORT usFile_,
                  K_USHORT usLine_,
                  K_USHORT usCode_ );

    //---------------------------------------------------------------------------
    /*!
     * \brief Trace
     *
     * Insert a kernel trace statement into the kernel-aware simulator's debug
     * data stream.
     *
     * \param usFile_   16-bit code representing the file
     * \param usLine_   16-bit code representing the line in the file
     * \param usCode_   16-bit data code, which indicates the line's format
     * \param usArg1_   16-bit argument to the format string.
     */
    static void Trace( K_USHORT usFile_,
                  K_USHORT usLine_,
                  K_USHORT usCode_,
                  K_USHORT usArg1_);

    //---------------------------------------------------------------------------
    /*!
     * \brief Trace
     *
     * Insert a kernel trace statement into the kernel-aware simulator's debug
     * data stream.
     *
     * \param usFile_   16-bit code representing the file
     * \param usLine_   16-bit code representing the line in the file
     * \param usCode_   16-bit data code, which indicates the line's format
     * \param usArg1_   16-bit argument to the format string.
     * \param usArg2_   16-bit argument to the format string.
     */
    static void Trace( K_USHORT usFile_,
                  K_USHORT usLine_,
                  K_USHORT usCode_,
                  K_USHORT usArg1_,
                  K_USHORT usArg2_);

private:

   //---------------------------------------------------------------------------
   /*!
    * \brief Trace_i
    *
    * Private function by which the class's Trace() methods are reflected, which
    * allows us to realize a modest code saving.
    *
    * \param usFile_   16-bit code representing the file
    * \param usLine_   16-bit code representing the line in the file
    * \param usCode_   16-bit data code, which indicates the line's format
    * \param usArg1_   16-bit argument to the format string.
    * \param usArg2_   16-bit argument to the format string.
    * \param eCmd_     Code indicating the number of arguments to emit.
    */
    static void Trace_i( K_USHORT usFile_,
                         K_USHORT usLine_,
                         K_USHORT usCode_,
                         K_USHORT usArg1_,
                         K_USHORT usArg2_,
                         KernelAwareCommand_t eCmd_);
};

#endif

#endif
