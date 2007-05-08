#include <iom8535v.h>
#include <stdio.h>

#include "putchar.c"
#include "getchar.c"

#define UART_TRANSMIT_ON()	UCSRB |= 0x8
#define UART_TRANSMIT_OFF()	UCSRB &= ~0x8
#define UART_RECEIVE_ON()	UCSRB |= 0x10
#define UART_RECEIVE_OFF()	UCSRB &= ~0x10

// Serial Backpack command delimiter
#define CMD_MODE 0xFE

// Display CLEAR text command
#define DSP_CLR_CMD 0x01
// Displat HOME cursor command
#define DSP_HOME_CMD 0x02

// Display ENTRY MODE command & parameters
#define DSP_ENTRY_CMD 0x04
#define CUR_DIR 0x02
#define CUR_MOVE 0x01

// Display CONTROL MODE & parameters
#define DSP_CON_CMD 0x08
#define DSP_ON 0x04
#define CUR_ON 0x02
#define CUR_BLINK 0x01

// Display CURSOR CONTROL & parameters
#define DSP_CUR_CMD 0x10
#define DSP_SHIFT 0x08
#define DISPLAY_L_R 0x04

// Display LINE POSITION addressing
#define LINE1 0x80
#define LINE2 0xC0
#define LINE3 0x94
#define LINE4 0xD4

// Display DATA & CHARACTER GENERATOR select bits
#define DD_RAM 0x80
#define CG_RAM 0x40

// Custom Character DDRAM location addressing
#define DDRAM_START_ADD_0 0x00
#define DDRAM_START_ADD_1 0x01
#define DDRAM_START_ADD_2 0x02
#define DDRAM_START_ADD_3 0x03
#define DDRAM_START_ADD_4 0x04
#define DDRAM_START_ADD_5 0x05
#define DDRAM_START_ADD_6 0x06
#define DDRAM_START_ADD_7 0x07

// Custom Character CGRAM Starting addresses
#define CGRAM_START_ADD_0 0x40
#define CGRAM_START_ADD_1 0x48
#define CGRAM_START_ADD_2 0x50
#define CGRAM_START_ADD_3 0x58
#define CGRAM_START_ADD_4 0x60
#define CGRAM_START_ADD_5 0x68
#define CGRAM_START_ADD_6 0x70
#define CGRAM_START_ADD_7 0x78

// LED enable switch input
#define LED 4
// ASCII control code (0xFC) turns on LED
#define LED_ON 252 
// ASCII control code (0xFD) turns on LED
#define LED_OFF 253

#define BannerMessage1 "My_LCD"
#define BannerMessage2 "Serial  Backpack"
#define BannerMessage3 "By"
#define BannerMessage4 "Carl W. Livingston"

void MakeCustomChar (const char*, char);
void delay(unsigned int);
void clear_disp(void);
void display_banner(void);
void timer1(void);
void uart0_init(void);
void init_devices(void);

// Smiley face
const char Smiley_face[] = {// Smiley face
	  	   				    0b00001110,
	  	   				    0b00011111,
						    0b00000000,
	  	   				    0b00001010,
						    0b00000100,
	  	   				    0b00010001,
						    0b00001110,
							0b00000000};
const char Thin_cross[] = {// Thin cross
							0b00010001,
	  	   			    	0b00000000,
					    	0b00011111,
	  	   			    	0b00010101,
					    	0b00011011,
	  	   			    	0b00001110,
					    	0b00010001,
	  	   			    	0b00000000};
const char Thick_cross[] = {// Thick cross
							0b00000000,
	  	   			    	0b00001110,
					    	0b00011011,
	  	   			    	0b00010001,
					    	0b00011011,
	  	   			    	0b00001110,
					    	0b00000000,
	  	   			    	0b00000000};
const char Music_note[] = {// Music note
							0b00000011,
	  	   			    	0b00000010,
					    	0b00000111,
	  	   			    	0b00000010,
					    	0b00011110,
	  	   			    	0b00010010,
					    	0b00011110,
	  	   			    	0b00000000};
const char Loop_Antenna[] = {// Loop Antenna
							0b00010001,
	  	   			    	0b00010001,
					    	0b00011111,
	  	   			    	0b00010101,
					    	0b00010101,
							0b00000100,
					    	0b00001110,
	  	   			    	0b00000000};
const char Lh_Yagi_Antenna[] = {// Lh Yagi Antenna
							0b00010000,
	  	   			    	0b00010100,
					    	0b00010101,
	  	   			    	0b00011111,
					    	0b00010101,
	  	   			    	0b00010100,
					    	0b00010000,
	  	   			    	0b00000000};
const char Diamond[] = {// Diamond
							0b00000000,
	  	   			    	0b00011111,
					    	0b00010001,
	  	   			    	0b00010101,
					    	0b00010001,
	  	   			    	0b00011111,
					    	0b00000000,
	  	   			    	0b00000000};
const char Rh_Yagi_Antenna[] = {// Rh Yagi Antenna
							0b00000001,
	  	   			    	0b00000101,
					    	0b00010101,
	  	   			    	0b00011111,
					    	0b00010101,
	  	   			    	0b00000101,
					    	0b00000001,
	  	   			    	0b00000000};

int	DelayCounter;
unsigned long int	DisplayCounter = 0; 

void main()
{
 	init_devices(); // Initialize the required Mega8535 devices
	
 	delay(40); // Wait for LCD to boot up	

	UART_TRANSMIT_ON(); // Turn on the USART transmitter		

    MakeCustomChar (Smiley_face, CGRAM_START_ADD_0);
	MakeCustomChar (Lh_Yagi_Antenna, CGRAM_START_ADD_1);
	MakeCustomChar (Diamond, CGRAM_START_ADD_2);
	MakeCustomChar (Rh_Yagi_Antenna, CGRAM_START_ADD_3);
    MakeCustomChar (Thin_cross, CGRAM_START_ADD_4);
	MakeCustomChar (Thick_cross, CGRAM_START_ADD_5);
	MakeCustomChar (Music_note, CGRAM_START_ADD_6);
	MakeCustomChar (Loop_Antenna, CGRAM_START_ADD_7);		
	
	if (PINB & (1<<LED)) // If LED enable switch is active,
	 	putchar(LED_ON); // Turn on the LED
		 
	// Put the banner message on the display	
	clear_disp(); // Clear the display for the banner message
	display_banner(); // Display the banner message
	delay(1000); // Display banner message for 1 second
	clear_disp(); // Clear the display for incoming data

	while(1)
	{
	 PORTB ^= (1<<PORTB0); // Sync Pulse for testing
	 
	 putchar(CMD_MODE); // Put the LCD in command mode
 	 putchar(LINE2);	// Set the LCD cursor position			 
	 printf("%8lu", DisplayCounter);

	 // Write the custom characters to the display
	 putchar(CMD_MODE); // Put the LCD in command mode
 	 putchar(LINE1);	// Set the LCD cursor position	
	 putchar(DDRAM_START_ADD_0); // Display custom character 0	
	 putchar(DDRAM_START_ADD_1); // Display custom character 1	
	 putchar(DDRAM_START_ADD_2); // Display custom character 2	
	 putchar(DDRAM_START_ADD_3); // Display custom character 3	
	 putchar(DDRAM_START_ADD_4); // Display custom character 4	
	 putchar(DDRAM_START_ADD_5); // Display custom character 5	
	 putchar(DDRAM_START_ADD_6); // Display custom character 6		 
	 putchar(DDRAM_START_ADD_7); // Display custom character 7	
	 	
	 if (PINB & (1<<LED)) // If LED enable switch is active,
		putchar(LED_ON); // Turn on the LED 
	 else putchar(LED_OFF); // Else, turn off the LED
	}
}

void MakeCustomChar (const char *Symbol, char Address) {
    // Set up the custom characters
	char n = NULL;
	putchar(CMD_MODE); // Put the LCD in command mode
	putchar(Address); // Set the display to CGRAM mode, address 0
	// Make the custom characters
	while (n < 8) {
	 	  putchar(Symbol[n++]); // Send current character to the display
	}
}

// Integer delay routine
// 65.535 seconds maximum
// All things wait on this delay function to term
void delay(unsigned int t)
{
 	 DelayCounter = t;
 	 while(DelayCounter != 0);
}	  

// Clear the display	
void clear_disp(void)
{
	putchar(CMD_MODE); // Put the LCD in command mode
	putchar(DSP_CLR_CMD); // Send the clear the LCD display
	delay(30);	// Wait 30 miliseconds
}

// Print the banner message	
void display_banner(void)
{
	putchar(CMD_MODE); // Put the LCD in command mode
	putchar(LINE1+5); // Set the LCD cursor position	
	printf("%s", BannerMessage1); // Display line 1 of the banner message

	putchar(CMD_MODE); // Put the LCD in command mode
	putchar(LINE2+0); // Set the LCD cursor position				
	printf("%s", BannerMessage2); // Display line 2 of the banner message	
		
	putchar(CMD_MODE); // Put the LCD in command mode
	putchar(LINE3+9); // Set the LCD cursor position			
	printf("%s", BannerMessage3); // Display line 3 of the banner message	

	putchar(CMD_MODE); // Put the LCD in command mode
	putchar(LINE4+1); // Set the LCD cursor position			
	printf("%s", BannerMessage4); // Display line 4 of the banner message		
}

// Timer 1 Intrrupt Service Routine
// Call one time per milisecond
#pragma interrupt_handler timer1:7
 void timer1(void)
  {
//   	  PORTB ^= (1<<PORTB0);  
   	  DelayCounter -= 1; // Decrement the delay counter
	  DisplayCounter++;  // Increment the LCD display test counter
  }
  
void port_init(void)
{
 PORTA = NULL;
 DDRA  = NULL;
 PORTB = 0xFF;
 DDRB  = 0x01;
 PORTC = NULL;
 DDRC  = 0xFF;
 PORTD = NULL;
 DDRD  = 0xFF;
}

// TIMER1 initialize - prescale:64
// WGM: 0) Normal, TOP=0xFFFF
// Desired value: 1KHz
// Actual value:  1.000KHz (0.0%)
void timer1_init(void)
{
 TCCR1B = NULL;		  // Stop Timer Counter 1
 TCNT1 = 0;			  // Clear Timer Counter 1
 OCR1AH = NULL;		  // We want a 1.000 Milisecond delay resolution
 OCR1AL = 0x72;
 TCCR1A = NULL;		  // Generate no outputs
 TCCR1B = 0x0B;		  // Start Timer Counter 1 with OCR1A, CLK / 64
}

// BAUD rate definitions
// Actual baud rate = 9600 BAUD, (0.0% error) @ 7.3728MHz
// Actual baud rate = 19.2K BAUD, (0.0% error) @ 7.3728MHz
// Actual baud rate = 38.4K BAUD, (0.0% error) @ 7.3728MHz
// Actual baud rate = 115.2K BAUD, (0.0% error) @ 7.3728MHz
#define BAUD_4800 95
#define BAUD_9600 47
#define BAUD_14400 31
#define BAUD_19200 23
#define BAUD_28800 15
#define BAUD_38400 11
#define BAUD_57600 7
#define BAUD_76800 5
#define BAUD_115200 3

// Initialize UART0
void uart0_init(void)
{
 UCSRB = NULL; // Disable while setting baud rate
 UCSRA = NULL;
 UCSRC = (1<<URSEL) | 0x06;
 UBRRL = BAUD_115200; // Set baud rate lo
 UBRRH = NULL; // Set baud rate hi
 UCSRB = 0x18;
}

// Initialize required controller peripherals
void init_devices(void)
{
 asm("cli"); // Disable global interrupts
 port_init(); // Call the I/O port initialization
 timer1_init(); // Call the timer 1 initialization
 uart0_init(); // Call the IUSART initialization 
 
 MCUCR = NULL;
 GICR  = NULL;
 TIMSK = 0x10; // Enable Timer 1 interrupt
 asm("sei"); // Re-enable global interrupts
 // Controller peripherals are now initialized
}
