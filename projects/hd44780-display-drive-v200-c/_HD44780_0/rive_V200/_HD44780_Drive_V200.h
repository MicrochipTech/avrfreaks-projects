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


/*
 +--------------------------------------+
 |    INTERFACE PORTS CONFIGURATION		|
 +--------------------------------------+
*/
	 
       	// DATA PORT SET (data port configuration)

    	#define	_lcd_DATA_OUTPUT	PORTD
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
    	#define	_lcd_E_INPUT		PIND

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

//////////////////////////
// VERSION CONTROL
//////////////////////////

const uint8_t _HD44780_VERSION_MAJOR = 2;
const uint8_t _HD44780_VERSION_MINOR = 0;

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
        uint8_t				Row,Rows;			//Row and Rows
        uint8_t				Col,Cols;			//Col and Cols
        _lcd_INTERFACE		InterfaceSize=_lcd_INTERFACE::SIZE_4_BITS;
        uint8_t				REG2,REG3;			//Temporary registers
        bool				BusyUse=true;		//true check busy flash false not check
        bool				PresentFlag=false;	//true Present false not present
        uint16_t			BusyDelay=2;		//2ms default busy delay
        void				*ScrSavePtr;        //to Save/Restore Screen Data
        void				*PatternSavePtr;    //to Save/Restore Pattern Data
        /*
         +------------------------------+
         |       PRIVATE FUNCTIONS      |
         +------------------------------+
        */

		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Check if LCD is present											|
		//	| @NAME    :  lcdPresenceCheck													|
		//	| @INPUT   :  lcdIntefaceType lcd_SIZE_4_BITS or lcd_SIZE_8_BITS				|
		//	| @OUTPUT  :  true present, false not present                                   |
		//	| @REMARKS :  this function must be used preferred before lcdInit				|
		//	+-------------------------------------------------------------------------------+
		bool	PresenceCheck(_lcd_INTERFACE lcdInterfaceType);		
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Send or read data from hd44780 interface							|
		//	| @NAME    :  lcdInterface														|
		//	| @INPUT   :  Interface type, data to be write									|
		//	| @OUTPUT  :  ----																|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	Interface(_lcd_INTERFACE lcdControlState,uint8_t *data);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Display Interface Initialization									|
		//	| @NAME    :  InterfaceInitialize											    |
		//	| @INPUT   : 																	|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  Enable And program all used pin of Display						|
		//	+-------------------------------------------------------------------------------+
		void	InterfaceInitialize(_lcd_INTERFACE lcdInterfaceType,uint8_t numberOfRows,uint8_t numberOfCols);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Display Interface Undo Initialization								|
		//	| @NAME    :  InterfaceUnInitialize											    |
		//	| @INPUT   : 																	|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  Disable all pins used by the Display Interface					|
		//	+-------------------------------------------------------------------------------+
		void	InterfaceUnInitialize();
        /*
         +------------------------------+
         |        PUBLIC FUNCTIONS		|
         +------------------------------+
        */
public:
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  UnInitialize HD44780 DisplayDrive									|
		//	| @NAME    :  Destructor														|
		//	| @INPUT   :                                                            		|
		//	| @OUTPUT  :                                                                    |
		//	|	                                                                            |
		//	| @REMARKS :  Release memory and set pointer null                               |
		//	+-------------------------------------------------------------------------------+
		~class_HD44780_Drive();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Initialize HD44780 DisplayDrive									|
		//	| @NAME    :  Constructor														|
		//	| @INPUT   :  lcdInterface lcd_SIZE_4_BITS or lcd_SIZE_8_BITS					|
		//  |             numberOfRows, numnberOfCols                                       |
		//	| @OUTPUT  :                                                                    |
		//	| @REMARKS :  interrupts are disabled during initialization then restored		|
		//	+-------------------------------------------------------------------------------+
		class_HD44780_Drive(_lcd_INTERFACE lcdInterfaceType,uint8_t numberOfRows,uint8_t numberOfCols);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  This constructor not initialize display drive, use constructor    | 
		//  |             overload after to correctly initialize display with rows and cols |
		//	| @NAME    :  Constructor    													|
		//	| @INPUT   :																	|
		//	| @OUTPUT  :																	|
		//	| @REMARKS :  interrupts are disabled during initialization then restored		|
		//	+-------------------------------------------------------------------------------+
		class_HD44780_Drive();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Prepare Display for enter in sleep	                            |
		//	| @NAME    :  PrepareForSleep                    	                            |
		//	| @INPUT   :  none                                                              |
		//	| @OUTPUT  :  none								                                |
		//	| @REMARKS :                                              		                |
		//	+-------------------------------------------------------------------------------+
		void    PrepareForSleep();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Restore Display from sleep state				                    |
		//	| @NAME    :  RestoreFromSleep                    	                            |
		//	| @INPUT   :  none                                                              |
		//	| @OUTPUT  :  none								                                |
		//	| @REMARKS :                                              		                |
		//	+-------------------------------------------------------------------------------+
		void    RestoreFromSleep();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Set interface to use or not busy read state						|
		//	| @NAME    :  lcdSetBusyType													|
		//	| @INPUT   :  true check busy false not check busy								|
		//	| @OUTPUT  :  ----																|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	inline SetBusyUse(bool busyType)
		{
			BusyUse=busyType;
		}
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Set interface busy delay when not used								|
		//	| NAME    :  lcdSetBusyDelay													|
		//	| INPUT   :  time in milliseconds												|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	inline SetBusyDelay(unsigned int time)
		{
			BusyDelay=time;
		}
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Read Display Status												|
		//	| @NAME    :  lcdCmdRead														|
		//	| @INPUT   :  																	|
		//	| @OUTPUT  :  Address read bits 0..6 and busy state bit 7						|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		uint8_t CmdRead();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  check if display is busy											|
		//	| @NAME    :  lcdBusy															|
		//	| @INPUT   :  																	|
		//	| @OUTPUT  :  return true if busy												|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		bool Busy();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Write a command into display										|
		//	| @NAME    :  lcdCmdWrite														|
		//	| @INPUT   :  data to be write													|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void CmdWrite(uint8_t data);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Write a data on display											|
		//	| @NAME    :  lcdDataWrite														|
		//	| @INPUT   :  data to be write													|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void DataWrite(uint8_t data);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Read a data from display											|
		//	| @NAME    :  lcdDataRead														|
		//	| @INPUT   :  																	|
		//	| @OUTPUT  :  data																|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		uint8_t DataRead();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Clear Display Screen												|
		//	| @NAME    :  lcdCLS															|
		//	| @INPUT   : 																	|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  Set Display Row to 1 and Col to 1									|
		//	+-------------------------------------------------------------------------------+
		void Cls();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Display Home, Set col=1											|
		//	| @NAME    :  lcdHome															|
		//	| @INPUT   : 																	|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  Set Display Col to 1												|
		//	+-------------------------------------------------------------------------------+
		void Home();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Scroll all characters in display to left							|
		//	| @NAME    :  lcdScrollLeft														|
		//	| @INPUT   : 																	|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void ScrollLeft();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Scroll all characters in display to right							|
		//	| @NAME    :  lcdScrollRight													|
		//	| @INPUT   : 																	|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void ScrollRight();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Set Cursor Visibility state										|
		//	| @NAME    :  lcdSetCursorState													|
		//	| @INPUT   :  state=false Invisible else Visible								|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void SetCursorState(bool cursorState);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Move Cursor to left												|
		//	| @NAME    :  lcdMoveCursorToLeft												|
		//	| @INPUT   :  																	|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void MoveCursorToLeft();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Move Cursor to Right												|
		//	| @NAME    :  lcdMoveCursorToRight												|
		//	| @INPUT   :  																	|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void MoveCursorToRight();
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Move Cursor to Right												|
		//	| @NAME    :  lcdMoveCursorToRight												|
		//	| @INPUT   :  blinkState=true Blink blinkState=false NotBlink					|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void SetCursorBlinkState(bool blinkState);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Locate display at position of row and col							|
		//	| @NAME    :  lcdLocate															|
		//	| @INPUT   :  Row & Col Row=1..NumberRows, Col=1 to NumberCols					|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void Locate(uint8_t row,uint8_t col);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Send char to display sram											|
		//	| @NAME    :  lcdPut															|
		//	| @INPUT   :  character character to be displayed								|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	PutC(char character);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Display string locate into sram									|
		//	| @NAME    :  lcdPuts															|
		//	| @INPUT   :  *msg string into sram												|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	PutS(char *string);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Display string locate into FLASH									|
		//	| @NAME    :  lcdPutsF															|
		//	| @INPUT   :  *msg string into sram												|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	PutSF(const char *string);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Set CGRAM Address													|
		//	| @NAME    :  lcdSetCGRAMAddr													|
		//	| @INPUT   :  addr	sgram address												|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	SetCGRAMAddr(uint8_t addr);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Set DDRAM Address		(Set Address of RAM)						|
		//	| @NAME    :  lcdSetDDRAMAddr													|
		//	| @INPUT   :  addr	ddram address												|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	SetDDRAMAddr(uint8_t addr);
		//	+-------------------------------------------------------------------------------+
		//	| @FUNCTION:  Redefine 0..7 characters											|
		//	| @NAME    :  lcdRedefineChar													|
		//	| @INPUT   :  char	ASCII character code 0..7									|
		//	|			  *fString	flash string with 8 sequential definitions byte			|
		//	| @OUTPUT  : 																	|
		//	| @REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	RedefineChar(uint8_t character,const char *fString);
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Return presence of display at hardware bus							|
		//	| NAME    :  lcdPresence														|
		//	| INPUT   :  																	|
		//	| OUTPUT  :  true if LCD Present at configured pins								|
		//	| REMARKS :  lcdInit must be caller before use this function					|
		//	+-------------------------------------------------------------------------------+
		bool inline Present()
		{
			return PresentFlag;
		}
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Initialize Big Numbers												|
		//	| NAME    :  lcdBigInit															|
		//	| INPUT   :  																	|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  Interrupts are disabled during initialization						|
		//	+-------------------------------------------------------------------------------+
		void	LCDBigInit();
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Print big number at column N character								|
		//	| NAME    :  PutCBig															|
		//	| INPUT   :  																	|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  This function works only in Displays with 2 rows					|
		//	+-------------------------------------------------------------------------------+
		void	PutCBig(char asciiDigit);
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Print big number at column N string								|
		//	| NAME    :  PutSBig															|
		//	| INPUT   :  string into sram													|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  This function works only in Displays with 2 rows					|
		//	+-------------------------------------------------------------------------------+
		void	PutSBig(char *string);
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Save character on screen into buffer         						|
		//	| NAME    :  ScrSave															|
		//	| INPUT   :                             										|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	ScrSave();
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Restore character from buffer                 						|
		//	| NAME    :  ScrRestore															|
		//	| INPUT   :                             										|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	ScrRestore();
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Save pattern character into buffer         						|
		//	| NAME    :  PatternSave														|
		//	| INPUT   :                             										|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	PatternSave();
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Restore pattern character from buffer         						|
		//	| NAME    :  PatternRestore														|
		//	| INPUT   :                             										|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		void	PatternRestore();
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Get Drive Version Minor number		        						|
		//	| NAME    :  GetVersionMinor													|
		//	| INPUT   :                             										|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		uint8_t	inline	GetVersionMinor(void)
		{
			return _HD44780_VERSION_MINOR;
		}
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Get Drive Version Major number		        						|
		//	| NAME    :  GetVersionMinor													|
		//	| INPUT   :                             										|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		uint8_t	inline	GetVersionMajor(void)
		{
			return _HD44780_VERSION_MAJOR;
		}
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Get Display number of Rows			        						|
		//	| NAME    :  GetRows															|
		//	| INPUT   :                             										|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+		
		uint8_t	inline	GetRows()
		{
			return Rows;
		}
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Get Display number of Cols			        						|
		//	| NAME    :  GetCols															|
		//	| INPUT   :                             										|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		uint8_t inline	GetCols()
		{
			return Cols;
		}
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Get Cursor Row						        						|
		//	| NAME    :  GetRow																|
		//	| INPUT   :                             										|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		uint8_t	inline	GetRow()
		{
			return Row;
		}
		//	+-------------------------------------------------------------------------------+
		//	| FUNCTION:  Get Cursor actually Col				        					|
		//	| NAME    :  GetCol																|
		//	| INPUT   :                             										|
		//	| OUTPUT  :  ----																|
		//	| REMARKS :  																	|
		//	+-------------------------------------------------------------------------------+
		uint8_t inline	GetCol()
		{
			return Col;
		}
};        

#endif  // _F8_WIFI_HD44780_DRIVE_H_