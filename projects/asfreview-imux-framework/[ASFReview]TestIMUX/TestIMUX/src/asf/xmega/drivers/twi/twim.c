/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief XMEGA TWI master source file.
 *
 * - Compiler:           GCC and IAR for AVR
 * - Supported devices:  All AVR XMega devices with TWI module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 *****************************************************************************/

/* Copyright (c) 2010 Atmel Corporation. All rights reserved.
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

#include "twim.h"

//! Pointer to the instance of the TWI registers for IT.
static volatile TWI_Master_t twi_master;

/*! \brief TWI write and/or read transaction.
 *
 *  This function is a TWI Master write and/or read transaction. The function
 *  can be used to both write and/or read bytes to/from the TWI Slave in one
 *  transaction.
 *
 *  \param address        The slave address.
 *  \param writeData      Pointer to data to write.
 *  \param bytesToWrite   Number of bytes to write.
 *  \param bytesToRead    Number of bytes to read.
 *
 *  \retval true  If transaction could be started.
 *  \retval false If transaction could not be started.
 */
bool twi_master_write_read(uint8_t address,uint8_t *writeData, uint8_t *readData,
			uint8_t bytesToWrite, uint8_t bytesToRead)
{
	/*Parameter sanity check. */
	if (bytesToWrite > TWIM_WRITE_BUFFER_SIZE) {
		return false;
	}
	if (bytesToRead > TWIM_READ_BUFFER_SIZE) {
		return false;
	}

	/*Initiate transaction if bus is ready. */
	if (twi_master.status == TWIM_STATUS_READY) {

		twi_master.status = TWIM_STATUS_BUSY;
		twi_master.result = TWIM_RESULT_UNKNOWN;

		twi_master.address = address<<1;

		twi_master.writeData = writeData;
		twi_master.readData = readData;
		
		twi_master.bytesToWrite = bytesToWrite;
		twi_master.bytesToRead = bytesToRead;
		twi_master.bytesWritten = 0;
		twi_master.bytesRead = 0;

		/* If write command, send the START condition + Address +
		 * 'R/_W = 0'
		 */
		if (twi_master.bytesToWrite > 0) {
			uint8_t writeAddress = twi_master.address & ~0x01;
			twi_master.interface->MASTER.ADDR = writeAddress;
		}

		/* If read command, send the START condition + Address +
		 * 'R/_W = 1'
		 */
		else if (twi_master.bytesToRead > 0) {
			uint8_t readAddress = twi_master.address | 0x01;
			twi_master.interface->MASTER.ADDR = readAddress;
		}
		return true;
	} else {
		return false;
	}
}

/*! \brief TWI master arbitration lost and bus error interrupt handler.
 *
 *  Handles TWI responses to lost arbitration and bus error.
 *
 */
void twi_master_arbitration_lost_bus_error_handler(void)
{
	uint8_t currentStatus = twi_master.interface->MASTER.STATUS;

	/* If bus error. */
	if (currentStatus & TWI_MASTER_BUSERR_bm) {
		twi_master.result = TWIM_RESULT_BUS_ERROR;
	}
	/* If arbitration lost. */
	else {
		twi_master.result = TWIM_RESULT_ARBITRATION_LOST;
	}

	/* Clear interrupt flag. */
	twi_master.interface->MASTER.STATUS = currentStatus | TWI_MASTER_ARBLOST_bm;

	twi_master.status = TWIM_STATUS_READY;
}

/*! \brief TWI transaction finished handler.
 *
 *  Prepares module for new transaction.
 *
 *  \param result  The result of the operation.
 */
void twi_master_transaction_finished(uint8_t result)
{
	twi_master.result = result;
	twi_master.status = TWIM_STATUS_READY;
}

/*! \brief TWI master write interrupt handler.
 *
 *  Handles TWI transactions (master write) and responses to (N)ACK.
 *
 */
void twi_master_write_handler(void)
{
	/* Local variables used in if tests to avoid compiler warning. */
	uint8_t bytesToWrite  = twi_master.bytesToWrite;
	uint8_t bytesToRead   = twi_master.bytesToRead;

	/* If NOT acknowledged (NACK) by slave cancel the transaction. */
	if (twi_master.interface->MASTER.STATUS & TWI_MASTER_RXACK_bm) {
		twi_master.interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		twi_master.result = TWIM_RESULT_NACK_RECEIVED;
		twi_master.status = TWIM_STATUS_READY;
	}

	/* If more bytes to write, send data. */
	else if (twi_master.bytesWritten < bytesToWrite) {
		uint8_t data = twi_master.writeData[twi_master.bytesWritten];
		twi_master.interface->MASTER.DATA = data;
		++twi_master.bytesWritten;
	}

	/* If bytes to read, send repeated START condition + Address +
	 * 'R/_W = 1'
	 */
	else if (twi_master.bytesRead < bytesToRead) {
		uint8_t readAddress = twi_master.address | 0x01;
		twi_master.interface->MASTER.ADDR = readAddress;
	}

	/* If transaction finished, send STOP condition and set RESULT OK. */
	else {
		twi_master.interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		twi_master_transaction_finished(TWIM_RESULT_OK);
	}
}

/*! \brief TWI master read interrupt handler.
 *
 *  This is the master read interrupt handler that takes care of
 *  reading bytes from the TWI slave.
 *
 */
void twi_master_read_handler(void)
{
	/* Fetch data if bytes to be read. */
	if (twi_master.bytesRead < TWIM_READ_BUFFER_SIZE) {
		uint8_t data = twi_master.interface->MASTER.DATA;
		twi_master.readData[twi_master.bytesRead] = data;
		twi_master.bytesRead++;
	}

	/* If buffer overflow, issue STOP and BUFFER_OVERFLOW condition. */
	else {
		twi_master.interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
		twi_master_transaction_finished(TWIM_RESULT_BUFFER_OVERFLOW);
	}

	/* Local variable used in if test to avoid compiler warning. */
	uint8_t bytesToRead = twi_master.bytesToRead;

	/* If more bytes to read, issue ACK and start a byte read. */
	if (twi_master.bytesRead < bytesToRead) {
		twi_master.interface->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
	}

	/* If transaction finished, issue NACK and STOP condition. */
	else {
		twi_master.interface->MASTER.CTRLC = TWI_MASTER_ACKACT_bm |
		                               TWI_MASTER_CMD_STOP_gc;
		twi_master_transaction_finished(TWIM_RESULT_OK);
	}
}
/*! \brief Common TWI master interrupt service routine.
 *
 *  Check current status and calls the appropriate handler.
 *
 */
void twi_master_interrupt_handler(void)
{
	uint8_t currentStatus = twi_master.interface->MASTER.STATUS;

	/* If arbitration lost or bus error. */
	if ((currentStatus & TWI_MASTER_ARBLOST_bm) ||
	    (currentStatus & TWI_MASTER_BUSERR_bm)) {

		twi_master_arbitration_lost_bus_error_handler();
	}

	/* If master write interrupt. */
	else if (currentStatus & TWI_MASTER_WIF_bm) {
		twi_master_write_handler();
	}

	/* If master read interrupt. */
	else if (currentStatus & TWI_MASTER_RIF_bm) {
		twi_master_read_handler();
	}

	/* If unexpected state. */
	else {
		twi_master_transaction_finished(TWIM_RESULT_FAIL);
	}
}

#ifdef TWIC
/*! TWIC Master Interrupt vector. */
ISR(TWIC_TWIM_vect)
{
	twi_master_interrupt_handler();
}
#endif
#ifdef TWID
/*! TWID Master Interrupt vector. */
ISR(TWID_TWIM_vect)
{
	twi_master_interrupt_handler();
}
#endif
#ifdef TWIE
/*! TWID Master Interrupt vector. */
ISR(TWIE_TWIM_vect)
{
	twi_master_interrupt_handler();
}
#endif
#ifdef TWIF
/*! TWID Master Interrupt vector. */
ISR(TWIF_TWIM_vect)
{
	twi_master_interrupt_handler();
}
#endif

int twi_master_init(TWI_t *twi, const twi_options_t *opt)
{	
  	int status = TWI_SUCCESS;
	twi->MASTER.CTRLA = TWI_MASTER_INTLVL_LO_gc |TWI_MASTER_RIEN_bm |TWI_MASTER_WIEN_bm |TWI_MASTER_ENABLE_bm;
	twi->MASTER.BAUD = opt->speed_reg;
	twi->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
	twi_master.interface = twi;
	/* Enable LO interrupt level. */
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	//Enable_global_interrupt();
  	return status;
}

int twi_master_write(TWI_t *twi, const twi_package_t *package)
{
	bool twi_status = twi_master_write_read(package->chip, package->buffer, 0, package->length, 0);
	if(twi_status == false)
	   while(1);
	while (twi_master.status != TWIM_STATUS_READY) {
	  	/* Wait until transaction is complete. */
	}
	//test if idle 
	while(twi->MASTER.STATUS != 0x01);
  
	if (twi_master.result == TWIM_RESULT_NACK_RECEIVED)
	  return TWI_RECEIVE_NACK;
	else
	  return  TWI_SUCCESS;
}


int twi_master_read(TWI_t *twi, const twi_package_t *package)
{

	  
	bool twi_status = twi_master_write_read(package->chip,0,(uint8_t*)package->buffer,  0, package->length);	
	
	if(twi_status == false)
	   while(1);
	  //test if idle 
	while(twi->MASTER.STATUS != 0x01);
	if (twi_master.result == TWIM_RESULT_NACK_RECEIVED)
	  return TWI_RECEIVE_NACK;
	else {
		 return  TWI_SUCCESS;
	}
	 
}

