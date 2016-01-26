/*
	//////////////////////////////////////////////////////////////////////////////////////////
	//											                                            //
	//		  _____ _                  _     _ _                          		            //
	//		 |_   _(_)                | |   (_) |                         		            //
	//		   | |  _ _ __ ___   ___  | |    _| |__  _ __ __ _ _ __ _   _ 		            //
	//		   | | | | '_ ` _ \ / _ \ | |   | | '_ \| '__/ _` | '__| | | |		            //
	//		   | | | | | | | | |  __/ | |___| | |_) | | | (_| | |  | |_| |		            //
	//		   \_/ |_|_| |_| |_|\___| \_____/_|_.__/|_|  \__,_|_|   \__, |		            //
	//		                                                         __/ |		            //
	//		                                                         |___/ 		            //
	//											                                            //
    //////////////////////////////////////////////////////////////////////////////////////////
 	// This Drive running on ATMEGA8,ATMEGA162,ATMEGA128 and atmega328p devices				//
 	// Global interrupts must enabled to work properly										//
 	//																						//
 	// Version.: 1.25																		//
 	// By......: João D´Artagnan A. Oliveira												//
 	// Date....: October, 16 of 2015														//
 	// Revision: 3																			//
 	//////////////////////////////////////////////////////////////////////////////////////////
 	----------------------- Time Drive V125 C++ Code File (tab 4) ----------------------------
*/

/**
******************************************************************************************
*@file   
*@author Joao Dartagnan Antunes Oliveira
*@date   16/08/2016
*@brief  <b>(C++)</b> File Implements Function for Time handdle
*@copyright GNU Public License.
******************************************************************************************
*/

//
//     +--------------------------------------+
//     |        LOCAL LIBRARY INCLUDES	      |
//     +--------------------------------------+
//

#include  "_Time_v125.h"

// 
//     +--------------------------------------+
//     |        LOCAL FUNCTIONS CONSTANTS     |
//     +--------------------------------------+
// 

#define TIME_TIMER_DIV         256
#define TIME_TIMER_FREQ        1250
#define TIME_1MS                ((uint64_t)(1000ULL*16777216UL)/TIME_TIMER_FREQ)
#define TIME_DIVISORS			7

//
//     +--------------------------------------+
//     |        LOCAL STRUCTS                 |
//     +--------------------------------------+
//
#if (_TIME_TIMER_NUMBER_==0)	
	const uint16_t _timePrescalers[TIME_DIVISORS] PROGMEM = 
	{
		#if defined(_AVR_IOM8_H_)
			1,8,64,256,1024,0,0
		#elif defined(_AVR_IOM162_H_)
			1,8,64,256,1024,0,0
		#elif defined(_AVR_IOM128_H_)
			1,8,32,64,128,256,1024
		#elif defined(_AVR_IOM328P_H_)
			1,8,64,256,1024,0,0
		#endif
	};
#elif (_TIME_TIMER_NUMBER_==1)
	const uint16_t _timePrescalers[TIME_DIVISORS] PROGMEM =
	{
		#if defined(_AVR_IOM8_H_)
			1,8,64,256,1024,0,0
		#elif defined(_AVR_IOM162_H_)
			1,8,64,256,1024,0,0
		#elif defined(_AVR_IOM128_H_)
			1,8,64,256,1024,0,0
		#elif defined(_AVR_IOM328P_H_)
			1,8,64,256,1024,0,0
		#endif
	};
#elif (_TIME_TIMER_NUMBER_==2)
	const uint16_t _timePrescalers[TIME_DIVISORS] PROGMEM =
	{
		#if defined(_AVR_IOM8_H_)
			1,8,32,64,128,256,1024
		#elif defined(_AVR_IOM162_H_)
			1,8,32,64,128,256,1024
		#elif defined(_AVR_IOM128_H_)
			1,8,64,256,1024,0,0
		#elif defined(_AVR_IOM328P_H_)
			1,8,32,64,128,256,1024
		#endif
	};
#elif (_TIME_TIMER_NUMBER_==3)
	const uint16_t _timePrescalers[TIME_DIVISORS] PROGMEM =
	{
		#if defined(_AVR_IOM8_H_)
			0,0,0,0,0,0,0
		#elif defined(_AVR_IOM162_H_)
			1,8,64,256,1024,16,32
		#elif defined(_AVR_IOM128_H_)
			1,8,64,256,1024,0,0
		#elif defined(_AVR_IOM328P_H_)
			0,0,0,0,0,0,0
		#endif
	};
#endif

static struct 
{
    volatile uint8_t         Hour=0;
    volatile uint8_t         Minute=0;
    volatile uint8_t         Second=0;
    volatile uint16_t        Milliseconds=0;
    volatile uint8_t		 Tenth=0;
    volatile bool            TimeInUse=false;
    volatile bool		     InterruptInUse=false;
    volatile bool            Initialized=false;
    volatile uint64_t        TimerTick=0;
    void					 (*TimeClockCallBackFunc)(uint8_t hour,uint8_t minute,uint8_t second)=nullptr;
    void					 (*TimeInterruptCallBackFunc)(void)=nullptr;
} _time;


// 
//         +-------------------------------------------------------+
//         |        CLASS TIMER IMPLEMENTATION                     |
//         +-------------------------------------------------------+
//         | @PURPOSE      : Perform all Timer or Delay Functions  |
//         | @OBSERVATIONS : Depend of processor type ISR resource |
//         |                 may be TIMER0_vect or                 |
//         |                 may be TIMER0_COMPA_vect or           |
//         |                 may be TIMER0_OVF_vect                |
//         +-------------------------------------------------------+
//
 
//	+---------------------------------------------------------------------------------------+
//	| FUNCTION:  Get Time Prescaler Divisor according processor and _TIME_TIMER_NUMBER_     |
//	| NAME    :  GetTimePrescaler  				        		                            |
//	| INPUT   :  none								                                        |
//	| OUTPUT  :  none								                                        |
//	| REMARKS :																				|
//	+---------------------------------------------------------------------------------------+
// static uint16_t GetTimePrescaler()
// {
// 	//search for all divisors
// 	for(uint8_t i=0;i<TIME_DIVISORS;i++)
// 	{
// 		//compute divisor according timer size
// 		switch (_TIME_TIMER_SIZE_)
// 		{
// 			//if 8 bits timer
// 			case 8:
// 			break;
// 			//if 16 bits timer
// 			case 16:
// 			break;
// 		}
// 	}
// 	//return zero if divisor not found
// 	return 0;
// }
//	+---------------------------------------------------------------------------------------+
//	| FUNCTION:  Time interrupt if inUse flag is true                    		            |
//	| NAME    : _timeInterrupt   				        		                            |
//	| INPUT   :  none								                                        |
//	| OUTPUT  :  none								                                        |
//	| REMARKS :  to used callback function define a routine following format                |
//  |            callback(uint8_t Hour,uint8_t Minute,uint8_t Second);                      |
//	+---------------------------------------------------------------------------------------+
static void _timeInterrupt()
{
    if (_time.InterruptInUse)
    {
        if (_time.TimeInterruptCallBackFunc!=nullptr) _time.TimeInterruptCallBackFunc();
    }
}

//	+---------------------------------------------------------------------------------------+
//	| FUNCTION:  Pump Time if inUse flag is true                    		                |
//	| NAME    : _timePump   				        		                                |
//	| INPUT   :  none								                                        |
//	| OUTPUT  :  none								                                        |
//	| REMARKS :  to used callback function define a routine following format                |
//  |            callback(uint8_t Hour,uint8_t Minute,uint8_t Second);                      |
//	|			 this callback function if activated is called 10 times per second			|
//	+---------------------------------------------------------------------------------------+
static void _timeClock()
{
    //check if on
    if (_time.TimeInUse)
    {
        //Time to pump Time?
        _time.Milliseconds++;
        if (_time.Milliseconds<TIME_TIMER_FREQ/10) return;
        //clear timer pump
        _time.Milliseconds=0;
        //tenth of seconds
        _time.Tenth++;
        if (_time.Tenth>9)
        {
            _time.Tenth=0;
            //yes, pump Seconds
            _time.Second++;
            if (_time.Second>59)
            {
                //yes, pump Minutes clear second
                _time.Second=0;
                _time.Minute++;
                if (_time.Minute>59)
                {
                    //yes, pump Hours clear Minute
                    _time.Minute=0;
                    _time.Hour++;
                    if (_time.Hour>23) _time.Hour=0;
                }
            }
        }
        //callback function giving to defined routine parms hour,min,second (updated each second)
        if (_time.TimeClockCallBackFunc!=nullptr) _time.TimeClockCallBackFunc(_time.Hour,_time.Minute,_time.Second);
    }
}

// 
//         +-----------------------------------------------+
//         |       PRIVATE FUNCTIONS IMPLEMENTATION        |
//         +-----------------------------------------------+
// 

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  time engine initialization					                        |
//	| NAME    :  Initialize								                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
void class_Time::Initialize()
{
    unsigned char sreg;
//	uint16_t	  prescaler;

    //Save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
//	prescaler=GetTimePrescaler();
	
    //program time/count 0 to generate 1250Hz at 8Mhz xtal
//----------------------------------------------------------------
#if (_TIME_TIMER_NUMBER_==0)
	#if defined(_AVR_IOM128_H_)
		//WGM01=1;WGM00=0 MODE CTC
        TCCR0&=~((1<<WGM01)|(1<<WGM00));
		TCCR0|= ((1<<WGM01)|(0<<WGM00));
		//CS02=1;CS01=1;CS00=0 clock divided bLy 256
        TCCR0&=~((1<<CS02)|(1<<CS01)|(1<<CS00));
		TCCR0|= ((1<<CS02)|(1<<CS01)|(0<<CS00));
		//program divisor
		OCR0=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
		TCNT0=0;
		//enable timer compare A interrupt
		TIMSK|=(1<<OCIE0);
    #elif defined(_AVR_IOM162_H_)
		//WGM02=0;WGM01=1;WGM00=0 MODE CTC
		TCCR0&=~(1<<WGM00);
		TCCR0|=(1<<WGM01);
		//CS02=1;CS01=0;CS00=0 clock divided by 256
		TCCR0&=~((1<<CS01)|(1<<CS00));
		TCCR0|=(1<<CS02);
		//program divisor
		OCR0=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
		TCNT0=0;
		//enable timer compare A interrupt
		TIMSK|=(1<<OCIE0);
	#elif defined(_AVR_IOM328P_H_)
		//WGM02=0;WGM01=1;WGM00=0 MODE CTC
		TCCR0A&=~(1<<WGM00);
		TCCR0A|=(1<<WGM01);
		TCCR0B&=~(1<<WGM02);
		//CS02=1;CS01=0;CS00=0 clock divided by 256
		TCCR0B&=~((1<<CS01)|(1<<CS00));
		TCCR0B|=(1<<CS02);
		//program divisor
		OCR0A=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
		TCNT0=0;
		//enable timer compare A interrupt
		TIMSK0|=(1<<OCIE0A);
	#elif defined(_AVR_IOM8_H_)
		//clock divided by 256 Timer0 Overflow
		TCCR0 = (1<<CS02)|(0<<CS01)|(0<<CS00);
		TCNT0 =256-(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ);
		//Enable TIMER0 CTC interrupt
		TIMSK|=(1<<TOIE0);
	#endif
//----------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==1)
	#if defined(_AVR_IOM128_H_)
	    //WGM13=0;WGM12=1;WGM11=0;WGM10=0; MODE CTC (Top=OCR1A)
	    TCCR1A&=~((1<<WGM11)|(1<<WGM10));
	    TCCR1B|= ((0<<WGM13)|(1<<WGM12));
	    //CS12=1;CS11=0;CS10=0 clock divided by 256
	    TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));
	    TCCR1B|= ((1<<CS12)|(0<<CS11)|(0<<CS10));
	    //program divisor
	    OCR1A=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
	    TCNT1=0;
	    //enable timer compare A interrupt
	    TIMSK|=(1<<OCIE1A);
	#elif defined(_AVR_IOM162_H_)
	    //WGM13=0;WGM12=1;WGM11=0;WGM10=0; MODE CTC (Top=OCR1A)
	    TCCR1A&=~((1<<WGM11)|(1<<WGM10));
	    TCCR1B|= ((0<<WGM13)|(1<<WGM12));
	    //CS12=1;CS11=0;CS10=0 clock divided by 256
	    TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));
	    TCCR1B|= ((1<<CS12)|(0<<CS11)|(0<<CS10));
	    //program divisor
	    OCR1A=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
	    TCNT1=0;
	    //enable timer compare A interrupt
	    TIMSK|=(1<<OCIE1A);
	#elif defined(_AVR_IOM328P_H_)
	    //WGM13=0;WGM12=1;WGM11=0;WGM10=0; MODE CTC (Top=OCR1A)
	    TCCR1A&=~((1<<WGM11)|(1<<WGM10));
	    TCCR1B|= ((0<<WGM13)|(1<<WGM12));
	    //CS12=1;CS11=0;CS10=0 clock divided by 256
	    TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));
	    TCCR1B|= ((1<<CS12)|(0<<CS11)|(0<<CS10));
	    //program divisor
	    OCR1A=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
	    TCNT1=0;
	    //enable timer compare A interrupt
	    TIMSK1|=(1<<OCIE1A);
	#elif defined(_AVR_IOM8_H_)
	    //WGM13=0;WGM12=1;WGM11=0;WGM10=0; MODE CTC (Top=OCR1A)
	    TCCR1A&=~((1<<WGM11)|(1<<WGM10));
	    TCCR1B|= ((0<<WGM13)|(1<<WGM12));
	    //CS12=1;CS11=0;CS10=0 clock divided by 256
	    TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));
	    TCCR1B|= ((1<<CS12)|(0<<CS11)|(0<<CS10));
	    //program divisor
	    OCR1A=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
	    TCNT1=0;
	    //enable timer compare A interrupt
	    TIMSK|=(1<<OCIE1A);
	#endif
//----------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==2)
	#if defined(_AVR_IOM128_H_)
	    //WGM21=1;WGM20; MODE CTC (Top=OCR2)
        TCCR2&=~((1<<WGM21)|(1<<WGM20));
        TCCR2|= ((1<<WGM21)|(0<<WGM20));
	    //CS22=1;CS21=0;CS20=0 clock divided by 256
        TCCR2&=~((1<<CS22)|(1<<CS21)|(1<<CS20));
        TCCR2|= ((1<<CS22)|(0<<CS21)|(0<<CS20));
	    //program divisor
	    OCR2=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
	    TCNT2=0;
	    //enable timer compare A interrupt
	    TIMSK|=(1<<OCIE2);
	#elif defined(_AVR_IOM162_H_)
	    //WGM21=1;WGM20; MODE CTC (Top=OCR2)
	    TCCR2&=~((1<<WGM21)|(1<<WGM20));
	    TCCR2|= ((1<<WGM21)|(0<<WGM20));
	    //CS22=1;CS21=1;CS20=0 clock divided by 256
	    TCCR2&=~((1<<CS22)|(1<<CS21)|(1<<CS20));
	    TCCR2|= ((1<<CS22)|(1<<CS21)|(0<<CS20));
	    //program divisor
	    OCR2=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
	    TCNT2=0;
	    //enable timer compare A interrupt
	    TIMSK|=(1<<OCIE2);
	#elif defined(_AVR_IOM328P_H_)
	    //WGM22=0;WGM21=1;WGM20=0; MODE CTC (Top=OCR2)
	    TCCR2A&=~(1<<WGM20);
        TCCR2A|= (1<<WGM21);
        TCCR2B&=~(1<<WGM22);
	    //CS22=1;CS21=1;CS20=0 clock divided by 256
	    TCCR2B&=~((1<<CS22)|(1<<CS21)|(1<<CS20));
	    TCCR2B|= ((1<<CS22)|(1<<CS21)|(0<<CS20));
	    //program divisor
	    OCR2A=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
	    TCNT2=0;
	    //enable timer compare A interrupt
	    TIMSK2|=(1<<OCIE2A);
	#elif defined(_AVR_IOM8_H_)
	    //WGM21=1;WGM20; MODE CTC (Top=OCR2)
	    TCCR2&=~((1<<WGM21)|(1<<WGM20));
	    TCCR2|= ((1<<WGM21)|(0<<WGM20));
	    //CS22=1;CS21=1;CS20=0 clock divided by 256
	    TCCR2&=~((1<<CS22)|(1<<CS21)|(1<<CS20));
	    TCCR2|= ((1<<CS22)|(1<<CS21)|(0<<CS20));
	    //program divisor
	    OCR2=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
	    TCNT2=0;
	    //enable timer compare A interrupt
	    TIMSK|=(1<<OCIE2);
	#endif
//----------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==3)
	#if defined(_AVR_IOM128_H_)
	   //WGM33=0;WGM32=1;WGM31=0;WGM30=0; MODE CTC (Top=OCR3A)
	   TCCR3A&=~((1<<WGM31)|(1<<WGM30));
	   TCCR3B|= ((0<<WGM33)|(1<<WGM32));
	   //CS32=1;CS31=0;CS30=0 clock divided by 256
	   TCCR3B&=~((1<<CS32)|(1<<CS31)|(1<<CS30));
	   TCCR3B|= ((1<<CS32)|(0<<CS31)|(0<<CS30));
	   //program divisor
	   OCR3A=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
	   TCNT3=0;
	   //enable timer compare A interrupt
	   ETIMSK|=(1<<OCIE3A);
	#elif defined(_AVR_IOM162_H_)
	   //WGM33=0;WGM32=1;WGM31=0;WGM30=0; MODE CTC (Top=OCR3A)
	   TCCR3A&=~((1<<WGM31)|(1<<WGM30));
	   TCCR3B|= ((0<<WGM33)|(1<<WGM32));
	   //CS32=1;CS31=0;CS30=0 clock divided by 256
	   TCCR3B&=~((1<<CS32)|(1<<CS31)|(1<<CS30));
	   TCCR3B|= ((1<<CS32)|(0<<CS31)|(0<<CS30));
	   //program divisor
	   OCR3A=(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ)-1;
	   TCNT3=0;
	   //enable timer compare A interrupt
	   ETIMSK|=(1<<OCIE3A);
	#elif defined(_AVR_IOM328P_H_)
	#elif defined(_AVR_IOM8_H_)
	#endif
#endif		
    //Restore Interrupts
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  time engine Undo initialization			                        |
//	| NAME    :  UnInitialize						                                |
//	| INPUT   :  none							                                    |
//	| OUTPUT  :  none							                                    |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void class_Time::UnInitialize()
{
    unsigned char sreg;

    //Save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
//----------------------------------------------------------------
#if (_TIME_TIMER_NUMBER==0)
    #if defined(_AVR_IOM128_H_)
        //disable timer interrupt
        TIMSK&=~(1<<OCIE0);
        //stop timer
        TCCR0&=~((1<<CS02)|(1<<CS01)|(1<<CS00));
    #elif defined(_AVR_IOM162_H_)
        //disable timer interrupt
        TIMSK&=~(1<<OCIE0);
        //stop timer
        TCCR0&=~((1<<CS02)|(1<<CS01)|(1<<CS00));
    #elif defined(_AVR_IOM328P_H_)
        //disable timer compare A interrupt
        TIMSK0&=~(1<<OCIE0A);
        //stop timer
		TCCR0B&=~((1<<CS02)|(1<<CS01)|(1<<CS00));
    #elif defined(_AVR_IOM8_H_)
        //Disable TIMER0 CTC interrupt
        TIMSK&=~(1<<TOIE0);
        //stop timer
        TCCR0&=~((1<<CS02)|(1<<CS01)|(1<<CS00));
    #endif
//----------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==1)
    #if defined(_AVR_IOM128_H_)
        //disable timer interrupt
        TIMSK&=~(1<<OCIE1A);
        //stop timer
        TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));
    #elif defined(_AVR_IOM162_H_)
        //disable timer interrupt
        TIMSK&=~(1<<OCIE1A);
        //stop timer
        TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));        
    #elif defined(_AVR_IOM328P_H_)
        //disable timer interrupt
        TIMSK1&=~(1<<OCIE1A);
        //stop timer
        TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));
    #elif defined(_AVR_IOM8_H_)
        //disable timer interrupt
        TIMSK&=~(1<<OCIE1A);
        //stop timer
        TCCR1B&=~((1<<CS12)|(1<<CS11)|(1<<CS10));
    #endif
//----------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==2)
    #if defined(_AVR_IOM128_H_)
        //disable timer interrupt
        TIMSK&=~(1<<OCIE2);
        //stop timer
        TCCR2&=~((1<<CS22)|(1<<CS21)|(1<<CS20));
    #elif defined(_AVR_IOM162_H_)
        //disable timer interrupt
        TIMSK&=~(1<<OCIE2);
        //stop timer
        TCCR2&=~((1<<CS22)|(1<<CS21)|(1<<CS20));        
    #elif defined(_AVR_IOM328P_H_)
        //disable timer interrupt
        TIMSK2&=~(1<<OCIE2A);
	    //stop timer
	    TCCR2B&=~((1<<CS22)|(1<<CS21)|(1<<CS20));
    #elif defined(_AVR_IOM8_H_)
    #endif
//----------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==3)
    #if defined(_AVR_IOM128_H_)
        //disable timer interrupts
        ETIMSK&=~(1<<OCIE3A);
        //stop timer
        TCCR3B&=~((1<<CS32)|(1<<CS31)|(1<<CS30));
    #elif defined(_AVR_IOM162_H_)
        //disable timer interrupts
        ETIMSK&=~(1<<OCIE3A);
        //stop timer
        TCCR3B&=~((1<<CS32)|(1<<CS31)|(1<<CS30));    
    #elif defined(_AVR_IOM328P_H_)
    
    #elif defined(_AVR_IOM8_H_)
    #endif
#endif
    //Restore Interrupts
    SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get system time Tick (Tick in milliseconds)                        |
//	| NAME    :  GetTimeTick    				                                    |
//	| INPUT   :                                     		                        |
//	| OUTPUT  : TimeTick    					                                    |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+

uint32_t class_Time::GetTimeTick()
{
    return *((uint32_t*)((uint8_t*)&_time.TimerTick+3));
}

// 
//         +-----------------------------------------------+
//         |       CONSTRUCTORS AND DESTRUCTORS            |
//         +-----------------------------------------------+
// 

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  time engine initialization						                    |
//	| NAME    :  timeInit								                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
class_Time::class_Time()
{
	unsigned char sreg;

    //save SREG
	sreg=SREG;
    //Disable Interrupts
	cli();
    //Check if already Initialized
    if (_time.Initialized==false)
    {
        //set initialized
        _time.Initialized=true;
        //Initialize
        Initialize();
    }
    
    //Restore SREG state
	SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Interrupt Service Routine for TIME0 compare			            |
//	| NAME    :  						        		                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+

//----------------------------------------------------------------
#if (_TIME_TIMER_NUMBER_==0)
    #if defined(_AVR_IOM128_H_)
        ISR(TIMER0_COMP_vect)
        {
		    //enable interrupts to permit that other pending interrupts is executed more fast as possible
		    sei();		
            _time.TimerTick+=TIME_1MS;
		    _timeInterrupt();
            _timeClock();
        }
    #elif defined(_AVR_IOM162_H_)
        ISR(TIMER0_COMP_vect)
        {
		    //enable interrupts to permit that other pending interrupts is executed more fast as possible
		    sei();
		    _time.TimerTick+=TIME_1MS;
		    _timeInterrupt();
		    _timeClock();
        }
    #elif defined(_AVR_IOM328P_H_)
        ISR(TIMER0_COMPA_vect)
        {
		    //enable interrupts to permit that other pending interrupts is executed more fast as possible
		    sei();		
            _time.TimerTick+=TIME_1MS;
		    _timeInterrupt();
            _timeClock();
        }
    #elif defined(_AVR_IOM8_H_)
        ISR(TIMER0_OVF_vect)
        {

            #define tcnt0_value (256-(F_CPU/TIME_TIMER_DIV/TIME_TIMER_FREQ))

		    TCNT0=tcnt0_value;
		    sei();
           _time.TimerTick+=TIME_1MS;
	       _timeInterrupt();
           _timeClock();
        }
    #endif
//----------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==1)

    #if defined(_AVR_IOM128_H_)
        ISR(TIMER1_COMPA_vect)
        {
            //enable interrupts to permit that other pending interrupts is executed more fast as possible
            sei();
            _time.TimerTick+=TIME_1MS;
            _timeInterrupt();
            _timeClock();
        }
    #elif defined(_AVR_IOM162_H_)
        ISR(TIMER1_COMPA_vect)
        {
            //enable interrupts to permit that other pending interrupts is executed more fast as possible
            sei();
            _time.TimerTick+=TIME_1MS;
            _timeInterrupt();
            _timeClock();
        }
    #elif defined(_AVR_IOM328P_H_)
        ISR(TIMER1_COMPA_vect)
        {
            //enable interrupts to permit that other pending interrupts is executed more fast as possible
            sei();
            _time.TimerTick+=TIME_1MS;
            _timeInterrupt();
            _timeClock();
        }
    #elif defined(_AVR_IOM8_H_)
        ISR(TIMER1_COMPA_vect)
        {
            //enable interrupts to permit that other pending interrupts is executed more fast as possible
            sei();
            _time.TimerTick+=TIME_1MS;
            _timeInterrupt();
            _timeClock();
        }
   #endif
//----------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==2)

    #if defined(_AVR_IOM128_H_)
        ISR(TIMER2_COMP_vect)
        {
            //enable interrupts to permit that other pending interrupts is executed more fast as possible
            sei();
            _time.TimerTick+=TIME_1MS;
            _timeInterrupt();
            _timeClock();
        }
    #elif defined(_AVR_IOM162_H_)
        ISR(TIMER2_COMP_vect)
        {
            //enable interrupts to permit that other pending interrupts is executed more fast as possible
            sei();
            _time.TimerTick+=TIME_1MS;
            _timeInterrupt();
            _timeClock();
        }
    #elif defined(_AVR_IOM328P_H_)
        ISR(TIMER2_COMPA_vect)
        {
            //enable interrupts to permit that other pending interrupts is executed more fast as possible
            sei();
            _time.TimerTick+=TIME_1MS;
            _timeInterrupt();
            _timeClock();
        }
    #elif defined(_AVR_IOM8_H_)
        ISR(TIMER2_COMP_vect)
        {
            //enable interrupts to permit that other pending interrupts is executed more fast as possible
            sei();
            _time.TimerTick+=TIME_1MS;
            _timeInterrupt();
            _timeClock();
        }
    #endif
//----------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==3)
     #if defined(_AVR_IOM128_H_)
         ISR(TIMER3_COMPA_vect)
         {
             //enable interrupts to permit that other pending interrupts is executed more fast as possible
             sei();
             _time.TimerTick+=TIME_1MS;
             _timeInterrupt();
             _timeClock();
         }
     #elif defined(_AVR_IOM162_H_)
         ISR(TIMER3_COMPA_vect)
         {
             //enable interrupts to permit that other pending interrupts is executed more fast as possible
             sei();
             _time.TimerTick+=TIME_1MS;
             _timeInterrupt();
             _timeClock();
         }
     #elif defined(_AVR_IOM328P_H_)
     #elif defined(_AVR_IOM8_H_)
     #endif
#endif

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Prepare Timer for enter in sleep                                  |
//	| @NAME    :  PrepareForSleep                    	                            |
//	| @INPUT   :  none                                                              |
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_Time::PrepareForSleep()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //Disable interrupts
    cli();
    //UnInitialize local I/O
    UnInitialize();
    //restore SREG
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Restore Time Delay from recovery of sleep state                   |
//	| @NAME    :  RestoreFromSleep                    	                            |
//	| @INPUT   :  none                                                              |
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_Time::RestoreFromSleep()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //Disable interrupts
    cli();
    //restore I/O from sleep
    Initialize();
    //restore SREG
    SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Delay 5 nops                                       		        |
//	| NAME    :  Delay5Nops			        				                        |
//	| INPUT   :  Number of 5Nops     						                        |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  Below code must be used with compiler optimization -OS or -O1      |
//	+-------------------------------------------------------------------------------+
void    class_Time::Delay5Nops(uint16_t count)
{
	for(;;)
	{
		asm volatile("nop");
		count--;
		if (count==0) break;
	}
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  milliseconds delay	by software					                    |
//	| NAME    :  DelayMsH							                                |
//	| INPUT   :  delay in milliseconds 						                        |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+

void	class_Time::DelayMillisBySoftware(uint16_t time)
{
	uint16_t countFor1Millisecond;
	
	//compute time to make a delay with 1ms
	//below equation is computed take in consideration that (lo while) loop time 4 clocks in assembler
	countFor1Millisecond=F_CPU/4000;
	

		asm volatile("movw	 r16,%[countTime]		\t\n"	\
					 "2:							\t\n"	\
					 "movw   r18,%[countForMilli]	\t\n"	\
					 "1:							\t\n"	\
					 "subi	r18,1					\t\n"	\
					 "sbci	r19,0					\t\n"	\
					 "brne 1b						\t\n"	\
					 "subi	r16,1					\t\n"	\
					 "sbci	r17,0					\t\n"	\
					 "brne 2b						\t\n"	\
					 :
					 :[countForMilli]"r" (countFor1Millisecond),
					  [countTime]	 "r"  (time)
					 :"r16","r17","r18","r19"
					);

}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get system time in milliseconds                                    |
//	| NAME    :  GetMilliseconds				                                    |
//	| INPUT   :  -> timeVariable                    		                        |
//	| OUTPUT  :  								                                    |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+

uint32_t class_Time::GetMilliseconds()
{
        uint32_t tmp1=0, tmp2=0;
        //This routine avoid Tick increments during read of consecutive Ticks
        do
        {
            sei();
            tmp1=GetTimeTick();
            sei();
            tmp2=GetTimeTick();
        } while (tmp1!=tmp2);
        //return tick value
        return tmp2;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set tick time variable to start time measure		                |
//	| NAME    :  SetElapsedTickTime		        			                        |
//	| INPUT   :  -> tickTimerVariable					                            |
//	| OUTPUT  :  								                                    |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void	class_Time::SetElapsed(uint32_t * timeVariable)
{
	*timeVariable=GetMilliseconds();
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get elapsed timer tick variable				                    |
//	| NAME    :  GetElapsed              					                        |
//	| INPUT   :  * tickTimerVariable					                            |
//	| OUTPUT  :  Elapsed time in ticks					                            |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
uint32_t class_Time::GetElapsed(uint32_t * timeVariable)
{
	return (GetMilliseconds()-*timeVariable);				//Get difference between actual tick and save tick
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Delay in milliseconds by ISR					                    |
//	| NAME    :  DelayMsI		        					                        |
//	| INPUT   :  timeMs in milliseconds					                            |
//	| OUTPUT  :  								                                    |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void	class_Time::DelayMillisByInterrupt(uint32_t timeMs)
{
	uint32_t tmp1,tmp2;

	tmp1=GetMilliseconds();             					//get actual tick value
	do{
		tmp2=GetMilliseconds();
	} while ((tmp2-tmp1)<timeMs);							//wait reach a predefined value tick
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Chrono Mark  (clear time)        				                    |
//	| NAME    :  Chrono Mark       	        				                        |
//	| INPUT   :                     					                            |
//	| OUTPUT  :  								                                    |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void    class_Time::ChronoMark()
{
        chrono_TimeMark=GetMilliseconds();
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Chrono Now  (Get value of chronometer at that moment)      	    |
//	| NAME    :  Chrono Now       	        					                    |
//	| INPUT   :                     						                        |
//	| OUTPUT  :  chronometer value in milliseconds  				                |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
uint32_t    class_Time::ChronoNow()
{
        return (GetMilliseconds()-chrono_TimeMark);				//Get difference between actual tick and save tick
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Callback function for clock                                  	|
//	| NAME    :  SetTimePumpCallback         					                    |
//	| INPUT   :  void (*callback)          						                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void    class_Time::SetTimeClockCallBack(void (*TimeClockCallBackFunc)(uint8_t hour,uint8_t minute,uint8_t second))
{
    //avoid routines that get more than 720uS long to execute
    _time.TimeClockCallBackFunc=TimeClockCallBackFunc;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Callback function for clock                                  	|
//	| NAME    :  SetTimeInterruptCallback      					                    |
//	| INPUT   :  void (*callback)          						                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void    class_Time::SetTimeInterruptCallBack(void (*TimeInterruptCallback)(void))
{
	//avoid routines that get more than 720uS long to execute
	_time.TimeInterruptCallBackFunc=TimeInterruptCallback;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Clock State ON(Running) or OFF(Stopped)                      	|
//	| NAME    :  SetClockState    	        					                    |
//	| INPUT   :  state ON or OFF          						                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void	class_Time::SetTimeClockCallBackState(bool inUse/* =false */)
{
        _time.TimeInUse=inUse;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Interrupt State ON(Running) or OFF(Stopped)                   	|
//	| NAME    :  SetInterruptState   	        					                |
//	| INPUT   :  state ON or OFF          						                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void	class_Time::SetTimeInterruptCallBackState(bool inUse/* =false */)
{
	_time.InterruptInUse=inUse;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get Clock State ON(Running) or OFF(Stopped)                      	|
//	| NAME    :  GetClockState    	        					                    |
//	| INPUT   :  state ON or OFF          						                    |
//	| REMARKS :  			        				                                |
//	+-------------------------------------------------------------------------------+
bool   class_Time::GetTimeClockCallBackState()
{
        return _time.TimeInUse;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get Interrupt State ON(Running) or OFF(Stopped)                   	|
//	| NAME    :  GetInterruptState 	        					                    |
//	| INPUT   :  state ON or OFF          						                    |
//	| REMARKS :  			        				                                |
//	+-------------------------------------------------------------------------------+
bool   class_Time::GetTimeInterruptCallBackState()
{
	return _time.InterruptInUse;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get Time Hour,Minute and Second and put into a temp vars          	|
//	| NAME    :  GetTime       	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  Interrupts are disabled and restore        		                |
//	+-------------------------------------------------------------------------------+
Time    class_Time::GetTime()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //clear interrupts
    cli();
    //Set Time
    TimeTmp.Hour=_time.Hour;
    TimeTmp.Minute=_time.Minute;
    TimeTmp.Second=_time.Second;
    //Restore SREG
    SREG=sreg;
	//return Time
	return TimeTmp;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get Time Hour,Minute and Second                                  	|
//	| NAME    :  GetTime       	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  Interrupts are disabled and restore        		                |
//	+-------------------------------------------------------------------------------+
void    class_Time::GetTime(uint8_t &hourParm,uint8_t &minuteParm,uint8_t &secondParm)
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //clear interrupts
    cli();
    //Set Time
    hourParm=_time.Hour;
    minuteParm=_time.Minute;
    secondParm=_time.Second;
    //Restore SREG
    SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Time Hour,Minute and Second                                  	|
//	| NAME    :  SetTime       	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  Interrupts are disabled and restore        		                |
//	+-------------------------------------------------------------------------------+
void    class_Time::SetTime(uint8_t hourParm,uint8_t minuteParm,uint8_t secondParm)
{
        uint8_t sreg;

        //save SREG
        sreg=SREG;
        //clear interrupts
        cli();
        //Set Time
        _time.Hour=hourParm;
        _time.Minute=minuteParm;
        _time.Second=secondParm;
        //Restore SREG
        SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Time Hour,Minute and Second form tmp time vars               	|
//	| NAME    :  SetTime       	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  Interrupts are disabled and restore        		                |
//	+-------------------------------------------------------------------------------+
void    class_Time::SetTime(Time time)
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //clear interrupts
    cli();
    //Set Time
    _time.Hour=time.Hour;
    _time.Minute=time.Minute;
    _time.Second=time.Second;
    //Restore SREG
    SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Called only for testing purpose						           	|
//	| NAME    :  Test       	        							                |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :											       		                |
//	+-------------------------------------------------------------------------------+

void class_Time::Test()
{
	
}