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
/**
******************************************************************************************
*@file   
*@author Joao Dartagnan Antunes Oliveira
*@date   16/08/2016
*@brief  <b>(Header)</b> File Implements Function for Time handdle
*@copyright GNU Public License.
******************************************************************************************
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

/**
******************************************************************************
*@struct struct_Time
*@brief This <b>structure</b> is used to hold a Time Hour,Minute and Second
******************************************************************************
*/
typedef struct struct_Time
{
	uint8_t	Hour,
			Minute,
			Second;	
} Time;

/**
******************************************************************************
*
*	@class class_Time
*	@brief This <b>Class</b> Implements functions to handdle time
*
******************************************************************************
*/
class class_Time
{
//         
//          +--------------------------------------+
//          |       PRIVATE VARIABLES			   |
//          +--------------------------------------+
//         
private:
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief used to save temporary Hour,Minute and Second
		//----------------------------------------------------------------------------------------------------------------------------------		
		Time		TimeTmp;
		//----------------------------------------------------------------------------------------------------------------------------------
		///@brief used by function SetElapsed() and getElapsed()
		//----------------------------------------------------------------------------------------------------------------------------------		
        uint32_t	chrono_TimeMark;
//         
//          +--------------------------------------+
//          |       PRIVATE FUNCTIONS              |
//          +--------------------------------------+
//  
		//	+-------------------------------------------------------------------------------+
		///@brief  Initialize Timer class engine
		///@fn     Initialize
		///@return none \n
		///@brief
		///@see class_Time() constructor
		//	+-------------------------------------------------------------------------------+       
        void        Initialize(void);
		//	+-------------------------------------------------------------------------------+
		///@brief  UnInitialize Timer class engine
		///@fn     UnInitialize
		///@return none \n
		///@brief
		///@see ~class_Time() constructor
		//	+-------------------------------------------------------------------------------+       
        void        UnInitialize(void);
		//	+-------------------------------------------------------------------------------+
		///@brief  Get A Tick number of Timer , in this version is a 1250 ticks per second
		///@fn     GetTimeTick
		///@return none \n
		///@brief
		//	+-------------------------------------------------------------------------------+       
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
        //	+-------------------------------------------------------------------------------+
		///@brief  Initialize Timer class engine
		///@fn     class_Time()
		///@return none \n
		///@brief
		//	+-------------------------------------------------------------------------------+       
       	class_Time();
        //	+-------------------------------------------------------------------------------+
		///@brief  Prepare Timer engine to enter in Sleep mode
		///@fn     PrepareForSleep
		///@return none \n
		///@brief
		//	+-------------------------------------------------------------------------------+       
        void            PrepareForSleep(void);
        //	+-------------------------------------------------------------------------------+
		///@brief  Restore Timer from Sleep mode
		///@fn     PrepareForSleep
		///@return none \n
		///@brief
		//	+-------------------------------------------------------------------------------+       
        void            RestoreFromSleep(void);
        //	+-------------------------------------------------------------------------------+
		///@brief  Delay 5 Assembler Nops intructions repetive count times
		///@fn     Delay5Nops
		///@param  [in] count Times of (5 nops) 
		///@return none \n
		///@brief
		//	+-------------------------------------------------------------------------------+       		
        void            Delay5Nops(uint16_t count);
        //	+-------------------------------------------------------------------------------+
		///@brief  Delay Milliseconds Software implementation
		///@fn     DelayMillisBySoftware
		///@param  [in] timeMs Number of milleseconds to delay
		///@return none \n
		///@brief The precision of this routine is affected by interrupts handlers
		//	+-------------------------------------------------------------------------------+       		
        void	        DelayMillisBySoftware(uint16_t timeMs);
        //	+-------------------------------------------------------------------------------+
		///@brief  Get the number of milliseconds elapsed since the Reset
		///@fn     GetMilliseconds
		///@return Number of milliseconds elapsed
		///@brief 
		//	+-------------------------------------------------------------------------------+       		
        uint32_t        GetMilliseconds(void);
        //	+-------------------------------------------------------------------------------+
		///@brief  Set a variable with the actual elapsed milliseconds
		///@fn     SetElapsed
		///@param  [in] timeVariable A Variable to hold a elapsed milliseconds
		///@return NNone\n
		///@brief 
		//	+-------------------------------------------------------------------------------+       		
        void	        SetElapsed(uint32_t * timeVariable);
        //	+-------------------------------------------------------------------------------+
		///@brief  Get a elapsed time since last SetElapsed() functions with the same variable called
		///@fn     GetElapsed
		///@param  [in] timeVariable A variable with last elapsed milliseconds saved
		///@return elapsed time from last SetElapsed() funtion
		///@brief ___________________________________________________________________________
		///@brief Below a example of use:\n\n
		///@brief <b>uint32_t tTime,TimeElapsed</b>      variables used to hold a elapsed times \n\n
		///@brief <b>SetElapsed(&tTime);</b>			  set tTime with actual elapsed time \n
		///@brief <b> ............</b>					  process anything \n
		///@brief <b>TimeElapsed=GetElapsed(&tTime);</b> TimeElapsed hold a elapsed time in milliseconds since SetElapsed(&tTime) is called \n
		///@brief ___________________________________________________________________________
		//	+-------------------------------------------------------------------------------+       		
        uint32_t        GetElapsed(uint32_t * timeVariable);
		//	+-------------------------------------------------------------------------------+
		///@brief  Make (n)Delay of milliseconds precision using a Timer interrupt
		///@fn     DelayMillisByInterrupt
		///@param  [in] timeMs Milliseconds to delay
		///@return NNone\n
		///@brief The precision of this routines is less affected by interrupts handlers
		//	+-------------------------------------------------------------------------------+       		
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