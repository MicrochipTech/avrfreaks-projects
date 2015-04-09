/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA DMA Controller driver source file.
 *
 *      This file contains the function implementations for the XMEGA DMA driver.
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

#include "dma_driver.h"


/*! \brief This function configures the double buffering feature of the DMA.
 *
 *  Channel pair 0/1 and/or channel pair 2/3 can
 *  be configured to operation in a chained mode. This means that
 *  once the first channel has completed its transfer, the second
 *  channel takes over automatically. It is important to setup the
 *  channel pair with equal block sizes, repeat modes etc.
 *
 *  Do not change these settings after a transfer has started.
 *
 *  \param  dbufMode  Double buffering mode.
 */
void DMA_ConfigDoubleBuffering( DMA_DBUFMODE_t dbufMode )
{
	DMA.CTRL = ( DMA.CTRL & ~DMA_DBUFMODE_gm ) | dbufMode;
}


/*! \brief This function selects what priority scheme to use for the DMA channels.
 *
 *  It decides what channels to schedule in a round-robin
 *  manner, which means that they take turns in acquiring the data bus
 *  for individual data transfers. Channels not included in the round-robin
 *  scheme will have fixed priorities, with channel 0 having highest priority.
 *
 *  \note  Do not change these settings after a transfer has started.
 *
 *  \param  priMode  An enum selection the priority scheme to use.
 */
void DMA_SetPriority( DMA_PRIMODE_t priMode )
{
	DMA.CTRL = ( DMA.CTRL & ~DMA_PRIMODE_gm ) | priMode;
}


/*! \brief This function checks if the channel has on-going transfers not
 *         finished yet.
 *
 *  \param  channel Channel to check.
 *
 *  \return  Non-zero if the selected channel have on-going transfers,
 *           zero otherwise.
 */
uint8_t DMA_CH_IsOngoing( volatile DMA_CH_t * channel )
{
	uint8_t flagMask;
	flagMask = channel->CTRLB & DMA_CH_CHBUSY_bm;
	return flagMask;
}

/*! \brief This function checks if any channel have on-going transfers are not
 *         finished yet.
 *
 *  \return  Non-zero if any channel have on-going transfers, zero otherwise.
 */
uint8_t DMA_IsOngoing( void )
{
	uint8_t flagMask;
	flagMask = DMA.STATUS & 0xF0;
	return flagMask;
}

/*! \brief This function check if the channel has transfers pending.
 *
 *  This function checks if the channel selected have transfers that are
 *  pending, which means that a transfer has been requested by a trigger source
 *  or by a manual request, but the channel haven't yet started its transfer.
 *
 *  \param  channel Channel to check.
 *
 *  \return  Non-zero if the selected channel have pending transfers,
 *           zero otherwise.
 */
uint8_t DMA_CH_IsPending( volatile DMA_CH_t * channel )
{
	uint8_t flagMask;
	flagMask = channel->CTRLB & DMA_CH_CHPEND_bm;
	return flagMask;
}


/*! \brief This function check if there are any transfers pending.
 *
 *  This function checks if any channel have transfers that are pending,
 *  which means that a transfer has been requested by a trigger source
 *  or by a manual request, but the channels haven't yet started its transfer.
 *
 *  \return  Non-zero if the selected channel have pending transfers,
 *           zero otherwise.
 */
uint8_t DMA_IsPending( void )
{
	uint8_t flagMask;
	flagMask = DMA.STATUS & 0x0F;
	return flagMask;
}

/*! \brief This function return the interrupt flag status of a channel.
 *
 *  This function return the status the channels selected finishes an on-going
 *  transfer or encounters an error and aborts the transfer.
 *
 *  \note  Flags covered by the channel will NOT be cleared when this
 *         function exits.
 *
 *  \param  channel  The channel to check.
 *
 *  \return  Relevant interrupt flags.
 */
uint8_t DMA_ReturnStatus_non_blocking( volatile DMA_CH_t * channel )
{
	uint8_t relevantFlags;
	relevantFlags = channel->CTRLB & (DMA_CH_ERRIF_bm | DMA_CH_TRNIF_bm);
	return relevantFlags;
}


/*! \brief This function return the interrupt flag status of a channel.
 *
 *  This function return the status of the channel selected either finishes
 *  an on-going transfer or encounters an error and aborts the transfer.
 *
 *  \note  Flags covered by the channel will be cleared when this
 *         function exits. However, it will return the flag status. This
 *         is a BLOCKING function, and will go into a dead-lock if the flags
 *         never get set.
 *
 *  \param  channel  The channel to check.
 *
 *  \return  Relevant interrupt flags.
 */
uint8_t DMA_ReturnStatus_blocking( volatile DMA_CH_t * channel )
{
	uint8_t flagMask;
	uint8_t relevantFlags;

	flagMask = DMA_CH_ERRIF_bm | DMA_CH_TRNIF_bm;

	do {
		relevantFlags = channel->CTRLB & flagMask;
	} while (relevantFlags == 0x00);

	channel->CTRLB = flagMask;
	return relevantFlags;
}

/*! \brief This function enables one DMA channel sub module.
 *
 *  \note A DMA channel will be automatically disabled
 *        when a transfer is finished.
 *
 *  \param  channel  The channel to enable.
 */
void DMA_EnableChannel( volatile DMA_CH_t * channel )
{
	channel->CTRLA |= DMA_CH_ENABLE_bm;
}


/*! \brief This function disables one DMA channel sub module.
 *
 *  \note On-going transfers will be aborted and the error flag be set if a
 *        channel is disabled in the middle of a transfer.
 *
 *  \param  channel  The channel to disable.
 */
void DMA_DisableChannel( volatile DMA_CH_t * channel )
{
	channel->CTRLA &= ~DMA_CH_ENABLE_bm;
}


/*! \brief This function forces a software reset of the DMA channel sub module.
 *
 *  All registers will be set to their default values. If the channel
 *  is enabled, it must and will be disabled before being reset.
 *  It will not be enabled afterwards.
 *
 *  \param  channel  The channel to reset.
 */
void DMA_ResetChannel( volatile DMA_CH_t * channel )
{
	channel->CTRLA &= ~DMA_CH_ENABLE_bm;
	channel->CTRLA |= DMA_CH_RESET_bm;
	channel->CTRLA &= ~DMA_CH_RESET_bm;
}


/*! \brief This function configures the interrupt levels for one DMA channel.
 *
 *  \note  The interrupt level parameter use the data type for channel 0,
 *         regardless of which channel is used. This is because we use the
 *         same function for all channel. This method relies upon channel
 *         bit fields to be located this way: CH3:CH2:CH1:CH0.
 *
 *  \param  channel      The channel to configure.
 *  \param  transferInt  Transfer Complete Interrupt Level.
 *  \param  errorInt     Transfer Error Interrupt Level.
 */
void DMA_SetIntLevel( volatile DMA_CH_t * channel,
                      DMA_CH_TRNINTLVL_t transferInt,
                      DMA_CH_ERRINTLVL_t errorInt )
{
	channel->CTRLB = (channel->CTRLB & ~(DMA_CH_ERRINTLVL_gm | DMA_CH_TRNINTLVL_gm)) |
			 transferInt | errorInt;
}


/*! \brief This function configures the necessary registers for a block transfer.
 *
 *  \note The transfer must be manually triggered or a trigger source
 *        selected before the transfer starts. It is possible to reload the
 *        source and/or destination address after each data transfer, block
 *        transfer or only when the entire transfer is complete.
 *        Do not change these settings after a transfer has started.
 *
 *  \param  channel        The channel to configure.
 *  \param  srcAddr        Source memory address.
 *  \param  srcReload      Source address reload mode.
 *  \param  srcDirection   Source address direction (fixed, increment, or decrement).
 *  \param  destAddr       Destination memory address.
 *  \param  destReload     Destination address reload mode.
 *  \param  destDirection  Destination address direction (fixed, increment, or decrement).
 *  \param  blockSize      Block size in number of bytes (0 = 64k).
 *  \param  burstMode      Number of bytes per data transfer (1, 2, 4, or 8 bytes).
 *  \param  repeatCount    Number of blocks, 0x00 if you want to repeat at infinitum.
 *  \param  useRepeat      True if reapeat should be used, false if not.
 */
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
                     bool useRepeat )
{
	channel->SRCADDR0 = (( (uint16_t) srcAddr) >> 0*8 ) & 0xFF;
	channel->SRCADDR1 = (( (uint16_t) srcAddr) >> 1*8 ) & 0xFF;
	channel->SRCADDR2 = 0;

	channel->DESTADDR0 = (( (uint16_t) destAddr) >> 0*8 ) & 0xFF;
	channel->DESTADDR1 = (( (uint16_t) destAddr) >> 1*8 ) & 0xFF;
	channel->DESTADDR2 = 0;

	channel->ADDRCTRL = (uint8_t) srcReload | srcDirection |
	                              destReload | destDirection;
	channel->TRFCNT = blockSize;
	channel->CTRLA = ( channel->CTRLA & ~( DMA_CH_BURSTLEN_gm | DMA_CH_REPEAT_bm ) ) |
	                  burstMode | ( useRepeat ? DMA_CH_REPEAT_bm : 0);

	if ( useRepeat ) {
		channel->REPCNT = repeatCount;
	}
}


/*! \brief This function enables single-shot transfer mode for a channel.
 *
 *  In single-shot mode, one transfer trigger (manual or from a trigger source)
 *  only causes one single data transfer (1, 2, 4, or 8 byte). When not
 *  in single-shot mode, one transfer trigger causes one entire block transfer.
 *  Do not change this setting after a transfer has started.
 *
 *  \param  channel  The channel to configure.
 */
void DMA_EnableSingleShot( volatile DMA_CH_t * channel )
{
	channel->CTRLA |= DMA_CH_SINGLE_bm;
}


/*! \brief This function disables single-shot transfer mode for a channel.
 *
 *  In single-shot mode, one transfer trigger (manual or from a trigger source)
 *  only causes one single data transfer (1, 2, 4, or 8 byte). When not
 *  in single-shot mode, one transfer trigger causes one entire block transfer.
 *
 *  Do not change this setting after a transfer has started.
 *
 *  \param  channel  The channel to configure.
 */
void DMA_DisableSingleShot( volatile DMA_CH_t * channel )
{
	channel->CTRLA &= ~DMA_CH_SINGLE_bm;
}


/*! \brief This function sets the transfer trigger source for a channel.
 *
 *  \note A manual transfer requests can be used even after setting a trigger
 *        source. Do not change this setting after a transfer has started.
 *
 *  \param  channel  The channel to configure.
 *  \param  trigger  The trigger source ID.
 */
void DMA_SetTriggerSource( volatile DMA_CH_t * channel, uint8_t trigger )
{
	channel->TRIGSRC = trigger;
}


/*! \brief This function sends a manual transfer request to the channel.
 *
 *  The bit will automatically clear when transfer starts.
 *
 *  \param  channel  The channel to request a transfer for.
 */
void DMA_StartTransfer( volatile DMA_CH_t * channel )
{
	channel->CTRLA |= DMA_CH_TRFREQ_bm;
}
