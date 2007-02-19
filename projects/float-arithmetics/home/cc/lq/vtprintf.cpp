#define DB_ENABLED 0
// vtprintf.cc

#include <hedeb.h>
#define DEF_VTPRINTF
#include <heport.h>
#include <helq.h>
#include <hevt.h>

extern "C" {

FUNCT void vtprintf( const char *fmt, char * p1,char * p2,char * p3,char * p4,char * p5,char * p6,char * p7,char * p8,char * p9 )
{
#if INTSIZE <= 16
static char buffer[800] ;	// Small buffer
#else
static char buffer[8000] ;	// Big buffer
#endif

DBent("vtprintf");
nsprintf( buffer, sizeof(buffer), fmt, p1, p2, p3, p4, p5, p6, p7, p8, p9 );
vtputs( buffer );
DBret("vtprintf");
return ;
}
		}		// end extern "C"
