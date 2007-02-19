#define DB_INFO 0
#define DB_ENABLED 0

#define IDENT "2007-02-04"

#include <math.h>

#include <heport.h>				// special type names, uint16_t, int16_t ...
#include <helq.h>				// uprintf
#include <helang.h>				// logopen() / logclose()
#include <helog.h>				// logopen() / logclose()
#include <heserial.h>			// prototypes for all functions defined here
#include <hevt.h>				// Video Screen

// DOS: 0=COM1: 1=COM2
void usage( void ) ;
void testserial( int argc, const char **argv ) ;
void trmmenu( void );

bool flag_exit = false ;
bool flag_reopen = false ;
bool flag_hexdisplay = false ;

class skeycode
	{
	class skeycode *pnext ;
	uint16_t	code ;
	int slen ;
	char txt[1] ;
	} ;



double qqtsfloaerr[4];



FUNCT int main(int argc, const char **argv)
{


DBmain( "qqtstrm.dbg" );
DBprintf(( "qqtstrm : Mi., 3.6.1998 19:39:17.98 " ));
logopen( "c:\\temp\\qqtstrm.log" );
vtopen();
// vtcls();		   // enable direct screen

DBprintf(( "start with argv[1]=%v", argv[1] ));
testserial( argc, argv );

vtclose();
logclose();
DBexit();
return 0 ;
}


FUNCT void testserial(int argc, const char **argv)
{
uint keycode ;
uint receivecode ;
uint old_receivecode ;
int iarg ;
const char *pch ;
int rv ;
uint loopcount ;
int old_if_no ;					// interface - Number before called diologue
bool flag_arg_done ;


DBent("testserial");

old_receivecode = 0 ;			// no old receivecode
// default settings
ser_settings_field[ser_if_no[0]].baud = 9600 ;	   // baudrate
ser_settings_field[ser_if_no[0]].stop = 0 ;		   // number of stopbits
ser_settings_field[ser_if_no[0]].parity = 'n' ;	   // 'n' 'e' 'o'
ser_settings_field[ser_if_no[0]].data = 8 ;		   // 5..8
ser_settings_field[ser_if_no[0]].handshake = 'n' ; // 'n'-none 'x'-Xone-Xoff  'h'-Hardware


rv = serargs(argc,argv);	// interprets -b : Baudrate
					// -i : Interface number (to ser_ifno[4])

for ( iarg = 1 ; iarg < argc ; ++iarg )
	{
	pch = argv[iarg] ;
	if ( *pch == '-' )
		{
		flag_arg_done = false ;
		for ( ++pch ; *pch ; )
			{
			if ( flag_arg_done ) break ;
			switch (*pch)		// check for options
				{
			case 'i' :		// Interface number
			case 'p' :		// parity
			case 'h' :		// Handshake
			case 's' :		// stop bits
			case 'd' :		// data bits
			case 'b' :		// baud rate
				flag_arg_done = true ;
				break ;

			default :
					uprintf( "bad switch %d at %s", *pch, pch );
					usage();
					DBret("testserial");
					return ;
				}
			if ( *pch != 0 ) ++pch ;
			}			// next character
		}
	}
vtprintf( "\nQQTSTRM : Terminal Exit from connection by PC:End-Key, VT340:Prev-Screen\n" );

M_reopen :
if ( (rv = serset(ser_if_no[0], ser_settings_field[ser_if_no[0]] )) != 0 )
	{
	uprintf( "cannot set serial" );
	}

if ( (rv = seropen(ser_if_no[0])) != 0 )
	{
	uprintf( "cannot open serial interface rv=%d", rv );
	DBret("testserial");
	return ;
	}
// -----------------------------------------------------------------
// ------------------------- main loop -----------------------------
// -----------------------------------------------------------------
loopcount = 0 ;
for(;;)
	{
	if( serisicha(ser_if_no[0]) )		// check if at serial line came something
		{
		DBprintf(( "serial character detected" ));
		receivecode = serichaw( ser_if_no[0] ) ;	// get received character
		DBprintf(( "serial to vt : %XH", receivecode ));
		if ( receivecode >= 256 )
			{
			vtprintf( "from serichaw(%d) : receivecode = %u (%XH)",
									 ser_if_no[0],			 receivecode, receivecode );
//			  break ;
			}
		if ( receivecode < 32 )
			{
			switch( receivecode )
				{
			case 0x0d :
				vtocha( '\n' );
				break ;
			case 0x0a :
				if ( old_receivecode != 0x0d )
					{
					vtocha( '\n' ) ;	// Newline when CR or LF
					}
				break ;
			case 0x08 :		// backspace
				vtocha( 0x08 );
				break ;
			default:
				if ( !flag_hexdisplay )
					{
					vtocha( '<' );
					vthex2( receivecode );
					vtocha( '>' );
					}
				}
			}
		else
			{
			vtocha( receivecode ) ;
			}
		if ( flag_hexdisplay )
			{			// shows even non displayable characters
			if (	(uint8_t)receivecode <= 0x20 ||
					(uint8_t)receivecode >= 0x7f )
				{		// only non standard ASCII
				vtocha( '<' );
				vthex2( receivecode );
				vtocha( '>' );
				}
			}
		old_receivecode = receivecode ;
		if ( ++loopcount < 20000 ) continue ;
		loopcount = 0 ;
		}

	// If there is still time, then ask keyboard and send what typed in
	if ( vtiskey() )
		{
		keycode = vtkey();
		DBprintf(( "vt to serial : %XH", keycode ));
		if ( keycode >= 256 )
			{
			if ( keycode == VTK_F6 )
				{
				trmmenu();
				if ( flag_exit ) break ;
				old_if_no = ser_if_no[0] ;
				if ( flag_reopen )
					{
					serclose( old_if_no );
					goto M_reopen ;
					}
				}

			}
		if ( keycode <= 255 )
			{
			if ( keycode == VTK_ENTER )
				{
				serocha( ser_if_no[0], 0x0d );		// CR
//				serocha( ser_if_no[0], 0x0a );		// LF
				}
			else
				{
				serocha( ser_if_no[0], keycode );
				}
			}
		}
	}
vtprintf( "\nterminal finished\n" );
DBret("testserial");
return ;
}


FUNCT int serwait( const char *p, uint32_t maxmsec )
// return Value : 0 = no timeout
//				  1 = timeout
{
char c ;
uint32_t startmsec ;
char buffer[40] ;
uint n ;
uint i ;
uint j ;

n = strlen(p);

if ( n >= sizeof(buffer)-1 )
	{
	uprintf( "serwait called with too long pattern %s", p );
	return 3 ;
	}

i = 0 ;
startmsec = getmsec();
for(;;)
	{
	if ( *p == 0 ) return 0 ;	// Has found
	for(;;)		// wait for next character
		{
		if ( serisicha(ser_if_no[0]) ) break ;
		if ( diffmsec( startmsec ) > maxmsec ) 
			{
			return 1 ;	// Timeout
			}
		}
	c = serichaw( ser_if_no[0] );
	logvtocha( c );
	if ( i < n )
		{			// can append to partly filler buffer
		buffer[i++] = c ;
		buffer[i] = 0 ;
		}
	else
		{			// must roll the buffer
		for ( j = 0 ; j+1 < n ; ++j )
			{
			buffer[j] = buffer[j+1] ;
			}
		buffer[j++] = c ;
		buffer[j] = 0 ;
		}
	if ( strcmp( p, buffer ) == 0 ) return 0 ;
	}
}

FUNCT int sergets( char *p, uint max, char term, uint32_t maxmsec )
// return Value : 0 = no timeout
//				  1 = timeout
{
char c ;
uint32_t startmsec ;

startmsec = getmsec();
for(;;)
	{
	if ( max == 0 ) return 2 ;		// overfilled
	for(;;)		// wait for next character
		{
		if ( serisicha(ser_if_no[0]) ) break ;
		if ( diffmsec( startmsec ) > maxmsec ) return 1 ;	// Timeout
		}
	c = serichaw( ser_if_no[0] );
	logvtocha( c );
	if ( c == term ) 
		{
		*p = 0 ;
		return 0 ;
		}
	*p++ = c ;
	}
}



// there is the lowest , then , thenn ,then the highest byte as HEX
// e.g 1.0 is "0000083f"
FUNCT int decodehex( char const **ppc, float *p )
{
uint8_t *pc ;
pc = (uint8_t *)p;
if ( asctohx2( ppc, &pc[0] ) != 0 ) return 1 ;
if ( asctohx2( ppc, &pc[1] ) != 0 ) return 1 ;
if ( asctohx2( ppc, &pc[2] ) != 0 ) return 1 ;
if ( asctohx2( ppc, &pc[3] ) != 0 ) return 1 ;
return 0 ;
}


FUNCT int testfun( double a, double b, char fun )
{
uint8_t *pc ;
double expect ;
const char *pch ;
double x, y, diff ;
double dresult, relative_error ;
float ha,hb ;
float fexpect ;
float result ;
char sendstring[9] ;
char answer[40] ;


ha = (float)a ;
hb = (float)b ;
switch ( fun )
	{
	case '+' : expect = a + b ; break ;
	case '-' : expect = a - b ; break ;
	case '*' : expect = a * b ; break ;
	case '/' : expect = a / b ; break ;
	case 's' : if ( a > 100 ) return 0;
				if ( a < -100 ) return 0;
				expect = sin(a) ; break ;
	case 'c' : if ( a > 100 ) return 0;
				if ( a < -100 ) return 0;
			 expect = cos(a) ; break ;
	case 't' :  if ( a > 100 ) return 0;
				if ( a < -100 ) return 0;
			expect = tan(a) ; break ;
	case 'u' : expect = (double)(uint32_t)(a) ; break ;
	case 'v' : expect = (double)(int32_t)(a) ; break ;
	}
fexpect = (float)expect ;
logvtprintf( "Test %c with ha=%16.12g, hb=%16.12g\n", fun, a, b );
serocha( ser_if_no[0], 'h' );
serocha( ser_if_no[0], fun );
if ( serwait( "Input ha ?" , 1000 ) ) 
	{
	logvtprintf( "Timeout waiting for Input ha ?" );
	return 21 ;
	}
pc = (uint8_t *)&ha ;
nsprintf( sendstring, sizeof(sendstring), "%02x%02x%02x%02x", pc[0], pc[1], pc[2], pc[3] );
serputs( ser_if_no[0], sendstring );
if ( serwait( "Input hb ?" , 1000 ) ) 
	{
	logvtprintf( "Timeout waiting for Input hb ?" );
	return 22 ;
	}
pc = (uint8_t *)&hb ;
nsprintf( sendstring, sizeof(sendstring), "%02x%02x%02x%02x", pc[0], pc[1], pc[2], pc[3] );
serputs( ser_if_no[0], sendstring );
if ( serwait( "Result =" , 1000 ) ) 
	{
	logvtprintf( "Timeout waiting for Result =" );
	return 23 ;
	}
if ( sergets( answer, sizeof(answer), '\r', 1000 ) ) 
	{
	logvtprintf( "Timeout waiting for number" );
	return 24 ;
	}
pch = answer ;
if ( decodehex( &pch, &result ) )
	{
	logvtprintf( "error convert string hex to uint32_t %v", answer );
	return 25 ;
	}
pc = (uint8_t *)&result ;
logvtprintf( "  result %02x%02x%02x%02x\n", pc[0], pc[1], pc[2], pc[3] );
pc = (uint8_t *)&fexpect ;
logvtprintf( "  expect %02x%02x%02x%02x\n", pc[0], pc[1], pc[2], pc[3] );
dresult = result ;
y = expect ;
if ( y < 0 ) y = -y ;
diff = dresult - expect ;
if ( diff < 0 ) diff = -diff ;
if ( y ) relative_error = diff / y ; else relative_error = diff ;

if ( fun == 's' || fun == 'c' )
	{
	relative_error = diff ;
	}



if ( fun == '+' || fun == '-' )
	{
	x = a ;
	if ( x < 0 ) x = -x ;
	y = b ;
	if ( y < 0 ) y = -y ;
	if ( y > x ) x = y ;	// Keep maximum of absolute value
	y = expect ;
	if ( y < 0 ) y = -y ;
	if ( y > x ) x = y ;	// Keep maximum of absolute value
	diff = dresult - expect ;
	if ( diff < 0 ) diff = -diff ;
	if ( x ) relative_error = diff / x ; else relative_error = 0 ;
	if ( relative_error > 0.00001 )
		{
		logvtprintf( "ERROR ") ;
		}
	else
		{
		logvtprintf( "      " );
		}
	logvtprintf( "ADD, expect=%16.12g, result=%16.12g, diff=%fppm\n", expect, dresult, relative_error*1000000.0);
	if ( qqtsfloaerr[0] < relative_error ) qqtsfloaerr[0] = relative_error ;
	}
else if ( fun == '*' )
	{
	if ( relative_error > 0.00001 )
		{
		logvtprintf( "ERROR " );
		}
	else
		{
		logvtprintf( "      " );
		}
	logvtprintf( "MUL, expect=%16.12g, result=%16.12g, diff=%fppm\n", expect, dresult, relative_error*1000000.0);
	if ( qqtsfloaerr[1] < relative_error ) qqtsfloaerr[1] = relative_error ;
	}

else if ( fun == '/' )
	{
	if ( relative_error > 0.00001 )
		{
		logvtprintf( "ERROR " );
		}
	else
		{
		logvtprintf( "      " );
		}
	logvtprintf( "DIV, expect=%16.12g, result=%16.12g, diff=%fppm\n", expect, dresult, relative_error*1000000.0);
	if ( qqtsfloaerr[2] < relative_error ) qqtsfloaerr[2] = relative_error ;
	}

else
	{
	if ( relative_error > 0.00001 )
		{
		logvtprintf( "ERROR " );
		}
	else
		{
		logvtprintf( "      " );
		}
	logvtprintf( "fun %c, expect=%16.12g, result=%16.12g, diff=%fppm\n", fun, expect, dresult, relative_error*1000000.0);
	if ( qqtsfloaerr[3] < relative_error ) qqtsfloaerr[3] = relative_error ;
	}
if ( serwait( "qqtsser $" , 1000 ) ) return 37 ;
return 0 ;
}

// ----------------------------------------------------------
// user pressed VK_END - The end key
void trmmenu()
{
int i ;
static char interrupted[] =		TDEU("unterbrochen Hilfe = '?'")
								TENG("interrupted Help = '?'")
								TFRA("interrompté Aide = '?'");
uint zch ;
uint zch2 ;
bool flag_return ;
uint32_t xulong ;
char buffer[20] ;				// dialog line buffer
const char *pb ;

vtcurpos( 0, 23 );				// The last line
vtdeleol() ;
vtputs( interrupted );
for( i = sizeof(interrupted)-1 ; i < vt_xmax ; ++i ) vtocha('-');
vtocha( '\n' );
for( flag_return = false ; flag_return == false ; )			// until pressed blank, c - continue e - end
	{
	zch = vtkey();
	if ( zch >= 'A' && zch <= 'Z' ) zch += 'a' - 'A' ;	// tolower
	switch ( zch )
		{

	case 'q' :		// Quit
		flag_exit = true ;
		flag_return = true ;
		break ;

	case ' ' :		// continue from menu
	case 'c' :		// continue from menu
		flag_return = true ;
		break ;

	case 'b' :		// baud rate dialog
		vtgetlin( buffer, sizeof(buffer) );
		pb = buffer ;
		if ( asctoul( &pb, &xulong ) == 0 )
			{
			ser_settings_field[ser_if_no[0]].baud = xulong ;
			flag_reopen = true ;
			}
		break ;

	case 'r' :					// Reset interface
		flag_reopen = true ;
		break ;

	case 'x' :
		flag_hexdisplay = (bool)!flag_hexdisplay ;	// toggle
		break ;

	case 'h' :
		zch2 = vtkey();
		if ( zch2 >= 'A' && zch2 <= 'Z' ) zch2 += 'a' - 'A' ;	// tolower
		switch ( zch2 )
			{
		case 'n' :
		case 'h' :
		case 'x' :	ser_settings_field[ser_if_no[0]].handshake	= zch2 ;
			flag_reopen = true ;
			}
		break ;

	case 'p' :
		zch2 = vtkey();
		if ( zch2 >= 'A' && zch2 <= 'Z' ) zch2 += 'a' - 'A' ;	// tolower
		switch ( zch2 )
			{
		case 'n' :
		case 'e' :
		case 'o' :	ser_settings_field[ser_if_no[0]].parity	 = zch2 ;
			flag_reopen = true ;
			}
		break ;

	case 's' :
		zch2 = vtkey();
		if ( zch2 >= 'A' && zch2 <= 'Z' ) zch2 += 'a' - 'A' ;	// tolower
		switch ( zch2 )
			{
		case '0' :
		case '1' :	ser_settings_field[ser_if_no[0]].stop = zch2 - '0' ;
			flag_reopen = true ;
			}
		break ;

	case 'd' :
		zch2 = vtkey();
		if ( zch2 >= 'A' && zch2 <= 'Z' ) zch2 += 'a' - 'A' ;	// tolower
		switch ( zch2 )
			{
		case '5' :
		case '6' :
		case '7' :
		case '8' :	ser_settings_field[ser_if_no[0]].data = zch2 - '0' ;
			flag_reopen = true ;
			}
		break ;
	case 't' : 
		{
		int loop ;
        double da ;
		double db ;

		vtputs( "\nTest qqtsfloa (hex interface)\n" );
	
		// The synchronisation
		for(;;)
			{
			serocha( ser_if_no[0], ' ' );
			if ( serwait( "qqtsser $" , 1000 ) == 0 )  break ;
			}

		testfun( 0, 0, '+' );
		for ( da = -10.0 ; da < 6.28 ; da += 0.1 )
			{
			testfun( da, 0, 's' );	// sin()
			testfun( da, 0, 'c' );	// cos()
			testfun( da, 0, 't' );	// tan()
			}

		testfun(           1.0, 0, 'u' );	// r = uint32_t
		testfun(         256.0, 0, 'u' );	// r = uint32_t
		testfun(       65536.0, 0, 'u' );	// r = uint32_t
		testfun(       65537.0, 0, 'u' );	// r = uint32_t

		testfun(          -1.0, 0, 'v' );	// r = int32_t
		testfun(        -256.0, 0, 'v' );	// r = int32_t
		testfun(      -65536.0, 0, 'v' );	// r = int32_t
		testfun( -1233165537.0, 0, 'v' );	// r = int32_t
		testfun(   333333333.0, 0, 'v' );	// r = int32_t
		testfun(    33333333.0, 0, 'v' );	// r = int32_t

		testfun( 5.0, 1.0,  '/' );
		testfun( 8.0, 2.0,   '/' );
		testfun( 90.0, 18.0, '/' );
		testfun( 0.0, 18.0,  '/' );
		testfun( 18.0, 0.0,  '/' );
		testfun( -18.0, 0.0, '/' );
		testfun( -18.0, 0.0, '*' );
		testfun( 0.0, 377.0, '*' );
		testfun( 0.0, 0.0,   '+' );
		testfun( 0.0, 0.0,   '-' );
		testfun( 0.0, 5.0,   '+' );
		testfun( 0.0, 5.0,   '-' );
		testfun( 6.0, 0.0,   '+' );
		testfun( 6.0, 0.0,   '-' );
		testfun( -41.0, 0.0,   '+' );
		testfun( -41.0, 0.0,   '-' );
		testfun( 0.0, -30.0,   '+' );
		testfun( 0.0, -30.0,   '-' );

		// The test loop
		for(loop = 0 ; ; ++loop )
			{
			da = dprbs() * 200000 - 100000 ;
			db = dprbs() * 200000 - 100000 ;
			if ( testfun( da, db, '+' ) ) break ;
			if ( testfun( da, db, '*' ) ) break ;
			if ( testfun( da, db, '/' ) ) break ;
			if ( testfun( da, db, 's' ) ) break ;
			if ( testfun( da, db, 'c' ) ) break ;
			if ( testfun( da, db, 't' ) ) break ;
			if ( testfun( da, db, 'u' ) ) break ;
			if ( testfun( da, db, 'v' ) ) break ;

			if ( vtiskey() )
				{
				zch = vtkey();
				if ( zch == 'e' || zch == 'x' ) 
					{
					logvtprintf( "\nUser key abort\n" );
					break ;
					}
				}
			}
		vtprintf( "End of Test qqtsfloa\n" );
		logvtprintf( "max diff ADD = %fppm\n", qqtsfloaerr[0]*1000000);
		logvtprintf( "max diff MUL = %fppm\n", qqtsfloaerr[1]*1000000);
		logvtprintf( "max diff DIV = %fppm\n", qqtsfloaerr[2]*1000000);
		}
		break ;

	default :
vtprintf(	"QQTSTRM - Test QQTSFLOA\n", IDENT );
vtcompil();
vtnl();
vtputs(	  "q - Quit Terminal program qqtstrm\n" );
vtputs(	  "c - Continue\n" );
vtputs(	  "        Interface settings:\n" );
vtprintf( "r - Reset interface (reopen)                         \002%s\001  \n",
	   flag_reopen ? "will be reopened" : "not to reopen" );
vtprintf( "b - Baud rate is                                     \002%d\001  \n", ser_settings_field[ser_if_no[0]].baud );
vtprintf( "s - Number of stopbits is 0 ... 1                    \002%d\001  \n", ser_settings_field[ser_if_no[0]].stop );
vtprintf( "p - Parity is 'n'-none 'e'-even 'o'-odd				\002%c\001	\n", ser_settings_field[ser_if_no[0]].parity );
vtprintf( "d - Data bits is 5 ... 8                             \002%d\001  \n", ser_settings_field[ser_if_no[0]].data );
vtprintf( "h - Handshake is 'n'-none 'x'-Xone-Xoff 'h'-Hardware \002%c\001	\n", ser_settings_field[ser_if_no[0]].handshake );
vtprintf( "x - HeX display of received characters               \002%s\001  \n", flag_hexdisplay ? "ON" : "OFF" );
vtprintf( "        Special tests for qqtsfloa                      \n" );
vtprintf( "t - Test with decimale number transfer                  \n" );
vtprintf( "u - Test with hexadecimale number transfer              \n" );
		}
	}
}


// ----------------------------------------------------------
void usage()
{
int i ;

vtputs( "25.9.1998 10:30\n");
vtputs( "usage : qqtstrm -i=1     communication through serial port\n" );
vtputs( "options : -i=0  -i=1  -i=2 ... use serial port number 0, 1, 2..\n" );
vtputs( "          -b=9600              set speed\n" );
for ( i = 0 ; i < sermaxnumber ; ++i )
	{
	vtprintf( "-i=%d = %s   ", i, sername[i] );
	}
vtputs("\n");
#ifdef DOSGNU
vtputs( "prepare baudrate : mode com2: 300,N,8,1  or mode com2: 2400 e 7 2\n" );
#endif
}
