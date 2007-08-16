/*

	 TWI functions (twi.c) for Atmega16 and WINAVR

	 Version 0.1ß		2005-10-07

	 Copyright (C) Andreas Kingbäck 2005 (andki@itn.liu.se)

   	 This program is free software; you can redistribute it and/or modify it 
	 under the terms of the GNU General Public License as published by the 
	 Free Software Foundation; either version 2 of the License, or 
	 (at your option) any later version.

   	 This program is distributed in the hope that it will be useful, 
	 but WITHOUT ANY WARRANTY; without even the implied warranty of 
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	 See the GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License along with this program; 
	 if not, write to the Free Software Foundation, Inc., 
	 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#pragma language=extended

#ifndef ENABLE_BIT_DEFINITIONS
#define ENABLE_BIT_DEFINITIONS
// Enable the bit definitions in the iom128.h file
#endif


#include "twi.h"
#include "servo.h"

unsigned char TWI_status;

void TWI_Init( void )
{
	TWAR = ULTRACONTROLER_TWI_ADDRESS << 1;				// TWI address 
	TWCR |= (1<<TWEN) | (1<<TWIE) | (1<<TWEA); 	// Enable TWI interface
	TWBR = 0x32;																	// (cpu/scl-16)/2*4^twps  (SCL Rate =100KHz, TWPS=0)  = 32
	TWSR = 0x00;																	// Bits 7..3 – TWS: TWI Status = cleared 
																								// Bit 2 – Res: Reserved Bit
																								// Bits 1..0 – TWPS: TWI Prescaler Bits = 0
	//twi_data = data_buffer;
}

void TWI_SendStartCondition( void )
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE);
}

void TWI_SendStopCondition( void )
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	//TWCR |= (1<<TWINT)|(1<<TWSTO);
}


void TWI_SendAddress(unsigned char address)
{
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);
}


void TWI_SendData(unsigned char data)
{
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
}

	
SIGNAL(SIG_2WIRE_SERIAL)
{
		
	__disable_interrupt;	

	TWI_status = TWSR & TW_STATUS_MASK;											// Mask out prescaler bits to get TWI status
	asm volatile("nop"::);
	
	switch(TWI_status) {
	
	// ---------------- Slave Transmitter -------------------
	
	case TW_BUS_ERROR:																			// Bus error
			asm volatile("nop"::);
			TWCR |= (1<<TWSTO)|(1<<TWINT); 				  					// A bus error occurs when a START or STOP condition occurs																		
																													// the format frame. To recover from a bus error, the TWSTO 
																													// Flag must set and TWINT must be cleared by writing a logic 
																													// one to it.
			break;
			
	case TW_NO_INFO:																				// No relevant state information available; TWINT = “0”
		asm volatile("nop"::);
		break;
	
	case TW_ST_SLA_ACK:																			// Own SLA+R has been received ACK has been returned
		//TWDR = turn_data;																			// Last data byte will be transmitted and NOT ACK should be received					
		TWCR |= (1<<TWINT);																														
		break;
		
	case TW_ST_ARB_LOST_SLA_ACK:
		asm volatile("nop"::);
		break;
	
	case TW_ST_DATA_ACK:																		// Data byte in TWDR has been transmitted; ACK has been received
		if(--twi_bytes > 0) {																// If there is more data to send,
			TWDR = twi_data;																		// Send it,
			twi_data++;																					// increment pointer
			TWCR |= (1<<TWINT);																// and clear TWINT to continue
		}
		else {
			TWI_SendStopCondition();														// Generate the stop condition if desired
			status &= ~(1<<BUSY);															// Bus no longer busy
		}
		break;
	
	case TW_ST_DATA_NACK:																		// Master didn't acknowledge data
		asm volatile("nop"::);
		//TWDR = turn_data;																			// Transmit data,
		TWCR |= (1<<TWINT);																	// and clear TWINT to continue		
		//TWI_SendStopCondition();															// Generate the stop condition if desired
		status &= ~(1<<BUSY);																// Bus no longer busy	
		break;
	
	case TW_ST_LAST_DATA:
		asm volatile("nop"::);
		TWCR |= (1<<TWEA)|(1<<TWINT);
		break;
	
		
	// ----------------- Slave Receiver -----------------------
	
	case TW_START:																					// Slave received start condition
		asm volatile("nop"::);
		twi_data=0;
		//TWCR |= (1<<TWINT);
		break;
		
	case TW_SR_STOP:																				// Slave received stop condition
		asm volatile("nop"::);
		TWCR |= (1<<TWSTO)|(1<<TWINT); 
		SetServo_Pos(twi_data_buffer);										 		// Set servo pos
		twi_data=0;
		break;
	
	case TW_SR_SLA_ACK:																			// Slave acknowledged address data
	  //twi_data = data_buffer;
		TWCR |= (1<<TWEA)|(1<<TWINT);												// Send Acknowledge 
		break;
	
	case TW_SR_DATA_ACK:																		// Slave received data byte
		TWCR |= (1<<TWEA)|(1<<TWINT);												// Get next databyte and acknowledge
		twi_data_buffer[twi_data] = TWDR;											// Read received data byte
		twi_data++;																						// Increment pointer		
		TWCR |= (1<<TWINT);
		break;
		
	case TW_SR_GCALL_DATA_ACK:															// Slave acknowledged data for general call (Not suported!)
		asm volatile("nop"::);
		TWCR |= (1<<TWINT);
		break;

	}
	
	__disable_interrupt;		
	// TWCR |= (1<<TWINT)|(1<<TWEA);													// Clear TWINT flag
}
