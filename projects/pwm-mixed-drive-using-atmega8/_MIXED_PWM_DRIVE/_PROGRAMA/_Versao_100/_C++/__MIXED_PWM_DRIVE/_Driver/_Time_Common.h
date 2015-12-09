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


#ifndef _TIME_COMMON_CPP_
#define _TIME_COMMON_CPP_

/*
        +-----------------------------------------------+
        |       common PUBLIC FUNCTIONS                 |
        +-----------------------------------------------+
*/

//	+---------------------------------------------------------------------------------------+
//	| FUNCTION:  Time interrupt if inUse flag is true                    		            |
//	| NAME    : _timeInterrupt   				        		                            |
//	| INPUT   :  none								                                        |
//	| OUTPUT  :  none								                                        |
//	| REMARKS :  to used callback function define a routine following format                |
//  |            callback(uint8_t Hour,uint8_t Minute,uint8_t Second);                      |
//	+---------------------------------------------------------------------------------------+
void    _timeInterrupt()
{
	if (_time.InterruptInUse)
	{
		if (_time.TimeInterruptCallBack!=nullptr) _time.TimeInterruptCallBack();
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
void    _timeClock()
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
                if (_time.TimeClockCallBack!=nullptr) _time.TimeClockCallBack(_time.Hour,_time.Minute,_time.Second);
        }
}

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
//	| FUNCTION:  Delay 4 nops                                       		        |
//	| NAME    :  Delay4Nops			        				                        |
//	| INPUT   :  Number of 4Nops     						                        |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  More precise time duration                                         |
//  |                                                                               |
//  |               t=(7*count+6)/F_CPU	loop=7*nops time 6=call+ret-1brxx           |
//  |               or                                                              |
//  |               count=(t*F_CPU-6)/7                                             |
//  |                                                                               |
//	+-------------------------------------------------------------------------------+
void    class_Time::Delay7Nops(uint16_t count)
{
        do 
        {
                asm("nop");
                count--;
        } while (count);
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  milliseconds delay	by software					                    |
//	| NAME    :  DelayMsH							                                |
//	| INPUT   :  delay in milliseconds 						                        |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+

void	class_Time::DelayMsS(uint16_t time)
{
	unsigned int t1Ms;
    //---------------------------------------------------------------------
    // This function must be used only width opmization -os or -o1
    //
    // the below assembler code is generated by compiler
    //
    // label:
    //    	sbiw	r24, 0x01	;2
    //      brne    label       ;2/1
    //
    // This program imply the following equation
    //
    //  tUs=(4*n-1)/F       tUs in microseconds
    //
    // thus converting this equation to milliseconds delay yields
    //
    //  tMs=(F_CPU/4000)    That is used by this routine
    //
    //  time used by external while loop not computed
    //  but is used 7 clocks each millisecond loop this yields
    //  7000 clock more than necessary. using a crystal of 16Mhz
    //  performs a error ~0.0004% in one second
    //----------------------------------------------------------------------
	while (time!=0)
	{
		//compute time to make a delay with 1ms
		//below equation is computed take in consideration that (lo while) loop time 4 clocks in assembler
		t1Ms=F_CPU/4000;
		asm volatile("movw   r24,%A0	\t\n"	\
					 "_delayMsS10:		\t\n"	\
					"sbiw r24,1			\t\n"	\
					"brne _delayMsS10	\t\n"	\
					:"+r" (t1Ms)
					:
					:"r24","r25"
					);
//		do {
//			asm("nop");
//			t--;
//		} while (t!=0);
		time--;
	}
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
void	class_Time::DelayMsI(uint32_t timeMs)
{
	uint32_t tmp1,tmp2;

	tmp1=GetMilliseconds();             						//get actual tick value
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
        chrono_time=GetMilliseconds();
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
        return (GetMilliseconds()-chrono_time);				//Get difference between actual tick and save tick
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Callback function for clock                                  	|
//	| NAME    :  SetTimePumpCallback         					                    |
//	| INPUT   :  void (*callback)          						                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void    class_Time::SetTimeClockCallback(void (*TimePumpCallback)(uint8_t hour,uint8_t minute,uint8_t second))
{
    //avoid routines that get more than 720uS long to execute
    _time.TimeClockCallBack=TimePumpCallback;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Callback function for clock                                  	|
//	| NAME    :  SetTimeInterruptCallback      					                    |
//	| INPUT   :  void (*callback)          						                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void    class_Time::SetTimeInterruptCallback(void (*TimeInterruptCallback)(void))
{
	//avoid routines that get more than 720uS long to execute
	_time.TimeInterruptCallBack=TimeInterruptCallback;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Clock State ON(Running) or OFF(Stopped)                      	|
//	| NAME    :  SetClockState    	        					                    |
//	| INPUT   :  state ON or OFF          						                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void    class_Time::SetClockState(bool inUse=false)
{
        _time.TimeInUse=inUse;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Intrrupt State ON(Running) or OFF(Stopped)                    	|
//	| NAME    :  SetInterruptState   	        					                |
//	| INPUT   :  state ON or OFF          						                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void    class_Time::SetInterruptState(bool inUse=false)
{
	_time.InterruptInUse=inUse;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get Clock State ON(Running) or OFF(Stopped)                      	|
//	| NAME    :  GetClockState    	        					                    |
//	| INPUT   :  state ON or OFF          						                    |
//	| REMARKS :  			        				                                |
//	+-------------------------------------------------------------------------------+
bool   class_Time::GetClockState()
{
        return _time.TimeInUse;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get Interrupt State ON(Running) or OFF(Stopped)                   	|
//	| NAME    :  GetInterruptState 	        					                    |
//	| INPUT   :  state ON or OFF          						                    |
//	| REMARKS :  			        				                                |
//	+-------------------------------------------------------------------------------+
bool   class_Time::GetInterruptState()
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
void    class_Time::GetTime()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //clear interrupts
    cli();
    //Set Time
    Hour=_time.Hour;
    Minute=_time.Minute;
    Second=_time.Second;
    //Restore SREG
    SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get tmp var Hour                                                	|
//	| NAME    :  GetHour       	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  GetTime() must be called before           		                    |
//	+-------------------------------------------------------------------------------+
uint8_t class_Time::GetHour()
{
    return Hour;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get tmp var Minut                                                	|
//	| NAME    :  GetMinute    	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  GetTime() must be called before           		                    |
//	+-------------------------------------------------------------------------------+
uint8_t class_Time::GetMinute()
{
    return Minute;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get tmp var Second                                               	|
//	| NAME    :  GetSecond    	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  GetTime() must be called before           		                    |
//	+-------------------------------------------------------------------------------+
uint8_t class_Time::GetSecond()
{
    return Second;
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
    Hour=hourParm=_time.Hour;
    Minute=minuteParm=_time.Minute;
    Second=secondParm=_time.Second;
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
//	| FUNCTION:  Set tmp var Hour                                                	|
//	| NAME    :  SetHour       	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                            		                    |
//	+-------------------------------------------------------------------------------+
void class_Time::SetHour(uint8_t hourParm)
{
    Hour=hourParm;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set tmp var Minute                                               	|
//	| NAME    :  SetMinute    	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                            		                    |
//	+-------------------------------------------------------------------------------+
void class_Time::SetMinute(uint8_t minuteParm)
{
    Minute=minuteParm;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set tmp var Second                                                	|
//	| NAME    :  SetSecond    	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                            		                    |
//	+-------------------------------------------------------------------------------+
void class_Time::SetSecond(uint8_t secondParm)
{
    Second=secondParm;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Time Hour,Minute and Second form tmp time vars               	|
//	| NAME    :  SetTime       	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  Interrupts are disabled and restore        		                |
//	+-------------------------------------------------------------------------------+
void    class_Time::SetTime()
{
    uint8_t sreg;

    //save SREG
    sreg=SREG;
    //clear interrupts
    cli();
    //Set Time
    _time.Hour=Hour;
    _time.Minute=Minute;
    _time.Second=Second;
    //Restore SREG
    SREG=sreg;
}
#endif //#define _TIME_COMMON_CPP_
