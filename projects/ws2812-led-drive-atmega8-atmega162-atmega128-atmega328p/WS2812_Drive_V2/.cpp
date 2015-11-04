/*
    //////////////////////////////////////////////////////////////////////////////////////////
                     _    _ _____  _____  _____  __   _____  ______      _
                    | |  | /  ___|/ __  \|  _  |/  | / __  \ |  _  \    (_)
                    | |  | \ `--. `' / /' \ V / `| | `' / /' | | | |_ __ ___   _____
                    | |/\| |`--. \  / /   / _ \  | |   / /   | | | | '__| \ \ / / _ \
                    \  /\  /\__/ /./ /___| |_| |_| |_./ /___ | |/ /| |  | |\ V /  __/
                     \/  \/\____/ \_____/\_____/\___/\_____/ |___/ |_|  |_| \_/ \___|
        
    //////////////////////////////////////////////////////////////////////////////////////////
    // This Drive running on ATMEGA8,ATMEGA162,ATMEGA128 and atmega328p devices             //
    // Some function need global interrupts must enabled to work properly                   //
    // Compile this library using optimization -os or -o1                                   //
	//////////////////////////////////////////////////////////////////////////////////////////
	------------------------------- Graph Basic Functions C++ (tab 4)-------------------------		  
*/


//     +--------------------------------------+
//     |        LOCAL LIBRARY INCLUDES	      |
//     +--------------------------------------+
//

    #include "_WS2812_Drive.h"

// 
//     +--------------------------------------+
//     |        LOCAL FUNCTIONS CONSTANTS     |
//     +--------------------------------------+
// 

//
//     +--------------------------------------+
//     |        LOCAL STRUCTS                 |
//     +--------------------------------------+
//

// 
//         +-------------------------------------------------------+
//         |        CLASS WS2812 DRIVE IMPLEMENTATION              |
//         +-------------------------------------------------------+
//         | @PURPOSE      : Implements virtual functions from     |
//         |                 graphbasic                            |
//         | @OBSERVATIONS :									   |
//         +-------------------------------------------------------+
//
// 
//         +-----------------------------------------------+
//         |       PRIVATE FUNCTIONS IMPLEMENTATION        |
//         +-----------------------------------------------+
// 

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic initialization 				                        |
//	| NAME    :  Initialize								                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
void class_WS2812_Drive::Initialize()
{
    unsigned char sreg;

    //Save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
    //set DIN bit as output high
    _WS2812_PORT_OUTPUT|=(1<<_WS2812_DIN_BIT);
    _WS2812_PORT_DIR|=(1<<_WS2812_DIN_BIT);
    //Restore Interrupts
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic Unitialization         		                        |
//	| NAME    :  UnInitialize						                                |
//	| INPUT   :  none							                                    |
//	| OUTPUT  :  none							                                    |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void class_WS2812_Drive::UnInitialize()
{
    unsigned char sreg;

    //Save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
    //set DIN bit as input high z
    _WS2812_PORT_DIR&=~(1<<_WS2812_DIN_BIT);
    _WS2812_PORT_OUTPUT&=~(1<<_WS2812_DIN_BIT);
    //Restore Interrupts
    SREG=sreg;
}

// 
//         +-----------------------------------------------+
//         |       CONSTRUCTORS AND DESTRUCTORS            |
//         +-----------------------------------------------+
// 

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic initialization with rows=1,cols=1                      |
//	| NAME    :  timeInit								                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
class_WS2812_Drive::class_WS2812_Drive()
:class_GraphBasic()
{
	unsigned char sreg;

    //save SREG
	sreg=SREG;
    //Disable Interrupts
	cli();
    //Initialize
    Initialize();
    //Restore SREG state
	SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic initialization	    				                    |
//	| NAME    :  Graph Basic Init overload 2 				                        |
//	| INPUT   :  rows,cols with rows and cols		                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
class_WS2812_Drive::class_WS2812_Drive(int rows,int cols)
:class_GraphBasic(rows,cols)
{
    unsigned char sreg;

    //save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
    //Initialize
    Initialize();
    //Restore SREG state
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  time engine initialization						                    |
//	| NAME    :  timeInit								                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
class_WS2812_Drive::~class_WS2812_Drive()
{
    unsigned char sreg;

    //save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
    //Initialize
    UnInitialize();
    //Restore SREG state
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Reset WS2812 LED Stream                                           	|
//	| NAME    :  SetTime       	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  Interrupts are disabled                     		                |
//	+-------------------------------------------------------------------------------+
void    class_WS2812_Drive::Reset()
{
    unsigned int timeCount;
    cli();
    //pulse reset low 60 microseconds
    _WS2812_PORT_OUTPUT&=~(1<<_WS2812_DIN_BIT);
    timeCount=(F_CPU*.060)/4000;
    asm volatile("movw   r24,%A0	    \t\n"	\
                 "1:                    \t\n"   \
                 "sbiw r24,1		    \t\n"	\
                 "brne 1b               \t\n"   \
                 :"+r" (timeCount)
                 :
                 :"r24","r25"
                 );
    
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Write r,g,b values to a ws2812 LED                               	|
//	| NAME    :  WriteRGBttoDevice        					                        |
//	| INPUT   :  red,green,blue values range 0..255			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  Interrupts are disabled and restore        		                |
//	+-------------------------------------------------------------------------------+
void    class_WS2812_Drive::WriteRGBtoDevice(uint8_t rValue,uint8_t gValue,uint8_t bValue)
{
	//save SREG
	uint8_t sreg=SREG;
	//clear interrupts
	cli();
    //green
    asm volatile(\
        "mov    r16,%[gV]                   \t\n"\
        "mov    r17,%[rV]                   \t\n"\
        "mov    r18,%[bV]                   \t\n"\
        "ldi    r19,24                      \t\n"\
    "0:                                     \t\n"\
        "sbi    %[portDin],%[portDinBit]    \t\n"\
        "rol    r18                         \t\n"\
        "rol    r17                         \t\n"\
        "rol    r16                         \t\n"\
        "brcs   1f                          \t\n"\
        "cbi    %[portDin],%[portDinBit]    \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "dec    r19                         \t\n"\
        "brne   0b                          \t\n"\
        "rjmp   2f                          \t\n"\
    "1:                                     \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "cbi    %[portDin],%[portDinBit]    \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "nop                                \t\n"\
        "dec    r19                         \t\n"\
        "brne   0b                          \t\n"\
    "2:                                     \t\n"\
        "nop                                \t\n"\
    :
    : [gV] "r" (gValue),
      [rV] "r" (rValue),
      [bV] "r" (bValue),
      [portDin] "I" (_SFR_IO_ADDR(_WS2812_PORT_OUTPUT)),
      [portDinBit] "I" (_WS2812_DIN_BIT)
    : "r16","r17","r18","r19"
    );
    //restore SREG
	SREG=sreg;
}
