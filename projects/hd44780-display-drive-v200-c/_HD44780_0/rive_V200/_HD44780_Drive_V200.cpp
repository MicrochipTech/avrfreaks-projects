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
  ---------------------------------- C++ FILE (Tab 4) ------------------------------------
*/

/*
 +--------------------------------------+
 |        LOCAL LIBRARY INCLUDES	    |
 +--------------------------------------+
*/

#include "_HD44780_Drive_V200.h"


/*
 +--------------------------------------+
 |        CONSTANTS                     |
 +--------------------------------------+
*/
//Enable bellow line to put display drive in mode test

//#define __DISPLAY_MODE_TEST__

//Big number characters for redefinitions
const uint8_t _lcd_BigNumber0[] PROGMEM ={0X1F,0X1F,0X1B,0X1B,0X1B,0X1B,0X1B,0X1B};
const uint8_t _lcd_BigNumber1[] PROGMEM ={0X03,0X03,0X03,0X03,0X03,0X03,0X03,0X03};
const uint8_t _lcd_BigNumber2[] PROGMEM ={0X1F,0X1F,0X03,0X03,0X03,0X03,0X03,0X03};
const uint8_t _lcd_BigNumber3[] PROGMEM ={0X1F,0X1F,0X18,0X18,0X18,0X18,0X18,0X18};
const uint8_t _lcd_BigNumber4[] PROGMEM ={0X1B,0X1B,0X1B,0X1B,0X1B,0X1B,0X1F,0X1F};
const uint8_t _lcd_BigNumber5[] PROGMEM ={0X1F,0X1F,0X18,0X18,0X18,0X18,0X1F,0X1F};
const uint8_t _lcd_BigNumber6[] PROGMEM ={0X1F,0X1F,0X03,0X03,0X03,0X03,0X1F,0X1F};
const uint8_t _lcd_BigNumber7[] PROGMEM ={0X1F,0X1F,0X1B,0X1B,0X1B,0X1B,0X1F,0X1F};

//Big number conversion table
const uint8_t _lcd_BigRow1[] PROGMEM = {0,1,2,2,4,3,3,2,0,0};
const uint8_t _lcd_BigRow2[] PROGMEM = {4,1,5,6,1,6,7,1,7,2};
/*
 +--------------------------------------+
 |        VARIABLES                     |
 +--------------------------------------+
*/

/*
 +--------------------------------------+
 |        STRUCTS VARIABLES     	    |
 +--------------------------------------+
*/


/*
        +-------------------------------------------------------+
        |        CLASS HD44780 DRIVE IMPLEMENTATION             |
        +-------------------------------------------------------+
        | @PURPOSE      : Access all HD44780 IO                 |
        | @OBSERVATIONS :                                       |
        +-------------------------------------------------------+
*/

/*
        +-----------------------------------------------+
        |       PRIVATE FUNCTIONS IMPLEMENTATION        |
        +-----------------------------------------------+
*/

/*
 +--------------------------------------+
 |        LOCAL FUNCTIONS VARIABLES	    |
 +--------------------------------------+
*/


/*
 +--------------------------------------+
 |        LOCAL FUNCTIONS STRUCTS		|
 +--------------------------------------+
*/

/*
        +-----------------------------------------------+
        |       PRIVATE FUNCTIONS                       |
        +-----------------------------------------------+
*/
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Generate a Milliseconds delay										|
//	| NAME    :  delayMs									     					|
//	| INPUT   :	 timeMS Delay in Milliseconds										|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+

static void delayMs(uint16_t timeMS)
{
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
/*
  +-----------------------------------------------------------------------------+
  |		HARDWARE INTERFACE TIMING AND FORMATING									|
  +-----------------------------------------------------------------------------+
  |
  | WRITE OPERATION TIME FLOW
  |
  | RS  ---\   /-----------------------------------------------\   /----
  |	    	\ /						        					\ /
  |	     	 +						         					 +
  |	    	/|\						       						/ \
  |     ---/ | \-----------------------------------------------/   \----
  |	     	 |<---- Tas --->|  
  | R/W ---\                |                                      /----
  |	    	\  	  	    	|			                		  /
  |	     	 \		    	|<---- TWeh ---->|	       	       	 /
  |	      	  \		    	|		     	 |		      		/ 
  |     -------\-----------------------------------------------/--------
  |			    			|		     	 |
  | E                       | /------------\ |                      
  |	     		    		|/		    	\|<------ Th ------>|   
  |	    		    		|		     	 |	       	        |  
  |	     		   		   /		      	  \		        	|  
  |     ------------------/                    \------------------------
  |							        | 
  | DO  --------------------------\   /------------------------\|  /----
  | to	                           \ /			        		| /
  | D7	                            +	    Valid Data          +
  |	                           	   / \			       		   / \
  |     --------------------------/   \-----------------------/   \----
  |
  | READ OPERATION TIME FLOW
  |
  | RS  ---\   /-----------------------------------------------\   /----
  |	    	\ /													\ /
  |	     	 +													 +
  |	    	/|\													/ \
  |     ---/ | \-----------------------------------------------/   \----
  |	     	 |<---- Tas --->|  
  | R/W -------/------------|----------------------------------\--------
  |	      	  / 	    	|			               			\
  |	     	 / 		    	|<---- PWeh ---->|	       	         \
  |	    	/   	    	|		     	 |			          \
  |     ---/                |                |                     \---
  |			    			|			     |
  | E                       | /------------\ |                      
  |	     		    		|/		    	\|<------ Th ------>|   
  |	    		    		|		     	 |	       	        |  
  |	     		   		   / 		      	  \		        	|  
  |     ------------------/                    \------------------------
  |							        | 
  | DO  --------------------------\   /------------------------\|  /----
  | to	                           \ /			        		| /
  | D7	                            +	    Valid Data          +
  |	                           	   / \			       		   / \
  |     --------------------------/   \-----------------------/   \----
  |
  | Tas min=40Ns PWeh Min=500Ns  Th min=10Ns
  +-----------------------------------------------------------------------------+
*/

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Send or read data from hd44780 interface							|
//	| @NAME    :  lcdInterface														|
//	| @INPUT   :  Interface type, data to be write									|
//	| @OUTPUT  :  ----																|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+

void	class_HD44780_Drive::Interface(_lcd_INTERFACE lcdControlState,uint8_t *data)
{
uint8_t sreg,tmp;

	//Save SREG state and disable interrupts
	sreg=SREG;
	cli();

	// case WRITE
	if (lcdControlState==_lcd_INTERFACE::CMD_WRITE || lcdControlState==_lcd_INTERFACE::DATA_WRITE)
	{
		if (lcdControlState==_lcd_INTERFACE::CMD_WRITE)
		{
			// Set RS=0 and WR=0 for command write
			_lcd_RS_OUTPUT&=~(1<<_lcd_RS_BIT);
			_lcd_RW_OUTPUT&=~(1<<_lcd_RW_BIT);
		}
		if (lcdControlState==_lcd_INTERFACE::DATA_WRITE)
		{
			// Set RS=1 and WR=0 for data write
			_lcd_RS_OUTPUT|=(1<<_lcd_RS_BIT);
			_lcd_RW_OUTPUT&=~(1<<_lcd_RW_BIT);
		}
		// check interface size
		if (InterfaceSize==_lcd_INTERFACE::SIZE_8_BITS)
		{
			//8bits!! program as output and send data output 8 bits
			_lcd_DATA_OUTPUT=*data;
			//Set port to output
			_lcd_DATA_DIR=0xff;
		}
		else
		{
			//4bits|| program as output with shift and send 4 bits data
			tmp=*data>>4;
			_lcd_DATA_OUTPUT=((~static_cast<uint8_t>(_lcd_INTERFACE::DATA_MASK)) & _lcd_DATA_OUTPUT)|(tmp<<_lcd_DATA_BIT);
			//Set port to output
			_lcd_DATA_DIR|=static_cast<uint8_t>(_lcd_INTERFACE::DATA_MASK);
		}
	}
	//case READ
	if (lcdControlState==_lcd_INTERFACE::CMD_READ)
	{
		// Set RS=0 and WR=1 for command read
		_lcd_RS_OUTPUT&=~(1<<_lcd_RS_BIT);
		_lcd_RW_OUTPUT|=(1<<_lcd_RW_BIT);
	}
	//case READ
	if (lcdControlState==_lcd_INTERFACE::DATA_READ)
	{
		// Set RS=1 and WR=1 for data read
		_lcd_RS_OUTPUT|=(1<<_lcd_RS_BIT);
		_lcd_RW_OUTPUT|=(1<<_lcd_RW_BIT);
	}
	//Tas
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	//E high
	_lcd_E_OUTPUT|=(1<<_lcd_E_BIT);
	//Pweh
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");	
	// read data from interface if the case
	if (lcdControlState==_lcd_INTERFACE::CMD_READ || lcdControlState==_lcd_INTERFACE::DATA_READ)
	{
		//read data according interface
		if (InterfaceSize==_lcd_INTERFACE::SIZE_8_BITS)
		{
			//read data
			*data=(_lcd_DATA_INPUT);
		}
		else
		{
			// read and shift
			tmp=(_lcd_DATA_INPUT & static_cast<uint8_t>(_lcd_INTERFACE::DATA_MASK))>>_lcd_DATA_BIT;
			// put into correctly position
			*data=tmp<<4;
		}
	}
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	//E low
	_lcd_E_OUTPUT&=~(1<<_lcd_E_BIT);
	//Th
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	// set all to pull up
	_lcd_DATA_OUTPUT|=static_cast<uint8_t>(_lcd_INTERFACE::DATA_MASK);
	// set as input
	_lcd_DATA_DIR&=~static_cast<uint8_t>(_lcd_INTERFACE::DATA_MASK);
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	//restore SREG state
	SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Display Interface Initialization									|
//	| @NAME    :  InterfaceInitialize											    |
//	| @INPUT   : 																	|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  Enable And program all used pin of Display						|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::InterfaceInitialize(_lcd_INTERFACE lcdInterfaceType,uint8_t numberOfRows,uint8_t numberOfCols)
{
    uint8_t sreg,tmp;

    //save SREG state and disabled interrupts
    sreg=SREG;
    cli();
    // Set general characteristics
    Rows=numberOfRows;
    Row=1;
    Cols=numberOfCols;
    Col=1;
    // Process according interface type
    switch (static_cast<uint8_t>(lcdInterfaceType))
    {
        //------------------------------
        //| Initialize 4 bits Interface
        //------------------------------
        case static_cast<uint8_t>(_lcd_INTERFACE::SIZE_4_BITS):
            InterfaceSize=lcdInterfaceType;
            //Set data input with pull up 4 bits
            _lcd_DATA_OUTPUT|=static_cast<uint8_t>(_lcd_INTERFACE::DATA_MASK);
            _lcd_DATA_DIR&=~static_cast<uint8_t>(_lcd_INTERFACE::DATA_MASK);
            break;
        //------------------------------
        //| Initialize 8 bits Interface
        //------------------------------
        case static_cast<uint8_t>(_lcd_INTERFACE::SIZE_8_BITS):
            InterfaceSize=lcdInterfaceType;
            //Set data input with pull up 8 bits
            _lcd_DATA_OUTPUT=0xff;
            _lcd_DATA_DIR=0X00;
            break;
    }
    //Set E output low level
    _lcd_E_OUTPUT&=~(1<<_lcd_E_BIT);
    _lcd_E_DIR|=(1<<_lcd_E_BIT);
    //Set RW output high level (read)
    _lcd_RW_OUTPUT|=(1<<_lcd_RW_BIT);
    _lcd_RW_DIR|=(1<<_lcd_RW_BIT);
    //Set RS output high level (command)
    _lcd_RS_OUTPUT|=(1<<_lcd_RS_BIT);
    _lcd_RS_DIR|=(1<<_lcd_RS_BIT);
    //wait power stabilize
	#ifndef __DISPLAY_MODE_TEST__
		delayMs(100);
	#endif
    //send start control initialization
    tmp=static_cast<uint8_t>(_lcd_CMD::FUNCTION_SET) | (1<<static_cast<uint8_t>(_lcd_BIT::DL)) | (1<<static_cast<uint8_t>(_lcd_BIT::N)) | (0<<static_cast<uint8_t>(_lcd_BIT::F));
    Interface(_lcd_INTERFACE::CMD_WRITE,&tmp);
	#ifndef __DISPLAY_MODE_TEST__
		delayMs(8);
	#endif
    tmp=static_cast<uint8_t>(_lcd_CMD::FUNCTION_SET) | (1<<static_cast<uint8_t>(_lcd_BIT::DL)) | (1<<static_cast<uint8_t>(_lcd_BIT::N)) | (0<<static_cast<uint8_t>(_lcd_BIT::F));
    Interface(_lcd_INTERFACE::CMD_WRITE,&tmp);
	#ifndef __DISPLAY_MODE_TEST__
		delayMs(1);
	#endif
    tmp=static_cast<uint8_t>(_lcd_CMD::FUNCTION_SET) | (1<<static_cast<uint8_t>(_lcd_BIT::DL)) | (1<<static_cast<uint8_t>(_lcd_BIT::N)) | (0<<static_cast<uint8_t>(_lcd_BIT::F));
    Interface(_lcd_INTERFACE::CMD_WRITE,&tmp);
	#ifndef __DISPLAY_MODE_TEST__
		delayMs(5);
	#endif
	if (numberOfRows==1)
	{
		CmdWrite(static_cast<uint8_t>(_lcd_CMD::FUNCTION_SET) | (0<<static_cast<uint8_t>(_lcd_BIT::DL)) | (0<<static_cast<uint8_t>(_lcd_BIT::N)) | (0<<static_cast<uint8_t>(_lcd_BIT::F)));
	}
	else
	{
		CmdWrite(static_cast<uint8_t>(_lcd_CMD::FUNCTION_SET) | (0<<static_cast<uint8_t>(_lcd_BIT::DL)) | (1<<static_cast<uint8_t>(_lcd_BIT::N)) | (0<<static_cast<uint8_t>(_lcd_BIT::F)));
	}
    REG2=static_cast<uint8_t>(_lcd_CMD::ENTRY_MODE_SET) | (1<<static_cast<uint8_t>(_lcd_BIT::ID)) | (0<<static_cast<uint8_t>(_lcd_BIT::S));
    CmdWrite(REG2);
    REG3=static_cast<uint8_t>(_lcd_CMD::ON_OFF) | (1<<static_cast<uint8_t>(_lcd_BIT::D)) | (1<<static_cast<uint8_t>(_lcd_BIT::C)) | (0<<static_cast<uint8_t>(_lcd_BIT::B));
    CmdWrite(REG3);
    //restore SREG state
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Display Interface Undo Initialization								|
//	| @NAME    :  InterfaceUnInitialize											    |
//	| @INPUT   : 																	|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  Disable all pins used by the Display Interface					|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::InterfaceUnInitialize()
{
    uint8_t sreg;

    //save SREG state and disabled interrupts
    sreg=SREG;
    cli();

    // Process according interface type
    switch (static_cast<uint8_t>(InterfaceSize))
    {
        //------------------------------
        //| Initialize 4 bits Interface
        //------------------------------
        case static_cast<uint8_t>(_lcd_INTERFACE::SIZE_4_BITS):
            //Set data input hi-impedance 4 bits
            _lcd_DATA_DIR&=~static_cast<uint8_t>(_lcd_INTERFACE::DATA_MASK);
            _lcd_DATA_OUTPUT&=~static_cast<uint8_t>(_lcd_INTERFACE::DATA_MASK);            
        break;
        //------------------------------
        //| Initialize 8 bits Interface
        //------------------------------
        case static_cast<uint8_t>(_lcd_INTERFACE::SIZE_8_BITS):
            //Set data input hi-impedance 8 bits
            _lcd_DATA_DIR=0X00;
            _lcd_DATA_OUTPUT=0x00;
            break;
    }
    //Set E input hi-z
    _lcd_E_DIR&=~(1<<_lcd_E_BIT);
    _lcd_E_OUTPUT&=~(1<<_lcd_E_BIT);
    //Set RW input hi-z
    _lcd_RW_DIR&=~(1<<_lcd_RW_BIT);
    _lcd_RW_OUTPUT&=~(1<<_lcd_RW_BIT);
    //Set RS input hi-z
    _lcd_RS_DIR&=~(1<<_lcd_RS_BIT);
    _lcd_RS_OUTPUT&=~(1<<_lcd_RS_BIT);
    //restore SREG state
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Check if LCD is present											|
//	| @NAME    :  lcdPresenceCheck													|
//	| @INPUT   :  lcdIntefaceType lcd_SIZE_4_BITS or lcd_SIZE_8_BITS				|
//	| @OUTPUT  :  true present, false not present                                   |
//	| @REMARKS :  this function must be used preferred before lcdInit				|
//	+-------------------------------------------------------------------------------+
bool class_HD44780_Drive::PresenceCheck(_lcd_INTERFACE lcdInterfaceType)
{
    uint8_t tmp,a,b;
    //save busy use
    tmp=BusyUse;
    //disable busy
    SetBusyUse(false);
    //Initialize display interface
    InterfaceInitialize(lcdInterfaceType,Rows,Cols);
    //set ddram to 0
    SetDDRAMAddr(0);
    //write 27 18 in consecutive address 0 & 1
    DataWrite(27);
    DataWrite(18);
    //read address 0 & 1 to check
    SetDDRAMAddr(0);
    a=DataRead();
    b=DataRead();
    //Restore busy use value
    SetBusyUse(tmp);
    if (a==27 && b==18)
    {
        //display present
        return true;
    }
    else
    {
        //display not present
        return false;
    }
}

/*
        +-----------------------------------------------+
        |       CONSTRUCTORS AND DESTRUCTORS            |
        +-----------------------------------------------+
*/
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  This constructor not initialize display drive, use constructor    | 
//  |             overload after to correctly initialize display with rows and cols |
//	| @NAME    :  Constructor    													|
//	| @INPUT   :																	|
//	| @OUTPUT  :																	|
//	| @REMARKS :  interrupts are disabled during initialization then restored		|
//	+-------------------------------------------------------------------------------+
class_HD44780_Drive::class_HD44780_Drive()
{
      uint8_t sreg;

      //Save SREG
      sreg=SREG;
      //Disable Interrupts
      cli();
      SetBusyUse(true);
      SetBusyDelay(2);
      //set screen buffer char and pattern to null
      ScrSavePtr=nullptr;
      PatternSavePtr=nullptr;
      //restore SREG state
      SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize HD44780 DisplayDrive									|
//	| @NAME    :  Constructor														|
//	| @INPUT   :  lcdInterface lcd_SIZE_4_BITS or lcd_SIZE_8_BITS					|
//  |             numberOfRows, numnberOfCols                                       |
//	| @OUTPUT  :                                                                    |
//	| @REMARKS :  interrupts are disabled during initialization then restored		|
//	+-------------------------------------------------------------------------------+

class_HD44780_Drive::class_HD44780_Drive(_lcd_INTERFACE lcdInterfaceType,uint8_t numberOfRows,uint8_t numberOfCols)
{
    uint8_t sreg;

    //Save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
    SetBusyUse(false);
    SetBusyDelay(20);
    //Initialize interface
    InterfaceInitialize(lcdInterfaceType,numberOfRows,numberOfCols);
    //first check if present
    PresentFlag=PresenceCheck(lcdInterfaceType);
    if (PresentFlag==true)
    {
        //Clear screen and set number of rows and cols
        Cls();
    }
    //set screen buffer char and pattern to null
    ScrSavePtr=nullptr;
    PatternSavePtr=nullptr;
    SetBusyDelay(2);
    SetBusyUse(true);
    //restore SREG state
    SREG=sreg;
    //return lcd present status
    //return PresentFlag;
}


//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  UnInitialize HD44780 DisplayDrive									|
//	| @NAME    :  Destructor														|
//	| @INPUT   :                                                            		|
//	| @OUTPUT  :                                                                    |
//	|	                                                                            |
//	| @REMARKS :  Release memory and set pointer null                               |
//	+-------------------------------------------------------------------------------+
class_HD44780_Drive::~class_HD44780_Drive()
{   
    //free memory areas
    free(ScrSavePtr);
    free(PatternSavePtr);
    //set screen buffer char and pattern to null
    ScrSavePtr=nullptr;
    PatternSavePtr=nullptr;
}

/*
        +-----------------------------------------------+
        |       PUBLIC FUNCTIONS                        |
        +-----------------------------------------------+
*/
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Prepare Display to enter in sleep		                            |
//	| @NAME    :  PrepareForSleep                    	                            |
//	| @INPUT   :  none                                                              |
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_HD44780_Drive::PrepareForSleep()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //Disable interrupts
    cli();
    //UnInitialize local I/O
    InterfaceUnInitialize();
    //restore SREG
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Restore Display from sleep state					                |
//	| @NAME    :  RestoreFromSleep                    	                            |
//	| @INPUT   :  none                                                              |
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_HD44780_Drive::RestoreFromSleep()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //Disable interrupts
    cli();
    //restore I/O from sleep
	#ifndef __DISPLAY_MODE_TEST__
		delayMs(50);
	#endif
    InterfaceInitialize(InterfaceSize,Rows,Cols);
	#ifndef __DISPLAY_MODE_TEST__
		delayMs(50);
	#endif
    //first check if present
    //restore SREG
    SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Read Address Count												|
//	| @NAME    :  lcdCmdRead														|
//	| @INPUT   :  																	|
//	| @OUTPUT  :  Address read bits 0..6 and busy state bit 7						|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
uint8_t class_HD44780_Drive::CmdRead()
{
uint8_t tmp1,tmp2;

	Interface(_lcd_INTERFACE::CMD_READ,&tmp1);

	// check if 4 bits interface
	if (InterfaceSize==_lcd_INTERFACE::SIZE_4_BITS)
	{
		// yes, read again
		Interface(_lcd_INTERFACE::CMD_READ,&tmp2);
		// return addr from 4 bits interface
		return (tmp1 & 0x0f0) | (tmp2>>4);
	}
	else
	{
		//else return in 8 bits interface
		return tmp1;
	}
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  check if display is busy											|
//	| @NAME    :  lcdBusy															|
//	| @INPUT   :  																	|
//	| @OUTPUT  :  return true if busy												|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
bool class_HD44780_Drive::Busy()
{
	#ifdef __DISPLAY_MODE_TEST__
		return false;
	#endif
	// check busy?
	if (BusyUse)
	{
		//yes, read
		if (CmdRead() & (1<<static_cast<uint8_t>(_lcd_BIT::BUSY)))
			return true;
		else
			return false;
	}
	else
	{
		//no, wait a busy delay
		delayMs(BusyDelay);
		// return not busy
		return false;
	}
}	

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Write a command into display										|
//	| @NAME    :  lcdCmdWrite														|
//	| @INPUT   :  data to be write													|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::CmdWrite(uint8_t data)
{

	// wait exit busy 
	while (Busy());
	// send data
	Interface(_lcd_INTERFACE::CMD_WRITE,&data);

	// check if 4 bits interface send low part

	if (InterfaceSize==_lcd_INTERFACE::SIZE_4_BITS)
	{
		// yes, read again
		data<<=4;
		Interface(_lcd_INTERFACE::CMD_WRITE,&data);
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Write a data on display											|
//	| @NAME    :  lcdDataWrite														|
//	| @INPUT   :  data to be write													|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::DataWrite(uint8_t data)
{
	// wait exit busy 
	while (Busy());
	// send data
	Interface(_lcd_INTERFACE::DATA_WRITE,&data);

	// check if 4 bits interface send low part
	if (InterfaceSize==_lcd_INTERFACE::SIZE_4_BITS)
	{
		// yes, read again
		data<<=4;
		Interface(_lcd_INTERFACE::DATA_WRITE,&data);
	}
}	

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Read a data from display											|
//	| @NAME    :  lcdDataRead														|
//	| @INPUT   :  																	|
//	| @OUTPUT  :  data																|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
uint8_t class_HD44780_Drive::DataRead()
{
uint8_t tmp1,tmp2;

	// wait exit busy 
	while (Busy());

	// read data
	Interface(_lcd_INTERFACE::DATA_READ,&tmp1);

	// check if 4 bits interface
	if (InterfaceSize==_lcd_INTERFACE::SIZE_4_BITS)
	{
		// yes, read again
		Interface(_lcd_INTERFACE::DATA_READ,&tmp2);
		// return addr from 4 bits interface
		return (tmp1 & 0x0f0) | (tmp2>>4);
	}
	else
	{
		//else return in 8 bits interface
		return tmp1;
	}
}


//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Clear Display Screen												|
//	| @NAME    :  lcdCLS															|
//	| @INPUT   : 																	|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  Set Display Row to 1 and Col to 1									|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::Cls()
{
	CmdWrite(static_cast<uint8_t>(_lcd_CMD::CLEAR));
	Col=1;
	Row=1;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Display Home, Cursor is positionated in col=1						|
//	| @NAME    :  lcdHome															|
//	| @INPUT   : 																	|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  Set Display Col to 1												|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::Home()
{
	CmdWrite(static_cast<uint8_t>(_lcd_CMD::HOME));
	Col=1;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Scroll all characters in display to left							|
//	| @NAME    :  lcdScrollLeft														|
//	| @INPUT   : 																	|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::ScrollLeft()
{
	CmdWrite(static_cast<uint8_t>(_lcd_CMD::SHIFT) | (1<<static_cast<uint8_t>(_lcd_BIT::SC)) | (0<<static_cast<uint8_t>(_lcd_BIT::RL)));
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Scroll all characters in display to right							|
//	| @NAME    :  lcdScrollRight													|
//	| @INPUT   : 																	|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::ScrollRight()
{
	CmdWrite(static_cast<uint8_t>(_lcd_CMD::SHIFT) | (1<<static_cast<uint8_t>(_lcd_BIT::SC)) | (1<<static_cast<uint8_t>(_lcd_BIT::RL)));
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Set Cursor Visibility state										|
//	| @NAME    :  lcdSetCursorState													|
//	| @INPUT   :  state=false Invisible else Visible								|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::SetCursorState(bool cursorState)
{
	if (cursorState)
	{
		REG3|=(1<<static_cast<uint8_t>(_lcd_BIT::C));
	}
	else
	{
		REG3&=~(1<<static_cast<uint8_t>(_lcd_BIT::C));
	}
	//send command to display
	CmdWrite(REG3);
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Move Cursor to left												|
//	| @NAME    :  lcdMoveCursorToLeft												|
//	| @INPUT   :  																	|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::MoveCursorToLeft()
{
	CmdWrite(static_cast<uint8_t>(_lcd_CMD::SHIFT) | (0<<static_cast<uint8_t>(_lcd_BIT::SC)) | (0<<static_cast<uint8_t>(_lcd_BIT::RL)));
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Move Cursor to Right												|
//	| @NAME    :  lcdMoveCursorToRight												|
//	| @INPUT   :  																	|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::MoveCursorToRight()
{
	CmdWrite(static_cast<uint8_t>(_lcd_CMD::SHIFT) | (0<<static_cast<uint8_t>(_lcd_BIT::SC)) | (1<<static_cast<uint8_t>(_lcd_BIT::RL)));
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Move Cursor to Right												|
//	| @NAME    :  lcdMoveCursorToRight												|
//	| @INPUT   :  blinkState=true Blink blinkState=false NotBlink					|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::SetCursorBlinkState(bool blinkState)
{
	if (blinkState)
	{
		REG3|=(1<<static_cast<uint8_t>(_lcd_BIT::B));
	}
	else
	{
		REG3&=~(1<<static_cast<uint8_t>(_lcd_BIT::B));
	}
	//send command to display
	CmdWrite(REG3);
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Locate display at position of row and col							|
//	| @NAME    :  lcdLocate															|
//	| @INPUT   :  Row & Col Row=1..NumberRows, Col=1 to NumberCols					|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void class_HD44780_Drive::Locate(uint8_t row,uint8_t col)
{
uint8_t tmp;

	if (row>=1 && row<=Rows && col>=1 && col<=Cols)
	{
		Row=row;
		Col=col;
		row--;
		col--;
		tmp=static_cast<uint8_t>(_lcd_CMD::DDRAM_ADDR) + (col & 0x3f);
		if (row!=0) tmp+=0x40;
		CmdWrite(tmp);
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Send char to display sram											|
//	| @NAME    :  lcdPut															|
//	| @INPUT   :  character character to be displayed								|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::PutC(char character)
{
	DataWrite(character);
	Col++;
	if (Col>Cols)
	{
		Col=1;
		Row++;
		Locate(Row,Col);
	}
}


//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Display string locate into sram									|
//	| @NAME    :  lcdPuts															|
//	| @INPUT   :  *msg string into sram												|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::PutS(char *string)
{
	  uint8_t data;
	  //send until zero is found
	  for(;;)
	  {
        //get char for sram
        	  data=*string;
        //check if end
    	  if (data==0) break;
        //process according data
    	  switch (data)
    	  {
        	  //new line ?
        	  case '\n':
        	      Row++;
        	      Locate(Row,Col);
        	      break;
        	  //carriage return?
        	  case '\r':
        	      Col=1;
        	      Locate(Row,Col);
        	      break;
        	  //other char?
        	  default:
        	     PutC(data);
        	     break;
    	  }
    	  //next char
    	  string++;
	  }
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Display string locate into FLASH									|
//	| @NAME    :  lcdPutsF															|
//	| @INPUT   :  *msg string into sram												|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::PutSF(const char *string)
{
	  uint8_t data;
	  //send until zero is found
	  for(;;)
	  {
        //get data form progmem
            data=pgm_read_byte(string);
        //check if end
    	  if (data==0) break;
        //process according data
    	  switch (data)
    	  {
        	  //new line ?
        	  case '\n':
        	      Row++;
        	      Locate(Row,Col);
        	      break;
        	  //carriage return?
        	  case '\r':
        	      Col=1;
        	      Locate(Row,Col);
        	      break;
        	  //other char?
        	  default:
        	     PutC(data);
        	     break;
    	  }
    	  //next char
    	  string++;
	  }
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Set CGRAM Address													|
//	| @NAME    :  lcdSetCGRAMAddr													|
//	| @INPUT   :  addr	sgram address												|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::SetCGRAMAddr(uint8_t addr)
{
	CmdWrite(static_cast<uint8_t>(_lcd_CMD::CGRAM_ADDR) | (addr & 0x3f));
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Set DDRAM Address													|
//	| @NAME    :  lcdSetDDRAMAddr													|
//	| @INPUT   :  addr	ddram address												|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::SetDDRAMAddr(uint8_t addr)
{
	CmdWrite(static_cast<uint8_t>(_lcd_CMD::DDRAM_ADDR) | (addr & 0x7f));
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Redefine 0..7 characters											|
//	| @NAME    :  lcdRedefineChar													|
//	| @INPUT   :  char	character code 0..7											|
//	|	     *fString	flash string with 8 sequential definitions byte				|
//	| @OUTPUT  : 																	|
//	| @REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::RedefineChar(uint8_t character,const char *fString)
{
uint8_t i;
	// set initial address of character to be redefined
	SetCGRAMAddr(character<<3);
	// redefine character
	for(i=0;i<8;i++)
	{
		//get character from flash and redefine
		DataWrite(pgm_read_byte(fString));
		fString++;
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Initialize Big Numbers												|
//	| NAME    :  lcdBigInit															|
//	| INPUT   :  																	|
//	| OUTPUT  :  ----																|
//	| REMARKS :  Interrupts are disabled during initialization						|
//	+-------------------------------------------------------------------------------+

void	class_HD44780_Drive::LCDBigInit()
{
    uint8_t sreg;
    //save SREG and disable interrupts during initialization
    sreg=SREG;
    cli();
    //lcd.PutSF((const char*)&toto[0]);
    RedefineChar(0,(const char*)&_lcd_BigNumber0);
    RedefineChar(1,(const char*)&_lcd_BigNumber1);
    RedefineChar(2,(const char*)&_lcd_BigNumber2);
    RedefineChar(3,(const char*)&_lcd_BigNumber3);
    RedefineChar(4,(const char*)&_lcd_BigNumber4);
    RedefineChar(5,(const char*)&_lcd_BigNumber5);
    RedefineChar(6,(const char*)&_lcd_BigNumber6);
    RedefineChar(7,(const char*)&_lcd_BigNumber7);
    // restore SREG state
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Print big number at column N character								|
//	| NAME    :  PutCBig															|
//	| INPUT   :  																	|
//	| OUTPUT  :  ----																|
//	| REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::PutCBig(char asciiDigit)
{

    // check if space
    if (asciiDigit==' ')
    {
        Locate(1,Col);
		DataWrite(' ');
        Locate(2,Col);
		DataWrite(' ');
        Col++;
    }
    else
    {
        // check if valid digit
        if (asciiDigit>='0' && asciiDigit<='9')
        {
            // 1st locate at 1 row and col
            Locate(1,Col);
            // convert ascii to number and get array index m for ROW 1
            DataWrite(pgm_read_byte(&_lcd_BigRow1[asciiDigit & 0x0f]));
            // convert ascii to number and get array index m for ROW 2
            Locate(2,Col);
            DataWrite(pgm_read_byte(&_lcd_BigRow2[asciiDigit & 0x0f]));
            Col++;
        }
    }
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Print big number at column N string								|
//	| NAME    :  PutSBig															|
//	| INPUT   :  string into sram													|
//	| OUTPUT  :  ----																|
//	| REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::PutSBig(char *string)
{
    //main string puts loop
    for(;;)
    {
        //exit if 0 end
        if (*string==0) break;
        //print big number
        PutCBig(*string++);
    }
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Save character on screen into buffer         						|
//	| NAME    :  ScrSave															|
//	| INPUT   :                             										|
//	| OUTPUT  :  ----																|
//	| REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::ScrSave()
{
    uint8_t *p;

    //memory already allocated?
    if (ScrSavePtr==nullptr)
    {
        //no, allocate then
        ScrSavePtr=malloc(Rows*Cols);
        //return if error when alloc memory
        if (ScrSavePtr==nullptr) return;
    }
    //save screen data
    p=(uint8_t*)ScrSavePtr;

    for(uint8_t i=1;i<=Rows;i++)
    {
        for(uint8_t j=1;j<=Cols;j++)
        {
            Locate(i,j);
            *p=DataRead();
            p++;
        }
    }
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Restore character form buffer                 						|
//	| NAME    :  ScrRestore															|
//	| INPUT   :                             										|
//	| OUTPUT  :  ----																|
//	| REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::ScrRestore()
{
   uint8_t *p;

   //return if error when alloc memory

   if (ScrSavePtr==nullptr) return;

   //restore screen data
   p=(uint8_t*)ScrSavePtr;

   for(uint8_t i=1;i<=Rows;i++)
   {
       for(uint8_t j=1;j<=Cols;j++)
       {
           Locate(i,j);
           DataWrite(*p);
           p++;
       }
   }
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Save pattern character into buffer         						|
//	| NAME    :  PatternSave														|
//	| INPUT   :                             										|
//	| OUTPUT  :  ----																|
//	| REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::PatternSave()
{
  uint8_t *p;

    //memory already allocated?
    if (PatternSavePtr==nullptr)
    {
        //no, allocate then
        PatternSavePtr=malloc(64);
        //return if error when alloc memory
        if (PatternSavePtr==nullptr) return;
    }

    //restore pattern data
    p=(uint8_t*)PatternSavePtr;

    for(uint8_t i=0;i<64;i++)
    {
        SetCGRAMAddr(i);
        *p=DataRead();
        p++;
    }   

}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Restore pattern character from buffer         						|
//	| NAME    :  PatternRestore														|
//	| INPUT   :                             										|
//	| OUTPUT  :  ----																|
//	| REMARKS :  																	|
//	+-------------------------------------------------------------------------------+
void	class_HD44780_Drive::PatternRestore()
{
    uint8_t *p;

    //return if error when alloc memory

    if (PatternSavePtr==nullptr) return;

    //restore pattern data
    p=(uint8_t*)PatternSavePtr;

    for(uint8_t i=0;i<64;i++)
    {
        SetCGRAMAddr(i);
        DataWrite(*p);
        p++;
    }
}
