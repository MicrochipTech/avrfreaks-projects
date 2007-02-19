#ifndef __helog_h__
#define __helog_h__
#ifndef __heport_h__
#include <heport.h>
#endif

#ifndef __helq_h__
#include <helq.h>
#endif

#include <stdio.h>

// extern bool log_opened ;
extern FILE *fp_logging ;
extern bool log_enabled ;
extern bool bLogerrors_occured ;

void	logopen( void );		// start logging with default logging file
void	logopen( char const *filename );

void	logflush( void );


void	logclose( void );
void	logocha( char character_to_save_on_disk );

void	logmsec( void );
void	logputs( char *p );

extern void loghex2( uint8_t x );

#ifndef PRINTF					// don't make prototypes
extern "C" {
void logprintf(char const *fmt, ... );
void logmsprintf(char const *fmt, ... );
void errprintf(char const *fmt, ... );
}

#endif	// PRINTF
#endif	// __helog_h__
