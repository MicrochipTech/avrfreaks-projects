
//Multithread version of SPI
//Semaphore is used to gain access.
//The SPI control registers are reset if the last user isn't the one gaining access - this
//facilitates more than one thread using the SPI. It is for the caller to
//manage its device wrt to chip select etc - otherwise there will be contention on the SPI
//lines, ie it must call SPITake then set the device specific chip select. And deselect the 
//device cs before calling SPIRelease().

//SPIInitMaster() must be called in main() before threads use SPI

#include <avr\sfr_defs.h>
#include <avr\io.h>

#include <..\libraries\avrx\avrx\avrx-io.h>
#include <..\libraries\avrx\avrx\avrx-signal.h>
#include <..\libraries\avrx\avrx\AvrX.h>

#include "..\libraries\SPI\MultiThread\SPILib.h"


//if not defined below, then they need defining for the specific AVR

#if	defined (__AVR_ATmega64__)  ||   defined (__AVR_ATmega64__) ||\
	defined (__AVR_ATmega128__) ||   defined (__AVR_ATmega128__) 
	#define SPIPORT   PORTB
	#define SPIPORTDD DDRB
	#define SS   	  PB0
	#define SCK  	  PB1
	#define MOSI 	  PB2
	#define MISO 	  PB3
#endif


static Mutex mutex;
static SPIControlBlock* lastUser =0;	//last user, if different reset SPI
static uint8_t initialised= 0;

//----------------------------------------------------------------------
//Gain access to the SPI
//If the last user isnt this one then reset the SPI control registers
//'cb' contains the threads desired SPI configuration - make this passed structure a
//static address as the a difference in address is used to determine if the last 
//caller is this caller.

//Caller MUST release with SPIRelease()

void SPITake(SPIControlBlock* cb)
{
	AvrXWaitSemaphore(&mutex);
	
	if (lastUser != cb)				//need to reset SPI if not last user
	{	
		SPCR=0;
		
		// set sck initial state depending upon callers clock polarity
		
		if (cb->ConfigSPCR & (1<<CPOL))	
			sbi(SPIPORT,SCK); 		//SCK hi
		else
			cbi(SPIPORT,SCK); 		//SCK lo
	
		SPSR = cb->ConfigSPSR;		//reset to callers config
		SPCR = cb->ConfigSPCR;		//reset to callers config
		inp(SPSR);					//clear status
		lastUser = cb;
	}
		
}
//----------------------------------------------------------------------

void SPIRelease(void)
{
	AvrXSetSemaphore(&mutex);
}
//----------------------------------------------------------------------
// Initialise the SPI ports.
// Must be called before any SPI activity. (call only once, eg from main())

void SPIInit(void)
{
	if (initialised)		//do this to prevent more than one init (esp wrt semaphore)
		return;
	
	initialised = 1;
	
	SPCR=0;					//disable SPI, is reset in the SPITake
	
	//setup I/O Pins (really necc, as setting up SPI may this ??)

	sbi(SPIPORTDD, SCK);  //SCK out
	cbi(SPIPORTDD, MISO); //MISO in
	sbi(SPIPORTDD, MOSI); //MOSI out
	sbi(SPIPORTDD, SS  ); //ss out for master (but a general IO pin when/as SPI is master)
		
	AvrXSetSemaphore(&mutex);	//must be set for mutex to start/operate correctly
}

//----------------------------------------------------------------------

uint8_t SPITransferByte(uint8_t data)
{
	
	SPDR = data;		//send byte
	
	while (!(SPSR &(1<<SPIF)))
	{
	}
	return inp(SPDR);
}
//----------------------------------------------------------------------

uint8_t SPIReciveByte( uint8_t dataOut)
{
	uint8_t dataIn = SPDR;
	SPDR = dataOut;
	return dataIn;	
}