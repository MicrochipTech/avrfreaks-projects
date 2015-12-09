/*
  ////////////////////////////////////////////////////////////////////////////////////////
  //																					//
  //  _   _______   ___   ___  ___________ _____  ______ _           _             		//
  // | | | |  _  \ /   | /   ||___  /  _  |  _  | |  _  (_)         | |            		//
  // | |_| | | | |/ /| |/ /| |   / / \ V /| |/' | | | | |_ ___ _ __ | | __ _ _   _ 		//
  // |  _  | | | / /_| / /_| |  / /  / _ \|  /| | | | | | / __| '_ \| |/ _` | | | |		//
  // | | | | |/ /\___  \___  |./ /  | |_| \ |_/ / | |/ /| \__ \ |_) | | (_| | |_| |		//
  // \_| |_/___/     |_/   |_/\_/   \_____/\___/  |___/ |_|___/ .__/|_|\__,_|\__, |		//
  //                                                          | |             __/ |		//
  //                                                          |_|            |___/ 		//
  //                            ______      _           								//
  //                            |  _  \    (_)          								//
  //                            | | | |_ __ ___   _____ 								//
  //                            | | | | '__| \ \ / / _ \								//
  //                            | |/ /| |  | |\ V /  __/								//
  //                            |___/ |_|  |_| \_/ \___|								//
  //                                                    								//
  ////////////////////////////////////////////////////////////////////////////////////////
  // This Drive running on ATMEGA8,ATMEGA162,ATMEGA128 and atmega328p devices           //
  // Global interrupts must enabled to work                                             //
  --------------------------- HD44780 C++ Header File (tab 4) ----------------------------
*/


#ifndef __H44780_DISPLAY_DRIVE_H__
#define __H44780_DISPLAY_DRIVE_H__

/*
 +--------------------------------------+
 |        HARDWARE DEPENDENT INCLUDES	|
 +--------------------------------------+
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
/*
 +--------------------------------------+
 |        LIBRARY INCLUDES	            |
 +--------------------------------------+
*/
#include <stdlib.h>
/*
 +--------------------------------------+
 |        LOCAL LIBRARY INCLUDES	    |
 +--------------------------------------+
*/

#include "_Time0_Lib.h"

/*
 +--------------------------------------+
 |    INTERFACE PORTS CONFIGURATION		|
 +--------------------------------------+
*/


	 
       	// DATA PORT SET (data port configuration)

    	#define	_lcd_DATA_OUTPUT		PORTD
    	#define	_lcd_DATA_DIR		DDRD
    	#define	_lcd_DATA_INPUT		PIND

    	// LCD DATA BIT IS USED ONLY FOR 4 BITS INTERFACE ELSE START BIT ALWAYS 0

    	#define _lcd_DATA_BIT		4

    	// RS PORT SET (RS Pin Configuration)

    	#define	_lcd_RS_OUTPUT		PORTD
    	#define	_lcd_RS_DIR			DDRD
    	#define	_lcd_RS_INPUT		PIND
    	    
    	#define	_lcd_RS_BIT			0

    	// RW PORT SET (RW Pin Configuration)

    	#define	_lcd_RW_OUTPUT		PORTD
    	#define	_lcd_RW_DIR			DDRD
    	#define	_lcd_RW_INPUT		PIND
    	    
    	#define	_lcd_RW_BIT			1

    	// E PORT SET (E Pin Configuration)

    	#define	_lcd_E_OUTPUT		PORTD
    	#define	_lcd_E_DIR			DDRD
    	#define	_lcd_E_INPUT			PIND

    	#define	_lcd_E_BIT			2

/*
 +--------------------------------------+
 |    INTERFACE ENUMERATES				|
 +--------------------------------------+
*/

/*
	-----------------------------------------------------------------------------------------------------
	; BITS INFORMATION AND POSITION OF INTERNAL REGISTERS COMMAND OF HD44780 LCD CONTROLLER
	;  7   6   5   4   3   2   1   0
	;| 0 | 0 | 0 | 0 | 0 | 0 | 0 | 1 | Clear display and sets DDRAM address 0 in address counter
	;| 0 | 0 | 0 | 0 | 0 | 0 | 1 | - | Set DDRAM address 0 in address counter
	;| 0 | 0 | 0 | 0 | 0 | 1 |I/D| S | I/D=cursor INCREMENT/cursor DECREMENT S=display SHIFT
	;| 0 | 0 | 0 | 0 | 1 | D | C | B | D  =DISPLAY state C=CURSOR state B=BLINK state
	;| 0 | 0 | 0 | 1 |S/C|R/L| - | - | S/C=SCREEN shift/CURSOR shift R=RIGHT L=LEFT
	;| 0 | 0 | 1 | DL| N | F | - | - | DL =DATA LENGTH N=display LINES F=character FONT
	;| 0 | 1 |AGC|AGC|AGC|AGC|AGC|AGC| AGC=CARACTER GENERATOR RAM ADDRESS
	;| 1 |ADD|ADD|ADD|ADD|ADD|ADD|ADD| ADD=DATA RAM ADDRESS
	------------------------------------------------------------------------------------------------------
*/

////////////////////////////
/// LCD COMMANDS
////////////////////////////
typedef enum class enum_lcd_CMD
{
//COMMAND CLEAR ENTIRE DISPLAY AREA
	CLEAR				=0x01,
//COMMAND RETURN DISPLAY CURSOR DO LINE 1 COLUMN 1
	HOME		 		=0x02,
//COMMAND DISPLAY MODE SET
	ENTRY_MODE_SET	 	=0x04,
//COMMAND DISPLAY ON/OFF
	ON_OFF		 		=0x08,
//COMMAND DISPLAY SHIFT
	SHIFT		 		=0x10,
//COMMAND FUNCTION SET
	FUNCTION_SET	 	=0x20,
//COMMAND SET CGRAM ADDRESS CG=CHARACTER GENERATOR
	CGRAM_ADDR	 		=0x40,
//COMMAND SET CGRAM ADDRESS CG=CHARACTER GENERATOR
	DDRAM_ADDR	 		=0x80
} _lcd_CMD;

////////////////////////////
/// LCD BIT CONTROL
////////////////////////////

/*
  +---------------------------------------------+
  |	BIT DEFINITIONS CONTROL						|
  +---------------------------------------------+
  | ID=1 cursor increment ID=0 cursor decrement	|
  | S=1 display shift							|
  | D=1 display ON								|
  | C=1 cursor ON								|
  | B=1 cursor blinking							|
  | SC=1 display shift SC=0 cursor move			|
  | RL=1 shift right RL=0 shift left			|
  | DL=1 8 bits DL=0 4 bits interface			|
  | N=1 2 LINES N=0 1 LINE						|
  | F=1 5X10 DOTS F=0 5X8 DOTS					|
  +---------------------------------------------+
*/
enum _lcd_BIT:uint8_t
{
	ID 		=1,
	S  	 	=0,
	D		=2,
	C	 	=1,
	B	 	=0,
	SC	 	=3,
	RL	 	=2,
	DL	 	=4,
	N	 	=3,
	F	 	=2,
	BUSY	=7	
};
////////////////////////////
/// INTERFACE CONTROL
////////////////////////////
typedef enum class enum_lcd_INTERFACE
{
	SIZE_4_BITS		=4,
	SIZE_8_BITS		=8,
	CMD_WRITE		=0,
	CMD_READ		=1,
	DATA_WRITE 		=2,
	DATA_READ		=3,
	DATA_MASK		=(0x0f << _lcd_DATA_BIT)
} _lcd_INTERFACE;

/*
        +-------------------------------------------------------+
        |        CLASS HD44780 Drive                            |
        +-------------------------------------------------------+
        | @PURPOSE      : Control HD44780 chip                  |
        | @OBSERVATIONS :                                       |
        +-------------------------------------------------------+
*/

class class_HD44780_Drive
{
       /*
         +------------------------------+
         |       PRIVATE VARIABLES		|
         +------------------------------+
        */
private:
        uint8_t				Row,Rows;
        uint8_t				Col,Cols;
        _lcd_INTERFACE		InterfaceSize=_lcd_INTERFACE::SIZE_4_BITS;
        uint8_t				REG2,REG3;			//Temporary registers
        bool				BusyUse=true;		//true check busy flash false not check
        bool				PresentFlag=false;	//true Present false not present
        uint16_t			BusyDelay=2;		//2ms default busy delay
        class_Time			_Time;
        void				*ScrSavePtr;        //to Save/Restore Screen Data
        void				*PatternSavePtr;    //to Save/Restore Pattern Data
        /*
         +------------------------------+
         |       PRIVATE FUNCTIONS      |
         +------------------------------+
        */
        bool    		PresenceCheck(_lcd_INTERFACE lcdInterfaceType);
        void			Interface(_lcd_INTERFACE lcdControlState,uint8_t *data);
        void			InterfaceInitialize(_lcd_INTERFACE lcdInterfaceType,uint8_t numberOfRows,uint8_t numberOfCols);
        void			InterfaceUnInitialize(void);
        /*
         +------------------------------+
         |        PUBLIC FUNCTIONS		|
         +------------------------------+
        */
public:
        //Constructor
        class_HD44780_Drive(void);
		class_HD44780_Drive(_lcd_INTERFACE lcdInterfaceType,uint8_t numberOfRows,uint8_t numberOfCols);
        //Destructor
        ~class_HD44780_Drive(void);
        //Remainder functions
        void            PrepareForSleep(void);
        void            RestoreFromSleep(void);
        void			SetBusyUse(uint8_t type);
        void			SetBusyDelay(unsigned int time);
        uint8_t      	CmdRead(void);
        bool      	    Busy(void);
        void 			CmdWrite(uint8_t data);
        void 			DataWrite(uint8_t data);
        uint8_t  	    DataRead(void);
        void 			Cls(void);
        void 			Home(void);
        void 			ScrollLeft(void);
        void 			ScrollRight(void);
        void 			SetCursorState(uint8_t state);
        void 			MoveCursorToLeft(void);
        void 			MoveCursorToRight(void);
        void 			SetCursorBlinkState(uint8_t state);
        void 			Locate(uint8_t row,uint8_t col);
        void			PutC(char character);
        void			PutS(char *string);
        void			PutSF(const char *string);
        void			SetCGRAMAddr(uint8_t addr);
        void			SetDDRAMAddr(uint8_t addr);
        void			RedefineChar(uint8_t character,const char *fString);
        bool          	Present(void);
        void            LCDBigInit();
        void            PutCBig(char asciiDigit);
        void            PutSBig(char *string);
        void            ScrSave(void);
        void            ScrRestore(void);            
        void            PatternSave(void);
        void            PatternRestore(void);
};        

#endif  // _F8_WIFI_HD44780_DRIVE_H_