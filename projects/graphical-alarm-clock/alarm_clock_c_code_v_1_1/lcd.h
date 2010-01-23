/*
Author:  Matt Meerian
Date:  December 22, 2004
Target Processor:  ATMEGA8535 from Atmel
Languate used:  AVR GCC compiler (in the "C" language)
Overview:
        This file contains functions for basic operation of the 128x64 pixel,
        2.8", blue and white, LCD from Optrex.  (Part Number F-51553GNBJ-LW-AB)  The
        hardware interface will be through a demo board from Apollo Display.  (Part
        Number:  F-51553-Eval
		The display will use the serial interface to drive the LCD.  The LCD is
		set to the "80 series" (intel) interface.
*/

#include <define.h>

void set_line(unsigned char uLineNumber);      //set the line of text to write
	// on the graphical LCD
void write_data(unsigned char uLCDData);		//write data to the graphical LCD
void send_serial_data(unsigned char uLCDSerial);	//write a command or data
	// serially to the display
void write_char(unsigned char uWriteLcdChar, bool boolCharInvert);	//write a character to the LCD
void clear_display(void);	//clear the entire LCD display
unsigned char get_status(void);	//NOT USED!!  get the status from the graphical LCD
void moveCursorToColumn(unsigned char uCurSpaces);	//move to a location on the current row
void write_pixel_spaces(unsigned char uPixelSpaces);	//write white spaces to the current row on the LCD


//----------------------------------------------------------------------------
//Routine:     InitLCD  (Initialize the LCD)
//
//Function:    initialization of the Optrex F-51553, 2.8 inch graphical LCD.
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//Routine:     set_line (set the line number of text to write to the LCD)
//
//Function:
//			Sets the current line for the display to write.  The column is
//			set to zero
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void set_line(unsigned char uLineNumber)
{
    //unsigned char uTango=0;
    write_command(0x10);	//column address high
    write_command(0x00);	//column address low

    uLineNumber=uLineNumber+0xb0;   //add an offset of B0h to the line number
    write_command(uLineNumber);
    //NOTE:  in the orginal 8051 assembly code from Apollo Display this routine fell through
    //  to the write_command function.
}


//----------------------------------------------------------------------------
//Routine:     write_data (write data to the graphical LCD)
//
//Function:
//		This function serially writes data to the LCD display
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void write_data(unsigned char uLCDData)
{
    sbi(defLCDContPort,defLcdRD);       //change state on the read line of the LCD
    sbi(defLCDContPort,defLcdA0);       //set A0 on the LCD for a command
    cbi(defLCDContPort,defLcdCS1);      //clear the Chip Select on the LCD for
    cbi(defLCDContPort,defLcdWR);       //change state on the Write line of the LCD
	send_serial_data(uLCDData);					//send the data to the LCD serially
    sbi(defLCDContPort,defLcdWR);
    sbi(defLCDContPort,defLcdCS1);      //now set the Chip Select on the LCD
    cbi(defLCDContPort,defLcdRD);       //change state on the read line of the LCD
}


//----------------------------------------------------------------------------
//Routine:     send_serial_data  (send serial data to the LCD)
//
//Function:
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void send_serial_data(unsigned char uLCDSerial)
{
	unsigned char uBitCount;
	unsigned char uLCDTempSerial;

	uLCDTempSerial=uLCDSerial;	//get the data
	uLCDTempSerial=uLCDTempSerial & 0x80;		//clear all the bits but D7

    //outp(uLCDTempSerial,defLCDDataPort);   //ouput data
	if(uLCDTempSerial==0x80)
	{
		sbi(defLCDDataPort,defLcdS1);
	}
	else
	{
		cbi(defLCDDataPort,defLcdS1);
	}
	asm("nop");
	cbi(defLCDDataPort,defLcdSCL);	//clear the clock
	asm("nop");
	sbi(defLCDDataPort,defLcdSCL);	//set the clock
	for(uBitCount=0;uBitCount<7;uBitCount++)
	{
		uLCDTempSerial=uLCDSerial;	//get the data
		uLCDTempSerial = uLCDTempSerial << 1;       // shift Bits one left
		uLCDSerial=uLCDTempSerial;	//save the data
		uLCDTempSerial=uLCDTempSerial & 0x80;		//clear all the bits but D7
		//outp(uLCDTempSerial,defLCDDataPort);   //ouput data
		if(uLCDTempSerial==0x80)
		{
			sbi(defLCDDataPort,defLcdS1);
		}
		else
		{
			cbi(defLCDDataPort,defLcdS1);
		}
		asm("nop");
		cbi(defLCDDataPort,defLcdSCL);	//clear the clock
		asm("nop");
		sbi(defLCDDataPort,defLcdSCL);	//set the clock
	}
}

//----------------------------------------------------------------------------
//Routine:     write_char (write an ASCII character to the display)
//
//Function:
//
//       The font file starts at zero, a space.
//		Note:  The routine set_line(XX); needs to be called to set the line for
//			characters to be displayed upon.
//
//Varibles passed:
//       uWriteLcdChar (the character to write to the LCD)
//Returns:
//       none
//----------------------------------------------------------------------------
void write_char(unsigned char uWriteLcdChar, bool boolCharInvert)
{
	unsigned char uLoaded=0;
	unsigned char uDelta;
	unsigned int intEpsilon;

	for(uLoaded=0;uLoaded<5;uLoaded++)
	{
		intEpsilon=(uWriteLcdChar*5)+uLoaded;	//figure out the offset to the slice of the character
		uDelta=PRG_RDB(&font5x7[intEpsilon]);	//get the raw data character data out of flash
		if(boolCharInvert)
		{
			uDelta=uDelta^255;
		}
		write_data(uDelta);		//write the data to the display
	}

	if(boolCharInvert)	//a blue character on a white background
	{
		write_data(0xff);	//this puts a verticle, on pixel space between characters
	}
	else	//a white character on a blue background
	{
		write_data(0x00);	//This puts a verticle, off pixel space between characters
	}

}

//----------------------------------------------------------------------------
//Routine:     clear_display (clears the currently displayed screen)
//
//Function:
//
//		This function steps through the 8 rows on the 128x64 pixel LCD display and
//		turns off all the display bits.
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void clear_display(void)
{
	unsigned char uVertPix=0;
	unsigned char uCurrLine=0;

	for(uCurrLine=0;uCurrLine<8;uCurrLine++)
	{
		set_line(uCurrLine);	//sets the current line to display information upon
		for(uVertPix=0;uVertPix<=128;uVertPix++)	//step through the 128 bytes on each row on the LCD
		{
			write_data(0);		//clear off the current byte in the LCD
		}
	}
}

//----------------------------------------------------------------------------
//Routine:     get_status (get the status from the graphical LCD)
//
//Function:
//		Is this only for a parallel bus?
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
unsigned char get_status(void)
{
    unsigned char uLCDStatus;

    //outp(0xFF,defLCDDataPort);   //release the data bus
	sbi(defLCDDataPort,defLcdS1);
	sbi(defLCDDataPort,defLcdSCL);

    cbi(defLCDContPort,defLcdA0);       //clear A0 on the LCD for a command
    cbi(defLCDContPort,defLcdCS1);      //clear the Chip Select on the LCD for
    cbi(defLCDContPort,defLcdRD);       //clear the Read line on the LCD
    uLCDStatus=inp(defLCDDataPort);
    sbi(defLCDContPort,defLcdRD);       //set the Read line on the LCD
    sbi(defLCDContPort,defLcdCS1);      //set the Chip Select line on the LCD
    return(uLCDStatus);
}

//----------------------------------------------------------------------------
//Routine:     moveCursorToColumn (moves the cursor to a specific location
//
//Function:
//				This function moves the current cursor on the row to whatever
//				column number is passed to it.  The command to the LCD to move the
//				cursor takes two bytes.  The high byte is in the format 0x1* and the
//				low byte is in the format 0x0*.  (where * is a number from 0x00 to
//				0x0f.
//Varibles passed:
//       uCurSpaces (the number of spaces from the left of the display)
//Returns:
//       none
//----------------------------------------------------------------------------
void moveCursorToColumn(unsigned char uCurSpaces)
{
	unsigned char x;

	x=uCurSpaces & 0xF0;	//first, we get rid of the low nibble
	x=x>>1;		//then, we shift the high nibble down to the low nibble
	x=x>>1;
	x=x>>1;
	x=x>>1;
	x=x+0x10;	//we then add 0x10 to it to tell the LCD it is a column address
	write_command(x);	//set the high byte of the column address
	x=uCurSpaces & 0x0F;	//next, we get rid of the orginal high nibble
	write_command(x);	//set the low byte of the column address
}

//----------------------------------------------------------------------------
//Routine:     write_pixel_spaces (write spaces to a row)
//
//Function:
//			This function fills pixels with white spaces.  For Example,
//			if the number passed to it is one, it writes in 8 verticle pixels that
//			are not active on the current row.
//Varibles passed:
//       uPixelSpaces
//Returns:
//       none
//----------------------------------------------------------------------------
void write_pixel_spaces(unsigned char uPixelSpaces)
{
	unsigned char r;

	for(r=0;r<uPixelSpaces;r++)
	{
		write_data(00);		//write the data to the display
	}
}




