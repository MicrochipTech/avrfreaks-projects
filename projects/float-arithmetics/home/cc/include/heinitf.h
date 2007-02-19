// heinitf : Initialisierungsdatei lesen
#ifndef __heinitf_h__
#define __heinitf_h__

#include <stdio.h>
#include <string.h>		// strcpy()
#include <cfifo.h>

#define NINITFBUFFER 300

class Cinitf {
private:
	tfh	 fd ;
//	  bool writeable ;
	char initfbuffer[NINITFBUFFER] ;
	char initf_filename[NAME_MAX] ;
	Cfifo	initfile_fifo ;		// The fifo needs initalisation
//	  long fpos ;
	char	actual_section[200] ;
	long fpos_of_actual_section ;
	uint32_t lineno ;
	uint32_t maxlineno ;

public:
	Cinitf(void):initfile_fifo(1024) {
		strcpy( initf_filename, "kein_initfile")  ;
		actual_section[0] = 0 ;
		lineno = 0 ;
		fd = -1 ;
//		writeable=false ;
		}
				// Create control structure
	int	   initfopen(const char *filename);	   // open that init-File
	int	   initfmopen(const char *filename);	// open that init-File for Modification

	Cinitf( const char *filename):initfile_fifo(1024)		// constructor
		{
		lineno = 0 ;
		actual_section[0] = 0 ;
		fd = -1 ;
//				  writeable=false ;
		initfopen(filename);		// and open for read
		}

	~Cinitf()							// destructor
		{
		if ( fd >= 0 )
			{		// not yet closed
			normclose(fd) ; fd = -1 ;
			}
		}

	int		initfclose()
		{
		if ( fd >= 0 )
			{		// not yet closed
			normclose(fd) ; fd = -1 ;
			}
		return 0 ;
		}

	int	   initfgetbool(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				bool *dest);		   // where to store the gotten number (0 or 1)

	int	   initfgetBOOL(				// Old Microsoft Windows variable type alias int
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				BOOL *dest);		   // where to store the gotten number (0 or 1)

	int	   initfgetuchar(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				uint8_t *dest);			// where to store the gotten number ( 0 ..255 )

	int	   initfgetschar(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				int8_t *dest);			// where to store the gotten number ( -128..127)

	int	   initfgetuword(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				uint16_t *dest);			// where to store the gotten number ( 0 .. 65535 )

	int	   initfgetsword(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				int16_t *dest);			// where to store the gotten number ( -32768 .. 32767 )

	int	   initfgetsword(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				int16_t *dest ,			// where to store the gotten number ( -32768 .. 32767 )
				int16_t minval ,			// lower limit
				int16_t maxval );			// Upper limit

	int	   initfgetuint(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				uint *dest);			// where to store the gotten number

	int	   initfgetuint(		// with error report
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				uint *dest,				// where to store the gotten string
				uint minval,
				uint maxval );

	int	   initfgetsint(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				sint *dest);		 // where to store the gotten string

	int	   initfgetsint(		// with error report
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				sint *dest,				// where to store the gotten string
				sint minval,
				sint maxval );

	int	   initfgetulong(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				uint32_t *dest);			// where to store the gotten string

	int	   initfgetulong(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				uint32_t *dest ,			// where to store the gotten string
				uint32_t minval ,
				uint32_t maxval );

	int	   initfgetulonghex(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				uint32_t *dest);			// where to store the gotten string

	int	   initfgetslong(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				int32_t *dest);		 // where to store the gotten string

	int	   initfgetdouble(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				double *dest);		 // where to store the gotten string

	int	   initfgetdouble(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				double *dest,		// where to store the gotten string
				double minvalue,
				double maxvalue );

	int	   initfgetstring(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				char *destbuffer,		// where to store the gotten string
				int sizeofdest);		// space for string (80 or more)

	int	   initfgetvstring(			// opposite to nsprintf( "var=%v", "\t\r\l\\abc[]" );
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				char *destbuffer,		// where to store the gotten string
				int sizeofdest);		// space for string (80 or more)

	int	   initfgettchar(
				const char *section,	// the [section] name
				const char *entry ,		// The entry= name
				char *dest);			// where to store the gotten char

	int	   initfsection_start(		// look for first entry of this section
				const char *section );	// the [section] name
	int	   initfsection_check(		// if we are currently not in this section
									// then look for first entry of this section
				const char *section );	// the [section] name
	int	   initfgetnextentry(
				char *entry ,			// The entry= name
				int sizeofentry,		// space for string (80 or more)
				char *destbuffer,		// where to store the gotten string
				int sizeofdest);		// space for string (80 or more)
	bool initfisopened( void )
		{
		return fd >= 0 ? true : false ;
		}
	char *errtxt( int err_no );
	} ;

#define ERR_INITF_NOT_OPENED 1
#define ERR_INITF_SECTIONFORMAT 2
#define ERR_INITF_ENTRYNOTFOUND 3
#define ERR_INITF_SECTIONNOTFOUND 4
#define ERR_INITF_ENTRYTOOLONG 5
#define ERR_INITF_AT_EOF 6
#define ERR_INITF_NAMETOOLONG 7
#define ERR_INITF_ENTRY_MISSPELLED 8
// Hint : change  in file initf.cpp the function Cinitf::errtxt() as well
// when adding numbers here
#define		MAX_ERR_INITF 9

#endif // __heinitf_h__
