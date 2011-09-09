/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief TWI driver for AVR.
 *
 * This file defines a useful set of functions for the TWI interface on AVR
 * devices.
 *
 * - Compiler:           IAR and GNU GCC for AVR
 * - Supported devices:  All AVR devices with an TWI module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef TWIM_H
#define TWIM_H

#include "compiler.h"
#include "twi_common.h"

/*! Baud register setting calculation. Formula described in datasheet. */
#define TWI_BAUD(F_SYS, F_TWI) ((F_SYS / (2 * F_TWI)) - 5)


/*! Transaction status defines. */
#define TWIM_STATUS_READY              0
#define TWIM_STATUS_BUSY               1


/*! Transaction result enumeration. */
typedef enum TWIM_RESULT_enum {
	TWIM_RESULT_UNKNOWN          = (0x00<<0),
	TWIM_RESULT_OK               = (0x01<<0),
	TWIM_RESULT_BUFFER_OVERFLOW  = (0x02<<0),
	TWIM_RESULT_ARBITRATION_LOST = (0x03<<0),
	TWIM_RESULT_BUS_ERROR        = (0x04<<0),
	TWIM_RESULT_NACK_RECEIVED    = (0x05<<0),
	TWIM_RESULT_FAIL             = (0x06<<0),
} TWIM_RESULT_t;

/*! Buffer size defines */
#define TWIM_WRITE_BUFFER_SIZE         8
#define TWIM_READ_BUFFER_SIZE          8


/*! \brief TWI master driver struct
 *
 *  TWI master struct. Holds pointer to TWI module,
 *  buffers and necessary varibles.
 */
typedef struct TWI_Master {
	TWI_t *interface;                  /*!< Pointer to what interface to use */
	register8_t address;                            /*!< Slave address */
	register8_t *writeData;                         /*!< Data to write */
	register8_t *readData;				/*!< Read data */
	register8_t bytesToWrite;                       /*!< Number of bytes to write */
	register8_t bytesToRead;                        /*!< Number of bytes to read */
	register8_t bytesWritten;                       /*!< Number of bytes written */
	register8_t bytesRead;                          /*!< Number of bytes read */
	register8_t status;                             /*!< Status of transaction */
	register8_t result;                             /*!< Result of transaction */
}TWI_Master_t;


/*! \name Error Codes for the Module
 */
//! @{
#define TWI_SUCCESS              0
#define TWI_INVALID_ARGUMENT    -1
#define TWI_ARBITRATION_LOST    -2
#define TWI_NO_CHIP_FOUND       -3
#define TWI_RECEIVE_OVERRUN     -4
#define TWI_RECEIVE_NACK        -5
#define TWI_SEND_OVERRUN        -6
#define TWI_SEND_NACK           -7
#define TWI_BUSY                -8
//! @}


/*!
 * \brief Initialize the twi master module
 *
 * \param twi   Base address of the TWI (i.e. &TWIC).
 * \param *opt  Options for initializing the twi module
 *              (see \ref twi_options_t)
 */
extern int twi_master_init(TWI_t *twi, const twi_options_t *opt);
 
/*!
 * \brief Read multiple bytes from a TWI compatible slave device
 *
 * \param twi     Base address of the TWI (i.e. &TWI_t).
 * \param package Package information and data
 *                (see \ref twi_package_t)
 * \return TWI_SUCCESS if all bytes were read, error code otherwhise
 */
extern int twi_master_read(TWI_t *twi, const twi_package_t *package);

/*!
 * \brief Write multiple bytes to a TWI compatible slave device
 *
 * \param twi       Base address of the TWI (i.e. &TWI_t).
 * \param *package  Package information and data
 *                  (see \ref twi_package_t)
 * \return TWI_SUCCESS if all bytes were written, error code otherwhise
 */
extern int twi_master_write(TWI_t *twi, const twi_package_t *package);

/*! \brief Enable Master Mode of the TWI.
 *
 * \param twi         Base address of the TWI instance.
 */
static inline void twi_master_enable(TWI_t *twi)
{
  twi->MASTER.CTRLA |= TWI_MASTER_ENABLE_bm;  
}

/*! \brief Disable Master Mode of the TWI.
 *
 * \param twi         Base address of the TWI instance.
 */
static inline void twi_master_disable(TWI_t *twi)
{
  twi->MASTER.CTRLA &= (~TWI_MASTER_ENABLE_bm);
}

#endif /* TWIM_H */
