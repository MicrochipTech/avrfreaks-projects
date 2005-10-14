#include <inttypes.h> 
#include <avr\pgmspace.h>
#include <avr\interrupt.h>
#include <avr\signal.h>
#include <..\libraries\avrx\avrx\AvrX.h>
#include "TextFormat.h"
#include "PCSerialIO.h"
#include "ProjectCommon.h"

//for 38k
//#define BAUDRATE  38400L     // serial io baudrate
//#define UBRR_INIT (CPUCLK/(16*BAUDRATE)-1)
//#define UCSR0A_INIT 0

//for 115K with 14.7456MHz CPU
#define UBRR_INIT 15
#define UCSR0A_INIT (1<<U2X0)

#define UCR_INIT  ((1<<TXEN) | (1<<RXEN) | (1<<RXCIE))

static Mutex RxReady;
static Mutex TxReady;

#define BUFFERSZ 128
static char buffer[BUFFERSZ];
static uint8_t wbi;			//write buffer index;
static uint8_t rbi;			//read buffer index
static volatile uint8_t count;

uint8_t  SIOOverflow;		//buffer overflow(debug)

static inline void PCSIOISR(uint8_t ch);
static char GetCharSkipSpaces(void);
static bool CharInStr(char c, const unsigned char *EOLTerminators);


//---------------------------------------------------------------------------
// Initialize the USART hardware with the passed baud rate value.

void PCSIOInitSerialIO(void)
{
	wbi=0;
	rbi=0;
	count=0;
	SIOOverflow=0;
	
	UBRR0L = UBRR_INIT;		
	UBRR0H = 0;
	UCSR0A = UCSR0A_INIT;
	UCSR0B = UCR_INIT;
    
}
//---------------------------------------------------------------------------

AVRX_SIGINT(SIG_UART0_RECV)
{
    IntProlog();			//context save
    cbi(UCSR0B, RXCIE);		//no serial interrupts (while processing)
    sei();          		//allow other interrupts while processing char
      
	PCSIOISR(inp(UDR0));	// call the desired handler

	cli();					//dont want reenterent calls on this ISR (Epilog does
							//a RETI (which enables interrutps). Infact Epilog does a
							//cli almost straight away, but, if there is a char waiting
							//the next instruction (sbi(UCSR0B, RXCIE)) will trigger
							//a renenterent ISR call immediatally
    sbi(UCSR0B, RXCIE);		//enable more serial interrupts 
    Epilog();				//restore & reschedule (if necc)
}

//---------------------------------------------------------------------------

AVRX_SIGINT(SIG_UART0_DATA)
{
    IntProlog();
    cbi(UCSR0B, UDRIE);
    sei();                          
    AvrXIntSetSemaphore(&TxReady);  
    Epilog();
}
//---------------------------------------------------------------------------
// the ISR
static void PCSIOISR(uint8_t ch)
{
                           
	buffer[wbi] = ch;
	count++;
	if (count>BUFFERSZ) SIOOverflow++;
	
	if (wbi == BUFFERSZ-1)
		wbi=0;
	else
		wbi++;
		
    AvrXIntSetSemaphore(&RxReady);
}

//---------------------------------------------------------------------------
// Get num chars in the input stream/buffer

uint8_t PCSIODataAvailable(void)
{
	return count;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

int8_t PCSIOPutChar(char d)
{

    if ((inp(UCSR0A) & BV(UDRE)) == 0)
    {
        sbi(UCSR0B, UDRIE);
        AvrXWaitSemaphore(&TxReady);
    }
    outp(d, UDR0);
	
	return 0;

}
//---------------------------------------------------------------------------
// read character from input buffer

char PCSIOGetChar(void)
{
	if (count == 0)
		AvrXWaitSemaphore(&RxReady);
		
    char ch = buffer[rbi];
	cli();
	{
		count--;
	}
	sei();
	if (rbi == BUFFERSZ-1)
		rbi=0;
	else
		rbi++;

	return ch;
}
//---------------------------------------------------------------------------

void PCSIOPutNum(int32_t value)
{
	PCSIOPutStr(LtoaN(value,DigitsLong));
}
//---------------------------------------------------------------------------

void PCSIOPutStr(const char *str)
{
	while (*str)
		PCSIOPutChar(*str++);
}

//---------------------------------------------------------------------------
//Display a string (string in Program memory) and a 32 bit number

void PCSIOPutStrNum_P(const char *progmemPtr, int32_t n)
{
	PCSIOPutStr_P(progmemPtr);
	PCSIOPutNum(n);
	PCSIOPutChar('\n');
}
 
//---------------------------------------------------------------------------
//Display a string (string in Program memory)

void PCSIOPutStr_P(const char *progmemPtr)
{
	char c;

    while ( (c = pgm_read_byte(progmemPtr) ) )
	{
		progmemPtr++;
		PCSIOPutChar(c);
	}
}
//---------------------------------------------------------------------------
// Read string. Terminator list is supplied and is in program memory (hence the _P)

void PCSIOGetStr_P(char *buffer, const unsigned char *EOLTerminators, uint8_t maxlen)
{
	char c;

	if (EOLTerminators==0)
		EOLTerminators= (char*)PSTR(" \n\r");
		
	c=GetCharSkipSpaces();
 
	while (!CharInStr(c,EOLTerminators))
	{
		if (maxlen)
		{
			*buffer++ =c;
			maxlen--;
		}
		c =PCSIOGetChar();
	}
	if (maxlen)
		*buffer='\0';
}
//---------------------------------------------------------------------------

int16_t PCSIOGetInt(void)
{
	return (int16_t)PCSIOGetLong();
}
//---------------------------------------------------------------------------

int32_t PCSIOGetLong(void)
{
	int32_t lv=0;
	bool neg = false;
	char c;
	
	c=GetCharSkipSpaces();
	
	if (c=='-') 
	{	
		neg=true;
		c=PCSIOGetChar();
	}
	if (c=='+')
		c=PCSIOGetChar();
		
	while (c>='0' && c<='9')
	{
		lv=lv*10 + (c-'0');
		c=PCSIOGetChar();		
	}	
	if (neg) 
		lv=-lv;
		
	return lv;	
}
//---------------------------------------------------------------------------

static char GetCharSkipSpaces()
{
	char c=PCSIOGetChar();
	while (c==' ')
		c=PCSIOGetChar();
	return c;
}

//---------------------------------------------------------------------------
// determine is character is in specified string (string in program memory)

static bool CharInStr(char c, const unsigned char *EOLTerminators)
{
	while (pgm_read_byte(EOLTerminators))
	{
		if (pgm_read_byte(EOLTerminators)==c)
			return true;
		EOLTerminators++;
	}
	return false;
}