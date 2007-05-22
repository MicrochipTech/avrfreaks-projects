/*________________________________________________________________________________________*\

  File:        cli.h
  Author:      M.J.Bauer 2007

  Header file for CLI module.
\*________________________________________________________________________________________*/

#ifndef  _CLI_H_
#define  _CLI_H_


/****************************************************************************************
*	APPLICATION-SPECIFIC DEFINITIONS REQUIRED BY THE COMMAND-LINE INTERFACE GO HERE...
*	There may be application-specific code in cli.c also.
*/
#define  CLI_MAX_ARGS	     8		  // Maximum # of Command Line arguments (incl. name)
#define  CMND_LINE_MAX_LEN  72		  // Maximum length of command string (chars)
#define  CMD_HIST_BUF_SIZE   8        // Maximum number of commands in recall buffer
#define  MAX_COMMANDS      250		  // Maximum number of CLI commands (arbitrary)
#define  CLI_BAUDRATE      (38400)    // For CLI UART (USART0)
#define  CLI_RXBUFSIZE     (80)       // Size of CLI RX FIFO buffer (bytes)
/*
*	END OF APPLICATION-DEPENDENT DEFINITIONS BLOCK
****************************************************************************************/

enum  eDebugDataType
{
	HEX_BYTE,
	CHAR_BYTE,
	DEC_BYTE,
	HEX_WORD,
	DEC_UWORD,
	DEC_IWORD,
	HEX_LONG,
	DEC_ILONG
};

extern  char  gacCLIprompt[];	// Prompt string (variable)

typedef  void (*CLIfunc)( uint16 argc, char *argv[] );		// pointer to CLI function


// Command table entry looks like this
struct  CmndTableEntry_t
{
	char __farflash  *phzName;	// pointer to command name (string literal in flash)
	bool     yAttribute;		// flag:  1 => end-user (visible); 0 => debug (hidden)
	CLIfunc  Function;			// pointer to CLI function
};


#define  USER_CMD    1
#define  DEBUG_CMD   0
#define  ASCII_DC2   18    /* ASCII 'DC2' = 18 = Ctrl+R */


// Function alias's -- to help improve code portability and/or readability...
#define  CLI_TX_READY        UART0_TX_Ready()
#define  CLI_TX_BYTEOUT(b)   UART0_TX_byteOut(b)
#define  CLI_RX_IRQ(x)       UART0_RX_IRQctrl(x)
#define  kbhit()             CLI_RXdataAvail()
#define  NEW_LINE            putstr_far("\n")


void    doBackgroundTasks( void );   // in main.c

// Prototypes of functions defined in CLI module...
void	ServicePortInput( void );
void	CommandLineInterpreter( void );
void	PrepareForNewCommand( void );
void    EnterCommandInHistory( void );
void    RecallCommand( void );
void    EraseLine( void );

void	CmndHelp( uint16 uwCmndArgCount, char * apcCmndArgVal[] );
void	CmndVersion( uint16 uwCmndArgCount, char * apcCmndArgVal[] );
void    CmndDefault( uint16 uwCmndArgCount, char * apcCmndArgVal[] );
void	CmndWatch( uint16 uwCmndArgCount, char * apcCmndArgVal[] );
void	CmndFlags( uint16 uwCmndArgCount, char * apcCmndArgVal[] );
void    CmndDiag( uint16 uwCmndArgCount, char * apcCmndArgVal[] );
void	CmndReset( uint16 uwCmndArgCount, char * apcCmndArgVal[] );

void    CmndBoot( uint16 uwCmndArgCount, char * apcCmndArgVal[] );
void	CmndPeekMemory( uint16 uwCmndArgCount, char * apcCmndArgVal[] );
void	CmndPokeMemory( uint16 uwCmndArgCount, char * apcCmndArgVal[] );
void	CmndDumpMemory( uint16 uwCmndArgCount, char * apcCmndArgVal[] );

void    putstr_far( const char __farflash * s );
void	putstr( char * );
void	putNewLine( void );
void	putBoolean( bool  );
void	putHexDigit( uint8 );
void	putHexByte( uint8 );
void	putHexWord( uint16 );
void	putHexLong( uint32 );
void	putDecimal( int32 iVal, uint8 bFieldSize );

bool    strmatch( char *pcStr1, const char __farflash *phzStr2 );
uint8   dectobin( char c );
uint16  decatoi( char * pnac, int8 bNdigs );
uint32  long_decatoi( char * pac, int8 bNdigs );
uint8   hexctobin( char c );
uint16  hexatoi( char * s );
uint32  long_hexatoi( char * s );
bool    isHexDigit( char c );

uint8   getch( void );
uint8   putch( uint8 b );
bool    CLI_RXdataAvail( void );
void    CLI_RxBufferReset( void );
void    CLI_RxBufferStore( char c );


#endif	/* _CLI_H_ */
