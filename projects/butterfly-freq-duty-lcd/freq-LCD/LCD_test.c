// LCD_test.c
#include "LCD_driver.h"
#include "LCD_functions.h"
#include "LCD_test.h"
#include "PC_Comm.h"
#include "Messages.h"

// Start-up delay before scrolling a string over the LCD. "LCD_driver.c"
extern char gLCD_Start_Scroll_Timer;


//	PUTF,# 
//	Verify that # represents a valid string in flash.
//	Set the pFlashStr pointer to the string
//	Call LCD_puts_f(*pFlashStr, scrollmode);
//	Send "Called LCD_puts_f" to the PC.
void OnPUTF(char *PUTFstr)
{
	sendFString(PUTF_msg);
	
	PGM_P text;
	
	text = PSTR("LCD_put_f test\0"); // won't show the _

	LCD_puts_f(text, 1);
}

//	PUTS,string
//	Load the string and point pStr to it.
//	Call LCD_puts(*pStr, scrollmode); 
//	Send "Called LCD_puts with 'string'" to the PC where string is the string sent.
void OnPUTS(char *pStr)
{
	sendFString(PUTS_msg);
	sendString(pStr);
	
	LCD_puts(&pStr[4],0); //Overlook the PUTS part of the string

}


//	PUTC,digit,character
//	Call LCD_putc(digit, character);
//	Send "Called LCD_putc" to PC.
void OnPUTC(char *PUTCstr)
{
	uint8_t digit;

	sendFString(PUTC_msg);
	
	digit = (uint8_t)(PUTCstr[4] - 48);// convert to integer
	
	if(digit <= 6)
	{
		LCD_putc(digit,PUTCstr[5]);
		LCD_UpdateRequired(1,0);
	}
	
}

//	CLEAR
//	Call LCD_Clear();
//	Send "Called "LCD_Clear()" to the PC
void OnCLEAR(void)
{
	sendFString(CLEAR_msg);
	
	// Stop any scrolling
	gScrollMode = 0;        
    gScroll = 0;	

	LCD_Clear();
	
	LCD_UpdateRequired(1,0);
		
}

//	COLON,on/off 
//	Verify that on/off is either "ON" or "OFF"
//	If ON call LCD_Colon(1);
//	Else call LCD_Colon(0);
//	Send "Called LCD_Colon" to the PC.
void OnCOLON(char *pOnoff)
{
	sendFString(COLON_msg);
	
	if(pOnoff[5] == '1')
	{
		LCD_Colon(1);
	}
	else if (pOnoff[5] == '0')
	{
		LCD_Colon(0);	
	}

}

//	SETC
//	Call SetContrast(input);
//	Send "Called SetContrast to the PC.
// Note values are from 0 to 15
void OnSETC(char *SETCstr)
{
	char temp[] = {'\0','\0','\0'};
	int input;
	
	sendFString(SETC_msg);
		
	temp[0] = SETCstr[4];
	temp[1] = SETCstr[5];

	input = atoi(temp);
	
	SetContrast(input);

}

// SCROLL
// Start scroll if input == 1
// Stop scroll if input == 0
//	Send "Called OnScroll" to the PC.
void OnSCROLL(char *scroll)
{
	sendFString(SCROLL_msg);
	
	if(scroll[6] == '1')
	{
        gScrollMode = 1;        // Scroll if text is longer than display size
        gScroll = 0;
        gLCD_Start_Scroll_Timer = 3;    //Start-up delay before scrolling the text
	}
	else if (scroll[6] == '0')
	{
        gScrollMode = 0;        
        gScroll = 0;	
	}

}


