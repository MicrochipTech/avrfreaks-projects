/*****************************************************************************
*
* File			TWI_I2C_EEPROM.c		Compiles with gcc.
* Created		Wednesday, 12/3/08	
* Author		jkl
*
*                     The example is written for the ATMega168
*
* AppNote          AVR315 - Using the TWI module as a TWI Master
*	NOte: Clock needs to be 4MHz for 100KHz operation.
*
*		Extensively modified to simply read/write an 8 bit I/O extender.
*	12/19/08	Reads/Writes EEPROM memory. Uses Random read function.	-jkl
*
****************************************************************************/

#include <avr/interrupt.h>
#define F_CPU 4000000UL	      // Sets up the default speed for delay.h
#include <util/delay.h>
#include <avr/io.h>
#include "TWI_Master.h"

//#define DEBUG				// Define this for Debug LED use

#define PORT_ADDR  0x38	// For 8574A I/O port expander
#define MEMORY_ADDR  0x50	// For first 256 bytes of EEPROM
//#define MEMORY_ADDR_1  0x51	// For next 256 bytes of EEPROM, etc.
//  ...
//#define MEMORY_ADDR_7  0x57	// For final 256 bytes of EEPROM. Total: 2K bytes

#define MESSAGEBUF_SIZE       18		// Two more than the memory block length

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
// Note that Receive Data NACK from Slave is expected so is not an error.
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
//		Write 16 bytes to the EEPROM starting at address 00.
//		Wait for button.
//		On button press:
//			Read 16 Bytes from EEPROM starting at address 00.
//			Write first byte read to Port Expander. If DEBUG, flash it on signal led.
//			Delay one second.
//			Repeat for every third byte (5 total), alternate signal leds.
//		Blink code on Error LED if there's an error.
//		If DEBUG is defined, blink Debug LED to indicate program progress.
//---------------------------------------------------------------------------
//

void main( void )
{
	unsigned char messageBuf[MESSAGEBUF_SIZE];
	unsigned char temp, indx, flag, myData=0;
	uint8_t but1, but1a, oldBut;
	
    // Make clock go 4MHz 
	CLKPR = 0x80;		// Initiate write cycle for clock setting
	CLKPR = 0x01;		// 4 MHz clock
	
	/* Initialize Button - Turn on pull-up*/
	PORT_BUTTON = _BV(P_BUTTON) ;
	oldBut = 0;	// button not pressed
	
	/* Initialize LED blinkers  */
	DDR_LED = _BV(P_LED_1) | _BV(P_LED_2) | _BV(P_LED_3);		      // enable output

#ifdef DEBUG    
    blinkEm(2,P_LED_1);		// Blink as init signal
#endif

	TWI_Master_Initialise();
  
	sei();		// Enable Interrupts so TWI driver runs

//	Do initial write to EEPROM. Write cycle takes 5 msecs, so flashing the led will delay enough.
//
 
	messageBuf[0] = (MEMORY_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] =	00;				// Starting address
	for (temp=0; temp<16; temp++)
	{
		if (temp<8)
		{
			messageBuf[temp+2] = temp;	
		}
		else
		{
			messageBuf[temp+2] = temp - 8;
		}
	}
	TWI_Start_Read_Write( messageBuf, 18 );
	// Should wait for completion of EEPROM write cycle. Blinking the LED will do this.
	while ( TWI_Transceiver_Busy() );    // Check for an error.
	if (!TWI_statusReg.lastTransOK )
	{             // Use TWI status information to determine cause of failure and take appropriate actions. 
		TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );  
	}
//#ifdef DEBUG
	blinkEm(1, P_LED_1);
//#endif

  while(1)
  {

	/* --------------------------------------------------------------
	** Read button. If it is pressed, then do actions.
	**    Don't care if it's released or not - update only done once
	**    until it is released and pressed again. An action is possible on release,
	**    but not implemented.
	**--------------------------------------------------------------*/
   
	but1 = ~PIN_BUTTON & _BV(P_BUTTON);	// look at buttons, invert
	if (but1 != oldBut)		
	{
		_delay_ms(50);		// correct delay at 4MHz clock
		_delay_ms(50);
		but1a = ~PIN_BUTTON & _BV(P_BUTTON);		// read again
		if (but1a == but1) 			// button has changed
		{
			oldBut = but1;
			if (oldBut > 0)		// Switch is pressed
			{
#ifdef DEBUG			
				blinkEm(2,P_LED_2);
#endif
//	Read the data from Memory. Value of read/write bit doesn't matter.
				messageBuf[0] = (MEMORY_ADDR<<TWI_ADR_BITS); // Read/write bit doesn't matter here
				messageBuf[1] = 0x00;       // Starting address in memory
				TWI_Start_Random_Read( messageBuf, 17 );	// Desired data length plus one.
				// Could do other actions in here, then get data.
				temp = TWI_Read_Data_From_Buffer( messageBuf, 17 ); 
				if (!TWI_statusReg.lastTransOK )	// Check Error.
				{             // Use TWI status information to determine cause of failure and take appropriate actions. 
						TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );  
				}
#ifdef DEBUG
				blinkEm(3, P_LED_2);
#endif
//------------------------------------------------------------------------------
//     Display the data read in from the EEPROM (every 3rd value)
//			If DEBUG, flash signal led with the value; alternate leds
//			Write data to I/O Port leds. Must invert to display properly.
//			Delay two seconds between values.
//------------------------------------------------------------------------------
				flag = 0;		// Used to alternate leds
				for (indx=1; indx<=5; indx++)
				{
#ifdef DEBUG				
					if (flag == 0)
					{
						blinkEm(messageBuf[indx*3], P_LED_2);
					}
					else
					{
						blinkEm(messageBuf[indx*3], P_LED_1);
					}
					flag ^= 1;		// flip the flag
#endif
					myData = ~messageBuf[indx*3];		// invert for Port LEDs
					messageBuf[0] = (PORT_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
					messageBuf[1] = myData;			// overwrites first data byte in buffer
					TWI_Start_Read_Write( messageBuf, 2 );
					// Should wait for completion on writes.
					while ( TWI_Transceiver_Busy() );	// Wait til operations complete
					if (!TWI_statusReg.lastTransOK )    // Check error.
					{             // Use TWI status information to determine cause of failure and take appropriate actions. 
						TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );  
					}	
					//	Delay two seconds
					for (temp=0;temp<40;temp++)
					{
						_delay_ms(50);
					}
				}
#ifdef DEBUG	
				blinkEm(4,P_LED_2);
#endif	
				//Turn off the leds on the Port.
				messageBuf[0] = (PORT_ADDR<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
				messageBuf[1] = 0xff;		
				TWI_Start_Read_Write( messageBuf, 2 );
				// Should wait for completion on writes.
				while ( TWI_Transceiver_Busy() );	// Wait til operations complete
				if (!TWI_statusReg.lastTransOK )    // Check error
				{             // Use TWI status information to determine cause of failure and take appropriate actions. 
						TWI_Act_On_Failure_In_Last_Transmission( TWI_Get_State_Info( ) );  
				}
#ifdef DEBUG
				blinkEm(5, P_LED_2);
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
void blinkEm( uint8_t count, uint8_t led)
{
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
