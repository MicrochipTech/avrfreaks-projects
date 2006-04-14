/***********************************************************************
Content: Common definitions and functions
Created: 13.10.2003
Last modified: 17.02.2004
Copyrights: Free to use, free to change, free to delete :-)
Compiler: ImageCraft AVR
Written by: Knut Baardsen @ Baardsen Software, Norway
Updates: http://www.baso.no
------------------------------------------------------------------------
This software is provided "as is"; Without warranties either express or
implied, including any warranty regarding merchantability, fitness for 
a particular purpose or noninfringement. 
In no event shall Baardsen Software or its suppliers be liable for any 
special,indirect,incidential or concequential damages resulting from 
the use or inability to use this software.
***********************************************************************/



/***********************************************************************
Choose the following include file according to the processor type used
NB! Remember to set CPU_CLOCK in MHz before using the delay function.
***********************************************************************/
#include <iom16v.h>
#define CPU_CLOCK		  8.000


/***********************************************************************
Type definitions
***********************************************************************/
typedef unsigned char     BOOLEAN;
typedef unsigned char     BYTE;
typedef signed char       CHAR;
typedef unsigned int      WORD;
typedef unsigned long     ULONG;


/***********************************************************************
Constants
***********************************************************************/
#define TRUE              1
#define FALSE             0


/***********************************************************************
Macros
***********************************************************************/
#define BIT(x)            (1 << (x))
#define SETBIT(addr,x)    (addr |= BIT(x))
#define CLEARBIT(addr,x)  (addr &= ~BIT(x))

#define NOP()             asm( "NOP" )
#define CLI()             asm( "CLI" )
#define SEI()             asm( "SEI" )
#define SLEEP()           asm( "SLEEP" )


/***********************************************************************
Delay routine
NB! Remember to set CPU_CLOCK in MHz before using this delay function.
***********************************************************************/
void waitms(long milliseconds);


/***********************************************************************
End of code listing
***********************************************************************/