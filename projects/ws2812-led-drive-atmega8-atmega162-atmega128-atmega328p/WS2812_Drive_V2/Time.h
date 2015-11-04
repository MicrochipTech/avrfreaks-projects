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
	----------------------------- Time Common Header (tab 4) ---------------------------------
		  
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

//----------------------------------------------------------------------------
#if defined(_TIME_TIMER_NUMBER_)
    #if (_TIME_TIMER_NUMBER_==0)
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

// 
//         +-------------------------------------------------------+
//         |        CLASS TIMER  COMMON IMPLEMENTATION F           |
//         +-------------------------------------------------------+
//         | @PURPOSE      : Perform all Timer or Delay Functions  |
//         | @OBSERVATIONS : Common function for timer0,timer1...  |
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
       
        uint8_t  Hour;
        uint8_t  Minute;
        uint8_t  Second;
        uint32_t chrono_TimeMark;
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
        friend void   _timeInterrupt(void);
        friend void   _timeClock(void);
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
        /************************************************************************/
        /* Prepare Timer for microprocessor in Sleep state                      */
        /************************************************************************/
        void            PrepareForSleep(void);
		/************************************************************************/
		/* Restore Timer from Sleep state                                       */
		/************************************************************************/
        void            RestoreFromSleep(void);
		/************************************************************************/
		/* Delay 7 NOPS instruction a count times                               */
		/************************************************************************/
        void            Delay7Nops(uint16_t count);
		/************************************************************************/
		/* Delay timeMS microseconds by software                                */
		/************************************************************************/
        void	        DelayMsS(uint16_t timeMs);
		/************************************************************************/
		/* Get elapsed milliseconds from CPU reset                              */
		/************************************************************************/
        uint32_t        GetMilliseconds(void);
		/************************************************************************/
		/* Set Elapsed Time in milliseconds to timeVariable                     */
		/************************************************************************/
        void	        SetElapsed(uint32_t * timeVariable);
		/************************************************************************/
		/* Get Elapset time in milliseconds on timeVariable since last			*/
		/* SetElapsed time														*/
		/************************************************************************/
        uint32_t        GetElapsed(uint32_t * timeVariable);
		/************************************************************************/
		/* Delay timeMS milliseconds using interrupt timer, global interrupts   */
		/* must be enabled i.e. sei();											*/
		/************************************************************************/
        void	        DelayMsI(uint32_t timeMs);
		/************************************************************************/
		/* Mark an actual time                                                  */
		/************************************************************************/
        void	        ChronoMark(void);
		/************************************************************************/
		/* Get Elapsed time since ChronoMark                                    */
		/************************************************************************/
        uint32_t        ChronoNow(void);
		/************************************************************************/
		/* Define call back routine for process time, this function when enabled*/
		/* is called 10 times per second										*/
		/************************************************************************/
        void            SetTimeClockCallback(void (*TimePumpCallback)(uint8_t hour,uint8_t minute,uint8_t second));
		/************************************************************************/
		/* Define call back routine for a timer interrupt, this function when	*/
		/* Enabled is called 1250 times per second								*/
		/************************************************************************/
		void			SetTimeInterruptCallback(void (*TimeInterruptCallback)(void));
		/************************************************************************/
		/* Enable clock call back function                                      */
		/************************************************************************/
        void            SetClockState(bool inUse);
		/************************************************************************/
		/* Enable Interrupt State call back function                            */
		/************************************************************************/
		void			SetInterruptState(bool inUse);
		/************************************************************************/
		/* Get clock call back function state ON=true                           */
		/************************************************************************/
        bool            GetClockState(void);
		/************************************************************************/
		/* Get Interrupt call back state ON=true                                */
		/************************************************************************/
		bool			GetInterruptState(void);
		/************************************************************************/
		/* Set Time hour,minute,second                                          */
		/************************************************************************/
        void            GetTime(uint8_t &hour,uint8_t &minute,uint8_t &second);
		/************************************************************************/
		/* Get Time hour,minute,second                                          */
		/************************************************************************/
        void            SetTime(uint8_t hour,uint8_t minute,uint8_t second);
};

#endif //_TIME_LIB_H_