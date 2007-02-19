// Thist is DOS File  C:\HOME\CC\LQ\uprintf.cpp
// or	  LINUX File  ~/cc/lq/uprintf.cpp
// or	   OS-9 File  /h0/HOME/CC/LQ/uprintf.cc

#define DB_ENABLED 0
#define DB_INFO 0
// uprintf.cpp	 Display a user message
// usage uprintf( format, ... );
// will Display ( formatting similar to printf) a message (with parameters)
// and prompt the user to press a key.
// ask_uprintf( ANSWER_ELEMENT_ABORTRETRYIGNORE, "...%d", ... );
// ask_uprintf( ANSWER_ELEMENT_OK 2
// ask_uprintf( ANSWER_ELEMENT_OKCANCEL 3
// ask_uprintf( ANSWER_ELEMENT_RETRYCANCEL 4
// ask_uprintf( ANSWER_ELEMENT_YESNO 5
// ask_uprintf( ANSWER_ELEMENT_YESNOCANCEL 6
// See also : ask( "RIC", "What to do? [R]etry [I]gnore [C]ontinue?" );

#include <stdio.h>			// sprintf
#include <heport.h>
#include <helang.h>
#include <hedeb.h>
#define DEF_UPRINTF
#include <helq.h>
#include <helog.h>
#include <hevt.h>



#include <helq.h>

bool uprintf_wait_for_answer = true ;
int i_uprintf_answer ;		  // 'q' entered
// here the last answer is saved
// ANSWERTYPE_OK 1
// ANSWERTYPE_CANCEL 2
// ANSWERTYPE_RETRY 3
// ANSWERTYPE_YES 4
// ANSWERTYPE_NO 5
// ANSWERTYPE_ABORT 6
// ANSWERTYPE_IGNORE 7


extern "C" {

/*********************************************************************/
#if defined( _WIN31_ ) || defined( WINDOWS )
// In Windows we will create a Windows Message Box
#define up_defined
FUNCT int ask_uprintf( // The Windows version
int answertype ,
const char *fmt ,
int p0, int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9 )
{
char buffer[255] ;
int rv ;
int myrv ;
int mb_type ;


myrv = 0 ;
DBent("uprintf");
Eprintf(( fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 ));

logprintf( fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 );
logprintf( "\n" );
// wsprintf does not work with Microsoft Windows 98 !
// sprintf( (LPSTR)buffer, (LPCSTR)fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 );
sprintf( buffer, fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 );

switch( answertype )
	{
case ANSWER_ELEMENT_ABORTRETRYIGNORE : mb_type = MB_ABORTRETRYIGNORE ; break ;

default:
case ANSWER_ELEMENT_OK				 : mb_type = MB_OK				 ; break ;
case ANSWER_ELEMENT_OKCANCEL		 : mb_type = MB_OKCANCEL		 ; break ;
case ANSWER_ELEMENT_RETRYCANCEL		 : mb_type = MB_RETRYCANCEL		 ; break ;
case ANSWER_ELEMENT_YESNO			 : mb_type = MB_YESNO			 ; break ;
case ANSWER_ELEMENT_YESNOCANCEL		 : mb_type = MB_YESNOCANCEL		 ; break ;
	}

rv = MessageBox( 0, buffer, "Message", mb_type );

switch( answertype )
	{
	case ANSWER_ELEMENT_ABORTRETRYIGNORE :
	switch ( rv )
		{
		case IDABORT  : myrv = ANSWERTYPE_ABORT ; break ;
		case IDRETRY  : myrv = ANSWERTYPE_RETRY ; break ;
		case IDIGNORE : myrv = ANSWERTYPE_IGNORE ; break ;
		} break ;

	default:
	case ANSWER_ELEMENT_OK				 :
		{
		myrv = ANSWERTYPE_OK ;
		} break ;

	case ANSWER_ELEMENT_OKCANCEL		 :
	switch ( rv )
		{
		case IDOK	  : myrv = ANSWERTYPE_OK ; break ;
		case IDCANCEL : myrv = ANSWERTYPE_CANCEL ; break ;
		} break ;

	case ANSWER_ELEMENT_RETRYCANCEL		 :
	switch ( rv )
		{
		case IDRETRY  : myrv = ANSWERTYPE_RETRY ; break ;
		case IDCANCEL : myrv = ANSWERTYPE_CANCEL ; break ;
		} break ;

	case ANSWER_ELEMENT_YESNO			 :
	switch ( rv )
		{
		case IDYES	  : myrv = ANSWERTYPE_YES ; break ;
		case IDNO	  : myrv = ANSWERTYPE_NO ; break ;
		} break ;

	case ANSWER_ELEMENT_YESNOCANCEL		 :
	switch ( rv )
		{
		case IDYES	  : myrv = ANSWERTYPE_YES ; break ;
		case IDNO	  : myrv = ANSWERTYPE_NO ; break ;
		case IDCANCEL : myrv = ANSWERTYPE_CANCEL ; break ;
		} break ;
	}
/*
if ( rv == IDNO )
	{
	myrv = ANSWERTYPE_ABORT ;
	}
if ( rv == IDYES )
	{
	myrv = ANSWERTYPE_OK ;
	}
*/
i_uprintf_answer = myrv ;
DBret("uprintf");
return myrv ;
}
#endif

#ifndef up_defined
// In other operating Systems we will display the message and ask the
// user to type a key
FUNCT int ask_uprintf( // All other Verions of subroutine uprintf
int ask_elements ,
char *fmt ,
int p0, int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9 )
{
int myrv ;

DBent( "ask_uprintf" );

logprintf( fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 );
logprintf( "\n" );

if ( ask_elements == 0 ) ask_elements = ANSWER_ELEMENT_OK ;

vtattr( VTASTRONG );
vtprintf( "\n" );
vtprintf( TDEU("FEHLER :") TENG("ERROR :") TFRA("ERREUR :") );
vtprintf( fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 );
vtprintf( "\n Press " );

if ( ask_elements == ANSWER_ELEMENT_OK || ask_elements == ANSWER_ELEMENT_OKCANCEL )
	vtputs( TDEU( "[O]k  " )		 TENG( "[O]k " )	  TFRA( "[D]'accord" ) );
if ( ask_elements == ANSWER_ELEMENT_ABORTRETRYIGNORE )
	vtputs( TDEU( "[A]bbruch " )	 TENG( "[A]bort " )	  TFRA( "[A]nnuller " ) );
if ( ask_elements == ANSWER_ELEMENT_ABORTRETRYIGNORE || ask_elements == ANSWER_ELEMENT_RETRYCANCEL )
	vtputs( TDEU( "[W]iederholen " ) TENG( "[R]etry " )	  TFRA( "[R]etrouver " ) );
if ( ask_elements == ANSWER_ELEMENT_ABORTRETRYIGNORE )
	vtputs( TDEU( "[I]gnorieren " )	 TENG( "[I]gnore " )  TFRA( "[I]gnorer " ) );
if ( ask_elements == ANSWER_ELEMENT_OKCANCEL || ask_elements == ANSWER_ELEMENT_RETRYCANCEL || ask_elements == ANSWER_ELEMENT_YESNOCANCEL )
	vtputs( TDEU( "[V]erwerfen " )	 TENG( "[C]ancel " )  TFRA( "[E]nlever " ) );
if ( ask_elements == ANSWER_ELEMENT_YESNO || ask_elements == ANSWER_ELEMENT_YESNOCANCEL )
	vtputs( TDEU( "[J]a " )			 TENG( "[Y]es " )	  TFRA( "[O]ui " ) );
if ( ask_elements == ANSWER_ELEMENT_YESNO || ask_elements == ANSWER_ELEMENT_YESNOCANCEL )
	vtputs( TDEU( "[N]ein " )		 TENG( "[N]o " )	  TFRA( "[N]on " ) );

for(;;)
	{
	uint zch ;
	zch = vtkey() ;
	zch = itolower( zch ) ;

	if ( ( ask_elements == ANSWER_ELEMENT_OK || ask_elements == ANSWER_ELEMENT_OKCANCEL )
				&& zch == TDEU( 'o' ) TENG( 'o' ) TFRA( 'd' ) )
		{
		vtputs( TDEU( "OK\n" ) TENG( "OK\n" ) TFRA( "D'accord") );
		myrv = ANSWERTYPE_OK ;
		break ;
		}

	if ( ( ask_elements == ANSWER_ELEMENT_ABORTRETRYIGNORE )
				&& zch == TDEU( 'a' ) TENG( 'a' ) TFRA( 'a' ) )		// Abbruch Abort
		{
		vtputs( TDEU( "Abbruch\n" ) TENG( "Abort\n" ) TFRA( "Interrompre\n" ) );
		myrv = ANSWERTYPE_ABORT ;
		break ;
		}

	if ( ( ask_elements == ANSWER_ELEMENT_ABORTRETRYIGNORE || ask_elements == ANSWER_ELEMENT_RETRYCANCEL )
				&& zch == TDEU( 'w' ) TENG( 'r' ) TFRA('r') )
		{
		vtputs( TDEU( "Wiederholen\n" ) TENG( "Retry\n" ) TFRA("Retrouver\n") );
		myrv = ANSWERTYPE_RETRY ;
		break ;
		}

	if ( ( ask_elements == ANSWER_ELEMENT_ABORTRETRYIGNORE )
				&& zch == TDEU( 'i' ) TENG( 'i' ) TFRA('i') )
		{
		vtputs( TDEU( "Ignorieren\n" ) TENG( "Ignore\n" ) TFRA( "Ignorer\n") );
		myrv = ANSWERTYPE_IGNORE ;
		break ;
		}

	if ( ( ask_elements == ANSWER_ELEMENT_OKCANCEL || ask_elements == ANSWER_ELEMENT_RETRYCANCEL || ask_elements == ANSWER_ELEMENT_YESNOCANCEL )
				&& zch == TDEU( 'v' ) TENG( 'c' ) )
		{
		vtputs( TDEU( "Verwerfen\n" ) TENG( "Cancel\n" ) );
		myrv = ANSWERTYPE_CANCEL ;
		break ;
		}

	if ( ( ask_elements == ANSWER_ELEMENT_YESNO || ask_elements == ANSWER_ELEMENT_YESNOCANCEL )
				&& zch ==  TDEU( 'j' ) TENG( 'y' ))
		{
		vtputs( TDEU( "Ja\n" ) TENG( "Yes\n" ) );
		myrv = ANSWERTYPE_YES ;
		break ;
		}

	if ( ( ask_elements == ANSWER_ELEMENT_YESNO || ask_elements == ANSWER_ELEMENT_YESNOCANCEL )
				&& zch ==  TDEU( 'n' ) TENG( 'n' ))
		{
		vtputs( TDEU( "Nein\n" ) TENG( "No\n" ) );
		myrv = ANSWERTYPE_NO ;
		break ;
		}

	if ( ( ask_elements == ANSWER_ELEMENT_OKCANCEL || ask_elements == ANSWER_ELEMENT_RETRYCANCEL || ask_elements == ANSWER_ELEMENT_YESNOCANCEL )
				&& zch == TENG( 'c' ) TDEU( 'v' ) )
		{
		vtputs( TDEU( "Verwerfen\n" ) TENG( "Cancel\n" ) );
		myrv = ANSWERTYPE_IGNORE ;
		break ;
		}

	}
vtattr( VTANORM );
vtrefresh();
i_uprintf_answer = myrv ;

DBret( "ask_uprintf" );
return myrv ;
}
#endif

	} // end extern "C"



extern "C" {

/*********************************************************************/
#ifdef _WIN31_
// In Windows we will create a Windows Message Box
#define up_defined
FUNCT void uprintf( // The Windows version
char *fmt ,
int p0, int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9 )
{
char buffer[255] ;

DBent("uprintf");

// wsprintf does not work with Microsoft Windows 98 !
// sprintf( (LPSTR)buffer, (LPCSTR)fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 );
nsprintf( buffer, sizeof(buffer), fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 );
logprintf( "%s\n", buffer );
Eprintf(( "%s\n", buffer ));
MessageBox( 0, buffer, "Message uprintf:", MB_OK );

DBret("uprintf");
return ;
}
#endif

#ifndef up_defined
// In other operating Systems we will display the message and ask the
// user to type a key
FUNCT void uprintf( // All other Verions of subroutine uprintf
char *fmt ,
int p0, int p1,int p2,int p3,int p4,int p5,int p6,int p7,int p8,int p9 )
{
DBent("uprintf");
logprintf( fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 );
logprintf( "\n" );

vtattr( VTASTRONG );
vtprintf( "\n" );
vtprintf( TDEU("FEHLER :") TENG("ERROR :") TFRA("ERREUR :") );
vtprintf( fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 );

DBprintf(( fmt, p0,p1,p2,p3,p4,p5,p6,p7,p8,p9 ));
if ( uprintf_wait_for_answer )
	{
	vtprintf( TDEU("\n Drueke ESC um fortzufahren")
			  TENG("\n Press ESC to continue")
			  TFRA("\n Appuyer ESC pout continuer") );
	for(;;)
		{
		uint zch ;
		zch = vtkey() ;
		if ( zch == VTK_ACK )
			{
			vtnl();
			break ;
			}
		continue ;
		}
	}
vtattr( VTANORM );
vtprintf( "\n" );
DBret("uprintf");
return ;
}


#endif

} // end extern "C"

