/*****************************************************************************
*
* File			TWI_I2C_Port.c		Compiles with gcc.
* Created		Wednesday, 12/3/08	
* Author		jkl
*
*                     The example is written for the ATMega168
*
* AppNote          AVR315 - Using the TWI module as a TWI Master
*
*		Extensively modified to simply read/write an 8 bit I/O extender.
*	NOte: Clock needs to be 4MHz for 100KHz operation.
*
****************************************************************************/
#include <avr/interrupt.h>
#define F_CPU 4000000UL	      // Sets up the default speed for delay.h
#include <util/delay.h>
#include <avr/io.h>
#include "TWI_Master.h"

//#define DEBUG				// Define this for Debug LED use

#define PORT_ADDR  0x38	// For 8574A I/O port expander

#define MESSAGEBUF_SIZE       4

// LED and Button Ports


#define PORT_LED	PORTD
#define PORT_BUTTON	PORTD
#define DDR_LED   	DDRD
#define DDR_BUTTON  DDRD
#define PIN_BUTTON  PIND
#define P_LED_1		PD7			// used for init & data signal (for Debug)
#define P_LED_2		PD6			// used for debug
#define P_LED_3		PD5			// used for errors
#define P_BUTTON	PD2

void blinkEm( uint8_t count, uint8_t led);

void TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg )
{
                    // A failure has occurred, use TWIerrorMsg to determine the nature of the failure
                    // and take appropriate actions.
                    // See header file for a list of possible failure messages.

// Error codes are just the state codes defined in TWI_Master.h. These tend to be
//	rather large to decode from a blinking LED. Since there are really only a few
//	that we care about, use a SWITCH/CASE to put out easy codes.
// Note that Receive Data NAck from Slave is expected so is not an error.
	switch (TWIerrorMsg & 0xf8)
	{
		case TWI_MTX_ADR_NACK:		// No Transmit Address Ack from Slave
			blinkEm(1,P_LED_3);
		break;
		case TWI_MTX_DATA_NACK:		// No Transmit Data Ack from Slave
			blinkEm(2,P_LED_3);
		break;		
		case TWI_MRX_ADR_NACK:		// No Receive Address Ack from Slave
			blinkEm(3,P_LED_3);
		break;		
		case TWI_ARB_LOST:			// Arbitration Lost -- How?
			blinkEm(4,P_LED_3);
		break;
		case TWI_NO_STATE:			// No State -- What happened?
			blinkEm(5,P_LED_3);
		break;
		case TWI_BUS_ERROR:			// Bus Error
			blinkEm(6,P_LED_3);
		break;
		default:					// Anything Else - further decoding is possible
			blinkEm(7,P_LED_3); 
	}
}

//---------------------------------------------------------------------------
// Behavior: 
//		Blink LED at start.
//		Wait for button.
//		On button press, read Port Expander, store code 
//			back to Port Expander (4 ins, 4 outs).
//		Blink code on Error LED if there's an error.
//		If DEBUG is defined, blink Debug LED to indicate program progress and
//			use Signal LED to echo switch reading.
//---------------------------------------------------------------------------

void main( void )
{
  unsigned char messageBuf[MESSAGEBUF_SIZE];
  unsigned char TWI_targetSlaveAddress, temp, myData=0;
	uint8_t but1, but1a, oldBut;
	
    // Make clock go 4MHz 
	CLKPR = 0x80;		// Initiate write cycle for clock setting
	CLKPR = 0x01;		// 4 MHz clock
	
	/* Initialize Button - Turn on pull-up*/
	PORT_BUTTON = _BV(P_BUTTON) ;
	oldBut = 0;	// button not pressed
	
	/* Initialize LED blinkers  */
	DDR_LED = _BV(P_LED_1) | _BV(P_LED_2) | _BV(P_LED_3);		      // enable output

    
    blinkEm(2,P_LED_1);		// Blink as init signal

  TWI_Master_Initialise();
  
  sei();		// Enable Interrupts so TWI driver runs
  
  TWI_targetSlaveAddress   = PORT_ADDR;	

	
  while(1){

	/* --------------------------------------------------------------
	** Read button. If it is pressed, then do actions.
	**    Don't care if it's released or not - update only done once
	**    until it is released again. An action is possible on release,
	**    but not implemented.
	**--------------------------------------------------------------*/
   
	but1 = ~PIN_BUTTON & _BV(P_BUTTON);	// look at buttons, invert
	if (but1 != oldBut)		
	{
		_delay_ms(50);		// 100ms @ 4MHz.
		_delay_ms(50);		
		but1a = ~PIN_BUTTON & _BV(P_BUTTON);		// read again
		if (but1a == but1) 			// button has changed
		{
			oldBut = but1;
			if (oldBut > 0)		// Switch is pressed
			{
#ifdef DEBUG			
				blinkEm(1,P_LED_2);
#endif
			// Read the I2C Port (1st 4 pins)
				messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT); 
				TWI_Start_Read_Write( messageBuf, 2 );
				temp = TWI_Read_Data_From_Buffer( messageBuf, 2 ); // TWI_statusReg.lastTransOK is returned
				
				if (!TWI_statusReg.lastTransOK )	// check for error
				{             // Use TWI status information to determine cause of failure and take appropriate actions. 
						TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );  
				}
				else
				{
					myData = messageBuf[1] & 0x0f;        // Data is least significant 4 bits
#ifdef DEBUG
					blinkEm(myData, P_LED_1);	// blink LED w/data
#endif
				}
#ifdef DEBUG
				blinkEm(2, P_LED_2);
#endif
				// Write to the I2C Port. Set LEDs according to switches
				messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
				messageBuf[1] = myData << 4 | 0x0f;       // The second byte is used for the data.
				//	Must be sure to set 4 LSB's HI so they work as inputs.
				TWI_Start_Read_Write( messageBuf, 2 );
				// Should wait for completion.
				while ( TWI_Transceiver_Busy() );
				
				if (!TWI_statusReg.lastTransOK )    // Check for errors
				{             // Use TWI status information to determine cause of failure and take appropriate actions. 
						TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );  
				}
#ifdef DEBUG
				blinkEm(3, P_LED_2);
#endif
			}
		}	
	}
  }

  return;
}

/*------------------------------------------------------------------------
**  blinkEm - function to blink LED for count passed in
**		Assumes that leds are all on the same port. Maximum delay 
**		is 262ms - using 200x5 delays 1 second for a clock rate of 1MHz.
**		Increase for proportionally for faster clocks. For 4MHZ Clock,
**		the values are 200/4 and 5*4, or 50 and 20.
** ---------------------------------------------------------------------*/
void blinkEm( uint8_t count, uint8_t led){
	uint8_t i;
	while (count > 0){
		PORT_LED |= _BV(led);
		for (i=0;i<20;i++)
		{
			_delay_ms(50);
		}
		PORT_LED &= ~_BV(led);
		for (i=0;i<20;i++)
		{
			_delay_ms(50);
		}
		count--;

	}
}

