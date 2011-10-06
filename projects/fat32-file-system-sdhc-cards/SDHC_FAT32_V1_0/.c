/*
	SD card routines
	Block size must be 512bytes, card must be SDHC
*/

#include <includes.h>

//remove if needed
#define	DEBUG_ON

// **** Private prototypes ****

	static char command_resetidle(void);
	static char command_ocr_read(unsigned long *ocr);
	static char command_initialize(void);
	static char command_sendifc(void);
	static char command_block_write(char* buffer, unsigned long blockadd);
	static char command_block_verify(char* buffer,unsigned long blockadd);

	static char response_r1(void);
	static char transfer(char data);
	static unsigned long transfer_long(unsigned long);

// **** Public variables ****

	unsigned char 	sd_version=0;
	unsigned char 	sd_sdhc=FALSE;

	struct fifo_control *sd_debug_fifo_ptr=NULL;

// **** Hardware configuration ****

#define CLK_DDR		DDRB
#define MISO_DDR	DDRB
#define MOSI_DDR	DDRB
#define SS_DDR		DDRD
#define CLK_PORT	PORTB
#define MISO_PORT	PORTB
#define MOSI_PORT	PORTB
#define SS_PORT		PORTD
#define CLK_PIN		PINB
#define MISO_PIN	PINB
#define MOSI_PIN	PINB
#define SS_PIN		PIND
#define CLK_BIT		PB1
#define MISO_BIT	PB3
#define MOSI_BIT	PB2
#define SS_BIT		PD7

// **** Local defines ****
#ifdef	DEBUG_ON

	static char text[20];
	#define DEBUG_P(arg1)		fifo_write_string_P(sd_debug_fifo_ptr, arg1)
	#define DEBUG(arg1)			fifo_write_string(sd_debug_fifo_ptr, arg1)
	#define DEBUG_CHAR(arg1)	fifo_write_char(sd_debug_fifo_ptr, arg1)
	#define DEBUG_INT(arg1)		fifo_write_string(sd_debug_fifo_ptr, itoa(arg1, text, 10))
	#define DEBUG_LONG(arg1)	fifo_write_string(sd_debug_fifo_ptr, ltoa(arg1, text, 10))

#else

	#define DEBUG_P(arg1)		
	#define DEBUG(arg1)			
	#define DEBUG_CHAR(arg1)	
	#define DEBUG_INT(arg1)		
	#define DEBUG_LONG(arg1)	
#endif

//OCR bits
#define OCR_27_28	15
#define OCR_28_29	16
#define OCR_29_30	17
#define OCR_30_31	18
#define OCR_31_32	19
#define OCR_32_33	20
#define OCR_33_34	21
#define OCR_34_35	22
#define OCR_35_36	23
#define OCR_SDHC	30

//R1 response bits
#define R1_IDLE						0
#define R1_ERROR_ERASE_RESET		1
#define R1_ERROR_ILLEGAL_COMMAND	2
#define R1_ERROR_CRC				3
#define R1_ERROR_ERASE_SEQ			4
#define R1_ERROR_ADDRESS			5
#define R1_ERROR_PARAMETER			6

#define TIMEOUT_R1_RESPONSE			5000
 
void sd_init(void)
{

	CLK_PORT	&=~_BV(CLK_BIT);
	SS_PORT		|=_BV(SS_BIT);
	MOSI_PORT	|=_BV(MOSI_BIT);
	MISO_PORT	|=_BV(MISO_BIT);
	
	CLK_DDR		|=_BV(CLK_BIT);
	MISO_DDR	&=~_BV(MISO_BIT);
	MOSI_DDR	|=_BV(MOSI_BIT);
	SS_DDR		|=_BV(SS_BIT);

	PRR0 &=~_BV(PRSPI);
	DDRB |=_BV(PB0);
	PORTB |=_BV(PB0);

	SPCR |=_BV(CPOL);	//leading edge is falling
	SPCR |=_BV(CPHA);	//setup on leading edge
	
	SPCR |=_BV(MSTR);	//master
	SPCR |=_BV(SPR0)+_BV(SPR1);	//fosc/64 (until after setup)
	SPCR |=_BV(SPE);
	SPSR |=_BV(SPI2X);
}

char sd_initcard(void)
{
	unsigned long ocr=0;
	char success=FALSE;

	SPCR |=_BV(SPR0)+_BV(SPR1);	//fosc/64 (until after setup)

	sd_sdhc=FALSE;
	
	success=command_resetidle();

	if(success)
	{
		sd_version=2;
		success=command_sendifc();
		if(!success)
			sd_version=1;
	};

	success=command_initialize();

	if(success && sd_version==2)
	{
		success=command_ocr_read(&ocr);
		if(success && (ocr & (1ul<<OCR_SDHC)))
			sd_sdhc=TRUE;
	};
	
	if(sd_version==1)
		DEBUG_P(PSTR("SD Version 1.xx\r\n"));
	else
		DEBUG_P(PSTR("SD Version 2.00 or later\r\n"));
	
	if(success)
		SPCR &=~(_BV(SPR0)+_BV(SPR1));	//fosc/2

	return success;
}

char sd_block_read(char* buffer, unsigned long blockadd)
{
	char success=FALSE;
	int tempint;
	unsigned char retry=3;

	do
	{
		if(sd_sdhc==FALSE)			//if not SDHC, convert block address to byte address
			blockadd<<=9;
		
		SS_PORT &=~_BV(SS_BIT);		//select
		transfer(17 | 0x40);		//CMD17 read single block
		transfer_long(blockadd);	//length
		transfer(0x00);				//checksum
		
		if( (response_r1() & ~_BV(R1_IDLE)) == 0)	//if all error bits are clear
			success=TRUE;
		
		transfer(0xFF);
		SS_PORT |=_BV(SS_BIT);		//de-select
		
		if(success)
		{
			SS_PORT &=~_BV(SS_BIT);		//select
			
			//wait for start token
			tempint=10000;
			success=FALSE;
			do
			{
				if(transfer(0xFF)==0xFE)
					success=TRUE;
				else
					tempint--;
			}while(tempint && !success);
			
			if(success)
			{
				//read block
				tempint=512;
				
				SPDR=0xFF;							//begin first transfer
				while(tempint)
				{
					while( !(SPSR & _BV(SPIF)) );	//wait for transfer to finish
					*buffer=SPDR;			//read byte
					SPDR=0xFF;				//begin next transfer
					buffer++;				//modify pointer
					tempint--;				//modify counter
				};
				
				while( !(SPSR & _BV(SPIF)) );	//wait for additonal transfer
				success=SPDR;					//waste data (actually first byte of CRC)
				success=TRUE;
			};
			transfer(0xFF);		//ignore CRC
			
			transfer(0xFF);		//additional clocks before de-select
			transfer(0xFF);		//additional clocks before de-select
			SS_PORT |=_BV(SS_BIT);		//de-select
		};
		
		if(!success)
		{
			DEBUG_P(PSTR("SD COMMAND READ BLOCK FAILED "));
			DEBUG_P(PSTR("@SECTOR "));
			DEBUG_LONG(blockadd);
			DEBUG_P(PSTR("\r\n"));
			sd_initcard();
		};
	}while(!success && retry--);

	return success;	
}

char sd_block_write(char* buffer, unsigned long blockadd)
{
	int retry=3;
	char success;
	
	retry=3;
	do
	{
		success=command_block_write(buffer, blockadd);
		if(success)
			success=command_block_verify(buffer, blockadd);
		if(!success)
		{
			sd_initcard();
			retry--;
		};
	}while(!success && retry);
	
	return success;
}

static char command_resetidle(void)
{
	int tempint;
	char success=FALSE;

	DEBUG_P(PSTR("SD COMMAND RESET/IDLE\r\n"));

	//send 80 clock pulses
	tempint=10;
	while(tempint--)
		transfer(0xFF);

	SS_PORT &=~_BV(SS_BIT);	//select
	
	transfer(0x00 | 0x40);		//CMD00 (reset/idle)
	transfer_long(0x00000000);
	transfer(0x95);				//checksum 95

	if(response_r1()==_BV(R1_IDLE))
		success=TRUE;

	transfer(0xFF);		//additional clocks before de-select
	transfer(0xFF);
	
	SS_PORT |=_BV(SS_BIT);	//de-select

	if(!success)
		DEBUG_P(PSTR("SD COMMAND RESET/IDLE FAILED.\r\n"));
	return success;
}

static char command_sendifc(void)
{
	char success=FALSE;

	DEBUG_P(PSTR("SD COMMAND SEND IFC\r\n"));
	SS_PORT &=~_BV(SS_BIT);	//select
	
	transfer(0x08 | 0x40);	//CMD8 
	transfer(0x00);
	transfer(0x00);
	transfer(0x01);	//2.7-3.3V
	transfer(0xAA);	//10101010 check pattern
	transfer(0x87);	//checksum for CMD8

	if( (response_r1() & ~_BV(R1_IDLE)) == 0)	//if all error bits are clear
	{
		transfer(0xFF);	//ignore R7 response bits 31-28 (command version), and 27-24 (reserved)
		transfer(0xFF); //ignore R7 response bits 23-16 (reserved)
		if((transfer(0xFF) & 0x0F) == 0x01) //ignore R7 response bits 15-12 (reserved), bits 11-8 should == 1 if voltage is accepted (it will be)
		{
			if(transfer(0xFF)==0xAA)
				success=TRUE;
		};
	};
	
	transfer(0xFF);		//additional clocks before de-select
	transfer(0xFF);
	SS_PORT |=_BV(SS_BIT);	//de-select
	
	if(!success)
		DEBUG_P(PSTR("SD COMMAND SEND IFC FAILED.\r\n"));
		
	return success;
}

static char command_ocr_read(unsigned long *ocr)
{
	char success=FALSE;
	unsigned long templong;
	int tempint;
	
	DEBUG_P(PSTR("SD COMMAND READ OCR\r\n"));
	SS_PORT &=~_BV(SS_BIT);	//select
	transfer(58 | 0x40);	//CMD58
	transfer_long(0x00000000);	//0 stuff bits
	transfer(0x00);			//checksum

	if( (response_r1() & ~_BV(R1_IDLE)) == 0)	//if all error bits are clear
	{
		success=TRUE;
		*ocr=transfer_long(0xFFFFFFFF);	
	}
	else
		transfer_long(0xFFFFFFFF);

	transfer(0xFF);		//additional clocks before de-select
	transfer(0xFF);
	SS_PORT |=_BV(SS_BIT);	//de-select	

	if(!success)
		DEBUG_P(PSTR("SD COMMAND READ OCR FAILED.\r\n"));
	else
	{
		DEBUG_P(PSTR("SD COMMAND OCR: SDHC="));
		if(*ocr & (1ul<<OCR_SDHC))
			DEBUG_P(PSTR("YES  "));
		else
			DEBUG_P(PSTR("NO  "));
		
		templong=*ocr;
		templong>>=15;
		tempint=27;		
		while(!templong & 1)
		{
			templong>>=1;
			tempint++;
		};
		DEBUG_INT(tempint);
		DEBUG_CHAR('-');
		
		while(templong & 1 && tempint <36)
		{
			templong>>=1;
			tempint++;
		};
		DEBUG_INT(tempint);
		DEBUG_P(PSTR("\r\n"));
		
	};
	return success;
}

static char command_initialize(void)
{
	char success=FALSE, response, finished=FALSE;
	int retry=1000;

	DEBUG_P(PSTR("SD COMMAND INITIALIZE\r\n"));

	do
	{
		//CMD 55
		SS_PORT &=~_BV(SS_BIT);	//select
		transfer(55 | 0x40);	//CMD55
		transfer_long(0x00000000);	//0 stuff bits
		transfer(0x00);			//checksum
		if((response_r1() & ~_BV(R1_IDLE)) != 0)	//if any error bits are set
		{
			finished=TRUE;
			success=FALSE;
		};
		transfer(0xFF);
		transfer(0xFF);		//additional clocks before de-select
		SS_PORT |=_BV(SS_BIT);	//de-select
		
		//CMD41
		if(!finished)
		{
			SS_PORT &=~_BV(SS_BIT);	//select
			transfer(41 | 0x40);	//CMD41
			transfer_long(0x40000000);	//?? unknown parameter??
			transfer(0x00);			//checksum
			response=response_r1();			
			transfer(0xFF);
			transfer(0xFF);		//additional clocks before de-select
			SS_PORT |=_BV(SS_BIT);	//de-select
			
			if((response & ~_BV(R1_IDLE)) != 0)	//if any error bits are set
			{
				success=FALSE;
				finished=TRUE;
			}
			else if(response==0)	//if busy bit is clear
			{
				success=TRUE;
				finished=TRUE;
			};
		};
		
		if(!finished)
		{
			retry--;
			if(retry==0)
			{
				finished=TRUE;
				success=FALSE;
			};
		};
		
	}while(!finished);
	
	if(!success)
		DEBUG_P(PSTR("SD COMMAND INITIALIZE FAILED.\r\n"));
	
	return success;
}

//same as reading a block, only it verifys instead of reading
static char command_block_verify(char* buffer, unsigned long blockadd)
{
	char success=FALSE;
	int tempint;

	if(sd_sdhc==FALSE)		//if not SDHC, convert block address to byte address
		blockadd<<=9;

	SS_PORT &=~_BV(SS_BIT);		//select
	transfer(17 | 0x40);		//CMD17 read single block
	transfer_long(blockadd);	//length
	transfer(0x00);				//checksum

	if( (response_r1() & ~_BV(R1_IDLE)) == 0)	//if all error bits are clear
		success=TRUE;

	transfer(0xFF);
	transfer(0xFF);		//additional clocks before de-select
	SS_PORT |=_BV(SS_BIT);		//de-select

	if(success)
	{
		SS_PORT &=~_BV(SS_BIT);		//select
		
		//wait for start token
		tempint=10000;
		success=FALSE;
		do
		{
			if(transfer(0xFF)==0xFE)
				success=TRUE;
			else
				tempint--;
		}while(tempint && !success);
		
		if(success)
		{
			//read block
			tempint=512;
			
			while(tempint--)
			{
				if(*buffer != transfer(0xFF))
					success=FALSE;
				buffer++;
			};
		};
		transfer(0xFF);		//ignore CRC
		transfer(0xFF);
		
		transfer(0xFF);
		transfer(0xFF);				//additional clocks before de-select
		SS_PORT |=_BV(SS_BIT);		//de-select
	};
	
	if(!success)
	{
		DEBUG_P(PSTR("SD COMMAND VERIFY BLOCK FAILED "));
		DEBUG_P(PSTR("@SECTOR "));
		DEBUG_LONG(blockadd);
		DEBUG_P(PSTR("\r\n"));
	};

	
	return success;	
}

//command to write a block, this will not return failed if the card is locked
//call command_block_verify() afterwards, this will fail if the card is locked or the write failed
static char command_block_write(char* buffer, unsigned long blockadd)
{
	char success=FALSE;
	char error=0;
	int tempint;
	long templong;

	if(sd_sdhc==FALSE)		//if not SDHC, convert block address to byte address
		blockadd<<=9;

	SS_PORT &=~_BV(SS_BIT);		//select
	transfer(24 | 0x40);		//CMD24 write single block
	transfer_long(blockadd);	//
	transfer(0x00);				//checksum

	if( (response_r1() & ~_BV(R1_IDLE)) == 0)	//if all error bits are clear
		success=TRUE;
	else
		error=1;

	transfer(0xFF);
	transfer(0xFF);		//additional clocks before de-select
	SS_PORT |=_BV(SS_BIT);		//de-select

	if(success)
	{
		SS_PORT &=~_BV(SS_BIT);		//select
	
		transfer(0xFE);				//send start token

		//write block
		tempint=512;
		
		while(tempint--)
		{
			transfer(*buffer);
			buffer++;
		};
		
		//send dummy CRC
		transfer(0xFF);
		transfer(0xFF);
		
		//if transfer not successful
		if((transfer(0xFF) & 0x1F) != 0x05)
		{
			success=FALSE;
			error=2;
		}
		else
		{
			//wait for write to complete
			templong=1000000ul;
			while(transfer(0xFF)==0x00 && --templong);
			if(!templong)
			{
				success=FALSE;	//timeout
				error=3;
			};
		};
		
		transfer(0xFF);
		transfer(0xFF);		//additional clocks before de-select
		SS_PORT |=_BV(SS_BIT);		//de-select
	};
	
	if(!success)
	{
		DEBUG_P(PSTR("SD COMMAND WRITE BLOCK FAILED "));
		DEBUG_P(PSTR("@SECTOR "));
		DEBUG_LONG(blockadd);
		DEBUG_P(PSTR(" err "));
		DEBUG_INT((int)error);
		DEBUG_P(PSTR("\r\n"));
	};
	
	return success;	
}

//Waits for R1 response, returns 0xFF if timeout
static char response_r1(void)
{
	int retry = TIMEOUT_R1_RESPONSE;
	char response;

	while(retry--)
	{
		response=transfer(0xFF);
		if(!(response & _BV(7)))	//if bit7 is clear, then this is the response.
			retry=0;
		else
			retry--;			
	};

	return response;
}

static unsigned long transfer_long(unsigned long data)
{
	unsigned long retval;
	
	retval=transfer(data>>24);
	retval<<=8;
	retval+=transfer(data>>16);
	retval<<=8;
	retval+=transfer(data>>8);
	retval<<=8;
	retval+=transfer(data);
	
	return retval;
}

static char transfer(char data)
{
	char retval;
	
	SPDR=data;
	while( !(SPSR & _BV(SPIF)) );
	retval=SPDR;

	return retval;
}

/*
//BIT BANGING TRANSFER
static char transfer(char data)
{
	char retval=0;
	unsigned char bitcount=8;

//	DEBUG_P(PSTR("->"));
//	DEBUG_INT((int)(unsigned char)data);

	while(bitcount)
	{		
		
		//clock low and setup mosi
		CLK_PORT &=~_BV(CLK_BIT);
		if(data&0x80)
			MOSI_PORT |=_BV(MOSI_BIT);
		else
			MOSI_PORT &=~_BV(MOSI_BIT);
		data<<=1;
		
		//clock low delay
		_delay_us(3);
		
		//clock high and sample miso
		CLK_PORT |=_BV(CLK_BIT);
		retval<<=1;
		if(MISO_PIN & _BV(MISO_BIT))
			retval |= 1;
			
		//clock high delay
		_delay_us(3);
		
		bitcount--;
	};

//	DEBUG_P(PSTR("  <-"));
//	DEBUG_INT((int)(unsigned char)retval);
//	DEBUG_P(PSTR("\r\n"));
	return retval;
}
*/
