// Aus J. Herberts Lib LQ

#define DB_INFO 0
#define DB_ENABLED 0

#include <heport.h>
#include <hedeb.h>
#include <hegettim.h>
#include <helq.h>
#define PRINTF
#include <helog.h>


#include <stdio.h>

FILE *fp_logging ;
bool bLogerrors_occured ;
static bool bReportNew = false ;
static char txLogFileName[NAME_MAX] =
#if FSYSTEM == FSYSTEM_DOS_WIN
		"C:\\temp\\xx.txt" ;
#endif
#if FSYSTEM == FSYSTEM_OSK
		"/h0/TEMP/xx.txt" ;
#endif
#if FSYSTEM == FSYSTEM_LINUX
		"/tmp/xx.txt" ;
#endif



// prepare logging file, no file name given
void logopen(const char *name)	// LOGFILE
{
txtcpy( txLogFileName,	sizeof(txLogFileName), name );
logopen();
}


// prepare logging file
void logopen()	// FILELOG, anonyn
{
struct sdate xdate;
struct sday xday ;

DBent("logopen");



if ( fp_logging == NULL )
	{		// open logging file only exactly once
	fp_logging = fopen( txLogFileName, "a" );
	if ( fp_logging != NULL )
		{
		if ( bReportNew == false )
			{
			bReportNew = true ;
			getlocaldatetime( &xdate, &xday );
			fprintf( fp_logging,  "\n//********************************************************************\n") ;
			fprintf( fp_logging,  "// new logging %4u-%2u-%2u %2u:%2u:%2u begins here \n",
									xdate.year, xdate.month, xdate.day,
									xday.hour, xday.minute, xday.second );
			fprintf( fp_logging,  "//********************************************************************\n") ;
			}
		DBprintf(( "Log file %v opened", txLogFileName ));
		}
	else
		{
		uprintf( "cannot open log file %v", txLogFileName );
		}
	}
DBret("logopen");
}


void logflush()
{
if ( fp_logging != NULL )
	{
	fflush( fp_logging );
	}
}


void logclose()							// Close logging file
{
if ( fp_logging != NULL )
	{
	fclose(fp_logging);
	fp_logging = NULL ;
	}
DBprintf(( "log file closed" ));
}


FUNCT void logocha(char x)
{
if ( fp_logging != NULL )
	{
	fputc( x, fp_logging );
	}
}





FUNCT void loghex2( uint8_t value )
{
static char hexlookuptable[16+1] = "0123456789ABCDEF" ;
char xc ;

if ( fp_logging != NULL )
	{
	xc = hexlookuptable[ (value>>4) & 0x0f] ;
	fputc( xc, fp_logging );
	xc = hexlookuptable[ (value	  ) & 0x0f] ;
	fputc( xc, fp_logging );
	}
}





extern "C" {
FUNCT void logprintf(const char *fmt, int x1, int x2, int x3, int x4, int x5,
					 int x6, int x7, int x8, int x9 )
{
DBent("logprintf");
char buffer[160] ;

if ( fp_logging != NULL )
	{
	nsprintf( buffer, sizeof(buffer), fmt, x1,x2,x3,x4,x5,x6,x7,x8,x9 );
	fputs( buffer, fp_logging );
	}
DBprintf(( fmt, x1,x2,x3,x4,x5,x6,x7,x8,x9 ));
DBret("logprintf");
}


FUNCT void errprintf(const char *fmt, int x1, int x2,int x3,int x4,
				int x5,int x6,int x7,int x8,int x9 )
{
DBent("logprintf");
char buffer[160] ;

bLogerrors_occured = true ;
if ( fp_logging != NULL )
	{
	fprintf( fp_logging, "ERROR" );
	nsprintf( buffer, sizeof(buffer), fmt, x1,x2,x3,x4,x5,x6,x7,x8,x9 );	// can handle %v
	fputs( buffer, fp_logging );
	}
Eprintf(( fmt, x1,x2,x3,x4,x5,x6,x7,x8,x9 ));
DBret("logprintf");
}

}		// end extern "C"

