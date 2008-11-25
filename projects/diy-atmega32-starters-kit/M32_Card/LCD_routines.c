// **************************************************************
//       *** 	    ROUTINES FOR LCD DISPLAY  		***
// **************************************************************
//Controller:	ATmega32 (16Mhz Crystal)
//Compiler:		ICCAVR
//Author:		CC Dharmani, Chennai, India
//Date:			Aug 2008
//**************************************************************
#include "LCD_routines.h"
#include "i2c_routines.h"
#include <iom32v.h>
#include <macros.h>

// ***** Define I/O pins ***** //

#if 0
// *** Set port for LCD Data Bus 8 bit mode *** //
#define LCD_OP_PORT				PORTC
#define LCD_IP_PORT				PINC
#define LCD_DIR_PORT			DDRC

// *** Set LCD Enable Port and Bit values *** //
#define LCD_EN_PORT				PORTD
#define EN_BIT					BIT7

// *** Set LCD Register Select Port and Bit values *** //
#define LCD_RS_PORT				PORTD
#define RS_BIT					BIT5

// *** Set LCD Read/Write Port and Bit values *** //
#define LCD_RW_PORT				PORTD
#define RW_BIT					BIT6
#endif

 
volatile unsigned char *LCD_DIR_PORT = &DDRC;
volatile unsigned char *LCD_IP_PORT = &PINC;
volatile unsigned char *LCD_OP_PORT = &PORTC;

#define ENABLE_LCD     	        PORTD |= 0x80 
#define DISABLE_LCD       	    PORTD &= ~0x80

#define SET_LCD_DATA      	    PORTD |= 0x20
#define SET_LCD_CMD       	    PORTD &= ~0x20

#define SET_LCD_READ      	    PORTD |= 0x40
#define SET_LCD_WRITE      	    PORTD &= ~0x40


#define LCD_ON				0x0F   
#define LCD_CURS_ON			0x0D
#define LCD_OFF				0x08
#define LCD_HOME			0x02  
#define LCD_CLEAR			0x01
#define LCD_NEW_LINE		0xC0

#define LCD_FUNCTION_SET	0x38
#define LCD_MODE_SET		0x06


 //*****************************************************//
 // This routine will check the busy flag of the LCD //
 //*****************************************************//
 
void LCD_CheckBusy ( void )
{
 
 delay_ms(5);	   	 	  //this delay will do instead of checking busy flag of LCD
 
  		 	   	 	  	  //disabling the checkBusy part if you don't want to connect LCD,
 						  //otherwise the program will go into infinite loop
 						  //even if LCD connected, the program will work
 /*
 unsigned char temp;
 
 DISABLE_LCD;	   				// Disable LCD

 *LCD_DIR_PORT = 0x00;			// Make I/O Port input

 SET_LCD_READ;					// Set LCD to READ
 SET_LCD_CMD;					// Set LCD to command

 ENABLE_LCD;	   				 

 asm("nop");
 asm("nop");

 do
 {
  temp = PINC;					 // Load data from port
 }
 while(temp & BIT7);			 // Wait till LCD is busy	

DISABLE_LCD;	   				 // Disable LCD
*/
}


// ********************************************** //
// *** Write a command instruction to the LCD *** //
// ********************************************** //

void LCD_WriteCommand (unsigned char CMD)
{
	DISABLE_I2C;
	
	LCD_CheckBusy();			// Test if LCD busy

	DISABLE_LCD;	   			// Disable LCD

	SET_LCD_WRITE ;				// Set LCD to write
	SET_LCD_CMD;				// Set LCD to command

	*LCD_DIR_PORT = 0xFF;		// LCD port output
	*LCD_OP_PORT = CMD;			// Load data to port

	ENABLE_LCD;	   				// Write data to LCD
	asm("nop");
	asm("nop");
	DISABLE_LCD;	   			// Disable LCD
	
	ENABLE_I2C;
}


// ***************************************** //
// *** Write one byte of data to the LCD *** //
// ***************************************** //

void LCD_WriteData (unsigned char Data)
{
	DISABLE_I2C;
	
	LCD_CheckBusy(); 		 	  	// Test if LCD Busy

	DISABLE_LCD;	   				// Disable LCD

	SET_LCD_WRITE ;					// Set LCD to write
	SET_LCD_DATA;					// Set LCD to data

	*LCD_DIR_PORT = 0xFF;			// LCD port output
	
	*LCD_OP_PORT = Data;			// Load data to port

	ENABLE_LCD;	   				 	// Write data to LCD
	asm("nop");
	asm("nop");
	DISABLE_LCD;	   				 // Disable LCD
	
	ENABLE_I2C;
}


// ********************************* //
// *** Initialize the LCD driver *** //
// ********************************* //
	
void LCD_init(void)
{
	delay_ms(100);
	LCD_WriteCommand (LCD_FUNCTION_SET);
	LCD_WriteCommand (LCD_MODE_SET);
	LCD_WriteCommand (LCD_OFF);
	LCD_WriteCommand (LCD_CLEAR);
	LCD_WriteCommand (LCD_ON);
	LCD_WriteCommand (LCD_HOME);
}


// ********************************************************************* //
// *** Display a string at the specified row and column, using FLASH *** //
// ********************************************************************* //

void LCD_DisplayString_F (char row, char column ,const unsigned char *string)
{
	LCD_Cursor (row, column);
	while (*string)
		LCD_WriteData(*string++);
}


void LCD_DisplayString (char row, char column , unsigned char *string)
{
	LCD_Cursor (row, column);
	while (*string)
		LCD_WriteData(*string++);
}


// *************************************************** //
// *** Position the LCD cursor at "row", "column". *** //
// *************************************************** //
void LCD_Cursor (char row, char column)
{
	switch (row) {
		case 1: LCD_WriteCommand (0x80 + column - 1); break;
		case 2: LCD_WriteCommand (0xc0 + column - 1); break;
		//case 3: LCD_WriteCommand (0x94 + column - 1); break;
		//case 4: LCD_WriteCommand (0xd4 + column - 1); break;
		default: break;
	}
}

// *************************************************** //
// ***** FUNCTION FOR SOFTWARE DELAY OF 1 mSEC ******* //
// *************************************************** //

void delay_ms(int miliSec)  //for 14.7456 Mhz crystal
{
  int i,j;
  
  for(i=0;i<miliSec;i++)
    for(j=0;j<1550;j++)
	{
	  asm("nop");
	  asm("nop");
	}
}

