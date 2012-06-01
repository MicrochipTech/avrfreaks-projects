/***********************************************************
/
/	MAIN.C
/	
/	Author:  Visakhan C
/	
/	Version 3
/	29 May, 2012
/	
/	Changes from v2
/
/	1. Combined Savefile() and Save() into single Save() function
/
/
/***********************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "lcd.h"
#include "ps2_kb.h"
#include "ext_keys.h"
#include "common.h"
#include "menu_utils.h"


#define R_Max 39
#define L_Min 0
#define R_Init 15
#define L_Init L_Min

						 
extern void WritePage (uint16_t page, const uint8_t *buf);

static void avr_init(void);
static void UpdateWindow(void);
static void Editor(uint8_t fileId);
static void New(void);
static void Open(void);
static void Edit(void);
static ERRORCODE Save(uint16_t* pFileId);
static void DisplayBuffer(void);
static ERRORCODE LoadFile(uint8_t fileId);


static char buffer[81];		//buffer to hold the text
static uint8_t WinR,WinL;	//current window borders, left anf right


const char Title[] PROGMEM = "TYPE IT!"; 
const char queSave[] PROGMEM = "Save the File?";
const char queFileId[] PROGMEM = "Enter fileID:";
const char queExit[] PROGMEM = "Exit Editor?";
const char infoFileId[] PROGMEM = "Invalid ID";

MenuItem MainMenu[] = {

	{ "NEW", New},
	{ "OPEN", Open},
	{ "EDIT" , Edit}

};


/********************** MAIN ***********************/

int main(void)
{
	_delay_ms(100);    //some initial delay
	avr_init();
	
    while(1)
	{		
		DoMenu((PGM_P)Title,(MenuItem*)&MainMenu); 

	}
    return(0);
}



/* initialise lcd and keyboard, enable interrupts */
static void avr_init(void)
{
    lcdInit();
	KbInit();
	sei(); //enable global interrupts
    return;
}

/* resets window borders to initial values */
static void ResetWindow(void) 
{
	WinR = R_Init;
	WinL = L_Init;
}

/* Update window borders according to LCD cursor changes */
static void UpdateWindow(void)
{
	uint8_t Y = lcdGetY();
	
	if( Y > WinR) {
	
		if( (WinL == L_Min) && (Y == R_Max) ) {
			
			WinR = R_Max;
			WinL = R_Max-15;
			lcdShiftLeft(24);
		}
		else if(WinR++ < R_Max) {
			WinL++;
			lcdShiftLeft(1);
		}
		
	}
	else if( Y < WinL) {
	
		if ( (WinR > R_Init) && (Y == L_Min) ) {  //Also handles explicit 
													//Return key
			lcdShiftRight(WinR-R_Init);
			ResetWindow();
		}
		else if(WinL-- > L_Min) {
		
			WinR--;
			lcdShiftRight(1);
		}
	}
}

/* Writes the whole characters in buffer to LCD */ 
static void DisplayBuffer(void)
{
	uint8_t index;
	lcdClear();
	for(index=0;index<80;index++) {		
			lcdChar(buffer[index]);
	}
	lcdHome();
	ResetWindow();

}

/* Callback function for option NEW */
static void New(void)
{
	Editor(0);
}

/* Callback function for option OPEN */
static void Open(void)
{
	
	uint16_t fileId;
	ERRORCODE error;
	uint8_t c, shift=0;
	
	error = PromptNumber( (PGM_P)queFileId, &fileId);  //file id to be opened 
	
	if(error == SUCCESS && (LoadFile( (uint8_t)fileId) == SUCCESS) ) {
		
		DisplayBuffer();
		lcdHome();
		lcdCursorOff();
		
		do {					//handles arrow keys
		
			c = KbGetChar();
			
			switch(c) {
			
				case L_ARROW	:	if(shift--)
										lcdShiftRight(1); 
									else {
									
										shift = 24;
										lcdShiftLeft(shift);
									}
									break;
				
				case R_ARROW	:	if(shift++ < 24)
										lcdShiftLeft(1); 
									else {
									
										shift = 0;
										lcdShiftRight(24);
									}
									break;
			}
		
		} while(c != ESC);
		
	} 
	else {		// invalid file id given
		
		lcdClear();
		lcdString_P(infoFileId);
		_delay_ms(1000);
		
	}
	
}

/* Saves the text buffer to a flash page, with a file ID 
   returns an Error code */ 
static ERRORCODE Save(uint16_t* pFileId)
{

	uint16_t  pageAddr;
	uint8_t id;
	ERRORCODE error;
	
	if(*pFileId == 0)	
	{
	
		do {   // until a file id is entered or ESC is pressed   
			
			error = PromptNumber( (PGM_P)queFileId, pFileId);
		
		} while((error != ERR_ABORT) && (error != SUCCESS));
		
		if(error == ERR_ABORT)  //ESC key
			return error;
	
	}
	
	id = (uint8_t)(*pFileId);	
	if(id == 0 || id > MAX_FILEID) {   //if not valid file Id

		lcdClear();						//error message
		lcdString_P(infoFileId);
		_delay_ms(1000);
		return ERR_FILEID;
	}
	
	pageAddr = START_PAGE_ADDR + id*SPM_PAGESIZE;  //calculate page address
	WritePage(pageAddr,(const uint8_t*)buffer);	   // write buffer to page
	
	lcdClear();
	lcdString("File Saved:");		//info message
	lcdNumberSmall(id);
	_delay_ms(1000);
	
	return SUCCESS;
}
	
/* loads the text buffer from a page in flash, corresponding to a file Id */
static ERRORCODE LoadFile(uint8_t fileId)
{
	uint16_t pageAddr;
	uint8_t index=2*40;		//index to buffer
	
	if(fileId == 0 || fileId > MAX_FILEID)    // if not valid file id
		return ERR_FILEID;
		
	pageAddr = START_PAGE_ADDR + fileId*SPM_PAGESIZE;   //find page address
	memcpy_P((uint8_t*)buffer, (PGM_P)pageAddr, 2*40 ); // copy from falsh page to buffer
	
	do {							//changes 0xff to spaces in the buffer
		
		if(buffer[index] == 0xFF)
			buffer[index] = ' ';
			
	}while(index--);
	
	return SUCCESS;

}



static void Edit(void)
{
	uint16_t fileId;
	ERRORCODE error;
	
	error = PromptNumber( (PGM_P)queFileId, &fileId);  //ask for file id
	
	if(error == SUCCESS && (LoadFile( (uint8_t)fileId) == SUCCESS) ) {
	
		Editor( (uint8_t)fileId);  //open the file in editor
		
	} 
	else {				//invalid file id
		
		lcdClear();			
		lcdString_P(infoFileId);
		_delay_ms(1000);
		
	}

}

static void Editor(uint8_t id)
{
	
	uint8_t c,index=80,file_changed=FALSE;
	RESPONSE qSave,exit=NO;
	ERRORCODE error;
	uint16_t fileId = id;
	
	do {						//space initialise the buffer
	
		buffer[index] = ' ';
	
	} while(index--);
	
	if(fileId) 						//if not a new file
		LoadFile((uint8_t)fileId);	//load that file from flash

	DisplayBuffer();
		
	lcdCursorBlink();
	ResetWindow();
	
	while(1) {
	
		c = KbGetChar(); //get a character from keyboard 
		index = 40*lcdGetX()+lcdGetY();
		
		switch(c) {		//handles various keys
			
				case '\r'		:   lcdGotoXY(1,0); //Enter key
									break;  
							
				case  BACKSPACE	: 	lcdCursorLeft(1);	
									lcdChar(' ');
									if(index)
										buffer[index-1] = ' ';
									lcdCursorLeft(1); 
								
									break;
									
				case ESC		:	if(file_changed == FALSE) {
				
										exit = YES;  
										break;
									}
									
									qSave = PromptYesNo( (PGM_P)queSave);
									
									if(qSave == YES) {
										
										if( Save(&fileId) == SUCCESS)
											file_changed = FALSE;
										
										exit = PromptYesNo( (PGM_P)queExit);
										DisplayBuffer();
										
									}
									
									else if(qSave == NO)
										exit = YES;
										
									else
										DisplayBuffer();
										
									break;
									
				case TAB		:	if (Save(&fileId) == SUCCESS)
										file_changed = FALSE;
									DisplayBuffer();
									
									break;	
									
				case L_ARROW	:   lcdCursorLeft(1);
									break; 
				case R_ARROW	:   lcdCursorRight(1);
									break;
									
				case D_ARROW	:	lcdGotoXY( 1,lcdGetY() ); break;
				case U_ARROW	:	lcdGotoXY( 0,lcdGetY() ); break;
				
				case HOME		:	lcdHome();
									ResetWindow();
									break;
				
				default			:	if(c < 0x80) {   //check if printable ASCII character
										
										file_changed = TRUE;
										buffer[index] = c;
										lcdChar(c);
																		
									}

			}
			
			UpdateWindow();
			if(exit == YES) break;
			
	}

}

