/*
	//////////////////////////////////////////////////////////////////////////////////////////

			______            _   _____ _                       ______  _____
			|  ___|          | | |_   _(_)                     |___  / /  ___|
			| |_ ___  ___  __| |   | |  _ _ __ ___   ___ _ __     / /  \ `--.  ___  __ _
			|  _/ _ \/ _ \/ _` |   | | | | '_ ` _ \ / _ \ '__|   / /    `--. \/ _ \/ _` |
			| ||  __/  __/ (_| |   | | | | | | | | |  __/ |    ./ /    /\__/ /  __/ (_| |
			\_| \___|\___|\__,_|   \_/ |_|_| |_| |_|\___|_|    \_/     \____/ \___|\__, |
																					__/ |
																					|___/

    //////////////////////////////////////////////////////////////////////////////////////////
    ------------------------------  FEED TIMER HEADER FILE (tab 4)-----------------------------
*/

#ifndef _FEED_TIMER_DISP_7_SEG_IO_H_
#define _FEED_TIMER_DISP_7_SEG_IO_H_

/*
 +--------------------------------------+
 |        HARDWARE DEPENDENT INCLUDES	|
 +--------------------------------------+
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

/*
 +--------------------------------------+
 |        C LIBRARY INCLUDES		    |
 +--------------------------------------+
*/
#include <string.h>
/*
 +--------------------------------------+
 |        LOCAL LIBRARY INCLUDES	    |
 +--------------------------------------+
*/
#include "_Time_Lib.h"
/*
 +--------------------------------------+
 |        TYPE DEFINITIONS		        |
 +--------------------------------------+
*/

//encoder ports and bits definition

#
/*
 +--------------------------------------+
 |        CONSTANT DEFINITIONS		    |
 +--------------------------------------+
*/

#define _FEED_TIMER_DISP_7_SEG_SEG1_PORT_OUTPUT		PORTD
#define _FEED_TIMER_DISP_7_SEG_SEG1_PORT_DIR		DDRD
#define _FEED_TIMER_DISP_7_SEG_SEG1_PORT_INPUT		PIND

#define _FEED_TIMER_DISP_7_SEG_SEG1_P_BIT				3
#define _FEED_TIMER_DISP_7_SEG_SEG1_C_BIT				4

#define _FEED_TIMER_DISP_7_SEG_SEG2_PORT_OUTPUT		PORTD
#define _FEED_TIMER_DISP_7_SEG_SEG2_PORT_DIR		DDRD
#define _FEED_TIMER_DISP_7_SEG_SEG2_PORT_INPUT		PIND

#define _FEED_TIMER_DISP_7_SEG_SEG2_F_BIT				0
#define _FEED_TIMER_DISP_7_SEG_SEG2_A_BIT				1
#define _FEED_TIMER_DISP_7_SEG_SEG2_B_BIT				2
#define _FEED_TIMER_DISP_7_SEG_SEG2_G_BIT				5
#define _FEED_TIMER_DISP_7_SEG_SEG2_D_BIT				6
#define _FEED_TIMER_DISP_7_SEG_SEG2_E_BIT				7

#define _FEED_TIMER_DISP_7_SEG_DIGITS_PORT_OUTPUT		PORTC
#define _FEED_TIMER_DISP_7_SEG_DIGITS_PORT_DIR			DDRC
#define _FEED_TIMER_DISP_7_SEG_DIGITS_PORT_INPUT		PINC

#define _FEED_TIMER_DISP_7_SEG_DIGITS_1_BIT			0
#define _FEED_TIMER_DISP_7_SEG_DIGITS_2_BIT			1
#define _FEED_TIMER_DISP_7_SEG_DIGITS_3_BIT			2
#define _FEED_TIMER_DISP_7_SEG_DIGITS_4_BIT			3

#define _FEED_TIMER_DISP_7_SEG_KEY_PORT_OUTPUT		PORTB
#define _FEED_TIMER_DISP_7_SEG_KEY_PORT_DIR			DDRB
#define _FEED_TIMER_DISP_7_SEG_KEY_PORT_INPUT		PINB

#define _FEED_TIMER_DISP_7_SEG_KEY_DOWN_BIT			2
#define _FEED_TIMER_DISP_7_SEG_KEY_UP_BIT			1
#define _FEED_TIMER_DISP_7_SEG_KEY_ENTER_BIT		0

#define _FEED_TIMER_DISP_7_SEG_MOTOR_PORT_OUTPUT	PORTC
#define _FEED_TIMER_DISP_7_SEG_MOTOR_PORT_DIR		DDRC
#define _FEED_TIMER_DISP_7_SEG_MOTOR_PORT_INPUT		PINC

#define _FEED_TIMER_DISP_7_SEG_MOTOR_BIT			4

#define _FEED_TIMER_DISP_7_ADC_OUTPUT				PORTC
#define _FEED_TIMER_DISP_7_ADC_DIR					DDRC
#define _FEED_TIMER_DISP_7_ADC_INPUT				PINC

#define _FEED_TIMER_DISP_7_ADC_BIT					5

typedef enum class _enum_KEYCODES
{
 	NONE,
	DOWN,
	UP,
	ENTER,	
	DOWN_OVER,
	UP_OVER,
	ENTER_OVER,
	DOWN_REPEAT,
	UP_REPEAT,	
	ENTER_REPEAT
} KeyCode;
/*
 +--------------------------------------+
 |        DISPLAY SPI STRUCTS         	|
 +--------------------------------------+
*/



/*
	+--------------------------------------+
	|       EXTERNAL DECLARATIONS          |
	+--------------------------------------+
*/
#if defined(_AVR_IOM8_H_)
	extern "C" void TIMER2_COMP_vect(void) __attribute__ ((signal));
#elif defined(_AVR_IOM328P_H_)
	extern "C" void TIMER2_COMPA_vect(void) __attribute__ ((signal));
#endif
						 
/*
        +-------------------------------------------------------+
        |        CLASS			DISPLAY SPI                     |
        +-------------------------------------------------------+
        | @PURPOSE      : Set of function for send and receive	|
		|				 data from/to display of 7 segments		|
		|				 6 digits using SPI line				|
        | @OBSERVATIONS : use Timer1 as resource				|
        +-------------------------------------------------------+
*/

class class_Display_7_Seg
{
/*
         +--------------------------------------+
         |       PRIVATE VARIABLES		        |
         +--------------------------------------+
        */
 private:
        class_Time				Time;    
        /*
         +--------------------------------------+
         |       PRIVATE FUNCTIONS      	    |
         +--------------------------------------+
        */
		#if defined(_AVR_IOM8_H_)
			friend void    TIMER2_COMP_vect(void);
		#elif defined(_AVR_IOM328P_H_)
			friend void		TIMER2_COMPA_vect(void);
		#endif		  
		void	KeysInitialize(void);
		void	DisplaysInitialize(void);
		void	MotorInitialize(void);
		void	KeysUnInitialize(void);
		void	DisplaysUnInitialize(void);
		void	MotorUnInitialize(void);
		void	Timer2Initialize(void);
		void    Initialize(void);
        void    UnInitialize(void);
        /*
         +--------------------------------------+
         |        PUBLIC FUNCTIONS		        |
         +--------------------------------------+
        */
public:
        /************************************************************************/
        /* Constructor                                                          */
        /************************************************************************/
        class_Display_7_Seg(void);
        /************************************************************************/
        /* Set segments (segmentBits) at digit                                  */
        /************************************************************************/
		void	DispSetSegments(uint8_t segmentBits,uint8_t digit);
		/************************************************************************/
		/* Set Display decimal point at digit ON=true OFF=false                 */
		/************************************************************************/
        void    DispSetPoint(uint8_t digit,bool digitState);
		/************************************************************************/
		/* Set ASCII character code at digit                                    */
		/************************************************************************/
		void	DispSetASCII(uint8_t asciiCode,uint8_t digit);
		/************************************************************************/
		/* Clear Display Screen                                                 */
		/************************************************************************/
		void	Cls(void);
		/************************************************************************/
		/* Set Cursor to Home locate col=1                                      */
		/************************************************************************/
		void	Home(void);
		/************************************************************************/
		/* Locate cursor at display column                                      */
		/************************************************************************/
		void	Locate(uint8_t dispCol);
		/************************************************************************/
		/* Put Character on display at column defined by Locate function        */
		/************************************************************************/
		void	PutC(char asciiCode);
		/************************************************************************/
		/* Put String on display at column defined by locate function           */
		/************************************************************************/
		void	PutS(const char *asciiString);
		/************************************************************************/
		/* Put String form FLASH on display at column defined by locate function*/
		/************************************************************************/
		void	PutSF(const char *asciiSTringFlash);
		/************************************************************************/
		/* Read keyboard without wait, return none if no keys pressed           */
		/* returned codes ara: KEY_UP,KEY_DOWN,KEY_ENTER,KEY_NONE				*/
		/************************************************************************/
		KeyCode	KeyReadFlash(void);
		/************************************************************************/
		/* Wait keyboard keys release											*/
		/************************************************************************/
		void	KeyWaitRelease(void);
		/************************************************************************/
		/* Read keyboard and wait, returned codes ara: KEY_UP,KEY_DOWN,KEY_ENTER*/
		/*											   KEY_NONE					*/
		/************************************************************************/
		KeyCode	KeyRead(void);
		/************************************************************************/
		/* Turn Feed Motor ON                                                   */
		/************************************************************************/
		void	inline MotorON(void)
		{
			//PUT MOTOR PORT TO OUTPUT DIRECTION TO ENABLE MOTOR
			_FEED_TIMER_DISP_7_SEG_MOTOR_PORT_DIR|=(1<<_FEED_TIMER_DISP_7_SEG_MOTOR_BIT);
		}
		/************************************************************************/
		/* Turn Feed Motor OFF                                                  */
		/************************************************************************/
		void	inline MotorOFF(void)
		{
			//PUT MOTOR PORT TO INPUT HI-Z DO DISABLE MOTOR
			_FEED_TIMER_DISP_7_SEG_MOTOR_PORT_DIR&=~(1<<_FEED_TIMER_DISP_7_SEG_MOTOR_BIT);
		}
		/************************************************************************/
		/* Get Motor State i.e. true=ON,false=OFF                               */
		/************************************************************************/
		bool MotorGetState(void);
		/************************************************************************/
		/* Put Feed Timer HARDWARE to sleep (POWER_SAVE MODE)                   */
		/************************************************************************/
		void	PutToSleep(void);
		/************************************************************************/
		/* Set call back function to be called each second                      */
		/************************************************************************/
		void	SetSecondCallBack(void (*callbackfunction)(void));
		/************************************************************************/
		/* Disable IO putting all used pins to hiz state                        */
		/* call this function only before sleep procedure						*/
		/************************************************************************/
		void	DisableIO(void);
		/************************************************************************/
		/* Enable IO putting all used pin to setting configuration              */
		/************************************************************************/
		void	EnableIO(void);
		/************************************************************************/
		/* Initialize ADC with prescaler factor                                 */
		/************************************************************************/
		void		ADCInitialize(uint8_t prescalerFactor);
		/************************************************************************/
		/* Get ADC Value                                                        */
		/************************************************************************/
		uint16_t	ADCGetValue(void);
		
		int32_t		Range(int32_t valueIn,int32_t rangeInMin,int32_t rangeInMax,int32_t rangeOutMin,int32_t rangeOutMax);
};

 

#endif

