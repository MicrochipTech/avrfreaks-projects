/*
 /////////////////////////////////////////////////////////////////////////////////////////
            _   _  _   _ _____  ______      _             _____ _____
           | | | || | | |  _  | |  _  \    (_)           |_   _|  _  |
           | |_| || |_| | | | | | | | |_ __ ___   _____    | | | | | |
           |  _  ||  _  | | | | | | | | '__| \ \ / / _ \   | | | | | |
           | | | || | | \ \_/ / | |/ /| |  | |\ V /  __/  _| |_\ \_/ /
           \_| |_/\_| |_/\___/  |___/ |_|  |_| \_/ \___|  \___/ \___/
           
 /////////////////////////////////////////////////////////////////////////////////////////
  ---------------------------------- I/O  C++ File (tab 4) -------------------------------
*/

#ifndef _IO_CPP_
#define _IO_CPP_
/*
 +--------------------------------------+
 |        GLOBALS INCLUDES          	|
 +--------------------------------------+
*/

#include "____GLOBAL/_Global.h"


/*
 +--------------------------------------+
 |        C LIBRARY INCLUDES		    |
 +--------------------------------------+
*/

/*
 +--------------------------------------+
 |        LOCAL LIBRARY INCLUDES	    |
 +--------------------------------------+
*/
#include "_IO.h"
#include "_Time0_Lib.h"

/*
	+--------------------------------------+
	|       LIBRARY INCLUDES               |
	+--------------------------------------+
*/

/*
	+--------------------------------------+
	|     FORWARD FUNCTION DEFINITIONS     |
	+--------------------------------------+
*/

/*
 +--------------------------------------+
 |        LOCAL EEPROM VARIABLES	 	|
 +--------------------------------------+
*/


/*
 +--------------------------------------+
 |        LOCAL FLASH CONSTANTS		 	|
 +--------------------------------------+
*/

/*
 +--------------------------------------+
 |        STRUCTS AND UNIONS		    |
 +--------------------------------------+
*/


struct _IO_Struct
{
		volatile uint8_t		ADCChannel=0;
		volatile uint8_t		pwm2ct=0;
		volatile uint8_t		pwm2cp=_IO_PWM_OC2_MAX_VALUE;
		class_Time				time;
}_io;

/*
 +--------------------------------------+
 |        FORWARD FUNCTIONS			 	|
 +--------------------------------------+
*/
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize status LED					                            |
//	| @NAME    :  LEDInitialize()	              		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_IO::LEDInitialize()
{
	uint8_t sreg;
	//save SREG
	sreg=SREG;
	//disable interrupts
	cli();
	//Initialize LED output off
	_IO_LED_PORT_OUTPUT|=(1<<_IO_LED_BIT);
	_IO_LED_PORT_DIR|=(1<<_IO_LED_BIT);
	//restore SREG
	SREG=sreg;

}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize PWM timer1 and timer2	                                |
//	| @NAME    :  InitializePWMs	              		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+

void	class_IO::PWMsInitialize()
{
	uint8_t sreg;
	//save SREG
	sreg=SREG;
	//disable interrupts
	cli();
	//OCR1A OC2 OUTPUT LOW 
	_IO_PWM_PORT_DIR|=(1<<_IO_PWM_OC1_BIT);		//OC1A output
	_IO_PWM_PORT_DIR|=(1<<_IO_PWM_OC2_BIT);		//OC2 output
	_IO_PWM_PORT_OUTPUT&=~(1<<_IO_PWM_OC1_BIT);	//OC1A low
	_IO_PWM_PORT_OUTPUT&=~(1<<_IO_PWM_OC2_BIT);	//OC2 low
	//set pwm 1 mode fast pwm top icr1 prescaler 1x
	TCCR1A=(1<<COM1A1)|(1<<COM1A0)|(1<<WGM11)|(0<<WGM10);
	TCCR1B=(1<<WGM13)|(1<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10);
	//program 50khz output 0% duty
	ICR1=160;
	OCR1A=0;	
	//set pwm 2 software generated using mode ctc ocr2 prescaler 32x enable output compare isr
	TCCR2=(1<<WGM21)|(0<<WGM20)|(0<<CS22)|(1<<CS21)|(1<<CS20);
	OCR2=255;	//start with low interrupt frequency
	TIMSK|=(1<<OCIE2);
	//restore SREG
	SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  TIMER2 COMPARE INTERRUPT SERVICE ROUTINE                          |
//	| @NAME    :  TIMER2 COMPARE ISR             		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
ISR(TIMER2_COMP_vect)
{
	//pump pwm count
	if (++_io.pwm2ct>_IO_PWM_OC2_MAX_VALUE)
	{
		_io.pwm2ct=0;
	}
	//check level to set
	if (_io.pwm2ct>_io.pwm2cp)
	{
		_IO_PWM_PORT_OUTPUT|=(1<<_IO_PWM_OC2_BIT);
		TCNT1=0;
	}
	else
	{
		_IO_PWM_PORT_OUTPUT&=~(1<<_IO_PWM_OC2_BIT);
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize ADC					                                |
//	| @NAME    :  InitializeADC		              		                            |
//	| @INPUT   :  ADC prescaler  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS : prescaler must be 2,4,8,16,32,64 or 128							|
//	+-------------------------------------------------------------------------------+

void	class_IO::ADCInitialize(uint8_t prescalerFactor)
{
	uint8_t sreg,prescalerLog;
	//save SREG
	sreg=SREG;
	//disable interrupts
	cli();
	//set ADC input with high z
	_IO_ADC_PORT_OUTPUT&=~((1<<_IO_ADC_CHANNEL_FQ1)|(1<<_IO_ADC_CHANNEL_FS1)|(1<<_IO_ADC_CHANNEL_FQ2)|(1<<_IO_ADC_CHANNEL_FS2));
	_IO_ADC_PORT_DIR   &=~((1<<_IO_ADC_CHANNEL_FQ1)|(1<<_IO_ADC_CHANNEL_FS1)|(1<<_IO_ADC_CHANNEL_FQ2)|(1<<_IO_ADC_CHANNEL_FS2));
	//set reference to internal 2.56v
	ADMUX|=(1<<REFS1)|(1<<REFS0);
	//no shift adjust
	ADMUX&=~(1<<ADLAR);
	//first compute log2(prescalerFactor)
	prescalerLog=7;
	if (prescalerFactor==0) prescalerFactor=1;
	for(;;)
	{
		//exit if log computed checking bit 7
		if ((prescalerFactor & (1<<7))!=0) break;
		//shift value
		prescalerFactor<<=1;
		//compute log
		prescalerLog--;
	}
	//Set prescaler
	if ((prescalerLog & (1<<0))==0)	ADCSRA&=~(1<<ADPS0); else ADCSRA|=(1<<ADPS0);
	if ((prescalerLog & (1<<1))==0)	ADCSRA&=~(1<<ADPS1); else ADCSRA|=(1<<ADPS1);
	if ((prescalerLog & (1<<2))==0)	ADCSRA&=~(1<<ADPS2); else ADCSRA|=(1<<ADPS2);
	//start first conversion
	ADCSRA|=(1<<ADEN)|(1<<ADSC);
	do {
		
	} while ((ADCSRA & (1<<ADSC))!=0);
	//restore SREG
	SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize KEYs					                                |
//	| @NAME    :  KEYsInitialize	              		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_IO::KeyInitialize()
{
	//set keys input with pull up
	_IO_KEY_PORT_OUTPUT|=(1<<_IO_KEY_BIT);
	_IO_KEY_PORT_DIR   &=~(1<<_IO_KEY_BIT);
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Initialize LED Temperatura drive                                  |
//	| @NAME    :  Initialize                    		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+


void    class_IO::Initialize()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
	ADCInitialize(32);
	KeyInitialize();
	PWMsInitialize();
	LEDInitialize();
    //restore SREG state
    SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Undo Initialization of WIFI I/O                                   |
//	| @NAME    :  UnInitialize                      	                            |
//	| @INPUT   :  none                                                              |
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :  interrupts are disabled and restored				                |
//	+-------------------------------------------------------------------------------+
void    class_IO::UnInitialize()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //disable interrupts
    cli();
    //lead Keys output and dir unchange

    //restore SREG state
    SREG=sreg;
}

/*
        +-----------------------------------------------+
        |       PUBLIC CONSTRUCTORS AND DESTRUCTORS     |
        +-----------------------------------------------+
*/

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Constructor                                                       |
//	| @NAME    :  General WIFI IO Initialization  		                            |
//	| @INPUT   :  none                                                              |
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :													                |
//	+-------------------------------------------------------------------------------+
class_IO::class_IO()
{
	
	Initialize();
	
}
/*
        +-----------------------------------------------+
        |       PUBLIC FUNCTIONS IMPLEMENTATION         |
        +-----------------------------------------------+
*/
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Select ADC channel				                                |
//	| @NAME    :  ADCSelectChannel	              		                            |
//	| @INPUT   :  Channel		 													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :  channel 0..7														|
//	+-------------------------------------------------------------------------------+
void	class_IO::ADCSetChannel(uint8_t adcChannel)
{
	uint8_t sreg;
	
	//save SREG
	sreg=SREG;
	//disable interrupts
	cli();
	//select only valid channels
	adcChannel&=7;
	_io.ADCChannel=adcChannel;
	//set multiplex
	ADMUX&=~((1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0));
	ADMUX|=adcChannel;
	//restore SREG
	SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Get ADC Select channel			                                |
//	| @NAME    :  ADCGetSelectedChannel            		                            |
//	| @INPUT   :				 													|
//	| @OUTPUT  :  channel							                                |
//	| @REMARKS :  channel 0..7														|
//	+-------------------------------------------------------------------------------+
uint8_t	class_IO::ADCGetChannel()
{
	return _io.ADCChannel;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Get ADC value						                                |
//	| @NAME    :  ADCGetValue		              		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  Value								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
uint16_t class_IO::ADCGetValue()
{
	uint8_t sreg;
	uint16_t adcValue;
	
	//save SREG
	sreg=SREG;
		//start conversion
		ADCSRA|=(1<<ADSC);
		//wait finish
		do{
			
		} while ((ADCSRA & (1<<ADSC))!=0);	
	//disable interrupts
	cli();
	//get value
	adcValue=ADC;
	//restore interrupts
	SREG=sreg;
	//return value
	return adcValue;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Range, ajdust input value in range min,max to output range min,max|
//	| @NAME    :  Range				              		                            |
//	| @INPUT   :  valueIn,rangeInMin,rangeInMax,rangeOutMin,rangeOutMax				|
//	| @OUTPUT  :  ValueOut							                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+

int32_t	class_IO::Range(int32_t valueIn,int32_t rangeInMin,int32_t rangeInMax,int32_t rangeOutMin,int32_t rangeOutMax)
{
	return ((valueIn-rangeInMin)*(rangeOutMax-rangeOutMin))/(rangeInMax-rangeInMin)+rangeOutMin;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Read Key without wait			                                    |
//	| @NAME    : KeyReadFlash 				  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  : keyCode							                                |
//	| @REMARKS : keyCode must be  _LED_TEMP_KEY_DOWN,  _LED_TEMP_KEY_UP,			|
//	|							   _LED_TEMP_KEY_ENTER or _LED_TEMP_KEY_NONE		|
//	+-------------------------------------------------------------------------------+
char	class_IO::KeyReadFlash()
{
	uint8_t tmp;

	//assume no key read
	tmp=_IO_KEY_PORT_INPUT;
	//check LED key
	if ((tmp & (1<<_IO_KEY_BIT))==0) return KEY_DOWN;
	//else none pressed
	return KEY_NONE;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Wait key release				                                    |
//	| @NAME    : KeyWaitRelease				  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  :									                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_IO::KeyWaitRelease()
{
	while (KeyReadFlash()!=KEY_NONE) _io.time.DelayMsI(20);  //check at 20ms
	_io.time.DelayMsI(100);										         //debounce time
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION: Read Key and wait key press by timeout value                       |
//	| @NAME    : KeyRead					  										|
//	| @INPUT   :  												                    |
//	| @OUTPUT  : KeyCode							                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
char	class_IO::KeyRead()
{
	uint8_t tmp,last,ct;
	static uint8_t repeat,first;
	
	tmp=KeyReadFlash();
	if (tmp==KEY_NONE)
	{
		repeat=OFF;
		do{
			_io.time.DelayMsS(20);
			tmp=KeyReadFlash();
		} while (tmp==KEY_NONE);
		ct=0;
		do{
			last=tmp;
			_io.time.DelayMsS(20);
			ct++;
			if (ct>25)
			{
				repeat=ON;
				first=ON;
				switch (tmp)
				{
					case KEY_DOWN:
						return KEY_DOWN_OVER;
					case KEY_UP:
						return KEY_UP_OVER;
					case KEY_ENTER:
						return KEY_ENTER_OVER;
				}
			}
			tmp=KeyReadFlash();
		} while (tmp!=KEY_NONE);
		_io.time.DelayMsS(50);
		return last;
	}
	else
	{
		if (repeat==ON)
		{
			if (first==ON)
			{
				_io.time.DelayMsS(500);
				first=OFF;
			}
			else
			{
				_io.time.DelayMsS(150);
			}
			switch (tmp)
			{
				case KEY_DOWN:
					return KEY_DOWN_REPEAT;
				case KEY_UP:
					return KEY_UP_REPEAT;
				case KEY_ENTER:
					return KEY_ENTER_REPEAT;
			}
		}
	}
	_io.time.DelayMsS(50);
	return tmp;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  MedianSortFilter								                    |
//	| @NAME    :  MedianSortFilter		          		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+

#define FILTER_SIZE	5

int16_t	class_IO::MedianFilter(int16_t newValue,int16_t sortBuffer[],int16_t queueBuffer[])
{
	int16_t tmp;
	//shift data into queue
	for(uint8_t i=0;i<(FILTER_SIZE-1);i++)
	{
		queueBuffer[i]=queueBuffer[i+1];
	}
	queueBuffer[FILTER_SIZE-1]=newValue;
	//transfer data to sortbuf
	for(uint8_t i=0;i<FILTER_SIZE;i++)
	{
		sortBuffer[i]=queueBuffer[i];
	}
	//sort data to get median value
	for(uint8_t j=FILTER_SIZE-2;j>1;j--)
	{
		for(uint8_t i=0;i<j;i++)
		{
			if (sortBuffer[i]>sortBuffer[i+1])
			{
				tmp=sortBuffer[i];
				sortBuffer[i]=sortBuffer[i+1];
				sortBuffer[i+1]=tmp;
			}
		}
	}
	return sortBuffer[FILTER_SIZE>>1];
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  SetPWMTopAndFase								                    |
//	| @NAME    :  SetPWMTopAndFase		          		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+

void	class_IO::PWMSetTopAndFase(uint16_t topPWM1,uint16_t fasePWM1,uint16_t topPWM2,uint16_t fasePWM2)
{
	uint8_t sreg;
	
	//save SREG
	sreg=SREG;
	cli();
	//FREQ AND FASE TIMER1 (FQ2)
	ICR1=topPWM2;
	OCR1A=fasePWM2;		
	//FREQ AND FASE TIMER2 (FQ1)
	OCR2=topPWM1;
	_io.pwm2cp=fasePWM1;
	//restore SREG
	SREG=sreg;
	
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  LED Set state							                            |
//	| @NAME    :  LEDSet			              		                            |
//	| @INPUT   :  state true=ON false=OFF											|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_IO::LEDSet(bool ledState)
{
	switch (ledState)
	{
		case true:
			_IO_LED_PORT_OUTPUT&=~(1<<_IO_LED_BIT);
			break;
		case false:
			_IO_LED_PORT_OUTPUT|=(1<<_IO_LED_BIT);
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  LED Swap states that means if ON turn OFF and vice versa          |
//	| @NAME    :  LEDSwap			              		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_IO::LEDSwap()
{
	if ((_IO_LED_PORT_INPUT & (1<<_IO_LED_BIT))==0)
	{
		_IO_LED_PORT_OUTPUT|=(1<<_IO_LED_BIT);
	}
	else
	{
		_IO_LED_PORT_OUTPUT&=~(1<<_IO_LED_BIT);
	}
}
#endif //_IO_CPP_
