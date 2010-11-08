/*****************************************************************************
*
* File              USI_I2C_RAM.c 		Compiles with gcc
* Date              Monday, 12/15/08
* Updated by        jkl
*
*                     The example is written for the ATtiny2313
*
* AppNote           AVR310 - Using the USI module as a TWI Master
*
*		Extensively modified to simply read/write an 8 bit I/O extender.
* 12/18/08	Modified to support reading/writing Ram		-jkl
*
****************************************************************************/
#include <avr/interrupt.h>
#define F_CPU 1000000UL	      // Sets up the default speed for delay.h
#include <util/delay.h>
#include <avr/io.h>
#include "USI_TWI_Master.h"

//#define DEBUG				// Define this for Debug LED use

#define PORT_ADDR  0x38		// For 8574A I/O port expander
#define MEMORY_ADDR  0x50	// For NXP Ram

#define MESSAGEBUF_SIZE       8

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
//		Write 4 bytes to the Ram starting at address 00
//		Wait for button.
//		On button press:
//			Read 3 Bytes from Ram - should be bytes 1, 2, and 3 written earlier.
//			If DEBUG, blink the first two bytes on signal leds (1&2).
//			Write first byte read to Port Expander.
//			Delay one second.
//			Write second byte read to Port Expander.
//			Delay one second.
//			Clear Port Expander leds.
//		Blink code on Error LED if there's an I2C error.
//		If DEBUG is defined, blink Debug LED to indicate program progress.
//---------------------------------------------------------------------------

void main( void )
{
  unsigned char messageBuf[MESSAGEBUF_SIZE];
  unsigned char temp, myData=0;
	uint8_t but1, but1a, oldBut;
	
	/* Initialize Button - Turn on pull-up*/
	PORT_BUTTON = _BV(P_BUTTON) ;
	oldBut = 0;	// button not pressed
	
	/* Initialize LED blinker  */
	DDR_LED = _BV(P_LED_1) | _BV(P_LED_2) | _BV(P_LED_3);		      // enable output
	
#ifdef DEBUG	
    blinkEm(2,P_LED_1);		// Blink as init signal
#endif

  USI_TWI_Master_Initialise();
  
// Write to the I2C Ram. 
	messageBuf[0] = (MEMORY_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = 0x00;       // Starting address in memory
	messageBuf[2] = 0x01;
	messageBuf[3] = 0x03;
	messageBuf[4] = 0x02;
	messageBuf[5] = 0x06;
				
	temp = USI_TWI_Start_Read_Write( messageBuf, 6 );
	if (!temp)    // Check Error.
	{             // Use TWI status information to detemine cause of failure and take appropriate actions. 
		TWI_Act_On_Failure_In_Last_Transmission( USI_TWI_Get_State_Info( ) );  
	}
//#ifdef DEBUG
	blinkEm(1, P_LED_1);
//#endif	
  
  while(1){

	/* --------------------------------------------------------------
	** Read button. If it is pressed, then do actions.
	**    Don't care if it's released or not - update only done once
	**    until it is released and pressed again. An action is possible on release,
	**    but not implemented.
	**--------------------------------------------------------------*/
   
	but1 = ~PIN_BUTTON & _BV(P_BUTTON);	// look at buttons, invert
	if (but1 != oldBut)		
	{
		_delay_ms(100);	
		but1a = ~PIN_BUTTON & _BV(P_BUTTON);		// read again
		if (but1a == but1) 			// button has changed
		{
			oldBut = but1;
			if (oldBut > 0)		// Switch is pressed
			{
#ifdef DEBUG			
				blinkEm(2,P_LED_1);
#endif
//	Read the data from Memory - read/write bit may be set or cleared - function handles it.
//  Also note that the length specified should be Slave Adrs byte + memory adrs byte + # bytes to read
//  Can also think of it as Slave Adrs Byte + # bytes to read + 1.
				messageBuf[0] = (MEMORY_ADDR<<TWI_ADR_BITS);
				messageBuf[1] = 0x00;       // Starting address in memory
				temp = USI_TWI_Start_Random_Read( messageBuf, 5 );	// Read 3 bytes of data
				if (!temp)    // Check error.
				{             // Use TWI status information to detemine cause of failure and take appropriate actions. 
					TWI_Act_On_Failure_In_Last_Transmission( USI_TWI_Get_State_Info( ) );  
				}
#ifdef DEBUG			
				blinkEm(3,P_LED_1);
				
				blinkEm(messageBuf[1], P_LED_1);
				blinkEm(messageBuf[2], P_LED_2);
#endif					
				myData = ~messageBuf[1];		// invert for Port LEDs
				messageBuf[1] = myData;
				myData = ~messageBuf[2];
#ifdef DEBUG			
				blinkEm(4,P_LED_1);
#endif	
// Write first read byte to the I2C Port.
				messageBuf[0] = (PORT_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
				temp = USI_TWI_Start_Read_Write( messageBuf, 2 );
				if (!temp)    // Check error.
				{             // Use TWI status information to detemine cause of failure and take appropriate actions. 
						TWI_Act_On_Failure_In_Last_Transmission( USI_TWI_Get_State_Info( ) );  
				}
#ifdef DEBUG
				blinkEm(5, P_LED_1);
#endif
//	Delay two seconds
				for (temp=0;temp<40;temp++)
				{
					_delay_ms(50);
				}
				
				// Write second read byte to the I2C Port.
				messageBuf[1] = myData;
				temp = USI_TWI_Start_Read_Write( messageBuf, 2 );
				if (!temp)    // Check Error.
				{             // Use TWI status information to detemine cause of failure and take appropriate actions. 
						TWI_Act_On_Failure_In_Last_Transmission( USI_TWI_Get_State_Info( ) );  
				}
#ifdef DEBUG
				blinkEm(6, P_LED_1);
#endif
				for (temp=0;temp<40;temp++)
				{
					_delay_ms(50);
				}
				//Turn off the leds on the Port.
				messageBuf[0] = (PORT_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
				messageBuf[1] = 0xff;		
				temp = USI_TWI_Start_Read_Write( messageBuf, 2 );
				if (!temp)    // Check error.
				{             // Use TWI status information to detemine cause of failure and take appropriate actions. 
						TWI_Act_On_Failure_In_Last_Transmission( USI_TWI_Get_State_Info( ) );  
				}
#ifdef DEBUG
				blinkEm(7, P_LED_1);
#endif
				
			}
		}	
	}
  }

  return;
}

/*------------------------------------------------------------------------
**  blinkEm - function to blink LED for count passed in
**		Assumes that leds are all on the same port.
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
