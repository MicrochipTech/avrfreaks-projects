/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA DMA Controller driver header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA DMA driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA DMA module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 * \par Application note:
 *      AVR1304: Using the XMEGA DMA Controller
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1653 $
 * $Date: 2008-05-21 10:26:08 +0200 (on, 21 mai 2008) $  \n
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
#ifndef DMA_DRIVER_H
#define DMA_DRIVER_H

#include "avr_compiler.h"


/*! \brief This function enable the DMA module.
 *
 *  \note Each individual DMA channel must be enabled separately
 *        using the DMA_EnableChannel() function.
 */


/*! \brief This function disables the DMA module.
 *
 *  \note On-going transfers will be aborted.
 */
#define DMA_Disable()   ( DMA.CTRL &= ~DMA_ENABLE_bm )

/*! \brief This function forces a software reset of the DMA module.
 *
 *  All registers will be set to their default values. If the DMA
 *  module is enabled, it must and will be disabled before being reset.
 *  It will not be enabled afterwards.
 */
#define DMA_Reset()                 \
{	                            \
	DMA.CTRL &= ~DMA_ENABLE_bm; \
	DMA.CTRL |= DMA_RESET_bm;   \
	DMA.CTRL &= ~DMA_RESET_bm   \
}

/*! Prototyping of functions. */

void DMA_ConfigDoubleBuffering( DMA_DBUFMODE_t dbufMode );
void DMA_SetPriority( DMA_PRIMODE_t priMode );
uint8_t DMA_CH_IsOngoing( volatile DMA_CH_t * channel );
uint8_t DMA_IsOngoing( void );
uint8_t DMA_CH_IsPending( volatile DMA_CH_t * channel );
uint8_t DMA_IsPending( void );
uint8_t DMA_ReturnStatus_non_blocking( volatile DMA_CH_t * channel );
uint8_t DMA_ReturnStatus_blocking( volatile DMA_CH_t * channel );
void DMA_EnableChannel( volatile DMA_CH_t * channel );
void DMA_DisableChannel( volatile DMA_CH_t * channel );
void DMA_ResetChannel( volatile DMA_CH_t * channel );
void DMA_SetIntLevel( volatile DMA_CH_t * channel,
                      DMA_CH_TRNINTLVL_t transferInt,
                      DMA_CH_ERRINTLVL_t errorInt );
void DMA_SetupBlock( volatile DMA_CH_t * channel,
                     const void * srcAddr,
                     DMA_CH_SRCRELOAD_t srcReload,
                     DMA_CH_SRCDIR_t srcDirection,
                     void * destAddr,
                     DMA_CH_DESTRELOAD_t destReload,
                     DMA_CH_DESTDIR_t destDirection,
                     uint16_t blockSize,
                     DMA_CH_BURSTLEN_t burstMode,
                     uint8_t repeatCount,
                     bool useRepeat );
void DMA_EnableSingleShot( volatile DMA_CH_t * channel );
void DMA_DisableSingleShot( volatile DMA_CH_t * channel );
void DMA_SetTriggerSource( volatile DMA_CH_t * channel, uint8_t trigger );
void DMA_StartTransfer( volatile DMA_CH_t * channel );

#endif
