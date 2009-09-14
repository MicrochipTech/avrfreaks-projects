/*
	Midi relay controller for EGNL SCreamer and Boss GT-10 by Jonne Karjalainen
	14-09-2009
	
	Great Thanks to mlanagan for the base c-source!
	mlanagan
	http://www.users.on.net/~mlanagan/index.html
*/

#include <avr/io.h>
#include <avr/iom32.h>
#include <global.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "midicontroller.h"
#include <util/delay.h>

//#include "lcd.h"

#define lead	PC0				// ns. lead channel
#define boost	PC1				// ns. boost
#define rele2	PC2
#define rele3	PC3
#define led		PC7



static volatile unsigned char key_press;	   						// Global variable to track key presses
static volatile unsigned char DIP_SW;								// Global variable to track the binary switch
static volatile unsigned char NewCCValue;							// Global variable to track the ADC


static volatile unsigned char PodCurrentProgram = 0;								// Global variable to track the current program number
static volatile unsigned char MIDIInfoReceived = 0;									// Global flag to indicate that we have received a program change message on MIDI_IN
static volatile unsigned char ChannelName[17];										// Global variable to store the channel name
static volatile unsigned char MIDIChannel = 0;

// USART Buffers
static volatile unsigned char ReceiveBuffer [RXBUFFERMAX];		// Buffer for received characters from USART
static volatile int ReceiveBufferTail = 0;							// Index for above Receive Buffer
static volatile int ReceiveBufferHead = 0;							// Index for above Receive Buffer
static volatile int ReceiveBufferLength = 0;						// Index for above Receive Buffer

static volatile unsigned char TransmitBuffer [TXBUFFERMAX];		// Buffer for received characters from USART
static volatile int TransmitBufferTail = 0;							// Index for above Transmit Buffer
static volatile int TransmitBufferHead = 0;							// Index for above Transmit Buffer
static volatile int TransmitBufferLength = 0;						// Length for above Transmit Buffer

/*
static const PROGMEM unsigned char BarGraphChar[] =
{
	0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98, 0x98,
	0x9c, 0x9c, 0x9c, 0x9c, 0x9c, 0x9c, 0x9c, 0x9c,
	0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e,
	0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f
};

*/


void LED(void);



int main (void)
{
	Init_Ports();											// Init the Ports
	
	sei();													// Enable Interrupts
	
//	lcd_init(LCD_DISP_ON);
	
	// Load the custom characters for the bar graph
//	lcd_command(_BV(LCD_CGRAM));  // set CG RAM start address 0
//    for(i=0; i<48; i++)
//    {
//		lcd_data(pgm_read_byte_near(&BarGraphChar[i]));
//    }
	
//	SendProgramChange(PodCurrentProgram);
	GetPodXTChannelSettings(PodCurrentProgram);
		
	for(;;) 
	{
		if (MIDIInfoReceived & PROG_CHANGED) { // Program Change at the POD
			bit_clear(MIDIInfoReceived, PROG_CHANGED);
			GetPodXTChannelSettings(PodCurrentProgram);
			
			cli();										// disabloidaan keskeytys kanavan vaihdon ajaksi
			asm("nop");
			
			//Ensimmäinen bank
			
			switch(PodCurrentProgram){
			
				case 0x00:
					PORTC |= _BV(boost);
					PORTC |= _BV(lead);
					LED();
					break;
				
				case 0x01:
					PORTC |= _BV(boost);
					PORTC |= _BV(lead);
					LED();
					break;
					
				case 0x02:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
				
				case 0x03:
					PORTC |= _BV(boost);
					PORTC |= _BV(lead);
					LED();
					break;
				
				//Toinen bank
				
				case 0x04:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
				
				case 0x05:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
					
				case 0x06:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
					
				case 0x07:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
				
				//Kolmas bank: A=Puhdas, B=crunch, C=LEAD, D=Boosted lead
				
				case 0x08:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
				
				case 0x09:
					PORTC |= _BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
					
				case 0x0A:
					PORTC &= ~_BV(boost);
					PORTC |= _BV(lead);
					LED();
					break;
				
				case 0x0B:					
					PORTC |= _BV(boost);
					PORTC |= _BV(lead);	
					LED();			
					break;
				
				//Neljäs bank: A=Puhdas, B=crunch, C=LEAD, D=Boosted lead
				
				case 0x0C:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
				
				case 0x0D:
					PORTC |= _BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
					
				case 0x0E:
					PORTC &= ~_BV(boost);
					PORTC |= _BV(lead);
					LED();
					break;
				
				case 0x0F:					
					PORTC |= _BV(boost);
					PORTC |= _BV(lead);
					LED();
					break;
					
				// 5. bank ... 
				case 0x10:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
				
				case 0x11:
					PORTC |= _BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
					
				case 0x12:
					PORTC &= ~_BV(boost);
					PORTC |= _BV(lead);
					LED();
					break;
				
				case 0x13:					
					PORTC |= _BV(boost);
					PORTC |= _BV(lead);
					LED();
					break;
				
				// 6. bank ... 
				case 0x14:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
				// 7. bank ... 
				case 0x15:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
				// 8. bank ... 
				case 0x16:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
				// 9. bank ... 
				case 0x17:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
				// 10. bank ... 
				case 0x18:
					PORTC &= ~_BV(boost);
					PORTC &= ~_BV(lead);
					LED();
					break;
			}
			
			
			sei();
			asm("nop");
			
			//lcd_put_int_xy(17, 1, PodCurrentProgram);
		}
		
	}
	
}


ISR(MIDI_USART_RX_vect)
{
	unsigned char ReceivedByte;
	int i;
	
	ReceivedByte = MIDI_UDR;
	
	if (ReceivedByte & 0x80) { //Status Byte Received
		ReceiveBufferTail = 0;
		ReceiveBufferHead = 0;
		ReceiveBufferLength = 0;
	}
	
	ReceiveBuffer[ReceiveBufferTail] = ReceivedByte;
	ReceiveBufferTail = (ReceiveBufferTail + 1) % RXBUFFERMAX;
	ReceiveBufferLength++;
	
	if ((ReceiveBuffer[ReceiveBufferHead] == (0xC0 | MIDIChannel)) && (ReceiveBufferLength > 1)) { 	// Program Change Message in Buffer
		PodCurrentProgram = ReceiveBuffer[(ReceiveBufferHead + 1)%RXBUFFERMAX];
		bit_set(MIDIInfoReceived, PROG_CHANGED);
	}
	
	
	if ((ReceiveBuffer[ReceiveBufferHead] == 0xF0) && (ReceiveBufferLength > 5)) { 					// Sysex String in Buffer
		if ((ReceiveBuffer[(ReceiveBufferHead + 5)%RXBUFFERMAX] == 0x71) && ReceiveBufferLength > 168) {				// Check the OpCode is 71 Block Dump and the full dump has been received
			// 9 bytes block dump header
			// 32 bytes channel name
			// 128 bytes of cc values
			
			for (i=0 ; i<16 ; i++) {
				ChannelName[i] = ReceiveBuffer[(ReceiveBufferHead + i + 9)%RXBUFFERMAX];
			}
			ChannelName[16] = '\0';
			
			(ReceiveBuffer[(ReceiveBufferHead + 66)%RXBUFFERMAX] == 127) ? (bit_set(MIDIInfoReceived, STOMP_STATUS)) : (bit_clear(MIDIInfoReceived, STOMP_STATUS));	// Stomp = CC 25 (25 + 9 + 32)
			(ReceiveBuffer[(ReceiveBufferHead + 91)%RXBUFFERMAX] == 127) ? (bit_set(MIDIInfoReceived, MOD_STATUS)) : (bit_clear(MIDIInfoReceived, MOD_STATUS));		// Modulation = CC 50 (50 + 9 + 32)
			(ReceiveBuffer[(ReceiveBufferHead + 69)%RXBUFFERMAX] == 127) ? (bit_set(MIDIInfoReceived, DELAY_STATUS)) : (bit_clear(MIDIInfoReceived, DELAY_STATUS));	// Delay = CC 28 (28 + 9 + 32)
			
			bit_set(MIDIInfoReceived,STATUS_CHANGED);
			bit_set(MIDIInfoReceived, BUTTON_CHANGED);
		}
	}
	
	if ((ReceiveBuffer[ReceiveBufferHead] == 0xb0) && ReceiveBufferLength > 2) {					// Control Change in Buffer
		if (ReceiveBuffer[(ReceiveBufferHead + 1)%RXBUFFERMAX] == 25) {
			(ReceiveBuffer[(ReceiveBufferHead + 2)%RXBUFFERMAX] == 127) ? (bit_set(MIDIInfoReceived, STOMP_STATUS)) : (bit_clear(MIDIInfoReceived, STOMP_STATUS));
		}
		
		if (ReceiveBuffer[(ReceiveBufferHead + 1)%RXBUFFERMAX] == 50) {
			(ReceiveBuffer[(ReceiveBufferHead + 2)%RXBUFFERMAX] == 127) ? (bit_set(MIDIInfoReceived, MOD_STATUS)) : (bit_clear(MIDIInfoReceived, MOD_STATUS));
		}
		
		if (ReceiveBuffer[(ReceiveBufferHead + 1)%RXBUFFERMAX] == 28) {
			(ReceiveBuffer[(ReceiveBufferHead + 2)%RXBUFFERMAX] == 127) ? (bit_set(MIDIInfoReceived, DELAY_STATUS)) : (bit_clear(MIDIInfoReceived, DELAY_STATUS));
		}
		bit_set(MIDIInfoReceived, BUTTON_CHANGED);
	}
}



ISR(MIDI_USART_UDRE_vect)
{

	if (TransmitBufferLength > 0) {  	// Data in buffer to transmit
		MIDI_UDR = TransmitBuffer[TransmitBufferHead];
		TransmitBufferHead = (TransmitBufferHead + 1) % TXBUFFERMAX;
		TransmitBufferLength--;
	}
	
	if (TransmitBufferLength == 0) {		// Buffer is empty turn off the interrupt
		MIDI_UCSRB &= ~_BV(MIDI_UDRIE);
	}
}


ISR(ADC_vect, ISR_NOBLOCK)
{ 
	NewCCValue = ((ADCH)>>1) & 0x7C;
	//NewCCValue = ((ADCH)>>1);
}


ISR(TIMER1_COMPA_vect)
{
	static unsigned char sw1, sw2, sw3, sw4, sw5, sw6, sw7;		// Shift registers for each switch that you want to track
	
	// Read the Dip switch on Port A (4,5,6,7) not concerned with bounce on this
	DIP_SW = ((~(PINA)) >> 4) & 0x0f;
		
	// Shift the oldest bit out of the register for each of the switches
	sw1 = sw1 << 1;											
	sw2 = sw2 << 1;
	sw3 = sw3 << 1;
	sw4 = sw4 << 1;
	sw5 = sw5 << 1;
	sw6 = sw6 << 1;
	sw7 = sw7 << 1;
	
	
	// If the switch is on, turn on the LSB of shift register
	(!((PINB) & 0x01)) ? (sw1 |= 0x01) : (sw1 &= ~(0x01));	//PB0
	(!((PINB) & 0x02)) ? (sw2 |= 0x01) : (sw2 &= ~(0x01));	//PB1
	(!((PINB) & 0x04)) ? (sw3 |= 0x01) : (sw3 &= ~(0x01));	//PB2
	(!((PINB) & 0x08)) ? (sw4 |= 0x01) : (sw4 &= ~(0x01));	//PB3
	(!((PINB) & 0x10)) ? (sw5 |= 0x01) : (sw5 &= ~(0x01));	//PB4
	(!((PIND) & 0x10)) ? (sw6 |= 0x01) : (sw6 &= ~(0x01));	//PD4
	(!((PIND) & 0x20)) ? (sw7 |= 0x01) : (sw7 &= ~(0x01));	//PD5


	
	// Action on the rising edge 00001111
	#define RISING_EDGE_MASK	0x07
	(sw1==RISING_EDGE_MASK) ? (key_press |= 0x01) : (key_press &= ~(0x01));
	(sw2==RISING_EDGE_MASK) ? (key_press |= 0x02) : (key_press &= ~(0x02));	
	(sw3==RISING_EDGE_MASK) ? (key_press |= 0x04) : (key_press &= ~(0x04));
	(sw4==RISING_EDGE_MASK) ? (key_press |= 0x08) : (key_press &= ~(0x08));
	(sw5==RISING_EDGE_MASK) ? (key_press |= 0x10) : (key_press &= ~(0x10));
	(sw6==RISING_EDGE_MASK) ? (key_press |= 0x20) : (key_press &= ~(0x20));
	(sw7==RISING_EDGE_MASK) ? (key_press |= 0x40) : (key_press &= ~(0x40));
	

	//Action on the falling edge 11110000
	#define	FALLING_EDGE_MASK 0xf0
	//(sw1==FALLING_EDGE_MASK) ? (key_press |= 0x01) : (key_press &= ~(0x01));
	//(sw2==FALLING_EDGE_MASK) ? (key_press |= 0x02) : (key_press &= ~(0x02));	
	//(sw3==FALLING_EDGE_MASK) ? (key_press |= 0x04) : (key_press &= ~(0x04));
	//(sw4==FALLING_EDGE_MASK) ? (key_press |= 0x08) : (key_press &= ~(0x08)); 
}



/*

void SendProgramChange(int ProgramNumber)
{
	unsigned char	Status;
	
	// MidiChannel = 0 - 15
	// Status		Program#	
	// 0xc[0-f]		0-127
	
	Status = 0xc0 | (MIDIChannel & 0x0f);  // Make sure MIDIChannel is 0 - 15
	
	MIDI_putc(Status);
	MIDI_putc(ProgramNumber & 0x7f); // Make sure the MSB is not set as this signifies a status byte
	
}

*/

void SendContinousControl(int ControlNumber, int Position)
{

	unsigned char	Status;
	
	// MidiChannel = 0 - 15
	// Status		Controller#		Position
	// 0xB[0-f]		0x4				0-127
	
	Status = 0xb0 | (MIDIChannel & 0x0f); // Make sure MIDIChannel is 0 - 15
	
	MIDI_putc(Status);
	MIDI_putc(ControlNumber & 0x7f);  // Make sure the MSB is not set as this signifies a status byte
	MIDI_putc(Position & 0x7f);  // Make sure the MSB is not set as this signifies a status byte
}

void GetPodXTChannelSettings(int ProgramNumber)
{
	// -----------------------------------------------------
	// Sysex string Block Dump Request - Sent to the Pod
	// F0 00 01 0C 03 73 00 00 00 01 F7
	
	// F0 			- Sysex start
	// 00 01 0c 	- Line 6 Manufacturer ID
	// 03 			- Pod XT ID
	// 73 			- Opcode ??
	// 00 00 00 01 	- Data ProgramNumber Memory User 1A
	// 00 3F 00 01  - Data ProgramNumber Memory User 16D
	// 01 40 00 01 	- Data ProgramNumber Memory User 17A
	// 01 7F 00 01  - Data ProgramNumber Memory User 32D
	// F7 			- Sysex end
	
	// -----------------------------------------------------
	// Sysex string Block Dump - Returned from Pod
	// F0 00 01 0C 03 71 02 nM nL [Data0..159] F7 F0 00 01 0C 03 72 F7
	
	// Data
	// 0-31 - Channel Name (ASCII)
	// 31-159
	
	
	// Sysex string Block Dump End - Returned from the Pod
	// F0 00 01 0C 03 72 F7
	
	MIDI_putc(0xf0);
	MIDI_putc(0x00);
	MIDI_putc(0x01);
	MIDI_putc(0x0c);
	MIDI_putc(0x03);
	MIDI_putc(0x73);
	if (ProgramNumber <= 63) {
		MIDI_putc(0x00);
	} else {
		MIDI_putc(0x01);
	}
	MIDI_putc(ProgramNumber);
	MIDI_putc(0x00);
	MIDI_putc(0x01);
	MIDI_putc(0xf7);
}

void Init_Ports (void)
{
	unsigned int baudrate;

	TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);			// Configure timer 1 for CTC mode, 64 prescale
	OCR1AL = (unsigned char) ((F_CPU/DEBOUNCE)/64);		// Set compare register 1152
	OCR1AH = (unsigned char) (((F_CPU/DEBOUNCE)/64)>>8);
	TIMSK |= _BV(OCIE1A); 									// Enable CTC interrupt


	// Set the baud rate
	baudrate = (((F_CPU / (MIDI_BAUDRATEVAL * 16.0))) - 1);
	MIDI_UBRRL = (unsigned char) baudrate;
	MIDI_UBRRH = (unsigned char) (baudrate >> 8);                  
	
	
	// Enable USART receiver and transmitter
	MIDI_UCSRB = _BV(MIDI_TXEN) | _BV(MIDI_RXEN) | _BV(MIDI_RXCIE); 
	
	
	// Set frame format: 8 data, 1stop, no parity
	MIDI_UCSRC = _BV(MIDI_UCSZ1) | _BV(MIDI_UCSZ0) | _BV(URSEL);
	
	// Setup PortB and PortD for switch input
	PORTB |= _BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2) | _BV(PORTB3) | _BV(PORTB4);
	PORTD |= _BV(PORTD4) | _BV(PORTD5);
	
	// Setup the ADC
	ADMUX |= _BV(REFS0) | _BV(ADLAR);				
	ADCSRA |= _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	
	
	DDRC = 0xFF;
	PORTC = 0x00;
	
	
	
	PORTC &= ~_BV(led);
	_delay_ms(500);
	PORTC |= _BV(led);
	_delay_ms(500);
	PORTC &= ~_BV(led);
	_delay_ms(500);
	PORTC |= _BV(led);
		
	
}

void MIDI_putc(unsigned char data)
{
	while (TransmitBufferLength > TXBUFFERMAX-1) { 		 // Wait for free space in the buffer
	
	}
	
	TransmitBuffer[TransmitBufferTail] = data;
	TransmitBufferTail = (TransmitBufferTail + 1) % TXBUFFERMAX;
	TransmitBufferLength++;
	
	// Enable the UDRE Transmit interrupt
	MIDI_UCSRB |= _BV(MIDI_UDRIE);
}

unsigned char MIDI_getc (void)
{
	unsigned char temp;
	
	if (ReceiveBufferLength > 0) {		// Data in the Receive Buffer
		temp = ReceiveBuffer[ReceiveBufferHead];
		ReceiveBufferHead = (ReceiveBufferHead + 1) % RXBUFFERMAX;
		ReceiveBufferLength--;
		return temp;
	} else {
		return 0;
	}
}

void LED(void){
	PORTC &= ~_BV(led);
	_delay_ms(100);
	PORTC |= _BV(led);
	_delay_ms(100);
	PORTC &= ~_BV(led);
	_delay_ms(100);
	PORTC |= _BV(led);
	}