
#include <heport.h>				// special type names, uint16_t, int16_t ...
#include <helq.h>				// uprintf
#include <hevt.h>				// vtputs
#include <hedeb.h>
#include <heserial.h>			// prototypes for all functions defined here


// Look for arguments case 'i': case 'b': case 'd': case 'h': case 's': case 'p':
// common to all compilers / libraries / systems
int serargs( int argc, const char **argv)
{
int iarg ;
const char *pch ;
char zch ;
uint16_t uwX ;

int if_no_local = 0 ;		// serial interface number
int if_index = -1 ;

for ( iarg = 1 ; iarg < argc ; ++iarg )
	{
	pch = argv[iarg] ;
	if ( *pch == '-' )			// this s a switch at all
		{
		++pch ;
		switch (*pch)		// check for options
			{
		case 'i' :			// -i=0, -i=1, -i=2, -i=3 interface number
			++pch ;
			if ( *pch == '=' || *pch == ':' ) ++pch ;
			if ( asctouw( &pch, &uwX ) == 0 )
				{
				if_no_local = uwX;
				if ( if_no_local < 0 || if_no_local >= sermaxnumber )
					{
					uprintf( "bad interface number -i=%d", if_no_local );
					DBret("serargs");
					return 1 ;
					}
#if defined( MDOS40 ) || defined( MSVC40 ) || defined( __TURBOC__ ) || defined( DOSGNU ) || defined( LINUX )
				if ( *pch == '=' || *pch == ':' )
					{	// We have an argument as -i=1:COM7
					++pch ;
					asctostr( &pch, sername[if_no_local],MAX_INTERFACENO );

					}
#endif
					++if_index ;
				if ( if_index >= MAX_INTERFACENO )
					{
					uprintf( "too much interfaces" );
					DBret("serargs");
					return 2 ;		// error
					}
				ser_if_no[if_index] = if_no_local ;
				}
			else
				{
				uprintf( "bad argument -i - no number" );
				DBret("serargs");
				return 1 ;
				}
			break ;
		case 'b' :	// -b=9600
			++pch ;
			if ( *pch == '=' || *pch == ':' ) ++pch ;
			ser_settings_field[if_no_local].baud = atol(pch) ;
			break ;
		case 'd' :	// -d=8 data bits
			++pch ;
			if ( *pch == '=' || *pch == ':' ) ++pch ;
			zch = *pch ;
			if ( zch >= '5' && zch <= '8' )
				{
				ser_settings_field[if_no_local].data = zch-'0' ;
				}
			else
				{
				uprintf( "bad -d parameter, allowed 5..8 data bits" );
				DBret("serargs");
				return 3 ;		// error
				}
			break ;
		case 's' :	// -s=0 stop bits
			++pch ;
			if ( *pch == '=' || *pch == ':' ) ++pch ;
			zch = *pch ;
			if ( zch >= '0' && zch <= '1' )
				{
				ser_settings_field[if_no_local].stop = zch-'0' ;
				}
			else
				{
				uprintf( "bad -s parameter, allowed 0..1 stop bits" );
				DBret("serargs");
				return 3 ;		// error
				}
			break ;
		case 'h' :	// -h=n x h handshake
			++pch ;
			if ( *pch == '=' || *pch == ':' ) ++pch ;
			zch = *pch ;
			switch ( zch )
				{
			case 'N':
			case 'X':
			case 'H':
				zch += 'a'-'A' ;
				}
			switch ( zch )
				{
			case 'n':
			case 'x':
			case 'h':

				ser_settings_field[if_no_local].handshake = zch ;
				break ;
			default :
				uprintf( "bad -h parameter, allowed handshake n (none) x (Xon-Xoff) h (hardware)" );
				DBret("serargs");
				return 3 ;		// error
				}	// end subswitch
			break ;
		case 'p' :	// -p=e o n parity
			++pch ;
			if ( *pch == '=' || *pch == ':' ) ++pch ;
			zch = *pch ;
			switch ( zch )
				{
			case 'N':
			case 'E':
			case 'O':
				zch += 'a'-'A' ;
				}
			switch ( zch )
				{
			case 'n':
			case 'e':
			case 'o':

				ser_settings_field[if_no_local].parity = zch ;
				break ;
			default :
				uprintf( "bad -p parameter, allowed parity n (none) e (even) o (odd)" );
				DBret("serargs");
				return 3 ;		// error
				}	// end subswitch
			}
		}		// end if *pch == '-'
	}			// next iarg
return 0 ;		// all arguments interpreted
}

// ----------------------------------------------------------
FUNCT void serargs_usage( void )
{
int i ;

vtputs( "Options for serial lines\n" );
vtputs( "-i=0 -i=2 ... use serial port number 0 and 2..\n" );
vtputs( "-b=9600              set speed\n" );
vtputs( "-p=e o n             set parity Even Odd None\n" );
vtputs( "-h=n x h             set handshake None, XonXoff, Hardware\n" );
vtputs( "-s=0 1               set additional stopbits\n" );
vtputs( "-d=8 7 6 5           set data bits\n" );
for ( i = 0 ; i < sermaxnumber ; ++i )
	{
	vtprintf( "-i=%d = %s   ", i, sername[i] );
	}
vtputs("\n");
#ifdef DOSGNU
vtputs( "prepare baudrate : mode com2: 300,N,8,1  or mode com2: 2400 e 7 2\n" );
#endif
}
