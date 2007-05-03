/*****************************************************************************
*
* Atmel Corporation
*
* File              : TWI_Master.c
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 1.13 $
* Date              : Date: May 3, 2007
* Updated by        : Author: Jeff Heiss 
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with a TWI module can be used.
*                     The example is written for the ATmega16
*
* AppNote           : AVR315 - TWI Master Implementation
*
* Description       : From Ateml: This is a sample driver for the TWI hardware modules.
*                     It is interrupt driveren. All functionality is controlled through 
*                     passing information to and from functions. Se main.c for samples
*                     of how to use the driver.
*                   : From Author: This is my edited TWI_Master.c file.  The main 
*                     difference between this and the origional is this file uses 
*                     a pointer to access the TWI buffer that is located in the main file 
*                     instead of creating another buffer (eats up more RAM) in this file.
*
****************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>         
#include "TWI_Master.h"

volatile unsigned char TWI_msgSize;                   // Number of bytes to be transmitted.
volatile unsigned char TWI_state = TWI_NO_STATE;      // State byte. Default set to TWI_NO_STATE.
volatile unsigned char *TWI_buf_ptr; 
volatile unsigned char *TWI_buf_ptr_head;
volatile unsigned char *TWI_buf_ptr_tail;

union TWI_statusReg TWI_statusReg = {0};            // TWI_statusReg is defined in TWI_Master.h

/****************************************************************************
Call this function to set up the TWI master to its initial standby state.
Remember to enable interrupts from the main application after initializing the TWI.
****************************************************************************/
void TWI_Master_Initialise(void)
{
  TWBR = TWI_TWBR;                                  // Set bit rate register (Baudrate). Defined in header file.
  //TWSR = TWI_TWPS;                                  // Not used. Driver presumes prescaler to be 00.
  TWDR = 0xFF;                                      // Default content = SDA released.
  TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins.
         (0<<TWIE)|(0<<TWINT)|                      // Disable Interupt.
         (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // No Signal requests.
         (0<<TWWC);                                 //
}    
    
/****************************************************************************
Call this function to test if the TWI_ISR is busy transmitting.
****************************************************************************/
unsigned char TWI_Transceiver_Busy( void )
{
  return ( TWCR & (1<<TWIE) );                  // IF TWI Interrupt is enabled then the Transceiver is busy
}

/****************************************************************************
Call this function to fetch the state information of the previous operation. The function will hold execution (loop)
until the TWI_ISR has completed with the previous operation. If there was an error, then the function 
will return the TWI State code. 
****************************************************************************/
unsigned char TWI_Get_State_Info( void )
{
  while ( TWI_Transceiver_Busy() );             // Wait until TWI has completed the transmission.
  return ( TWI_state );                         // Return error state.
}

/****************************************************************************
Call this function to send a prepared message. The first byte must contain the slave address and the
read/write bit. Consecutive bytes contain the data to be sent, or empty locations for data to be read
from the slave. Also include how many bytes that should be sent/read including the address byte.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/
void TWI_Start_Transceiver_With_Data( unsigned char *msg, unsigned char msgSize )
{
  while ( TWI_Transceiver_Busy() );             // Wait until TWI is ready for next transmission.
  
  TWI_msgSize = msgSize;                        // Number of data to transmit.
  TWI_buf_ptr = msg;
  TWI_buf_ptr_head = msg;
  TWI_buf_ptr_tail = msg+msgSize--;
  TWI_statusReg.all = 0;      
  TWI_state = TWI_NO_STATE ;
  TWCR = (1<<TWEN)|                             // TWI Interface enabled.
         (1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.
         (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
         (0<<TWWC);                             //
}

/****************************************************************************
Call this function to resend the last message. The driver will reuse the data previously put in the transceiver buffers.
The function will hold execution (loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/
void TWI_Start_Transceiver( void )
{
  while ( TWI_Transceiver_Busy() );             // Wait until TWI is ready for next transmission.
  TWI_statusReg.all = 0;      
  TWI_state         = TWI_NO_STATE ;
  TWCR = (1<<TWEN)|                             // TWI Interface enabled.
         (1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.
         (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
         (0<<TWWC);                             //
}

/****************************************************************************
Call this function to read out the requested data from the TWI transceiver buffer. I.e. first call
TWI_Start_Transceiver to send a request for data to the slave. Then Run this function to collect the
data when they have arrived. Include a pointer to where to place the data and the number of bytes
requested (including the address field) in the function call. The function will hold execution (loop)
until the TWI_ISR has completed with the previous operation, before reading out the data and returning.
If there was an error in the previous transmission the function will return the TWI error code.
****************************************************************************/
unsigned char TWI_Get_Data_From_Transceiver(unsigned char *msg)
{
  while ( TWI_Transceiver_Busy() );             // Wait until TWI is ready for next transmission.

  if( TWI_statusReg.lastTransOK )               // Last transmission competed successfully.              
  {                                             
    msg = TWI_buf_ptr_head; 					// point the buffer in main.c to the new data
  }
  return( TWI_statusReg.lastTransOK );                                   
}

// ********** Interrupt Handlers ********** //
/****************************************************************************
This function is the Interrupt Service Routine (ISR), and called when the TWI interrupt is triggered;
that is whenever a TWI event has occurred. This function should not be called directly from the main
application.
****************************************************************************/
ISR(TWI_vect)
{ 
  switch (TWSR)
  {
    case TWI_START:             // START has been transmitted  
    case TWI_REP_START:         // Repeated START has been transmitted
		TWI_buf_ptr = TWI_buf_ptr_head;
    case TWI_MTX_ADR_ACK:       // SLA+W has been tramsmitted and ACK received
    case TWI_MTX_DATA_ACK:  	// Data byte has been tramsmitted and ACK received
      	if (TWI_buf_ptr < (TWI_buf_ptr_tail) )
      	{
		TWDR = *TWI_buf_ptr++; // mine
        	TWCR = (1<<TWEN)|                                 // TWI Interface enabled
               		(1<<TWIE)|(1<<TWINT)|                     // Enable TWI Interupt and clear the flag to send byte
               		(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           
               		(0<<TWWC);                                   
      	}
		else                    // Send STOP after last byte
      	{
        	TWI_statusReg.lastTransOK = TRUE;                 // Set status bits to completed successfully. 
	        TWCR = (1<<TWEN)|                                 // TWI Interface enabled
        	       (0<<TWIE)|(1<<TWINT)|                      // Disable TWI Interrupt and clear the flag
	               (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.
	               (0<<TWWC);                                 
      	}
      	break;
    case TWI_MRX_DATA_ACK:      // Data byte has been received and ACK tramsmitted
	  *TWI_buf_ptr++ = TWDR;
    case TWI_MRX_ADR_ACK:       // SLA+R has been tramsmitted and ACK received
		if (TWI_buf_ptr < (TWI_buf_ptr_tail-1) )                  // Detect the last byte to NACK it.
		{
        	TWCR = (1<<TWEN)|                                 // TWI Interface enabled
               		(1<<TWIE)|(1<<TWINT)|                     // Enable TWI Interupt and clear the flag to read next byte
               		(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|          // Send ACK after reception
               		(0<<TWWC);                                  
		}
		else                    // Send NACK after next reception
		{
			TWCR = (1<<TWEN)|                                 // TWI Interface enabled
				(1<<TWIE)|(1<<TWINT)|                     // Enable TWI Interupt and clear the flag to read next byte
				(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|          // Send NACK after reception
				(0<<TWWC);                                 
		}    
		break; 
    case TWI_MRX_DATA_NACK:     // Data byte has been received and NACK tramsmitted
		*TWI_buf_ptr++ = TWDR;
		TWI_statusReg.lastTransOK = TRUE;                 	// Set status bits to completed successfully. 
		TWCR = (1<<TWEN)|                                 	// TWI Interface enabled
			(0<<TWIE)|(1<<TWINT)|                     	// Disable TWI Interrupt and clear the flag
			(0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           	// Initiate a STOP condition.
			(0<<TWWC);                                
	      break;      
    case TWI_ARB_LOST:          			// Arbitration lost
		TWCR = (1<<TWEN)|                                 // TWI Interface enabled
			(1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag
			(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|           // Initiate a (RE)START condition.
			(0<<TWWC);                                 
		break;
    case TWI_MTX_ADR_NACK:      // SLA+W has been tramsmitted and NACK received
    case TWI_MRX_ADR_NACK:      // SLA+R has been tramsmitted and NACK received    
    case TWI_MTX_DATA_NACK:     // Data byte has been tramsmitted and NACK received
    case TWI_BUS_ERROR:         // Bus error due to an illegal START or STOP condition
    default:     
		TWI_state = TWSR;                                 	// Store TWSR and automatically sets clears noErrors bit.
	                                                        	// Reset TWI Interface
		TWCR = (1<<TWEN)|                                 	// Enable TWI-interface and release TWI pins
			(0<<TWIE)|(0<<TWINT)|                      	// Disable Interupt
			(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           	// No Signal requests
			(0<<TWWC);                                 	
  } // end switch
}
