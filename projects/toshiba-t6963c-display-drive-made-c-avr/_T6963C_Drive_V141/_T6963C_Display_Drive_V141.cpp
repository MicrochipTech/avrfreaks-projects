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
  ---------------------------------- C FILE (Tab 4) --------------------------------------
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
*@brief <b>(C++)</b> File for T6963C Drive
*@copyright GNU Public License.
******************************************************************************************
*/
/*
 +--------------------------------------+
 |        LIBRARY INCLUDES				|
 +--------------------------------------+
*/
#include "_T6963C_Display_Drive_V141.h"

/*
 +--------------------------------------+
 |        FLASH CONSTANTS				|
 +--------------------------------------+
*/

const uint8_t _T6963C_F_PATTERN_EMPTY[8]			PROGMEM ={0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00};
const uint8_t _T6963C_F_PATTERN_SLASH[8]			PROGMEM ={0X10,0X20,0X40,0X80,0X01,0X02,0X04,0X08};
const uint8_t _T6963C_F_PATTERN_BACKSLASH[8]		PROGMEM ={0X08,0X04,0X02,0X01,0X80,0X40,0X20,0X10};
const uint8_t _T6963C_F_PATTERN_GRID[8]				PROGMEM ={0X08,0X08,0X08,0XFF,0X08,0X08,0X08,0X08};
const uint8_t _T6963C_F_PATTERN_DIAGONAL_GRID[8]    PROGMEM ={0X81,0X42,0X24,0X18,0X18,0X24,0X42,0X81};
const uint8_t _T6963C_F_PATTERN_CIRCLE[8]			PROGMEM ={0X00,0X18,0X24,0X42,0X42,0X24,0X18,0X00};
const uint8_t _T6963C_F_PATTERN_POINT[8]			PROGMEM ={0X88,0X00,0X00,0X00,0X88,0X00,0X00,0X00};
const uint8_t _T6963C_F_PATTERN_BOX[8]				PROGMEM ={0X00,0X7C,0X44,0X44,0X44,0X7C,0X00,0X00};
const uint8_t _T6963C_F_PATTERN_SOLID[8]			PROGMEM ={0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF};

/*
 +--------------------------------------+
 |        LOCAL FUNCTIONS CONSTANTS		|
 +--------------------------------------+
*/



// DISPLAY NUMBER OF COLUNMS SET IN HARDWARE


const uint16_t	_T6963C_PAGE0	    =0X0000;
const uint16_t	_T6963C_PAGE1 	    =0X8000;

/*
 +--------------------------------------+
 |        LOCAL TYPEDEF					|
 +--------------------------------------+
*/
//
// DISPLAY INTERFACE COMMANDS
//
typedef enum class _enum_T6963C_InterfaceCommands:uint8_t
{
	 _COMMAND_WRITE=0,
	 _DATA_WRITE,
	 _STATUS_READ,
	 _DATA_READ
} _T6963C_IntefaceCommands;

/*
 +--------------------------------------+
 |        LOCAL FUNCTIONS VARIABLES		|
 +--------------------------------------+
*/



/*
 +--------------------------------------+
 |										|
 |       DISPLAY DRIVE FUNCTIONS		|
 |										|
 +--------------------------------------+
*/

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Read Procedure from T6963C 										|
//	| NAME    :  InterfaceRead														|
//	| INPUT   :																		|
//	| OUTPUT  :	 data or status														|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
uint8_t class_T6963C_Drive_V141::InterfaceRead()
{
	uint8_t tmp;
	
	#ifdef _T6963C_TEST_MODE_ 
		return	0;
	#endif
	//CE=Low,RD=Low
	_T6963C_CE_OUTPUT&=~(1<<_T6963C_CE_BIT);
	asm("nop");
	_T6963C_RD_OUTPUT&=~(1<<_T6963C_RD_BIT);
	//Wait a minimal time delay
	#if (F_CPU>=4000000)
	asm("nop");
	#endif
	#if (F_CPU>=8000000)
	asm("nop");
	#endif
	#if (F_CPU>=12000000)
	asm("nop");
	#endif
	#if (F_CPU>=16000000)
	asm("nop");
	#endif
	asm("nop");
	tmp=_T6963C_DATA_INPUT;
	//Wait a minimal time delay
	#if (F_CPU>=4000000)
	asm("nop");
	#endif
	#if (F_CPU>=8000000)
	asm("nop");
	#endif
	#if (F_CPU>=12000000)
	asm("nop");
	#endif
	#if (F_CPU>=16000000)
	asm("nop");
	#endif
	//RD=High,CE=High
	_T6963C_RD_OUTPUT|=(1<<_T6963C_RD_BIT);
	asm("nop");
	_T6963C_CE_OUTPUT|=(1<<_T6963C_CE_BIT);
	//return data or status
	return tmp;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Write Procedure to T6963											|
//	| NAME    :  InterfaceWrite														|
//	| INPUT   :	 data or command													|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::InterfaceWrite()
{
	#ifdef _T6963C_TEST_MODE_
		return;
	#endif
	//CE=Low,WR=Low
	_T6963C_CE_OUTPUT&=~(1<<_T6963C_CE_BIT);
	asm("nop");
	_T6963C_WR_OUTPUT&=~(1<<_T6963C_WR_BIT);
	//Wait a minimal time delay
	#if (F_CPU>=4000000)
	asm("nop");
	#endif
	#if (F_CPU>=8000000)
	asm("nop");
	#endif
	#if (F_CPU>=12000000)
	asm("nop");
	#endif
	#if (F_CPU>=16000000)
	asm("nop");
	#endif
	//CE=High,WR=High
	_T6963C_WR_OUTPUT|=(1<<_T6963C_WR_BIT);
	asm("nop");
	_T6963C_CE_OUTPUT|=(1<<_T6963C_CE_BIT);
	//thold
	asm("nop");
	asm("nop");
	//set data to input as pull up
	_T6963C_DATA_DIR=0x00;
	_T6963C_DATA_OUTPUT=0xff;
	//thold
	asm("nop");
	asm("nop");
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Read a Status from T6963											|
//	| NAME    :  ReadStatus															|
//	| INPUT   :																		|
//	| OUTPUT  :	 status																|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
uint8_t class_T6963C_Drive_V141::InterfaceStatusRead()
{
	//CD=Command(High)
	_T6963C_CD_OUTPUT|=(1<<_T6963C_CD_BIT);
	//return Status
	return InterfaceRead();
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Read a data from  T6963											|
//	| NAME    :  DataRead															|
//	| INPUT   :																		|
//	| OUTPUT  :	 data																|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
uint8_t class_T6963C_Drive_V141::InterfaceDataRead()
{
	//CD=Data(Low)
	_T6963C_CD_OUTPUT&=~(1<<_T6963C_CD_BIT);
	//return Status
	return InterfaceRead();
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Write Command to T6963												|
//	| NAME    :  CmdWrite															|
//	| INPUT   :	 Cmd Command to be write											|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+

void	class_T6963C_Drive_V141::InterfaceCmdWrite(uint8_t cmd)
{
	//Set data to be write and set as output
	_T6963C_DATA_OUTPUT=cmd;
	_T6963C_DATA_DIR=0xff;
	//CD=Command(High)
	_T6963C_CD_OUTPUT|=(1<<_T6963C_CD_BIT);
	//write cmd
	InterfaceWrite();
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Write Data to T6963												|
//	| NAME    :  DataWrite															|
//	| INPUT   :	 data to be write													|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+

void	class_T6963C_Drive_V141::InterfaceDataWrite(uint8_t data)
{
	//Set data to be write and set as output
	_T6963C_DATA_OUTPUT=data;
	_T6963C_DATA_DIR=0xff;
	//CD=Data(Low)
	_T6963C_CD_OUTPUT&=~(1<<_T6963C_CD_BIT);
	//write data
	InterfaceWrite();
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Wait Until Can be read or Write to T6963							|
//	| NAME    :  CanReadWrite														|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::CanReadWrite()
{
	uint8_t tmp,statusBit;
	#ifdef _T6963C_TEST_MODE_
		return;
	#endif

	//set Status Bits
	statusBit=(1<<static_cast<uint8_t>(_T6963C_StatusBit::_STA0_COMMAND_EXECUTION_CAPABILITY_BIT)) + \
			  (1<<static_cast<uint8_t>(_T6963C_StatusBit::_STA1_DATA_READ_WRITE_CAPABILITY_BIT));
	//check if can write
	do{
		tmp=InterfaceStatusRead();
		tmp&=statusBit;
	} while (tmp!=statusBit);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Wait Until can be Auto Write to T6963								|
//	| NAME    :  CanAutoWrite														|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::CanAutoWrite()
{
	uint8_t tmp,statusBits;
	#ifdef _T6963C_TEST_MODE_
		return;
	#endif
	
	//Set Status Bits
	statusBits=(1<<static_cast<uint8_t>(_T6963C_StatusBit::_STA3_AUTO_MODE_WRITE_CAPABILITY_BIT));
	//check if can write
	do{
		tmp=InterfaceStatusRead();
		tmp&=statusBits;
	} while (tmp!=statusBits);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Wait Until can be Auto Read to T6963								|
//	| NAME    :  CanAutoRead														|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::CanAutoRead()
{
	uint8_t tmp,statusBits;
	#ifdef _T6963C_TEST_MODE_
		return;
	#endif
	
	//Set Status Bits
	statusBits=(1<<static_cast<uint8_t>(_T6963C_StatusBit::_STA2_AUTO_MODE_READ_CAPABILITY_BIT));
	//check if can read
	do{
		tmp=InterfaceStatusRead();
		tmp&=statusBits;
	} while (tmp!=statusBits);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Write a data to T6963												|
//	| NAME    :  DataWrite															|
//	| INPUT   :	 data data to be write												|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::DataWrite(uint8_t dataToBeWritten)
{
	//check if can write
	CanReadWrite();
	//write a data
	InterfaceDataWrite(dataToBeWritten);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Read a data from T6963												|
//	| NAME    :  DataRead															|
//	| INPUT   :																		|
//	| OUTPUT  :  data																|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
uint8_t class_T6963C_Drive_V141::DataRead()
{
	//check if can write
	CanReadWrite();
	//read a data
	return InterfaceDataRead();
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Write a command to T6963											|
//	| NAME    :  CmdWrite															|
//	| INPUT   :	 Cmd to be write													|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::CmdWrite(uint8_t commandToBeWritten)
{
	//check if can write
	CanReadWrite();
	//write a data
	InterfaceCmdWrite(commandToBeWritten);
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Auto Write data to T6963											|
//	| NAME    :  DataAutoWrite														|
//	| INPUT   :	 dataToBeWritten													|
//	| OUTPUT  :	 Data Read															|
//	| REMARKS :	to use DataAutoWrite must follow below sequence						|
//	|																				|
//	|		SetModeAutoWrite();	Start Auto Mode Write								|
//	|		DataAutoWrite(0xaa);invoque at this point how many write data you want	|
//	|		SetModeAutoReset();	End Auto Mode Reset									|
//	|																				|
//	|		When enter in SetModeAutoWrite no more any commands is accepted except	|
//	|		SetModeAutoReset and off coarse DataAutoWrite							|
//	+-------------------------------------------------------------------------------+

void	class_T6963C_Drive_V141::DataAutoWrite(uint8_t dataToBeWritten)
{
	//check if can write
	CanAutoWrite();
	//write a data
	InterfaceDataWrite(dataToBeWritten);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Auto read data from T6963											|
//	| NAME    :  DataAutoRead														|
//	| INPUT   :																		|
//	| OUTPUT  :	 Data Read															|
//	| REMARKS :	to use DataAutoRead must follow below sequence						|
//	|																				|
//	|		SetModeAutoRead();	Start Auto Mode Read								|
//	|		DataAutoRead();		invoque at this point how many read data you want	|
//	|		SetModeAutoReset();	End Auto Mode Read									|
//	|																				|
//	|		When enter in SetModeAutoRead no more any commands is accepted except	|
//	|		SetModeAutoReset and off coarse DataAutoRead							|
//	+-------------------------------------------------------------------------------+

uint8_t class_T6963C_Drive_V141::DataAutoRead()
{
	//check if can write
	CanAutoRead();
	//read a data
	return InterfaceDataRead();
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Generate a Milliseconds delay										|
//	| NAME    :  delayMs									     					|
//	| INPUT   :	 timeMS Delay in Milliseconds										|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+

static void delayMs(uint16_t timeMS)
{
	#ifdef _T6963C_TEST_MODE_
		return;
	#endif
	unsigned int timeCount;
	//compute 1 millisecond time
	//this function generate a error of 0.375uS by millisecond at 16Mhz
	timeCount=(F_CPU)/4000;
	//generate N milliseconds time
	for(;;)
	{
		//exit if zero(0)
		if (timeMS==0) break;
		//delay 1 millisecond
		asm volatile("movw   r24,%A0	    \t\n"	\
					 "1:                    \t\n"   \
				 	 "sbiw r24,1		    \t\n"	\
					 "brne 1b               \t\n"   \
					 :"+r" (timeCount)
					 :
					 :"r24","r25"
					 );
		//pump Time
		timeMS--;
	}
	
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Blink LED Error													|
//	| NAME    :  BlinkLEDError								     					|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::BlinkLEDError(uint16_t blinkTimeMs)
{
	//Set BLINK LED as output
	_T6963C_BLINK_LED_DIR|=(1<<_T6963C_BLINK_LED_BIT);
	for(;;)
	{
		//Blink LED
		_T6963C_BLINK_LED_OUTPUT^=(1<<_T6963C_BLINK_LED_BIT);
		//Wait a Time
		delayMs(blinkTimeMs);
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Reset T6963 Display Drive											|
//	| NAME    :  Reset										     					|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::Reset()
{
	//wait power establish
	#ifndef _T6963C_TEST_MODE_
		delayMs(100);
	#endif
	//Set Reset Low
	_T6963C_RESET_OUTPUT&=~(1<<_T6963C_RESET_BIT);
	//Wait 1 millisecond
	delayMs(1);
	//Set Reset High
	_T6963C_RESET_OUTPUT|=(1<<_T6963C_RESET_BIT);
	//Wait 1 millisecond
	delayMs(1);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set T6963 Text Address (Area where text is placed)					|
//	| NAME    :  SetTextAddress								     					|
//	| INPUT   :	 textAddress start text memory address								|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetTextAddress(uint16_t textAddress)
{
	DataWrite((uint8_t)textAddress);
	DataWrite((uint8_t)(textAddress>>8));
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::SET_CONTROL_WORD)+ \
			 static_cast<uint8_t>(_T6963C_Functions::SET_TEXT_HOME_ADDRESS));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set T6963 Text number of visible Cols								|
//	| NAME    :  SetTextArea								     					|
//	| INPUT   :	 displayCols number of displayed columns							|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetTextArea(uint8_t displayCols)
{
	DataWrite(displayCols);
	DataWrite(0);
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::SET_CONTROL_WORD)+ \
			 static_cast<uint8_t>(_T6963C_Functions::SET_TEXT_AREA));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set T6963 Start Graph Address										|
//	| NAME    :  SetGraphAddress							     					|
//	| INPUT   :	 graphAddr  Graph Start Address										|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetGraphAddress(uint16_t graphAddr)
{
	DataWrite((uint8_t)graphAddr);
	DataWrite((uint8_t)(graphAddr>>8));
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::SET_CONTROL_WORD)+ \
			 static_cast<uint8_t>(_T6963C_Functions::SET_GRAPHIC_HOME_ADDRESS));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set T6963 Visible Area												|
//	| NAME    :  SetGraphAddress							     					|
//	| INPUT   :	 graphAddr  Graph Start Address										|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetGraphArea(uint8_t displayCols)
{
	DataWrite((uint8_t)displayCols);
	DataWrite((uint8_t)0);
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::SET_CONTROL_WORD)+ \
			 static_cast<uint8_t>(_T6963C_Functions::SET_GRAPHIC_AREA));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Address Pointer Where Graph or Text is Write After				|
//	| NAME    :  SetAddressPointer							     					|
//	| INPUT   :	 addressPointer  Address where write or read occur					|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetAddressPointer(uint16_t addressPointer)
{
	DataWrite((uint8_t)addressPointer);
	DataWrite((uint8_t)(addressPointer>>8));
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::REGISTERS_SETTINGS)+ \
		 	 static_cast<uint8_t>(_T6963C_Functions::SET_ADDRESS_POINTER));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Cursor Position using Row and Col								|
//	| NAME    :  SetCursorPos								     					|
//	| INPUT   :	 Row,Col at Display Locate											|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetCursorPos(uint8_t Row,uint8_t Col)
{
	if (Col>=1 && Col<=_DisplayCols && Row>=1 && Row<=_HardwareRows)
	{
		switch (this->_ScanMode)
		{
			case _T6963C_ScanMode::SINGLE:
				Row--;
				Col--;
				DataWrite(Col);
				DataWrite(Row);
				CmdWrite(static_cast<uint8_t>(_T6963C_Commands::REGISTERS_SETTINGS)+ \
           				 static_cast<uint8_t>(_T6963C_Functions::SET_CURSOR_POINTER));
			break;
			case _T6963C_ScanMode::DUAL:
				Col--;			
				DataWrite(Col);
				(Row>(_HardwareRows/2))?DataWrite((Row-(_HardwareRows/2)-1)+0x10):DataWrite(Row-1);
				CmdWrite(static_cast<uint8_t>(_T6963C_Commands::REGISTERS_SETTINGS)+ \
		 				 static_cast<uint8_t>(_T6963C_Functions::SET_CURSOR_POINTER));
			break;
		}
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set CGRAM (Character Generator RAM) Offset Address					|
//	| NAME    :  SetCGRAMOffset							     						|
//	| INPUT   :	 cgramAddress Character Generator Star RAM Address					|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetCGRAMOffset(uint16_t cgramAddress)
{
	DataWrite((uint8_t)cgramAddress>>11);
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::REGISTERS_SETTINGS)+ \
			 static_cast<uint8_t>(_T6963C_Functions::SET_OFFSET_REGISTER));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set MODE AUTO WRITE MODE											|
//	| NAME    :  SetModeAutoWrite							     					|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetModeAutoWrite()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::DATA_AUTO_READ_WRITE)+ \
			 static_cast<uint8_t>(_T6963C_Functions::SET_DATA_AUTO_WRITE));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set MODE AUTO READ MODE											|
//	| NAME    :  SetModeAutoRead							     					|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetModeAutoRead()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::DATA_AUTO_READ_WRITE)+ \
			 static_cast<uint8_t>(_T6963C_Functions::SET_DATA_AUTO_READ));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set MODE AUTO RESET MODE											|
//	| NAME    :  SetModeAutoReset							     					|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetModeAutoReset()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::DATA_AUTO_READ_WRITE)+ \
			 static_cast<uint8_t>(_T6963C_Functions::AUTO_RESET));
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Display Modes  Status Enabled(true) or Disabled(false)			|
//	| NAME    :  SetDisplayMode								     					|
//	| INPUT   :	 displayMode may be DISPLAY,TEXT,CURSOR or BLINK					|
//	|			 true to Enabled Mode false to Disable								|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetDisplayMode(_T6963C_DisplayModes	displayMode,bool status)
{
	uint8_t mode=0;
	
	//Check Text Mode
	if (_DisplayMode.Text)
		mode|=(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_TEXT));
	else
		mode&=~(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_TEXT));
	//Check Graph Mode
	if (_DisplayMode.Graph)
		mode|=(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_GRAPH));
	else
		mode&=~(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_GRAPH));
	//Check Cursor Mode
	if (_DisplayMode.Cursor)
		mode|=(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_CURSOR));
	else
		mode&=~(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_CURSOR));
	//Check Blink Mode
	if (_DisplayMode.Blink)
		mode|=(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_BLINK));
	else
		mode&=~(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_BLINK));
	
	//switch to new state
	switch (displayMode)
	{
		case _T6963C_DisplayModes::_TEXT:
			_DisplayMode.Text=status;
			status?mode|=(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_TEXT)) \
			      :mode&=~(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_TEXT));
		break;
		case _T6963C_DisplayModes::_GRAPH:
			_DisplayMode.Graph=status;
			status?mode|=(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_GRAPH)) \
			      :mode&=~(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_GRAPH));
		break;
		case _T6963C_DisplayModes::_CURSOR:
			_DisplayMode.Cursor=status;
			status?mode|=(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_CURSOR)) \
			      :mode&=~(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_CURSOR));
		break;
		case _T6963C_DisplayModes::_BLINK:
			_DisplayMode.Blink=status;
			status?mode|=(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_BLINK)) \
			      :mode&=~(1<<static_cast<uint8_t>(_T6963C_DisplayModes::_BLINK));
		break;
	}
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::DISPLAY_MODE)+mode);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Cursor Size													|
//	| NAME    :  SetCursorSize								     					|
//	| INPUT   :	 cursorSize	Cursor Size 1..8										|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetCursorSize(uint8_t cursorSize)
{
	if (cursorSize>=1 && cursorSize<=8)
	{
		cursorSize--;
		CmdWrite(static_cast<uint8_t>(_T6963C_Commands::CURSOR_PATTERN_SELECT)+ cursorSize);		
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Fill RAM with pattern												|
//	| NAME    :  FillRAM															|
//	| INPUT   :	 pattern Pattern is character in text mode and byte in graph mode	|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::FillRAM(uint16_t startAddress,uint16_t fillSize,uint8_t patternByte)
{
	//Fill RAM is fillsize is not igual zero
	if (fillSize!=0)
	{
		//Set Start Address to Fill	
		SetAddressPointer(startAddress);
		//Set AutoWrite Mode
		SetModeAutoWrite();
		//Fill RAM
		for(uint16_t i=0;i<fillSize;i++) DataAutoWrite(patternByte);
		//Set Auto Reset
		SetModeAutoReset();		
	}
	
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Clear Text Screen with default character							|
//	| NAME    :  ClsText															|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::ClsText()
{
	uint16_t	page=0;
	//check if a valid ASCII characters
	if (_ClsTextChar>=' ' && _ClsTextChar<=('~'+' '))
	{
		//select active page
		switch (_TextActivePage)
		{
			case 1:
				page=_TextStartAddress;
			break;
			case 2:
				page=_TextStartAddress2;
			break;
		}
		//Fill according scan mode
		switch (_ScanMode)
		{
			case _T6963C_ScanMode::SINGLE:
				FillRAM(page,_HardwareRows*_HardwareCols,_ClsTextChar-' ');
			break;
			case _T6963C_ScanMode::DUAL:
				FillRAM(page| _T6963C_PAGE0,(_HardwareRows/2)*_HardwareCols,_ClsTextChar-' ');
				FillRAM(page| _T6963C_PAGE1,(_HardwareRows/2)*_HardwareCols,_ClsTextChar-' ');			
			break;
		}		
	}
	//set row=1,col=1
	_Row=_Col=1;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Clear Graph Screen													|
//	| NAME    :  ClsGraph															|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::ClsGraph()
{
	/*
	uint16_t page=0;
	//select active page
	switch (_GraphActivePage)
	{
		case 1:
			page=_GraphStartAddress;
		break;
		case 2:
			page=_GraphStartAddress2;
		break;
	}
	//Fill according scan mode
	switch (_ScanMode)
	{
		case _T6963C_ScanMode::SINGLE:
			FillRAM(page ,_HardwareRows*_HardwareCols*8,_ClsGraphPattern);
		break;
		case _T6963C_ScanMode::DUAL:
			FillRAM(page| _T6963C_PAGE0,(_HardwareRows/2)*_HardwareCols*8,_ClsGraphPattern);
			FillRAM(page| _T6963C_PAGE1,(_HardwareRows/2)*_HardwareCols*8,_ClsGraphPattern);
		break;
	}	
	*/
	
	_T6963C_Pattern p;
	DrawMode dm;
	//save Actual pattern
	GetDrawPattern(p);
	//set Empty pattern
	SetDrawPatternType(_T6963C_Patterns::EMPYT);
	//save Actual Draw Mode
	dm=this->GetDrawMode();
	//set new draw mode
	this->SetDrawMode(DrawMode::PATTERN);
	//Draw Box if pattern 
	this->DrawBox(_Window.xMin,_Window.yMin,_Window.xMax,_Window.yMax,Pen::White);	//Pen is ignored in Patter fill mode
	//Restore Draw mode
	this->SetDrawMode(dm);
	//Restore graph pattern
	SetDrawPattern(p);
	
	//Set Coordinates to 0,0
	_X=_Y=0;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Data Write and Increment Pointer									|
//	| NAME    :  DataWriteInc														|
//	| INPUT   :	 data value															|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::DataWriteInc(uint8_t dataToBeWritten)
{
	DataWrite(dataToBeWritten);
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::DATA_READ_WRITE)+static_cast<uint8_t>(_T6963C_Functions::DATA_WRITE_AND_INCREMENT_ADP));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Data Write and Decrement Pointer									|
//	| NAME    :  DataWriteDec														|
//	| INPUT   :	 data value															|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::DataWriteDec(uint8_t dataToBeWritten)
{
	DataWrite(dataToBeWritten);
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::DATA_READ_WRITE)+static_cast<uint8_t>(_T6963C_Functions::DATA_WRITE_AND_DECREMENT_ADP));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Data Write and no Move Pointer										|
//	| NAME    :  DataWriteNoMove													|
//	| INPUT   :	 data value															|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::DataWriteNoMove(uint8_t dataToBeWritten)
{
	DataWrite(dataToBeWritten);
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::DATA_READ_WRITE)+static_cast<uint8_t>(_T6963C_Functions::DATA_WRITE_AND_NOVARIABLE_ADP));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Data Read and increment Pointer									|
//	| NAME    :  DataReadInc														|
//	| INPUT   :																		|
//	| OUTPUT  :	 data read															|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
uint8_t class_T6963C_Drive_V141::DataReadInc()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::DATA_READ_WRITE)+static_cast<uint8_t>(_T6963C_Functions::DATA_READ_AND_INCREMENT_ADP));
	return DataRead();
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Data Read and decrement Pointer									|
//	| NAME    :  DataReadInc														|
//	| INPUT   :																		|
//	| OUTPUT  :	 data read															|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
uint8_t class_T6963C_Drive_V141::DataReadDec()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::DATA_READ_WRITE)+static_cast<uint8_t>(_T6963C_Functions::DATA_READ_AND_DECREMENT_ADP));
	return DataRead();
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Data Read and no Move Pointer										|
//	| NAME    :  DataReadNoMove														|
//	| INPUT   :																		|
//	| OUTPUT  :	 data read															|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
uint8_t class_T6963C_Drive_V141::DataReadNoMove()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::DATA_READ_WRITE)+static_cast<uint8_t>(_T6963C_Functions::DATA_READ_AND_NOVARIABLE_ADP));
	return DataRead();
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Or Logic Mode with Text and Graph								|
//	| NAME    :  SetOrMode															|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetOrMode()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::MODE_SET)+static_cast<uint8_t>(_T6963C_Functions::OR_MODE));	
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set EXor Logic Mode with Text and Graph							|
//	| NAME    :  SetEXorMod															|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetEXorMode()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::MODE_SET)+static_cast<uint8_t>(_T6963C_Functions::EXOR_MODE));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set And Logic Mode with Text and Graph								|
//	| NAME    :  SetAndMode															|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetAndMode()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::MODE_SET)+static_cast<uint8_t>(_T6963C_Functions::AND_MODE));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set CG(Character Generator) from ROM								|
//	| NAME    :  SetInternalCGROM													|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetInternalCGROM()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::MODE_SET)+static_cast<uint8_t>(_T6963C_Functions::INTERNAL_CG_ROM_MODE));
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set CG(Character Generator) from RAM								|
//	| NAME    :  SetExternalCGRAM													|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetExternalCGRAM()
{
	CmdWrite(static_cast<uint8_t>(_T6963C_Commands::MODE_SET)+static_cast<uint8_t>(_T6963C_Functions::EXTERNAL_CG_RAM_MODE));
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Check RAM position													|
//	| NAME    :  CheckRAMPosition													|
//	| INPUT   :																		|
//	| OUTPUT  :	 true if RAM position OK											|
//	| REMARKS :	 RAM Position if Ok if in that position written 0 and read back 0	|
//	|			 write 0x55 and read back 0x55 write 0xaa and return 0xaa			|
//	+-------------------------------------------------------------------------------+
bool class_T6963C_Drive_V141::CheckRAMPosition(uint16_t RAMAddr)
{
	//Initial RAM Start Address
	SetAddressPointer(RAMAddr);
	//write 0xAA
	DataWriteNoMove(0XAA);
	//return false if not read back 0XAA
	if (DataReadNoMove()!=0XAA)	return false;
	//write 0x55
	DataWriteNoMove(0X55);
	//return false if not read back 0X55
	if (DataReadNoMove()!=0X55)	return false;
	//write 0x55
	DataWriteNoMove(0X00);
	//return false if not read back 0x55
	if (DataReadNoMove()!=0X00)	return false;
	//else ok return true
	return true;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Compute RAM SIZE													|
//	| NAME    :  ComputeRAMSize														|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
uint16_t class_T6963C_Drive_V141::GetComputedRAMSize()
{
	uint16_t ramSize=0;
	#define BYTE_CHECK_1 0X31
	#define BYTE_CHECK_2 0X41
	#define BYTE_CHECK_3 0X59
	#define BYTE_CHECK_4 0X26
	#define MINIMAL_REQUIRED_RAM	(uint16_t(_HardwareRows*_HardwareCols))+(uint16_t(_HardwareCols*_HardwareRows*8))
	
	if (CheckRAMPosition(ramSize)==false) return 0;
	DataWriteNoMove(BYTE_CHECK_1);
	ramSize++;
	if (CheckRAMPosition(ramSize)==false) return 0;
	DataWriteNoMove(BYTE_CHECK_2);
	ramSize++;
	if (CheckRAMPosition(ramSize)==false) return 0;
	DataWriteNoMove(BYTE_CHECK_3);
	ramSize++;
	if (CheckRAMPosition(ramSize)==false) return 0;
	DataWriteNoMove(BYTE_CHECK_4);
	ramSize++;
	for(;;)
	{
		//check patterns
		SetAddressPointer(ramSize);
		if (DataReadNoMove()==BYTE_CHECK_1)
		{
			SetAddressPointer(ramSize+1);
			if (DataReadNoMove()==BYTE_CHECK_2)
			{
				SetAddressPointer(ramSize+2);
				if (DataReadNoMove()==BYTE_CHECK_3)
				{
					SetAddressPointer(ramSize+3);
					if (DataReadNoMove()==BYTE_CHECK_4) break;
				}
			}
			SetAddressPointer(ramSize);
		}
		//if (CheckRAMPosition(ramSize)==false) return 0;
		ramSize++;
		if (ramSize==0x0000) break;
	}
	//return size according scan mode
	switch (_ScanMode)
	{
		case _T6963C_ScanMode::SINGLE:
			if (ramSize<MINIMAL_REQUIRED_RAM) return 0;
			return ramSize;
		case _T6963C_ScanMode::DUAL:
			if ((ramSize*2)<MINIMAL_REQUIRED_RAM) return 0;
			return ramSize*2;
	}
	return 0;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Initialize T6963 Display Drive										|
//	| NAME    :  InterfaceInitialize												|
//	| INPUT   :  hardwareRows How many Rows have a Display selected by hardware pins|
//	|			 hardwareCols How many Cols have a Display selected by hardware pins|
//	|			 displayCols How may Cols to Display have a Display selected by		|
//	|			 hardware pins														|
//	| OUTPUT  :  true if Initialization process performed correctly					|
//	| REMARKS :  interrupts are disabled during initialization then restored		|
//	+-------------------------------------------------------------------------------+
bool class_T6963C_Drive_V141::InterfaceInitialize(uint8_t hardwareRows,uint8_t hardwareCols,uint8_t displayCols,_T6963C_ScanMode scanMode)
{
	uint8_t		sreg;
	uint16_t	ramEnd;
	//save SREG state
	sreg=SREG;
	cli();
	//INITIALIZE HARDWARE PINS
	//------------------------------------------------------------
	//set Rows and Cols
	//Set display data port as input with pull up
	_T6963C_DATA_OUTPUT=0xff;
	_T6963C_DATA_DIR=0x00;
	//Set RW as output hi state
	_T6963C_WR_OUTPUT|=(1<<_T6963C_WR_BIT);
	_T6963C_WR_DIR|=(1<<_T6963C_WR_BIT);
	//Set RD as output hi state
	_T6963C_RD_OUTPUT|=(1<<_T6963C_RD_BIT);
	_T6963C_RD_DIR|=(1<<_T6963C_RD_BIT);
	//Set CE as output hi state
	_T6963C_CE_OUTPUT|=(1<<_T6963C_CE_BIT);
	_T6963C_CE_DIR|=(1<<_T6963C_CE_BIT);
	//Set CD as output hi state
	_T6963C_CD_OUTPUT|=(1<<_T6963C_CD_BIT);
	_T6963C_CD_DIR|=(1<<_T6963C_CD_BIT);
	//Set RESET as output hi state
	_T6963C_RESET_OUTPUT|=(1<<_T6963C_RESET_BIT);
	_T6963C_RESET_DIR|=(1<<_T6963C_RESET_BIT);
	//------------------------------------------------------------
	//Set variables
	_ScanMode=scanMode;
	_HardwareRows=hardwareRows;
	_HardwareCols=hardwareCols;
	_DisplayCols=displayCols;
	_DisplayRows=hardwareRows;
	//------------------------------------------------------------
	//Reset display
	Reset();
	#ifndef _T6963C_TEST_MODE_
		//Get Ram Size
		if (CheckRAMPosition(0)==false)			return false; //return false if no RAM
	#endif
	//------------------------------------------------------------
	//Set Display Modes
 	SetDisplayMode(_T6963C_DisplayModes::_TEXT,false);
 	SetDisplayMode(_T6963C_DisplayModes::_GRAPH,false);
	//------------------------------------------------------------
	//Get Ram Size
	#ifndef _T6963C_TEST_MODE_
		if ((_RAMSize=GetComputedRAMSize())==0)	return false; //return false if no RAM
	#else
		_RAMSize=8192;
	#endif
	//_RAMSize=2000;  //For test when Display has a little memory
	//------------------------------------------------------------
	//Compute Text and Graph Start Address
	switch (_ScanMode)
	{
		case _T6963C_ScanMode::SINGLE:
			_CGRAMStartAddress=0;					//<<<<<<not yed computed
			//check number of support pages
			_TextStartAddress=0;
			_TextStartAddress2=_TextStartAddress+(uint16_t((_HardwareRows)*_HardwareCols));
			//hold one text page?
			if (_TextStartAddress2<(_RAMSize)) _TextPages=1;
			_GraphStartAddress=_TextStartAddress2+(uint16_t((_HardwareRows)*_HardwareCols));
			//hold two text page?
			if (_GraphStartAddress<(_RAMSize)) _TextPages++;
			_GraphStartAddress2=_GraphStartAddress+(uint16_t(uint16_t(_HardwareRows<<3)*_HardwareCols));
			//hold one graph page?
			if (_GraphStartAddress2<(_RAMSize)) _GraphPages=1;
			ramEnd=_GraphStartAddress2+(uint16_t(uint16_t(_HardwareRows<<3)*_HardwareCols));
			if (ramEnd<(_RAMSize))
			{
				_GraphPages++;
			}
			else
			{
				//TRY TO ALLOCATE ONE PAGE1 FOR TEXT AND GRAPH
				_TextStartAddress=0;
				_GraphStartAddress=_HardwareCols*_HardwareRows;
				_TextPages=_GraphPages=1;
				ramEnd=_GraphStartAddress+(uint16_t(uint16_t(_HardwareRows<<3)*_HardwareCols));
				if (ramEnd>_RAMSize) return false;
			}
		break;
		case _T6963C_ScanMode::DUAL:
			_CGRAMStartAddress=0;					//<<<<<<not yed computed
			//check number of support pages
			_TextStartAddress=0;
			_TextStartAddress2=_TextStartAddress+(uint16_t((_HardwareRows>>1)*_HardwareCols));
			//hold one text page?
			if (_TextStartAddress2<(_RAMSize>>1)) _TextPages=1;
			_GraphStartAddress=_TextStartAddress2+(uint16_t((_HardwareRows>>1)*_HardwareCols));
			//hold two text page?
			if (_GraphStartAddress<(_RAMSize>>1)) _TextPages++;
			_GraphStartAddress2=_GraphStartAddress+(uint16_t(uint16_t(_HardwareRows<<2)*_HardwareCols));
			//hold one graph page?
			if (_GraphStartAddress2<(_RAMSize>>1)) _GraphPages=1;
			ramEnd=_GraphStartAddress2+(uint16_t(uint16_t(_HardwareRows<<2)*_HardwareCols));
			if (ramEnd<(_RAMSize>>1))
			{
				_GraphPages++;
			}
			else
			{
				//TRY TO ALLOCATE ONE PAGE1 FOR TEXT AND GRAPH
				_TextStartAddress=0;
				_GraphStartAddress=_HardwareCols*(_HardwareRows>>1);
				_TextPages=_GraphPages=1;
				ramEnd=_GraphStartAddress+(uint16_t(uint16_t(_HardwareRows<<2)*_HardwareCols));
				if (ramEnd>_RAMSize) return false;
			}
		break;
	}
	//Set Default Text and Graph Visible And Active page
	_TextActivePage=1;
	_TextVisiblePage=1;
	_GraphActivePage=1;
	_GraphVisiblePage=1;
	_GraphWidth=int(_DisplayCols*8);
	_GraphHeight=int(_HardwareRows*8);
	_Window.xMin=0;
	_Window.yMin=0;
	_Window.xMax=_GraphWidth-1;
	_Window.yMax=_GraphHeight-1;
	//------------------------------------------------------------
	//Set Text Home Address
	SetTextAddress(_TextStartAddress);
	//------------------------------------------------------------
	//Set Text Area
	SetTextArea(_DisplayCols);
	//------------------------------------------------------------
	//Set Graph Home Address
	SetGraphAddress(_GraphStartAddress);
	//------------------------------------------------------------
	//Set Graph Area
	SetGraphArea(_DisplayCols);
	//------------------------------------------------------------
	//Set Mode
	SetInternalCGROM();
	SetOrMode();
	//------------------------------------------------------------
	//Set CGRAM Offset Address
	SetCGRAMOffset(0);
	//------------------------------------------------------------
	//Set Display Modes
	SetDisplayMode(_T6963C_DisplayModes::_TEXT,true);
	SetDisplayMode(_T6963C_DisplayModes::_GRAPH,true);
	SetDisplayMode(_T6963C_DisplayModes::_CURSOR,true);
	SetDisplayMode(_T6963C_DisplayModes::_BLINK,true);
	//------------------------------------------------------------	
	//Clear Text and Graph Area
	switch (_TextPages)
	{
		case 1:
			SetTextActivePage(1);
			ClsText();
		break;
		case 2:
			SetTextActivePage(2);
			ClsText();
			SetTextActivePage(1);
			ClsText();
		break;
	}
	switch (_GraphPages)
	{
		case 1:
			SetGraphActivePage(1);
			ClsGraph();
		break;
		case 2:
			SetGraphActivePage(2);
			ClsGraph();
			SetGraphActivePage(1);
			ClsGraph();
		break;
	}
	//------------------------------------------------------------
	//Set Address Pointer
	SetAddressPointer(0);
	//------------------------------------------------------------
	//Set Row,Col and X,Y
	_X=_Y=0;
	_Row=_Col=1;
	//------------------------------------------------------------
	//Initialize Cursor Position and Cursor Size to 2
	SetCursorSize(1);
	SetCursorPos(1,1);
	//restore SREG state
	SREG=sreg;
	//return  present status
	return true;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  T6963C Constructor													|
//	| NAME    :  Constructor														|
//	| INPUT   :  hardwareRows How many Rows have a Display selected by hardware pins|
//	|			 hardwareCols How many Cols have a Display selected by hardware pins|
//	|			 displayCols How may Cols to Display have a Display selected by		|
//	|			 hardware pins														|
//	| REMARKS :  interrupts are disabled during initialization then restored		|
//	+-------------------------------------------------------------------------------+
class_T6963C_Drive_V141::class_T6963C_Drive_V141(uint8_t hardwareRows,uint8_t hardwareCols,uint8_t displayCols,_T6963C_ScanMode scanMode)
:class_GraphBasic(hardwareRows,displayCols,displayCols*8,hardwareRows*8)
//:_Rows(hardwareRows),_Cols(displayCols),_GraphWidth(displayCols*8),_GraphHeight(_HardwareRows*8)
{
	_Present=InterfaceInitialize(hardwareRows,hardwareCols,displayCols,scanMode);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  T6963C Destructor													|
//	| NAME    :  Destructor															|
//	| INPUT   :																		|
//	| REMARKS :  interrupts are disabled during initialization then restored		|
//	+-------------------------------------------------------------------------------+
class_T6963C_Drive_V141::~class_T6963C_Drive_V141()
{
	
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Pattern with Mask												|
//	| NAME    :  DrawPatternWithMask												|
//	| INPUT   :																		|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::DrawPatternWithMask(uint8_t mask,uint8_t pattern,Pen pen,bool autoWrite)
{
	//Write Pattern with Mask (11111111),(00011111),(00000111)//(11000000),(11111000) etc
	switch (this->GetDrawMode())
	{
		case DrawMode::LINE:
		case DrawMode::SOLID:
			if (autoWrite)
			{
				DataAutoWrite(static_cast<uint8_t>(pen));
			}
			else
			{
				if (mask==0xff)
				{
					DataWriteInc(static_cast<uint8_t>(pen));
				}
				else
				{
					DataWriteInc((DataReadNoMove() & (~mask)) | (mask & static_cast<uint8_t>(pen)));
				}				
			}
		break;
		case DrawMode::PATTERN:
			if (autoWrite)
			{
				DataAutoWrite(pattern);
			}
			else
			{
				if (mask==0xff)
				{
					DataWriteInc(pattern);
				}
				else
				{
					DataWriteInc((DataReadNoMove() & (~mask)) | (mask & pattern));
				}				
			}
		break;
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Fast Horizontal Line											|
//	| NAME    :  HorzLine															|
//	| INPUT   :	 y Coordinate xS Start X coordinate, xE end X coordinate			|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::DrawHorzLine(int y,int xS,int xE,Pen pen)
{
	#ifdef _T6963C_TEST_MODE_
	static
	#endif
	uint16_t	acPage=0,			//Active Page
				page=0,				//page if dual mode
				addr,
				widthPixel;			//width in pixels
	#ifdef _T6963C_TEST_MODE_
	static
	#endif				
	uint8_t		bS,					//bs=Bit Start Number
				Pat,
				MaskMove,			//individual and mask for each bit
				MaskResult;			//result mask
	
	//if start X > end X swap xS,Xe
	if (xS>xE)
	{
		page=xS;
		xS=xE;
		xE=page;
	}
	//return if  y<yMin
	if (y<this->_Window.yMin) return;
	//return if y>yMax
	if (y>this->_Window.yMax) return;
	//according Active Page
	switch (this->_GraphActivePage)
	{
		case 1:
			acPage=this->_GraphStartAddress;
		break;
		case 2:
			acPage=this->_GraphStartAddress2;
		break;	
	}
	//according scanMode
	switch (_ScanMode)
	{
		case _T6963C_ScanMode::SINGLE:
			//none need
		break;
		case _T6963C_ScanMode::DUAL:
			//assume page 0
			page=_T6963C_PAGE0;
			//check if in page 1
			if (y>=(_GraphHeight>>1))
			{
				//yes, set page 1
				y-=(_GraphHeight>>1);
				page=_T6963C_PAGE1;
			}
		break;
	}		
	//return if out of range
	if (xS<this->_Window.xMin && xE<this->_Window.xMin) return;
	if (xS>this->_Window.xMax && xE>this->_Window.xMax) return;
	//saturate
	if (xS<this->_Window.xMin)	xS=this->_Window.xMin;
	if (xE>this->_Window.xMax)  xE=this->_Window.xMax;
	//Get Start and End Bits
	bS=xS & 7;
	//compute Start Address,Byte Start and End
	addr=(y*this->_HardwareCols)+(xS>>3)+(acPage+page);
	SetAddressPointer(addr);
	//get this line patter
	Pat=_Pattern.row[y&7];
	//Compute start mask
	MaskMove=0x80>>bS;	
	MaskResult=0;
	//Compute Width
	widthPixel=xE-xS+1;
	//Draw pixels
	
	bool bAutoWrite=false;
	
	for(uint16_t i=0;i<widthPixel;i++)
	{
		//compute result mask
		MaskResult|=MaskMove;
		//shift maskmove
		MaskMove>>=1;
		//time to write results?
		if (MaskMove==0)
		{
			//yes,reset maskmove
			MaskMove=0x80;
			//Draw Pattern with Mask
			if (bAutoWrite)
			{
				DrawPatternWithMask(MaskResult,Pat,pen,true);	
			}
			else
			{
				DrawPatternWithMask(MaskResult,Pat,pen,false);		
				//set autowrite mode
				SetModeAutoWrite();
				bAutoWrite=true;				
			}
			//clear maskResult
			MaskResult=0;
		}
	}
	//
	SetModeAutoReset();
	//Draw Pattern with Mask
	if (MaskResult!=0) DrawPatternWithMask(MaskResult,Pat,pen,false);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Locate Graph Cursor to X,Y coordinates								|
//	| NAME    :  LocateXY															|
//	| INPUT   :	 x,y coordinates													|
//	| OUTPUT  :  true if x,y in range else false									|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
bool class_T6963C_Drive_V141::LocateXY(int x,int y)
{
	uint16_t page=0,acPage=0,aAddr=0;
	static uint16_t lAddr=0;
	//set coordinates
	_X=x;
	_Y=y;
	//check if in valid range
	if (x>=this->_Window.xMin && x<=this->_Window.xMax && y>=this->_Window.yMin && y<=this->_Window.yMax)
	{
		switch (_GraphActivePage)
		{
			case 1:
				acPage=_GraphStartAddress;
			break;
			case 2:
				acPage=_GraphStartAddress2;
			break;
		}
		//according scanMode
		switch (_ScanMode)
		{
			case _T6963C_ScanMode::SINGLE:
				//none need
			break;
			case _T6963C_ScanMode::DUAL:
				//assume page 0
				page=_T6963C_PAGE0;
				//check if in page 1
				if (y>=(_GraphHeight>>1))
				{
					//yes, set page 1
					y-=(_GraphHeight>>1);
					page=_T6963C_PAGE1;
				}
			break;
		}
		//set address pointer according active page
		aAddr=(y*_HardwareCols)+(x>>3)+(acPage+page);
		if (aAddr!=lAddr) SetAddressPointer(aAddr);
		SetAddressPointer(aAddr);
		lAddr=aAddr;
		return true;
	}
	else
	{
		return false;
	}
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set a pixel on screen												|
//	| NAME    :  SetPixel															|
//	| INPUT   :  x,y,color coordinates in pixel, color pixel 1 or 0					|
//	| OUTPUT  :  																	|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::SetPixel(int x,int y,Pixel pixel)
{
	uint8_t p;
	//Set graph cursor coordinates
	_X=x;
	_Y=y;
	//set X,Y on base class
	p=x & 7;
	//locate pixel and check if range
	if (LocateXY(x,y))
	{
		//set or reset
		if ((pixel & 0x01)==0x01)
		{
			//set pixel
			CmdWrite(static_cast<uint8_t>(_T6963C_Commands::BIT_SET_RESET) + \
					 static_cast<uint8_t>(_T6963C_Functions::BIT_SET)+(~p & 7));
		}
		else
		{
			//reset pixel
			CmdWrite(static_cast<uint8_t>(_T6963C_Commands::BIT_SET_RESET) + \
					 static_cast<uint8_t>(_T6963C_Functions::BIT_RESET)+(~p & 7));
		}
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get a pixel from screen											|
//	| NAME    :  GetPixel															|
//	| INPUT   :  x,y																|
//	| OUTPUT  :  pixel 1 or 0														|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
Pixel class_T6963C_Drive_V141::GetPixel(int x,int y)
{
	//locate pixel and check if range
	if (LocateXY(x,y))
	{
		//read pixel
		if ((DataReadNoMove() & (0x80>>(x & 7)))==0) return 0; else return 1;
	}
	else
	{
		return 0;
	}
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Row & Column where next string will put						|
//	| NAME    :  LocateRC															|
//	| INPUT   :	 Row,Col Row and Column to set										|
//	| OUTPUT  :	 true if row,col in range else false								|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
bool class_T6963C_Drive_V141::LocateRC(int8_t row,int8_t col)
{
	uint16_t page,acPage=0;
	
	//set new text coordinates
	_Row=row;
	_Col=col;
	//check if in valid range
	if (row>=1 && col>=1 && row<=_HardwareRows && col<=_DisplayCols)
	{
		//select active page
		switch (_TextActivePage)
		{
			case 1:
				acPage=_TextStartAddress;
			break;
			case 2:
				acPage=_TextStartAddress2;
			break;
		}
		//locate according scan mode
		switch (_ScanMode)
		{
			case _T6963C_ScanMode::SINGLE:
				//compute position and set
				SetAddressPointer((row-1)*_HardwareCols+(col-1)+acPage);
			break;
			case _T6963C_ScanMode::DUAL:
				//check if in page 1
				page=_T6963C_PAGE0;
				if (row>_HardwareRows>>1)
				{
					//yes, set page 1
					row-=_HardwareRows>>1;
					page=_T6963C_PAGE1;
				}
				//compute position and set according active page
				SetAddressPointer((row-1)*_HardwareCols+(col-1)+acPage+page);
			break;
		}
		return true;
	}
	else
	{
		return false;
	}
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Put a Character on Display Screen at last row,col position			|
//	| NAME    :  PutC																|
//	| INPUT   :  char ASCII character range ' ' to '~' and more 32 more characters	|
//	| OUTPUT  :  																	|
//	| REMARKS :  check data sheet to seed more 32 characters						|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::PutC(char charASCII)
{
	//locate at coordinates
	LocateRC(_Row,_Col);
	if (charASCII>=' ' && charASCII<=('~'+' '))
	{
		DataWriteNoMove(charASCII-' ');
		//increment column position
		_Col++;
		//check if greater than max cols
		if (_Col>_DisplayCols)
		{
			//if yes, col=1; row++
			_Col=1;
			_Row++;
		}
		if (_UpdateCursorPos) SetCursorPos(_Row,_Col);
	}
	else
	{
		//check for special control characters
		switch (charASCII)
		{
			case '\r':
				_Col=1;
			break;
			case '\n':
				_Row++;
			break;
		}		
		SetCursorPos(_Row,_Col);
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Put a String from RAM on Display Screen at last row,col position	|
//	| NAME    :  PutS																|
//	| INPUT   :  charString a string with valid ASCII character range ' ' to '~'	|
//	|			 and more 32 more characters										|
//	| OUTPUT  :  																	|
//	| REMARKS :  check data sheet to seed more 32 characters						|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::PutS(const char *charString)
{
	while (*charString!='\0') PutC(*charString++);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Put a String FLASH on Display Screen at last row,col position		|
//	| NAME    :  PutSF																|
//	| INPUT   :  charString a string with valid ASCII character range ' ' to '~'	|
//	|			 and more 32 more characters										|
//	| OUTPUT  :  																	|
//	| REMARKS :  check data sheet to seed more 32 characters						|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::PutSF(const char *charString)
{
	for(;;)
	{
		//read char form FLASH
		uint8_t tmp=pgm_read_byte(charString++);
		//if zero end
		if (tmp=='\0') break;
		//put char
		PutC(tmp);
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Text Active Page												|
//	| NAME    :  SetTextActivePage													|
//	| INPUT   :  page Active Page													|
//	| OUTPUT  :  																	|
//	| REMARKS :																		|	
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::SetTextActivePage(uint8_t activePage)
{
	if (activePage>=1 && activePage<=_TextPages)
	{
		_TextActivePage=activePage;
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Text Visible Page												|
//	| NAME    :  SetTextVisiblePage													|
//	| INPUT   :  page Visible Page													|
//	| OUTPUT  :  																	|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::SetTextVisiblePage(uint8_t activePage)
{
	if (activePage>=1 && activePage<=_TextPages)
	{
		switch (activePage)
		{
			case 1:
				SetTextAddress(_TextStartAddress);
			break;
			case 2:			
				SetTextAddress(_TextStartAddress2);
			break;
		}
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Graph Active Page												|
//	| NAME    :  SetGraphActivePage													|
//	| INPUT   :  page Active Page													|
//	| OUTPUT  :  																	|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::SetGraphActivePage(uint8_t activePage)
{
	if (activePage>=1 && activePage<=_GraphPages)
	{
		_GraphActivePage=activePage;
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Graph Visible Page												|
//	| NAME    :  SetGraphVisiblePage												|
//	| INPUT   :  page Visible Page													|
//	| OUTPUT  :  																	|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::SetGraphVisiblePage(uint8_t activePage)
{
	if (activePage>=1 && activePage<=_TextPages)
	{
		switch (activePage)
		{
			case 1:
				SetGraphAddress(_GraphStartAddress);
			break;
			case 2:
				SetGraphAddress(_GraphStartAddress2);
			break;
		}
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get Actual Draw Pattern											|
//	| NAME    :  GetGraphPattern													|
//	| INPUT   :	 																	|
//	| OUTPUT  :  Pattern															|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::GetDrawPattern(_T6963C_Pattern &patternToSave)
{
	//Get a copy of actual pattern
	for(uint8_t i=0;i<8;i++)
	{
		  patternToSave.row[i]=_Pattern.row[i];	
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Actual Draw Pattern											|
//	| NAME    :  GetGraphPattern													|
//	| INPUT   :	 Pattern															|
//	| OUTPUT  :  Pattern															|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::SetDrawPattern(_T6963C_Pattern patternToSet)
{
	//Get a copy of actual pattern
	for(uint8_t i=0;i<8;i++)
	{
		_Pattern.row[i]=patternToSet.row[i];
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Draw Pattern type												|
//	| NAME    :  SetGraphPatternType												|
//	| INPUT   :  PatternType as predefined already exist pattern					|
//	| OUTPUT  :  																	|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::SetDrawPatternType(_T6963C_Patterns patternType)
{
	for(uint8_t i=0;i<8;i++)
	{
		switch (patternType)
		{
			case _T6963C_Patterns::EMPYT:
				this->_Pattern.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_EMPTY[i]);
			break;
			case _T6963C_Patterns::SLASH:
				this->_Pattern.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_SLASH[i]);
			break;
			case _T6963C_Patterns::BACKSLASH:
				this->_Pattern.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_BACKSLASH[i]);
			break;
			case _T6963C_Patterns::CIRCLE:
				this->_Pattern.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_CIRCLE[i]);
			break;
			case _T6963C_Patterns::POINT:
				this->_Pattern.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_POINT[i]);
			break;
			case _T6963C_Patterns::BOX:
				this->_Pattern.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_BOX[i]);
			break;
			case _T6963C_Patterns::GRID:
				this->_Pattern.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_GRID[i]);
			break;
			case _T6963C_Patterns::DIAGONAL_GRID:
				this->_Pattern.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_DIAGONAL_GRID[i]);
			break;
			case _T6963C_Patterns::SOLID:
				this->_Pattern.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_SOLID[i]);
			break;
		}		
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get Draw Pattern type												|
//	| NAME    :  GetGraphPatternType												|
//	| INPUT   :  PatternType as predefined already exist pattern					|
//	| OUTPUT  :  																	|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void class_T6963C_Drive_V141::GetDrawPatternType(_T6963C_Patterns patternType,_T6963C_Pattern &patternToGet)
{
	for(uint8_t i=0;i<8;i++)
	{
		switch (patternType)
		{
			case _T6963C_Patterns::EMPYT:
				patternToGet.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_EMPTY[i]);
			break;
			case _T6963C_Patterns::SLASH:
				patternToGet.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_SLASH[i]);
			break;
			case _T6963C_Patterns::BACKSLASH:
				patternToGet.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_BACKSLASH[i]);
			break;
			case _T6963C_Patterns::CIRCLE:
				patternToGet.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_CIRCLE[i]);
			break;
			case _T6963C_Patterns::POINT:
				patternToGet.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_POINT[i]);
			break;
			case _T6963C_Patterns::BOX:
				patternToGet.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_BOX[i]);
			break;
			case _T6963C_Patterns::GRID:
				patternToGet.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_GRID[i]);
			break;
			case _T6963C_Patterns::DIAGONAL_GRID:
				patternToGet.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_DIAGONAL_GRID[i]);
			break;
			case _T6963C_Patterns::SOLID:
				patternToGet.row[i]=pgm_read_byte(&_T6963C_F_PATTERN_SOLID[i]);
			break;			
		}
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Window Graph where graphics appear								|
//	| NAME    :  SetWindow															|
//	| INPUT   :  xMin,yMin,xMax,yMax Window coordinates								|
//	| OUTPUT  :  																	|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::SetWindow(int xMin,int yMin,int xMax,int yMax)
{
	//swap x if need
	if (xMin>xMax)
	{
		int t;
		t=xMin;
		xMin=xMax;
		xMax=t;
	}
	//swap y if need
	if (yMin>yMax)
	{
		int t;
		t=yMin;
		yMin=yMax;
		yMax=t;
	}
	//saturate
	if (xMin<0) xMin=0;
	if (xMax>(this->_GraphWidth-1)) xMax=this->_GraphWidth-1;
	if (yMin<0) yMin=0;
	if (yMax>(this->_GraphHeight-1)) yMax=this->_GraphHeight-1;
	//set new windows
	this->_Window.xMin=xMin;
	this->_Window.xMax=xMax;
	this->_Window.yMin=yMin;
	this->_Window.yMax=yMax;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get Window Graph coordinates where graphics appear					|
//	| NAME    :  GetWindow															|
//	| INPUT   :																		|
//	| OUTPUT  :  xMin,yMin,xMax,yMax Window coordinates								|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::GetWindow(int &xMin,int &yMin,int &xMax,int &yMax)
{
	xMin=this->_Window.xMin;
	xMax=this->_Window.xMax;
	yMin=this->_Window.yMin;
	yMax=this->_Window.yMax;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get BMP Parms														|
//	| NAME    :  GetBMPParms														|
//	| INPUT   :																		|
//	| OUTPUT  :	 bmpHeader		bmp object											|
//	|			 bitDeep		1,2,4,8 etc											|
//	|			 widthPixels	width of bmp in pixels								|
//	|			 heightPixels	width of bmp in pixels								|
//	|			 widthByte		width of bmp in bytes								|
//	|			 sizeByte		size of bmp in bytes								|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
void	class_T6963C_Drive_V141::GetBMPParms(BitMap		*bitMap,
											 uint16_t	&bmpHeader,
											 int16_t	&bitDeep,
											 int16_t	&widthPixel,
											 int16_t	&heightPixel,
											 uint16_t	&widthByte,
											 uint16_t	&sizeByte)
{
	uint8_t  *bmp;
	//
	bmp=(uint8_t*)bitMap;
	//get signature
	bmpHeader=pgm_read_byte(bmp++);								//no 'B' signature return
	bmpHeader+=pgm_read_byte(bmp++);							//no 'M' signature return
	//Check if bmp 1 Bit deep
	bitDeep=pgm_read_byte(bmp++);								//Get Bit Deep, and hold it temporary in widthpixel
	bitDeep+=pgm_read_byte(bmp++)<<8;
	//Get bmp Width in Pixels
	widthPixel=pgm_read_byte(bmp++);
	widthPixel+=pgm_read_byte(bmp++)<<8;
	//Get bmp Height in Pixels
	heightPixel=pgm_read_byte(bmp++);
	heightPixel+=pgm_read_byte(bmp++)<<8;
	//Get Width in Bytes
	widthByte=pgm_read_byte(bmp++);
	widthByte+=pgm_read_byte(bmp++)<<8;
	//Get bmp Size in Bytes
	sizeByte=pgm_read_byte(bmp++);
	sizeByte+=pgm_read_byte(bmp++)<<8;	
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get BMP Width														|
//	| NAME    :  GetBMPWidth														|
//	| INPUT   :																		|
//	| OUTPUT  :  BMP Width in pixels												|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
int	class_T6963C_Drive_V141::GetBMPWidth(BitMap *bmp)
{
	int widthPixel;
	//check if a BMP
	if (pgm_read_byte(bmp++)!='B') return 0;
	if (pgm_read_byte(bmp++)!='M') return 0;
	//position of height pixel
	bmp+=sizeof(int)*1;
	//Get bmp Height in Pixels
	widthPixel=pgm_read_byte(bmp++);
	widthPixel+=pgm_read_byte(bmp++)<<8;
	//return height
	return widthPixel;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get BMP Height														|
//	| NAME    :  GetBMPHeight														|
//	| INPUT   :																		|
//	| OUTPUT  :  BMP Height in pixels												|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
int	class_T6963C_Drive_V141::GetBMPHeight(BitMap *bmp)
{
	int heightPixel;
	//check if a BMP
	if (pgm_read_byte(bmp++)!='B') return 0;
	if (pgm_read_byte(bmp++)!='M') return 0;
	//position of height pixel
	bmp+=sizeof(int)*2;
	//Get bmp Height in Pixels
	heightPixel=pgm_read_byte(bmp++);
	heightPixel+=pgm_read_byte(bmp++)<<8;
	//return height
	return heightPixel;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get BMP Pixel														|
//	| NAME    :  GetBMPPixel														|
//	| INPUT   :																		|
//	| OUTPUT  :  Pen of the pixel													|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
uint8_t	class_T6963C_Drive_V141::GetBMPPixel(BitMap *bmp,int  x,int y)
{
	#ifdef _T6963C_TEST_MODE_
	static
	#endif
	int16_t bitDeep,widthPixel,heightPixel;
	#ifdef _T6963C_TEST_MODE_
	static
	#endif	
	uint16_t widthByte,sizeBytes,bmpHeader,addr;
	#ifdef _T6963C_TEST_MODE_
	static
	#endif
	uint8_t *bitMap;
	//Get bmp parms
	GetBMPParms(bmp,bmpHeader,bitDeep, widthPixel,heightPixel,widthByte,sizeBytes);	
	//check range
	if (x<0 || y<0) return 0;
	if (x>(widthPixel-1) || y>(heightPixel-1)) return 0;
	//compute pixel to get
	addr=(y*widthByte)+(x>>3);
	//point to flash bitmap
	bitMap=(uint8_t*)bmp;
	bitMap+=addr+sizeof(int)*6;
	//return color
	if ((pgm_read_byte(bitMap) & (0x80>>(x&7)))==0)
	{
		return 0;
	}
	else
	{
		return 0xff;
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw BitMap on Screen												|
//	| NAME    :  DrawBitMap															|
//	| INPUT   :	 x,y start coordinates where bmp is drawing							|
//	|		     bmp Valid Bit Map in Flash											|
//	| OUTPUT  :  return true if BMP Drawn in display ok								|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+
bool	class_T6963C_Drive_V141::DrawBitMap(int x,int y,BitMap *bmp)
{
	#ifdef _T6963C_TEST_MODE_
	static
	#endif
	int16_t widthBmp,heightBmp,										//Width and Height of BitMap
	widthDraw,heightDraw;									//Width and Height of Drawable area
	#ifdef _T6963C_TEST_MODE_
	static
	#endif
	uint16_t widthBytes,sizeBytes,addrBmp=0,addrDraw,acPage=0,scanPage=0;
	#ifdef _T6963C_TEST_MODE_
	static
	#endif
	int16_t xI,yI,xF,yF,											//Coordinates of BitMap
			xS,yS,													//Start Coordinates of BitMap will be draw in Display Area
			xD,yD,													//Coordinates of display area
			yScan;
	#ifdef _T6963C_TEST_MODE_
	static
	#endif
	uint8_t MaskMove,												//Destine Draw mask
			MaskResult=0,
			bmpBits,
			bmpMask,
			bmpData;
	#ifdef _T6963C_TEST_MODE_
	static
	#endif
	bool	bAutoWrite;
	DrawMode dm=GetDrawMode();
	//get signature
	if (pgm_read_byte(bmp++)!='B')	return false;					//no 'B' signature return
	if (pgm_read_byte(bmp++)!='M')	return false;					//no 'M' signature return
	//Check if bmp 1 Bit deep
	{
		uint16_t tmp;
		tmp=pgm_read_byte(bmp++);
		tmp+=pgm_read_byte(bmp++)<<8;
		if (tmp!=1) return false;									//Return if Bit Deep not equal 0
	}
	//Get bmp Width in Pixels
	widthBmp=pgm_read_byte(bmp++);
	widthBmp+=pgm_read_byte(bmp++)<<8;
	if (widthBmp==0)	return false;								//return if width in pixel equal 0
	//Get bmp Height in Pixels
	heightBmp=pgm_read_byte(bmp++);
	heightBmp+=pgm_read_byte(bmp++)<<8;
	if (heightBmp==0) return false;								//return if height in pixel equal 0
	//Get Width in Bytes
	widthBytes=pgm_read_byte(bmp++);
	widthBytes+=pgm_read_byte(bmp++)<<8;
	if (widthBytes==0) return false;								//return if size in bytes equal 0
	//Get bmp Size in Bytes
	sizeBytes=pgm_read_byte(bmp++);
	sizeBytes+=pgm_read_byte(bmp++)<<8;
	if (sizeBytes==0)	return false;								//return if size equal 0
	//after this positioi bmp-->to image data
	//Translate x,y,bitmap.width,bitmap height	to xI,yI,xf,yF coordinates
	xI=x;
	yI=y;
	xF=xI+widthBmp-1;
	yF=yI+heightBmp-1;
	//
	//if one of the following below cases occur, return without drawing
	//
	//										#2
	//										+--------+
	//										|  bmp   |
	//										|		 |
	//										+--------+
	//
	//				#1											    #3
	//				+--------+				+--------+              +--------+
	//				|  bmp   |				|Display |				|  bmp   |
	//				|        |				|		 |				|        |
	//				+--------+				+--------+              +--------+
	//
	//										#4
	//										+--------+
	//										|  bmp   |
	//										|		 |
	//										+--------+
	//
	//check case #1
	if (xI<_Window.xMin && xF<_Window.xMin) return false;			//bmp is below display minX coordinate then return
	//check case #2
	if (yI<_Window.yMin && yF<_Window.yMin) return false;			//bmp is below display minY coordinate then return
	//check case #3
	if (xI>_Window.xMax && xF>_Window.xMax) return false;			//bmp is above display maxX coordinate then return
	//check case #4
	if (yI>_Window.yMax && yF>_Window.yMax) return false;			//bmp is above display maxY coordinate then return
	//saturate xF if need
	if (xF>this->_Window.xMax) xF=this->_Window.xMax;
	if (yF>this->_Window.yMax) yF=this->_Window.yMax;
	//compute xS and xD
	if (xI<this->_Window.xMin)
	{
		xS=this->_Window.xMin-xI;
		xD=this->_Window.xMin;
	}
	else
	{
		xS=0;
		xD=xI;
	}
	//compute width draw
	widthDraw=xF-xD+1;
	//compute yS and yD
	if (yI<this->_Window.yMin)
	{
		yS=this->_Window.yMin-yI;
		yD=this->_Window.yMin;
	}
	else
	{
		yS=0;
		yD=yI;
	}
	//compute height draw
	heightDraw=yF-yD+1;
	//Set according Active Page
	switch (this->_GraphActivePage)
	{
		case 1:
			acPage=this->_GraphStartAddress;
		break;
		case 2:
			acPage=this->_GraphStartAddress2;
		break;
	}

	////////////////////////////////////////////
	//Main Draw BitMap loop
	////////////////////////////////////////////
	for(int y=0;y<heightDraw;y++)
	{
		//according scanMode
		yScan=yD;
		switch (_ScanMode)
		{
			case _T6963C_ScanMode::SINGLE:
			//none need
			break;
			case _T6963C_ScanMode::DUAL:
			//assume page 0
			scanPage=_T6963C_PAGE0;
			//check if in page 1
			if (yScan>=(_GraphHeight>>1))
			{
				//yes, set page 1
				yScan-=(_GraphHeight>>1);
				scanPage=_T6963C_PAGE1;
			}
			break;
		}
		//Compute Draw and Bitmap start position
		addrBmp=(yS*widthBytes)+(xS>>3);
		bmpMask=0x80>>(xS&7);
		bmpBits=0;
		yS++;
		addrDraw=(yScan*this->_HardwareCols)+(xD>>3)+acPage+scanPage;
		yD++;
		SetAddressPointer(addrDraw);
		
		//Compute start mask
		MaskMove=0x80>>(xD&7);
		MaskResult=0;
		bAutoWrite=false;
		//set Pattern Draw Mode
		SetDrawMode(DrawMode::PATTERN);
		//Draw Cols pixels
		
		bmpData=pgm_read_byte(bmp+addrBmp);
		
		for(int x=0;x<widthDraw;x++)
		{
			//copy bmp data bo draw data
			if ((bmpData & bmpMask)!=0)
			{
				bmpBits|=MaskMove;
			}
			else
			{
				bmpBits&=~(MaskMove);
			}
			//Check if time to load new bmp data
			bmpMask>>=1;
			if (bmpMask==0)
			{
				bmpMask=0x80;
				addrBmp++;
				bmpData=pgm_read_byte(bmp+addrBmp);
			}
			//compute result mask
			MaskResult|=MaskMove;
			//shift maskmove
			MaskMove>>=1;
			//time to write results?
			if (MaskMove==0)
			{
				//yes,reset maskmove
				MaskMove=0x80;
				//Draw Pattern with Mask
				if (bAutoWrite)
				{
					DrawPatternWithMask(MaskResult,bmpBits,Pen::Black,true);
				}
				else
				{
					DrawPatternWithMask(MaskResult,bmpBits,Pen::Black,false);
					//set autowrite mode
					SetModeAutoWrite();
					bAutoWrite=true;
				}
				//clear maskResult
				MaskResult=0;
			}
		}
		SetModeAutoReset();
		//Draw Pattern with Mask
		if (MaskResult!=0) DrawPatternWithMask(MaskResult,bmpBits,Pen::Black,false);
//		delayMs(250);
	}
	//restore draw mode
	SetDrawMode(dm);
	//for now return true
	return true;
}


/////////////////////////////////////////////+----------------------------+//////////////////////////////////////////////////////
/////////////////////////////////////////////| GRAPH CHARACTERS FUNCTIONS |//////////////////////////////////////////////////////
/////////////////////////////////////////////+----------------------------+//////////////////////////////////////////////////////
#ifdef _T6963C_USE_GRAPH_CHARS

	//	+-------------------------------------------------------------------------------+
	//	| LOAD GRAPH CHARACTER TABLE											     	|
	//	+-------------------------------------------------------------------------------+

	#include "_GraphBasic_ASCII_Table_5x8.h"

	//	+-------------------------------------------------------------------------------+
	//	| FUNCTION:  Put character on graph mode at last x,y coordinates		     	|
	//	| NAME    :  GPutC       	        					                        |
	//	| INPUT   :                                 			                        |
	//	| OUTPUT  :                                                                     |
	//	| REMARKS :									            		                |
	//	+-------------------------------------------------------------------------------+
	void  class_T6963C_Drive_V141::GPutC(char c,Pixel pixel)
	{
		uint8_t		b,mask;
		uint16_t	index;
		DrawMode	dm;
		int x,y;
		//actual coords
		x=_X;
		y=_Y;
		dm=GetDrawMode();
		//check if in range
		if (c>=' ' && c<='~')
		{
			//translate ascii to binary
			c-=' ';
			index=(c*8);
			//draw chars
			for(uint8_t j=0;j<8;j++)
			{
				mask=0x80;
				b=pgm_read_byte(&_T6963C_ASCII_Table[index++]);
				for(uint8_t i=0;i<=5;i++)
				{
					if ((b & mask)!=0)
					{
						if (this->_GraphTextScale==1)
						{
							if (i<5) SetPixel(x+i,y+j,pixel);
						}
						else
						{
							SetDrawMode(DrawMode::SOLID);
							if (i<5) DrawBox(x+i*this->_GraphTextScale,y+j*this->_GraphTextScale, \
											 x+i*this->_GraphTextScale+this->_GraphTextScale-1,y+j*this->_GraphTextScale+this->_GraphTextScale-1, \
											 (pixel&1)?Pen::Black:Pen::White,false);
							SetDrawMode(dm);
						}
					}
					else
					{
						switch (GetDrawMode())
						{
							case DrawMode::LINE:
							break;
							case DrawMode::SOLID:
							case DrawMode::PATTERN:
								if (this->_GraphTextScale==1)
								{
									SetPixel(x+i,y+j,~pixel);
								}
								else
								{
									DrawBox(x+i*this->_GraphTextScale,y+j*this->_GraphTextScale, \
											x+i*this->_GraphTextScale+this->_GraphTextScale-1,y+j*this->_GraphTextScale+this->_GraphTextScale-1, \
											(~pixel&1)?Pen::Black:Pen::White,false);
								}
							break;
						}
						
					}
					mask>>=1;
				}
			}
// 			//set new coords
			this->_X=x+_T6963C_Char_Width*this->_GraphTextScale;
 			this->_Y=y;
		}
		else
		//check for lf(Line Feed) and cr(carriage return)
		{
			switch (c)
			{
				case '\r':
					this->_X=0;
				break;
				case '\n':
					this->_Y+=_T6963C_Char_Height*this->_GraphTextScale;
				break;
			}
		}
	}
	//	+-------------------------------------------------------------------------------+
	//	| FUNCTION:  Put a String from RAM on Display Graph at last row,col position	|
	//	| NAME    :  GPutS																|
	//	| INPUT   :  charString a string with valid ASCII character range ' ' to '~'	|
	//	|			 and more 32 more characters										|
	//	| OUTPUT  :  																	|
	//	| REMARKS :  check data sheet to seed more 32 characters						|
	//	+-------------------------------------------------------------------------------+
	void class_T6963C_Drive_V141::GPutS(const char *charString,Pixel pixel)
	{
		while (*charString!='\0') GPutC(*charString++,pixel);
	}
	//	+-------------------------------------------------------------------------------+
	//	| FUNCTION:  Put a String FLASH on Graph Screen at last row,col position		|
	//	| NAME    :  PutSF																|
	//	| INPUT   :  charString a string with valid ASCII character range ' ' to '~'	|
	//	|			 and more 32 more characters										|
	//	| OUTPUT  :  																	|
	//	| REMARKS :  check data sheet to seed more 32 characters						|
	//	+-------------------------------------------------------------------------------+
	void class_T6963C_Drive_V141::GPutSF(const char *charString,Pixel pixel)
	{
		for(;;)
		{
			//read char form FLASH
			uint8_t tmp=pgm_read_byte(charString++);
			//if zero end
			if (tmp=='\0') break;
			//put char
			GPutC(tmp,pixel);
		}
	}
#endif