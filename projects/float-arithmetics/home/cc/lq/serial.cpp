// From J. Herberts library LQ

#define DB_INFO 0		// activation of DBprintf(( ... )); logging of operation
#define DB_ENABLED 0	// activation of debugging logging of occured faults

#include <heport.h>				// special type names, uint16_t, int16_t ...
#ifdef MDOS40
#include <mapidefs.h>
#endif

#ifdef OSK
#include <string.h>
#endif

#include <helq.h>				// uprintf
#include <hedeb.h>		// DBprintf
#include <hevt.h>		// vt...
#include <cfifo.h>
#include <heserial.h>			// prototypes for all functions defined here



// direct debugging
#if 0
#define DBvt( a ) vtprintf a , vtrefresh()
#include <hevt.h>
#else
#define DBvt( a )
#endif


// determine which of all the driveres from this source to be compiled
#ifdef DOSGNU
#define OWN_DOS
#define POLLING 1
#endif
#ifdef __TURBOC__
#define OWN_DOS
#define POLLING 1
#endif
#ifdef MDOS10
#define BIOS_SERVICE   // or the classical dos compatible shit
// #define OWN_DOS		// or own driver,
#endif


#ifdef _WIN31_
#	 define _WIN31_HANDLE	// either the win32- function
						   // who cannot receive properly
						// because of a lack of information to me
						// with or without polling
#endif

// #ifdef DOSGNU
// #define DOSCLASSIC
// #endif

#ifdef DOSGNU
#include <dpmi.h>		// dpmi - interface
#endif

#ifdef __TURBOC__
#include <dos.h>
#endif




struct sserial ser_settings_field[MAX_INTERFACENO] =
	{
	// baud, stop, parity, data, handshake
#if MAX_INTERFACENO > 4
		{ 9600, 0, 'n', 8, 'n' } ,
#endif
#if MAX_INTERFACENO > 3
		{ 9600, 0, 'n', 8, 'n' } ,
#endif
#if MAX_INTERFACENO > 2
		{ 9600, 0, 'n', 8, 'n' } ,
#endif
		{ 9600, 0, 'n', 8, 'n' } ,
		{ 9600, 0, 'n', 8, 'n' }
	} ;




#ifdef OSK
bool bu_ser_valid[MAX_INTERFACENO] ;
struct	sgbuf bu_ser_save[MAX_INTERFACENO] ;
struct	sgbuf bu_ser_path[MAX_INTERFACENO] ;
#endif

// the sequence of -i parameters : interface parameters of invocation
uint8_t ser_if_no[MAX_INTERFACENO] = {
0
#if MAX_INTERFACENO > 1
, 1
#if MAX_INTERFACENO > 2
, 2
#if MAX_INTERFACENO > 3
, 3
#if MAX_INTERFACENO > 4
, 4
#endif
#endif
#endif
#endif

} ;

#if defined( MDOS40 ) || defined( MSVC40 ) || defined( __TURBOC__ ) || defined( DOSGNU )
char *sername[MAX_INTERFACENO] = { // <a name=MDOS40:sername></a>
"COM1\000PATCH...---...-"		// -i=0
#if MAX_INTERFACENO > 1
, "COM7\000PATCH---...---."
#if MAX_INTERFACENO > 2
, "COM2\000PATCH---...---."
#if MAX_INTERFACENO > 3
, "COM4\000PATCH---..---.."
#if MAX_INTERFACENO > 4
, "COM5\000PATCH---..---.." // -i=4 USB - Serial, digitus
#endif
#endif
#endif
#endif
 } ;

#endif

// until 2005-01-14 : /dev/cua0 ...
#ifdef LINUX
char *sername[MAX_INTERFACENO] = { // <a name=LINUX:sername></a>
	"/dev/ttyS0\000PATCH"
#if MAX_INTERFACENO > 1
	,"/dev/ttyUSB0\000PATCH"
#if MAX_INTERFACENO > 2
	,"/dev/ttyUSB1\000PATCH"
#if MAX_INTERFACENO > 3
	,"/dev/ttyS1\000PATCH"
#if MAX_INTERFACENO > 4
	,"/dev/ttyS3\000PATCH"
#endif
#endif
#endif
#endif
		} ;
#endif

#ifdef OSK
char *sername[MAX_INTERFACENO] = {// <a name=OSK:sername></a>


	"/t1\000PATCH"
#if MAX_INTERFACENO > 1
	,"/t10\000PATCH"
#if MAX_INTERFACENO > 2
	,"/t11\000PATCH"
#if MAX_INTERFACENO > 3
	,"/t12\000PATCH"
#if MAX_INTERFACENO > 4
	,"/t13\000PATCH"
#if MAX_INTERFACENO > 5
	,"/t14\000PATCH"
#if MAX_INTERFACENO > 6
	,"/t15\000PATCH"
#if MAX_INTERFACENO > 7
	,"/t16\000PATCH"
#endif
#endif
#endif
#endif
#endif
#endif
#endif
	} ;
#endif



int sermaxnumber = MAX_INTERFACENO ;

#if defined( MSVC40 ) || defined( MDOS40 )
HANDLE fdser[ MAX_INTERFACENO ] ;
#else
tfh fdser[ MAX_INTERFACENO ] ;
#endif
bool seropened[ MAX_INTERFACENO ] ;
struct sserial serdeffield[MAX_INTERFACENO] ;

FUNCT int sersetact(uint8_t ser_port_no);



#ifdef XXX_DOSGNU_UNUSED


char *sername[MAX_INTERFACENO] = { "COM1", "COM2", "COM3", "COM4" } ;
int sermaxnumber = MAX_INTERFACENO ;

int fdser[MAX_INTERFACENO] ;
bool seropened[MAX_INTERFACENO] ;

#endif



#ifdef LINUX
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>


struct termios ser_stat_orig ;	 /* Terminal-Einstellung bei Programmstart */
uint8_t linux_ser_code_c1[ MAX_INTERFACENO ] ;	   // last got character

extern int errno ;

FUNCT int seropen( uint8_t ser_port_no ) // LINUX
{
int rv ;
struct termios xt ;


if ( ser_port_no < MAX_INTERFACENO )
	{
	if ( !seropened[ser_port_no] )
		{
		fdser[ser_port_no] = normopen( sername[ser_port_no] , O_RDWR );
		if ( fdser[ser_port_no] >= 0 )
			{
			fcntl( fdser[ser_port_no], F_SETFL, O_NONBLOCK );
			tcgetattr( fdser[ser_port_no] , &xt );			  /* Aktuelle Einstellung holen für Änderung */
			tcgetattr( fdser[ser_port_no] , &ser_stat_orig );  /* Aktuelle Einstellung für Exit */
			xt.c_lflag &= ~ICANON ;
			xt.c_lflag &= ~ECHO ;
/* xt.c_iflag &= ~ICRNL ; */
			rv = tcsetattr( fdser[ser_port_no] , TCSANOW, &xt );
			if ( rv < 0 )
				{
				uprintf( "cannot tcsetattr serial line %s", sername[ser_port_no] );
				}
			seropened[ser_port_no] = true ;
			return 0 ;		// opened succesful
			}
		uprintf( "cannot open %s for RW, errno=%d", sername[ser_port_no] , errno );
		return 1 ;
		}
	return 2 ;
	}
else
	{
	uprintf( "bad interface number" );
	return 4 ;
	}
}



// --------------------------------------------------------------
// Set serial parameters
FUNCT int serset(uint8_t ser_port_no,	 struct sserial &definition ) // LINUX
{
int rv ;

rv = 0 ;
// but never set the parameters
return rv ;
}



FUNCT int serclose( uint8_t ser_port_no )		// LINUX
{
int rv ;

if ( ser_port_no < MAX_INTERFACENO )
	{
	tcsetattr( fdser[ser_port_no], TCSANOW, &ser_stat_orig );
	close( fdser[ser_port_no] );
	// vt_opened = false ;
	rv = 0 ;			// no error
	}
else
	{
	rv = 1 ;			// some error
	}
}


FUNCT bool serisicha( uint8_t ser_port_no )	 // LINUX
{
int rv ;
uint8_t cz ;

if ( ser_port_no < MAX_INTERFACENO )
	{
	if ( linux_ser_code_c1[ser_port_no] == 0 )
		{
		cz = 0 ;
		rv = read( fdser[ser_port_no], &cz, 1);
		linux_ser_code_c1[ser_port_no]	= cz ;
		}
	else
		{
		rv = 1 ;
		}
	return ( rv == 1 ) ;
	}
return 0 ;
}


FUNCT void serflush( uint8_t ser_port_no, uint32_t ulTimeout )	 // LINUX
{
int rv ;
uint8_t cz ;
uint32_t starttime ;

starttime = getmsec();
if ( ser_port_no < MAX_INTERFACENO )
	{
	linux_ser_code_c1[ser_port_no] = 0 ;

	for(;;)
		{
		cz = 0 ;
		rv = read( fdser[ser_port_no], &cz, 1);
		if ( cz == 0 )
			{
			if ( diffmsec(starttime) > ulTimeout ) break ;
			}
		else
			{
			starttime = getmsec();
			}
		}
	}
return ;
}



FUNCT uint serichaw(uint8_t ser_port_no)					// LINUX
{
uint8_t c2 ;

while ( !serisicha( ser_port_no ) )
	{
	if ( i_uprintf_answer == ANSWERTYPE_CANCEL )
		{
		return 8 ;
		}
	}
if ( linux_ser_code_c1[ser_port_no]	 == 0 )
	{
	Eprintf(( "ERROR vtkey: vtiskey true, even if linux_ser_code_c1==0" ));
	}
c2 = linux_ser_code_c1[ser_port_no]	 ;
linux_ser_code_c1[ser_port_no]	= 0 ;
return c2 ;
}




FUNCT void serocha( uint8_t ser_port_no , char c1 )	 // LINUX
{
int rv ;

if ( ser_port_no < MAX_INTERFACENO )
	{
	rv = write( fdser[ser_port_no], &c1, 1);
//		fflush( fdser[ ser_port_no] );
	if ( rv != 1 )
		{
		uprintf( "cannot send serial character" );
		}
	}
return ;
}

FUNCT int setrts(uint8_t ser_port_no, bool status)	 // LINUX
{

return 0 ;
}


FUNCT int setdtr(uint8_t ser_port_no, bool status)	 // LINUX
{

return 0 ;
}

#endif		// LINUX





#ifdef XXX_DOSGNU_UNUSED
// --------------------------------------------------------------
FUNCT int seropen(uint8_t ser_port_no)		// XXX_DOSGNU_UNUSED
{
if ( ser_port_no >= MAX_INTERFACENO )
	{
	uprintf( "ser_port_no %d not supported", ser_port_no );
	return 3 ;
	}
if ( !seropened[ser_port_no] )
	{
	fdser[ser_port_no] = normopen( sername[ser_port_no] , O_RDWR );
	if ( fdser[ser_port_no] >= 0 )
		{
		seropened[ser_port_no] = true ;
		return 0 ;		// opened succesful
		}
	uprintf( "cannot open %s for RW", sername[ser_port_no] );
	return 1 ;
	}
return 2 ;
}



// --------------------------------------------------------------
FUNCT int serclose(uint8_t ser_port_no)		// XXX_DOSGNU_UNUSED
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	if ( seropened[ser_port_no] )
		{
		seropened[ser_port_no] = false ;
			normclose( fdser[ ser_port_no ] );
		}
	}
return 0 ;
}

// --------------------------------------------------------------
FUNCT void serocha( uint8_t ser_port_no, char c1 )  // XXX_DOSGNU_UNUSED
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	normwrite( fdser[ser_port_no], &c1, 1 );
	}
}

// --------------------------------------------------------------
FUNCT uint serichaw(uint8_t ser_port_no)		// XXX_DOSGNU_UNUSED
{
int rv ;
char c1 ;
if ( ser_port_no < MAX_INTERFACENO )
	{
	rv = normread( fdser[ser_port_no], &c1, 1 );
	if ( rv == 1 )
		{
		return c1 ;
		}
	}
return ~0 ;		// any error
}

// --------------------------------------------------------------
FUNCT bool serisicha(uint8_t ser_port_no)		// XXX_DOSGNU_UNUSED
{
int rv ;
char c1 ;

DBent("serisicha");
if ( ser_port_no < MAX_INTERFACENO )
	{
	rv = normread( fdser[ser_port_no], &c1, 1 );
	if ( rv == 1 )
		{
		DBprintf(( "normread returned 1" ));
		DBret("serisicha");
		return c1 ;
		}
	DBprintf(( "normread returned rv=%d", rv ));
	}
DBret("serisicha");
return -1 ;		// any error
}
#endif		// DOSGNU






// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------
#ifdef DOSCLASSIC

char *sername[MAX_INTERFACENO] = { "COM1", "COM2", "COM3", "COM4" } ;

// int fdser[MAX_INTERFACENO] ;
// bool seropened[MAX_INTERFACENO] ;

// --------------------------------------------------------------
int seropen(uint8_t ser_port_no)			// DOSCLASSIC
{
DBvt(( "seropen: will open port %d\n", ser_port_no ));


if ( ser_port_no >= MAX_INTERFACENO )
	{
	uprintf( "ser_port_no %d not supported", ser_port_no );
	return 3 ;
	}
if ( !seropened[ser_port_no] )
	{
	fdser[ser_port_no] = normopen( sername[ser_port_no] , O_RDWR );
	if ( fdser[ser_port_no] >= 0 )
		{
		seropened[ser_port_no] = true ;
		return 0 ;		// opened succesful
		}
	uprintf( "cannot open %s for RW", sername[ser_port_no] );
	return 1 ;
	}
uprintf( "tried to open already opened serial port %s", sername[ser_port_no] );
return 2 ;
}

// --------------------------------------------------------------
// Set serial parameters
FUNCT int serset(uint8_t ser_port_no,	 struct sserial &definition ) //
{
int rv ;

rv = 0 ;
// but never set the parameters
return rv ;
}


// --------------------------------------------------------------
FUNCT int serclose(uint8_t ser_port_no)		// DOSCLASSIC
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	if ( seropened[ser_port_no] )
		{
		seropened[ser_port_no] = false ;
		normclose( fdser[ ser_port_no ] );
		}
	}
return 0 ;
}

// --------------------------------------------------------------
FUNCT void serocha(uint8_t ser_port_no,char c1)	   // DOSCLASSIC
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	normwrite( fdser[ser_port_no], &c1, 1 );
	}
}

// --------------------------------------------------------------
FUNCT uint serichaw(uint8_t ser_port_no)		// DOSCLASSIC
{
int rv ;
char c1 ;
if ( ser_port_no < MAX_INTERFACENO )
	{
	rv = normread( fdser[ser_port_no], &c1, 1 );
	if ( rv == 1 )
		{
		return c1 ;
		}
	}
return ~0 ;		// any error
}

// --------------------------------------------------------------
FUNCT void serflush(uint8_t ser_port_no,uint32_t timeout)	  // DOSCLASSIC
{
bool flag_timeout = false ;
uint32_t startmsec ;
int is ;
char rch ;


is = 0 ;
for(;;)
	{
	if ( serisicha(if_no) )
		{
		rch = serichaw( if_no );
		flag_timeout = false ;
		}
	else
		{
		if ( flag_timeout == false )
			{
			startmsec = getmsec() ;
			flag_timeout = true ;
			}
		else
			{
			if ( diffmsec(startmsec) > timeout )
				{
				break ;
				}
			}
		}
	}
return ;
}

// --------------------------------------------------------------
FUNCT bool serisicha(uint8_t ser_port_no)		// DOSCLASSIC
{
DBent("serisicha");

DBret("serisicha");
return true ;	// A character is at input
}
#endif		// DOSCLASSIC









// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------
#ifdef BIOS_SERVICE
#include <bios.h>

char *sername[MAX_INTERFACENO] = { "COM1", "COM2", "COM3", "COM4" } ;

// int fdser[MAX_INTERFACENO] ;
// bool seropened[MAX_INTERFACENO] ;

// --------------------------------------------------------------
int seropen(uint8_t ser_port_no)			// BIOS_SERVICE
{
uint initdata ;

DBvt(( "seropen: will open port %d\n", ser_port_no ));


if ( ser_port_no >= MAX_INTERFACENO )
	{
	uprintf( "ser_port_no %d not supported", ser_port_no );
	return 3 ;
	}
if ( !seropened[ser_port_no] )
	{
	initdata = 0 ;
	switch( ser_settings_field[ser_port_no].data )
		{
	case 5 : uprintf( "5 Data bits not supported" ); break ;
	case 6 : uprintf( "6 Data bits not supported" ); break ;
	case 7 : initdata |= _COM_CHR7 ; break ;
	case 8 : initdata |= _COM_CHR8 ; break ;
	default : uprintf( "unknown data bits %d", ser_settings_field[ser_port_no].data );
		}
	switch( ser_settings_field[ser_port_no].parity )
		{
	case 'n' : initdata |= _COM_NOPARITY ; break ;
	case 'e' : initdata |= _COM_EVENPARITY ; break ;
	case 'o' : initdata |= _COM_ODDPARITY ; break ;
	default : uprintf( "unknown parity key %c", ser_settings_field[ser_port_no].parity );
		}
	switch( ser_settings_field[ser_port_no].baud )
		{
	case 110L : initdata |= _COM_110 ; break ;
	case 150L : initdata |= _COM_150 ; break ;
	case 300L : initdata |= _COM_300 ; break ;
	case 600L : initdata |= _COM_600 ; break ;
	case 1200L : initdata |= _COM_1200 ; break ;
	case 2400L : initdata |= _COM_2400 ; break ;
	case 4800L : initdata |= _COM_4800 ; break ;
	case 9600L : initdata |= _COM_9600 ; break ;
	default : uprintf( "Supported baud rates: 110,150,300,600,1200,2400,4800,9600, not %lu",
				ser_settings_field[ser_port_no].baud );
		}
	switch( ser_settings_field[ser_port_no].stop )
		{
	default :
	case 0 : initdata |= _COM_STOP1 ; break ;
	case 1 : initdata |= _COM_STOP2 ; break ;
		}

	_bios_serialcom( _COM_INIT, ser_port_no, initdata );
	seropened[ser_port_no] = true ;
	return 0 ;		// opened succesful
	}
uprintf( "tried to open already opened serial port %s", sername[ser_port_no] );
return 2 ;
}

// --------------------------------------------------------------
// Set serial parameters
FUNCT int serset(uint8_t ser_port_no,	 struct sserial &definition ) //
{
int rv ;

rv = 0 ;
// but never set the parameters
return rv ;
}


// --------------------------------------------------------------
FUNCT int serclose(uint8_t ser_port_no)		// BIOS_SERVICE
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	if ( seropened[ser_port_no] )
		{
		seropened[ser_port_no] = false ;
		normclose( fdser[ ser_port_no ] );
		}
	}
return 0 ;
}

// --------------------------------------------------------------
FUNCT void serocha(uint8_t ser_port_no,char c1)	   // BIOS_SERVICE
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	_bios_serialcom( _COM_SEND, ser_port_no, c1 );
	}
}

// --------------------------------------------------------------
FUNCT uint serichaw(uint8_t ser_port_no)		// BIOS_SERVICE
{
uint rv ;
char c1 ;
if ( ser_port_no < MAX_INTERFACENO )
	{
	rv = _bios_serialcom( _COM_RECEIVE, ser_port_no, c1 );
	return rv & 0x0FF ;
	}
return ~0 ;		// any error
}

// --------------------------------------------------------------
FUNCT bool serisicha(uint8_t ser_port_no)		// BIOS_SERVICE
{
uint rv ;


rv = _bios_serialcom( _COM_STATUS, ser_port_no, 0 );
if ( rv & 0x100 )
	{
	return true ;	// A character is at input
	}
else
	{
	return false ;
	}
}


FUNCT void serflush( uint8_t ser_port_no, uint32_t timeout_msec )	 // BIOS_SERVICE
{
uint32_t starttime ;

starttime = getmsec();
while ( diffmsec(starttime) < timeout_msec )
	{
	if ( serisicha( ser_port_no ) )
		{
		serichaw( ser_port_no );
		starttime = getmsec();
		}
	}
return ;
}




FUNCT int setrts(uint8_t ser_port_no, bool status)	 // BIOS_SERVICE
{

return 0 ;
}


FUNCT int setdtr(uint8_t ser_port_no, bool status)	 // BIOS_SERVICE
{

return 0 ;
}

FUNCT bool getdsr(uint8_t ser_port_no )		// BIOS_SERVICE
{
uint rv ;

rv = _bios_serialcom( _COM_STATUS, ser_port_no, 0 );
if ( rv & 0x20 )
	{
	return true ;	// A character is at input
	}
else
	{
	return false ;
	}

}

FUNCT bool getcts( uint8_t ser_port_no )	   // BIOS_SERVICE
{
uint rv ;


rv = _bios_serialcom( _COM_STATUS, ser_port_no, 0 );
if ( rv & 0x10 )
	{
	return true ;	// A character is at input
	}
else
	{
	return false ;
	}

}



#endif		// BIOS_SERVICE



// --------------------------------------------------------------------
// --------------------------------------------------------------------



#if defined( MDOS40 ) || defined( MSVC40 )
int seropen(uint8_t ser_port_no)		// _WIN31_HANDLE
{
SECURITY_ATTRIBUTES seca ;
COMMTIMEOUTS  CommTimeOuts ;
int rv ;


if ( ser_port_no >= MAX_INTERFACENO )
	{
	uprintf( "ser_port_no %d not supported", ser_port_no );
	return 3 ;
	}
if ( !seropened[ser_port_no] )
	{
	seca.nLength = sizeof(SECURITY_ATTRIBUTES);
	seca.lpSecurityDescriptor = 0 ; // use systems default
	seca.bInheritHandle = FALSE ;	// do not inherit handle


	fdser[ser_port_no] = CreateFile(
sername[ser_port_no],			// #1 LPCTSTR  lpFileName,
//										  // pointer to name of the file
GENERIC_READ | GENERIC_WRITE,	// #2DWORD	dwDesiredAccess,
//										  // access (read-write) mode
// HJH990501
// 0,							   // #3 exclusive access
FILE_SHARE_READ | FILE_SHARE_WRITE ,
&seca,							// #4 no security attrs
OPEN_EXISTING,					// #5
FILE_ATTRIBUTE_NORMAL |
// FILE_FLAG_OVERLAPPED |		// overlapped I/O
0 ,								// end of parameter #6
NULL );							// parameter #7
	if ( fdser[ser_port_no] == INVALID_HANDLE_VALUE )
		{
		uprintf( "cannot open %s for RW", sername[ser_port_no] );
		return 1 ;
		}
	seropened[ser_port_no] = true ;
		  // get any early notifications

	SetCommMask( fdser[ser_port_no], EV_RXCHAR ) ;

	// setup device buffers

	SetupComm( fdser[ser_port_no], 4096, 4096 ) ;

	// purge any information in the buffer

	PurgeComm( fdser[ser_port_no], PURGE_TXABORT | PURGE_RXABORT |
									  PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	// set up for overlapped I/O

	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.WriteTotalTimeoutConstant = 1000 ;
	SetCommTimeouts( fdser[ser_port_no], &CommTimeOuts ) ;

	rv = sersetact(ser_port_no) ;
	// rv = 0 = succesful
	return rv ;		 // opened succesful
	}
return 2 ;
}

// --------------------------------------------------------------

FUNCT int sersetact(uint8_t ser_port_no)	   // _WIN31_HANDLE
{
int rv ;
DCB		   dcb ;

rv = 0 ;
dcb.DCBlength = sizeof( DCB ) ;
GetCommState( fdser[ser_port_no], &dcb ) ;

dcb.BaudRate = serdeffield[ser_port_no].baud ;
dcb.ByteSize = serdeffield[ser_port_no].data ;
switch ( serdeffield[ser_port_no].parity )
	{
	case 'n' :
		dcb.fParity = FALSE ;
		dcb.Parity = NOPARITY ;
		break ;
	case 'e' :
		dcb.fParity = TRUE ;
		dcb.Parity = EVENPARITY ;
		break ;
	case 'o' :
		dcb.fParity = TRUE ;
		dcb.Parity = ODDPARITY ;
		break ;
	case 'm' :
		dcb.fParity = FALSE ;
		dcb.Parity = MARKPARITY ;
		break ;
	default :
		uprintf( "bad parity selection %c", serdeffield[ser_port_no].parity );
		break ;
	}

if ( serdeffield[ser_port_no].stop < 0 || serdeffield[ser_port_no].stop > 2 )
	{
	uprintf( "illegal stop bits, is %d, should be 0 / 1 / 2", serdeffield[ser_port_no].stop );
	dcb.StopBits = 0 ;		// 1 Stopbit
	}
else
	{
	dcb.StopBits = serdeffield[ser_port_no].stop ;		// 0,1,2 = 1, 1.5, 2
	}

// setup hardware flow control
switch ( serdeffield[ser_port_no].handshake )
	{
case 'x' :		// xon-xoff handshake
	dcb.fDtrControl = DTR_CONTROL_ENABLE ;
	dcb.fOutxDsrFlow = 0 ;
	dcb.fOutxCtsFlow = 0 ;
	dcb.fRtsControl = RTS_CONTROL_ENABLE ;
	dcb.fInX = dcb.fOutX = 1 ;
	dcb.XonChar = 0x11 ;		// Control-Q
	dcb.XoffChar = 0x13 ;		// Control-S
	dcb.XonLim = 100 ;
	dcb.XoffLim = 100 ;
	break ;
case 'h' :		// hardware handshake
	// this way I intended to use parameters
	dcb.fDtrControl = 1 ; // DTR_CONTROL_HANDSHAKE ;
	dcb.fRtsControl = RTS_CONTROL_HANDSHAKE ;

	dcb.fOutxDsrFlow = 0 ;		// stop output
	dcb.fOutxCtsFlow = 1 ;

	dcb.fInX = dcb.fOutX = 0 ;
	dcb.XonChar = 0 ;
	dcb.XoffChar = 0 ;
	dcb.XonLim = 100 ;
	dcb.XoffLim = 100 ;

	break ;
default :
	uprintf( "handshake selector =%c", serdeffield[ser_port_no].handshake );
	rv = 6 ;

case 'n' :		// no handshake
	dcb.fDtrControl = 0 ;	// 0 = wird gruen 1 = wird rot
	dcb.fOutxDsrFlow = 0 ;
	dcb.fOutxCtsFlow = 0 ;
	dcb.fRtsControl = 0 ;	// 0 = wird gruen 1 = wird rot
	dcb.fInX = dcb.fOutX = 0 ;
	dcb.XonChar = 0 ;
	dcb.XoffChar = 0 ;
//	  dcb.XonLim = 65535 ;		// win 98 presets and accepts 65535
								// NT presets 2048 and does not accets 65535
//	  dcb.XoffLim = 65535 ;		// win 98 presets and accepts 65535
								// NT presets 512 and does not accets 65535
	dcb.fNull = 0 ;
	break ;
	}
// other various settings

dcb.fBinary = TRUE ;
dcb.fParity = TRUE ;

if ( SetCommState( fdser[ser_port_no], &dcb ) == 0 )
	{
	uprintf( "cannot set parameters of serial interface %s",
				sername[ser_port_no] );
	rv = GetLastError();
	}

return ( rv ) ;
}



// --------------------------------------------------------------

FUNCT int setrts(	  // _WIN31_HANDLE
	uint8_t ser_port_no,
	bool status)		// Signal 4(25) RTS
						// false = RS252 MINI TESTER: green = -8 Volts
						// true =  RS252 MINI TESTER: red = +8 Volts
{
int rv ;
DCB		   dcb ;

rv = 0 ;
dcb.DCBlength = sizeof( DCB ) ;
GetCommState( fdser[ser_port_no], &dcb ) ;


//	  dcb.fDtrControl = DTR_CONTROL_ENABLE ;
dcb.fRtsControl = status ; // 0 = Off, 1 = On 2 = Auto

if ( SetCommState( fdser[ser_port_no], &dcb ) == 0 )
	{
	uprintf( "setrts(): cannot set parameters of serial interface %s",
				sername[ser_port_no] );
	rv = GetLastError();
	}

return ( rv ) ;
}



// --------------------------------------------------------------

FUNCT int setdtr(	  // _WIN31_HANDLE
	uint8_t ser_port_no,
	bool status)		// Signal 4(25) DTR
						// false = RS252 MINI TESTER: green = -8 Volts
						// true =  RS252 MINI TESTER: red = +8 Volts
{
int rv ;
DCB		   dcb ;

rv = 0 ;
dcb.DCBlength = sizeof( DCB ) ;
GetCommState( fdser[ser_port_no], &dcb ) ;


dcb.fDtrControl = status ;
// dcb.fRtsControl = status ;

if ( SetCommState( fdser[ser_port_no], &dcb ) == 0 )
	{
	uprintf( "setdtr() cannot set parameters of serial interface %s",
				sername[ser_port_no] );
	rv = GetLastError();
	}

return ( rv ) ;
}



FUNCT bool getdsr(uint8_t ser_port_no)	 // _WIN31_HANDLE
{
_COMSTAT comstat ;
DWORD comerror ;

ClearCommError( fdser[ser_port_no], &comerror, &comstat ) ;
if ( comstat.fCtsHold )
	{
	return true ;
	}
else
	{
	return false ;
	}
}


FUNCT bool getcts(uint8_t ser_port_no)	 // _WIN31_HANDLE
{
_COMSTAT comstat ;
DWORD comerror ;

ClearCommError( fdser[ser_port_no], &comerror, &comstat ) ;
if ( comstat.fCtsHold )
	{
	return true ;
	}
else
	{
	return false ;
	}
}


FUNCT bool getdcd(uint8_t ser_port_no)	 // _WIN31_HANDLE
{
_COMSTAT comstat ;
DWORD comerror ;

ClearCommError( fdser[ser_port_no], &comerror, &comstat ) ;
return true ;
}



FUNCT bool getallsent(uint8_t ser_port_no)	 // _WIN31_HANDLE
{
_COMSTAT comstat ;
DWORD comerror ;

if ( ser_port_no < MAX_INTERFACENO )
	{
	ClearCommError( fdser[ser_port_no], &comerror, &comstat ) ;
	if ( comstat.cbOutQue > 0 || comstat.fTxim )
		{
		return false ;
		}
	else
		{
		return true ;
		}
	}
return true ;
}


// --------------------------------------------------------------
// Set serial parameters MS-Windows-Handle
FUNCT int serset(uint8_t ser_port_no,	 struct sserial &definition ) // _WIN31_HANDLE
{
int rv ;

rv = 0 ;
// 1. save parameters
serdeffield[ser_port_no] = definition ;

// 2. if already opened then set new parameters
if ( seropened[ser_port_no] )
	{
	rv = sersetact( ser_port_no ) ;
	}
return rv ;
}

// --------------------------------------------------------------
FUNCT int serclose(uint8_t ser_port_no)	// _WIN31_HANDLE
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	if ( seropened[ser_port_no] )
		{
		seropened[ser_port_no] = false ;
		CloseHandle( fdser[ ser_port_no ] );
		}
	}
	return 0 ;		// no error
}

// --------------------------------------------------------------
FUNCT void serocha(uint8_t ser_port_no,char c1)  // _WIN31_HANDLE
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	if ( seropened[ser_port_no] )
		{
		// code from here is from Microsoft SAMPLES\WIN32\SDK\COMM\tty.c
		BOOL		fWriteStat ;
		DWORD		dwBytesWritten ;
		DWORD		dwErrorFlags;
		DWORD		dwError;
		COMSTAT		ComStat;

		static OVERLAPPED	overlappedinfo ;

		if ( ser_settings_field[ser_port_no].handshake	== 'h' )
			{		// with hardware handshake
			int rv ;
			DCB		   dcb ;

			rv = 0 ;
			dcb.DCBlength = sizeof( DCB ) ;
			GetCommState( fdser[ser_port_no], &dcb ) ;
			rv = 0 ;
			}

//		fWriteStat = WriteFile( fdser[ser_port_no], &c1, 1,
//						   &dwBytesWritten, &overlappedinfo ) ;

		fWriteStat = WriteFile( fdser[ser_port_no], &c1, 1,
						   &dwBytesWritten, NULL ) ;
	// Note that normally the code will not execute the following
	// because the driver caches write operations. Small I/O requests
	// (up to several thousand bytes) will normally be accepted
	// immediately and WriteFile will return true even though an
	// overlapped operation was specified

		if (!fWriteStat)
		{
			if(GetLastError() == ERROR_IO_PENDING)
			{
		// We should wait for the completion of the write operation
		// so we know if it worked or not

		// This is only one way to do this. It might be beneficial to
		// the to place the writing operation in a separate thread
		// so that blocking on completion will not negatively
		// affect the responsiveness of the UI

		// If the write takes long enough to complete, this
		// function will timeout according to the
		// CommTimeOuts.WriteTotalTimeoutConstant variable.
		// At that time we can check for errors and then wait
		// some more.

				while(!GetOverlappedResult( fdser[ser_port_no],
					&overlappedinfo, &dwBytesWritten, TRUE ))
					{
					dwError = GetLastError();
					if(dwError == ERROR_IO_INCOMPLETE)
						// normal result if not finished
						continue;
					else
						{
						// an error occurred, try to recover
						uprintf( "serial port error, dwError=%u", dwError ) ;
						ClearCommError( fdser[ser_port_no], &dwErrorFlags, &ComStat ) ;
						if ( dwErrorFlags > 0 )
							{
							uprintf( "serial port error, dwErrorFlags=%u", dwErrorFlags ) ;
							}
						break;
						}
					}
				}
			else
				{
				// some other error occurred
				ClearCommError( fdser[ser_port_no], &dwErrorFlags, &ComStat ) ;
				if ( dwErrorFlags > 0 )
					{
					uprintf( "other serial error dwErrorFlags =%XH", dwErrorFlags ) ;
					}
				return ;
				}
			}
		}
	else
		{
		uprintf( "using unopened serial port %d", ser_port_no );
		}
	}
else
	{
	uprintf( "using bad serial port %d", ser_port_no );
	}
return ;
}




#define SEROBUF
// --------------------------------------------------------------
FUNCT void serobuf(uint8_t ser_port_no,const char *p1, uint cnt)	// _WIN31_HANDLE
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	if ( seropened[ser_port_no] )
		{
		// code from here is from Microsoft SAMPLES\WIN32\SDK\COMM\tty.c
		BOOL		fWriteStat ;
		DWORD		dwBytesWritten ;
		DWORD		dwErrorFlags;
		DWORD		dwError;
		COMSTAT		ComStat;

		static OVERLAPPED	overlappedinfo ;

		if ( ser_settings_field[ser_port_no].handshake	== 'h' )
			{		// with hardware handshake
			int rv ;
			DCB		   dcb ;

			rv = 0 ;
			dcb.DCBlength = sizeof( DCB ) ;
			GetCommState( fdser[ser_port_no], &dcb ) ;
			rv = 0 ;
			}

//		fWriteStat = WriteFile( fdser[ser_port_no], &c1, 1,
//						   &dwBytesWritten, &overlappedinfo ) ;

		fWriteStat = WriteFile( fdser[ser_port_no], p1, cnt,
						   &dwBytesWritten, NULL ) ;
	// Note that normally the code will not execute the following
	// because the driver caches write operations. Small I/O requests
	// (up to several thousand bytes) will normally be accepted
	// immediately and WriteFile will return true even though an
	// overlapped operation was specified

		if (!fWriteStat)
		{
			if(GetLastError() == ERROR_IO_PENDING)
			{
		// We should wait for the completion of the write operation
		// so we know if it worked or not

		// This is only one way to do this. It might be beneficial to
		// the to place the writing operation in a separate thread
		// so that blocking on completion will not negatively
		// affect the responsiveness of the UI

		// If the write takes long enough to complete, this
		// function will timeout according to the
		// CommTimeOuts.WriteTotalTimeoutConstant variable.
		// At that time we can check for errors and then wait
		// some more.

				while(!GetOverlappedResult( fdser[ser_port_no],
					&overlappedinfo, &dwBytesWritten, TRUE ))
					{
					dwError = GetLastError();
					if(dwError == ERROR_IO_INCOMPLETE)
						// normal result if not finished
						continue;
					else
						{
						// an error occurred, try to recover
						uprintf( "serial port error, dwError=%u", dwError ) ;
						ClearCommError( fdser[ser_port_no], &dwErrorFlags, &ComStat ) ;
						if ( dwErrorFlags > 0 )
							{
							uprintf( "serial port error, dwErrorFlags=%u", dwErrorFlags ) ;
							}
						break;
						}
					}
				}
			else
				{
				// some other error occurred
				ClearCommError( fdser[ser_port_no], &dwErrorFlags, &ComStat ) ;
				if ( dwErrorFlags > 0 )
					{
					uprintf( "other serial error dwErrorFlags =%XH", dwErrorFlags ) ;
					}
				return ;
				}
			}
		}
	else
		{
		uprintf( "using unopened serial port %d", ser_port_no );
		}
	}
else
	{
	uprintf( "using bad serial port %d", ser_port_no );
	}
return ;
}


// --------------------------------------------------------------
FUNCT uint serichaw(uint8_t ser_port_no)	// _WIN31_HANDLE
{
char c1 ;

if ( ser_port_no < MAX_INTERFACENO )
	{
	DWORD		dwErrorFlags;
	COMSTAT		ComStat;
	static OVERLAPPED	overlappedinfo ;
	DWORD dwLength ;
	BOOL fReadStat ;
	DWORD dwError ;

	dwLength = 0 ;
	c1 = 0 ;
   //  ClearCommError( fdser[ser_port_no], &dwErrorFlags, &ComStat ) ;
	while ( dwLength == 0 )
		{
		if ( i_uprintf_answer == ANSWERTYPE_CANCEL )
			{
			return 0 ;
			}
		fReadStat = ReadFile(		// returns BOOL
				fdser[ser_port_no],
				&c1,			// lpBuffer
				(DWORD)1,				// NumberOfBytesToRead
				&dwLength,		// lpNumberOfBytesRead
				NULL );			// The lpOverlapped parameter is not supported

		if (!fReadStat)
			{
			if (GetLastError() == ERROR_IO_PENDING)
				{
				OutputDebugString("\n\rIO Pending");
				// We have to wait for read to complete.
				// This function will timeout according to the
				// CommTimeOuts.ReadTotalTimeoutConstant variable
				// Every time it times out, check for port errors
				while(!GetOverlappedResult( fdser[ser_port_no],
					&overlappedinfo, &dwLength, TRUE ))
					{
					dwError = GetLastError();
					if(dwError == ERROR_IO_INCOMPLETE)
						// normal result if not finished
						continue;
					else
						{
						// an error occurred, try to recover
						uprintf( "Comm read error dwError=%u", dwError ) ;
						ClearCommError( fdser[ser_port_no], &dwErrorFlags, &ComStat ) ;
						if ( dwErrorFlags > 0 )
							{
							uprintf( "Comm read error dwErrorFlags=%u", dwErrorFlags ) ;
							}
						break;
						}
					}
				}
			else
				{
				// some other error occurred

				dwLength = 0 ;
				ClearCommError( fdser[ser_port_no], &dwErrorFlags, &ComStat ) ;
				if ( dwErrorFlags > 0 )
					{
					ask_uprintf( ANSWER_ELEMENT_OKCANCEL, "serial read dwErrorFlags=%u", dwErrorFlags ) ;
					// changed extern int i_uprintf_answer
					}
				}
			}
		}
	return ( c1 & 0xFF ) ;
	}
return ~0 ;		// any error
}

// --------------------------------------------------------------
FUNCT bool serisicha(uint8_t ser_port_no) // _WIN31_HANDLE
{
bool flag_result ;

DBent("serisicha");
if ( ser_port_no < MAX_INTERFACENO )
	{
	COMSTAT	   ComStat ;
	DWORD	   dwErrorFlags;

	ClearCommError( fdser[ser_port_no], &dwErrorFlags, &ComStat ) ;
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
#if DB_INFO
	if ( ComStat.fCtsHold )	 DBprintf(( "fCtsHold: Tx waiting for CTS signal" ));
	if ( ComStat.fDsrHold )	 DBprintf(( "fDsrHold: Tx waiting for DSR signal" ));
	if ( ComStat.fRlsdHold ) DBprintf(( "fRlsdHold: Tx waiting for RLSD signal" ));
	if ( ComStat.fXoffHold ) DBprintf(( "fXoffHold: Tx waiting, XOFF char received" ));
	if ( ComStat.fXoffSent ) DBprintf(( "fXoffSent: Tx waiting, XOFF char sent" ));
	if ( ComStat.fEof )		 DBprintf(( "fEof: EOF character sent" ));
	if ( ComStat.fTxim )	 DBprintf(( "fTxim: character waiting for Tx" ));
	// DBprintf(( "cbInQue=%d, cdOutQue=%d", ComStat.cbInQue, ComStat.cbOutQue ));
#endif
	if ( ComStat.cbInQue )
		{		// at least one character for ReadFile available
		flag_result = true ;
		}
	else
		{
		flag_result = false ;
		}
	DBret("serisicha");
	return flag_result ;	 // any error
	}
DBret("serisicha");
return false ;	   // any error
}





// --------------------------------------------------------------
FUNCT void serflush(uint8_t ser_port_no,uint32_t timeout)	  // _WIN31_HANDLE
{
bool flag_timeout = false ;
uint32_t startmsec ;
int is ;
char rch ;


is = 0 ;
for(;;)
	{
	if ( serisicha( ser_port_no ) )
		{
		rch = serichaw( ser_port_no );
		flag_timeout = false ;
		}
	else
		{
		if ( flag_timeout == false )
			{
			startmsec = getmsec() ;
			flag_timeout = true ;
			}
		else
			{
			if ( diffmsec(startmsec) > timeout )
				{
				break ;
				}
			}
		}
	}
return ;
}
#endif		// _WIN31_HANDLE



// --------------------------------------------------------------------
// --------------------------------------------------------------------
#ifdef OSK

int fdser[MAX_INTERFACENO] ;
bool seropened[MAX_INTERFACENO] ;

// --------------------------------------------------------------
int seropen(uint8_t ser_port_no)		// OSK
{
int nul ;

if ( ser_port_no >= MAX_INTERFACENO )
	{
	uprintf( "ser_port_no %d not supported", ser_port_no );
	return 3 ;
	}
if ( !seropened[ser_port_no] )
		{
	fdser[ser_port_no] = normopen( sername[ser_port_no] , O_RDWR );
	if ( fdser[ser_port_no] >= 0 )
		{
		vtprintf( "opened %s\n", sername[ser_port_no] );
		DBprintf(( "opened sername[%d]={%s}", ser_port_no, sername[ser_port_no] ));
		seropened[ser_port_no] = true ;
				_gs_opt( fdser[ser_port_no] ,&bu_ser_save[ser_port_no]) ;		/* Get terminalcharacteristic */
				_gs_opt( fdser[ser_port_no] ,&bu_ser_path[ser_port_no]) ;		/* Get terminalcharacteristic */

				nul = 0 ;
				bu_ser_path[ser_port_no].sg_backsp = nul ;
				bu_ser_path[ser_port_no].sg_echo   = nul ;
				bu_ser_path[ser_port_no].sg_pause  = nul ;
				bu_ser_path[ser_port_no].sg_eorch  = nul ;
				bu_ser_path[ser_port_no].sg_eofch  = nul ;
				bu_ser_path[ser_port_no].sg_rlnch  = nul ;
				bu_ser_path[ser_port_no].sg_dulnch = nul ;
				bu_ser_path[ser_port_no].sg_psch   = nul ;
				bu_ser_path[ser_port_no].sg_bsech  = nul ;
				bu_ser_path[ser_port_no].sg_xon	   = nul ;
				bu_ser_path[ser_port_no].sg_xoff   = nul ;
				bu_ser_path[ser_port_no].sg_kbich  = nul ;		 // Keyboard interrupt character
				bu_ser_path[ser_port_no].sg_kbach  = nul ;		 // Keyboard abort character

				_ss_opt( fdser[ser_port_no] ,&bu_ser_path[ser_port_no]) ;		// For direct dialogue

				bu_ser_valid[ser_port_no] = true ;			 // may use b0save later
		DBprintf(( "called _ss_opt for ser_port_no=%d", ser_port_no ));
		bu_ser_valid[ser_port_no] = true ;
		return 0 ;		// opened succesful
		}
	uprintf( "cannot open %s for RW", sername[ser_port_no] );
	return 1 ;
	}
return 2 ;
}

// --------------------------------------------------------------
FUNCT int serclose(uint8_t ser_port_no)		// OSK
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	if ( seropened[ser_port_no] )
		{
		seropened[ser_port_no] = false ;
		normclose( fdser[ ser_port_no ] );
		}
	}
return 0 ;	// no error
}

// --------------------------------------------------------------
// Set serial parameters
FUNCT int serset(uint8_t ser_port_no,	 struct sserial &definition ) // OSK
{
int rv ;

rv = 0 ;
// but never set the parameters
return rv ;
}


// --------------------------------------------------------------
FUNCT void serocha(uint8_t ser_port_no,char c1)	   // OSK
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	normwrite( fdser[ser_port_no], &c1, 1 );
	}
}



#define SEROBUF		// OS9 can output a buffer
FUNCT void serobuf( uint8_t ser_port_no, const char *pbuf, uint maxbuf )
{
if ( ser_port_no < MAX_INTERFACENO )
	{
	normwrite( fdser[ser_port_no], pbuf, maxbuf );
	}
}

// --------------------------------------------------------------
FUNCT uint serichaw(uint8_t ser_port_no)	// OSK
{
int rv ;
char c1 ;

if ( ser_port_no < MAX_INTERFACENO )
	{
	rv = normread( fdser[ser_port_no], &c1, 1 );
	if ( rv == 1 )
		{
		// DBprintf(( "from Port %d received %XH [%c]", ser_port_no, (uint)(uint8_t)c1 & 0xFF, c1 ));
		return (uint)(uint8_t)c1 & 0xFF ;
		}
	// This happens often in OSK : Ist 244 a input buffer overflow ?
	Eprintf(( "error %d in normread from Port %d", errno, ser_port_no ));
	return ~0 ;		// any error
	}
Eprintf(( "Bad Port %d", ser_port_no ));
return ~0 ;		// any error
}

// --------------------------------------------------------------
FUNCT bool serisicha(uint8_t ser_port_no)		// OSK
{
int rv ;
if ( ser_port_no < MAX_INTERFACENO )
	{
	rv = _gs_rdy( fdser[ser_port_no] );
	if ( rv >= 1 )
		{
		return true ;
		}
	}
return false ;		// any error or no character available
}

FUNCT int setrts(uint8_t ser_port_no, bool status)	 // OSK
{
	// unimplemented UNFERTIG not ready

return 0 ;
}

FUNCT int setdtr(uint8_t ser_port_no, bool status)	 // OSK
{
	// unimplemented UNFERTIG not ready

return 0 ;
}



#define SERIBUF		// OS9 can read a buffer
FUNCT int seribuf( uint8_t ser_port_no, char *pbuf, uint maxbuf, uint *pactlen )
{
int rv ;
int rv2 ;

rv = _gs_rdy( fdser[ser_port_no] );
if ( rv >= 1 )
	{
	if ( rv >= (int)maxbuf ) rv = maxbuf ;
	*pactlen = rv ;
	rv2 = normread( fdser[ser_port_no], pbuf, rv );
	if ( rv != rv2 )
		{
		return E 2 ;	// cannot read bytes at input
		}
	return 0 ;
	}
*pactlen = 0 ;
return 0 ;
}

#endif		// OSK



#ifdef OWN_DOS	// spans very far, until endif // OWN_DOSend

#ifdef DOSGNU
/*
DOSGNU has defined outportb in <inlines/pc.h>
uint32_t portaddresse = 0 ;
uint32_t iobyte = 0 ;

inline void outportb(uint16_t port, uint8_t b)
{
iobyte = (uint32_t)b ;
portaddresse = (uint32_t)port ;

asm(  "mov _portaddresse, %dx\n" );
asm(  "mov _iobyte, %al\n" );
asm(  ".byte 0xee\n" );		// out %dx, %al\n
// DBprintf(( "sent %XH from port %XH", iobyte, portaddresse ));
}
*/
#endif		// DOSGNU


// for statistics :
int count_int ;
int old_count_int ;

// const char *cominifilename = "C:\\home\\cc\\lq.ini" ;

// I-O Offset values
#define DATA 0
#define IER 1	// Interrupt enable register
#define IIR 2
#define LCR 3
#define MCR 4
#define LSR 5
#define MSR 6
#define SPR 7

#define MAX_INFIFO 2000
#define STOP_INFIFO 100		// if below then send XOFF
#define RESTART_INFIFO 700	// if again above send XON

class Cdosser {
public:
	void InitAddr( int n );
	uint16_t IOAddr ;
	int IRQNumber ;
	uint8_t intmask ;
	uint8_t notintmask ;
	bool opened ;
	struct sserial pserial ;	// <a href=../include/heserial.h>see</a>
	class Cfifo *pinfifo ;
	class Cfifo *potfifo ;
	int error ;
	bool xoffreceived ;
	bool sendxon ;
	bool sendxoff ;
	bool sendxoffdone ;
	uint8_t Portplus4 ;
	uint8_t ier ;
	} ;



void Cdosser::InitAddr( int n )
	{
	switch ( n )
		{
	case 0:
		this->IOAddr = 0x3F8 ;
		this->IRQNumber = 4 ;
		this->intmask = 1 << 4 ;
		this->notintmask = ~this->intmask ;
		this->opened = false ;
		this->pserial.baud = 9600 ;
		this->pserial.stop = 0 ;
		this->pserial.parity = 'n' ;
		this->pserial.data = 8 ;
		this->pserial.handshake = 'n' ;
		break ;
	case 1:
		this->IOAddr = 0x2F8 ;
		this->IRQNumber = 3 ;
		this->intmask = 1 << 3 ;
		this->notintmask = ~this->intmask ;
		this->opened = false ;
		this->pserial.baud = 9600 ;
		this->pserial.stop = 0 ;
		this->pserial.parity = 'n' ;
		this->pserial.data = 8 ;
		this->pserial.handshake = 'n' ;
		break ;
	case 2:
		this->IOAddr = 0x3E8 ;
		this->IRQNumber = 4 ;
		this->intmask = 1 << 4 ;
		this->notintmask = ~this->intmask ;
		this->opened = false ;
		this->pserial.baud = 9600 ;
		this->pserial.stop = 0 ;
		this->pserial.parity = 'n' ;
		this->pserial.data = 8 ;
		this->pserial.handshake = 'n' ;
		break ;
	case 3:
		this->IOAddr = 0x2E8 ;
		this->IRQNumber = 3 ;
		this->intmask = 1 << 3 ;
		this->notintmask = ~this->intmask ;
		this->opened = false ;
		this->pserial.baud = 9600 ;
		this->pserial.stop = 0 ;
		this->pserial.parity = 'n' ;
		this->pserial.data = 8 ;
		this->pserial.handshake = 'n' ;
		break ;
		}
	}






// Bug in Borlands TURBO C does not pass argument #1 as 0x3F8 but as 'h'
class Cdosser adosser[ MAX_INTERFACENO ] ;

int ser_have_old3 = 0 ;
int ser_have_old4 = 0 ;




#if POLLING == 0

#ifdef __TURBOC__
void interrupt (far * old_vect3)(...) ; // Not polling, but interrupt drivers
void interrupt (far * old_vect4)(...) ;
#endif

#ifdef DOSGNU
_go32_dpmi_seginfo rm_old_irq3; // old COM2/COM4 interrupt driver
_go32_dpmi_seginfo rm_old_irq4; // old COM1/COM3 interrupt driver
#endif

#endif


// locally used functions
#if POLLING == 0
#ifdef __TURBOC__
// extern "C" {
// void far com_interruptdriver3(...);
// void far com_interruptdriver4();
//	} ;
typedef		void interrupt (far * intservice)(...) ;
#endif
#ifdef DOSGNU
interrupt void far com_interruptdriver3();
interrupt void far com_interruptdriver4();
#endif
extern void com_interruptdrivern(int nirq);
#endif		// Polling

void serinit(int n);


// --------------------------------------------------------<v>
#if POLLING == 0
void  interrupt far com_interruptdriver4()		// OWN_DOS && !POLLING
{
com_interruptdrivern(4);
// outportb( 0x20, 0x20 );	   // Int Ack to interrupt controller
}
#endif

// --------------------------------------------------------<v>
#if POLLING == 0
void interrupt far com_interruptdriver3()	// OWN_DOS && !POLLING
{
com_interruptdrivern(3);
// outportb( 0x20, 0x20 );	   // Int Ack to interrupt controller
}
#endif

// --------------------------------------------------------<v>
#if POLLING == 0
void com_interruptdrivern(int nirq) // OWN_DOS && !POLLING
{
int n ;
uint16_t combase ;
struct Cdosser *pdosser ;
uint8_t cb ;
uint8_t lsr ;
uint max_count ;
int spfree ;

++count_int ;
// disable();		// clear interrup flag
// check what interrupt came
pdosser = adosser ;

DBvt(( "com_interruptdrivern(%d)\n", nirq ));

for( n = 0 ; n < MAX_INTERFACENO ; ++n, ++pdosser )
	{
	DBvt(( "Check nirq=%d against %d\n", nirq, pdosser->IRQNumber ));
	if ( pdosser->IRQNumber == nirq )
		{
		combase = pdosser->IOAddr ;
		for( max_count = 37 ; --max_count ; )	// Loop max 37 times
			{									// or until nothing to do
			lsr = inportb( combase+LSR );	// Line status register
DBvt(( "lsr=%XH combase=%XH\n", lsr, combase ));
			if ( lsr == 0xff ) break ;
			if ( lsr & 0x01 )
				{							// data is recevied
				cb = inportb( combase+DATA );
DBvt(( "received data cb=%XH from Port %XH\n", cb, combase+DATA ));
				if ( cb == 0x13 )			// control - S
					{
					pdosser->xoffreceived = true ;
					continue ;				// check for more input
					}
				if ( cb == 0x11 )			// control - Q
					{
					pdosser->xoffreceived = false ;
					continue ;				// check for more input
					}
				spfree = pdosser->pinfifo->spacefree() ;
				if ( ser_settings_field[n].handshake == 'x' && pdosser->sendxoffdone == false )
					{
					if ( spfree < STOP_INFIFO )
						{
						pdosser->sendxoff = true ;
						}
					}
				if ( ser_settings_field[n].handshake == 'x' && pdosser->sendxoffdone )
					{
					if ( spfree > RESTART_INFIFO )
						{
						pdosser->sendxon = true ;
						}
					}

				if ( spfree > 0 )
					{
					pdosser->pinfifo->put(cb) ;		// save in fifo
					}
DBvt(( "stored in fifo\n" ));
				continue ;			// check if next byte also

									// available
				}
			if ( lsr & 0x20 )		// TBE 0x20 )
				{							// tmit buffer empty
				if ( pdosser->sendxoff )
					{						// special order
					outportb( combase+DATA, 0x13 );
					pdosser->sendxoff  = false ;
					pdosser->sendxon   = false ;
					pdosser->sendxoffdone = true ;
					continue ;
					}
				if ( pdosser->sendxon )
					{						// special order
					outportb( combase+DATA, 0x11 );
					pdosser->sendxon  = false ;
					pdosser->sendxoff = false ;
					pdosser->sendxoffdone = false ;
					continue ;
					}
				if ( pdosser->potfifo->filled() )
					{
					outportb( combase+DATA, pdosser->potfifo->get() );
					continue ;
					}
				// nothing to send
				outportb( combase+IER, pdosser->ier = 1 );
				break ;
				}
			break ;		// nothing to do
			}		// end loop for(;;) - check for more
		}			// end if this chip
	}
#ifdef __TURBOCDOS__
outportb( 0x20, pdosser->intmask );		// int Ack
enable();		 // set interrupt flag
#endif
}
#endif



// ------------------------------------------------------------------------
#if POLLING == 0
FUNCT bool serisicha(uint8_t n)		// OWN_DOS && !POLLING
{
struct Cdosser *pdosser ;
bool rv ;

DBent("serisicha");
if ( old_count_int != count_int )
	{
	DBprintf(( "count_int=%d", count_int ));
	old_count_int = count_int ;
	}
rv = false ;
if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
	if ( pdosser->pinfifo->empty - pdosser->pinfifo->fill )
		{
		rv = true ;
		DBprintf(( "n=%d, rv=%d empty=%d, fill=%d",
				n, rv, pdosser->pinfifo->empty, pdosser->pinfifo->fill ));
		}
	}
else
	{
	Eprintf(( "n=%d", n ));
	}
DBret("serisicha");
return rv ;
}
#endif



// ------------------------------------------------------------------------
#if POLLING == 0
FUNCT uint serichaw(uint8_t n)	// OWN_DOS && !POLLING
{
struct Cdosser *pdosser ;
uint8_t rv ;

DBent( "serichaw" );

if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
	rv = pdosser->pinfifo->get() ;	// queue data on interrupt
	DBprintf(( "n=%d, rv=%XH = %c", n, rv, rv ));
	}
else
	{
	Eprintf(( "n=%d", n ));
	rv = ~0 ;
	}
DBret( "serichaw" );
return (uint)rv ;
}
#endif



#if POLLING == 0
FUNCT void serflush( uint8_t ser_port_no, uint32_t timeout_msec )	// OWN_DOS && !POLLING
{
uint32_t starttime ;

starttime = getmsec();
while ( diffmsec(starttime) < timeout_msec )
	{
	if ( serisicha( ser_port_no ) )
		{
		serichaw( ser_port_no );
		starttime = getmsec();
		}
	}
return ;
}
#endif


// ------------------------------------------------------------------------
#if POLLING == 0
FUNCT void serocha(uint8_t n , char b )  // OWN_DOS && !POLLING
{
uint16_t combase ;
struct Cdosser *pdosser ;

DBent("serochaI");
if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
	combase = pdosser->IOAddr ;
DBprintf(( "combase=%XH, n=%d", combase, n ));




		for(;;)
			{
			if ( ( inportb( combase+LSR ) & 0x20 ) == 0 )
					continue ;	// full : wait
			// THRE Transmitter holding register empty
			break ;
			}
		outportb( combase+DATA, b );


#if 0
	disable();
	if ( pdosser->intmask & 0x02 )
		{			// Interrupt has been active
		pdosser->potfifo->put(b) ;	// queue data on interrupt
		enable();
		DBprintf(( "Byte %xH put to otfifo at %d fifo error=%d",
				b, pdosser->potfifo->fill, pdosser->potfifo->error ));
		}
	else
		{
		// output data
		for(;;)
			{
			if ( ( inportb( combase+LSR ) & 0x20 ) == 0 )
					continue ;	// full : wait
			// THRE Transmitter holding register empty
			break ;
			}
		outportb( combase+DATA, b );
		// re-enable tmit interrupt
//		outportb( combase+IER, pdosser->ier = 3 );
		enable();
		DBprintf(( "Byte %xH output direct to %XH", b, combase+DATA ));
		}
#endif
	}
else
	{
	Eprintf(( "n=%d", n ));
	}
DBret("serochaI");
}
#endif


// -----------------------------------------------------------------------
#if POLLING == 0
FUNCT int serinterrupt(int vect_num)	// OWN_DOS && !POLLING
{
int rv ;
#ifdef DOSGNU
_go32_dpmi_registers rm_regs ;
#endif

DBvt(( "serinterrupt, install handler %XH\n", vect_num ));

// install interrupt driver

#ifdef DOSGNU
_go32_dpmi_seginfo rm_si;
#endif

DBent("serinterrupt");
rv = 0 ;

switch ( vect_num )
	{
case 4+8 :
	if ( ++ser_have_old4 == 1 )
		{				// first allocation
#ifdef DOSGNU
		rv = _go32_dpmi_allocate_real_mode_callback_iret(&rm_si, &rm_regs);
		if ( rv == 0 )
			{
			disable();
			_go32_dpmi_get_real_mode_interrupt_vector(vect_num, &rm_old_irq4);
			rm_si.pm_offset = (int) com_interruptdriver4;
			_go32_dpmi_set_real_mode_interrupt_vector(vect_num, &rm_si);
			enable();
			}
		else
			{
			Eprintf(( "(4)rv = %d", rv ));
			}
#endif
#ifdef __TURBOC__
		old_vect4 = getvect( 4+8 );

		setvect( 4+8, (intservice)com_interruptdriver4 );
#endif
		}
	break ;
case 3+8 :
	if ( ++ser_have_old3 == 1 )
		{				// first allocation
#ifdef DOSGNU
		rv = _go32_dpmi_allocate_real_mode_callback_iret(&rm_si, &rm_regs);
		if ( rv == 0 )
			{
			disable();
			_go32_dpmi_get_real_mode_interrupt_vector(vect_num, &rm_old_irq4);
			rm_si.pm_offset = (int) com_interruptdriver3;
			_go32_dpmi_set_real_mode_interrupt_vector(vect_num, &rm_si);
			enable();
			}
		else
			{
			Eprintf(( "(3)rv = %d", rv ));
			}
#endif
#ifdef __TURBOC__
		old_vect3 = getvect( 3+8 );
		setvect( 3+8, (intservice)com_interruptdriver3 );
#endif
		}
	break ;

default :
	uprintf( "illegal vector number %d for INT %d", vect_num, vect_num-8 );
	rv = 4 ;			// preset some error
	break ;
	}

//	rm_si_1c.pm_offset = (int) rm_isr_1c;
//	disable();
//	_go32_dpmi_get_real_mode_interrupt_vector(0x1c, &rm_old_irq_1c);
//	_go32_dpmi_set_real_mode_interrupt_vector(0x1c, &rm_si_1c);
//	enable();
DBret("serinterrupt");
return rv ;
}
#endif		// POLLING == 0


FUNCT int setrts( uint8_t n, bool state ) // OWN_DOS
{
class Cdosser *pdosser ;
uint16_t combase ;
uint8_t mcr ;

if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
//	pdosser->bRTS = state ;
	combase = pdosser->IOAddr ;
	mcr = inportb( combase+MCR );
	if ( state )
		{
		mcr |= 0x02 ;	// RTS
		}
	else
		{
		mcr &= ~0x02 ;	// RTS
		}
	outportb( combase+MCR, mcr ); // xf9   / interrupt enable register IER
	return 0 ;
	}
return 1 ;	// somehow an error
}

FUNCT int setdtr( uint8_t n, bool state ) // OWN_DOS POLLING 0 or 1
{
class Cdosser *pdosser ;
uint16_t combase ;
uint8_t mcr ;

if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
//	pdosser->bRTS = state ;
	combase = pdosser->IOAddr ;
	mcr = inportb( combase+MCR );
	if ( state )
		{
		mcr |= 0x01 ;	// DTR
		}
	else
		{
		mcr &= ~0x01 ;	// DTR
		}
	outportb( combase+MCR, mcr ); // xf9   / interrupt enable register IER
	return 0 ;
	}
return 1 ;	// somehow an error
}





FUNCT bool getdsr(uint8_t n)	   // OWN_DOS && POLLING
{
class Cdosser *pdosser ;
uint16_t combase ;
uint8_t msr ;

if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
//	pdosser->bRTS = state ;
	combase = pdosser->IOAddr ;
	msr = inportb( combase+MSR );		// Get Line Status Register

	if ( msr & 0x20 )
		{
		return true ;
		}
	else
		{
		return false ;
		}
	}
else
	{
	return false ;
	}
}



FUNCT bool getcts(uint8_t n)		 // OWN_DOS && POLLING
{
class Cdosser *pdosser ;
uint16_t combase ;
uint8_t msr ;

if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
//	pdosser->bRTS = state ;
	combase = pdosser->IOAddr ;
	msr = inportb( combase+MSR );		// Get Line Status Register

	if ( msr & 0x10 )
		{
		return true ;
		}
	else
		{
		return false ;
		}
	}
else
	{
	return false ;
	}
}



FUNCT bool getdcd(uint8_t n)		 // OWN_DOS && POLLING
{
class Cdosser *pdosser ;
uint16_t combase ;
uint8_t msr ;

if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
//	pdosser->bRTS = state ;
	combase = pdosser->IOAddr ;
	msr = inportb( combase+MSR );		// Get Line Status Register

	if ( msr & 0x80 )	// DCD = 80 , Delta DCD = 08
		{
		return true ;
		}
	else
		{
		return false ;
		}
	}
else
	{
	return false ;
	}
}





FUNCT bool getallsent( uint8_t n )	// OWN_DOS
{
n = n ;
return true ;
}

// -----------------------------------------------------------------------
FUNCT void serinit(uint8_t n)		   // OWN_DOS
{
uint16_t combase ;
uint16_t divisor ;
class Cdosser *pdosser ;
uint8_t lcr ;

DBent("serinit");
DBprintf(( "n=%d", n ));
if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
	pdosser->InitAddr(n);

	DBprintf(( "baud=%d, data=%d, parity=%c",
				pdosser->pserial.baud,
				pdosser->pserial.data,
				pdosser->pserial.parity ));
	if ( pdosser->pserial.baud < 4 ) pdosser->pserial.baud = 4 ;
	divisor = (uint16_t)(115200L / pdosser->pserial.baud) ;
	pdosser->pserial.baud = 115200L / (uint32_t)divisor ;
DBvt(( "baud=%lu, divisor=%u\n", pdosser->pserial.baud, divisor ));

	disable();
	combase = pdosser->IOAddr ;
	outportb( combase+IER, pdosser->ier = 0 ); // xf9	/ interrupt enable register IER
								//	Bit 1: Tx Int , Bit0:rec. Int.
								// disable all interrupts of this port


	outportb( combase+LCR, 0x80 );	// Divisor latch bit address
	outportb( combase+DATA+1, divisor>>8 );
	outportb( combase+DATA+0, divisor );

	lcr = 0 ;
	if ( pdosser->pserial.data >= 5 || pdosser->pserial.data <= 8 )
		{
		lcr |= pdosser->pserial.data - 5 ;
		}
	else
		{
		Eprintf(( "n=%d, pdosser->pserial.data=%d", n, pdosser->pserial.data ));
		lcr |= 3 ;			// set 8 bits
		}
	switch ( pdosser->pserial.parity )
		{
	case 'n' : break ;
	case 'e' : lcr |= 0x08 ; break ;
	case 'o' : lcr |= 0x18 ; break ;
	default : break ;
		}

	outportb( combase+LCR, lcr );
	//				  1 0  00 = 5 Databits
	//				  1 0  01 = 6 Databits
	//				  1 0  10 = 7 Databits
	//				  1 0  11 = 8 Databits
	//				2	   1 oder 1.5 Stoppbits
	//			  3		   1 = Parity enable
	//			4		   1 = parity odd
	//		  5			   Stick parity
	//		6			   Set Break
	//	  7				   Divisor Latch Acces Bit
	outportb( combase+IER, pdosser->ier = 0 );
//	outportb( combase+IER, pdosser->ier = 1 );
	outportb( combase+MCR, 8 ); // Interrupt tristate buffers on
	// prepare memory
	pdosser->pinfifo->fill = 0 ;
	pdosser->pinfifo->empty = 0 ;
	pdosser->potfifo->fill = 0 ;
	pdosser->potfifo->empty = 0 ;
	pdosser->error = 0 ;					// No error number
	pdosser->xoffreceived  = false ;			// No x-off came
	pdosser->sendxon = true ;				// request sending of XON
	pdosser->sendxoffdone = true ;			// XOFF sent, do not send again
	// disable 8259
	outportb( 0x21, inportb( 0x21 ) | pdosser->intmask );

//	outportb( combase+IER, pdosser->ier = 0 );
#if POLLING == 0
	outportb( combase+IER, pdosser->ier = 1 );
										// enable recv int & tx(2) Int (1)
	// enable 8259
	outportb( 0x21, inportb( 0x21 ) & pdosser->notintmask );
#endif
	pdosser->Portplus4 = inportb( combase+MCR );
	enable();

	DBvt(( "init intmask=%XH notintmask=%XH\n", pdosser->intmask, pdosser->notintmask ));

	DBvt(( "port 21 = %XH", inportb(0x21) ));
	}
else
	{
	Eprintf(( "bad n=%d", n ));
	}
DBret("serinit");
return ;
}



// ------------------------------------------------------------------
FUNCT int seropen(uint8_t n)			// OWN_DOS && POLLING
{
char fnbuffer[6] ;
uint32_t baud ;
struct Cdosser *pdosser ;

DBent("seropen");
DBprintf(( "n=%d", n ));
if ( n >= MAX_INTERFACENO )
	{
	uprintf( "serial.cpp seropen() n=%d", n );
	DBret("seropen");
	return 20001 ;	// error
	}


sprintf( fnbuffer, "COM%d", n+1 );

pdosser = &adosser[n] ;
pdosser->InitAddr(n);
if ( pdosser->IOAddr == 0 )
	{
	uprintf( "serial.cpp seropen() n=%d", n );
	DBret("seropen");
	return 20002 ;	// error
	}

pdosser->pinfifo = new Cfifo( MAX_INFIFO );
if ( pdosser->pinfifo == NULL )
	{
	Eprintf(( "pdosser->pinfifo" ));
	DBret("seropen");
	return 20002 ;	// error
	}
pdosser->potfifo = new Cfifo(64);
if ( pdosser->pinfifo == NULL )
	{
	Eprintf(( "pdosser->pinfifo" ));
	DBret("seropen");
	return 20003 ;	// error
	}

DBprintf(( "pdosser->IOAddr=%XH, IRQNumber=%d, intmask=%XH, notintmask=%XH",
			pdosser->IOAddr, pdosser->IRQNumber, pdosser->intmask,
			pdosser->notintmask ));
DBprintf(( "will overwrite old pdosser->pserial.data=%d, baud=%d, parity=%c",
			pdosser->pserial.data,
			pdosser->pserial.baud,
			pdosser->pserial.parity ));
/*
pdosser->pserial.data = 8 ;
pdosser->pserial.baud = 9600 ;
pdosser->pserial.parity = 'n' ;
*/
if ( pdosser->pserial.data < 5 || pdosser->pserial.data > 8 )
	{
	uprintf( "bad data length data=%d, should be 5..8",
				pdosser->pserial.data );
	DBret("seropen");
	return 20006 ;
	}
switch ( pdosser->pserial.parity )
	{
case 'n' : break ;
case 'e' : break ;
case 'o' : break ;
default :
	uprintf( "bad parity definition par=<%c> should be p e or o",
				pdosser->pserial.parity );
	DBret("seropen");
	return 20008 ;
	}

#if POLLING == 0
if ( serinterrupt( pdosser->IRQNumber + 8 ) )
	{			// cannot install interrupt handler
	Eprintf(( "cannot install interrupt handler" ));
	DBret("seropen");
	return 2 ;	// error
	}
#endif
serinit(n);
DBret("seropen");
return 0 ;
}


// --------------------------------------------------------<v>
// public function : Set serial parameters to serial port n
FUNCT int serparam(uint8_t n, struct sserial &setparam)	// OWN_DOS && POLLING
{
struct Cdosser *pdosser ;
int rv ;

DBent("serparam");
rv = 0 ;			// Assume no error

if ( n >= MAX_INTERFACENO )
	{
	rv = 1 ;		// BAD INTERFACE NUMBER, OUT OF RANGE
	Eprintf(( "n=%d, MAX_INTERFACENO=%d", n, MAX_INTERFACENO ));
	}
else
	{
	pdosser = &adosser[n] ;
	pdosser->pserial = setparam ;
	serinit(n);
	}
DBret("serparam");
return rv ;
}



// --------------------------------------------------------<v>
FUNCT int serclose(uint8_t n )	// OWN_DOS
{
struct Cdosser *pdosser ;
int vect_num ;

DBent("serclose");
if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
	// disable 8259
	outportb( 0x21, inportb( 0x21 ) | pdosser->intmask );

#if POLLING == 0
	vect_num = pdosser->IRQNumber+8 ;
	switch ( vect_num )
		{
	case 4+8 :
		if ( --ser_have_old4 != 0 )
			{				// not last deallocation
			break ;
			}
#ifdef DOSGNU
		_go32_dpmi_set_real_mode_interrupt_vector(vect_num, &rm_old_irq4);
#endif
#ifdef __TURBOC__
		setvect( 4+8, old_vect4 );
#endif
		break ;
	case 3+8 :
		if ( ++ser_have_old3 != 1 )
			{				// not last deallocation
			break ;
			}
#ifdef DOSGNU
		_go32_dpmi_set_real_mode_interrupt_vector(vect_num, &rm_old_irq3);
#endif
#ifdef __TURBOC__
		setvect( 3+8, old_vect3 );
#endif
		break ;
	default : uprintf( "illegal vector number %d for INT %d", vect_num, vect_num-8 );
		break ;
		}
#endif

	}
DBret("serclose");
return 0 ;
}

// --------------------------------------------------------<v>
#if POLLING == 1
FUNCT bool serisicha(uint8_t n)	// OWN_DOS && POLLING
{
uint16_t combase ;
struct Cdosser *pdosser ;
bool rv ;
uint8_t ib ;

DBent("serisichaP");
rv = false ;
if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
	combase = pdosser->IOAddr ;
	ib = inportb( combase+LSR );	   // Line status register
	if ( ib & 1 )					   // data ready
		{
		rv = true ;
		DBprintf(( "n=%d, rv=%d", n, rv ));
		}
	}
else
	{
	Eprintf(( "n=%d", n ));
	}
DBret("serisichaP");
return rv ;
}
#endif




// ------------------------------------------------------------------------
#if POLLING == 1
FUNCT uint serichaw(uint8_t n)	// OWN_DOS && POLLING
{
uint16_t combase ;
struct Cdosser *pdosser ;
uint rv ;
uint8_t ib ;

DBent( "serichawP" );

if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
	combase = pdosser->IOAddr ;
	for(;;)
		{
		ib = inportb( combase+LSR );		// Line status register
		if ( ib & 0x01 ) break ;			// DR Data Ready
		}
	rv = inportb( combase+DATA );	// Get data byte
	DBprintf(( "n=%d, rv=%XH = %c", n, rv, rv ));
	}
else
	{
	Eprintf(( "n=%d", n ));
	rv = ~0 ;
	}
DBret( "serichawP" );
return rv ;
}
#endif



#if POLLING == 1
FUNCT void serflush( uint8_t ser_port_no, uint32_t timeout_msec )	// OWN_DOS && POLLING
{
uint32_t starttime ;

starttime = getmsec();
while ( diffmsec(starttime) < timeout_msec )
	{
	if ( serisicha( ser_port_no ) )
		{
		serichaw( ser_port_no );
		starttime = getmsec();
		}
	}
return ;
}
#endif

// ------------------------------------------------------------------------
#if POLLING == 1
FUNCT void serocha(uint8_t n , char b )  // OWN_DOS && POLLING
{
uint16_t combase ;
struct Cdosser *pdosser ;
uint8_t ib ;

DBent("serochaP");
if ( n < MAX_INTERFACENO )
	{
	pdosser = &adosser[n] ;
	combase = pdosser->IOAddr ;
	// output data
	for(;;)
		{
		ib = inportb( combase+LSR );		// Line status register
		if ( ib & 0x20 ) break ;			// THRE Transmitter holding register empty
		}
	outportb( combase+DATA, b );
	DBprintf(( "Byte %xH output direct to %XH", b, combase+DATA ));
	}
else
	{
	Eprintf(( "n=%d", n ));
	}
DBret("serochaP");
}
#endif



// --------------------------------------------------------------
// Set serial parameters from external definition into internal buffer
// long definition.baud Baudrate 50L, 300L, ... 115200L
// uint8_t .stop		   0 or 1
// char .parity		 'n', 'e' or 'o'
// uint8_t .data		  5,   6,	7, or 8
// uint8_t .handshake	  'n', 'x' or 'h'
FUNCT int serset(uint8_t ser_port_no,	 struct sserial &definition ) // OWN_DOS
{
int rv ;

rv = 0 ;		// no error
if ( ser_port_no < MAX_INTERFACENO )
	{
	adosser[ser_port_no].pserial = definition ; // baud, stop, parity,
												// data, handshake
	}
else
	{
	rv = 2 ;
	}
// but never set the parameters
return rv ;
}
#endif		//	OWN_DOS




#ifndef SERIBUF		// not yet compiled a seribuf function

FUNCT int seribuf( uint8_t ser_port_no, char *pbuf, uint maxbuf, uint *pactlen )
// read as much as is at input
{
uint ibuf = 0 ;
uint ic ;


for(;;)
	{
	if ( ibuf >= maxbuf )
		{
		break ;
		}
	if ( serisicha( ser_port_no ) )
		{
		ic = serichaw( ser_port_no );
		if ( ic == ~0 )
			{
			*pactlen = ibuf ;
			return 1 ;		// error detected
			}
		pbuf[ibuf++] = ic ;
		}
	else
		{	// no more bytes at input
		break ;
		}
	}
*pactlen = ibuf ;
return 0 ;		// no error detected
}
#endif	// SERIBUF		// not yet compiled a seribuf function



#ifndef SEROBUF		// not yet compiled a serobuf function
					// This should only be compiled for OS
					// which do not support output of
					// a buffer to serial line
FUNCT void serobuf( uint8_t ser_port_no, const char *pbuf, uint maxbuf )
// read as much as is at input
{
uint ibuf ;

for( ibuf = 0 ; ibuf < maxbuf ; ++ibuf )
	{
	serocha( ser_port_no , pbuf[ibuf] );
	}

return ;
}
#endif	// SEROBUF		// not yet compiled a seribuf function


#ifndef SERPUTS		// not yet compiled a serobuf function
					// This should only be compiled for OS
					// which do not support output of
					// a buffer to serial line
FUNCT void serputs( uint8_t ser_port_no, const char *pch )
// read as much as is at input
{
uint ibuf ;

for( ibuf = 0 ; pch[ibuf] ; ++ibuf )
	{
	serocha( ser_port_no , pch[ibuf] );
	}

return ;
}
#endif	// SERPUTS		// not yet compiled a seribuf function
