/*****************************************************************************
*
* File				TWI_Master.h compiled with gcc
* Date              Dec 2, 2008
* Source			Modified from AVR315 - TWI Master Implementation 
* Supported devices : All devices with a TWI module can be used.
*                     The example is written for the ATmega168. Requires 4MHz clock.
*
* Description       : This is a sample driver for the TWI hardware modules.
*                     It is interrupt driven. All functionality is controlled through 
*                     passing information to and from functions. 
*Note: Error checking is implemented by setting lastTransOK to false in the TWI_statusReg.
*	The transceiver state can be read by calling TWI_Get_State_Info to read the TWI_state 
*	to know in what state the failure occurred. 
* 12/19/08	Added Random (Memory) read function. Requires repeated Start without 
*				Stop read function.	-jkl
*
****************************************************************************/

#include <avr/interrupt.h>
#include <avr/io.h>
#include "TWI_Master.h"


static unsigned char TWI_buf[ TWI_BUFFER_SIZE ];    // Transceiver buffer
static unsigned char TWI_msgSize;                   // Number of bytes to be transmitted.
static unsigned char SavedMsgSize;                   // For multiple memory reads.
static unsigned char TWI_state = TWI_NO_STATE;      // State byte. Default set to TWI_NO_STATE.

void TWI_Start_Transceiver( void );

union TWI_statusReg TWI_statusReg = {0};            // TWI_statusReg is defined in TWI_Master.h

/****************************************************************************
Call this function to set up the TWI master to its initial standby state.
Remember to enable interrupts from the main application after initializing the TWI.
Requires 4MHz clock rate.
****************************************************************************/
void TWI_Master_Initialise(void)
{
  TWBR = TWI_TWBR;                                  // Set bit rate register (Baudrate). Defined in header file.
// TWSR = TWI_TWPS;                                  // Not used. Driver presumes prescaler to be 00.
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
Call this function to fetch the state information of the previous operation. 
The function will hold execution (loop) until the TWI_ISR has completed with 
the previous operation. If there was an error, then the function will return 
the TWI State code. 
****************************************************************************/
unsigned char TWI_Get_State_Info( void )
{
  while ( TWI_Transceiver_Busy() );             // Wait until TWI has completed the transmission.
  return ( TWI_state );                         // Return error state.
}

/****************************************************************************
Call this function to perform a random read. That is, a starting memory address
is written, a second START is sent after receiving ACK, the slave address is re-
transmitted with the read bit set, and multiple bytes are read from memory. This
function works by setting the TWI_statusReg.memRead bit and calling the
TWI_Start_Read_Write function. The ISR handles the repeated start.
****************************************************************************/
void TWI_Start_Random_Read( unsigned char *msg, unsigned char msgSize )
{
	msg[0] &= ~(TRUE<<TWI_READ_BIT);	// Be sure read/write bit is clear (write) regardless.
	SavedMsgSize = msgSize;				// Save msgSize - it'll be restored in the ISR
	TWI_statusReg.memRead = TRUE;		// Flag memory read
	TWI_Start_Read_Write( msg, 2 );		// Set size to 2 for initial call.
}
/****************************************************************************
Call this function to send a prepared message. The first byte must contain the 
slave address and the read/write bit. Consecutive bytes contain the data to be 
sent, or empty locations for data to be read from the slave. Also include how 
many bytes should be sent/read including the address byte.The function 
will hold execution (loop) until the TWI_ISR has completed with the previous 
operation, then initialize the next operation and return.
****************************************************************************/
void TWI_Start_Read_Write( unsigned char *msg, unsigned char msgSize )
{
  unsigned char temp;

  while ( TWI_Transceiver_Busy() );              // Wait until TWI is ready for next transmission.
	TWI_msgSize = msgSize; 						  // Number of data to transmit.                       
  TWI_buf[0]  = msg[0];                          // Store slave address with R/W setting.
  if (!( msg[0] & (TRUE<<TWI_READ_BIT) ))       // If it is a write operation, then also copy data.
  {
    for ( temp = 1; temp < msgSize; temp++ )
      TWI_buf[ temp ] = msg[ temp ];
  }
  //TWI_statusReg.all = 0;
  TWI_statusReg.lastTransOK = FALSE;			//  clear OK bit 
  TWI_state         = TWI_NO_STATE ;
  
  TWCR = (1<<TWEN)|                             // TWI Interface enabled.
         (1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.
         (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
         (0<<TWWC);                             
}

/****************************************************************************
Call this function to resend the last message. The driver will reuse the data 
previously put in the transceiver buffers. The function will hold execution 
(loop) until the TWI_ISR has completed with the previous operation,
then initialize the next operation and return.
****************************************************************************/
void TWI_Start_Transceiver( void )
{
  while ( TWI_Transceiver_Busy() );             // Wait until TWI is ready for next transmission.
    //TWI_statusReg.all = 0;
  TWI_statusReg.lastTransOK = FALSE;			// can't clear all bits      
  TWI_state         = TWI_NO_STATE ;
  TWCR = (1<<TWEN)|                             // TWI Interface enabled.
         (1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.
         (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
         (0<<TWWC);                             //
}

/****************************************************************************
Call this function to read out the requested data from the TWI transceiver buffer. 
I.e. first call TWI_Start_Transceiver to send a request for data to the slave. 
Then Run this function to collect the data when they have arrived. Include a 
pointer to where to place the data and the number of bytes requested (including 
the address field) in the function call. The function will hold execution (loop)
until the TWI_ISR has completed with the previous operation, before reading out 
the data and returning. If there was an error in the previous transmission the 
function will return the TWI error code.
****************************************************************************/
unsigned char TWI_Read_Data_From_Buffer( unsigned char *msg, unsigned char msgSize )
{
  unsigned char i;

  while ( TWI_Transceiver_Busy() );             // Wait until TWI is ready for next transmission.

  if( TWI_statusReg.lastTransOK )               // Last transmission competed successfully.              
  {                                             
    for ( i=0; i<msgSize; i++ )                 // Copy data from Transceiver buffer.
    {
      msg[ i ] = TWI_buf[ i ];
    }
	//return (TRUE);
  }
  return( TWI_statusReg.lastTransOK );         //!!!!!!!!!!!Fix This !!!!!!!!!!!!!!!!!!   
  // return (FALSE);
}

// ********** Interrupt Handlers ********** //
/****************************************************************************
This function is the Interrupt Service Routine (ISR), and called when the TWI 
interrupt is triggered; that is whenever a TWI event has occurred. This function 
should not be called directly from the main application.
Notes: (12/2/08  -jkl)
- Setup of Interrupt vector is changed to the gcc method.
- The TWSR is masked in case a value is in the prescaler (TWI_TWPS).
- TWSR is also masked before it is stored to TWI_state.
Notes (12/19/08	-jkl) 
- Provides a dual start without stop read mode for Memory.
	To use it, set the Dual Start Bit in the TWI_statusReg. More????
	Need to set size to 2, then without sending a STOP:
- set READ bit in Slave Address
- set size to correct value
- clear memRead bit in status reg
- send a new START sequence - everything else should work ;)}
 
****************************************************************************/

ISR(TWI_vect)
{
  static unsigned char TWI_bufPtr;

  switch (TWSR & 0xf8)			// Mask off prescaler bits
  {
    case TWI_START:             // START has been transmitted  
    case TWI_REP_START:         // Repeated START has been transmitted
      TWI_bufPtr = 0;                                     // Set buffer pointer to the TWI Address location
    case TWI_MTX_ADR_ACK:       // SLA+W has been tramsmitted and ACK received
    case TWI_MTX_DATA_ACK:      // Data byte has been tramsmitted and ACK received
      if (TWI_bufPtr < TWI_msgSize)
      {
        TWDR = TWI_buf[TWI_bufPtr++];
        TWCR = (1<<TWEN)|                                 // TWI Interface enabled
               (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to send byte
               (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           //
               (0<<TWWC);                                 //  
      }else                    // Send STOP after last byte
      {
        if (TWI_statusReg.memRead)					// memory read with dual start
		{
			TWI_buf[0] |= (TRUE<<TWI_READ_BIT);	// set READ bit in Slave Address
			TWI_msgSize =	SavedMsgSize;			// set size to correct value
			TWI_statusReg.all = 0;					// clear memRead bit in status reg
					// send a new START sequence - everything else should work ;)}
			TWI_state         = TWI_NO_STATE ;
			TWCR = (1<<TWEN)|                             // TWI Interface enabled.
				(1<<TWIE)|(1<<TWINT)|                  // Enable TWI Interupt and clear the flag.
				(0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|       // Initiate a START condition.
				(0<<TWWC);     
		}
		else
		{
			TWI_statusReg.lastTransOK = TRUE;                 // Set status bits to completed successfully. 
			TWCR = (1<<TWEN)|                                 // TWI Interface enabled
               (0<<TWIE)|(1<<TWINT)|                      // Disable TWI Interrupt and clear the flag
               (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.
               (0<<TWWC);                       
		}
      }
      break;
    case TWI_MRX_DATA_ACK:      // Data byte has been received and ACK tramsmitted
      TWI_buf[TWI_bufPtr++] = TWDR;
    case TWI_MRX_ADR_ACK:       // SLA+R has been tramsmitted and ACK received
      if (TWI_bufPtr < (TWI_msgSize-1) )                  // Detect the last byte to NACK it.
      {
        TWCR = (1<<TWEN)|                                 // TWI Interface enabled
               (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to read next byte
               (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Send ACK after reception
               (0<<TWWC);                                 //  
      }else                    // Send NACK after next reception
      {
        TWCR = (1<<TWEN)|                                 // TWI Interface enabled
               (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag to read next byte
               (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // Send NACK after reception
               (0<<TWWC);                                 // 
      }    
      break; 
    case TWI_MRX_DATA_NACK:     // Data byte has been received and NACK tramsmitted
      TWI_buf[TWI_bufPtr] = TWDR;
      TWI_statusReg.lastTransOK = TRUE;                 // Set status bits to completed successfully. 
      TWCR = (1<<TWEN)|                                 // TWI Interface enabled
             (0<<TWIE)|(1<<TWINT)|                      // Disable TWI Interrupt and clear the flag
             (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|           // Initiate a STOP condition.
             (0<<TWWC);                                 //
      break;      
    case TWI_ARB_LOST:          // Arbitration lost
      TWCR = (1<<TWEN)|                                 // TWI Interface enabled
             (1<<TWIE)|(1<<TWINT)|                      // Enable TWI Interupt and clear the flag
             (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)|           // Initiate a (RE)START condition.
             (0<<TWWC);                                 //
      break;
    case TWI_MTX_ADR_NACK:      // SLA+W has been tramsmitted and NACK received
    case TWI_MRX_ADR_NACK:      // SLA+R has been tramsmitted and NACK received    
    case TWI_MTX_DATA_NACK:     // Data byte has been tramsmitted and NACK received
//    case TWI_NO_STATE              // No relevant state information available; TWINT = “0”
    case TWI_BUS_ERROR:         // Bus error due to an illegal START or STOP condition
    default:     
      TWI_state = TWSR & 0xf8;                          // Store TWSR and automatically sets clears noErrors bit.
                                                        // Reset TWI Interface
      TWCR = (1<<TWEN)|                                 // Enable TWI-interface and release TWI pins
             (0<<TWIE)|(0<<TWINT)|                      // Disable Interupt
             (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|           // No Signal requests
             (0<<TWWC);                                 //
  }
}
