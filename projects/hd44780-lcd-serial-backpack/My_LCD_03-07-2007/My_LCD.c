// My_LCD.c

/*
Two port, 8 BIT LCD control program
Writen by Carl W. Livingston
microcarl@roadrunner.com
AVR Freaks member - microcarl
January 02, 2007
*/

/*************************************************************/
/*************************************************************/
#include <iot2313v.h>
//#include <iom16v.h>
//#include <iom32v.h>
/*************************************************************/
/*************************************************************/

/*	//// From the LCD perspective \\\\
		RxD --> PORTD:0
		LED <-- PORTD:1 // High = ON, Low = OFF
		J_1 --> PORTD:2 // BAUD rate select
		J_2 --> PORTD:3 // BAUD rate select
		LCD:R/W <-- PORTD:4
		LCD:RS <-- PORTD:5
		LCD:E <-- PORTD:6
		N/A <-> PORTD:7

		LCD:R/W <-- PORTD:4
		LCD:RS <-- PORTD:5
		LCD:E <-- PORTD:6
		LCD:Vee <-- CONTRAST
		LCD:DB0 <-- PORTB:0
		LCD:DB1 <-- PORTB:1
		LCD:DB2 <-- PORTB:2
		LCD:DB3 <-- PORTB:3
		LCD:DB4 <-- 4.7K Ohm <-- PORTx:4
		LCD:DB5 <-- 4.7K Ohm <-- PORTx:5
		LCD:DB6 <-- 4.7K Ohm <-- PORTx:6
		LCD:DB7 <-- 4.7K Ohm <-- PORTx:7

	//// From the I/O PORTx perspective \\\\
		PORTD:0 <--RxD
		PORTD:1 --> LED // High = ON, Low = OFF
		PORTD:2 <-- J_1 // BAUD rate select
		PORTD:3 <-- J_2 // BAUD rate select
		PORTD:4 --> LCD:R/W
		PORTD:5 --> LCD:RS
		PORTD:6 --> LCD:E
		PORTD:7 <-> N/A	
	
		PORTB:0 --> LCD:DB0
		PORTB:1 --> LCD:DB1
		PORTB:2 --> LCD:DB2
		PORTB:3 --> LCD:DB3
		PORTB:4 --> 4.7K Ohm --> LCD:DB4
		PORTB:5 --> 4.7K Ohm --> LCD:DB5
		PORTB:6 --> 4.7K Ohm --> LCD:DB6
		PORTB:7 --> 4.7K Ohm --> LCD:DB7
*/

/*************************************************************/
/*************************************************************/
// If you want to use a different I/O port for LCD control & data,
// do it here!!!
#define LCD_DATA_OUT PORTB
#define LCD_DATA_IN PINB
#define LCD_DATA_DDR DDRB

#define LCD_CONTROL_OUT PORTD
#define LCD_CONTROL_IN PIND
#define LCD_CONTROL_DDR DDRD

// Define LCD Read/Write as PORTx, 0x10;
#define LCD_RW 4
// Define LCD Register Select as PORTx, 0x20;  
#define LCD_RS 5
// Define LCD Enable as PORTx, 0x40;  
#define LCD_E 6
/*************************************************************/
/*************************************************************/

// LCD busy status bit
#define LCD_BUSY 7
// LED control bit
#define LED 1
// BAID rate control bits
#define J_1 2
#define J_2 3 
#define CGRAM 6  

// Turn on power to the display, no cursor
#define	PWR_ON	0x0C
// Set 8 data bits
#define	DATA_8	0x30
// Set 8 data bits, 4 display lines
#define	LINE_8x4	0x38
// Clear display command		
#define CLR_DSP 0x01
// Character generator RAM command
#define REG_MODE 0xFE

#define NULL 0x00

// ASCII control code (0xFC) turns on LED
#define LED_SW_ON 252
// ASCII control code (0xFD) turns off LED
#define LED_SW_OFF 253
							
void MakeCustomChar (const char *, char);
void LCD_Delay (unsigned int d);
void LCD_INIT (void);
void LCD_PutCmd (char);
void LCD_PutChar (char); 
char LCD_BusyWait (void);
void USART_Init (void);
char USART_BAUD (void);

char USART_DATA = NULL;

void main (void) {
	LCD_INIT ();
	
	// Initialize the AVR USART	
 	USART_Init ();
	
	while(1) {
    	while (!(UCSRA & (1 << RXC))); // RXC1: USART Recieve Complete
		USART_DATA = UDR; // Get USART data		  


		if (USART_DATA == LED_SW_OFF)
			LCD_CONTROL_OUT &= ~(1<<LED);
		else if (USART_DATA == LED_SW_ON)
			LCD_CONTROL_OUT |= (1<<LED);
		else if (USART_DATA == REG_MODE) {
		   while (!(UCSRA & (1 << RXC))); // RXC1: USART Recieve Complete	
		   LCD_PutCmd (UDR); // Send LCD command character
		}
		else LCD_PutChar (USART_DATA); // Send LCD data character


	}			
}

// Clock cycle = 67nS @ 14.7456MHz	
// Delay resolution ~ 1uS @ 14.7456MHz
void LCD_Delay (unsigned int d) {
	 while (d-- != 0);
	asm("nop");
}

void LCD_INIT (void) {
	// Set J_2:J_1 PULL-UPS active
	LCD_CONTROL_OUT = (1<<J_2) | (1<<J_1);
	// Set LCD_control J_2:J_1 to inputs	
	LCD_CONTROL_DDR = 0xF2;
	
    // Delay resolution ~ 1uS @ 14.7456MHz			
	LCD_Delay (15000); // Need 15mS delay for LCD to power up
	LCD_CONTROL_OUT |= (1<<LCD_RS); // Set LCD_RS HIGH

	// Initialize the AVR controller I/O
	LCD_DATA_DDR = 0xFF; // Set LCD_DATA_OUT as all outputs
	LCD_DATA_OUT = 0x00; // Set LCD_DATA_OUT to logic low

	// Initialize the LCD controller
	LCD_CONTROL_OUT &= ~(1<<LCD_RS);

	LCD_PutChar (DATA_8);
	// Delay resolution ~ 1uS @ 14.7456MHz	
	LCD_Delay (4100); // Need 4.1mS delay

	LCD_PutChar (DATA_8);
	// Delay resolution ~ 1uS @ 14.7456MHz	
	LCD_Delay (100); // Need 100uS delay

	LCD_PutChar (DATA_8);
	LCD_CONTROL_OUT |= (1<<LCD_RS);
	
	// The display will be out into 8 bit data mode here
	LCD_PutCmd (LINE_8x4); // Set 8 bit data, 4 display lines
	LCD_PutCmd (PWR_ON); // Power up the display
	LCD_PutCmd (CLR_DSP); // Power up the display
}

void LCD_PutCmd (char Cmd) {
	LCD_CONTROL_OUT &= ~(1<<LCD_RS);
	LCD_BusyWait();
	LCD_DATA_OUT = Cmd;	
	asm ("nop");	
	LCD_CONTROL_OUT |= (1<<LCD_E);
	asm ("nop"); // PWeh must be 230nS minimum 
	asm ("nop"); // nop = 67nS @ 14.7456MHz	
	asm ("nop"); 
	asm ("nop"); // Here, E = 271nS @ 14.7456MHz	
	LCD_CONTROL_OUT &= ~(1<<LCD_E);
	asm ("nop");
	LCD_CONTROL_OUT |= (1<<LCD_RS);
}

void LCD_PutChar (char c) {
	LCD_CONTROL_OUT &= ~(1<<LCD_RS);
	LCD_BusyWait();
	LCD_CONTROL_OUT |= (1<<LCD_RS);
	LCD_DATA_OUT = c;
	asm ("nop");			
	LCD_CONTROL_OUT |= (1<<LCD_E);
	asm ("nop"); // PWeh must be 230nS minimum 
	asm ("nop"); // nop = 67nS @ 14.7456MHz	
	asm ("nop"); 	
	asm ("nop"); // Here, E = 271nS @ 14.7456MHz
	LCD_CONTROL_OUT &= ~(1<<LCD_E);
}

char LCD_BusyWait (void) {
	unsigned char LCDStatus; 
	LCD_DATA_DDR = 0x00; // Set LCD data port to inputs
	LCD_CONTROL_OUT |= (1<<LCD_RW);
	asm ("nop");
	do {
	   LCD_CONTROL_OUT |= (1<<LCD_E);	
	   asm ("nop"); // PWeh must be 230nS minimum 
	   asm ("nop"); // nop = 67nS @ 14.7456MHz	
	   asm ("nop"); 	
	   asm ("nop"); // Here, E = 271nS @ 14.7456MHz
	   LCDStatus = LCD_DATA_IN;
	   LCD_CONTROL_OUT &= ~(1<<LCD_E);
	} while ((LCDStatus & (1<<LCD_BUSY)) == (1<<LCD_BUSY));
	LCD_CONTROL_OUT &= ~(1<<LCD_RW);	
	LCD_DATA_DDR |= 0xFF; // Set LCD data port to outputs		
	return (LCDStatus);
}   		 

// Initialize UART0 
void USART_Init(void)
{
 UCSRB = NULL; // Disable while setting baud rate
 UCSRA = NULL;
 UCSRC = (1<<UCSZ1) | (1<<UCSZ0); // 8 bit data
 UBRRL = USART_BAUD (); // Set baud rate
 UBRRH = NULL; // Set baud rate hi
 UCSRB = (1<<RXEN); // RXEN = Enable
}

// Actual baud rate = 9600 BAUD, (0.0% error) @ 14.7456MHz
// Actual baud rate = 19.2K BAUD, (0.0% error) @ 14.7456MHz
// Actual baud rate = 38.4K BAUD, (0.0% error) @ 14.7456MHz
// Actual baud rate = 115.2K BAUD, (0.0% error) @ 14.7456MHz
#define BAUD_4800 191
#define BAUD_9600 95
#define BAUD_14400 63
#define BAUD_19200 47
#define BAUD_28800 31
#define BAUD_38400 23
#define BAUD_57600 15
#define BAUD_76800 11
#define BAUD_115200 7 

char USART_BAUD (void) {
	char BaudSelectJumpersValue;
	static char BaudLookupTable[] = {BAUD_115200, BAUD_38400, BAUD_19200, BAUD_9600};

	// Get BAUD rate jumper settings	   
	BaudSelectJumpersValue  = LCD_CONTROL_IN;
	// Mask off unwanted bits
	BaudSelectJumpersValue &= (1<<J_2) | (1<<J_1);
	// J_2 = PD.3, J_1 = PD.2
	// This is two bits too far to the left and the array index will
	// increment by 4, instead of 1.
	// Shift J_2 & J_1 right by two bit positions for proper array indexing
	BaudSelectJumpersValue = (BaudSelectJumpersValue >> J_1);

	return BaudLookupTable[BaudSelectJumpersValue];
}
