// debugging
#ifndef __hedeb_h__
#define __hedeb_h__
#define NDBNEST 25			// Allowed nesting of subroutines
#define NDBNAMES 80			// max length of names (includeing 0 byte)

#if DB_ENABLED



extern char *ptxLastFormat ;	// What message has been displayed last recently

#define DBmain(x)	dbmain(x)	//
#define DBexit()   dbexit()		//
#define Eprintf(x)	deprintf x	//
#define Euprintf(x)	 { deprintf x ; uprintf x ; } //
#define Wprintf(x)	dwprintf x	//
#define DBmsec()	dbmsec()	 //
#define DBent(x)	dbent(x)
#define DBret(x)	dbret(x)
#define DBflush()	dbflush()

#if DB_INFO
#define DBprintf(x) dbprintf x	//
#define RAWprintf(x) rawprintf x	//
#else					// not DB_INFO, but DB_ENABLED
#define DBprintf(x)				//
#define RAWprintf(x)				//
#endif					// DB_INFO

#else	// not DB_ENABLED - do nothing at all
#define DBmain(x) //
#define DBprintf(x) //
#define RAWprintf(x) //
#define Wprintf(x) //
#define DBmsec() //
#define Eprintf(x) //
#define Euprintf(x)	 uprintf x //
#define DBent(x) //
#define DBret(x) //
#define DBexit() //
#define DBflush()
#endif	// end if DB_ENABLED

extern void dbmain( const char *name );
extern void dbexit( void );
extern void dbmsec( void );
extern void dbreopen( void );
extern void dbflush( void );
extern void dbprident( void );	// print identification of subroutines nested
extern void dbent( const char *idstring );
extern void dbret( const char *idstring );
#ifndef DEF_DEBPRINTF  // tricky : I do not know how to write printf in cplusplus
extern "C" {
void rawprintf( const char *fmt, ... );
void dbprintf( const char *fmt, ... );
void deprintf( const char *fmt, ... );
void dwprintf( const char *fmt, ... );
}
#endif

#endif // __hedeb_h__
