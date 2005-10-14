
#include <avr\io.h>
#include <avr\pgmspace.h>
#include <avr\interrupt.h>
#include <avr\signal.h>
#include <..\libraries\avrx\avrx\AvrX.h>
#include <inttypes.h> 
#include <string.h>		//memset (its inline)
#include "ReceiverIO.h"
#include "GPSSerialio.h"
#include "PCSerialio.h"


static Mutex RxReady;
static Mutex TxReady;

static char bufferGSA[MaxMessageLen+1];				// io buffers, 1 per message type.
static char bufferGSV[MaxMessageLen+1];				// +1 for checksum.
static char bufferRMC[MaxMessageLen+1];
static char bufferGGA[MaxMessageLen+1];
static char msgTypeBuffer[NMEAMessageTypeLen];		// 5 chars just for msg type (eg "GPGSA")

static eReadMessage bufferBeingRead;				// id of the buffer the task is reading
static eReadMessage bufferBeingWritten;				// id of the buffer the ISR is writing
static eReadMessage lastFilledBuffer;				// id of the last buffer to be filled

uint8_t GPSSIObufferOverflow;						// overflow count
static char *bp;
static uint8_t wbi;									// current write buffer index
static bool lookingForDollar;						// looking for new message
static bool wantSatelliteInfo= false;				// get GSA & GSV messages?


static inline void GPSSIOISR(uint8_t ch);

#define BAUDRATE  4800L     						//  baudrate of GPS module
#define UBRR_INIT (CPUCLK/(16*BAUDRATE)-1)
//#define UCR_INIT  ((1<<TXEN) | (1<<RXEN) | (1<<RXCIE))
#define UCR_INIT  ((1<<RXEN) | (1<<RXCIE))

//---------------------------------------------------------------------------
// Initialize the USART hardware with the passed baud rate value.

void GPSInitSerialIO(void)
{
	wbi=0;
	lookingForDollar = true;
	
	memset(bufferGSA,0,sizeof(bufferGSA));		//produces inline code 
	memset(bufferGSV,0,sizeof(bufferGSV));		//produces inline code 
	memset(bufferRMC,0,sizeof(bufferRMC));		//produces inline code 
	memset(bufferGGA,0,sizeof(bufferGGA));		//produces inline code 
	
	bufferGSV[0]=bufferRMC[0]=bufferGGA[0]=bufferGSA[0]='G';
	bufferGSV[1]=bufferRMC[1]=bufferGGA[1]=bufferGSA[1]='P';	
	
	bufferGSA[2]='G';bufferGSA[3]='S';bufferGSA[4]='A';	
	bufferGSV[2]='G';bufferGSV[3]='S';bufferGSV[4]='V';
	bufferRMC[2]='R';bufferRMC[3]='M';bufferRMC[4]='C';
	bufferGGA[2]='G';bufferGGA[3]='G';bufferGGA[4]='A';	
	
	bufferBeingRead    = eNone;
	bufferBeingWritten = eNone;
	lastFilledBuffer   = eNone;
	
	GPSSIObufferOverflow=0;
	
	
	UBRR1L = UBRR_INIT;		//max 64K baud rate, with UBRRH=0
	UBRR1H = 0;
	UCSR1B = UCR_INIT;
	
}
//---------------------------------------------------------------------------

AVRX_SIGINT(SIG_UART1_RECV)
{
    IntProlog();			//context save
    cbi(UCSR1B, RXCIE);		//no serial interrupts (while processing)
    sei();          		//allow other (different) interrupts while processing char
      
	uint8_t c = inp(UDR1);	//always, otherwise interrupts keep going (till read)
	  
	GPSSIOISR(c);			// call the desired handler

	cli();					//dont want reenterent calls on this ISR (Epilog does
							//a RETI which enables interrutps). Though Epilog does a
							//cli almost straight away, if there is a char waiting
							//the next instruction (sbi(UCSR1B, RXCIE)) will trigger
							//a renenterent ISR call immediatally.
    sbi(UCSR1B, RXCIE); 	//enable more serial interrupts 
    Epilog();				//restore & reschedule (if necc)
}

//---------------------------------------------------------------------------

AVRX_SIGINT(SIG_UART1_DATA)
{
    IntProlog();
    cbi(UCSR1B, UDRIE);
    sei();                          
    AvrXIntSetSemaphore(&TxReady);  
    Epilog();
}
//---------------------------------------------------------------------------
// the ISR. 
// code here is run in an ISR, so needs t be short & sweet.

static void GPSSIOISR(uint8_t ch)
{
    
	static uint8_t checksum;
	static bool    calcChecksum;
	
	//## GPS data to PC serial out
	//Also see ReadAndProcessMessage(), for alternate output

	//PCSIOPutCharNoWait(ch);		//want to echo NMEA message data to serial port?
									//char out to 'PC' serial port (no waiting)
		
	
	if(lookingForDollar)
	{
		if (ch=='$')
		{
			//if we are about to write to the buffer the reader is reading
			//then dont, as this will corrupt their data.
			//backoff and look for the next message, and try again.
			
			if ( (bufferBeingRead != eNone) && (bufferBeingRead== bufferBeingWritten ))
				GPSSIObufferOverflow++;
			else
			{
				lookingForDollar = false;		//msg start, 
				checksum=0;
				calcChecksum = true;
				wbi=0;
			}
		}
		return;
	}
	
	if (wbi < NMEAMessageTypeLen)		// getting message type, record in special buffer
		msgTypeBuffer[wbi++]= ch;
	else
		bp[wbi++]=ch;					//store message boDy in appropriate buffer
	
	
	// calc checksum on the fly as the message is received
	// stop calc checksum when the '*' is received. 
	// checksum between $ and *, (excluding '$' & '*')
	
	if (ch == '*')
		calcChecksum = false;
	if (calcChecksum)
		checksum ^= ch;
	
	// got the message type, select the buffer for the rest of the message
	
	if (wbi == NMEAMessageTypeLen)
	{
		if  ((msgTypeBuffer[2] =='G') && (msgTypeBuffer[3]=='G') && (msgTypeBuffer[4] =='A')) 
		{
			bp = bufferGGA;
			bufferBeingWritten = eGGA;
		}
		else
		if  ((msgTypeBuffer[2] =='R') && (msgTypeBuffer[3]=='M') && (msgTypeBuffer[4] =='C'))
		{
			bp = bufferRMC;
			bufferBeingWritten =eRMC;
		}			 
		else
		// want satellite info? if so, check 1st two chars as they are the same for these msgs
		
		if  (wantSatelliteInfo && (msgTypeBuffer[2] =='G') && (msgTypeBuffer[3]=='S') ) 
		{
			if ((msgTypeBuffer[4] =='A'))
			{
				bp = bufferGSA;
				bufferBeingWritten = eGSA;
			}
			else
			if ((msgTypeBuffer[4] =='V'))
			{
				bp = bufferGSV;
				bufferBeingWritten =eGSV;
			}
		}
		lookingForDollar = bufferBeingWritten==eNone;
		return;
	}
	
	//will only pass message on to app if its wanted.
	//ie, will only get here if it one we want, or still builing message
	
	if (wbi>= MaxMessageLen)					//buffer overflow
	{
		lookingForDollar = true;				//give up, find next message(unless ch==lf)
		bufferBeingWritten =eNone;
	}
	if (ch == LF)								//end of message?
	{
		if (wbi > NMEAMessageTypeLen)			//check its valid message
		{
			bp[wbi++]='\0';
			bp[wbi]  = checksum;				//store calc'td checksum after terminator

			lastFilledBuffer =bufferBeingWritten;//latest filled buffer
			AvrXIntSetSemaphore(&RxReady);		//tell app of new message
		}
		lookingForDollar = true;				//look for next message start
		bufferBeingWritten =eNone;
	}	
}

//---------------------------------------------------------------------------
// Wait if nedd & return the ID of the buffer just filled.
// This means that the caller must keep up with the speed at which the buffers are
// being filled. If it lags behind then it will skip a buffer. The caller should
// be aware of the expected order and manage this if it happens.

eReadMessage GPSSIOGetNMEAMessage(void)
{
	AvrXWaitSemaphore(&RxReady);
	bufferBeingRead = lastFilledBuffer;		//remember which one caller has
											//this allows for checking r/w clash.
	return bufferBeingRead;	
}
//---------------------------------------------------------------------------

void GPSIOReleaseNMEABuffer(void)
{
	bufferBeingRead = eNone;
}
//---------------------------------------------------------------------------
// once the caller has the ID of the buffer, It calls this method to get a 
// pointer to the buffer.

char * GPSIOGetBufferPtr(eReadMessage bufferId)
{
	switch (bufferId)
	{
		case eGSA : return bufferGSA;
		case eGSV : return bufferGSV;
		case eRMC : return bufferRMC;
		case eGGA : return bufferGGA;
		case eNone: return 0;
	}
	return 0;	//oh dear
}
//---------------------------------------------------------------------------
// UI calls this to tell the ISR to record the GSA & GSV messages.
// Can be toggled to reduce CPU usage.

void GPSIOWantSatelliteInfo(bool yes)
{
	wantSatelliteInfo = yes;
}
