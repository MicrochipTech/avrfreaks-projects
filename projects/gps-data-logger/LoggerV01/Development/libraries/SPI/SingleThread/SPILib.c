
#include <avr\sfr_defs.h>
#include <io.h>
#include "SPILib.h"

static uint8_t SPITransferComplete= 0;
//----------------------------------------------------------------------

void SPIInitMaster(void)
{
	//setup I/O Pins
	sbi(PORTB,PB7); //SCK hi
	sbi(DDRB, PB7); //SCK out
	cbi(DDRB, PB6); //MISO in
	sbi(DDRB, PB5); //MOSI out
	sbi(DDRB, PB4); //ss out for master

	//SPI interface
	//clock f/16,+ve in middle of data, master mode, enable SPI
	//outp ((1<<MSTR) | (1<<SPE) | (1<<SPR0), SPCR);
	
	//outp((1<<CPHA)|(0<<CPOL)|(1<<MSTR)|(1<<SPE)|(0<<SPR0)|(1<<SPR1), SPCR );
	//outp((1<<CPHA)|(1<<CPOL)|(1<<MSTR)|(1<<SPE)|(1<<SPR0)|(0<<SPR1), SPCR );
	
	SPCR = (1<<CPHA)|(1<<CPOL)|(1<<MSTR)|(1<<SPE)|(1<<SPR0)|(0<<SPR1);
	
	// some other possible configs
	//clock f/128,, master mode, enable SPI
	//outp((1<<CPHA)|(1<<CPOL)|(1<<MSTR)|(1<<SPE)|(1<<SPR0)|(1<<SPR1), SPCR );
	//outp((1<<CPHA)|(1<<MSTR)|(1<<SPE)|(1<<SPR0), SPCR );
	
	inp(SPSR);	//clear status
	
	SPITransferComplete = 1;
	
	#ifdef SPI_USEINT
	sbi(SPCR,SPIE);
	#endif
}


//----------------------------------------------------------------------

void SPIInitSlave(void)
{
	// setup SPI I/O pins	
	cbi(DDRB, PB7);	// set SCK as input
	sbi(DDRB, PB6);	// set MISO as output
	cbi(DDRB, PB5);	// set MOSI as input
	cbi(PORTB,PB4);	//Set SS low
	cbi(DDRB, PB4);	// SS must be input for Slave mode to work

	// setup SPI interface :
	// clock = f/16
	// select clock phase positive-going in middle of data
	// slave mode
	// enable SPI
	
	SPCR = (1<<SPE)|(1<<SPR0);
	// some other possible configs
	//outp((1<<CPHA)|(1<<CPOL)|(1<<MSTR)|(1<<SPE)|(1<<SPR0)|(1<<SPR1), SPCR );
	//outp((1<<CPHA)|(1<<MSTR)|(1<<SPE)|(1<<SPR0), SPCR );
	
	// clear status
	inp(SPSR);
	SPITransferComplete = 1;

	// enable SPI interrupt
	#ifdef SPI_USEINT
	sbi(SPCR, SPIE);
	#endif
}

//----------------------------------------------------------------------

uint8_t SPITransferByte(uint8_t data)
{
	//send byte
	if (SPITransferComplete)
	{
		SPITransferComplete = 0;
		SPDR = data;
	}
	//wait for completion
	#ifdef SPI_USEINT
		while (!SPITransferComplete)
	#else
		while (!(SPSR &(1<<SPIF)))
			;
		SPITransferComplete = 1;
	#endif	
	
	return inp(SPDR);
}

//----------------------------------------------------------------------

uint8_t SPIReciveByte( uint8_t dataOut,uint8_t *dataIn)
{
	if(SPITransferComplete)
	{
		*dataIn = SPDR;
		SPDR = dataOut;
		return 1;
	}
	
	return 0;
}