// From J. Herberts library LQ

#define DB_INFO 0		// activation of DBprintf(( ... )); logging of operation
#define DB_ENABLED 0	// activation of debugging logging of occured faults

#include <heport.h>		// special type names, uint16_t, int16_t ...
#include <helq.h>		// uprintf
#include <tshar.h>
#include <tsser.h>		// prototypes for all functions defined here



#ifdef MDOS40	// When MDOS40, then select if vt or serial
// #define MDOS40_COM1
#define MDOS40_VT
#endif

#ifdef __TURBOC__
#include <dos.h>
#endif


#ifdef MDOS40_VT
#include <hevt.h>
#endif

char tsser_charbuffer[TSSER_NCHARBUFFER] ;


// ---------------------------------------- AVR begin
#ifdef AVR


#ifndef UCSRB
#define UCSRA UCSR0A
#define UCSRB UCSR0B
#define UDR UDR0
#define UBRRL UBRR0L
#define UBRRH UBRR0H
#endif

FUNCT void tsseropen( void )		// AVR
// initalize the serial Port
{
#define BAUD 9600L



#define UBRRinitval (((F_CPU+8L*BAUD)/(16L*BAUD))-1L) // See also /home/atmel/baudrate.xls

UCSRB = 0<<RXCIE |
		0<<TXCIE |
		0<<UDRIE |
		1<<RXEN |
		1<<TXEN |
		0<<UCSZ2 |
		0<<RXB8 |
		0<<TXB8 ;

#ifdef UCSRC
UCSRC= 1<<URSEL |	// Select UCSRC instead UBRHH
	   1<<USBS |	// 1 = 2 Stop bits
	   3<<UCSZ0 ;	// UCSZ2=0 UCSZ1=1 UCSZ0=1 = 8 bits
#endif

UBRRH = (uint8_t)( (UBRRinitval>>8L) & 0x07f) ;
UBRRL = (uint8_t)UBRRinitval ;

//	; 7	  RXCIE Receive Interrupt enable
//	; 6	  TXCIE Transmit Interrupt enable
//	; 5	  UDRIE Data register empty interrupt enable
//	; 4 * RXEN	Receiver Enable
//	; 3 * TXEN	Transmitter Enable
//	; 2	  CHR9	nine data bits
//	; 1	  RXB8	the controller puts here the received 8th data bit
//	; 0	  TXB8	Transmit data bit 8,
//	ldi r16,0x18
//	out UART_CONTROL,r16	; UCR=0x0a / 0x2A


return ;
}



FUNCT void tsserclose( void )		// AVR
{
UCSRB = 0<<RXCIE |
		0<<TXCIE |
		0<<UDRIE |
		0<<RXEN |
		0<<TXEN |
	//	0<<CHR9 |
		0<<RXB8 |
		0<<TXB8 ;
return ;
}



FUNCT bool tsserisicha( void )		// AVR
{
if ( UCSRA & ( 1<<RXC) )
	{
	return true ;
	}
else
	{
	return false ;
	}
}



FUNCT uint8_t tsserichaw()			// AVR
{

while ( tsserisicha() == false )
	{
	tsnext( "tsserichaw" );
	}
return UDR ;
}



FUNCT void tsserochaw( char c )	   // AVR
{
// Register UCSRA :
// 7
do
	{
	tsnext( "tsserochaw" );
	}
	while ( !(UCSRA & (1<<UDRE)) ) ;
UDR = c ;
return ;
}
#endif
// ---------------------------------------- AVR end




















// ---------------------------------------- MDOS40_COM1 begin

#ifdef MDOS40_COM1
#include <mapidefs.h>
HANDLE fdtsser ;

FUNCT void tsseropen( void )		// MDOS40_COM1
{
SECURITY_ATTRIBUTES seca ;
COMMTIMEOUTS  CommTimeOuts ;

seca.nLength = sizeof(SECURITY_ATTRIBUTES);
seca.lpSecurityDescriptor = 0 ; // use systems default
seca.bInheritHandle = FALSE ;	// do not inherit handle

fdtsser = CreateFile(
	"COM1",			  // #1 LPCTSTR	 lpFileName,
		//										  // pointer to name of the file
	GENERIC_READ | GENERIC_WRITE,	// #2DWORD	dwDesiredAccess,
		//										  // access (read-write) mode
	FILE_SHARE_READ | FILE_SHARE_WRITE ,
	&seca,							// #4 no security attrs
	OPEN_EXISTING,					// #5
		FILE_ATTRIBUTE_NORMAL |
		// FILE_FLAG_OVERLAPPED |		// overlapped I/O
		0 ,								// end of parameter #6
	NULL );							// parameter #7

if ( fdtsser == INVALID_HANDLE_VALUE )
	{
	uprintf( "cannot open %s for RW", "COM1" );
	return	;
	}


SetCommMask( fdtsser, EV_RXCHAR ) ;


// setup device buffers
SetupComm( fdtsser, 4096, 4096 ) ;


// purge any information in the buffer
PurgeComm( fdtsser, PURGE_TXABORT | PURGE_RXABORT |
								  PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

// set up for overlapped I/O

CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
CommTimeOuts.WriteTotalTimeoutMultiplier = 0 ;
CommTimeOuts.WriteTotalTimeoutConstant = 1000 ;
SetCommTimeouts( fdtsser, &CommTimeOuts ) ;

return ;	  // opened succesful
}



FUNCT void tsserclose( void )		// MDOS40_COM1
{
CloseHandle( fdtsser );
return ;
}



FUNCT bool tsserisicha( void )		// MDOS40_COM1
{
bool flag_result ;

COMSTAT	   ComStat ;
DWORD	   dwErrorFlags;

ClearCommError( fdtsser, &dwErrorFlags, &ComStat ) ;
if ( dwErrorFlags )
	{
	if ( dwErrorFlags & CE_BREAK )
		{
		Eprintf(( "CE_BREAK" ));
		}
	if ( dwErrorFlags & CE_DNS)
		{
		Eprintf(( "CE_DNS" ));
		}
	if ( dwErrorFlags & CE_FRAME )
		{
		Eprintf(( "CE_FRAME" ));
		}
	if ( dwErrorFlags & CE_IOE )
		{
		Eprintf(( "CE_IOE" ));
		}
	if ( dwErrorFlags & CE_MODE )
		{
		Eprintf(( "CE_MODE" ));
		}
	if ( dwErrorFlags & CE_OOP )
		{
		Eprintf(( "CE_OOP" ));
		}
	if ( dwErrorFlags & CE_OVERRUN )
		{
		Eprintf(( "CE_OVERRUN" ));
		}
	if ( dwErrorFlags & CE_PTO )
		{
		Eprintf(( "CE_PTO" ));
		}
	if ( dwErrorFlags & CE_RXOVER )
		{
		Eprintf(( "CE_RXOVER" ));
		}
	if ( dwErrorFlags & CE_RXPARITY )
		{
		Eprintf(( "CE_RXPARITY" ));
		}
	if ( dwErrorFlags & CE_TXFULL )
		{
		Eprintf(( "CE_TXFULL" ));
		}
	}
if ( ComStat.cbInQue )
	{		// at least one character for ReadFile available
	flag_result = true ;
	}
else
	{
	flag_result = false ;
	}
return flag_result ;
}



FUNCT uint8_t tsserichaw()			// MDOS40_COM1
{
char c ;
BOOL fReadStat ;
DWORD dwLength ;

while ( tsserisicha() == false )
	{
	tsnext( "tsserichaw" );
	}
// there is a byte available at input - read it
fReadStat = ReadFile(		// returns BOOL
		fdtsser,
		&c,			   // lpBuffer
		(DWORD)1,				// NumberOfBytesToRead
		&dwLength,		// lpNumberOfBytesRead
		NULL );			// The lpOverlapped parameter is not supported
return c ;
}



FUNCT void tsserochaw( char c )	   // MDOS40_COM1
{
unsigned long dwBytesWritten ;
BOOL		fWriteStat ;


fWriteStat = WriteFile( fdtsser, &c, 1,
					   &dwBytesWritten, NULL ) ;
return ;
}
#endif
// ---------------------------------------- MDOS40_COM1 end







// ---------------------------------------- MDOS40_VT begin
#ifdef MDOS40_VT
FUNCT void tsseropen( void )		// MDOS40_VT
{
vtopen();
return ;
}



FUNCT void tsserclose( void )		// MDOS40_VT
{
vtclose();
return ;
}



FUNCT bool tsserisicha( void )		// MDOS40_VT
{
return vtiskey() ;
}



FUNCT uint8_t tsserichaw()			// MDOS40_VT
{
while ( tsserisicha() == false )
	{
	tsnext( "tsserichaw" );
	}
return vtkey();
}



FUNCT void tsserochaw( char c )	   // MDOS40_VT
{
vtocha( c );
return ;
}
#endif
// ---------------------------------------- MDOS40_VT end





#ifdef AVR

// FUNCT void tsserputs_P( const prog_char * p )
// #define PGM_P const prog_char *
// Output / send a string out of program memory


asm(".text");
asm(".global _Z11tsserputs_PPKc");
asm("_Z11tsserputs_PPKc:");
asm("    PUSH    R30              ; Push register on stack\n");
asm("    PUSH    R31              ; Push register on stack\n");
asm("    MOVW    R30,R24          ; Copy register pair (Pointer)\n" );
asm("tsserputs_P_1:\n" );
asm("    LPM     R24,Z+           ; Load a byte from Program Memory\n" );
asm("    TST     R24              ; Check if 0\n" );
asm("    BRNE    tsserputs_P_2    ; else ...\n" );
asm("    POP     R31              ; Then Exit  \n");
asm("    POP     R30                \n");
asm("    RET                        \n");
asm("tsserputs_P_2:\n" );
asm("    CPI     R24,0x0a         ; Check if is Newline character\n" );
asm("    BRNE    tsserputs_P_3    ; else output the character\n" );
asm("    RCALL	 _Z7tssernlv      ; Output CR-LF Combo or whatever\n" );
asm("    RJMP    tsserputs_P_4      \n");
asm("tsserputs_P_3:\n" );
asm("    RCALL	 _Z10tsserochawc  ; Output CR-LF Combo or whatever\n" );
asm("tsserputs_P_4:\n" );
asm("    RJMP    tsserputs_P_1      \n");



#else
// TC, MDOS40, LINUX, ...
FUNCT void tsserputs( const char *p )	// Output / send a string
{
char c ;

while ( c = *p++ )
	{
	if ( c == '\n' )
		{
		tssernl();
		}
	else
		{
		tsserochaw( c );
		}
	}
return ;
}
#endif


FUNCT void tssernl( void )		// Output / send new line
{
tsserochaw( 0x0d );		// CR
tsserochaw( 0x0a );		// LF
return ;
}


