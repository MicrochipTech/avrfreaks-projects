/*
 /////////////////////////////////////////////////////////////////////////////////////////
 //																					    //
 //  _   _______   ___   ___  ___________ _____  ______ _           _             		//
 // | | | |  _  \ /   | /   ||___  /  _  |  _  | |  _  (_)         | |            		//
 // | |_| | | | |/ /| |/ /| |   / / \ V /| |/' | | | | |_ ___ _ __ | | __ _ _   _ 		//
 // |  _  | | | / /_| / /_| |  / /  / _ \|  /| | | | | | / __| '_ \| |/ _` | | | |		//
 // | | | | |/ /\___  \___  |./ /  | |_| \ |_/ / | |/ /| \__ \ |_) | | (_| | |_| |		//
 // \_| |_/___/     |_/   |_/\_/   \_____/\___/  |___/ |_|___/ .__/|_|\__,_|\__, |		//
 //                                                          | |             __/ |		//
 //                                                          |_|            |___/ 		//
 //                            ______      _           								    //
 //                            |  _  \    (_)          								    //
 //                            | | | |_ __ ___   _____ 								    //
 //                            | | | | '__| \ \ / / _ \								    //
 //                            | |/ /| |  | |\ V /  __/								    //
 //                            |___/ |_|  |_| \_/ \___|								    //
 //                                                    								    //
 /////////////////////////////////////////////////////////////////////////////////////////
--------------------------------- Main Program (tab 4) C++ ------------------------------

==========================================================================================
@AUTOR   : João D´Artagnan Antunes Oliveira
@DATA    : 13/12/2015
@HORÁRIO : 15:38
==========================================================================================
@PROPOSTA: Drive for HD44780 Chip Display Controller

@HARDWARE:
@OBSERVAÇÃO:
==========================================================================================
______                                      _
| ___ \                                    | |
| |_/ / __ ___   __ _ _ __ __ _ _ __ ___   | |     ___   __ _
|  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \  | |    / _ \ / _` |
| |  | | | (_) | (_| | | | (_| | | | | | | | |___| (_) | (_| |
\_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_| \_____/\___/ \__, |
                 __/ |                                   __/ |
                 |___/                                   |___/
==========================================================================================
DATA		HORA		DESCRIÇÃO
__________	_____		__________________________________________________________________________
2015/13/12	21:32	    Projeto do Software Iniciado.

*/

/*
+--------------------------------------+
|        AVR/GNU C++ COMPILER	       |
+--------------------------------------+

Symbols		    = -DF_CPU=8000000L -D_TIME_TIMER_NUMBER_=0
Optimization	= -Os
Miscellaneous	= -std=gnu++11
*/

/*
+--------------------------------------+
|        GLOBALS INCLUSION		       |
+--------------------------------------+
*/


#define _HD44780_VERSION    2
#define _HD44780_REVISION	0

/*
+--------------------------------------+
|        HARDWARE DEPENDENT INCLUDES   |
+--------------------------------------+
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
/*
+--------------------------------------+
|        C LIBRARY INCLUDES		       |
+--------------------------------------+
*/

#include <stdio.h>


/*
+--------------------------------------+
|        HARDWARE RESOURCES	           |
+--------------------------------------+
*/

/*
+--------------------------------------+
|        LIBRARY INCLUDES	           |
+--------------------------------------+
*/

#include "_Time_v125.h"
#include "_HD44780_Drive_V200.h"


/*
==================================================================================
=        ___  ___      _        ______                                           =
=        |  \/  |     (_)       | ___ \                                          =
=        | .  . | __ _ _ _ __   | |_/ / __ ___   __ _ _ __ __ _ _ __ ___         =
=        | |\/| |/ _` | | '_ \  |  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \        =
=        | |  | | (_| | | | | | | |  | | | (_) | (_| | | | (_| | | | | | |       =
=        \_|  |_/\__,_|_|_| |_| \_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_|       =
=                                                __/ |                           =
=                                               |___/                            =
==================================================================================
*/


const char newChar1_Box[8] PROGMEM ={0X00,0X1F,0X11,0X11,0X11,0X11,0X1F,0X00};	//into the flash define a new character 1 image, in this case a BOX
const char newChar2_Box[8] PROGMEM ={0X04,0X04,0X0A,0X0A,0X11,0X11,0X1F,0X00};	//into the flash define a new character 2 image, in this case a TRIANGLE
const char newChar3_Box[8] PROGMEM ={0X00,0X0E,0X11,0X11,0X11,0X0E,0X00,0X00};	//into the flash define a new character 3 image, in this case a ROUND BOX
const char newChar4_Box[8] PROGMEM ={0X00,0X0A,0X15,0X0E,0X04,0X00,0X11,0X0E};	//into the flash define a new character 4 image, in this case a FACE


int main()
{

	//
	// DEMO Below demonstrating use o this Drive
	//

	
	
	class_Time				time;										//Initialize Timer Class
	class_HD44780_Drive		lcd(_lcd_INTERFACE::SIZE_4_BITS,2,16);		//Initialize LCD with 2 row 16 columns interface of 4 bits

	char buf[20];								//sprintf buffer	
	int  count=1;								//Demos Count

	#define		_DELAY_BETWEEN_DEMOS	2000
	//ENABLE GLOBAL INTERRUPTS

	sei();

	//MAIN LOOP

	//------------------------------------------------------------------------------------------------------
	//check if LCD is present

	if (lcd.Present())
	{
		//YES, show message
		lcd.SetCursorState(false);
		lcd.PutSF(PSTR("LCD Present?Yes"));		//Show present
	}
	else
	{
		//NO, put here a code if lcd not present like blink a LED etc,
		#define _LED_WARNING_ 0

		DDRB|=(1<<_LED_WARNING_);

		for(;;)
		{
			PORTB^=(1<<_LED_WARNING_);			//Blink LED
			time.DelayMillisByInterrupt(250);	//0.5Hz LED Blink
		}
	}
	time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//OFF by one(1) second

	//==============================================================================
	//==========                   M A I N   L O O P                      ==========
	//==============================================================================
	for(;;)
	{
		//------------------------------------------------------------------------------------------------------
		// SHOW DEMO LOOP COUNTER
		lcd.Cls();												//Clear Screen
					  //1234567890123456
		lcd.PutSF(PSTR("DEMO LOOP COUNT "));					//Show Version print characters into FLASH
		lcd.Locate(2,1);
		sprintf(buf,   "     [%4d]     ",count++);
		lcd.PutS(buf);											//Print characters from SRAM
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//Wait One(1) Second
		//------------------------------------------------------------------------------------------------------
		// SHOW DRIVE VERSION
		lcd.Cls();												//Clear Screen
		lcd.PutSF(PSTR("HD4470"));								//Show Version print characters into FLASH
		lcd.Locate(2,1);
		sprintf(buf,"Version %d.%02d",lcd.GetVersionMajor(),lcd.GetVersionMinor());
		lcd.PutS(buf);											//Print characters from SRAM	
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//Wait One(1) Second
		//------------------------------------------------------------------------------------------------------
		// PUTC PUTS PUTSF ALLOWING CR(CARRIAGE RETURN) AND LF(LINE FEED)
		lcd.Cls();												//Clear Screen
		//123456789123456
		lcd.PutSF(PSTR("PutC PutS PutSF\r\nAllow CR & LF"));	//Show PutC PutS PutSF characters into FLASH
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//Wait One(1) Second
		//------------------------------------------------------------------------------------------------------
		// SCROLL LEFT
		lcd.Cls();												//Clear Screen
		lcd.Locate(1,6);										//Locate cursor row 1 column
		lcd.PutSF(PSTR("Scroll Left"));							//Write Scroll Left characters into Flash
		for(uint8_t i=0;i<5;i++)								//Scroll LEFT 5 times
		{
			lcd.ScrollLeft();
			time.DelayMillisByInterrupt(500);
		}
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);
		//------------------------------------------------------------------------------------------------------
		// SCROLL RIGHT
		lcd.Cls();												//Clear Screen
		lcd.PutSF(PSTR("Scroll Right"));						//Write Scroll Right characters into Flash
		for(uint8_t i=0;i<5;i++)								//Scroll LEFT 5 times
		{
			lcd.ScrollRight();
			time.DelayMillisByInterrupt(500);
		}
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);
		//------------------------------------------------------------------------------------------------------
		// SET CURSOR OFF
		lcd.Cls();												//Clear Screen
		lcd.PutSF(PSTR("Cursor Off"));							//Show Cursor Off characters into flash
		lcd.SetCursorState(false);
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//OFF by one(2) second
		//------------------------------------------------------------------------------------------------------
		//SET CURSOR ON	
		lcd.Cls();												//Clear Screen
		lcd.PutSF(PSTR("Cursor On "));							//Show Cursor On characters into flash
		lcd.SetCursorState(true);
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//ON by one(2) second
		//------------------------------------------------------------------------------------------------------
		// SET BLINK ON
		lcd.Cls();												//Clear Screen
		lcd.PutSF(PSTR("Set Blink On"));						//Show Blink On characters into flash
		lcd.SetCursorBlinkState(true);
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//ON by one(2) second
		//------------------------------------------------------------------------------------------------------
		// SET BLINK OFF
		lcd.Cls();												//Clear Screen
		lcd.PutSF(PSTR("Set Blink Off"));						//Show Blink Off characters into flash
		lcd.SetCursorBlinkState(false);
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//ON by one(2) second
		//------------------------------------------------------------------------------------------------------
		//SET CURSOR OFF
		lcd.Cls();												//Clear Screen
		lcd.PutSF(PSTR("Cursor Off Again"));					//Show Cursor Off Again characters into flash
		lcd.SetCursorState(false);
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//OFF by one(2) second
		//------------------------------------------------------------------------------------------------------
		// SAVE SCREEN
		lcd.Cls();												//Clear Screen
		              //1234567890123456
		lcd.PutSF(PSTR("Next Msg. Will  "));					//Show Save This Screen characters into flash
		lcd.Locate(2,1);
		lcd.PutSF(PSTR("save Into SRAM  "));					//Show Into Sram characters into flash
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//OFF by one(2) second
		lcd.Cls();
		lcd.Locate(1,1);
	    lcd.PutSF(PSTR("0123456789012345"));
		lcd.Locate(2,1);
		lcd.PutSF(PSTR("0123456789012345"));
		lcd.ScrSave();											//Screen Saved
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		//OFF by one(2) second
		//------------------------------------------------------------------------------------------------------
		// RESTORE SCREEN
		lcd.Cls();
		lcd.PutSF(PSTR("Screen Restore"));
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		
		lcd.ScrRestore();										//Restore Screen
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);
		//------------------------------------------------------------------------------------------------------
		// BIG NUMBERS
		lcd.PatternSave();										//Save Screen Pattern area
		lcd.Cls();												//Clear Screen
		lcd.LCDBigInit();										//Initialize Big Numbers
		for(uint16_t i=0;i<1000;i++)							//Count 0 to 9999
		{
			lcd.Locate(1,1);									//Locate
			lcd.PutSF(PSTR("Big Number="));
			sprintf(buf,"%03d",i);								//format
			lcd.PutSBig(buf);									//print PigNumber
			time.DelayMillisByInterrupt(10);					//10ms
		}
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);		
		lcd.PatternRestore();									//Restore old patterns
		//------------------------------------------------------------------------------------------------------
		// MOVE CURSOR RIGHT
		lcd.Cls();												//Clear Screen
		lcd.SetCursorState(true);								//Enable cursor
		lcd.PutSF(PSTR("CURSOR RIGHT"));						//Show Cursor Right character into flash
		lcd.Locate(1,1);										//Set cursor at row 1 column 1
		for(uint8_t i=0;i<12;i++)
		{
			lcd.MoveCursorToRight();							//move cursor
			time.DelayMillisByInterrupt(250);					//250ms at cursor move
		}
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);
		//------------------------------------------------------------------------------------------------------
		// MOVE CURSOR LEFT
		lcd.Cls();												//Clear Screen
		lcd.SetCursorState(true);								//Enable cursor
		lcd.PutSF(PSTR("CURSOR LEFT  "));						//Show Cursor Right character into flash
		for(uint8_t i=0;i<13;i++)
		{
			lcd.MoveCursorToLeft();								//move cursor
			time.DelayMillisByInterrupt(250);					//250ms at cursor move
		}
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);
		//------------------------------------------------------------------------------------------------------
		// WRITE DIRECTLY TO CHARACTERS RAM
		lcd.Cls();												//Clear Screen
		lcd.Locate(2,1);
					  //1234567890123456
		lcd.PutSF(PSTR("WRITE DIRECTLY  "));					//Show Cursor Right character into flash
		for(uint8_t i=0;i<lcd.GetCols();i++)
		{
			lcd.SetDDRAMAddr(i);								//Set directly ram address
			lcd.DataWrite(i+'A');								//Write directly into ram chars 'A,B,C,D....'
			time.DelayMillisByInterrupt(250);					//250ms between writes to visible show A,B,C,D.....characters at screen slowly
		}
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);
		//------------------------------------------------------------------------------------------------------
		// REDEFINE CHARACTER
		lcd.Cls();												//Clear Screen
		lcd.Locate(2,1);
					  //1234567890123456
		lcd.PutSF(PSTR("REDEFINED CHARS."));					//Show Cursor Right character into flash
		
	
		lcd.RedefineChar(1,newChar1_Box);						//Redefine character ASCII 1 to a box
		lcd.RedefineChar(2,newChar2_Box);						//Redefine character ASCII 2 to a triangle
		lcd.RedefineChar(3,newChar3_Box);						//Redefine character ASCII 4 to a round box
		lcd.RedefineChar(4,newChar4_Box);						//Redefine character ASCII 5 to a face
		lcd.Locate(1,1);										//Locate at 1,1						
		lcd.PutC(0x01);lcd.PutC(0x01);lcd.PutC(' ');			//Draw 2 boxex
		lcd.PutC(0x02);lcd.PutC(0x02);lcd.PutC(' ');			//Draw 2 triangle2
		lcd.PutC(0x03);lcd.PutC(0x03);lcd.PutC(' ');			//Draw 2 round boxes
		lcd.PutC(0x04);lcd.PutC(0x04);lcd.PutC(' ');			//Draw 2 faces
		time.DelayMillisByInterrupt(_DELAY_BETWEEN_DEMOS);
	}
}
