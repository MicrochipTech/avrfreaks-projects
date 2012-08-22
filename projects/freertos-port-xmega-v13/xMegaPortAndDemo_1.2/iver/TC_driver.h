/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA Timer/Counter driver header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA Timer/Counter driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA Timer/Counter module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 * \par Application note:
 *      AVR1306: Using the XMEGA Timer/Counter
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1569 $
 * $Date: 2008-04-22 13:03:43 +0200 (ti, 22 apr 2008) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#ifndef TC_DRIVER_H
#define TC_DRIVER_H

#include "avr_compiler.h"

/* Definition of macros. */

/*! \brief Enables the event delay for this TC.
 *
 *  This macro enables a one clock cycle delay of event sources for this TC.
 *  Mainly used for the high word TC when two TCs are cascaded to form a 32-bit
 *  TC.
 *
 *  \param _tc  The Timer/Counter to enable delay on.
 */
#define TC_EnableEventDelay( _tc )  ( (_tc)->CTRLD |= TC0_EVDLY_bm )

/*! \brief  This macro disables the event delay for this TC.
 *
 *  \param _tc  The Timer/Counter to disable delay on.
 */
#define TC_DisableEventDelay( _tc ) ( (_tc)->CTRLD &= ~TC0_EVDLY_bm )

/*! \brief Locks automatic updating of compare and period registers.
 *
 *  This macro will lock automatic updates of compare registers from the
 *  corresponding buffer registers. To enable automatic updates again, use the
 *  \ref TC_UnlockCompareUpdate macro.
 *
 *  \param _tc  Timer/Counter module instance.
 */
#define TC_LockCompareUpdate( _tc ) ( (_tc)->CTRLFSET = TC0_LUPD_bm )

/*! \brief Unlocks automatic updating of compare and period registers.
 *
 *  This macro will unlock automatic updates of compare registers from the
 *  corresponding buffer registers. Note that the output compare registers will
 *  not be updated until an update event occurs, e.g. that the counter hits
 *  TOP or BOTTOM. To force an update of the compare register from the buffer
 *  register, use the \ref TC_ForceUpdate macro.
 *
 *  \param _tc               Timer/Counter module instance.
 */
#define TC_UnlockCompareUpdate( _tc ) ( (_tc)->CTRLFCLR = TC0_LUPD_bm )


/*! \brief Force an update of the output compare and period registers.
 *
 *  This macro will trigger a transfer from the output compare and period
 *  buffer registers ( CCxBUF and PERBUF ) to the output compare and period
 *  registers ( CCx and PER ). Calling this macro has the same effect as an
 *  update condition.
 *
 *  \param _tc    Timer/Counter module instance.
 */
#define TC_ForceUpdate( _tc ) ( (_tc)->CTRLFSET = TC_CMD_UPDATE_gc )

/*! \brief Restart the Timer/Counter.
 *
 *  This macro will restart the timer. The effect of running this command is:
 *    - The CNT[H:L] register is cleared.
 *    - The direction (DIR ) is reset. (Next clock cycle will increase the counter )
 *    - All compare outputs are set to 0.
 *    - If a DTI module is connected to the timer, it will be reset as well.
 *
 *  \param _tc    Timer/Counter module instance.
 */
#define TC_Restart( _tc ) ( (_tc)->CTRLFSET = TC_CMD_RESTART_gc )

/*! \brief Manually set the count.
 *
 *  This macro writes a new value to the CNT[H:L] register.
 *  Note that the CNT[H:L] register is not double buffered, so it is recommended
 *  that the timer is not running when this macro is used.
 *
 *  \param _tc               Timer/Counter module instance.
 *  \param _count            New count value.
 */
#define TC_SetCount( _tc, _count ) ( (_tc)->CNT = (_count) )

/*! \brief Sets the timer period.
 *
 *  This macro sets a new timer period. The period buffer register is not
 *  used, so the new period will be valid immediately after the 16-bit write
 *  is finished.
 *
 *  \param _tc               Timer/Counter module instance.
 *  \param _period           New Timer/Counter period.
 */
#define TC_SetPeriod( _tc, _period ) ( (_tc)->PER = (_period) )

/*! \brief Sets the timer period ( double buffered ).
 *
 *  This macro sets a new timer period. The period buffer register is used,
 *  so the new period will be valid after the next update condition.
 *
 *  \param _tc               Timer/Counter 0 or 1 module instance.
 *  \param _period           New Timer/Counter period.
 */
#define TC_SetPeriodBuffered( _tc, _period ) ( (_tc)->PERBUF = (_period) )

/*! \brief Set new compare value for compare channel A. ( Double buffered )
 *
 *  This macro sets a new compare value for compare channel A. The compare
 *  channel A buffer register is used, so the new period will be valid from the
 *  next update condition.
 *
 *  \param _tc               Timer/Counter 0 or 1 module instance.
 *  \param _compareValue     New compare value for compare channel A.
 */
#define TC_SetCompareA( _tc, _compareValue ) ( (_tc)->CCABUF = (_compareValue) )

/*! \brief Set new compare value for compare channel B. ( Double buffered )
 *
 *  This macro sets a new compare value for compare channel B. The compare
 *  channel B buffer register is used, so the new period will be valid from the
 *  next update condition.
 *
 *  \param _tc               Timer/Counter 0 or 1 module instance.
 *  \param _compareValue     New compare value for compare channel B.
 */
#define TC_SetCompareB( _tc, _compareValue ) ( (_tc)->CCBBUF = (_compareValue) )

/*! \brief Set new compare value for compare channel C. ( Double buffered )
 *
 *  This macro sets a new compare value for compare channel C. The compare
 *  channel C buffer register is used, so the new period will be valid from the
 *  next update condition.
 *
 *  \note The CCC channel is not available on TCx1.
 *
 *  \param _tc               Timer/Counter 0 module instance.
 *  \param _compareValue     New compare value for compare channel C.
 */
#define TC_SetCompareC( _tc, _compareValue ) ( (_tc)->CCCBUF = (_compareValue) )

/*! \brief Set new compare value for compare channel D. ( Double buffered )
 *
 *  This macro sets a new compare value for compare channel D. The compare
 *  channel D buffer register is used, so the new period will be valid from the
 *  next update condition.
 *
 *  \note The CCD channel is not available on TCx1.
 *
 *  \param _tc               Timer/Counter 0 module instance.
 *  \param _compareValue     New compare value for compare channel D.
 */
#define TC_SetCompareD( _tc, _compareValue ) ( (_tc)->CCDBUF = (_compareValue) )

/*! \brief Test whether an overflow has occurred.
 *
 *  The return value of this macro indicates if an overflow has occurred.
 *
 *  \param  _tc Timer/Counter module instance.
 *
 *  \return Non-zero if overflow flag is set, zero otherwise.
 */
#define TC_GetOverflowFlag( _tc ) ( (_tc)->INTFLAGS & TC0_OVFIF_bm )

/*! \brief Clears the Timer/Counter overflow flag.
 *
 *   This macro clears the Timer/Counter overflow flag.
 *
 *  \param _tc  Timer/Counter module instance.
 */
#define TC_ClearOverflowFlag( _tc ) ( (_tc)->INTFLAGS = TC0_OVFIF_bm )

/*! \brief Test whether an error has occurred.
 *
 *  The return value of this macro indicates if an error has occurred.
 *
 *  \param _tc  Timer/Counter module instance.
 *
 *  \return Non-zero if the Timer error flag is set.
 */
#define TC_GetErrorFlag( _tc ) ( (_tc)->INTFLAGS & TC0_ERRIF_bm )

/*! \brief Clears the Timer/Counter error flag.
 *
 *   This macro clears the Timer/Counter error flag.
 *
 *  \param _tc  Timer/Counter module instance.
 */
#define TC_ClearErrorFlag( _tc ) ( (_tc)->INTFLAGS = TC0_ERRIF_bm )

/*! \brief Get the status for Compare or Capture channel A.
 *
 *  When the TC is in Input Capture mode, the return value of this macro
 *  indicates whether there is an unread input capture value available for
 *  capture channel A.
 *
 *  When the TC is in any other mode, the return value of this macro
 *  indicates whether there has been a compare match between channel A and the
 *  counter.
 *
 *  \param _tc  Timer/Counter 0 or 1 module instance.
 *
 *  \return Non-zero if Compare or capture has occured, zero otherwise.
 */
#define TC_GetCCAFlag( _tc ) ( (_tc)->INTFLAGS & TC0_CCAIF_bm )

/*! \brief Clears the Timer/Counter compare or capture A interrupt flag.
 *
 *  \param _tc   Timer/Counter 0 or 1 module instance.
 */
#define TC_ClearCCAFlag( _tc ) ( (_tc)->INTFLAGS = TC0_CCAIF_bm )

/*! \brief Get the status for Compare or Capture channel B.
 *
 *  When the TC is in Input Capture mode, the return value of this macro
 *  indicates whether there is an unread input capture value available for
 *  capture channel B.
 *
 *  When the TC is in any other mode, the return value of this macro
 *  indicates whether there has been a compare match between channel B and the
 *  counter.
 *
 *  \param _tc   Timer/Counter 0 or 1 module instance.
 *
 *  \return Non-zero if Compare or capture has occured, zero otherwise.
 */
#define TC_GetCCBFlag( _tc ) ( (_tc)->INTFLAGS & TC0_CCBIF_bm )

/*! \brief Clears the Timer/Counter compare or capture B interrupt flag.
 *
 *  \param _tc   Timer/Counter 0 or 1 module instance.
 */
#define TC_ClearCCBFlag( _tc ) ( (_tc)->INTFLAGS = TC0_CCBIF_bm )

/*! \brief Get the status for Compare or Capture channel C.
 *
 *  When the TC is in Input Capture mode, the return value of this macro
 *  indicates whether there is an unread input capture value available for
 *  capture channel C.
 *
 *  When the TC is in any other mode, the return value of this macro
 *  indicates whether there has been a compare match between channel C and the
 *  counter.
 *
 *  \note The CCC channel is not available on TCx1.
 *
 *  \param _tc    Timer/Counter 0 module instance.
 *
 *  \return       Non-zero if Compare or capture has occured, zero otherwise.
 */
#define TC_GetCCCFlag( _tc ) ( (_tc)->INTFLAGS & TC0_CCCIF_bm )

/*! \brief Clears the Timer/Counter compare or capture C interrupt flag.
 *
 *   This macro clears the Timer/Counter compare or capture C interrupt flag.
 *
 *  \note The CCC channel is not available on TCx1.
 *
 *  \param _tc   Timer/Counter 0 module instance.
 */
#define TC_ClearCCCFlag( _tc ) ( (_tc)->INTFLAGS = TC0_CCCIF_bm )

/*! \brief Get the status for Compare or Capture channel D.
 *
 *  When the TC is in Input Capture mode, the return value of this macro
 *  indicates whether there is an unread input capture value available for
 *  capture channel D.
 *
 *  When the TC is in any other mode, the return value of this macro
 *  indicates whether there has been a compare match between channel D and the
 *  counter.
 *
 *  \note The CCD channel is not available on TCx1.
 *
 *  \param _tc    Timer/Counter 0 module instance.
 *
 *  \return       Non-zero if Compare or capture has occured, zero otherwise.
 */
#define TC_GetCCDFlag( _tc ) ( (_tc)->INTFLAGS & TC0_CCDIF_bm )

/*! \brief Clears the Timer/Counter compare or capture D interrupt flag.
 *
 *   This macro clears the Timer/Counter compare or capture D interrupt flag.
 *
 *  \note The CCD channel is not available on TCx1.
 *
 *  \param _tc    Timer/Counter 0 module instance.
 */
#define TC_ClearCCDFlag( _tc ) ( (_tc)->INTFLAGS = TC0_CCDIF_bm )


/*! \brief Reads the first available input capture value for channel A.
 *
 *  This macro returns the first available input capture value for
 *  capture channel A.
 *
 *  \param _tc    Timer/Counter 0 or 1 module instance.
 *
 *  \return       The first available input capture value for channel A.
 */
#define TC_GetCaptureA( _tc ) ( (_tc)->CCA )

/*! \brief Reads the first available input capture value for channel B.
 *
 *  This macro returns the first available input capture value for
 *  capture channel B.
 *
 *  \param _tc    Timer/Counter 0 or 1 module instance.
 *
 *  \return       The first available input capture value for channel B.
 */
#define TC_GetCaptureB( _tc ) ( (_tc)->CCB )

/*! \brief Reads the first available input capture value for channel C.
 *
 *  This macro returns the first available input capture value for
 *  capture channel C.
 *
 *  \note The CCC channel is not available on TCx1.
 *
 *  \param _tc    Timer/Counter 0 module instance.
 *
 *  \return       The first available input capture value for channel C.
 */
#define TC_GetCaptureC( _tc ) ( (_tc)->CCC )

/*! \brief Reads the first available input capture value for channel D.
 *
 *  This macro returns the first available input capture value for
 *  capture channel D.
 *
 *  \note The CCD channel is not available on TCx1.
 *
 *  \param _tc    Timer/Counter 0 module instance.
 *
 *  \return       The first available input capture value for channel D.
 */
#define TC_GetCaptureD( _tc ) ( (_tc)->CCD )


/* Prototyping of functions. Documentation can be found in source file. */

void TC0_ConfigClockSource( volatile TC0_t * tc, TC_CLKSEL_t clockSelection );
void TC0_ConfigWGM( volatile TC0_t * tc, TC_WGMODE_t wgm );
void TC0_ConfigInputCapture( volatile TC0_t * tc, TC_EVSEL_t eventSource );
void TC0_EnableCCChannels( volatile TC0_t * tc, uint8_t enableMask );
void TC0_DisableCCChannels( volatile TC0_t * tc, uint8_t disableMask );
void TC0_SetOverflowIntLevel( volatile TC0_t * tc, TC_OVFINTLVL_t intLevel );
void TC0_SetErrorIntLevel( volatile TC0_t * tc, TC_ERRINTLVL_t intLevel );
void TC0_SetCCAIntLevel( volatile TC0_t * tc, TC_CCAINTLVL_t intLevel );
void TC0_SetCCBIntLevel( volatile TC0_t * tc, TC_CCBINTLVL_t intLevel );
void TC0_SetCCCIntLevel( volatile TC0_t * tc, TC_CCCINTLVL_t intLevel );
void TC0_SetCCDIntLevel( volatile TC0_t * tc, TC_CCDINTLVL_t intLevel );
void TC0_Reset( volatile TC0_t * tc );

void TC1_ConfigClockSource( volatile TC1_t * tc, TC_CLKSEL_t clockSelection );
void TC1_ConfigWGM( volatile TC1_t * tc, TC_WGMODE_t wgm );
void TC1_ConfigInputCapture( volatile TC1_t * tc, TC_EVSEL_t eventSource );
void TC1_EnableCCChannels( volatile TC1_t * tc, uint8_t enableMask );
void TC1_DisableCCChannels( volatile TC1_t * tc, uint8_t disableMask );
void TC1_SetOverflowIntLevel( volatile TC1_t * tc, TC_OVFINTLVL_t intLevel );
void TC1_SetErrorIntLevel( volatile TC1_t * tc, TC_ERRINTLVL_t intLevel );
void TC1_SetCCAIntLevel( volatile TC1_t * tc, TC_CCAINTLVL_t intLevel );
void TC1_SetCCBIntLevel( volatile TC1_t * tc, TC_CCBINTLVL_t intLevel );
void TC1_SetCCCIntLevel( volatile TC1_t * tc, TC_CCCINTLVL_t intLevel );
void TC1_SetCCDIntLevel( volatile TC1_t * tc, TC_CCDINTLVL_t intLevel );
void TC1_Reset( volatile TC1_t * tc );


#endif
