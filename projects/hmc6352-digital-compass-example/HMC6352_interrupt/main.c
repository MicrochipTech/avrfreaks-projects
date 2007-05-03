/*****************************************************************************
*
* Atmel Corporation
*
* File              : main.h
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 1.13 $
* Date              : Date: May 3, 2007
* Updated by        : Author: Jeff Heiss (jeff dot heiss at comcast dot net)
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with a TWI module can be used.
*                     The example is written for the ATmega series
*
* AppNote           : AVR315 - TWI Master Implementation
*
* Description       : This program reads the heading from the MHC6352 
*                     TWI compass.  The program works using the ATmega's 
*                     TWI interrupts.  The heading result is saved in the
*                     "heading" variable.  The heading is a two byte value so
*                     a heading value of 2564 equals 256.4 degrees.  If you 
*                     have trouble reading the HHMC6352, try disconnecting and
*                     reapplying power to it (this is something I have found)
*
****************************************************************************/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "TWI_Master.h"

unsigned char TWI_Act_On_Failure_In_Last_Transmission(unsigned char TWIerrorMsg);

int main( void )
{
	uint8_t TWI_transBuff[4], TWI_recBuff[3];
	uint8_t TWI_targetSlaveAddress, TWI_operation;
	uint16_t heading;
	
	TWI_Master_Initialise();
	TWI_targetSlaveAddress = 33;		// MHCMC6352's address
	TWI_transBuff[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	TWI_transBuff[1] = 'A';
	sei();								// enable interrupts
	TWI_Start_Transceiver_With_Data(TWI_transBuff,2);
	TWI_operation = REQUEST_DATA; 		// Set the next operation
  	
	for (;;)
	{

		// Check if the TWI Transceiver has completed an operation.
		if ( ! TWI_Transceiver_Busy() )                              
		{
		    // Check if the last operation was successful
			if ( TWI_statusReg.lastTransOK )
			{
				// Determine what action to take now
				if (TWI_operation == SEND_DATA)
				{ 
					// Send data to slave
					TWI_transBuff[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
					TWI_transBuff[1] = 'A';
					TWI_Start_Transceiver_With_Data(TWI_transBuff,2);	// send TWI_transBuff[0] and TWI_transBuff[1] to HMC6352
					TWI_operation = REQUEST_DATA; 						// Set next operation
				}
				else if (TWI_operation == REQUEST_DATA)
				{ 
					// Request data from slave
					TWI_recBuff[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
					TWI_Start_Transceiver_With_Data(TWI_recBuff,3); // 3 = TWI_recBuff[0] byte + heading high byte + heading low byte
					TWI_operation = READ_DATA_FROM_BUFFER; 			// Set next operation        
				}
				else if (TWI_operation == READ_DATA_FROM_BUFFER)
				{ 
					// Get the received data from the transceiver buffer
					TWI_Get_Data_From_Transceiver(TWI_recBuff);        
					TWI_operation = SEND_DATA;    					// Set next operation        
					// compute the heading.  If you have JTAG, you may wish to set a breakpoint on this
					// line to see the heading during development or trouble shooting.
					heading = (uint16_t)TWI_recBuff[1] << 8 | TWI_recBuff[2];
				}
			}
			else // Got an error during the last transmission
			{
				// Use TWI status information to detemine cause of failure and take appropriate actions. 
				TWI_Act_On_Failure_In_Last_Transmission(TWI_Get_State_Info( ));
			}
		} //end of TWI status check
    
	    // put your main program code here
		//_delay_ms(500); // don't read the HMC6352 too fast

	} // end for(;;)
	return 0;
}


/*****************************************************************************
// A failure has occurred, use TWIerrorMsg to determine the nature of the failure
// and take appropriate actions.
// Se header file for a list of possible failures messages.
// Here is a simple sample, where if received a NACK on the slave address,
// then a retransmission will be initiated.
****************************************************************************/
unsigned char TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg )
{
	if ( (TWIerrorMsg == TWI_MTX_ADR_NACK) | (TWIerrorMsg == TWI_MRX_ADR_NACK) )
    {
		TWI_Start_Transceiver();
	}
    
 	return TWIerrorMsg; 
}
