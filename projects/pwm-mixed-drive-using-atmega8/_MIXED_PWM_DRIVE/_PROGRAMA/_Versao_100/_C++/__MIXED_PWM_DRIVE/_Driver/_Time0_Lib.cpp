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
    // This Drive running on ATMEGA8,ATMEGA162,ATMEGA128 and atmega328p devices             //
    // Some function need global interrupts must enabled to work properly                   //
    // Compile this library using optimization -os or -o1                                   //
	//////////////////////////////////////////////////////////////////////////////////////////
	-------------------------- Time & Delay Functions C++ (tab 4)----------------------------		  
*/


#ifndef _TIME0_LIB_CPP_
#define _TIME0_LIB_CPP_


/*
 +--------------------------------------+
 |        LOCAL LIBRARY INCLUDES	    |
 +--------------------------------------+
*/

#include  "_Time0_Lib.h"

/*
 +--------------------------------------+
 |        LOCAL FUNCTIONS CONSTANTS 	|
 +--------------------------------------+
*/

#define TIME_TIMER_DIV         256
#define TIME_TIMER_FREQ        1250
#define TIME_1MS                ((uint64_t)(1000ULL*16777216UL)/TIME_TIMER_FREQ)


/*
 +--------------------------------------+
 |        STRUCTS AND UNIONS		    |
 +--------------------------------------+
*/
struct _timeStruct
{
        volatile uint8_t        Hour=0;
        volatile uint8_t        Minute=0;
        volatile uint8_t        Second=0;
        volatile uint16_t       Milliseconds=0;
        volatile uint8_t		Tenth=0;
        volatile bool           TimeInUse=false;
        volatile bool			InterruptInUse=false;
        volatile bool           Initialized=false;
        volatile uint64_t       TimerTick=0;
        volatile uint8_t		ErrorCode=TIMER_ERROR_CODE_OK;
        void					(*TimeClockCallBack)(uint8_t hour,uint8_t minute,uint8_t second)=nullptr;
        void					(*TimeInterruptCallBack)(void)=nullptr;
} _time;


/*
 +--------------------------------------+
 |        FORWARD FUNCTION			    |
 +--------------------------------------+
*/
		void		_timeInterrupt(void);
		void		_timeClock(void);
/*
        +-------------------------------------------------------+
        |        CLASS TIMER IMPLEMENTATION                     |
        +-------------------------------------------------------+
        | @PURPOSE      : Perform all Timer or Delay Functions  |
        | @OBSERVATIONS : Depend of processor type ISR resource |
        |                 may be TIMER0_vect or                 |
        |                 may be TIMER0_COMPA_vect or           |
        |                 may be TIMER0_OVF_vect                |
        +-------------------------------------------------------+
*/

/*
        +-----------------------------------------------+
        |       PRIVATE FUNCTIONS IMPLEMENTATION        |
        +-----------------------------------------------+
*/
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  time engine initialization					                        |
//	| NAME    :  Initialize								                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
void class_Time::Initialize()
{
    uint8_t sreg;

    //Save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
    //program time/count 0 to generate 1250Hz at 8Mhz xtal
	#if (defined(_AVR_IOM128_H_) || defined(_AVR_IOM162_H_))
		//WGM02=0;WGM01=1;WGM00=0 MODE CTC
		TCCR0&=~(1<<WGM00);
		TCCR0|=(1<<WGM01);
		//Set WGM bits
		#if defined(_AVR_IOM128_H_)
			//CS02=1;CS01=1;CS00=0 clock divided by 256
			TCCR0&=~(1<<CS00);
			TCCR0|=(1<<CS02)|(1<<CS01);
		#elif defined(_AVR_IOM162_H_)
			//CS02=1;CS01=0;CS00=0 clock divided by 256
			TCCR0&=~((1<<CS01)|(1<<CS00));
			TCCR0|=(1<<CS02);
		#endif
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
    uint8_t sreg;

    //Save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
#if (defined(_AVR_IOM128_H_) || defined(_AVR_IOM162_H_))
    TIMSK&=~(1<<OCIE0);
#elif defined(_AVR_IOM328P_H_)
    //disable timer compare A interrupt
    TIMSK0&=~(1<<OCIE0A);
#elif defined(_AVR_IOM8_H_)
    //Disable TIMER0 CTC interrupt
    TIMSK&=~(1<<TOIE0);
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

/*
        +-----------------------------------------------+
        |       CONSTRUCTORS AND DESTRUCTORS            |
        +-----------------------------------------------+
*/

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  time engine initialization						                    |
//	| NAME    :  timeInit								                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
class_Time::class_Time()
{
	uint8_t sreg;

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

#if (defined(_AVR_IOM128_H_) || defined(_AVR_IOM162_H_))
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

//#######################
//### COMMON FUNCTION ###
//#######################

#include  "_Time_Common.h"

#endif

//	+----------------------------#endif //#define _TIME0_LIB_CPP_
