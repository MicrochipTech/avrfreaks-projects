/*
	//////////////////////////////////////////////////////////////////////////////////////////
	//																						//
	//		  _____ _                  _     _ _                          					//
	//		 |_   _(_)                | |   (_) |                         					//
	//		   | |  _ _ __ ___   ___  | |    _| |__  _ __ __ _ _ __ _   _ 					//
	//		   | | | | '_ ` _ \ / _ \ | |   | | '_ \| '__/ _` | '__| | | |					//
	//		   | | | | | | | | |  __/ | |___| | |_) | | | (_| | |  | |_| |					//
	//		   \_/ |_|_| |_| |_|\___| \_____/_|_.__/|_|  \__,_|_|   \__, |					//
	//		                                                         __/ |					//
	//		                                                         |___/ 					//
	//																						//
    //////////////////////////////////////////////////////////////////////////////////////////
    // This Drive running on ATMEGA8,ATMEGA162,ATMEGA128 and atmega328p devices             //
    // Some function need global interrupts must enabled to work properly                   //
	//////////////////////////////////////////////////////////////////////////////////////////
	----------------------------- Time & Delay Header (tab 4) -------------------------------
		  
*/

#ifndef _TIME0_LIB_H_
#define _TIME0_LIB_H_

/*
 +--------------------------------------+
 |        HARDWARE DEPENDENT INCLUDES	|
 +--------------------------------------+
*/
#include <avr/io.h>
#include <avr/interrupt.h>

/*
 +--------------------------------------+
 |        C LIBRARY INCLUDES	        |
 +--------------------------------------+
*/
#include <stdint.h>


/*
	+--------------------------------------+
	|       CONSTANTS DEFINITIONS          |
	+--------------------------------------+
*/

/*
	+--------------------------------------+
	|       ENUMERATIONS                   |
	+--------------------------------------+
*/

enum _Timers_
{
	TIMER_0,
	TIMER_1,
	TIMER_2,
	TIMER_3
};

enum _Timers_Error_Codes
{
	TIMER_ERROR_CODE_OK,
	TIMER_ERROR_CODE_IN_USE
};
/*
	+--------------------------------------+
	|       DECLARATIONS                   |
	+--------------------------------------+
*/

#if (defined(_AVR_IOM128_H_) || defined(_AVR_IOM162_H_))
    extern "C" void TIMER0_COMP_vect(void) __attribute__ ((signal));
#elif defined(_AVR_IOM328P_H_)
    extern "C" void TIMER0_COMPA_vect(void) __attribute__ ((signal));
#elif defined(_AVR_IOM8_H_)
    extern "C" void TIMER0_OVF_vect(void) __attribute__ ((signal));
#endif


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

class class_Time
{
        /*
         +--------------------------------------+
         |       PRIVATE VARIABLES				|
         +--------------------------------------+
        */
private:
                uint8_t  Hour;
                uint8_t  Minute;
                uint8_t  Second;
                uint32_t chrono_time;
        /*
         +--------------------------------------+
         |       PRIVATE FUNCTIONS              |
         +--------------------------------------+
        */
        void        Initialize();
        void        UnInitialize();
        uint32_t    GetTimeTick();
    #if (defined(_AVR_IOM128_H_) || defined(_AVR_IOM162_H_))
        friend void    TIMER0_COMP_vect(void);
    #elif defined(_AVR_IOM328P_H_)
        friend void    TIMER0_COMPA_vect(void);
    #elif defined(_AVR_IOM8_H_)
        friend void    TIMER0_OVF_vect(void);
    #endif
        /*
         +--------------------------------------+
         |        PUBLIC FUNCTIONS		        |
         +--------------------------------------+
        */
public:
       //Constructor
       	class_Time();
        //Remainder functions
        void            PrepareForSleep(void);
        void            RestoreFromSleep(void);
        void            Delay7Nops(uint16_t count);
        void	        DelayMsS(uint16_t timeMs);
        uint32_t        GetMilliseconds(void);
        void	        SetElapsed(uint32_t * timeVariable);
        uint32_t        GetElapsed(uint32_t * timeVariable);
        void	        DelayMsI(uint32_t timeMs);
        void	        ChronoMark(void);
        uint32_t        ChronoNow(void);
        void            SetTimeClockCallback(void (*TimePumpCallback)(uint8_t hour,uint8_t minute,uint8_t second));
		void			SetTimeInterruptCallback(void (*TimeInterruptCallback)(void));
        void            SetClockState(bool inUse);
		void			SetInterruptState(bool inUse);
        bool            GetClockState(void);
		bool			GetInterruptState(void);
        void            GetTime(void);
        uint8_t         GetHour(void);
        uint8_t         GetMinute(void);
        uint8_t         GetSecond(void);
        void            GetTime(uint8_t &hour,uint8_t &minute,uint8_t &second);
        void            SetTime(uint8_t hour,uint8_t minute,uint8_t second);
        void            SetHour(uint8_t hourParm);
        void            SetMinute(uint8_t minuteParm);
        void            SetSecond(uint8_t secondParm);
        void            SetTime(void);
};

#endif /* _TIME0_LIB_H_ */