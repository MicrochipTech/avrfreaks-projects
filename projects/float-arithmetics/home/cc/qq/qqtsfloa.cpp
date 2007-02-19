// qqtsfloa - Demonstrates the dialog through a serial (simple) interface
//
// This main program demonstrates the usage of fLoating point routines for the ATMEL
// together with the functions for serial interface tsser.cpp
// This program is intended to run on a ATMEL microprocessor, but has been successfully
// compiled ant run as well for :

// MDOS40 with #define MDOS40_VT in tsser.cpp
//
// This tigether with the programs in ffladd.s ffldiv.s and fflmul.s
// are verified by qqtstrm, after enter [F6]t
 



#include <math.h>
#include <heport.h>
#include <helq.h>
#include <hemath.h>
#include <heimath.h>
#include <tshar.h>
#include <hevt.h>
#include <hegettim.h>
#include <tsser.h>

#ifdef AVR
#include <avr/io.h>
#endif

#ifdef AVR
#define NSTK1 80
#else
#define NSTK1 2000
#endif

void *stk1[NSTK1] ;		// 00
#define STACK_EMPTY 0x7EAB	// for Stackcheck - command z

uint16_t counter ;
#ifdef AVR
		// The stack must not go over a 0x100 page limit ?
#if ( stk1 & 0xff00 ) != ( stk1+NSTK1 & 0xff00 )
#error "stk1 over page (256) boundary"
#endif
#if ( stk2 & 0xff00 ) != ( stk2+NSTK2 & 0xff00 )
#error "stk2 over page (256) boundary"
#endif
#endif

extern struct sdate_uc date_uc ;
//	  uint32_t day ;
//	  uint8_t hour ;
//	  uint8_t minute ;
//	  uint8_t second ;


float ha ;		// 0x00
float hb ;		// 0x00
float hr1 ;	// 0x00
float hr2 ;	// 0x00
float hr3 ;	// 0x00
float hr4 ;	// 0x00
float xx ;		// 0x00


// Output all 4 bytes of a float as hexadecimal (without losses) serial
FUNCT void ohex8( float *p)
{
uint8_t i;
uint8_t *pc ;

pc = (uint8_t *)p ; 
for( i=0;i<4;++i)
	{
	tsserhx2_out(*pc++);	// Serial out one byte as 2 hex characters
	}
}





// Perform a float calculation
FUNCT void calculate(char c)
{
switch ( c )
	{
case '+' :
	hr1 = ha + hb ;
	break ;
case '-' :
	hr1 = ha - hb ;
	break ;
case '*' :
	hr1 = ha * hb ;
	break ;
case '/' :
	hr1 = ha / hb ;
	break ;
case 's' :
	hr1 = sin(ha) ;
	break ;
case 'c' :
	hr1 = cos(ha) ;
	break ;
case 't' :
	hr1 = tan(ha) ;
	break ;
case 'u' :
	hr1 = (float)(uint32_t)(ha) ;
	break ;
case 'v' :
	hr1 = (float)(int32_t)(ha) ;
	break ;
default:
	hr1 = ha ;		
	}
}


// The dialog function
FUNCT void fun1(void)
{
char c ;
uint8_t i ;
uint8_t *pc ;
uint is ;

for(;;)
	{
	tsserputs("\nqqtsser $");
	c = tsserichaw() ;
	tsserochaw(c);
	switch ( c )
		{
	case '?' :
		tsserputs(
			"\nMenu is: \n"
			"? - Get this help\n"
			"a - Arithmetic Test\n"
			"h - Arithmetic Test (HEX)\n"
			"z - Stack-Test\n"
				);
		break ;
	case 'a' :
		c = tsserichaw() ;
		tsserochaw(c);
		tsserfloat_dec( "\nInput ha ?", &ha );
		tsserfloat_dec( "\nInput hb ?", &hb );

		calculate(c);

		tsserputs( "\n ha= " );
		tsserdec_fl_out(ha);
		tsserputs( " hb= " );
		tsserdec_fl_out(hb);
		tsserputs( " Operation= " );
		tsserochaw(c);
		tssernl();

		tsserputs( "\nResult =" ); tsserdec_fl_out(hr1);
	
		tssernl();

		break ;

	case 'h' :		// This hexadecimale test is server for the PC qqtstrm.cpp
		c = tsserichaw() ;
		tsserochaw(c);
		tsserputs( "\nInput ha ?" );
		pc = (uint8_t *)&ha ;
 		for( i=0;i<4;++i) tsseruchar_hex2(pc++);
		tsserputs( "\nInput hb ?" );
		pc = (uint8_t *)&hb ;
 		for( i=0;i<4;++i) tsseruchar_hex2(pc++);

		calculate(c);

		tsserputs( "\n ha= " );
		tsserdec_fl_out(ha);
		tsserputs( " hb= " );
		tsserdec_fl_out(hb);
		tssernl();

		tsserputs( "\nResult =" ); ohex8(&hr1);
	
		tssernl();

		break ;

	case 'z' :
		for ( is = 0 ; is < NSTK1 ; ++is )
			{
			if ( stk1[is] != (void *)STACK_EMPTY )
				{
				break ;
				}
			}
		tsserputs( "\nStack1 unused =" );
		tsserdec_uw_out( is );
		tsserputs( " out of " );
		tsserdec_uw_out( NSTK1 );
		tssernl();
		break ;

	default:
		tsserputs( "\n??? Help=?\n" );
		break ;
		}
	tsnext("fun1");
	}
}



// The main program
FUNCT int main( int argc, const char **argv )
{
uint is ;

//2286
tsseropen();

for ( is = 0 ; is < NSTK1 ; ++is )
	{
	stk1[is] = (void *)STACK_EMPTY ;
	}

tsopen();		// Clear all tables

tseinri( 1 , fun1, "fun1", stk1, sizeof(stk1) );

for(;;)
	{
	tsnext( "main" );	// run one piece of fun1 and fun2
	}

return 0 ;		// return - will never return !
}

