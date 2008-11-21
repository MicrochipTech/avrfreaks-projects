#include "WiSHABI_RX.h"

//Includes code for mouse operation loop and its required functions
#include "mouse_mode.h"
//Includes code for keyboard operation loop and its required functions
#include "keyboard_mode.h"

//Globally accessible variable representing the last valid data byte received by the USART
unsigned char Global_RX_Bytes[4];
unsigned char Global_Mode;
unsigned char Global_Timeout = 0;
/*******************************************************************************************************************************************/

//Usart Byte Reception Interrupt Sub Routine
ISR(USART_RXC_vect)
{
	unsigned char ReceivedByte = UDR; //Reading from the UDR clears the RXC interrupt flag

	if (ReceivedByte == SOH) //Start of header confirmed
	{
		usbPoll();
		if (USART_Receive() == STX) //Start of text confirmed
		{
			usbPoll();
			Global_RX_Bytes[0] = USART_Receive();//Store the button byte
			Global_RX_Bytes[1] = USART_Receive();//Store the ADXL330 X-axis  byte
			Global_RX_Bytes[2] = USART_Receive();//Store the ADXL330 Y-axis  byte
			Global_RX_Bytes[3] = USART_Receive();//Store the ADXL330 Z-axis  byte

			Global_Timeout = 0;//Reset the time out counter
		}
	}
}
/*******************************************************************************************************************************************/

void hardware_Init (void)
{
	//Data Direction Registers
	DDRB = 0xFF; 	//All Outputs
	DDRC = 0xFF;	//All Outputs (ATMega8 has 7 pins on portc)
	DDRD = 0xFF;	//USB Port, USB pins to be set later

	//Initial Port I/O Pin Values
	PORTB = 0x00;
	PORTC = 0b01110000;	//Keep 'RESET high, set the Tri-Colour LED to red and all other LEDs off until system initialised.
	PORTD = 0x00;		//USB Port 

	//Timer Initialisation
	//Timer/Counter 1 Control Register B
	TCCR1B |= ( (1<<CS12)|(1<<CS10) );//Timer frequency set to FCPU/1024

	//USART Initialisation
	USART_Init(BAUDRATE);	// Set the baudrate to 2400 bps using a 12MHz crystal = UBRR value
	SendInitConfirmation();	//To Be Removed Eventually.  Just sends some text to the terminal confirminf USART operation.

	//USB Initialisation
	usbReset();
	DDRD = ~USBMASK; //USB Port Pins
	usbInit();

	//Enable Interrupts
	sei();
}
/*******************************************************************************************************************************************/

//12lsb of an integer used to control 12 LEDs
//9 LEDs with common cathode
//1 RBG LED with common annode (bits need to be flipped)
void Set_LEDs (unsigned int leds, unsigned char mode)
{ 
	unsigned char to_PORTB, to_PORTC;
	//2MSBs are "don't cares"
	to_PORTB = (leds & 0b00111111);
	//PC6 must remain high (This is the reset pin).
	to_PORTC = ( (leds>>6) | (0b01000000) );

	//Check Mode for mode leds
	//Active mode LED bit must be 0 (common annode)
	if (mode == 1)
	{
		to_PORTC |= 0b00011000;
	}
	else if (mode == 2)
	{
		to_PORTC |= 0b00101000;
	
	}
	else if (mode == 3)
	{
		to_PORTC |= 0b00110000;
	
	}
	PORTB = to_PORTB;
	PORTC = to_PORTC;
}
/*******************************************************************************************************************************************/

//Determines the tilt orientation of the unit and appoints a corresponding sector value
unsigned char Poll_Sector (void)
{
	unsigned char sector;

	//Check ADXL330 X-axis for sector X Position

	//Could be sector 1,4,7
	if ((Global_RX_Bytes[1] >= 0xAD) & (Global_RX_Bytes[1] <= 0xBA))
	{
		//Check ADXL330 Z-axis for sector Y Position
		//Could be sector 1,2,3
		if((Global_RX_Bytes[2] >= 0xB4) & (Global_RX_Bytes[2] <= 0xB9))
		{
			sector = SECT_1;
		}
		//Could be sector 7,8,9
		else if((Global_RX_Bytes[2] >= 0xAC) & (Global_RX_Bytes[2] <= 0xB0))
		{
			sector = SECT_7;
		}
		//Could be sector 4,5,6
		else
		{
			sector = SECT_4;
		}
	}
	//Could be sector 3,6,9
	else if((Global_RX_Bytes[1] >= 0xBF) & (Global_RX_Bytes[1] <= 0xCA))
	{
		//Check ADXL330 Z-axis for sector Y Position
		//Could be sector 1,2,3
		if((Global_RX_Bytes[2] >= 0xB8) & (Global_RX_Bytes[2] <= 0xC0))
		{
			sector = SECT_3;
		}
		//Could be sector 7,8,9
		else if((Global_RX_Bytes[2] >= 0xAF) & (Global_RX_Bytes[2] <= 0xB4))
		{
			sector = SECT_9;
		}
		//Could be sector 4,5,6
		else
		{
			sector = SECT_6;
		}
	}
	//Could be sector 2,5,8
	else
	{
		//Check ADXL330 Z-axis for sector Y Position
		//Could be sector 1,2,3
		if((Global_RX_Bytes[2] >= 0xB4) & (Global_RX_Bytes[2] <= 0xBD))
		{
			sector = SECT_2;
		}
		//Could be sector 7,8,9
		else if((Global_RX_Bytes[2] >= 0xAC) & (Global_RX_Bytes[2] <= 0xAF))
		{
			sector = SECT_8;
		}
		//Could be sector 4,5,6
		else
		{
			sector = SECT_5;
		}
	}
	
	return (sector);
}
/*******************************************************************************************************************************************/

//Updates current tilt orientation, and sets grid & mode leds accordingly.
void Poll_And_Update_LEDs(unsigned char mode)
{
	unsigned char pos = Poll_Sector();
	unsigned int leds = 0;

	leds = ((1<<pos) | (1<<LED_5));//Center LED (5) always on so orientation is obvious even in the dark.

	Set_LEDs(leds, mode);
}
/*******************************************************************************************************************************************/

int main(void)
{
	hardware_Init();//Initialise hardware.

	////////////////////////////////////////////////////////////
	//This section added to prevent USB initialisation
	//interruption by the USART Receive Interrupt.
	UCSRB &= ~( 1 << RXCIE );//Disable USART Receive Interrupt
	TIMER=0;//Wait for three seconds (continuously polling)
	while(TIMER<FIVE_SECONDS)
	{
		usbPoll();
	}
	UCSRB |= ( 1 << RXCIE );//Enable USART Receive Interrupt
	////////////////////////////////////////////////////////////

	//Initiate Global_Mode so that on startup, unit enters mouse mode
	Global_Mode = MODE_MOUSE;	

	//Send first signals (keyboard[no keys] and mouse[no buttons, motion]) to clear anything from previous executions
	Send_Keyboard_Char(0, KEY_NO_KEY);
	Send_Mouse_Command (0, 0, 0);
	Poll_And_Update_LEDs(MODE_MOUSE);	

	//Main loop
	for(;;)
	{
		if (Global_Mode == MODE_MOUSE)
		{
			Mouse_Mode();
		}
		if (Global_Mode == MODE_KEYBOARD)
		{
			Keyboard_Mode();
		}
	}

	return 0;
}


