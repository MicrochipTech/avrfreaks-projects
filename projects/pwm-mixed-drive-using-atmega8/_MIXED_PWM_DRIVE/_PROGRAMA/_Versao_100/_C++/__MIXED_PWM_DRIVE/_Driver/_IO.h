/*
	/////////////////////////////////////////////////////////////////////////////////////////
			 _   _  _   _ _____  ______      _             _____ _____
			| | | || | | |  _  | |  _  \    (_)           |_   _|  _  |
			| |_| || |_| | | | | | | | |_ __ ___   _____    | | | | | |
			|  _  ||  _  | | | | | | | | '__| \ \ / / _ \   | | | | | |
			| | | || | | \ \_/ / | |/ /| |  | |\ V /  __/  _| |_\ \_/ /
			\_| |_/\_| |_/\___/  |___/ |_|  |_| \_/ \___|  \___/ \___/

	/////////////////////////////////////////////////////////////////////////////////////////
	---------------------------------- I/O  C++ Header File (tab 4) ------------------------
*/

#ifndef _IO_H_
#define _IO_H_

/*
     +--------------------------------------+
     |        HARDWARE DEPENDENT INCLUDES	|
     +--------------------------------------+
*/

#include <avr/io.h>
#include <avr/interrupt.h>

/*
	+--------------------------------------+
	|       LIBRARY INCLUDES               |
	+--------------------------------------+
*/

/*
 +--------------------------------------+
 |        TYPE DEFINITIONS		        |
 +--------------------------------------+
*/

//encoder ports and bits definition


/*
 +--------------------------------------+
 |        CONSTANT DEFINITIONS		    |
 +--------------------------------------+
*/


#define _IO_KEY_PORT_OUTPUT			PORTC
#define _IO_KEY_PORT_DIR			DDRC
#define _IO_KEY_PORT_INPUT			PINC

#define _IO_KEY_UP_BIT				1
#define _IO_KEY_ENTER_BIT			0

#define _IO_ADC_PORT_OUTPUT			PORTC
#define _IO_ADC_PORT_DIR			DDRC
#define _IO_ADC_PORT_INPUT			PINC

#define _IO_ADC_CHANNEL_FQ1			0
#define _IO_ADC_CHANNEL_FS1			1
#define _IO_ADC_CHANNEL_FQ2			2
#define _IO_ADC_CHANNEL_FS2			3

#define _IO_KEY_PORT_OUTPUT			PORTC
#define _IO_KEY_PORT_DIR			DDRC
#define _IO_KEY_PORT_INPUT			PINC

#define _IO_KEY_BIT					5

#define _IO_LED_PORT_OUTPUT			PORTB
#define _IO_LED_PORT_DIR			DDRB
#define _IO_LED_PORT_INPUT			PINB

#define _IO_LED_BIT					0

#define _IO_PWM_PORT_OUTPUT			PORTB
#define _IO_PWM_PORT_DIR			DDRB
#define _IO_PWM_PORT_INPUT			PINB

#define _IO_PWM_OC1_BIT				1
#define _IO_PWM_OC2_BIT				3

#define _IO_PWM_OC2_MAX_VALUE		63

/*
	+--------------------------------------+
	|       ENUMERATIONS                   |
	+--------------------------------------+
*/

enum _IO_Adc_Channels
{
	ADC_CHANNEL_FQ1=0,
	ADC_CHANNEL_FS1=1,
	ADC_CHANNEL_FQ2=2,
	ADC_CHANNEL_FS2=3
};

enum _IO_Key_Codes
{	KEY_NONE,
	KEY_DOWN,
	KEY_UP,
	KEY_ENTER,	
	KEY_DOWN_OVER,
	KEY_UP_OVER,
	KEY_ENTER_OVER,
	KEY_DOWN_REPEAT,
	KEY_UP_REPEAT,	
	KEY_ENTER_REPEAT
};


	extern "C" void TIMER2_COMP_vect(void) __attribute__ ((signal));
						 
/*
        +-------------------------------------------------------+
        |        CLASS		IO				                    |
        +-------------------------------------------------------+
        | @PURPOSE      : Set of functions to control I/O		|
		|				  Hardware (Bios)						|
        | @OBSERVATIONS : use Timer1 as resource				|
        +-------------------------------------------------------+
*/

class class_IO
{
/*
         +--------------------------------------+
         |       PRIVATE VARIABLES		        |
         +--------------------------------------+
        */
 private:
        /*
         +--------------------------------------+
         |       PRIVATE FUNCTIONS      	    |
         +--------------------------------------+
        */
		friend void    TIMER2_COMP_vect(void);
		
		void	KeyInitialize(void);
		void	ADCInitialize(uint8_t adcPrescalerFactor);
		void	PWMsInitialize(void);
		void	LEDInitialize(void);
		void    Initialize(void);
        void    UnInitialize(void);
        /*
         +--------------------------------------+
         |        PUBLIC FUNCTIONS		        |
         +--------------------------------------+
        */
public:
        //Constructors
        class_IO(void);
        //Remainder functions
		void		ADCSetChannel(uint8_t adcChannel);
		uint8_t		ADCGetChannel(void);
		uint16_t	ADCGetValue(void);
		int32_t		Range(int32_t valueIn,int32_t rangeInMin,int32_t rangeInMax,int32_t rangeOutMin,int32_t rangeOutMax);
		char		KeyReadFlash(void);
		void		KeyWaitRelease(void);
		char		KeyRead(void);
		int16_t		MedianFilter(int16_t newValue,int16_t sortBuffer[],int16_t queueBuffer[]);
		void		PWMSetTopAndFase(uint16_t topPWM1,uint16_t fasePWM1,uint16_t topPWM2,uint16_t fasePWM2);
		void		LEDSet(bool ledStatus=true);
		void		LEDSwap(void);
};


#endif

