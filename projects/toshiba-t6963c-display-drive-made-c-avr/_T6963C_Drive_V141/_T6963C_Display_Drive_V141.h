
/*
  ////////////////////////////////////////////////////////////////////////////////////////
  //																					//
  //		 _____ ____  _____  ____  _____  ______ _           _						//
  //		|_   _/ ___||  _  |/ ___||____ | |  _  (_)         | |						//
  //		  | |/ /___ | |_| / /___     / / | | | |_ ___ _ __ | | __ _ _   _			//
  //		  | || ___ \\____ | ___ \    \ \ | | | | / __| '_ \| |/ _` | | | |			//
  //		  | || \_/ |.___/ / \_/ |.___/ / | |/ /| \__ \ |_) | | (_| | |_| |			//
  //		  \_/\_____/\____/\_____/\____/  |___/ |_|___/ .__/|_|\__,_|\__, |			//
  //										         | |				 __/ |			//
  //												 |_|				|___/			//
  //                            ______      _           								//
  //                            |  _  \    (_)          								//
  //                            | | | |_ __ ___   _____ 								//
  //                            | | | | '__| \ \ / / _ \								//
  //                            | |/ /| |  | |\ V /  __/								//
  //                            |___/ |_|  |_| \_/ \___|								//
  //                                                    								//
  ////////////////////////////////////////////////////////////////////////////////////////
  --------------------------------- HEADER FILE (Tab 4) ----------------------------------
  //																					//
  // This Driver was made to control only one(1) T6963 based chip display				//
  //																					//
  ----------------------------------------------------------------------------------------
*/

/**
******************************************************************************************
*@file   
*@author Joao Dartagnan Antunes Oliveira
*@date 07/02/2016
*@brief <b>(Header)</b> File for T6963C Drive
*@copyright GNU Public License.
******************************************************************************************
*/
//+-------------------------------------------------------
//| Doxygen Group Definitions
//+-------------------------------------------------------

///@defgroup groupBMP 			BitMaps
///@brief 					    Functions to Handdle BitMaps
///@defgroup groupGraphicText 	Text To Graphics
///@brief 						Functions to Send Texts to Graphic Area


#ifndef __T6963C_DISPLAY_DRIVE_V141_H__
#define __T6963C_DISPLAY_DRIVE_V141_H__

//+-------------------------------------------------------
//| Enable Bellow Line to Put Display in Text Mode
//+-------------------------------------------------------



//#define _T6963C_TEST_MODE_



//+-------------------------------------------------------
//| ENABLE BELOW TO USE GRAPHIC CHARACTERS FUNCTIONS
//+-------------------------------------------------------



#define _T6963C_USE_GRAPH_CHARS

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

/*
 +--------------------------------------+
 |        LOCAL LIBRARY INCLUDES	    |
 +--------------------------------------+
*/

#include "_GraphBasic.h"

/*
 +--------------------------------------+
 |    INTERFACE PORTS CONFIGURATION		|
 +--------------------------------------+
*/


// DATA PORT SET (data port configuration)

///@brief Defined AVR Port for _T6963C Data Output
#define _T6963C_DATA_OUTPUT		PORTA
///@brief Defined AVR Port for _T6963C Data Direction
#define _T6963C_DATA_DIR		DDRA
///@brief Defined AVR Port for _T6963C Data Input
#define _T6963C_DATA_INPUT		PINA

// WR PORT SET (RW Pin Configuration)

#define	_T6963C_WR_OUTPUT		PORTD
#define	_T6963C_WR_DIR			DDRD
#define	_T6963C_WR_INPUT		PIND
	
#define	_T6963C_WR_BIT			7

// RD PORT SET (RD Pin Configuration)

#define	_T6963C_RD_OUTPUT		PORTD
#define	_T6963C_RD_DIR			DDRD
#define	_T6963C_RD_INPUT		PIND

#define	_T6963C_RD_BIT			6

// CE PORT SET (CE Pin Configuration)

#define	_T6963C_CE_OUTPUT		PORTD
#define	_T6963C_CE_DIR			DDRD
#define	_T6963C_CE_INPUT		PIND

#define	_T6963C_CE_BIT			5

// CD PORT SET (CD Pin Configuration)

#define	_T6963C_CD_OUTPUT		PORTD
#define	_T6963C_CD_DIR			DDRD
#define	_T6963C_CD_INPUT		PIND

#define	_T6963C_CD_BIT			4

// RESET PORT SET (RESET Pin Configuration)

#define	_T6963C_RESET_OUTPUT	PORTD
#define	_T6963C_RESET_DIR		DDRD
#define	_T6963C_RESET_INPUT		PIND

#define	_T6963C_RESET_BIT		1

// BLINK LED ERROR PORT

#define _T6963C_BLINK_LED_OUTPUT	PORTB
#define _T6963C_BLINK_LED_DIR		DDRB
#define _T6963C_BLINK_LED_INPUT		PINB

#define _T6963C_BLINK_LED_BIT		0

//////////////////////////
// TYPEDEFS
//////////////////////////
/**
********************************************
*@class _enum_T6963C_ScanMode
*@brief This <b>typedef</b> enum class defines a Scan Modes below \n
********************************************
*/
typedef enum class _enum_T6963C_ScanMode
{
	SINGLE,
	DUAL
} _T6963C_ScanMode;
//
// T6963C DISPLAY MODES
//
/**
********************************************
*@class _enum_T6963C_DisplayModes
*@brief This <b>typedef</b> enum class defines a display modes bellow \n
********************************************
*/
typedef enum class _enum_T6963C_DisplayModes
{
	_GRAPH  =3,
	_TEXT   =2,
	_CURSOR	=1,
	_BLINK  =0
} _T6963C_DisplayModes;
//
// T6963C DISPLAY STATUS BIT
//
/**
********************************************
*@class _enum_T6963C_StatusBit
*@brief This <b>typedef</b> enum class defines a Status Bits of T6963C Chip \n
********************************************
*/
typedef enum class _enum_T6963C_StatusBit
{
	_STA0_COMMAND_EXECUTION_CAPABILITY_BIT=0,
	_STA1_DATA_READ_WRITE_CAPABILITY_BIT,
	_STA2_AUTO_MODE_READ_CAPABILITY_BIT,
	_STA3_AUTO_MODE_WRITE_CAPABILITY_BIT,
	_STA4_NOT_USED_BIT,
	_STA5_CONTROLLER_OPERATION_CAPABILITY_BIT,
	_STA6_ERROR_FLAG_BIT,
	_STA7_BLINK_CONDITION_BIT
} _T6963C_StatusBit;
//
//
//T6963C COMMANDS
//
/**
********************************************
*@class _enum_T6963C_Commands
*@brief This <b>typedef</b> enum class defines a commands of T6963C Chip \n
********************************************
*/
typedef enum class _enum_T6963C_Commands
{
	//                        76543210
	REGISTERS_SETTINGS    = 0X20,
	SET_CONTROL_WORD      = 0X40,
	MODE_SET		      = 0X80,
	DISPLAY_MODE	      = 0X90,
	CURSOR_PATTERN_SELECT = 0XA0,
	DATA_AUTO_READ_WRITE  = 0XB0,
	DATA_READ_WRITE		  = 0XC0,
	SCREEN_PEEK			  = 0XE0,
	SCREEN_COPY			  = 0XE8,
	BIT_SET_RESET		  = 0XF0,
} _T6963C_Commands;
//
//T6963C FUNCTIONS
//
/**
********************************************
*@class _enum_T6963C_Functions
*@brief This <b>typedef</b> enum class defines a functions of T6963C Chip \n

********************************************
*/
typedef enum class _enum_T6963C_Functions
{
	//                                76543210
	///@brief Command to Set Cursor Point (External RAM Address)
	SET_CURSOR_POINTER			  = 0X01,
	///@brief Command to Set External RAM address(d15..d11) of external character generator 
	SET_OFFSET_REGISTER			  = 0X02,
	SET_ADDRESS_POINTER           = 0X04,
	SET_TEXT_HOME_ADDRESS		  = 0X00,
	SET_TEXT_AREA		          = 0X01,
	SET_GRAPHIC_HOME_ADDRESS	  = 0X02,
	SET_GRAPHIC_AREA			  = 0X03,
	OR_MODE						  = 0X00,
	EXOR_MODE					  = 0X01,
	AND_MODE					  = 0X03,
	TEXT_ATTRIBUTE_MODE			  = 0X04,
	INTERNAL_CG_ROM_MODE		  = 0X00,
	EXTERNAL_CG_RAM_MODE		  = 0X08,
	DISPLAY_OFF					  = 0X00,
	CURSOR_ON_BLINK_OFF			  = 0X02,
	CURSOR_ON_BLINK_ON			  = 0X03,
	TEXT_ON_GRAPHIC_OFF			  = 0X04,
	TEXT_OFF_GRAPHIC_ON			  = 0X08,
	TEXT_ON_GRAPHIC_ON			  = 0X0C,
	LINE_CURSOR_1				  = 0X00,
	LINE_CURSOR_2				  = 0X01,
	LINE_CURSOR_3				  = 0X02,
	LINE_CURSOR_4				  = 0X03,
	LINE_CURSOR_5				  = 0X04,
	LINE_CURSOR_6				  = 0X05,
	LINE_CURSOR_7				  = 0X06,
	LINE_CURSOR_8				  = 0X07,
	SET_DATA_AUTO_WRITE			  = 0X00,
	SET_DATA_AUTO_READ			  = 0X01,
	AUTO_RESET					  = 0X02,
	DATA_WRITE_AND_INCREMENT_ADP  = 0X00,
	DATA_READ_AND_INCREMENT_ADP   = 0X01,
	DATA_WRITE_AND_DECREMENT_ADP  = 0X02,
	DATA_READ_AND_DECREMENT_ADP   = 0X03,
	DATA_WRITE_AND_NOVARIABLE_ADP = 0X04,
	DATA_READ_AND_NOVARIABLE_ADP  = 0X05,
	SCREEN_PEEK					  = 0X00,
	SCREEN_COPY					  = 0X01,
	BIT_RESET					  = 0X00,
	BIT_SET						  = 0X08,
	BIT_0						  = 0X00,
	BIT_1						  = 0X01,
 	BIT_2						  = 0X02,
	BIT_3						  = 0X03,
	BIT_4						  = 0X04,
	BIT_5						  = 0X05,
	BIT_6						  = 0X06,
	BIT_7						  = 0X07
} _T6963C_Functions;
//
//DEFINE A TYPEDEF PATTERN
//
/**
********************************************
*@struct _T6963C_Pattern
*@brief This <b>structure</b> hold a T6963C Pattern that will be used
********************************************
*/
typedef  struct 
{
	uint8_t row[8];	
} _T6963C_Pattern;
//
//DEFINE A ENUMERATION OF PATTERNS TO FILL
//
/**
********************************************
*@class _enum_T6963C_patterns
*@brief This <b>typedef</b> enum class defines a patterns for T6963C chip used in functions when need to fill \n
********************************************
*/
typedef enum class _enum_T6963C_patterns
{
	EMPYT,
	SLASH,
	BACKSLASH,
	GRID,
	DIAGONAL_GRID,
	CIRCLE,
	POINT,
	BOX,
	SOLID	
} _T6963C_Patterns;
//
//DEFINE A BITMAP TYPE DEF
//
typedef const uint8_t BitMap;

/**
********************************************
*@class _struct_T6963C_DisplayModes
*@brief This <b>structure</b> hold a Display Modes Status
********************************************
*/
/////////////////////////////
// LOCAL FUNCTIONS STRUCTS
/////////////////////////////
struct _struct_T6963C_DisplayModes
{
	///@brief If true Enable Text Mode Area else Disable
	bool Text:1;
	///@brief If true Enable Graph Mode Area else Disable
	bool Graph:1;
	///@brief If true Enable a Cursor else Disable
	bool Cursor:1;
	///@brief If true Enable a Cursor Blinkink else Disable
	bool Blink:1;
};
/**
********************************************
*@class _struct_T6963C_Window
*@brief This <b>structure</b> is used by SetWindow() and GetWindow() functions to hold defination of display graph area
********************************************
*/
struct _struct_T6963C_Window
{
	int xMin,xMax,yMin,yMax;	
};
//////////////////////////
// VERSION CONTROL
//////////////////////////

const uint8_t _T6963C_VERSION_MAJOR = 1;
const uint8_t _T6963C_VERSION_MINOR = 41;

//////////////////////////
// CONSTANTS
//////////////////////////

const uint8_t _T6963C_Char_Width=6;
const uint8_t _T6963C_Char_Height=8;

/**
******************************************************************************
*
*	@class class_T6963C_Drive_V141
*	@brief This <b>Class</b> Implement a Drive with T6963C Display chip
*
******************************************************************************
*/
class class_T6963C_Drive_V141:public class_GraphBasic
{

       /*
         +------------------------------+
         |       PRIVATE VARIABLES		|
         +------------------------------+
        */
private:
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Number of Hardware Rows defined by pin during manufacturing process of Display hardware
		///@brief and Set by the user when initialization of this class
		//----------------------------------------------------------------------------------------------------------------------------------
		volatile uint8_t								_HardwareRows;
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Number of Hardware Columns defined by pin during manufacturing process of Display hardware
		///@brief and Set by the user when initialization of this class
		//----------------------------------------------------------------------------------------------------------------------------------
		volatile uint8_t								_HardwareCols;
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Number of Columns that is active and visible area of display started from 0 to Columns
		//----------------------------------------------------------------------------------------------------------------------------------
		volatile uint8_t								_DisplayCols;							//Number of Displayed Cols
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Number of Rows that is active and visible area of display started from 0 to Rows
		//----------------------------------------------------------------------------------------------------------------------------------
		volatile uint8_t								_DisplayRows;
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief A ASCII character that will be used by ClsText() function to fill text area with this character
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint8_t								_ClsTextChar=' ';						//Clear Text Screen character
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief A byte Graphic pattern that will be used by ClsGraph(), but actually remains 0x00
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint8_t								_ClsGraphPattern=0x0;					//Clear Graph Screen pattern
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Define a Text Page that is current visible
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint8_t								_TextVisiblePage=1;						//Visible Text Page
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Define a Text Page that is current active, where text commands will be sent
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint8_t								_TextActivePage=1;						//Active Text Page
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Number of Text pages available
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint8_t								_TextPages=1;							//Text Number of Pages
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Define a Graphic Page that current visible
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint8_t								_GraphVisiblePage=1;					//Visible Graph Page
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Define a Graphic Page that current active, where the graphic commands will be sent
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint8_t								_GraphActivePage=1;						//Active Graph Page
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Number of Graphic pages available
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint8_t								_GraphPages=1;							//Graph Number of Pages
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Define a scale of text in use when GPutC(),GPutS() or GPutSF() is used
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint8_t								_GraphTextScale=1;						//Default graph text scale
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief External RAM size used by T6963C chip
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint16_t								_RAMSize=0;								//Total External RAM Size
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief External RAM start address of Text Page 1 
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint16_t								_TextStartAddress=0;					//Text Start Address
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief External RAM start address of Text Page 2 if not enought memory this is always 0
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint16_t								_TextStartAddress2=0;					//Text Start Address 2 (used only if external ram size is great enough)
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief External RAM start address of Graphic Page 1
		//----------------------------------------------------------------------------------------------------------------------------------		
		volatile uint16_t								_GraphStartAddress=0;					//Graph Start Address
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief External RAM start address of Graphic Page 2 if not enought memory this is always 0
		//----------------------------------------------------------------------------------------------------------------------------------				
		volatile uint16_t								_GraphStartAddress2=0;					//Graph Start Address 2 (used only if external ram size is great enough)
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief External RAM start address of Character Generator
		//----------------------------------------------------------------------------------------------------------------------------------				
		volatile uint16_t								_CGRAMStartAddress=0;					//CGRAM(Character Generator RAM) Start Address
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Define to true if display interface is correctly connected and initialized ok
		//----------------------------------------------------------------------------------------------------------------------------------				
		volatile bool									_Present=false;							//Display InteFace ok and Present if true
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief This variable is true if want to update cursor positon after write any text to Text Area
		//----------------------------------------------------------------------------------------------------------------------------------				
		volatile bool									_UpdateCursorPos=true;					//Update Cursor Position if true
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Actual of Row and Col position of Text at Text Area, i.e.  Last writable position
		//----------------------------------------------------------------------------------------------------------------------------------				
		volatile int									_Row=1,_Col=1;							//Display Row and Column 1..n,1..n
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Actual of X,Y position of graphic area, i.e. last used graphic position exemple: last position of SetPixel()
		//----------------------------------------------------------------------------------------------------------------------------------				
		volatile int									_X=0,_Y=0;								//Display X, Y Coordinates for Graph
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Graphics Display Width and Height in pixels
		//----------------------------------------------------------------------------------------------------------------------------------				
		volatile int									_GraphWidth=0,_GraphHeight=0;			//Display Width, Height in pixels
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Used ScanMode can be SINGLE or DUAL
		//----------------------------------------------------------------------------------------------------------------------------------						
		volatile _T6963C_ScanMode						_ScanMode;								//ScanMode
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Hold a actual display mode, that can be _GRAPH,_TEXT,_CURSOR,_BLINK
		//----------------------------------------------------------------------------------------------------------------------------------						
		volatile struct _struct_T6963C_DisplayModes		_DisplayMode;							//DisplayMode
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Definid graphic area where graphical functions is write
		//----------------------------------------------------------------------------------------------------------------------------------						
		volatile struct _struct_T6963C_Window			_Window={.xMin=0,.xMax=0,.yMin=0,.yMax=0};		//Display Window
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief Default Pattern is a Slash 
		//----------------------------------------------------------------------------------------------------------------------------------								
		volatile _T6963C_Pattern						_Pattern={.row={0X10,0X20,0X40,0X80,0X01,0X02,0X04,0X08}}; //default vertical line pattern
		/*
         +------------------------------+
         |       PRIVATE FUNCTIONS      |
         +------------------------------+
        */
		//	+-------------------------------------------------------------------------------+
		///@brief  Initialize Interface and return true if initialization is ok
		///@fn     InterfaceInitialize
		///@param  [in] hardwareRows	number of display rows defined by hardware pins
		///@param  [in] hardwareCols  number of display rows defined by hardware pins
		///@param  [in] displayCols   number of displayed cols
		///@param  [in] scanMode		defined by hardware maybe SINGLE or DUAL
		///@return return true if initialization procedure is ok \n
		///@brief  _______________________________________________________________________________
		///@brief   this function is called by constructor
		///@see class_T6963C_Drive_V141
		//	+-------------------------------------------------------------------------------+
		bool		InterfaceInitialize(uint8_t hardwareRows,uint8_t hardwareCols,uint8_t displayCols,_T6963C_ScanMode scanMode);
		//	+-------------------------------------------------------------------------------+
		///@brief Perform all need to read data from chip interface
		///@fn    InterfaceRead
		///@return data read
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint8_t		InterfaceRead(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Perform all need to write data to chip interface
		///@fn    InterfaceWrite
		///@return None \n
		///@brief 
		//	+-------------------------------------------------------------------------------+
		void		InterfaceWrite(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Perform all need to read Status from chip interface
		///@fn    InterfaceStatusRead
		///@return status read
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint8_t		InterfaceStatusRead(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Read Data from interface with chip
		///@fn    InterfaceDataRead
		///@return data read
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint8_t		InterfaceDataRead(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Write Command to interface with chip
		///@fn    InterfaceCmdWrite
		///@param [in] commandToBeWritten
		///@return none \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void		InterfaceCmdWrite(uint8_t commandToBeWritten);
		//	+-------------------------------------------------------------------------------+
		///@brief Write Data to interface with chip
		///@fn    InterfaceDataWrite
		///@param [in] dataToBeWritten Data to be write at interface
		///@return none\n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void		InterfaceDataWrite(uint8_t dataToBeWritten);
		//	+-------------------------------------------------------------------------------+
		///@brief  Check if interface ready to Read or Write data/commands
		///@fn     CanReadWrite
		///@return none\n
		///@brief  _______________________________________________________________________________
		///@brief  Exit from this function only interface is ready to read or write
		//	+-------------------------------------------------------------------------------+
		void		CanReadWrite(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Check if interface ready to Write fast
		///@fn    CanAutoWrite
		///@return none\n
		///@brief  _______________________________________________________________________________ 
		///@brief Exit from this function only interface is ready for fast write
		//	+-------------------------------------------------------------------------------+
		void		CanAutoWrite(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Check if interface ready to read fast
		///@fn    CanAutoRead
		///@return none\n
		///@brief  _______________________________________________________________________________ 
		///@brief  Exit from this function only interface is ready for fast read
		//	+-------------------------------------------------------------------------------+
		void		CanAutoRead(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Reset Display Chip
		///@fn    Reset
		///@return none\n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void		Reset(void);		
		//	+-------------------------------------------------------------------------------+
		///@brief Check RAM position
		///@fn    CheckRAMPosition
		///@param [in] RAMAddr External RAM address
		///@return true if RAM position OK \n
		///@brief  _______________________________________________________________________________ 
		///@brief RAM Position if Ok if in that position written 0 and read back 0\n
		///@brief write 0x55 and read back 0x55 write 0xaa and return 0x55
		//	+-------------------------------------------------------------------------------+
		
		bool	CheckRAMPosition(uint16_t RAMAddr);
		//	+-------------------------------------------------------------------------------+
		///@brief Compute RAM SIZE
		///@fn    GetComputedRAMSize
		///@return RAM Size in bytes \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint16_t	GetComputedRAMSize(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Draw Pattern with Mask
		///@fn    DrawPatternWithMask
		///@param [in] mask Bit in the mask iqual 1 represent areas where pattern or pen take effect
		///@param [in] pattern Pattern used to fill Line with scanMode=PATTERN
		///@param [in] pen Pen used to fill with scanMode=SOLID
		///@param [in] autoWrite iqual true we must call SetAutoWriteMode after
		///@return none\n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	DrawPatternWithMask(uint8_t mask,uint8_t pattern,Pen pen,bool autoWrite=false);
        /*
         +------------------------------+
         |        PUBLIC FUNCTIONS		|
         +------------------------------+
        */
public:
		//	+-------------------------------------------------------------------------------+
		///@brief 		First Constructor of class_T6963C_Drive_V141
		///@param [in]  hardwareRows number of display rows defined by hardware pins
		///@param [in]  hardwareCols number of display rows defined by hardware pins
		///@param [in]  displayCols  number of displayed cols
		///@param [in]  scanMode	 defined by hardware maybe SINGLE or DUAL
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief Only this constructor is need to initialize the T6963C Chip
		//	+-------------------------------------------------------------------------------+
		class_T6963C_Drive_V141(uint8_t hardwareRows,uint8_t hardwareCols,uint8_t displayCols,_T6963C_ScanMode scanMode);
		//	+-------------------------------------------------------------------------------+
		///@brief   UnInitialize T6963C Display Drive Version 1.41
		///@fn      ~class_T6963C_Drive_V141
		///@return  None \n
		///@brief  _______________________________________________________________________________
		///@brief   UnInitialize T6963C chip
		///@brief   Interrupts are disabled during this process and restored at the end
		//	+-------------------------------------------------------------------------------+
		~class_T6963C_Drive_V141(); 
		//	+-------------------------------------------------------------------------------+
		///@brief   Set Update Cursor Position State
		///@fn      SetUpdateCursorPositionState
		///@param  [in] updateState is true if Cursor State position is to be updated
		///@return None \n
		///@brief 
		//	+-------------------------------------------------------------------------------+
		void	inline SetUpdateCursorPositionState(bool updateState) { _UpdateCursorPos=updateState; }
		//	+-------------------------------------------------------------------------------+
		///@brief  Get External RAM SIZE
		///@fn     GetRAMSize
		///@return RAM Size in bytes
		///@brief 
		//	+-------------------------------------------------------------------------------+
		uint16_t	inline GetRAMSize(void) { return _RAMSize; }
		//	+-------------------------------------------------------------------------------+
		///@brief  Check if T6963 Interface is Ok and Present
		///@fn     Present
		///@return true if present else false
		///@brief
		//	+-------------------------------------------------------------------------------+
		bool	inline Present(void) { return _Present; }
		//	+-------------------------------------------------------------------------------+
		///@brief Write a data to T6963
		///@fn    DataWrite
		///@param [in] dataToBeWritten 	data to be write in T6963C chip
		///@return None \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	DataWrite(uint8_t dataToBeWritten);
		//	+-------------------------------------------------------------------------------+
		///@brief  Read a data from T6963
		///@fn     DataRead
		///@return data	from T6963C chip
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint8_t DataRead(void);
		//	+-------------------------------------------------------------------------------+
		///@brief  Write a command to T6963
		///@fn     CmdWrite
		///@param  [in] commandToBeWritten command to be write in T6963C chip
		///@return None \n
		///@brief 
		//	+-------------------------------------------------------------------------------+
		void	CmdWrite(uint8_t commandToBeWritten);
		//	+-------------------------------------------------------------------------------+
		///@brief  Auto Write data to T6963	chip
		///@fn     DataAutoWrite
		///@param  [in] dataToBeWritten The data to be autowrite in T6963C chip
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief  To use DataAutoWrite must follow below sequence \n
		///@brief  \n
		///@brief  SetModeAutoWrite();	to Start Auto Mode Write \n
		///@brief  DataAutoWrite(0xaa); invoque this command how many times you want to write data \n
		///@brief  SetModeAutoReset();	End Auto Mode using this function \n
		///@brief  \n																			
		///@brief  When enter in SetModeAutoWrite() no more commands is accepted except \n
		///@brief  SetModeAutoReset() and off coarse DataAutoWrite()
		//	+-------------------------------------------------------------------------------+
		void	DataAutoWrite(uint8_t dataToBeWritten);
		//	+-------------------------------------------------------------------------------+
		///@brief  Auto read data from T6963
		///@fn     DataAutoRead
		///@return Data Read \n
		///@brief  _______________________________________________________________________________
		///@brief to use DataAutoRead must follow below sequence \n
		///@brief \n
		///@brief SetModeAutoRead();	Start Auto Mode Read \n
		///@brief DataAutoRead();		invoque at this point how many read data you want \n
		///@brief SetModeAutoReset();	End Auto Mode Read \n
		///@brief \n
		///@brief When enter in SetModeAutoRead no more any commands is accepted except \n
		///@brief SetModeAutoReset and off coarse DataAutoRead
		//	+-------------------------------------------------------------------------------+
		uint8_t	DataAutoRead(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Set T6963 Text Address (Area where text is placed)
		///@fn    SetTextAddress
		///@param [in] textAddress start text memory address
		///@return None \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	SetTextAddress(uint16_t textAddress);
		//	+-------------------------------------------------------------------------------+
		///@brief Set T6963 Text number of visible Cols
		///@fn    SetTextArea
		///@param [in] displayCols number of displayed columns
		///@return None \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	SetTextArea(uint8_t displayCols);
		//	+-------------------------------------------------------------------------------+
		///@brief Set T6963 Start Graph Address
		///@fn    SetGraphAddress
		///@param [in] graphAddr  Graph Start Address
		///@return None \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	SetGraphAddress(uint16_t graphAddr);
		//	+-------------------------------------------------------------------------------+
		///@brief Set T6963 Visible Area
		///@fn    SetGraphArea
		///@param [in] displayCols Graph Start Address
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief Take vary care using this function, see T6963C user manual
		//	+-------------------------------------------------------------------------------+
		void	SetGraphArea(uint8_t displayCols);
		//	+-------------------------------------------------------------------------------+
		///@brief Set Address Pointer Where Graph or Text is Write After
		///@fn    SetAddressPointer
		///@param [in] addressPointer  Address where write or read occur
		///@return None \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	SetAddressPointer(uint16_t addressPointer);
		//	+-------------------------------------------------------------------------------+
		///@brief Set Display Modes  Status Enabled(true) or Disabled(false)
		///@fn    SetDisplayMode
		///@param [in] displayMode may be DISPLAY,TEXT,CURSOR or BLINK
		///@param [in] status true to Enabled Mode false to Disable
		///@return None \n
		///@brief 
		//	+-------------------------------------------------------------------------------+
		void	SetDisplayMode(_T6963C_DisplayModes	displayMode,bool status);
		//	+-------------------------------------------------------------------------------+
		///@brief Set Cursor Position using Row and Col
		///@fn    SetCursorPos
		///@param [in] Row,Col at Display Locate
		///@return None \n
		///@brief 
		//	+-------------------------------------------------------------------------------+
		void	SetCursorPos(uint8_t Row,uint8_t Col);
		//	+-------------------------------------------------------------------------------+
		///@brief Set CGRAM (Character Generator RAM) Offset Address
		///@fn    SetCGRAMOffset
		///@param [in] cgramAddress Character Generator Star RAM Address
		///@return None \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void SetCGRAMOffset(uint16_t cgramAddress);	
		//	+-------------------------------------------------------------------------------+
		///@brief Set MODE AUTO WRITE MODE
		///@fn    SetModeAutoWrite
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief After call this function only DataAutoWrite and SetModeAutoReset \n
		///@brief will be accepted
		//	+-------------------------------------------------------------------------------+
		void	SetModeAutoWrite(void);
		//	+-------------------------------------------------------------------------------+
		///@brief  Set MODE AUTO READ MODE
		///@fn     SetModeAutoRead
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief after call this function only DataAutoRead and SetModeAutoReset \n
		///@brief will be accepted
		//	+-------------------------------------------------------------------------------+
		void	SetModeAutoRead(void);
		//	+-------------------------------------------------------------------------------+
		///@brief  Set MODE AUTO RESET MODE
		///@fn     SetModeAutoReset
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief Reset T6963C Interface to normal operation
		//	+-------------------------------------------------------------------------------+
		void	SetModeAutoReset(void);
		//	+-------------------------------------------------------------------------------+
		///@brief  Set Cursor Size
		///@fn     SetCursorSize
		///@param [in] cursorSize Cursor Size 1..8	
		///@return None \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	SetCursorSize(uint8_t cursorSize);
		//	+-------------------------------------------------------------------------------+
		///@brief Fill RAM with pattern
		///@fn    FillRAM
		///@param [in] startAddress Start Address Area of External RAM 
		///@param [in] fillSize Size of Area of External RAM 
		///@param [in] patternByte Pattern to be used to fill
		///@return none \n
		///@brief  _______________________________________________________________________________
		///@brief Fill RAM with pattern byte
		//	+-------------------------------------------------------------------------------+
		void	FillRAM(uint16_t startAddress,uint16_t fillSize,uint8_t patternByte);
		//	+-------------------------------------------------------------------------------+
		///@brief Set Clear Text Screen Character
		///@fn    SetClsTextChar
		///@param [in] character is a character used to fill entire text area
		///@return None \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	inline SetClsTextChar(char character) { _ClsTextChar=character; }
		//	+-------------------------------------------------------------------------------+
		///@brief Clear Text Screen with default character
		///@fn    ClsText
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief Fill entire text area with character defined in SetClsTextChar()
		//	+-------------------------------------------------------------------------------+
		void	ClsText(void);
		//	+-------------------------------------------------------------------------------+
		///@brief  Clear Graph Screen
		///@fn     ClsGraph
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief  Clearn Entire Graphic Screen Area
		//	+-------------------------------------------------------------------------------+
		void	ClsGraph();
		//	+-------------------------------------------------------------------------------+
		///@brief Data Write and Increment Pointer
		///@fn    DataWriteInc
		///@param [in] dataValue data value to be write
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief Data is Write at External RAM and after this write Pointer is incremented
		//	+-------------------------------------------------------------------------------+
		void	DataWriteInc(uint8_t dataValue);
		//	+-------------------------------------------------------------------------------+
		///@brief Data Write and Decrement Pointer
		///@fn    DataWriteDec
		///@param [in] dataValue data value to Write
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief Data is Write at External RAM and after this write Pointer is decremented
		//	+-------------------------------------------------------------------------------+
		void	DataWriteDec(uint8_t dataValue);
		//	+-------------------------------------------------------------------------------+
		///@brief Data Write and no Move Pointer
		///@fn    DataWriteNoMove
		///@param [in] dataToBeWritten data value to write
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief Data is Write at External RAM and after this write Pointer remainder unchanged
		//	+-------------------------------------------------------------------------------+
		void	DataWriteNoMove(uint8_t dataToBeWritten);		
		//	+-------------------------------------------------------------------------------+
		///@brief Data Read and increment Pointer
		///@fn    DataReadInc
		///@return data read
		///@brief  _______________________________________________________________________________
		///@brief Data is Read From External RAM and after	this read Pointer is incremented
		//	+-------------------------------------------------------------------------------+
		uint8_t	DataReadInc(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Data Read and decrement Pointer
		///@fn    DataReadDec
		///@return data read
		///@brief  _______________________________________________________________________________
		///@brief Data is Read from External RAM and after this read Pointer is decremented
		//	+-------------------------------------------------------------------------------+
		uint8_t	DataReadDec(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Data Read and no Move Pointer
		///@fn    DataReadNoMove
		///@return data read
		///@brief  _______________________________________________________________________________
		///@brief Data is Read from External RAM and after this read Pointer reaminder unchanged
		//	+-------------------------------------------------------------------------------+
		uint8_t	DataReadNoMove();
		//	+-------------------------------------------------------------------------------+
		///@brief  Set Or Logic Mode with Text and Graph
		///@fn     SetOrMode
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief  The Text in Text Area and Graph Area is logically ORed
		//	+-------------------------------------------------------------------------------+
		void	SetOrMode(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Set EXor Logic Mode with Text and Graph
		///@fn    SetEXorMode
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief The Text in Text Area and Graph Area is logically EXored
		//	+-------------------------------------------------------------------------------+
		void	SetEXorMode(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Set And Logic Mode with Text and Graph
		///@fn    SetAndMode
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief The Text in Text Area and Graph Area is logically Anded
		//	+-------------------------------------------------------------------------------+
		void	SetAndMode(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Set CG(Character Generator) from Internal ROM
		///@fn    SetInternalCGROM
		///@return None \n
		///@brief 
		//	+-------------------------------------------------------------------------------+
		void	SetInternalCGROM(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Set CG(Character Generator) from External RAM
		///@fn    SetExternalCGRAM
		///@return None \n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	SetExternalCGRAM(void);
		//	+-------------------------------------------------------------------------------+
		///@brief  Get Version Major Value
		///@fn     VersionMajor
		///@return Major Version value
		///@brief  _______________________________________________________________________________
		///@brief Version 1.41, 1 is a Major Version Number
		//	+-------------------------------------------------------------------------------+
		uint8_t inline VersionMajor() { return _T6963C_VERSION_MAJOR; }
		//	+-------------------------------------------------------------------------------+
		///@brief Get Version Minor Value
		///@fn    VersionMinor
		///@return Minor Version value
		///@brief  _______________________________________________________________________________
		///@brief Version 1.41, .41 is a Minor Version Number
		//	+-------------------------------------------------------------------------------+
		uint8_t inline VersionMinor() { return _T6963C_VERSION_MINOR; }
		//	+-------------------------------------------------------------------------------+
		///@brief Locate Graph Cursor to X,Y coordinates
		///@fn    LocateXY
		///@param [in] x,y coordinates
		///@return true if x,y in range else false
		///@brief
		//	+-------------------------------------------------------------------------------+
		bool	LocateXY(int x,int y);			
		//	+-------------------------------------------------------------------------------+
		///@brief Set Pixel at coordinates x,y with pixel
		///@fn    SetPixel
		///@param [in]  x,y Coordinates where to Pixel is set
		///@param [in]  pixel Pixel value (0 or 1) (1)=black
		///@return None \n
		///@brief ___________________________________________________________________________
		///@brief Below example show how to put a graphic text at specific coordinates \n\n
		///@brief LocateXY(60,100);			Locate at coordinates X=60,Y=100
		///@brief GPutSF(PSTR("Text"));     Print Message "Text" at this location
		//	+-------------------------------------------------------------------------------+;
		virtual void	 SetPixel(int x,int y,Pixel pixel=1);
		//	+-------------------------------------------------------------------------------+
		///@brief Get Pixel at coordinates x,y
		///@fn    SetPixel
		///@param [in] x,y Coordinates where pixel is obtained
		///@return pixel (0 or 1) (1)=black
		///@brief
		//	+-------------------------------------------------------------------------------+;
		virtual Pixel	 GetPixel(int x,int y);
		//	+-------------------------------------------------------------------------------+
		///@brief  Set Row & Column where next string will puted
		///@fn     LocateRC
		///@param  [in] row,col Row and Column to set
		///@return true if row,col in range else false
		///@brief ___________________________________________________________________________
		///@brief Below example show how to put a text in Text area at specific location \n\n
		///@brief LocateRC(2,10);			Locate at location Row=62,Column=10
		///@brief PutSF(PSTR("Text"));      Print Message "Text" at this location
		//	+-------------------------------------------------------------------------------+
		bool	LocateRC(int8_t row,int8_t col);
		//	+-------------------------------------------------------------------------------+
		///@brief Put a Character on Display Screen at last row,col position
		///@fn    PutC
		///@param [in] charASCII character range ' ' to '~' and more 32 more characters
		///@return None \n
		///@brief ___________________________________________________________________________
		///@brief Below example show how to put a character in Text area
		///@brief PutC('A');      Print chracter 'A' in text Area
		//	+-------------------------------------------------------------------------------+
		void	PutC(char charASCII);
		//	+-------------------------------------------------------------------------------+
		///@brief Put a String on Display Screen at last row,col position
		///@fn    PutS
		///@param [in]  charString a string with valid ASCII character range ' ' to '~' and more 32 more characters
		///@return None \n
		///@brief ___________________________________________________________________________
		///@brief Below example show how to put a string of character in Text area
		///@brief PutS("Text");      		Print chracter string "Text" comming from SRAM at Text Area
		//	+-------------------------------------------------------------------------------+
		void	PutS(const char *charString);
		//	+-------------------------------------------------------------------------------+
		///@brief Put a String FLASH on Display Screen at last row,col position
		///@fn    PutSF
		///@param [in] charString a string with valid ASCII character range ' ' to '~' and more 32 more characters
		///@return None \n
		///@brief ___________________________________________________________________________
		///@brief Below example show how to put a string of character in Text area
		///@brief PutS(PSTR("Text"));      Print chracter string "Text" comming from Flash at Text Area
		//	+-------------------------------------------------------------------------------+
		void	PutSF(const char *charString);
		//	+-------------------------------------------------------------------------------+
		///@brief  Get Graph Width
		///@fn     GraphWidth
		///@return return graph width in pixels
		///@brief _______________________________________________________________________________
		///@brief Graphic Width and Height showed below \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Graph_Width_Height.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________			
		//	+-------------------------------------------------------------------------------+
		int	inline GraphWidth() { return _GraphWidth; }
		//	+-------------------------------------------------------------------------------+
		///@brief Get Graph Height
		///@fn    GraphHeight
		///@return return graph height in pixels
		///@brief _______________________________________________________________________________
		///@brief Graphic Width and Height showed below \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Graph_Width_Height.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________			
		//	+-------------------------------------------------------------------------------+
		int	inline GraphHeight() { return _GraphHeight; }
		//	+-------------------------------------------------------------------------------+
		///@brief Get Text Width in characters
		///@fn    TextWidth
		///@return return Text width in characters
		///@brief 
		//	+-------------------------------------------------------------------------------+
		uint8_t	inline TextWidth() { return _DisplayCols; }
		//	+-------------------------------------------------------------------------------+
		///@brief Get Text Height in characters
		///@fn    TextHeight
		///@return return graph height in characters
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint8_t	inline TextHeight() { return _DisplayRows; }
		//	+-------------------------------------------------------------------------------+
		///@brief  Get Text Start Address
		///@fn     GetTextAddress
		///@return return Text Start Address
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint16_t	inline GetTextAddress() { return _TextStartAddress; }
		//	+-------------------------------------------------------------------------------+
		///@brief Get Text2 Start Address
		///@fn    GetText2Address
		///@return return Text2 Start Address
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint16_t	inline GetText2Address() { return _TextStartAddress2; }
		//	+------------------------------------------------------------------------------+
		///@brief  Get Graph Start Address
		///@fn     GetGraphAddress
		///@return return Graph Start Address
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint16_t	inline GetGraphAddress(void) { return _GraphStartAddress; }
		//	a+------------------------------------------------------------------------------+
		///@brief  Get Graph2 Start Address
		///@fn     GetGraph2Address
		///@return return Graph2 Start Address
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint16_t	inline GetGraph2Address() { return _GraphStartAddress2; }
		//	+-------------------------------------------------------------------------------+
		///@brief  Get CGRAM Start Address
		///@fn     GetCGRAMAddress
		///@return return CGRAM Start Address
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint16_t	inline GetCGRAMAddress() { return _TextStartAddress; }
		//	+-------------------------------------------------------------------------------+
		///@brief  Get Text Number of Pages
		///@fn     GetTextPages
		///@return number of text pages
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint8_t	inline GetTextPages() { return _TextPages; }
		//	+-------------------------------------------------------------------------------+
		///@brief  Get Graph Number of Pages
		///@fn     GetGraphPages
		///@return number of graph pages
		///@brief
		//	+-------------------------------------------------------------------------------+
		uint8_t	inline GetGraphPages() { return _GraphPages; }
		//	+-------------------------------------------------------------------------------+
		///@brief Set Text Active Page
		///@fn    SetTextActivePage
		///@param [in] activePage	Active page number, page where text will be placed
		///@return none\n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	SetTextActivePage(uint8_t activePage);
		//	+-------------------------------------------------------------------------------+
		///@brief Set Text Visible Page
		///@fn    SetTextVisiblePage
		///@param [in] activePage  set Visible Page for text
		///@return none\n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	SetTextVisiblePage(uint8_t activePage);
		//	+-------------------------------------------------------------------------------+
		///@brief  Set Graph Active Page
		///@fn     SetGraphActivePage
		///@param  [in] activePage Active graph page, where graphics commands will be sent
		///@return None \n
		///@brief  __________________________________________________________________________
		///@brief 	Set Graph Active Page, where graphics commands are sent
		//	+-------------------------------------------------------------------------------+
		void	SetGraphActivePage(uint8_t activePage);
		//	+-------------------------------------------------------------------------------+
		///@brief Set Graph Visible Page
		///@fn    SetGraphVisiblePage
		///@param [in] activePage Visible graph page number
		///@return none\n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	SetGraphVisiblePage(uint8_t activePage);
		//	+-------------------------------------------------------------------------------+
		///@brief Blink LED Error
		///@fn    BlinkLEDError
		///@param [in] blinkTimeMs Blink Time in milliseconds
		///@return None \n
		///@brief  __________________________________________________________________________		
		///@brief Blink LED if any error occurr during initialization
		//	+-------------------------------------------------------------------------------+
		void	BlinkLEDError(uint16_t blinkTimeMs);
		//	+-------------------------------------------------------------------------------+
		///@brief Draw Fast Horizontal Line
		///@fn    DrawHorzLine
		///@param [in] y Coordinate where line start
		///@param [in] xS Coordinate where X Start
		///@param [in] xE Coordinate where X end
		///@param [in] pen if pen no provide, pen=Pen::Black
		///@return None \n
		///@brief  _______________________________________________________________________________
		///@brief Draw Fast Horizontal Line at Display Graphic Area
		//	+-------------------------------------------------------------------------------+
		void	DrawHorzLine(int y,int xS,int xE,Pen pen=Pen::Black);
		//	+-------------------------------------------------------------------------------+
		///@brief Get Actual Draw Pattern
		///@fn    GetDrawPattern
		///@param [in] pattern 
		///@return Pattern\n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	GetDrawPattern(_T6963C_Pattern &pattern);
		//	+-------------------------------------------------------------------------------+
		///@brief Set Draw Pattern
		///@fn    SetDrawPattern
		///@param [in] pattern Pattern to be used
		///@return none\n
		///@brief
		//	+-------------------------------------------------------------------------------+
		void	SetDrawPattern(_T6963C_Pattern pattern);
		//	+-------------------------------------------------------------------------------+
		///@brief Get Draw Pattern Type
		///@fn    GetDrawPatternType
		///@param [in] patternType  PatternType as predefined already exist pattern
		///@param [in,out] &patternToGet  Pattern to receive PatternType
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Below a Defined Patterns Types
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Pattern_Types.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________		
		//	+-------------------------------------------------------------------------------+
		void	GetDrawPatternType(_T6963C_Patterns patternType,_T6963C_Pattern &patternToGet);		
		//	+-------------------------------------------------------------------------------+
		///@brief Set Draw Pattern Type
		///@fn    SetDrawPatternType
		///@param [in] patternType   PatternType as predefined already exist pattern
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Below a Defined Patterns Types
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Pattern_Types.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________		
		//	+-------------------------------------------------------------------------------+
		void	SetDrawPatternType(_T6963C_Patterns patternType);		
		//	+-------------------------------------------------------------------------------+
		///@brief Set Window Graph where graphics appear
		///@fn    SetWindow
		///@param [in]  xMin,yMin,xMax,yMax Window coordinates
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Transiction from default window(all screen surface) \n
		///@brief and final new window, all drawing is placed only in new defined area \n\n
		///@brief <table style="width:290px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Windows_Default.jpg" style="width:128px;height:96px;"></td>
		///@brief 		<td><img src="_Photos\Windows_New_Area.jpg" style="width:128px;height:96px;"></td>
		///@brief 		<td><img src="_Photos\Windows_New.jpg" style="width:128px;height:96px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________
		//	+-------------------------------------------------------------------------------+
		void	SetWindow(int xMin,int yMin,int xMax,int yMax);		
		//	+-------------------------------------------------------------------------------+
		///@brief Get Window Graph coordinates where graphics appear
		///@fn    GetWindow
		///@return xMin,yMin,xMax,yMax Window coordinates
		///@brief Example of already modified windows area \n\n
		///@brief <table style="width:290px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Windows_New_Area.jpg" style="width:128px;height:96px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________
		//	+-------------------------------------------------------------------------------+
		void	GetWindow(int &xMin,int &yMin,int &xMax,int &yMax);
		//	+-------------------------------------------------------------------------------+
		///@brief  Draw BitMap on Screen
		///@fn     DrawBitMap
		///@param  [in] x,y start coordinates where bmp is drawing
		///@param  [in] bmp Valid Bit Map in Flash
		///@return true if BitMap Draw ok
		///@brief _______________________________________________________________________________
		///@brief BitMaps Examples Below  \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\BitMaps.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________		
		//	+-------------------------------------------------------------------------------+
		bool	DrawBitMap(int x,int y,BitMap *bmp);
		//	+-------------------------------------------------------------------------------+
		///@brief    Get BMP Parms
		///@fn    	 GetBMPParms
		///@return	 bmpHeader		bmp object
		///@return	 bitDeep		1,2,4,8 etc
		///@return	 widthPixels	width of bmp in pixels
		///@return	 heightPixels	width of bmp in pixels
		///@return	 widthByte		width of bmp in bytes
		///@return	 sizeByte		size of bmp in bytes
		///@brief  _______________________________________________________________________________
		///@brief 	Get BitMap Parameters
		//	+-------------------------------------------------------------------------------+
		void	GetBMPParms(BitMap		*bmp,
							uint16_t	&bmpHeader,
							int16_t		&bitDeep,
							int16_t		&widthPixel,
							int16_t		&heightPixel,
							uint16_t	&widthByte,
							uint16_t	&sizeByte);
		//	+-------------------------------------------------------------------------------+
		///@brief  Get BMP Width
		///@fn     GetBMPWidth
		///@param  [in] bmp BitMap
		///@return BMP Width in pixels\n
		///@brief _______________________________________________________________________________
		///@brief BitMap Width and Height  \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\bmp_width_height.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________		
		//	+-------------------------------------------------------------------------------+
		int		GetBMPWidth(BitMap *bmp);
		//	+-------------------------------------------------------------------------------+
		///@brief  Get BMP Height
		///@fn     GetBMPHeight
		///@param  [in] bmp BitMap
		///@return BMP Height in pixels\n
		///@brief _______________________________________________________________________________
		///@brief BitMap Width and Height
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\bmp_width_height.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________		
		//	+-------------------------------------------------------------------------------+
		int		GetBMPHeight(BitMap *bmp);	
		//	+-------------------------------------------------------------------------------+
		///@brief Get BMP Pixel
		///@fn    GetBMPPixel
		///@param [in] bmp BitMap
		///@param [in] x,y coordinates of the pixel to get
		///@return Value of the pixel can be (0 or 1) (1)=black \n
		///@brief  _______________________________________________________________________________
		///@brief Get BitMap Pixel Value from graphic screen
		//	+-------------------------------------------------------------------------------+
		uint8_t	GetBMPPixel(BitMap *bmp,int  x,int y);	
		//	+-------------------------------------------------------------------------------+
		///@brief Put character on graph mode at last x,y coordinates
		///@fn    GPutC
		///@param [in] c Character to put at graphic screen
		///@param [in] pixel Pixel value (0 or 1) (1)=black
		///@return None \n
		///@brief  _______________________________________________________________________________		
		///@brief Put Character on graphic area at last x,y coordinates used
		//	+-------------------------------------------------------------------------------+ 
		void	GPutC(char c,Pixel pixel=1);
		//	+-------------------------------------------------------------------------------+
		///@brief Put a String from RAM on Display Graph at last row,col position
		///@fn    GPutS
		///@param [in] charString a string with valid ASCII character range 
		///@param [in] pixel this paramter is optional, when used can be (0 or 1) by default is (1)
		///@return none\r
		///@brief  _______________________________________________________________________________		
		///@brief check data sheet to seed more 32 characters
		//	+-------------------------------------------------------------------------------+
		void	GPutS(const char *charString,Pixel pixel=1);
		//	+-------------------------------------------------------------------------------+
		///@brief Put a String FLASH on Graph Screen at last row,col position
		///@fn    GPutSF
		///@param [in] charString a string with valid ASCII character range 
		///@param [in] pixel Pixel value (0 or 1) (1)=black
		///@return None \n
		///@brief  _______________________________________________________________________________		
		///@brief   check data sheet to seed more 32 characters
		//	+-------------------------------------------------------------------------------+
		void	GPutSF(const char *charString,Pixel pixel=1);
		//	+-------------------------------------------------------------------------------+
		///@brief Get Character Width in pixels of Character Text Mode
		///@fn    GetTextCharWidth
		///@return return character width in pixels	\n
		///@brief _______________________________________________________________________________
		///@brief Text Width and Height showed below \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Char_Width_Height.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________					
		//	+-------------------------------------------------------------------------------+
		uint8_t inline GetTextCharWidth(void) { return 8;}
		//	+-------------------------------------------------------------------------------+
		///@brief Get Character Height in pixels of Character Text Mode	
		///@fn    GetTextCharHeight
		///@return return character heigth in pixels \n
		///@brief _______________________________________________________________________________
		///@brief Text Width and Height showed below \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Char_Width_Height.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________					
		//	+-------------------------------------------------------------------------------+
		uint8_t inline GetTextCharHeight(void) { return 8; }
		//	+-------------------------------------------------------------------------------+
		///@brief  Get Character Width in pixels of Character Graph Mode
		///@fn     GetGraphCharWidth
		///@return return character width in pixels \n
		///@brief _______________________________________________________________________________
		///@brief Text Width and Height showed below \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Char_Width_Height.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________					
		//	+-------------------------------------------------------------------------------+
		uint8_t inline GetGraphCharWidth(void) { return _T6963C_Char_Width;}
		//	+-------------------------------------------------------------------------------+
		///@brief  Get Character Height in pixels of Character Graph Mode
		///@fn     GetGraphCharHeight
		///@return return character height in pixels \n
		///@brief _______________________________________________________________________________
		///@brief Text Width and Height showed below \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Char_Width_Height.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________					
		//	+-------------------------------------------------------------------------------+
		uint8_t inline GetGraphCharHeight(void) { return _T6963C_Char_Height; }
		//	+-------------------------------------------------------------------------------+
		///@brief  Set Graph Text Scale
		///@fn     SetGraphTextScale
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Text Width and Height showed below \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Char_Width_Height.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________					
		//	+-------------------------------------------------------------------------------+
		void	inline SetGraphTextScale(uint8_t newScale) { this->_GraphTextScale=newScale;}
		//	+-------------------------------------------------------------------------------+
		///@brief  Get Graph Text Scale
		///@fn     GetGraphTextScale
		///@return return scale Text_Scales
		///@brief _______________________________________________________________________________
		///@brief Scale is an integer number 1,2,3... \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Text_Scales.jpg" style="width:300px;height:200px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief ______________________________________________________________________________		
		//	+-------------------------------------------------------------------------------+
		uint8_t inline GetGraphTextScale(void)	{ return this->_GraphTextScale;}

//===========================================================================================
// TAKE VERY CARE TO NOT DELETE BELOW LINE, I LOST HOURS SEARCHING FOR A ERROR THEN WHEN A FIND WASH THIS BELOW LINE MISSING
//============================================================================================
};
//============================================================================================


#endif //__T6963C_DISPLAY_DRIVE_V141_H__


