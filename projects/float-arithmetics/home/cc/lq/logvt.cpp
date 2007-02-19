#define DB_INFO 0
#define DB_ENABLED 0

#include <heport.h>
#define DEF_LOGVTPRINTF
#include <helq.h>
#include <hedeb.h>
#include <helog.h>
#include <hevt.h>

;

bool log_enabled = true ;



FUNCT void logvtochavisible( uint8_t cDisplay )
{
if ( cDisplay >= 0x20 && cDisplay < 0x7f )
	{				// display text characters
	logvtocha( cDisplay );
//	  atlinepos1 = false ;
	}
else
	{
	if ( cDisplay == 0x0d || cDisplay == 0x0a || cDisplay == 0x02 )
		{
//		  newlineifnot();
		}
	else
		{
//		  atlinepos1 = false ;
		}
	switch( cDisplay )
		{
	case 0x1b : logvtputs("[ESC]"); break ;
	case 0x7f : logvtputs("[DEL]"); break ;
	case 0x01 : logvtputs("[SOH]"); break ;
	case 0x02 : logvtputs("[STX]"); break ;
	case 0x03 : logvtputs("[ETX]"); break ;
	case 0x08 : logvtputs("[BS]"); break ;
	case 0x0c : logvtputs("[FF]"); break ;
	case 0x0d : logvtputs("[CR]"); break ;
	/*		case 0x20 : logvtputs("[SP]"); break ;	*/
	case 0x0a : logvtputs("[LF]");
			break ;
	case 0x11 : logvtputs("[XON]"); break ;
	case 0x13 : logvtputs("[XOFF]"); break ;
	case 0x09 : logvtputs("[TAB]"); break ;
	default:	logvtocha('[');
				logvthex2(cDisplay);
				logvtocha('H');
				if ( cDisplay >= 0xA0 && cDisplay < 0xFF )
					{
					logvtocha( cDisplay & 0x7f );
					}
				logvtocha(']');
				break ;
		}
	}
}

FUNCT void logvthex8( uint32_t value )
{
logvthex4( (uint16_t)(value >> 16) );
logvthex4( (uint16_t)(value) );
}

FUNCT void logvthex4( uint16_t value )
{
logvthex2( (uint8_t) (value >> 8) );
logvthex2( (uint8_t) (value) );
}


FUNCT void logvthex2( uint8_t value )
{
static char hexlookuptable[16+1] = "0123456789ABCDEF" ;
uint8_t xc ;

xc = hexlookuptable[ (value>>4) & 0x0f] ;
vtocha(xc);
if ( log_enabled ) logocha(xc);
xc = hexlookuptable[ (value	  ) & 0x0f] ;
vtocha(xc);
if ( log_enabled ) logocha(xc);
}

FUNCT void logvtocha( char xc )
{
vtocha(xc);
if ( log_enabled ) logocha(xc);
}


FUNCT void logvtputs( const char *pch )
{
const char *pchr ;

pchr = pch ;
while ( *pchr )
	{
	vtocha( *pchr++ );
	}

if ( log_enabled )
	{
	pchr = pch ;
	while ( *pchr )
		{
		logocha( *pchr++ );
		}
	}
}


extern "C" {
FUNCT void logvtprintf(const char *fmt, int x1, int x2, int x3, int x4, int x5,
					 int x6, int x7, int x8, int x9 )
{
extern void vtprintf( const char *fmt,...);
char buffer[512] ;


DBent("logvtprintf");

nsprintf( buffer, sizeof(buffer) , fmt, x1,x2,x3,x4,x5,x6,x7,x8,x9 );
logvtputs( buffer );
DBret("logvtprintf");
}

}	// end extern "C"
