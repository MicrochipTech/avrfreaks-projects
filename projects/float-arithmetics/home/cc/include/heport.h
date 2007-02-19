#ifndef __heport_h__
#define __heport_h__
// heport.h		 Helps to write portable code
// This file is usable for the 3 operation systems :
// #define OSK					compiled by GNU 2.5.8
// #define LINUX				compiled by GNU 2.7.1
// #define DOSGNU				compiled by GNU 2.8.2
//				  the define is built in C:\home/cc/djbat/ccs.bat
// #define BORWIN				Borlands C++ for Windows
// #define MSVC10				Microsofts Windows C++-Compiler 16 Bit, WIN Application
// #define MDOS10				Microsofts Windows C++-Compiler 16 Bit, DOS Application
// #define MSVC40				Microsofts 32 Bit-Compiler 4 or 5, WIN Application
// #define MDOS40				Microsofts 32 Bit-Compiler, Console application modus
// #define MSVCXX		MSVC10 oder MSVC40
// #define _WIN31_				Windows, Microsoft oder Borland


// #define __TURBOC__			Borland C++ for Windows or for DOS
// #define __TORBOCDOS__		Borland C++ for DOS Only

// The SET OS= - Variable used to detect different operationg systems, on
// which one compiler must generate different code.
// The SET OS= is predefined in Windows-NT 4.0 as ......
// for other operating Systems on DOS-Base it must be set in AUTOEXEC.BAT
// SET OS=WFW311
// SET OS=DOS622
// SET OS=WIN95
// C-Code sees these environments as #define WIN95 or #define DOS622

// Borland has Predefined : __TURBOC__ for both DOS and TCWIN31
//
// recalculate compiler versions, build final #defines
// The #define BYTEORDER variable shows how WORD or LONG is stored
// and can be accessed as CHAR array :
// BYTEORDER = 1 as 8088 / 80386 : 2^0, 2^8, 2^16, 2^24
// BYTEORDER = 2 as 68000		 : 2^24, 2^16, 2^8, 2^0

#ifdef AVR // for ATMEL AVR processors as e.g. ATMEGA8535

#define AVRASMC(x,y) asm( x #y "\n" )
// defined by the invocation line :
// #define __AVR_ATmega8535__ // see C:\WinAVR\AVR\INCLUDE\io.h

#  define HEFAR
#  define INTSIZE 16
#  define LONGSIZE 32
#  define BYTEORDER 1	// 8088
// #define AVR
#else
#define PGM_P const char *
#endif





#ifdef __TURBOC__ // for Windows
#  define HEFAR far
#  define INTSIZE 16
#  define LONGSIZE 32
#  define BYTEORDER 1	// 8088
#  ifdef _Windows
#	 define BORWIN
#	 define _WIN31_		// uses WinMain as main program, redefined main to mymain
#  else
#	 define __TURBOCDOS__
#  endif
// #define TC
#endif





#if defined( _MSC_VER )
#define BYTEORDER 1 // 8088
// some microsoft compiler Version 1.0 1.5 4.0 or 5.0
#  if _MSC_VER <= 899		// MSVC 1.52 or 1.00
#include <stdlib.h>
#include <dos.h>
#	 define INTSIZE 16
#	 define LONGSIZE 32
#	 ifdef _WINDOWS
#	   define MSVC10	// MSVC10 / 15
#	 else
#	   define MDOS10	// MSVC10 / 15
#	 endif
#  else	 // somehow 40 or 50
#	 define MDOS40	// MSVC40, Console application
#	 define HEFAR		// Flat Memory Model
#	 define INTSIZE 32
#	 define LONGSIZE 32
#	 ifdef _WINDOWS
#	   define MSVC40	// MSVC40 / 50
#	 else
#	   define MDOS40	// MSVC40 / 50
#	 endif
#  endif
#  ifdef _WINDOWS
#	 define _WIN31_
#	 define MSVCXX
#	 define main mymain
	 extern int mymain( int ac, const char **av );				 // called by WinMain
#  endif
#endif


// Set dependings :
#ifdef OSK

#define HEFAR
#ifndef WIN32
#define BYTEORDER 2 // 8088
#define OPSYS 3
#define INTSIZE 32
#define LONGSIZE 32
#endif

#endif


#ifdef DOSGNU
#define HEFAR
#define BYTEORDER 1 // 8088
#define OPSYS 2
#define INTSIZE 32
#define LONGSIZE 32
#define BOOL int	// do not use BOOL, rather bool !
#define TRUE 1	// do not use TRUE, rather true !
#define FALSE 0 // do not use FALSE, rather false !
#endif


#ifdef LINUX		// Linux means Linux on a PC (80386 ++) or AMD64
#define BYTEORDER 1 // 8088
#define HEFAR
#define LINUX_SVGA 0	// Either SVGA
#define LINUX_KDE 1		// or KDE graphics should be set, not both !
#define OPSYS 1
#ifdef AMD64
#define INTSIZE 32
#define LONGSIZE 64
#else
#define INTSIZE 32
#define LONGSIZE 32
#endif
#endif



// --------------------------------------------------------------
#ifdef BORWIN
#define main mymain
extern int mymain( int ac, const char **av );	// called by WinMain
#endif



// File system type FSYSTEM

#define FSYSTEM_OSK 4		// OSK								 "/h0/TEMP/jj,txt"
#define FSYSTEM_LINUX 3		// LINUX							 "/tmp/jj.txt"
#define FSYSTEM_DOS_WIN 1	// classical or extended DOS		 "C:\\TEMP\\xy.txt"

#if defined( OSK )
#define FSYSTEM FSYSTEM_OSK
#elif defined( LINUX )
#define FSYSTEM FSYSTEM_LINUX		// LINUX Type File system
#else
#define FSYSTEM FSYSTEM_DOS_WIN		// DOS / Windows Type "C:\\XXX\\jj.txt"
#endif

// File system is case sensitive ?
#if defined( OSK )
const int FSYSTEMCASESENSITIVE = 0 ;
#elif defined( LINUX )
const int FSYSTEMCASESENSITIVE = 1 ;
#else
const int FSYSTEMCASESENSITIVE = 0 ;
#endif



// my comment for functions / (methods)
#define FUNCT
#if ! ( defined( AVR ) || defined( LINUX ) )
// generate my favorite data types
typedef unsigned long	uint32_t ; // 32 Bits, or 64 bits
typedef	  signed long	int32_t ;
typedef unsigned short	uint16_t ; // Always 16 bits
typedef	  signed short	int16_t ;
typedef unsigned char	uint8_t ; // Always 8 bits
typedef	  signed char	int8_t ;
#else
#include <stdint.h>
#endif
typedef unsigned int	uint ;	// sometimes 32 Bits, sometimes 16 bits
typedef	  signed int	sint ;

// include Compilers standard headers
#ifdef OSK
#include <stdlib.h>
#include <errno.h>
#endif


#ifdef DOSGNU
#include <stdlib.h>
#include <bool.h>
#include <limits.h> // NAME_MAX
#undef NAME_MAX
#define NAME_MAX 128 // max Path
#endif


#ifdef BORWIN
#include <stdlib.h>
#include <windows.h>
#endif

#ifdef __TURBOC__
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>			// read write open close
#include <dir.h>		// chdir
#include <string.h>
#endif

#ifdef MSVC10
#include <string.h>
#include <stdlib.h>		// system()
#include <process.h>		// system()
int __cdecl system(const char *);

#endif

#ifdef LINUX
# ifndef _UNISTD_H
# include <unistd.h>
# endif
# ifndef _STDLIB_H
# include <stdlib.h>
# endif
# ifndef _FCNTL_H
# include <fcntl.h>
# endif
# ifndef _DIRENT_H
# include <dirent.h>
# endif
#endif

#ifdef MSVCXX
// #include <afx.h>
# include <stdlib.h>
# include <malloc.h>
# ifndef __AFXWIN_H__
#  if defined(_MFC) || defined (_AFXDLL)
#	include <afx.h>
#  else
#	include <windows.h>
#  endif
# endif
#endif



// define the bool stuff
#ifdef OSK
#ifndef WIN32
enum bool { false = 0, true = 1 };

#define BOOL bool
#define FALSE false
#define TRUE true

#endif
#endif

#if defined( MSVC10 ) || defined( MDOS10 )
enum bool { false = 0, true = 1 };
// typedef int bool ;
#endif



#ifdef AVR	  // Win-AVR
#define bool char
#define false 0
#define true 1
#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif
typedef int tfh ;										// File handle type
#endif





#ifdef BORWIN	// Turbo Cpp for Windows
// typedef uint8_t bool ;
#endif

// define the bool stuff
#ifdef __TURBOC__	// for DOS or for Windows
enum bool { false = 0, true = 1 };
#define BOOL int	// do not use BOOL
#define FALSE 0
#define TRUE 1
#endif





#ifdef LINUX	// Linux
#define bool char
#define false 0
#define true 1
#ifndef BOOL
#define BOOL int
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#endif
typedef int tfh ;										// File handle type
#endif




#ifdef OSK
// Special with OS-9
int tsleep(unsigned int ntick);
#define NAME_MAX 80				// posix size of subdirectory name

#ifndef WIN32
#define S_IRUSR 0001
#define S_IWUSR 0002
#define S_IXUSR 0002
#define S_IRGRP 0010
#define S_IWGRP 0020
#define S_IXGRP 0020
#define S_IROTH 0200
#define mkdir(x,y) mknod(x,y)
#endif

#endif

#ifdef __TURBOC__
// Special with TC
// #define NAME_MAX 80			   // posix size of subdirectory name

#define S_IRUSR 0001
#define S_IWUSR 0002
#define S_IXUSR 000
#define S_IRGRP 0010
#define S_IWGRP 0020
#define S_IXGRP 000
#define S_IROTH 0200
#endif






// heport.hxx
// Notwendige Sachen


#ifdef _MSC_VER		// defined as 800, 1100
#ifndef FALSE		// do not use this - but this makes some programs compatible to
#define FALSE 0		// Microsoft Windows SDK
#define TRUE 1
typedef int BOOL ;
#endif
#endif


#ifdef _MSC_VER	  // QUICKWIN MSVC 1.0 oder 4.0

#include <fcntl.h>
#include <io.h>

#ifdef _MFC
typedef int tfh ;		// HANDLE	void *
#else			// MSVC10
typedef int tfh ;										// File handle type
#endif


// typedef unsigned char bool ;
// const bool FALSE = 0 ;
// const bool TRUE = 1 ;

#define S_IRUSR 0002
#define S_IWUSR 0004
#define S_IRGRP 0020
#define S_IWGRP 0040
#define S_IROTH 0200
#define S_ISDIR(m) ((m)&0x4000) // Directory bit in DOS	 0x4000 = 040000 Oct
#define NAME_MAX 80				// posix size of subdirectory name
// fun : O_RDONLY, O_WRONLY or O_RDWR
// #define normcreat(name) creat(name,0x07)
#if _MSC_VER >= 1100

inline tfh normcreat( const char *name)
	{
	return ::_open(name,O_RDWR|O_TRUNC|O_CREAT|O_BINARY, -1 );
	}
inline tfh normopen( const char *name, int fun)
	{
	return ::_open(name,fun|O_BINARY, -1 ) ;
	}
#define normread ::_read		// (tfh fd, char *pbuffer, (u?)int count )
#define normwrite ::_write
#define normlseek ::_lseek		// (tfh fd, long lpos, int cd) cd=0 begin, cd=1 current cd=2 From End
#define normclose(fd) _close(fd)
#else
inline tfh normcreat( const char *name)
	{
	return ::open(name,O_RDWR|O_TRUNC|O_CREAT|O_BINARY, -1 );
	}
inline tfh normopen( const char *name, int fun)
	{
	return ::open(name,fun|O_BINARY, -1 ) ;
	}
#define normread ::read
#define normwrite ::write
#define normlseek ::lseek
#define normclose(fd) close(fd)
#endif
#endif // MSVCXX == MSVC10 or MSVC40





#if defined( MDOS40 )

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <io.h>
typedef int tfh ;		// HANDLE	void *

#if _MSC_VER < 1100		// before MSVC 5.00 as 4.00 or
typedef int bool ;									 // DOS bool
#endif

#define false 0										 // I will use true / false
#define true 1										 // not: TRUE / FALSE


// typedef unsigned char bool ;
// const bool FALSE = 0 ;
// const bool TRUE = 1 ;

#define S_IRUSR 0002
#define S_IWUSR 0004
#define S_IRGRP 0020
#define S_IWGRP 0040
#define S_IROTH 0200
#define S_ISDIR(m) ((m)&0x4000) // Directory bit in DOS	 0x4000 = 040000 Oct
#define NAME_MAX 80				// posix size of subdirectory name
// fun : O_RDONLY, O_WRONLY or O_RDWR
// #define normcreat(name) creat(name,0x07)

#define normcreat(name) _open(name,O_RDWR|O_TRUNC|O_CREAT|O_BINARY, -1 )
#define normopen(name,fun) _open(name,fun|O_BINARY, -1 )
/*
#define normread _read
#define normwrite _write
#define normlseek _lseek	// ( fd, offset, origin ) origin 0:Begin, 1:Current, 2:End
#define normclose(fd) _close(fd)
*/
#endif		// MDOS40




// How to handle a file with the different operating systems

#ifdef DOSGNU
#include <fcntl.h>
// #include <dos.h>
// fun : O_RDONLY, O_WRONLY or O_RDWR
// #define normcreat(name) creat(name,0x07)
inline int normcreat( const char *name)
	{
		return ::open(name,O_RDWR|O_TRUNC|O_CREAT|O_BINARY,00200) ;
	}
		// Creation file mode cntl number :
		// 0 : Readonly
		// 00777 : ordinary file
		// 00700 : ordinary file
		// 00400 : RO File
		// 00200 = 0x0080 : ordinary file (on Windows 95!)

#define normopen(name,fun) ::open(name,fun|O_BINARY,0x07)
#define normread ::read
#define normwrite ::write
#define normlseek ::lseek
#define normclose(fd) ::close(fd)
// #define NAME_MAX 80							   // posix size of subdirectory name (limits.h)
typedef int tfh ;				// File handle type
#endif


#ifdef __TURBOC__
#include <fcntl.h>				// in C:\TC\INCLUDE
// fun : O_RDONLY, O_WRONLY or O_RDWR
// #define normcreat(name) ::creat(name,0x07)

#define normopen(name,fun) ::open(name,fun|O_BINARY,0x07)
// #define normread(x,y,z) ::read(x,y,z)
inline int normread( int fd, const void *ptr, int len)
		{
		return ::read( fd, ptr, len) ;
		}
inline int normwrite( int fd, const void *ptr, int len)
		{
		return ::write( fd, ptr, len) ;
		}


inline int normcreat( const char *name)
	{
		return ::open(name,O_RDWR|O_TRUNC|O_CREAT|O_BINARY,00200) ;
	}

#define normlseek(x,y,z) ::lseek(x,y,z) // z=0 SEEK_SET, from begin, z=1, from z=2 SEEK_END
#define normclose(fd) ::close(fd)
#define NAME_MAX FILENAME_MAX	// from	 stdio.h. max length of a files name
								// us as :	 char filename[NAME_MAX]
								// posix size of subdirectory name
typedef int tfh ;				// File handle type
#endif


#ifdef LINUX
// #include <statbuf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef int tfh ;				// File handle type
// #define normcreat(name) creat(name,0755) // this would open in
// write only mode, but for heap.cpp it is required to open in RW mode.
inline tfh normcreat(const char *name)
	{		// Siehe man 2 open
	return ::open( name, O_RDWR|O_TRUNC|O_CREAT, S_IRUSR | S_IWUSR );
	}
#define normopen(name,fun) ::open( name, fun, 0777 )
#define normread ::read
#define normwrite ::write
#define normlseek ::lseek
#define normclose ::close
extern int errno ;
#endif



#ifdef OSK
#	ifndef S_IWRITE						// defined in modes.h
#	include <modes.h>						// access to files
#	endif

#ifndef WIN32

# include <sys/file.h>					// O_RDONLY ...
// #define O_RDONLY 0	   // O_READ
// #define O_WRONLY 1
// #define O_RDWR 2

#define normcreat(name) ::creat(name,S_IWRITE|S_IREAD)
#define normopen(name,fun) ::open(name,fun)
#define normread ::read
inline int normwrite(int fd, const void *p, int n)
	{
	return ::write( fd, (void *)p, n );
	}
#define normlseek ::lseek
#define normclose ::close
typedef int tfh ;				// File handle type

#endif	// WIN32

#endif	// OSK


#ifdef AVR
#define O_RDONLY 0		// O_READ
#define O_WRONLY 1
#define O_RDWR 2
#endif


// Access to DOS page 0
#ifdef __TURBOC__
#include <dos.h>
// used by UTIL/bootsect.cpp
inline uint8_t hepeek( uint16_t offs, uint16_t segm )
	{
	return (uint8_t)peekb( segm, offs );
	}
inline void hepoke( uint8_t databyte, uint16_t offs, uint16_t segm )
	{
	pokeb( segm, offs, databyte );
	}

#endif	// __TURBOC__



// How to tell the operating system there is nothing o do (few to check)
void hesleep( uint32_t msec);




#endif	// ifndef __heport_h__
