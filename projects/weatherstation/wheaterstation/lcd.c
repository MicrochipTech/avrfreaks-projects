/********************************************************
**Project: 	Signalprocessing and Embedded Systems laboratory work
**
**Filename:	lcd.c
**
**Author:	Niklas Wiik, N85745
**
**Description: 	Implementation of lcd functions for Seiko
**				display L1672. Display datasheet can be 
**				found at http://www.elfa.se
********************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

/*Register select as PA1, 0=instruction
1=Data input*/
#define RS PA1 

/*Read/Write as PA2
**Datawrite = 0, Data read = 1*/ 
#define RW PA2 

#define EN PA3 //Enable as PA3

/*Databits
#define DB4 = PA4
#define DB5 = PA5
#define DB6 = PA6*/
#define DB7 = PA7  //DB7 is also busyflag

/*Function writes a command to the lcd display*/
void lcd_print(char aChar)
{
	lcd_busy(); //Don't write before lcd isn't busy
	PORTA = (aChar & 0xf0) | (1 << RS); //4 most significant bits to LCD
	PORTA |= (1 << EN);	//Write to LCD, enable = 1
	PORTA &= ~(1 << EN); //Enable back to 0
	PORTA = (aChar << 4) | (1 << RS);	//the following 4 bits to LCD
	PORTA |= (1 << EN);	//Write to LCD, enable = 1
	PORTA &= ~(1 << EN); //Enable back to 0
} 

/*Checks if busy flag in lcd display is set*/
void lcd_busy()
{
	PORTA = 0xf4; 	//Pull up data bits and set RW bit for read
	DDRA = 0x0f; 	//Data bits set as input for AVR
	PORTA |= (1 << EN);		 	//Enable set;
	while(PINA & (1 << PA7));		//loop until busyflag is zero
	DDRA = 0xff;	//Data bits as output
	PORTA = 0x00;   //Display ready for instructions
}

/*Initializes the LCD display*/
void lcd_init()
{
    lcd_command(0x20);   // Change to 4-bit mode from 8-bit mode    
    lcd_command(0x28);    // 4-bit mode, Two line, 5 * 7dot
    lcd_command(0x0c);   //Disp ON, cursor OFF
    lcd_command(0x01);   //Clear display                   
    lcd_command(0x06);   //no disp shift  
    lcd_command(0x02);  //Cursor to first character
}

		

/*Gives commands to the display*/
void lcd_command(char aChar)
{
	lcd_busy(); //Don't write before lcd isn't busy
	PORTA = (aChar & 0xf0); //4 most significant bits to LCD
	PORTA |= (1 << EN);	//Write to LCD, enable = 1
	PORTA &= ~(1 << EN); //Enable back to 0
	PORTA = (aChar << 4);	//the following 4 bits to LCD
	PORTA |= (1 << EN);	//Write to LCD, enable = 1
	PORTA &= ~(1 << EN);
} 


/*Mehtod to print several characters to display*/
void lcd_printf(char *data)
{
	while(*data != '\0')
	{
		lcd_print(*data++);
	}
}


/*Change cursor position*/
void lcd_goto(int x_point, int y_point)
{
	if(y_point == 1)
	{
	x_point = x_point + 0x40; //Cursor to second line
	}
	x_point |= (1 << PA7); //DB7 must be set to change cursor
	lcd_command(x_point);
}

/*Writes weather data to LCD*/
void lcd_update_weather(int temperatur)
{
	char res[4]; //3 digit result
	itoa(temperatur, res, 10); //Convert int to string
    res[3] = '\0';

	char firstDig[3];

	strncpy(firstDig, res, 2); //Copy first two digits
	
	firstDig[2] = '\0'; 
	lcd_goto(5,0);	 
	lcd_printf(firstDig); //Print the first two digits
	lcd_print('.'); //print point
	lcd_print(res[2]); //Print last digit
	lcd_print('C');
	char c = 0b11011111; 
	lcd_print(c); // print celsius square 
			
}


void lcd_clr()
{
	lcd_goto(0,0);
	int i = 0;
	for(i; i<40; i++)
		lcd_print(' ');
	lcd_goto(0,0);	
}
