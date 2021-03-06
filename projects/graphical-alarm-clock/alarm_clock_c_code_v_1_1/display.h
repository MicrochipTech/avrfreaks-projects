/*
Author:  Matt Meerian
Date:  January 21, 2005
Target Processor:  ATMEGA8535 from Atmel
Languate used:  AVR GCC compiler (in the "C" language)
Overview:
	This file will be used to test the basic operation of the 128x64 pixel,
	2.8", blue and white, LCD from Optrex.  (Part Number F-51553GNBJ-LW-AB)  The
	hardware interface will be through a demo board from Apollo Display.  (Part
	Number:  F-51553-Eval
	Writes different things to the LCD screen.
*/

//#include <define.h>

void alphabet(void);		//display the alphabet
void kustom_image(void);
void stop_image(void);
void BigDigits(unsigned char uStartRow, unsigned char uStartColumn, unsigned int intNumber, unsigned char uNumbersDisplayed, bool boolInvert);
void write_eagle_display(void);	//write a bit-mapped image to the LCD
void drawText(unsigned char *pts,unsigned char uPosition, unsigned char uRow, bool boolInvert);
void subLargeCOlon(unsigned char uStartRow, unsigned char uStartColumn, bool boolInvert);

//----------------------------------------------------------------------------
//Routine:     alphabet ()
//
//Function:
//		This writes all the characters in the font set to the LCD display.
//		Starts at the top left of the screen and ends at the bottom right
//		of the screen
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void alphabet(void)
{
	unsigned char uAlphaLine=0;
	unsigned char uAlpha;
	unsigned char uBeta;
	unsigned char uGama=0x00;
	unsigned char uDelta;
	unsigned int intEpsilon;

	for(uAlphaLine=0;uAlphaLine<8;uAlphaLine++)
	{
		set_line(uAlphaLine);	//set the line for writing
		for(uAlpha=0;uAlpha<21;uAlpha++)
		{
			if(uGama>95)	//have we got to the end of the font set?
			{
				uGama=0x00;	//yes, we got to the end, reset the font character counter
			}
			for(uBeta=0;uBeta<5;uBeta++)	//This steps through the 5 characters for the ASCII character
			{
				intEpsilon=(uGama*5)+uBeta;	//figure out the offset to the slice of the character
				uDelta=PRG_RDB(&font5x7[intEpsilon]);	//get the raw data character data out of flash
				write_data(uDelta);		//write the data to the display
			}
			write_data(0);	//put in a space between characters
			uGama=uGama+1;
		}
	}
}

//----------------------------------------------------------------------------
//Routine:     kustom_image (write the  logo to the full screen)
//
//Function:
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void kustom_image(void)
{
	unsigned int uByteCounter;		//count off every byte, there are 128 bytes in each row
	unsigned char uRowCounter;		//count off all 8 rows on the screen

	for(uRowCounter=0;uRowCounter<8;uRowCounter++)		//there are 8 rows to write
	{
		set_line(uRowCounter);	//set the row for writing
		for(uByteCounter=0;uByteCounter<127;uByteCounter++)	//128 bytes of data in each row
		{
			write_data(PRG_RDB(&strKustom[(128*uRowCounter)+uByteCounter]));	//get the raw data
			//  character data out of flash, 128 bytes for each row of data.
		}
	}
}

void stop_image(void)
{
	unsigned int uByteCounter;		//count off every byte, there are 128 bytes in each row
	unsigned char uRowCounter;		//count off all 8 rows on the screen

	for(uRowCounter=0;uRowCounter<8;uRowCounter++)		//there are 8 rows to write
	{
		set_line(uRowCounter);	//set the row for writing
		for(uByteCounter=0;uByteCounter<127;uByteCounter++)	//128 bytes of data in each row
		{
			write_data(PRG_RDB(&strStop[(128*uRowCounter)+uByteCounter]));	//get the raw data
			//  character data out of flash, 128 bytes for each row of data.
		}
	}
}


//Parameters for the passed variables:
//	uStartRow: 0 to 7, the start row of the big text
//	uStartColum:  0 to 112, the pixel count to start the number from the left side of the display
//  IntNumber:  0 to 999, the number to display in big pixels
//	uPeriodPos:  NOT IMPLIMENTED:  0 to 6, the position to display the period, ".", in the number
//  boolInvert: if true, blue on a white background, if false, white on a blue background

void BigDigits(unsigned char uStartRow, unsigned char uStartColumn, unsigned int intNumber, unsigned char uNumbersDisplayed, bool boolInvert)
{
	unsigned char uAlpha;
	unsigned char uBeta;
	unsigned char uGamma;

	unsigned int intTor;
	unsigned int intS;

	bool boolFirstNonZero=false;

	integerToArray(intNumber);	//convert the big font number to a string
	set_line(uStartRow);		//set the line for writing a string of characters
	moveCursorToColumn(uStartColumn);	//write white spaces to get to the right column
	boolFirstNonZero=false;	//preperation for the blanking the leading zeros
	for(uGamma=1;uGamma<4;uGamma++)	//this for loop writes the top 1/3 of each of the 3 digits
	{
		uBeta=str[uGamma]-0x30;		//convert the number into an ASCII character
		if(uBeta==0 && uGamma==1)	//if there is a zero in the hundreds place, then write a blank space
		{
			write_pixel_spaces(15);	//write white spaces to get to the right column
		}
		else if(uBeta==0 && uGamma==2 && boolFirstNonZero==false && uNumbersDisplayed<2)
		//go write spaces if the hundreds digit was zero and tens digit is zero
		{
			write_pixel_spaces(15);	//write white spaces to get to the right column
		}
		else if(uBeta==0 && uGamma==3 && boolFirstNonZero==false && uNumbersDisplayed<1)
		//go write spaces if the hundreds digit was zero, the tens digit is zero,
		//  and the ones digit is zero
		{
			//moveCursorToColumn(uStartColumn+15);	//write white spaces to get to the right column
			write_pixel_spaces(15);	//write white spaces to get to the right column
		}
		else
		{
			boolFirstNonZero=true;	//we must have had our first non-zero digit
			for(uAlpha=0;uAlpha<15;uAlpha++)	//get the top 16 bytes out of flash for the current digit
			{
				intTor=(uBeta*48)+uAlpha;	//calculate the offset to the character we need
				intS=PRG_RDB(&strBigFont0[intTor]);	//get the raw data character data out of flash
				if(boolInvert)
				{
					intS=intS^255;
				}
				write_data(intS);		//write the data to the display
			}
		}
	}
	set_line(uStartRow+1);		//set the line for writing a string of characters
	moveCursorToColumn(uStartColumn);
	boolFirstNonZero=false;	//preperation for the blanking the leading zeros
	for(uGamma=1;uGamma<4;uGamma++)	//this for loop writes the middle 1/3 of each of the 3 digits
	{
		uBeta=str[uGamma]-0x30;
		if(uBeta==0 && uGamma==1)	//if there is a zero in the hundreds place, then write a blank space
		{
			write_pixel_spaces(15);	//write white spaces to get to the right column
		}
		else if(uBeta==0 && uGamma==2 && boolFirstNonZero==false && uNumbersDisplayed<2)
		//go write spaces if the hundreds digit was zero and tens digit is zero
		{
			write_pixel_spaces(15);	//write white spaces to get to the right column
		}
		else if(uBeta==0 && uGamma==3 && boolFirstNonZero==false && uNumbersDisplayed<1)
		//go write spaces if the hundreds digit was zero, the tens digit is zero,
		//  and the ones digit is zero
		{
			write_pixel_spaces(15);	//write white spaces to get to the right column
		}
		else
		{
			boolFirstNonZero=true;
			for(uAlpha=16;uAlpha<31;uAlpha++)	//get the middle 16 bytes out of flash for the current digit
			{
				intTor=(uBeta*48)+uAlpha;	//calculate the offset to the character we need
				intS=PRG_RDB(&strBigFont0[intTor]);	//get the raw data character data out of flash
				if(boolInvert)
				{
					intS=intS^255;
				}
				write_data(intS);		//write the data to the display
			}
		}
	}
	set_line(uStartRow+2);		//set the line for writing a string of characters
	moveCursorToColumn(uStartColumn);
	boolFirstNonZero=false;	//preperation for the blanking the leading zeros
	for(uGamma=1;uGamma<4;uGamma++)	//this loop writes the bottom 1/3 of each of the 3 digits
	{
		uBeta=str[uGamma]-0x30;		//convert the number into an ASCII character
		if(uBeta==0 && uGamma==1)	//if there is a zero in the hundreds place, then write a blank space
		{
			write_pixel_spaces(15);	//write white spaces to get to the right column
		}
		else if(uBeta==0 && uGamma==2 && boolFirstNonZero==false && uNumbersDisplayed<2)
		//go write spaces if the hundreds digit was zero and tens digit is zero
		{
			write_pixel_spaces(15);	//write white spaces to get to the right column
		}
		else if(uBeta==0 && uGamma==3 && boolFirstNonZero==false && uNumbersDisplayed<1)
		//go write spaces if the hundreds digit was zero, the tens digit is zero,
		//  and the ones digit is zero
		{
			write_pixel_spaces(15);	//write white spaces to get to the right column
		}
		else
		{
			boolFirstNonZero=true;
			for(uAlpha=32;uAlpha<47;uAlpha++)	//get the bottom 16 bytes out of flash for the current digit
			{
				intTor=(uBeta*48)+uAlpha;	//calculate the offset to the character we need
				intS=PRG_RDB(&strBigFont0[intTor]);	//get the raw data character data out of flash
				if(boolInvert)
				{
					intS=intS^255;	//inverts the background with forground color, This exclusive or's the value from flash
				}
				write_data(intS);		//write the data to the display
			}
		}
	}
}

//----------------------------------------------------------------------------
//Routine:     write_eagle_display (write a bit-mapped image to the graphical LCD)
//
//Function:
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void write_eagle_display(void)
{
	//drawText(string to print, column to start on, line (row) to put the text upon, invert the colors?)
	//drawText((unsigned char *)strPatrol,93,0,false);
	//drawText((unsigned char *)strTarget,10,0,false);
	//drawText((unsigned char *)strLock,55,4,false);

	//BigDigits(unsigned char uStartRow, unsigned char uStartColumn, unsigned int intNumber, unsigned char )

	//BigDigits(5,40,156,0,true);

	//write an example time to the display
	BigDigits(1,50,59,0,false);
	subLargeCOlon(1,52,false);
	BigDigits(1,10,12,0,false);
}


//----------------------------------------------------------------------------
//Routine:     drawText (prints an array of 5x7 characters to the LCD)
//
//Function:
//		This function draws a string of data to the LCD in 5x7 pixel format.
//		The X (column) and Y (row) position is passed to the array.  The position
//		numbers start from the upper left hand corner of the display.
//
//Varibles passed:
//      unsigned char *pts:  This is a pointer to the array that will be printing to the LCD
//		unsigned char uPosition:  (value =0 to 120) this is the number of pixels from the left side of the
//			screen that the text will start.  (it sets the column start position of the text)
//		unsigned char uRow:  (value = 0 to 7) this sets the row the text will be printed upon.
//		ubool boolInvert:  When false- white characters on a blue background
//			when true - blue characters on a white background.
//Returns:
//       none
//----------------------------------------------------------------------------
void drawText(unsigned char *pts,unsigned char uPosition,unsigned char uRow, bool boolInvert)
{
	unsigned char x=0;

	if(uRow!=255 && uPosition!=255)	//when the row and column is 255, do not set the cursor position, use the current one
	{
		set_line(uRow);	//set the line (row) for writing
		moveCursorToColumn(uPosition);	//set the number of pixels from the left to start writing.
	}
	if(boolInvert)	//a blue character on a white background
	{
		write_data(0xff);	//this puts a verticle, one pixel space between characters
	}

	while(x!=0xff)		//The array will be terminated with a 0xFF character
	{
		x=PRG_RDB(pts);		//get the byte out of flash based upon the pointer,
		if(x!=0xff)		//have we reached the end of the display array?
		{
			write_char(x-32,boolInvert);	//adjust the character to the font table and
				// then write it to the screen.
			pts++;	//advance the pointer to the flash table in preparation for the next reading.
		}
	}
	if(!boolInvert)	//a blue character on a white background, takes off the 8 pixels on the end of the text
	{
		write_data(0x00);	//this puts a verticle, on pixel space after the line has printed
	}

}


//Parameters for the passed variables:
//	uStartRow: 0 to 7, the start row of the big text
//	uStartColum:  0 to 112, the pixel count to start the number from the left side of the display
//  boolInvert: if true, blue on a white background, if false, white on a blue background
void subLargeCOlon(unsigned char uStartRow, unsigned char uStartColumn, bool boolInvert)
{
	unsigned char uAlpha;
	unsigned int intS;

	bool boolFirstNonZero=false;

	set_line(uStartRow);		//set the line for writing a string of characters
	moveCursorToColumn(uStartColumn);	//write white spaces to get to the right column
	boolFirstNonZero=true;	//we must have had our first non-zero digit
	for(uAlpha=0;uAlpha<15;uAlpha++)	//get the top 16 bytes out of flash for the current digit
	{
		intS=PRG_RDB(&strBigFontColon[uAlpha]);	//get the raw data character data out of flash
		if(boolInvert)
		{
			intS=intS^255;
		}
		write_data(intS);		//write the data to the display
	}
	set_line(uStartRow+1);		//set the line for writing a string of characters
	moveCursorToColumn(uStartColumn);
	for(uAlpha=16;uAlpha<31;uAlpha++)	//get the middle 16 bytes out of flash for the current digit
	{
		intS=PRG_RDB(&strBigFontColon[uAlpha]);	//get the raw data character data out of flash
		if(boolInvert)
		{
			intS=intS^255;
		}
		write_data(intS);		//write the data to the display
	}
	set_line(uStartRow+2);		//set the line for writing a string of characters
	moveCursorToColumn(uStartColumn);
	boolFirstNonZero=false;	//preperation for the blanking the leading zeros
	boolFirstNonZero=true;
	for(uAlpha=32;uAlpha<47;uAlpha++)	//get the bottom 16 bytes out of flash for the current digit
	{
		intS=PRG_RDB(&strBigFontColon[uAlpha]);	//get the raw data character data out of flash
		if(boolInvert)
		{
			intS=intS^255;	//inverts the background with forground color, This exclusive or's the value from flash
		}
		write_data(intS);		//write the data to the display
	}
}








