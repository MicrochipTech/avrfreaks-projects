//TaskUSerInterface.c

#include <inttypes.h>

#include <..\libraries\avrx\avrx\avrx-signal.h>
#include <..\libraries\avrx\avrx\AvrX.h>

#include <avr\pgmspace.h>
#include <avr\interrupt.h>
#include <avr\signal.h>
#include <avr\sfr_defs.h>


#include "ProjectCommon.h"
#include "TaskUserInterface.h"
#include "GPSFlashRW.h"
#include "PCSerialIO.h"
#include "TextFormat.h"
#include "GPSSerialIO.h"
#include "UITripUpload.h"
#include "..\libraries\Watchdog\Watchdog.h"
#include "..\libraries\DataFlash\MultiThread\dataflash.h" 

static GPSLocation transferUILoc;	//buffer shared with GPS routines (incomming data)
GPSLocation UILoc={0,0,0,0};		//data copied from trasferUILoc
volatile bool bufferUpdated;		//GPS thread has updated transferUILoc

uint16_t gADCValue=0;

extern uint8_t gResetCause;			//cause of last reset

#define MAX_FLASH 	5				//max number of LED flashes for a notification
#define WDResetLimit 10				//software timeout before WDog resets. about 20s

TimerControlBlock UIdelay;


extern void UIUpdateDisplay(void);
static void CommandModeRequested(void);
static void SerialCommandsMode(void);

static void InitADC(void);
static void StartADCConversion(void);
static uint16_t GetADCResult(void);
static void GetBattStatus(uint16_t ADCValue);
static void FlashLEDS(void);
static void CheckWD(void);
static void ProcessMessage(void);



#define LED1  PD4						//status leds on these ports
#define LED2  PD5

static void StatusLED(uint8_t led, int8_t onOrOff);

extern Mutex waitForGUISignal;			//tell GPS thread UI is ready
extern Mutex flashInitialisedSignal;	//flash init'zd (but may not be working!)

bool     gCommitted=false;				//flag to indicate Flash has been comitted
uint16_t noCommsCount=0;				//counter to identify comms error, will be zeroed
										//when comms is received.
uint8_t led1State;						
uint8_t led2State;

uint8_t gReceiverWatchdog=0;			//coutner for software watchdog
bool    gForceReset=false;

uint16_t gAveBattADC=-1;				//Batt ADC value, -1 indicates invalid till 1st sample.


//-----------------------------------------------------------------------------------
// message called from the GPS task when it has a new GPSLocation structure
// copy it to a transfer buffer. The transfer buffer is read & copied by the
// UI thread at its leasure. The UI thread will check that the transfer buffer
// does not change while it was copying it.
// dont use strncmp as its not thread safe!
// If there is NO data from the GPS module to CPU, this method is not called - 
// so, if there is no call for a while then UI can assume the GPS module isn't 
// sending data - ie no serial comms. 
// If the GPS is getting bad satellite data and the comms is
// ok (from GPS module to CPU) then this method will be called, but the 
// .mode & .fix bits will indicate receiver GPS errors


void UIDisplayUpdate(const GPSLocation* loc)
{
	bufferUpdated = true;
//	transferUILoc =*loc;						//copy data, used for LCD display
}
//-----------------------------------------------------------------------------------
// Status messages from both Threads
// This method can be called from either thread and can be called async'ly.
// Wont bother with access semaphore as there shouldn't be a problem.
// The only visible problem is that if this is called while the NMEA message
// is sent out there me multiplexed output. Problem will menifest as a comms/NMEA
// checksum error. PC receiver should handle this ok. (The NEMA is sent out by the 
// GPS thread which does most of the output anyway - the UI thread sends few
// status messages, so the chances of a clash is low.

void GPSMsgStatus(uint16_t code)
{
	if (code == GPSStatus_Commit)				//set flag so other msgs dont obliterate this
		gCommitted = true;
		
	if (code == GPSStatus_CommsOK)				//no ui stuff, just indicates comms ok
	{
		noCommsCount=0;							
		return;
	}
	
	if ((code & LEDMaskID) == LEDStatus1)			//flash led 1 ?
	{
		led1State = code & LEDFlashMask;
		DBGPutc('a' + M_ExtractStatusCode(code));	//add status code to get character
	}
	else											//flash led 2
	{
		led2State = code & LEDFlashMask;
		DBGPutc('A' + M_ExtractStatusCode(code));
	}
}

//-----------------------------------------------------------------------------------
//Used to get the latest GPS data ready for displaying on the LCD.
//copy the message from the transfer buffer to this tasks buffer. Do this so that 
//the GPS thread is free to update it. With a shared buffer, this GPS thread might
//update it while this thread is processing it.
//the flag bufferUpdated indicates that while the copy took place, it has been updated and
//this thread will need to reacquire it. Must be a volatile.
//If there was no message from the GPS thread increment counter - this counter is used
//to indicate that the GPS thread isn't transmitting (ie its not getting any valid GPS 
//comms) - note this isn't that the GPS data is invalid - its that there is NO GPS comms.

static void CopyInMessage(void)
{
	if (!bufferUpdated)
	{
		if (noCommsCount < 0xFFFF)		//ensure no wrap to 0
			noCommsCount++;				//just a counter, need to convert to get secs
		
		return;
	}	
	noCommsCount=0;
	do
	{	
		bufferUpdated = false;		
		UILoc= transferUILoc;
	}
	while (bufferUpdated);
}

//-----------------------------------------------------------------------------------
//--------------------------- THE THREAD --------------------------------------------
//-----------------------------------------------------------------------------------
 
void TaskUserInterface(void)
{
		
	sbi(DDRD,PD4);					//status LED.  Led1 ddr output
	sbi(DDRD,PD5);					//led2
	sbi (PORTD,LED1);				//leds off
	sbi (PORTD,LED2);

	sbi(DDRB,PB7);								//Crumb128 Hardwired led - on to show wer're alive	
	cbi(PORTB,PB7);								//Will go off after init'ed (should be a v short flash)

	AvrXWaitSemaphore(&flashInitialisedSignal);	//wait for flash initialised

	sbi(PORTB,PB7);								//off now initalised
	
	CommandModeRequested();						//see if connected to PC
		
	AvrXSetSemaphore(&waitForGUISignal);		//tell TaskLogData GU ready. can start GPS & logging
	
//	InitADC();									//initialise ADC for the batt voltage

	WDStart(WD_2200ms);
	
	//Power up flash, flash green*5, red depends upon last MPU reset cause (see ATMega128 doco)
	
	led1State=0x05;	
	led2State=0x00;	
	uint8_t cause = gResetCause;							
	while (cause)
	{
		cause>>=1;
		led2State++;
	}
	FlashLEDS();				
	led2State=0;
		
	while (true)
	{
		//note: total loop period: 'LED flash time' + 500ms = 2000ms		

//		StartADCConversion();								//new ADC conversion for batt monitor
	
		FlashLEDS();										//this takes time...
		AvrXDelay(&UIdelay,MStoTickRate(500));				//more delay...
		
		CheckWD();
		++gReceiverWatchdog;								//counter is reset externally
		
//		GetBattStatus(GetADCResult());						//get batt voltage
						
//		CopyInMessage();									//new display data from GPS?
//		UIUpdateDisplay();									//LCD display
			
	}
}


//-----------------------------------------------------------------------------------
//Kick the watchdog unless the receiver has failed to reset its timeout.

static void CheckWD(void)
{
	if (( gReceiverWatchdog < WDResetLimit) && !gForceReset)
	{
		WDKick();
	}
}
//-----------------------------------------------------------------------------------
//flash the leds (1500ms to complete)
//see header for details of the flashing format

static void FlashLEDS(void)
{
	
	int8_t sLED1 = led1State;							//take copy as async change
	int8_t sLED2 = led2State;							//take copy as async change
	
	//clear the comms error messages, dont want to keep repeating them.
	//however, keep any fatal hardware state messages etc
	
	led1State=0;
	if (led2State != GPSStatus_HWFatalMask)
		led2State=0;
		
	if (gCommitted)										//commit takes led prioriry
	{
		gCommitted = false;								//indicate its been displayed
		sLED1 = 0x55;
	}
	if (noCommsCount++ >10)								//no comms counter & threshold
	{													//ie no rs232 data from GPS module
		sLED2 = GPSStatus_CommsFailMask;
		DBGPutc('X');
	}
	StatusLED(LED1,false);
	StatusLED(LED2,false);
	
	for (int8_t i=0;i<MAX_FLASH;i++)
	{
		StatusLED(LED1, (sLED1&0x0F)-i);				//t0 component
		StatusLED(LED2, (sLED2&0x0F)-i);				//t0 component
			
		AvrXDelay(&UIdelay,MStoTickRate(150));			//delay

		StatusLED (LED1, (sLED1>>4)-i);					//t1 component
		StatusLED (LED2, (sLED2>>4)-i);					//t1 component
		
		AvrXDelay(&UIdelay,MStoTickRate(150));			//delay
		CheckWD();
	}
}

//-----------------------------------------------------------------------------------

static void StatusLED(uint8_t led, int8_t onOrOff)
{
	if (onOrOff >0)
		cbi (PORTD,led);
	else
		sbi (PORTD,led);

}
//-----------------------------------------------------------------------------------
// see if route data upload to PC requested.

static void CommandModeRequested(void)
{

	AvrXDelay(&UIdelay,MStoTickRate(50));	//allow input to settle
		
	// look to see if there is a command comming from the PC
	// sample input stream for 2 secs
	// scan looking for a "#c" command.
	
	for (int i=0;i<20;i++)
	{
		AvrXDelay(&UIdelay,MStoTickRate(100));	//delay
		while (PCSIODataAvailable()>=2)
		{
			if ((PCSIOGetChar()=='#') && (PCSIOGetChar()== 'c'))
				return SerialCommandsMode();
		}	
	}
}

//-----------------------------------------------------------------------------------
// process commands as they arrive

static void SerialCommandsMode(void)
{	

	uint16_t lastTrip = FRWStateReset();
	bool     done=false;

	PCSIOPutStr_P(PSTR("Command Mode\n"));	//to PC
	
	
	while (!done)
	{
		while ( PCSIOGetChar() != '#')		//lose everything till #
		{
		}
		switch (PCSIOGetChar())
		{
			case 'u' : DBGPutStr_P(PSTR("Upload\n"));      UITripUpload(lastTrip--); break;		//upload trips
			case 'E' : DBGPutStr_P(PSTR("Erase Flash\n")); _UIEraseFlash();			 break;		//clear flash
			case 'q' : DBGPutStr_P(PSTR("Done\n"));        done = true;				 break;		//all done
			case 'b' : UIDisplayHeader(--lastTrip);	 								 break;		//last trip
			case 'f' : UIDisplayHeader(++lastTrip);	 								 break;		//next trip
			case 'h' : UIDisplayHeader(  lastTrip);	 								 break;		//current trip
		}
	}
}
//-----------------------------------------------------------------------------------


static void InitADC(void)
{
	#define ADC_PRESCALE_MASK		0x07
	#define ADC_PRESCALE_DIV128		0x07	///< 0x07 -> CPU clk/128
	#define ADC_CH_ADC3				0x03
	#define ADC_REFERENCE_AREF		0x00	///< 0x00 -> AREF pin, internal VREF turned off

	ADCSRA=0;
	ADMUX=0;							// default to right-adjusted result

	ADCSRA |= (1 << ADEN);
	ADCSRA |= ADC_PRESCALE_DIV128 ;
	
	ADMUX |= (ADC_REFERENCE_AREF<<6);	//actually does nothing for this option!
	ADMUX |= ADC_CH_ADC3;				// set channel

}
//-----------------------------------------------------------------------------------

static void StartADCConversion(void)
{
	ADCSR |= (1 << ADIF);	// clear hardware "conversion complete" flag 
	ADCSR |= (1 << ADSC);	// start conversion
}
//-----------------------------------------------------------------------------------

static uint16_t GetADCResult(void)
{
	return (inp(ADCL) | (inp(ADCH)<<8));	// read ADC (full 10 bits);
}
//-----------------------------------------------------------------------------------
//Takes the ADC value and calculates the battery reading over a number of samples.
// It's sample because level fluctuates with loads from system components.


static void GetBattStatus(uint16_t ADCValue)
{
	#define SampleSize		 5					//average sample over 10 secs (see call freq)
	#define WarningLevel     540				//ADC value to trigger batt low warning
	
	static uint16_t maxADC=0;
	static uint16_t minADC=-1;
	static uint8_t  sample=0;
	
		
	// get the sample size, so Batt value is upated only every n Samples
	
	if (sample++ <SampleSize)
	{
		if (ADCValue > maxADC) maxADC=ADCValue;		//save min & max ADC values.
		if (ADCValue < minADC) minADC=ADCValue;
	}
	else
	{
		//calc average and reset counters
		
		gAveBattADC = (maxADC+minADC)/2;			//ADC value in global, for use elsewhere
		maxADC=0;
		minADC=-1;
		sample=0;	
				
		if (gAveBattADC < WarningLevel)				//if low bat, send msg to status fn()
			GPSMsgStatus(GPSStatus_LowBatt);
		
	}	
		
}
