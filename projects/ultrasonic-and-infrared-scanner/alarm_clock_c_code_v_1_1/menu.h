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
	Handles the basic painting and matence of the menus.
*/


//#include <define.h>

void paintMenu(unsigned char *pts,unsigned char uMinHighlight,unsigned char uMaxHighLight);

//----------------------------------------------------------------------------
//Routine:     paintMenu(paints the menus when there is a change)
//
//Function:
//		This routine handles painting the menus.  (all 8 lines)  A highlight bar can be 
//		moved up and down.
//
//Varibles passed:
//      unsigned char *pts:  This is the pointer to the 2D array to print the full menu screen
//		unsigned char uMinHighlight:  the text lines on the LCD are numbered 0 to 7, starting
//			from the top down.  This is the closest that the highlight can go to line 0.
//		unsigned char uMaxHighlight:  the text lines on the LCD are numbered 0 to 7, starting
//			from the top down.  This is the closest that the highlight can go to line 7. 
//			(the bottom of the screen)
//Returns:
//       none
//----------------------------------------------------------------------------
void paintMenu(unsigned char *pts,unsigned char uMinHighlight,unsigned char uMaxHighlight)
{
	unsigned char x;
	
	if(uMinHighlight != 0 && uMaxHighlight != 0)	//do not put up the highlight when 
		// the min and max highlight value is 0 and 0.
	{
		if(uSelectedLine > uMaxHighlight)	//have we reached the bottom of the area to highlight?
		{
			uSelectedLine=uMinHighlight;	//yes, circle back around to the top of the area we can highlight
		}
		if(uSelectedLine < uMinHighlight)	//have we reached the top of the area to highlight?
		{
			uSelectedLine=uMaxHighlight;	//yes, circle back around to the bottom of the area we can hightlight
		}
	}
	//the next line draws the title line on the menu
	drawText(&pts[0],0,0,false);	//string, column, line, inverted
	//The next line was "for(x=1;x<7;x++)" when there was soft button labels
	for(x=1;x<8;x++)	//This steps through the main area on lines 1 to 7
	{		
		if(uMinHighlight != 0 && uMaxHighlight != 0 && uSelectedLine==x)
		{	//only go in here when we are reached the highlighted line, there is only one
			//This prints the non highlighted lines, 22 is the offset in the 2d 
			// table to the line we want to display
			drawText(&pts[x*22],defTab,x,true);	//string, column, line, inverted
			/*
			if(uState==50 && (x==1 || x==2))	
			{	//go in here if we are in the main menu, on the audio (line 1) or range (line 2) line
				if(x==1)	//we must be on the audio line
				{
					write_char(uAudio+0x10,true);	//write the highlighted audio value to 
						// the end of "AUDIO = "
				}
				if(x==2)
				{
					write_char(uRange+0x10,true);	//write the highlighted range value to
						// the end of "RANGE = "
				}
			}*/
		}
		else
		{	//print all the non highlighted menu items
			drawText(&pts[x*22],defTab,x,false);	//string, column, line, inverted
			/*
			if(uState==50 && (x==1 || x==2))
			{ //go in here if we are in the main menu and on the audio (line 1) or range (line 2) line
				if(x==1)	//we must be on the audio line
				{
					write_char(uAudio+0x10,false);	//write the non highlighted audio value to
						// the end of "AUDIO = "
				}
				if(x==2)	//we must be on the range line
				{
					write_char(uRange+0x10,false);	//write the non highlighted range value to
						// the end of "RANGE = "
				}
			}
			*/
		}
	}
	//The next line is for the soft button labels, no indention, on the bottom line
	//7 lines x 22 chars each line = 154, <-this number is the offset to the last 
	// line that will be printed
//	drawText(&pts[154],0,7,false);	//string, column, line, inverted
}

