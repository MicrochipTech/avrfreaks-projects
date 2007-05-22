/*________________________________________________________________________________________*\

  File:        cli.c
  Author:      M.J.Bauer 2007

  This module implements the AVR Command Line user Interface (CLI).
  This version customized for ATmega128.
  MCU on-chip UART0 is dedicated to the "Service/Debug Port" CLI facility.

  NB:  The IAR EWAVR compiler requires the option "--string_literals_in_flash".
  This allows, for example, constructs of the kind:  putstr_far("Hello, world.");

  If compiling this module using IAR EWAVR, ignore the warning:
  [PE1053] "conversion from integer to smaller pointer".

\*________________________________________________________________________________________*/

#include  "system.h"
#include  "periph.h"
#include  "cli.h"


// Declare any application-specific globals referenced by the CLI module here...
extern  uint16  gwDebugFlags;


/*****
*   CLI global variables...
*
*   CLI Prompt string... can change on-the-fly to indicate system status, etc.
*/
char    gacCLIprompt[] = "\r:--> ";

static  char     acCmndLine[CMND_LINE_MAX_LEN+2];  // Command Line buffer
static  char *   pcCmndLinePtr;                    // Pointer into Cmnd Line buffer
static  uint16   uwCmndLineLen;                    // Cmnd line length (char count)
static  uint16   uwCmndArgCount;                   // Number of Cmnd Line args (incl. Cmnd name)
static  char *   apcCmndArgVal[CLI_MAX_ARGS];      // Array of pointers to Command Line args
static  char     szNULLstring[] = { '\0' };        // Empty string

static  char     acCmndHistoryBuffer[CMD_HIST_BUF_SIZE][CMND_LINE_MAX_LEN+2];
static  int16    wCmndHistoryMarker;    // Index to next free place in Command History Buf
static  int16    wCmndRecallMarker;     // Index of previous command to be recalled
static  bool     yHistoryInitialised = FALSE;


/***************************  COMMAND TABLE  ***********************************
*
*   Application-specific command functions go at top of table...
*   (preferably in alphabetical order, or some other systematic arrangement)
*/
const  struct  CmndTableEntry_t  asCommand[] =
{
//   Name       Attribute       Cmd Function

	"help",     USER_CMD,       CmndHelp,
	"ver",      USER_CMD,       CmndVersion,
	"default",  USER_CMD,       CmndDefault,

	"watch",    USER_CMD,       CmndWatch,
	"flags",    USER_CMD,       CmndFlags,
	"reset",    USER_CMD,       CmndReset,
	"boot",     USER_CMD,       CmndBoot,

//--------------  Debug commands ------------------

	"peek",     DEBUG_CMD,      CmndPeekMemory,
	"poke",     DEBUG_CMD,      CmndPokeMemory,
	"dump",     DEBUG_CMD,      CmndDumpMemory,
	"$",        DEBUG_CMD,      CmndReset           // Dummy last entry
} ;


/*
*                         SERVICE PORT SERIAL INPUT HANDLER
*
*   Called frequently from main kernel loop, when the background is otherwise idle.
*
*   If the serial port input (RX) buffer is not empty, the function will...
*     .  Input a char from the serial RX buffer;
*     .  Provide simple line editing (Backspace, Ctrl-X);
*     .  Append the char, if printable, to the command line input buffer;
*     .  If CR received (cmd line terminator), call the CommandLineInterpreter();
*     .  If Ctrl+R received, recall previously entered command from history buffer;
*     .  If TAB received, it is converted to a single space;
*     .  Otherwise, ASCII CTRL chars are ignored.
*
*   Service Port serial I/O must be performed from the CLI via getch() and putch().
*   Other (scheduled) background tasks MUST NOT call getch() or putch().
*/
void  ServicePortInput( void )
{
	char  c;

	if ( CLI_RXdataAvail() )     // char(s) available in serial input buffer
	{
		c = getch();             // Fetch the char... no echo (yet)
		switch ( c )
		{
		case ASCII_CAN:                 // Ctrl+X... cancel line...
			EraseLine();
			PrepareForNewCommand();
			break;

		case ASCII_BS:                  // BACKSPACE...
			if ( uwCmndLineLen > 0 )
			{
				pcCmndLinePtr-- ;           // Remove last-entered char from buffer
				uwCmndLineLen-- ;
				putch( ASCII_BS );          // Backspace the VDU cursor
				putch( SPACE );             // Erase offending char at VDU cursor
				putch( ASCII_BS );          // Re-position the VDU cursor
			}
			break;

		case ASCII_CR:                  // ENTER...
			if ( uwCmndLineLen > 0 )        // Got a command string...
			{
				putch( '\r' );              // Echo NewLine
				putch( '\n' );
				*pcCmndLinePtr = 0;         // Terminate the command string
				EnterCommandInHistory();    // Enter it into the history buffer
				CommandLineInterpreter();   // Interpret and execute the cmnd.
			}
			putch( '\n' );                  // New line
			PrepareForNewCommand();
			break;

		case ASCII_DC2:                 // Ctrl+R... recall command
			EraseLine();                    // Trash the current command line
			PrepareForNewCommand();         // Output the prompt
			RecallCommand();                // Retrieve previous command from history
			break;

		case ASCII_TAB:                 // TAB...  convert to single space
			c = SPACE;
		//  no break... fall thru to default case
		default:
			if ( isprint( c ) && uwCmndLineLen < CMND_LINE_MAX_LEN )
			{
				putch( c );                 // echo char
				*pcCmndLinePtr++ = c;       // Append char to Cmnd Line buffer
				uwCmndLineLen++ ;
			}
			break;
		} // end_switch
	}
}


/*****
*   COMMAND LINE INTERPRETER
*
*   Processes the received command line when entered.
*
*   The CommandLineInterpreter() function is called by ServicePortInput()
*   when a valid command string is entered (CR received). It finds & counts "arguments"
*   in the command line and makes them NUL-terminated strings in-situ.
*   It then searches the Command Table for command name, apcCmndArgVal[0],
*   and if the name is found, executes the respective command function.
*
*   If there is a cmd line argument following the cmd name and it is "-h" (Help option),
*   the argument is converted to "?", which is the (simpler) alternative syntax.
*
*   A command string is comprised of a command name and one or more "arguments" separated
*   by one or more spaces, or TABs. The ordering of arguments is determined by particular
*   command functions, so may or may not be important. The degree of syntax checking is also
*   the responsibility of command functions.
*
*   Command names, hexadecimal arg's, and option switches (e.g. "-a") are not case-sensitive;
*   other arguments may or may not be case-sensitive, as interpreted by particular command
*   functions. (The strmatch() function is not case-sensitive.)
*/
void  CommandLineInterpreter( void )
{
	char    c;
	uint8    bArgIndex, bCmndIndex;
	bool    yCmndNameFound = FALSE;

	pcCmndLinePtr = acCmndLine;             // point to start of Cmnd Line buffer
	uwCmndArgCount = 0;

	// This loop finds and terminates (with a NUL) any user-supplied arguments...
	for ( bArgIndex = 0;  bArgIndex < CLI_MAX_ARGS;  bArgIndex++ )
	{
		if ( !isprint( *pcCmndLinePtr ) )               // stop at end of line
			break;
		while ( *pcCmndLinePtr == SPACE )               // skip leading spaces
			pcCmndLinePtr++ ;
		if ( !isprint( *pcCmndLinePtr ) )               // end of line found
			break;
		apcCmndArgVal[bArgIndex] = pcCmndLinePtr;       // Make ptr to arg
		uwCmndArgCount++ ;

		while ( ( c = *pcCmndLinePtr ) != SPACE )       // find first space after arg
		{
			if ( !isprint( c ) )                        // end of line found
				break;
			pcCmndLinePtr++ ;
		}
		if ( !isprint( *pcCmndLinePtr ) )               // stop at end of line
			break;
		*pcCmndLinePtr++ = 0;                           // NUL-terminate the arg
	}

	// This loop searches the command table for the supplied command name...
	for ( bCmndIndex = 0;  bCmndIndex < MAX_COMMANDS;  bCmndIndex++ )
	{
		if ( *asCommand[bCmndIndex].phzName == '$' )     // reached end of table
			break;
		if ( strmatch( apcCmndArgVal[0], asCommand[bCmndIndex].phzName ) )
		{
			yCmndNameFound = TRUE;
			break;
		}
	}

	if ( uwCmndArgCount > 1 )       // If there is one or more user-supplied arg(s)...
	{
		if ( strmatch( apcCmndArgVal[1], "-h" ) )       // convert "-h" to '?' ...
			*apcCmndArgVal[1] = '?';                    // ... to simplify cmd fn
	}

	if ( yCmndNameFound )
	{
		(*asCommand[bCmndIndex].Function)( uwCmndArgCount, apcCmndArgVal );     // Do it
	}
	else  putstr_far( "? Command error\n" );
}


/*****
*   Flush Command Line buffer, clear CLI arg's and output CLI prompt.
*/
void  PrepareForNewCommand( void )
{
	uint8  bArgIndex;

	acCmndLine[0] = 0;
	pcCmndLinePtr = acCmndLine;             // point to start of Cmnd Line buffer
	uwCmndLineLen = 0;
	for ( bArgIndex = 0;  bArgIndex < CLI_MAX_ARGS;  bArgIndex++ )   // Clear CLI args
		apcCmndArgVal[bArgIndex] = szNULLstring;
	putstr( gacCLIprompt );
}


/*****
*   Copy the newly entered command into the history buffer for later recall...
*   if it's length is non-zero.
*/
void  EnterCommandInHistory( void )
{
	short  line;

	if ( !yHistoryInitialised )
	{
		for ( line = 0 ; line < CMD_HIST_BUF_SIZE ; line++ )
		{
			acCmndHistoryBuffer[line][0] = 0;    // make empty cmnd string
		}
		wCmndHistoryMarker = 0;
		wCmndRecallMarker = 0;
		yHistoryInitialised = TRUE;
	}

	if ( strlen( acCmndLine ) != 0 )   // Not an empty cmnd string
	{
		strncpy( acCmndHistoryBuffer[wCmndHistoryMarker], acCmndLine, CMND_LINE_MAX_LEN );
		wCmndRecallMarker = wCmndHistoryMarker;
		wCmndHistoryMarker++ ;
		if ( wCmndHistoryMarker >= CMD_HIST_BUF_SIZE ) wCmndHistoryMarker = 0;
	}
}


/*
*   Recall a previously entered command from the history buffer...
*   The function selects the next previous command from the buffer (if any)
*   as indicated by wCmndRecallMarker, and outputs the command string to the user's
*   terminal for editing. At the same time, the selected command is copied to the
*   current command line buffer.
*   The selected command is not executed until the user hits ENTER (CR).
*/
void  RecallCommand( void )
{
	strncpy( acCmndLine, acCmndHistoryBuffer[wCmndRecallMarker], CMND_LINE_MAX_LEN );
	if ( wCmndRecallMarker == 0 ) wCmndRecallMarker = CMD_HIST_BUF_SIZE;
	--wCmndRecallMarker;
	uwCmndLineLen = strlen( acCmndLine );
	pcCmndLinePtr = acCmndLine + uwCmndLineLen;
	*pcCmndLinePtr = 0;
	putstr( acCmndLine );
}


/*
*   Erase the command line on user terminal; cursor remains on same line at col 1.
*/
void  EraseLine( void )
{
	short  col;

	putch('\r');
	for ( col=0 ; col < (CMND_LINE_MAX_LEN + 5) ; col++ )
	{
		putch(SPACE);
	}
	putch('\r');
}


/******************************************************************************************
*                                CLI COMMAND FUNCTIONS
*
*   All CLI command functions have access to user-supplied command line arguments.
*   These are passed to the function via two arguments (parameters)...
*
*     (uint16) uwCmndArgCount  : number of user-supplied command line arguments,
*                                including the command name, apcCmndArgVal[0].
*
*     (char *) apcCmndArgVal[] : array of pointers to user-supplied argument strings
*
*   All CLI commands, when entered with the arg "-h" (or '?') following the command name,
*   will show command usage details (and do nothing else).
*   Otherwise, a CLI command which expects at least one user-supplied arg, when entered
*   without any, will simply show command usage details (and do nothing else).
*/

/*****
*   CLI command function:  CmndHelp
*
*   The "help" command lists CLI command names, formatted into 6 columns.
*   By default, only "end-user" commands are listed; "debug" commands are hidden.
*   The command line can have one user-supplied argument, which selects an option...
*
*   -d : list debug commands only
*   -a : list all commands, first the "user" set, then the "debug" set.
*   -h : show command usage (applies to any CLI command)
*    ? : alternative to "-h"
*
*   Commands are listed in the order they occur in the Command Table (see structure above).
*/
void  CmndHelp( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	bool    yOptionDebug = FALSE;
	bool    yOptionAll = FALSE;
	char  __farflash  *phzCommandName;
	uint8   bCmndIndex;
	uint8   bSpacesToPadColumn;
	uint8   bColumn = 0;

	if ( uwCmndArgCount > 1 )       // There should be only 1 or 2 arg's (incl. cmnd name)
	{
		if ( strmatch( apcCmndArgVal[1], "-a" ) )  yOptionAll = TRUE;
		else if ( strmatch( apcCmndArgVal[1], "-d" ) )  yOptionDebug = TRUE;
		else
		{
			putstr_far( "Usage:  help { ? | -h | -d | -a } \n" );
			putstr_far( "Arg's:   ? : show command usage \n" );
			putstr_far( "        -h : same as '?' \n" );
//			putstr_far( "        -d : list debug commands only \n" );
//			putstr_far( "        -a : list all commands \n" );
			putstr_far( "\n" );
			putstr_far( "Hit Ctrl+R to recall previous commands.\n" );
			return;
		}

	}

	// To allow discrimination between "user" and "hidden" (debug) commands...
	if ( uwCmndArgCount == 1 ) yOptionAll = TRUE;       // ... remove this line

	for ( bCmndIndex = 0;  bCmndIndex < MAX_COMMANDS;  bCmndIndex++ )   // List commands
	{
		phzCommandName = asCommand[bCmndIndex].phzName;
		if ( *phzCommandName == '$' )  break;            // Reached end of Cmnd Table

		if ( yOptionAll
		|| yOptionDebug && asCommand[bCmndIndex].yAttribute == DEBUG_CMD
		|| uwCmndArgCount == 1 && asCommand[bCmndIndex].yAttribute == USER_CMD )
		{
			putstr_far( phzCommandName );
			bSpacesToPadColumn = 12 - strlen_P( phzCommandName );
			while ( bSpacesToPadColumn-- != 0 )
				putch( SPACE );
			if ( ++bColumn >= 6 )  { bColumn = 0;  NEW_LINE; }
		}
	}

}


/*****
*   CLI command function:  ver
*
*   The "ver" command displays firmware version number and other build information.
*
*   User-supplied arguments:  none (except '?' or "-h" for help)
*/
void  CmndVersion( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	putstr_far( "AVR Debug Monitor\n" );
	putstr_far( "Version: " );
	putDecimal( BUILD_VER_MAJOR, 1 );
	putch( '.' );
	putDecimal( BUILD_VER_MINOR, 1 );
	putch( '.' );
	putDecimal( BUILD_VER_DEBUG, 1 );
	putstr_far( "  " );
	putstr_far( __TIME__ );
	putstr_far( ", " );
	putstr_far( __DATE__ );
	NEW_LINE;
}


/*****
*   CLI command function:  CmndDefault
*
*   The "default" command sets various configuration parameters to default values,
*   the same as when the memory backup battery fails or a firmware update is installed.
*   Command probably should be followed by hardware reset.
*/
void  CmndDefault( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	char  key;

	putstr_far( "Set all configuration parameters to default values...\n" );
	putstr_far( "Are you sure? " );
	key = getch();  putch( key );

	if ( toupper(key) == 'Y' )
	{
		// TODO:
		// call param default function
		// putstr_far(" ... done!");
	}
}


/*****
*   CLI command function:  CmndWatch
*
*   The "watch" command activates a real-time display which shows the value(s)
*   of one or more global variables.
*   The function exits when it receives ASCII_ESC from the serial input stream,
*   ie. when user hits the [Esc] key on their terminal.
*/
void  CmndWatch( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	static  uint32  captureTime;
	bool    yQuit = FALSE;
	uint32  lWatchTime;
	uint32  lWatchStartTime;

	putstr_far( "Hit [ESC] to quit. \n\n" );
	putstr_far( "Time  ......... other variable names ................  \n");
	lWatchTime = 0;
	lWatchStartTime = getTickCount();

	while ( ! yQuit )
	{
		lWatchTime = getTickCount() - lWatchStartTime;     // ticks elapsed
		lWatchTime = (lWatchTime * MSEC_PER_TICK) / 1000;  // seconds elapsed

		// TODO: Output variables to be watched here... all on a single line...
		// (typically beginning with the elapsed time, lWatchTime)
		putDecimal( lWatchTime, 5);



		// Delay 200mS (for refresh rate of approx. 5 updates/sec)
		// While waiting, any pending background tasks are executed.
		captureTime = getTickCount();
		while ( getTickCount() - captureTime < TICKS_PER_200MSEC )
		{
			// So that other tasks are not held up by the "watch" routine...
			doBackgroundTasks();
		}
		putch ( '\r' );       // Return VDU cursor to start of output line
		if ( kbhit() )        // Check for key hit
		{
			if ( getch() == ASCII_ESC )  yQuit = TRUE;
		}
	}
	NEW_LINE;
}


/*****
*   CLI command function:  CmndFlags
*
*   The "flags" command outputs various status flags, error flags, etc.
*   Exits with transient Error and Debug flags cleared after output, optionally.
*/
void  CmndFlags( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	putstr_far("Debug flags : 0x");
	putHexWord( gwDebugFlags );
	NEW_LINE;
	gwDebugFlags = 0;
}


/*****
*   CLI command function:  CmndDiag
*
*   The "diag" command outputs various application-specific diagnostic info.
*/
void  CmndDiag( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	// TODO:  Output interesting stuff!
}


/*****
*   CLI command function:  CmndReset
*
*   The "reset" command resets various device functions, depending on the option specified.
*/
void  CmndReset( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	pfnvoid  pfnExecAddr = (pfnvoid) PROGRAM_ENTRY_POINT;

	if ( uwCmndArgCount == 1 || *apcCmndArgVal[1] == '?' )   // help wanted
	{
		putstr_far( "Usage:  reset {-p|-d|-u}\n" );
		putstr_far( "Opt's:  -p   : reset processor \n" );
		putstr_far( "        -d   : reset device operation \n" );
		putstr_far( "        -o   : reset other \n" );
	}
	else if ( strmatch( apcCmndArgVal[1], "-p" ) )
	{
		DISABLE_GLOBAL_IRQ;
		(*pfnExecAddr)();             // Jump to PROGRAM_ENTRY_POINT
	}
	else if ( strmatch( apcCmndArgVal[1], "-d" ) )
	{
		// TODO: whatever is required
	}
	else if ( strmatch( apcCmndArgVal[1], "-o" ) )
	{
		// TODO: whatever is required
	}
}


/*****
*   CLI command function:  CmndBoot
*
*   The "boot" command starts the flash bootloader, if installed.
*/
void  CmndBoot( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	pfnvoid  boot_entry = (pfnvoid) BOOTLDR_ENTRY_ADDRESS;

	if ( PEEK_CODE_WORD( BOOTLDR_ENTRY_ADDRESS ) != 0xFFFF )  // bootloader installed
	{
		DISABLE_GLOBAL_IRQ;
		CLEAR_RESET_FLAGS;
		(*boot_entry)();
	}
	else  putstr_far("Bootloader not installed.");
}


/*____________________________  "DEBUG" COMMAND FUNCTIONS  _______________________________*/

/*
*   CLI command function:  CmndPeekMemory
*
*   The "peek" command reads a byte (default), word or longword at the data memory
*   (RAM or external I/O register) address specified in a command line argument.
*   Byte data is displayed in hex ($xx), unsigned decimal and signed decimal.
*   Word data is displayed in hex ($xxxx), unsigned decimal and signed decimal.
*   Long data is displayed in hex ($xxxxxxxx) and signed decimal only.
*   The data size and address are saved for next time the command is given.
*
*   User-supplied arguments:
*   aaaa : address (hex)
*   -w   : data size is word (16 bits)
*/
void  CmndPeekMemory( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	static  uint8  DataSize = 1;      // Data size (bytes) : default = 1 (byte)
	static  uint8  *pAddr;
	uint8   n, bData;
	int16   wData;
	int32   lData;

	if ( uwCmndArgCount > 1 && *apcCmndArgVal[1] == '?' )   // help wanted
	{
		putstr_far( "Usage:  peek [addr] {-b|-w|-l}\n" );
		putstr_far( "Arg's:  addr : data memory address (hex) \n" );
		putstr_far( "        -b   : size is Byte ( 8b)\n" );
		putstr_far( "        -w   : size is Word (16b)\n" );
		putstr_far( "        -l   : size is Long (32b)\n" );
		putstr_far( "Display data memory (RAM) byte, word or longword.\n" );
		return;
	}

	for ( n = 1;  n < uwCmndArgCount;  n++ )    // process arg's
	{
		if ( strmatch( apcCmndArgVal[n], "-l" ) )  DataSize = 4;
		else if ( strmatch( apcCmndArgVal[n], "-w" ) )  DataSize = 2;
		else if ( strmatch( apcCmndArgVal[n], "-b" ) )  DataSize = 1;
		else if ( isHexDigit( *apcCmndArgVal[n] ) )
		{
			pAddr = (uint8 *) hexatoi( apcCmndArgVal[n] );
		}
	}

//	if ( DataSize > 1 ) pAddr = (uint8 *) ((uint32) pAddr & 0xFFFFFFFE);  // Word align?

	putHexWord( (uint16) pAddr );        // show address
	putstr_far(":  0x");
	if ( DataSize == 4 )  // long
	{
		lData = *(int32 *)pAddr;
		putHexLong( lData );     putstr_far("  (");          // show as 8 digit hex
		putDecimal( lData, 12 ); putstr_far(")  ");          // show as signed decimal
	}
	else if ( DataSize == 2 )  // word
	{
		wData = *(int16 *)pAddr;
		putHexWord( wData );     putstr_far("  ");           // show as 4 digit hex
		putDecimal( (uint32)wData, 5 );  putstr_far("  (");  // show as unsigned decimal
		putDecimal( (int32) wData, 6 );  putstr_far(")  ");  // show as signed decimal
	}
	else   // byte
	{
		bData = *pAddr;
		putHexByte( bData );     putstr_far("  ");           // show as 2 digit hex
		putDecimal( (uint32)bData, 3 );  putstr_far("  (");  // show as unsigned decimal
		putDecimal( (int32) bData, 4 );  putstr_far(")  ");  // show as signed decimal
	}
}


/*****
*   CLI command function:  CmndPokeMemory
*
*   The "poke" command writes a byte (default), word or longword at the data memory
*   (RAM or external I/O register) address specified in a command line argument.
*   Data size is byte (8b) by default.
*
*   User-supplied arguments:
*   <addr> : address (hex)
*   <data> : data o write
*   -w     : data size is word (16 bits)
*/
void  CmndPokeMemory( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	bool     yOptionWord = FALSE;
	uint8    n;
	uint8   *pByteAddr = 0;
	uint16  *pWordAddr = 0;
	uint16   wData;

	if ( uwCmndArgCount < 3 )           // help needed...
	{
		putstr_far( "Usage:  poke <addr> <data> [-w] \n" );
		putstr_far( "Arg's:  <addr> : data memory address (hex)\n" );
		putstr_far( "        <data> : value to write (hex)\n" );
		putstr_far( "        -w     : size is Word (16b), default is Byte\n" );
		putstr_far( "Write data at specified address.\n" );
		return;
	}

	for ( n = 1;  n < uwCmndArgCount;  n++ )        // look for option "-w"
	{
		if ( strmatch( apcCmndArgVal[n], "-w" ) )  yOptionWord = TRUE;
	}

	for ( n = 1;  n < uwCmndArgCount-1;  n++ )      // look for <addr>
	{
		if ( isHexDigit( *apcCmndArgVal[n] ) )      // assume arg[n] is <addr>
		{
			pByteAddr = (uint8 *) hexatoi( apcCmndArgVal[n] );
			pWordAddr = (uint16 *) pByteAddr;
			break;
		}
	}
	if ( isHexDigit( *apcCmndArgVal[n+1] ) )        // assume arg[n+1] is <data>
	{
		wData = hexatoi( apcCmndArgVal[n+1] );
		if ( yOptionWord )  *pWordAddr = wData;     // write word
		else  *pByteAddr = (uint8) wData;           // write byte
	}
	else  putstr_far( "Command syntax error." );
}


/*****
*   CLI command function:  CmndDumpMemory
*
*   The "dump" command outputs a block of memory, formatted 16 rows x 16 columns,
*   as hex bytes and (printable) ASCII chars, beginnng at the specified address.
*
*   User-supplied arguments:
*   addr : begin address (hex), masked to 16-byte boundary (addr & 0xFFFFF0)
*     -p : dump program memory
*     -d : dump data (RAM) memory
*/
void  CmndDumpMemory( uint16 uwCmndArgCount, char * apcCmndArgVal[] )
{
	static  bool  yOptionPgrm = 0;
	static  long  lNextAddr = 0;
	char    c;
	uint8   n, ubRow, ubCol, bData;

	if ( uwCmndArgCount != 1 && *apcCmndArgVal[1] == '?' )       // help wanted...
	{
		putstr_far( "Usage:  dump [addr][-p] \n" );
		putstr_far( "Arg's:  <addr> : start address (hex) \n" );
		putstr_far( "          -p   : access Program space (Flash) \n" );
		putstr_far( "          -d   : access Data space (RAM) \n" );
		putstr_far( "Dumps a 256-byte page of memory at <addr>, 16 byte aligned.\n" );
		putstr_far( "Memory space option (Pgrm or Data) is remembered.\n" );
		putstr_far( "If <addr> is omitted, the next page is output.\n" );
		return;
	}

	for ( n = 1;  n < uwCmndArgCount;  n++ )        // look for option switch
	{
		if ( strmatch( apcCmndArgVal[n], "-p" ) )  yOptionPgrm = TRUE;
		if ( strmatch( apcCmndArgVal[n], "-d" ) )  yOptionPgrm = FALSE;
	}

	for ( n = 1;  n < uwCmndArgCount;  n++ )      // look for <addr> (hex)
	{
		if ( isHexDigit( *apcCmndArgVal[n] ) )      // assume arg[n] is <addr>
		{
			lNextAddr = long_hexatoi( apcCmndArgVal[n] ) & 0xFFFFF0;
			if ( lNextAddr > 0x00FFFF ) yOptionPgrm = TRUE;
			break;
		}
	}

	for ( ubRow = 0;  ubRow < 16;  ubRow++ )
	{
		if ( yOptionPgrm ) putHexByte( (uint8)(lNextAddr >> 16) );   // MSB of 24 bit addr
		putHexWord( (uint16)lNextAddr );    // Low-order 16 bits of addr
		putch( SPACE );
		for ( ubCol = 0;  ubCol < 16;  ubCol++ )        // Hex dump
		{
			if ( (ubCol % 8) == 0 ) putch( SPACE );
			if ( yOptionPgrm ) bData = *(uint8 __farflash *)lNextAddr;
			else  bData = *(uint8 *)lNextAddr;
			putHexByte( bData );
			lNextAddr++;
		}
		lNextAddr -= 16;
		putch( SPACE );
		putch( SPACE );
		for ( ubCol = 0;  ubCol < 16;  ubCol++ )       // ASCII dump
		{
			if ( yOptionPgrm ) c = *(char __farflash *)lNextAddr;
			else  c = *(char *)lNextAddr;
			if ( isprint(c) )  putch( c );
			else  putch( SPACE );
			lNextAddr++;
		}
		NEW_LINE;
	}
}


/******************************************************************************************
*
*                         SERVICE PORT CHARACTER OUTPUT FUNCTIONS
*
*
*   Function:   putstr_far
*
*   Purpose:    Output string literal to Service Port.
*               The string must be stored in __farflash program memory.
*               A Newline is NOT automatically output following the string.
*               A Newline char (\n) within the argument string is expanded to CR+LF.
*
*   Arg(s):     (const char __farflash*) s  :  pointer to string literal in flash
*/
void  putstr_far( const char __farflash * s )
{
	char   ch;

	while ( (ch = *s++) != 0 )
	{
		if ( ch == 0x0A )
		{
			putch( '\r' );
			putch( '\n' );
		}
		else   putch( ch );
	}
}


/*
*   Function:   putstr
*
*   Purpose:    Output string variable (char array) to Service Port.
*               The string must be stored in data memory (RAM).
*               A Newline is NOT automatically output following the string.
*               A Newline char (\n) within the argument string is expanded to CR+LF.
*
*   Arg(s):     (char *) s  :  pointer to string variable
*/
void  putstr( char * s )
{
	char   ch;

	while ( (ch = *s++) != 0 )
	{
		if ( ch == 0x0A )
		{
			putch( '\r' );
			putch( '\n' );
		}
		else   putch( ch );
	}
}


/*****
*  Output Boolean value as ASCII '0' or '1'.
*
*  Called by:  CLI command functions (only)
*  Entry args: b = Boolean variable (zero or non-zero)
*  Returns:    void
*  Affects:
*/

void  putBoolean( bool  b )
{
	if ( b )  putch( '1');
	else  putch ( '0' );
}


/*****
*  Output 4 LS bits of a byte as Hex (or BCD) ASCII char.
*
*  Called by:  CLI command functions (only)
*  Entry args: d = value of Hex digit (0 to 0xf)
*  Returns:    void
*  Affects:    --
*/

void  putHexDigit( uint8 d )
{
	d &= 0x0F;
	if ( d < 10 )  putch ( '0' + d );
	else  putch ( 'A' + d - 10 );
}


/*****
*  Output byte as 2 Hex ASCII chars, MSD first.
*
*  Called by:  CLI command functions (only)
*  Entry args: b = byte to output
*  Returns:    void
*  Affects:    --
*/

void  putHexByte( uint8 b )
{
	putHexDigit( b >> 4 );
	putHexDigit( b );
}


/*****
*  Output 16-bit word as 4 Hex ASCII chars, MSD first.
*
*  Called by:  CLI command functions (only)
*  Entry args: uW = word to output
*  Returns:    void
*  Affects:    --
*/

void  putHexWord( uint16 uW )
{
	putHexDigit( (uint8) (uW >> 12) );
	putHexDigit( (uint8) (uW >> 8) );
	putHexDigit( (uint8) (uW >> 4) );
	putHexDigit( (uint8) (uW & 0xF) );
}


/*****
*  Output 32-bit longword as 8 Hex ASCII chars, MSD first.
*
*  Called by:  CLI command functions (only)
*  Entry args: uL = longword to output
*  Returns:    void
*  Affects:    --
*/

void  putHexLong( uint32 uL )
{
	uint8  count, digit;

	for ( count=0 ; count < 8 ; count++ )
	{
		digit = (uint8) (uL >> 28);
		putHexDigit( digit );
		uL = uL << 4;
	}
}


/*****
*   Function:   putDecimal
*   Called by:  CLI command functions (only)
*
*   Purpose:    Outputs a 32-bit word as a signed decimal integer, up to 10 digits & sign,
*               right justified in the specified field, with leading zeros suppressed.
*               If the value is too big to fit into the specified minimum field size,
*               the field will be expanded to accommodate the number of digits (& sign).
*               Negative numbers are output with a 'minus' sign to the left of the first
*               non-zero digit. The field size includes the minus sign. Positive numbers
*               are output with space(s) to the left of the first digit, if the space(s)
*               can fit into the specified minimum field width.
*
*   Args:       (int32) lVal = signed longword to be converted and output
*               (uint8) bFieldSize = minimum number of character places to output (1..12)
*/
void  putDecimal( int32 lVal, uint8 bFieldSize )
{
	uint8    acDigit[12];     /* ASCII result string, acDigit[0] is LSD */
	uint8    bPlace;
	uint8    c;
	uint8    bSignPlace = 0;
	bool     yNegative = FALSE;
	bool     yLeadingZero = TRUE;

	if ( bFieldSize > 12 )  bFieldSize = 12;
	if ( bFieldSize < 1 )  bFieldSize = 1;
	if ( lVal < 0 )  { yNegative = TRUE;  lVal = 0 - lVal; }   /* make value absolute */

	for ( bPlace = 0;  bPlace < 12;  bPlace++ )      /* begin conversion with LSD */
	{
		acDigit[bPlace] = '0' + (lVal % 10);
		lVal = lVal / 10;
	}

	for ( bPlace = 11;  bPlace < 12;  bPlace-- )    /* begin processing with MSD */
	{
		c = acDigit[bPlace];
		if ( bPlace != 0 && c == '0' && yLeadingZero )    /* leave digit 0 (LSD) alone */
			acDigit[bPlace] = SPACE;

		if ( bPlace == 0 || c != '0' )              /* found 1st significant digit (MSD) */
		{
			yLeadingZero = FALSE;
			if ( bSignPlace == 0 ) bSignPlace = bPlace + 1;
		}
	}
	if ( yNegative ) acDigit[bSignPlace] = '-';     /* else there will be a SPACE already */

	for ( bPlace = 11;  bPlace < 12;  bPlace-- )    /* begin output with MSD (or SP, or sign) */
	{
		c = acDigit[bPlace];
		if ( bPlace < bFieldSize || c != SPACE ) putch( c );
	}
}


/*****
*   Function:   strmatch
*
*   Purpose:    Compares two NUL-terminated strings, each up to 255 chars in length.
*               Returns TRUE if the strings have the same contents, up to the terminator
*               of either string, except that the comparison is not case-sensitive.
*
*   Args:       (char *) pcStr1                  :  pointer to string variable
*               (const char __farflash *) phStr2 :  pointer to string literal (flash PROM)
*
*   Returns:    (bool) TRUE if string #1 matches string #2 (case-insensitive), else FALSE.
*/
bool  strmatch( char *pcStr1, const char __farflash *phStr2 )
{
	char    c1, c2;
	uint8   b = 255;
	bool    yResult = TRUE;

	while ( b-- != 0 )
	{
		c1 = tolower( *pcStr1++ );
		c2 = tolower( *phStr2++ );
		if ( c1 != c2 )  yResult = FALSE;
		if ( c1 == 0 || c2 == 0 )  break;
	}
	return yResult;
}


/*****************************************************************************************
*                           CHARACTER CONVERSION FUNCTIONS
*
*  Convert decimal ASCII char to 4-bit BCD value (returned as unsigned byte).
*
*  Entry args: c = decimal digit ASCII encoded
*  Returns:    0xFF if arg is not a decimal digit, else unsigned byte (0..9)
*  Affects:    --
*/
uint8  dectobin( char c )
{
	if ( c >= '0'  &&  c <= '9')
		return ( c - '0' );
	else
		return 0xFF ;
}


/*****
*  Convert decimal ASCII string, up to 5 digits, to 16-bit unsigned word.
*  There may be leading zeros, but there cannot be any leading white space.
*  Conversion is terminated when a non-decimal char is found, or when the
*  specified number of characters has been processed.
*
*  Entry args: (char *) pac = pointer to first char of decimal ASCII string
*              (int8)  bNdigs = number of characters to process (max. 5)
*  Returns:    Unsigned 16bit word ( 0 to 0xffff ).
*              If the target string (1st char) is non-numeric, returns 0.
*/
uint16  decatoi( char * pnac, int8 bNdigs )
{
	uint8   ubDigit, ubCount;
	uint16  uwResult = 0;

	for ( ubCount = 0;  ubCount < bNdigs;  ubCount++ )
	{
		if ( (ubDigit = dectobin( *pnac++ )) == 0xFF )
			break;
		uwResult = 10 * uwResult + ubDigit;
	}
	return  uwResult;
}


/*****
*  Convert decimal ASCII string, up to 12 digits, to 32-bit unsigned long word.
*  There may be leading zeros, but there cannot be any leading white space.
*  Conversion is terminated when a non-decimal char is found, or when the
*  specified number of characters has been processed.
*
*  Entry args: (char *) pac = pointer to first char of decimal ASCII string
*              ubNdigs = number of characters to process (max. 5)
*  Returns:    Unsigned long (32 bit) word ( 0 to 0xffffffff ).
*              If the target string (1st char) is non-numeric, returns 0.
*/
uint32  long_decatoi( char * pac, int8 bNdigs )
{
	uint8   ubDigit, ubCount;
	uint32  ulResult = 0L;

	for ( ubCount = 0;  ubCount < bNdigs;  ubCount++ )
	{
		if ( (ubDigit = dectobin( *pac++ )) == 0xFF )
			break;
		ulResult = 10L * ulResult + (long)ubDigit;
	}
	return  ulResult;
}


/*****
*  Convert Hexadecimal ASCII char (arg) to 4-bit value (returned as unsigned byte).
*
*  Called by:  various, background only
*  Entry args: c = Hex ASCII character
*  Returns:    0xFF if arg is not hex, else digit value as unsigned byte ( 0..0x0F )
*/

uint8  hexctobin( char c )
{
	if ( c >= '0'  &&  c <= '9')
		return ( c - '0' );
	else if ( c >= 'A'  &&  c <= 'F' )
		return ( c - 'A' + 10 );
	else if ( c >= 'a'  &&  c <= 'f' )
		return ( c - 'a' + 10 );
	else
		return 0xFF ;
}


/*****
*  Convert Hexadecimal ASCII string, up to 4 digits, to 16-bit unsigned word.
*  The string must be stored in the data RAM space.
*  There cannot be any leading white space.
*  Conversion is terminated when a non-Hex char is found.
*
*  Entry args: s = pointer to first char of hex string.
*  Returns:    Unsigned 16bit word ( 0 to 0xffff ).
*              If the target string (1st char) is non-Hex, returns 0.
*/

uint16  hexatoi( char * s )
{
	uint8   ubDigit, ubCount;
	uint16  uwResult = 0;

	for ( ubCount = 0;  ubCount < 4;  ubCount++ )
	{
		if ( (ubDigit = hexctobin( *s++ )) == 0xFF )
			break;
		uwResult = 16 * uwResult + ubDigit;
	}
	return  uwResult;
}


/*****
*  Convert Hexadecimal ASCII string, up to 8 digits, to 32-bit unsigned long word.
*  There cannot be any leading white space.
*  Conversion is terminated when a non-Hex char is found.
*
*  Entry args: s = pointer to first char of hex string.
*  Returns:    Unsigned 32 bit long word ( 0 to 0xffffffff ).
*              If the target string (1st char) is non-Hex, returns 0.
*/

uint32  long_hexatoi( char * s )
{
	uint8   ubDigit, ubCount;
	uint32  ulResult = (uint32) 0;

	for ( ubCount = 0;  ubCount < 8;  ubCount++ )
	{
		if ( (ubDigit = hexctobin( *s++ )) == 0xFF )
			break;
		ulResult = 16 * ulResult + ubDigit;
	}
	return  ulResult;
}


bool  isHexDigit( char c )
{
	if ( hexctobin( c ) == 0xFF ) return FALSE;
	else  return TRUE;
}


/*________________________________________________________________________________________*\
*
*   DEBUG PORT / CLI SERIAL COMMS SUPPORT FUNCTIONS
\*________________________________________________________________________________________*/

static  uint8   acRx0buffer[CLI_RXBUFSIZE];   /* CLI serial input RX buffer */
static  uint8  *pcRx0Head;                    /* Pointer to next available unread char */
static  uint8  *pcRx0Tail;                    /* Pointer to next free place for writing */
static  uint8   bRx0Count;                    /* Number of unread chars in RX buffer */


/*****
*   getch() - Fetches next unread char from the CLI RX FIFO buffer.
*
*   The function does not wait for data available in the RX FIFO buffer;
*   the caller should first check using CLI_RXdataAvail().
*   If there is no data available, the function returns NUL (0).
*   The input char is not echoed back to the serial output stream.
*
*   Returns:    Byte from CLI RX buffer (or 0, if buffer is empty).
*/
uint8  getch( void )
{
	char  b = 0;

	if ( bRx0Count )
	{
		CLI_RX_IRQ( DISABLE );
		b = *pcRx0Head++;                /* Fetch char from buffer */
		--bRx0Count;
		if ( pcRx0Head >= (acRx0buffer + CLI_RXBUFSIZE) )
			pcRx0Head = acRx0buffer;      /* Wrap pointer */
		CLI_RX_IRQ( ENABLE );
	}

	return  b;
}


/*****
*   putch(c) - Output single char to CLI serial port.
*
*   Waits for UART transmitter ready, then writes the char to the TX data register.
*   While waiting, any pending background tasks are executed.
*
*   Entry args: (uint8) b = TX byte
*   Returns:    (uint8) b = TX byte
*/
uint8  putch( uint8 b )
{
	while ( !CLI_TX_READY )    /* Wait while UART transmitter is busy */
	{
		doBackgroundTasks();
	}
	CLI_TX_BYTEOUT( b );
	return  b;
}


/*****
*   Test if any unread data is available in the CLI RX FIFO buffer.
*   Returns TRUE if char available, FALSE if the RX buffer is empty.
*/
bool  CLI_RXdataAvail( void )
{
	if ( bRx0Count ) return TRUE;
	else  return FALSE;
}


/*****
*   Initialise serial port serial input (RX) RAM data buffer...
*   Reset RX buffer pointers, clear RX byte count.
*   Must be called before SCI RX interrupt (or global IRQ) is enabled!!!
*/
void  CLI_RxBufferReset( void )
{
	CLI_RX_IRQ( DISABLE );
	pcRx0Tail = acRx0buffer;
	pcRx0Head = acRx0buffer;
	bRx0Count = 0;
	CLI_RX_IRQ( ENABLE );
}


/*****
*   Store received char in serial input (RX) data buffer.
*   If the buffer is full, the received char will be lost.
*   This function is called by the UART RX interrupt service routine,
*   so the UART IRQ will be automatically disabled while the function executes.
*
*   Entry args:     (char) c = character to place into RX buffer
*   Returns:        --
*   Affects:        acRx0buffer[]
*/
void  CLI_RxBufferStore( char c )
{
	if ( bRx0Count < CLI_RXBUFSIZE )     // No buffer overflow
	{
		*pcRx0Tail++ = c;                // store char from UART RX data register
		if ( pcRx0Tail >= (acRx0buffer + CLI_RXBUFSIZE) )
			pcRx0Tail = acRx0buffer;
		bRx0Count++;
	}
//	else  SET_BIT( wSystemErrors, RX_BUFFER_OVFL );
}
