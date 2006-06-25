#include <avr/io.h>
// Servo controller by Mattias Ottosson
// mattias.ottosson@home.se

// Connections:
// Mega48 running with internal rc-osc @ 8.0000MHz
// VCC and AVCC connected to +5V
// GND x 2 connected to 0V
// Rx, Tx and GND connected to MAX232
// PORTB0 connected to servo (Futaba FP-S148) (white signal)
// PORTD2 connected to green LED


// Software description:
// µcomputer connected to PC
// PC send one of the following commands on serial port using 2400,8n1
// - µ-computer turn servo left
// + µ-computer turn servo right
// h µ-computer turn servo to center position

// Circuit board:
// See attached files board.pdf or board.brd
// C5 = 10µF
// C1, C2, C3, C4, C6 = 1µF
// R1 = 370 Ohm
// DO NOT forget jumper at pin 22 to ground

// Used "WinAVR-20060421-install.exe" for developing
// Used Peter Flurys "AVR Starter kir" (http://homepage.hispeed.ch/peterfleury/avr-starterkit.html) for programming

void USART_init(void);								// Init USART - 2400bps using 8.0000MHz
void USART_Transmit( unsigned char);				// Transmit one byte
void USART_SendString(char *);						// Transmit one string of bytes ending with NULL
unsigned char USART_Receive( void );				// Receive one byte

void delay_20us(void);								// Delay 20µs
void moveservo(char);

void main(void)
{
	char steplength=5;
	char position=52;
	char input=0;
	
	DDRB=1;											// PB0 set to output
	DDRD=4;											// PD2 set to output
	
	TCCR0B=2;										// CLK/8 -> tick length = 1µs
		
	USART_init();
	moveservo(position);							// Move servo to home position
	
	while(1)
	{
		PORTD=4;									// Turn on LED to show waiting for command
		input=USART_Receive();						// Wait for serial command
													
													// Make sure servo is within work area (12<position<92)
		if(input=='-' && position<92)				// Move to the left
			position=position+steplength;
		if(input=='+' && position>17)				// Move to the right
			position=position-steplength;
		if(input=='h')								// Move home			
			position=52;		
				
		PORTD=0;									// Turn off LED while moving servo
		moveservo(position);						// Move servo
	}
}

void moveservo(char pos)							// Move servo
{
	for(int yy=0;yy<7;yy++)						// Send new position 7 times (~7*30ms)
	{
		PORTB=1;									// Pulse is high for position*20µs
		for(int i=0;i<pos;i++)
			delay_20us();
			
		PORTB=0;
		for(int i=0;i<(1500-pos);i++)				// Pulse is low for ~(1500-position)*20µs
			delay_20us();
	}
}

void delay_20us(void)								// Wait 20µs
{
	TCNT0=0;
	while(TCNT0<20);
}


void USART_init(void)								// Init USART to 2400bps using 8.0000MHz internal rc-osc.
{
	UBRR0=207;										// 2400 bps running @ 8.0000 MHz
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);				// Enable receiver and transmitter @ USART0
}

void USART_Transmit( unsigned char data )			// Transmit one byte on USART
{
	while ( !( UCSR0A & (1<<UDRE0)) );			// Wait for empty transmit buffer
	UDR0 = data;									// Put data into USART buffer and send data
}

unsigned char USART_Receive( void )				// Receive string on USART
{
	while ( !(UCSR0A & (1<<RXC0)) );				// Wait for data to be received
	return UDR0;									// Return USART buffer
}

void USART_SendString(char *string)				// Transmit string to USART
{
	unsigned char counter=0;
	while(*(string+counter))						// Data to send is != NULL
	{
		USART_Transmit(*(string+counter));			// Transmit one byte
		counter++;									// Increase pointer
	}
}

