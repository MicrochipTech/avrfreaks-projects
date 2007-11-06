/***************************

IR Test

Written by Ken J Jewell

Compiled with AVR IAR

******************************/

#pragma language=extended // enable use of extended keywords

#ifndef ENABLE_BIT_DEFINITIONS // enable the bit definitions in the iom32.h file
#define ENABLE_BIT_DEFINITIONS
#endif

#include <inavr.h>
#include <ioavr.h>

// macros
#define	BIN2BCD(b) (((b)%10) | (((b)/10)<<4)) /* Binary 2 BCD conversion */
#define	BCD2BIN(b) (((b)&0xf) + ((b)>>4)*10)  /* BCD 2 Binary conversion */

// PORT A
#define	DB0			0x01;
#define	DB1			0x02;
#define	DB2			0x04;
#define	DB3			0x08;
#define	DB4			0x10;
#define	DB5			0x20;
#define	DB6			0x40;
#define	DB7			0x80;

// PORT B
#define	RS			0x01;
#define	RW			0x02;
#define	E				0x04;

// PORT D
#define	IR			0x04;
#define	LED			0x40;

// function prototypes
int main(void);
void writeHex(unsigned char data);
void clockLCD(unsigned char data);
void writeString(unsigned char *string);
void writeASCII(unsigned char data);
void writeLCD(unsigned char data);
unsigned char readLCD(void);
void delay_ms(unsigned int x);
void delay_1us(void);
void delay_us(unsigned int x);

// registers
static unsigned int oscDrift;
static unsigned char retry;
static unsigned char command, address, toggle;
static unsigned char command_last, address_last, toggle_last;
static unsigned char bit_sync;
static unsigned char bit[28];

int main(void)
{
	unsigned char temp;
	
	PORTA = 0xFF;
	DDRA = 0xFF;
	
	PORTB = 0xFF;
	DDRB = 0xFF;
	
	PORTC = 0xFF;
	DDRC = 0xFF;
	
	PORTD = 0xFF;
	DDRD = 0xFB;
		
  MCUCSR = (1<<JTD);
  GICR = (1<<INT0); 
  MCUCR = (1<< ISC00);

  __enable_interrupt();
	
	delay_ms(100);		// wait > 15ms
		
	clockLCD(0x30);
	delay_ms(5);
	clockLCD(0x30);
	delay_us(150);
	clockLCD(0x30);
	delay_us(100);
	
	writeLCD(0x38);	// function set DL: 8bit, N: 2line, F: 5x7
	
	writeLCD(0x08);	// display off
	
	writeLCD(0x01);	// clear display
	
	writeLCD(0x06);	// entry mode set I/D: 1, S: 0
		
	writeLCD(0x0C);	// display on, C: off, B: off
	
	// splash screen
	writeLCD(0x80);	// line 1, position 0
	writeString ("IR Test Unit");
	writeLCD(0xC0);	// line 2, position 0
	writeString ("Firmware V1.0");
	
	delay_ms(1000);
	
	writeLCD(0x01);	// clear display
		
	writeLCD(0xC0);	// line 2, position 0
	writeString ("A:   C:   S/T:  ");

	toggle_last = 0;
	command_last = 0;
	address_last = 0;

	address = 0xFF;
	command = 0xFF;
	toggle = 0xFF;

	for (;;)
	{
		if ((address_last != address) || (command_last != command) || (toggle_last != toggle))
		{
			address_last = address;
			command_last = command;
			toggle_last = toggle;
			
  		writeLCD(0xC2);	// line 2, position 2
  		writeHex(address);	// display address
  							
  		writeLCD(0xC7);	// line 2, position 7
  		writeHex(command);	// display command
  
  		writeLCD(0xCE);	// line 2, position 14
  		writeHex(toggle);
  		
  		writeLCD(0x80);	// line 1, position 0
  		writeString ("B:             ");	// clear last field
  
  		writeLCD(0x82);	// line 1, position 2
  		
  		if ((toggle & 0x30) == 0x20)
  		{
  			temp = command | 0x80;
  		}
  		else
  		{
  			temp = command;
  		}	
  			
  		if (address == 0x03)
  		{
  			retry = 0;
  			
  			switch (temp)
  			{
  				case 0x1D:
  					writeString ("GREEN (TV)");
  					break;
  
  				case 0x1F:
  					writeString ("BLUE (TV)");
  					break;
  
  				case 0x1C:
  					writeString ("PAY (TV)");
  					break;
    
  				case 0x1E:
  					writeString ("RADIO (TV)");
  					break;

  				case 0x0B:
  					writeString ("AUDIO (TV)");
  					break;

  				case 0x0A:
  					writeString ("PICTURE (TV)");
  					break;
  
  				// extra start bit commands

   				case 0x80 + 0x0E:
  					writeString ("YELLOW (TV)");
  					break;
  				
  				case 0x80 + 0x28:
  					writeString ("ALARM (TV)");
  					break;

  				case 0x80 + 0x0D:
  					writeString ("INFO (TV)");
  					break;

  				case 0x80 + 0x29:
  					writeString ("RED (TV)");
  					break;

  				default :
  					writeString ("UNKNOWN (TV)");
  					break;
  			}
  		}
  		else if ((address == 0x00) || (address == 0x07))
  		{
  			switch (temp)
  			{
  				case 0x01:
  					writeString ("CH 1 (TV)");
  					break;
  
  				case 0x02:
  					writeString ("CH 2 (TV)");
  					break;
  
  				case 0x03:
  					writeString ("CH 3 (TV)");
  					break;
  
  				case 0x04:
  					writeString ("CH 4 (TV)");
  					break;
  
  				case 0x05:
  					writeString ("CH 5 (TV)");
  					break;
  
  				case 0x06:
  					writeString ("CH 6 (TV)");
  					break;
  
  				case 0x07:
  					writeString ("CH 7 (TV)");
  					break;
  
  				case 0x08:
  					writeString ("CH 8 (TV)");
  					break;
  
  				case 0x09:
  					writeString ("CH 9 (TV)");
  					break;
  										  
  				case 0x00:
  					writeString ("CH 0 (TV)");
  					break;
  
   				case 0x3C:
  					writeString ("TEXT (TV)");
  					break; 

  				case 0x38:
  					writeString ("AV (TV)");
  					break;
  
  				case 0x11:
  					writeString ("VOL- (TV)");
  					break;
  
   				case 0x10:
  					writeString ("VOL+ (TV)");
  					break;

  				case 0x0D:
  					writeString ("MUTE (TV)");
  					break;
    
  				case 0x21:
  					writeString ("CH - (TV)");
  					break;
  
  				case 0x20:
  					writeString ("CH + (TV)");
  					break;
  
  				case 0x0C:
  					writeString ("STANDBY (TV)");
  					break;

  				case 0x26:
  					writeString ("SLEEP (TV)");
  					break;
  					
  				case 0x0F:
  					writeString ("OSD (TV)");
  					break;
  
  				case 0x23:
  					writeString ("I-II (TV)");
  					break;
  					
  				// extra start bit commands
  				case 0x80 + 0x15:
  					writeString ("LEFT (TV)");
  					break;
  
  				case 0x80 + 0x16:
  					writeString ("RIGHT (TV)");
  					break;
  
  				case 0x80 + 0x12:
  					writeString ("MENU (TV)");
  					break;
  
  				case 0x80 + 0x17:
  					writeString ("OK (TV)");
  					break;
  
  				case 0x80 + 0x11:
  					writeString ("DOWN (TV)");
  					break;
  
  				case 0x80 + 0x10:
  					writeString ("UP (TV)");
  					break;
  				
  				default :
  					writeString ("UNKNOWN (TV)");
  					break;
  			}		
  		}
  		else
  		{
  			switch (oscDrift)
  			{
  				case 929:
  					oscDrift = 889;	// normal
  					retry = 1;
  					break;

  				case 889:
  					oscDrift = 849;	// -40uS tolerance
  					retry = 2;
  					break;
  					  					
					case 849:
						oscDrift = 929;	// +40uS tolerance
						retry = 3;
						break;
					
					default:
						oscDrift = 889;	// normal
						retry = 0;
						break;
				}
				
				writeLCD(0x80);	// line 1, position 0
				
				if (retry > 0)
				{
					writeString ("TRY AGAIN!");
					
					writeLCD(0x8F);	// line 1, position 15
					
					if (retry == 3)
					{
						writeString ("+");
					}
					else if (retry == 2)
					{
						writeString ("-");
					}
					else
					{
						writeString (" ");
					}
				}
				else
				{
					writeString ("READY");
				}
				
				
			}
  	}
  }
}

void writeHex(unsigned char data)
{
	unsigned char temp;
	
	temp = (data & 0xF0) >> 4;
		
	if (temp < 10)
	{
		temp += 48;	// convert to ASCII 0-9
	}
	else
	{
		temp += 55;	// convert to ASCII A-F
	}

	writeASCII(temp);

	temp = data & 0x0F;
		
	if (temp < 10)
	{
		temp += 48;	// convert to ASCII 0-9
	}
	else
	{
		temp += 55;	// convert to ASCII A-F
	}

	writeASCII(temp);
}

void clockLCD(unsigned char data)
{
	PORTA = data;

	delay_1us();
	PORTB &= ~E;
	delay_1us();
	PORTB |= E;
}

void writeString(unsigned char *string)
{
	while (*string)
	{
 	  writeASCII(*string++);
	}
}

void writeASCII(unsigned char data)
{
	PORTB |= RS;
	PORTB &= ~RW;
	PORTB |= E;
	delay_us(100);
	clockLCD(data);	// write high nibble
	
	while (readLCD() & 0x80)	// wait for busy flag
		;
}

void writeLCD(unsigned char data)
{
	PORTB &= ~RS;
	PORTB &= ~RW;
	PORTB |= E;
	delay_us(100);
	clockLCD(data);	// write high nibble
	
	while (readLCD() & 0x80)	// wait for busy flag
		;
}

unsigned char readLCD(void)
{
	unsigned char temp;
	DDRA = 0x00;
	PORTA = 0xFF;
	
	PORTB &= ~RS;
	PORTB |= RW;
	
	PORTB |= E;
	delay_us(100);
	temp = PINA;	// read high nibble
	delay_us(100);
	PORTB &= ~E;
	delay_1us();
	
	DDRA = 0xFF;
		
	return temp;
}
	
// delay x millisecond
void delay_ms(unsigned int x)
{
  delay_us(997); // The function call and setup will take exactly 3 us.
  // This while loop will take exactly 1 ms per turn. Run ms - 1 times.
  while (--x) 
  {
    delay_us(999);
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
  }
}

// delay 1 microsecond
void delay_1us(void)
{
  // call is 1 instruction
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  __no_operation();
  // return is 1 instruction
}
  
// delay x microsecond
void delay_us(unsigned int x) 
{
  x--;
  __no_operation();
  __no_operation();
    
  // This while loop will take exactly 1 us per turn.
  while (--x) 
  {
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __watchdog_reset(); // kick the dog
  }
}

#pragma vector=INT0_vect
__interrupt void INT0_irqHandler(void)
{
	PORTD &= ~LED;
	
	bit_sync = 0;
	
	GICR &= ~(1<<INT0); 

  TCCR1B = (1<<WGM12) | (1<<CS11); // timer1: ctc, clk/8
  OCR1A = 444;	// 444uS - half bit
	TCNT1 = 0;
	TIMSK |= (1<<OCIE1A); // timer0: overflow, timer1: output_compare a
	bit[bit_sync++] = 0x04;
}

#pragma vector=TIMER1_COMPA_vect
__interrupt void OCR1A_irqHandler(void)
{
	unsigned char bitR;

	unsigned char startBit;
	unsigned char toggleBit;
	unsigned char addressByte;
	unsigned char commandByte;
	
	bit[bit_sync++] = PIND & IR;
	
	OCR1A = oscDrift;
	
	if (bit_sync == 28)
	{
		TCCR1B = 0;
		TIMSK &= ~(1<<OCIE1A);

		GICR |= (1<<INT0);

		bit_sync = 0;
		startBit = 0;
		toggleBit = 0;
		addressByte = 0;
		commandByte = 0;
		
		while (bit_sync < 28)
		{
			bitR = 0xFF;
			
			if ((bit[bit_sync] == 0) && (bit[bit_sync + 1] != 0))
			{
				bitR = 0;
			}
			
			if ((bit[bit_sync] != 0) && (bit[bit_sync + 1] == 0))
			{
				bitR = 1;
			}
						
			bit_sync ++;
			
			if (bit_sync <= 3)
			{
				startBit = startBit << 1;
				startBit |= bitR;
			}
			else if (bit_sync == 5)
			{
				toggleBit = bitR;
			}
			else if ((bit_sync >= 6) && (bit_sync <= 15))
			{
				addressByte = addressByte << 1;
				addressByte |= bitR;
			}
			else if ((bit_sync >= 16) && (bit_sync <= 27))
			{
				commandByte = commandByte << 1;
				commandByte |= bitR;
			}
			
			bit_sync ++;
		}
				
		if ((startBit == 0x02) || (startBit == 0x03))
		{
			address = addressByte & 0x3F;
			command = commandByte & 0x3F;
			toggle = ((startBit & 0x03) << 4) | toggleBit & 0x01;
		}
			
		PORTD |= LED;
	}
}
