
#if !defined AVR
#define NPRGPATH 8	// Number of program pathes
#else
// #define NPRGPATH 4	// Number of program pathes in makefile / avr/ccs.bat file
#endif


#ifdef AVR
FUNCT void tseinri_P(			// add a function to the list of functions to run
					int index,
					void (*funaddr)(),
					void *stackbase,
					int stacksize);
#define tseinri(i,f,p,s,z) tseinri_P(i,f,s,z)
#else
FUNCT int tseinri(			// add a function to the list of functions to run
					int index,
					void (*funaddr)(),
					char *pname,
					void *stackbase,
					int stacksize);
#endif



#ifdef AVR
FUNCT void tsnext_P( void );	// Debug info: a fixed string
#define tsnext(x) tsnext_P()
#else
FUNCT void tsnext( const char *where); // Debug info: a fixed string
#endif


FUNCT int tsgetlin(		// Read a line from the console (vt)
	char *pb,
	int max_len );

FUNCT void tsopen(void);		// clear all tables

extern const char *tsnext_p[NPRGPATH] ;
extern const char *proces_p[NPRGPATH] ;
