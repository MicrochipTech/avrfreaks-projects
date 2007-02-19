// vtcompil.cpp

#include <heport.h>
#include <helq.h>
#include <hevt.h>

// displays information about the compiler
void vtcompil()
{

vtputs( "compiled by : ");
#ifdef LINUX
vtputs( "LINUX" );
#endif

#ifdef OSK
vtputs( "OSK" );
#endif

#ifdef MDOS10
vtputs( "MDOS10 MSVC1.52 DOS" );
#endif
#ifdef MSVC10
vtputs( "MSVC10 MSVC1.0 Windows" );
#endif
#ifdef MDOS40
vtputs( "MDOS40 Microsoft Visual C Console Application Version 4.0 or 5.0" );
#endif
#ifdef MSVC40
vtputs( "MSVC40 Microsoft Visual C Windows Application Version 4.0 or 5.0" );
#endif
#ifdef DOSGNU
vtputs( "DOSGNU: DJ GNU Compiler for DOS" );
#endif
#ifdef __TURBOC__
vtputs( "__TURBOC__: Borlands Turbo C++ for DOS - 16 Bit from 1990" );
#endif
vtocha( '\n' );
}

// displays information about the compiler
void logvtcompil()
{

logvtputs( "compiled by : ");
#ifdef LINUX
logvtputs( "LINUX" );
#endif

#ifdef OSK
logvtputs( "OSK" );
#endif

#ifdef MDOS10
logvtputs( "MDOS10 MSVC1.52 DOS" );
#endif
#ifdef MSVC10
logvtputs( "MSVC10 MSVC1.0 Windows" );
#endif
#ifdef MDOS40
logvtputs( "MDOS40 Microsoft Visual C Console Application Version 4.0 or 5.0" );
#endif
#ifdef MSVC40
logvtputs( "MSVC40 Microsoft Visual C Windows Application Version 4.0 or 5.0" );
#endif
#ifdef DOSGNU
logvtputs( "DOSGNU: DJ GNU Compiler for DOS" );
#endif
#ifdef __TURBOC__
logvtputs( "__TURBOC__: Borlands Turbo C++ for DOS - 16 Bit from 1990" );
#endif
logvtocha( '\n' );
}
