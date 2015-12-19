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
	// This Drive running on ATMEGA8,ATMEGA162,ATMEGA128 and atmega328p devices				//
	// Global interrupts must enabled to work properly										//
	//																						//
	// Version.: 1.25																		//
	// By......: João D´Artagnan A. Oliveira												//
	// Date....: October, 16 of 2015														//
	// Revision: 3																			//
	//////////////////////////////////////////////////////////////////////////////////////////
	--------------------- Time Drive V125 C++ Header File (tab 4) ----------------------------
		  
*/

#ifndef _TIME_LIB_H_
#define _TIME_LIB_H_

// 
//  +--------------------------------------+
//  |        HARDWARE DEPENDENT INCLUDES   |
//  +--------------------------------------+
// 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// 
//  +--------------------------------------+
//  |        TIMER STRUCTS             	   |
//  +--------------------------------------+
// 

 
//
// 	+--------------------------------------+
// 	|       EXTERNAL DECLARATIONS          |
// 	+--------------------------------------+
//

//---------------------------------------------------------------------------------------------
// Definition of _TIME_TIMER_NUMBER_=0 must be set in compiler with as global definition
//---------------------------------------------------------------------------------------------
#if defined(_TIME_TIMER_NUMBER_)
    #if (_TIME_TIMER_NUMBER_==0)
		#define _TIME_TIMER_SIZE_ 8
        #if defined(_AVR_IOM128_H_)
            extern "C" void TIMER0_COMP_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM162_H_)
            extern "C" void TIMER0_COMP_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM328P_H_)
            extern "C" void TIMER0_COMPA_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM8_H_)
            extern "C" void TIMER0_OVF_vect(void) __attribute__ ((signal));
        #else
            #error Time Library not suport this processor
        #endif
    //----------------------------------------------------------------------------
    #elif (_TIME_TIMER_NUMBER_==1)
		#define _TIME_TIMER_SIZE_ 16
        #if defined(_AVR_IOM128_H_)
            extern "C" void TIMER1_COMPA_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM162_H_)
            extern "C" void TIMER1_COMPA_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM328P_H_)
            extern "C" void TIMER1_COMPA_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM8_H_)
            extern "C" void TIMER1_COMPA_vect(void) __attribute__ ((signal));
        #else
            #error Time Library not suport this processor
        #endif
    //----------------------------------------------------------------------------
    #elif (_TIME_TIMER_NUMBER_==2)
		#define _TIME_TIMER_SIZE_ 8
        #if defined(_AVR_IOM128_H_)
            extern "C" void TIMER2_COMP_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM162_H_)
            extern "C" void TIMER2_COMP_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM328P_H_)
            extern "C" void TIMER2_COMPA_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM8_H_)
            extern "C" void TIMER2_COMP_vect(void) __attribute__ ((signal));
        #else
            #error Time Library not suport this processor
        #endif
    //----------------------------------------------------------------------------
    #elif (_TIME_TIMER_NUMBER_==3)
		#define _TIME_TIMER_SIZE_ 16
        #if defined(_AVR_IOM128_H_)
            extern "C" void TIMER3_COMPA_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM162_H_)
            extern "C" void TIMER3_COMPA_vect(void) __attribute__ ((signal));
        #elif defined(_AVR_IOM328P_H_)
            #error ATMEGA328P not have a Timer3
        #elif defined(_AVR_IOM8_H_)
            #error ATMEGA8 not have a Timer3
        #else
            #error Time Library not suport this processor
        #endif
    //----------------------------------------------------------------------------
    #else
        #error _TIME_TIMER_NUMBER_ invalid timer number
    #endif
#else   
    #error _TIME_TIMER_NUMBER_ Not defined
#endif
// 
// 	+--------------------------------------+
// 	|       CONSTANTS DEFINITIONS          |
// 	+--------------------------------------+
// 

// 
// 	+--------------------------------------+
// 	|       ENUMERATIONS                   |
// 	+--------------------------------------+
// 
typedef struct struct_Time
{
	uint8_t	Hour,
			Minute,
			Second;	
} Time;
// 
//         +-------------------------------------------------------+
//         |        CLASS TIMER  COMMON IMPLEMENTATION F           |
//         +-------------------------------------------------------+
//         | @PURPOSE      : Common Time and Time measure routines |
//         | @OBSERVATIONS : user Hard resources Timer0,..,Timer3  |
//         +-------------------------------------------------------+
// 

class class_Time
{
//         
//          +--------------------------------------+
//          |       PRIVATE VARIABLES			   |
//          +--------------------------------------+
//         
private:
		Time		TimeTmp;
        uint32_t	chrono_TimeMark;
//         
//          +--------------------------------------+
//          |       PRIVATE FUNCTIONS              |
//          +--------------------------------------+
//         
        void        Initialize(void);
        void        UnInitialize(void);
        uint32_t    GetTimeTick(void);
//        
//          +--------------------------------------+
//          |       FRIEND PRIVATE FUNCTIONS       |
//          +--------------------------------------+
//        
//        friend void   _timeInterrupt(void);
//        friend void   _timeClock(void);
//----------------------------------------------------------------------------
#if (_TIME_TIMER_NUMBER_==0)
    #if defined(_AVR_IOM128_H_)
        friend void    TIMER0_COMP_vect(void);
    #elif defined(_AVR_IOM162_H_)
        friend void    TIMER0_COMP_vect(void);
    #elif defined(_AVR_IOM328P_H_)
        friend void    TIMER0_COMPA_vect(void);
    #elif defined(_AVR_IOM8_H_)
        friend void    TIMER0_OVF_vect(void);
    #endif       
//----------------------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==1)
    #if defined(_AVR_IOM128_H_)
        friend void     TIMER1_COMPA_vect(void);
    #elif defined(_AVR_IOM162_H_)
        friend void     TIMER1_COMPA_vect(void);
    #elif defined(_AVR_IOM328P_H_)
        friend void     TIMER1_COMPA_vect(void);
    #elif defined(_AVR_IOM8_H_)
        friend void     TIMER1_COMPA_vect(void);
    #endif
//----------------------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==2)
    #if defined(_AVR_IOM128_H_)
        friend void     TIMER2_COMP_vect(void);
    #elif defined(_AVR_IOM162_H_)
        friend void     TIMER2_COMP_vect(void);
    #elif defined(_AVR_IOM328P_H_)
        friend void     TIMER2_COMPA_vect(void);
    #elif defined(_AVR_IOM8_H_)
        friend void     TIMER2_COMP_vect(void);     
    #endif
//----------------------------------------------------------------------------
#elif (_TIME_TIMER_NUMBER_==3)
    #if defined(_AVR_IOM128_H_)
        friend void     TIMER3_COMP_vect(void);
    #elif defined(_AVR_IOM162_H_)
        friend void     TIMER3_COMP_vect(void);
    #elif defined(_AVR_IOM328P_H_)
        
    #elif defined(_AVR_IOM8_H_)
        
    #endif
//----------------------------------------------------------------------------
#endif
//         
//          +--------------------------------------+
//          |        PUBLIC FUNCTIONS		       |
//          +--------------------------------------+
//         
public:
       //Constructor
       	class_Time();
        void            PrepareForSleep(void);
        void            RestoreFromSleep(void);
        void            Delay5Nops(uint16_t count);
        void	        DelayMillisBySoftware(uint16_t timeMs);
        uint32_t        GetMilliseconds(void);
        void	        SetElapsed(uint32_t * timeVariable);
        uint32_t        GetElapsed(uint32_t * timeVariable);
        void	        DelayMillisByInterrupt(uint32_t timeMs);
        void	        ChronoMark(void);
        uint32_t        ChronoNow(void);
        void            SetTimeClockCallBack(void (*TimePumpCallback)(uint8_t hour,uint8_t minute,uint8_t second));
		void			SetTimeInterruptCallBack(void (*TimeInterruptCallback)(void));
        void            SetTimeClockCallBackState(bool inUse=false);
		void			SetTimeInterruptCallBackState(bool inUse=false);
        bool            GetTimeClockCallBackState(void);
		bool			GetTimeInterruptCallBackState(void);
        Time            GetTime(void);
        void            GetTime(uint8_t &hour,uint8_t &minute,uint8_t &second);
        void            SetTime(uint8_t hour,uint8_t minute,uint8_t second);
        void            SetTime(Time time);
		uint8_t			inline GetTimerSize(void) { return _TIME_TIMER_SIZE_;};
		uint8_t			inline GetTimerNumber(void) { return _TIME_TIMER_NUMBER_;};
		void			Test(void);
};

#endif //_TIME_LIB_H_