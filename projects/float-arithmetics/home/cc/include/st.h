#ifndef __ST_H__
#define __ST_H__

#include <hegettim.h>


enum e_move_file_order {
	OT_FILE ,		// a single file to file move
	OT_DIRECTORY ,	// a directory to directory move
	OT_FILEDIR ,	// a single file to directory move
	} ;

// a single element of chain - list of files to be moved
struct s_move_file_order {			// a single order instruction
	e_move_file_order ordertype ;
	char *source ;			// where from to move
	char *dest ;			// where to to move
	struct s_move_file_order *pnext ;	// next order
				} ;



// This size limited strings are used to hold a line which is under
// construction and where more and more text is printed into

class Cst
{		// String with limit
private:
	char *baseptr ;		// Start of buffer : Start()
	uint basemaxsize ;
	char *ptr ;			// The actual string pointer
	uint maxsize ;			// the rest until buffer end
	void Init( uint iMax ); // common initialisation for Cst(int) and Cst()
public:
	// Member functions
	Cst( uint iMax );		// creation
	Cst( void );			// creation
	~Cst( void );			// destruction
	void Destroy( void );	// destruction
	void New( uint iMax );	// Make new with other size
	void rewind(void) ;		// back to start, make reusable

	inline char *Start( void ) const
		{					// Get Start Address of the content string
		return baseptr ;
		}
	inline char *End( void ) const
		{
		return ptr ;
		}
	inline uint GetRest(void) const
		{
		return this->maxsize ;
		}
	inline uint Total(void) const
		{
		return this->basemaxsize ;
		}

	inline uint GetCount(void) const
		{
		return ( ptr - baseptr ) ;		// basemaxsize - maxsize );
		}

	bool operator>( const class Cst &x ) const ;

#ifndef __TURBOC__	// Turbo C++ cannot expant this function
	Cst &operator=( const Cst &y )
	{
	uint i ;
	uint n ;

	n = y.GetCount();
	rewind();
	for ( i = 0 ; i < n ; ++i )
		{
		ochabin( y.baseptr[i] );
		}
	ochatx( 0 );
	return *this ;
	}
#endif

// next methods bring data to string
	int puts( const char *p ); // print a zero terminated string
	int sttolower( const char *p );	// print a zero terminated string and convert
	int sttoupper( const char *p );	// print a zero terminated string and convert
	int putmem( const char *p, const uint size );	// print a zero terminated string

	// ochabin can fill all memory
	// ochatx keeps a byte for the trailing 0 pointer
	// ochabin increments pointer even after put 0 byte
	// ochatx does not increment pointer after 0 byte in
	inline int ochatx( const char z )	// print a single char, increment pointer if non zero
		{
		if ( z == 0 )
			{
			if ( maxsize <= 0 )
				{
				return 1 ;		// error detected
				}
			else
				{
				*ptr = z ;	// can finish up
				return 0 ;
				}
			}
		else
			{
			if ( maxsize <= 1 )
				{
				return 1 ;		// error detected
				}
			else
				{
				*ptr = z ;
				++ptr ;
				--maxsize ;
				return 0 ;
				}
			}
		} ;

	inline int ochabin( const uint8_t z ) // store a single byte, increment pointer anyway
		{
		if ( maxsize < 1 )
			{
			return 1 ;		// error detected
			}
		else
			{
			*ptr++ = z ;
			--maxsize ;
			return 0 ;
			}
		} ;

	Cst &operator=( const char *p )
		{
		rewind();
		puts( p );
		ochatx( 0 );
		return *this ;
		}

	int dirname( const char *pn );		// extract file name of path
	int dirapp( const char *pn );		// append path to given path
	int nameapp( const char *pn )		// append path to given path
		{
		return dirapp( pn );
		}
	int filename( const char *pn );		// extract file name of path

	int reldirpos( const char *pfile, const char *pdir ); // cstreldi.cpp
					// make string: relative file postition of file to directory
					// sample : pfile = /home/html/hjh/pc/myfile.htm
					//			pdir = /home/html/hjh/xone
					// result : ../pc/myfile.htm

/*	bool operator>( class Cst const &x )const
		{
		if ( strcmp( baseptr, x.baseptr) > 0 ) return true ;
		return false ;
		}
*/
	int hex8( uint32_t z );	// convert to string
	int hex4( uint16_t z );	// print to string
	int hex3( uint16_t z );	// print to string
	int hex2( uint8_t z );	// print to string
	int hex1( uint8_t z );	// print to string
	int daysec( const uint32_t secval );
	int dec( double val, uint prec );
	int decsz( double val, uint fieldsize );
	int dec( int8_t val );
	int dec( uint8_t val );
	int dec( int16_t val );
	int dec( uint16_t val );
	int dec( int32_t val );
	int dec( uint32_t val );
//	int dec( sint val );
//	int dec( uint val );

// Binary conversion
	inline int binlow( uint8_t z )	// // convert binary to single byte
		{
		if ( maxsize <= 1 )
			{
			return 1 ;		// error detected
			}
		else
			{
			*ptr++ = (char)z ;
			--maxsize ;
			return 0 ;
			}
		} ;

	int binlow( uint16_t z );	// convert binary to 2 bytes
	int binlow( uint32_t z );	// convert bin to 4 bytes

	int binhigh( uint16_t z ); // convert binary to 2 bytes
	int binhigh( uint32_t z ); // convert bin to 4 bytes

	inline int binlow( int32_t z )	// convert binary to 4 bytes
		{
		return binlow( (uint32_t)z );
		}

	inline int binlow( int16_t z )	// convert binary to 4 bytes
		{
		return binlow( (uint16_t)z );
		}

	void epochst( CEpoch epochval );

	int msecst( uint32_t msec );		// File Cstmsec.cpp

	bool move_file( const char *filename,
		s_move_file_order *pfirst,
		const char *basedirectory );

	char *PicRegF( uint mc );		// Display a PIC 16F84 register Name
	char *PicBit( uint mc );		// Display a PIC 16F84 Status Bit Name C DC Z ...IRP
	int DisaPic( uint16_t code );		// Disassemble a single PIC 16F84 machine word


	int DisaAtmel( uint pc, uint16_t code );	// Disassemble a single Atmel 2313 machine word

} ;			// end class Cst - Size limited string


class CstchainEl  {

public:
	class CstchainEl *prev ;
	class CstchainEl *next ;
	class Cst *p ;
	CstchainEl( uint size )		// constructor
		{
		prev = 0 ;
		next = 0 ;
		p = new Cst( size );
		}
	} ;

class Cstchain	{

public:
	class CstchainEl *first ;
	class CstchainEl *last ;
	class CstchainEl *current ;
	int add(int ss );
	int rewind( void );
	int end( void );
	Cstchain() ;		// Constructor
	int vt( void );
	int get( const char **ppch );
	int next() ;
	} ;


#endif
