/*****************************************************************************
*
* File              USI_I2C_Port.c 		Compiles with gcc
* Date              Friday, 10/31/08		Boo!
* Updated by        jkl
*
*                     The example is written for the ATtiny2313
*
* AppNote           AVR310 - Using the USI module as a TWI Master
*
*		Extensively modified to simply read/write an 8 bit I/O extender.
*
****************************************************************************/
#include <avr/interrupt.h>
#define F_CPU 1000000UL	      // Sets up the default speed for delay.h
#include <util/delay.h>
#include <avr/io.h>
#include "USI_TWI_Master.h"

//#define DEBUG				// Define this for Debug LED use

#define PORT_ADDR  0x38	// For 8574A I/O port expander

#define MESSAGEBUF_SIZE       4

// LED and Button Ports - specific to ATtiny2313
#define PORT_LED	PORTD
#define PORT_BUTTON	PORTD
#define DDR_LED   	DDRD
#define DDR_BUTTON  DDRD
#define PIN_BUTTON  PIND
#define P_LED_1		PD6			// used for init & data signal (for Debug)
#define P_LED_2		PD5			// used for debug
#define P_LED_3		PD4			// used for errors
#define P_BUTTON	PD2

void blinkEm( uint8_t count, uint8_t led);

void TWI_Act_On_Failure_In_Last_Transmission ( unsigned char TWIerrorMsg )
{
                    // A failure has occurred, use TWIerrorMsg to determine the nature of the failure
                    // and take appropriate actions.
                    // See header file for a list of possible failure messages.

	blinkEm(TWIerrorMsg,P_LED_3);
                     
  //return TWIerrorMsg; 
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
	
	/* Initialize Button - Turn on pull-up*/
	PORT_BUTTON = _BV(P_BUTTON) ;
	oldBut = 0;	// button not pressed
	
	/* Initialize LED blinker  */
	DDR_LED = _BV(P_LED_1) | _BV(P_LED_2) | _BV(P_LED_3);		      // enable output

    
    blinkEm(2,P_LED_1);		// Blink as init signal

  USI_TWI_Master_Initialise();
  
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
		_delay_ms(100);		// 100ms @ 1MHz clock.
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
				temp = USI_TWI_Start_Read_Write( messageBuf, 2 );
				if (!temp)    // Check Error.
				{             // Use TWI status information to detemine cause of failure and take appropriate actions. 
						TWI_Act_On_Failure_In_Last_Transmission( USI_TWI_Get_State_Info( ) );  
				}
				else
				{
					myData = messageBuf[1] & 0x0f;        // Data is ls 4 bits
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
				temp = USI_TWI_Start_Read_Write( messageBuf, 2 );
				if (!temp)    // Check Error
				{             // Use TWI status information to detemine cause of failure and take appropriate actions. 
						TWI_Act_On_Failure_In_Last_Transmission( USI_TWI_Get_State_Info( ) );  
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
**		Assumes that leds are all on the same port. 1MHz Clock rate.
** ---------------------------------------------------------------------*/
void blinkEm( uint8_t count, uint8_t led){
	uint8_t i;
	while (count > 0){
		PORT_LED |= _BV(led);
		for (i=0;i<5;i++)
		{
			_delay_ms(200);
		}
		PORT_LED &= ~_BV(led);
		for (i=0;i<5;i++)
		{
			_delay_ms(200);
		}
		count--;

	}
}
