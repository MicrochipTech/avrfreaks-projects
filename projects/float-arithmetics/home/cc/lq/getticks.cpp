// getusec.cc
// get the number of mikroseconds, maximum time difference is 1 minute !

#include <heport.h>
#include <helq.h>
#include <hegettim.h>

#if defined( AVR )
#include <avr/io.h>
#endif

#if defined( MSVC40 ) || defined( MDOS40 )
#include <mapidefs.h>	// FILETIME ...
#endif



#ifdef OSK
FUNCT	uint32_t	gettick_disabled( void ) /* get count of microseconds after midnight */
{
int atime ;
int adate ;
short aday ;
int atick ;	 /* Highbyte : tick per second , Lowbyte : current clock tick */
uint32_t tick256 ;

_sysdate( 3, &atime, &adate, &aday, &atick );
tick256 = ( atime * 256		   /* seconds after midnight */
			+ ( ( atick & 0x7FFF ) * 256 / ( ( atick >> 16 ) & 0x7FFF ) ) );
// return 1000000L * tick256 / 256L ;	  /* Milliseconds after midnight */
return 15625L * ( tick256 % (256L*60L) ) / 64L ;	 /* Milliseconds after midnight */
}
#endif // OSK




#if defined( __TURBOC__ )

FUNCT	uint16_t	getticks_disabled( void ) /* get count of microseconds after midnight */
{
uint8_t ctrlow , ctrhigh ;	// content of 16 bit counter
uint16_t ctr ;

outp( 0x43, 0 );	// Latch counter
ctrlow	= inp( 0x40 );
ctrhigh = inp( 0x40 );

ctr = ~ (( ctrhigh << 8 ) + ctrlow) ;

return ctr ;

}
#endif									// __TURBOC__



#if defined( AVR )


// Timer1
// 2^16 = 65536
// 65536 * 8 / 16.000000 MHz = 32768 usec
uint16_t lastcnt1 ;	// The last read value
uint16_t actcnt1 ;		// the actual read value
uint16_t countextender ;

FUNCT void tickopen(void)
{
// Initialize the Timer
TCCR1A = 0 ;		// Bit 7,6 : no connection to output line
					// Bit 1,0 : no PWM operation
TCCR1B = 2 ;		//
					// Bit 7 : 0 = No Input noise capture
					// Bit 6 : Edge Select
					// Bit 3 : 0 = No Clear on compare
					// Bit 2,1,0 : 001 = CK/1
					//			   010 = CK/8 <<<
					//			   011 = CK/64
					//			   100 = CK/256
					//			   101 = CK/1024
}


FUNCT uint32_t gettick( void )
{
uint8_t oldstat ;
union {
	uint32_t ul ;
	struct {
		uint16_t lw ;
		uint16_t hw ;
		} w ;
	} retval ;
oldstat = SREG ;
SREG &= ~(1<<SREG_I) ;	// Clear the Interrupt Enable bit
actcnt1 = TCNT1 ;		// Read the 16 bit value ( 2MHz ) from counter 1
if ( actcnt1 < lastcnt1 )
	{					// once per 65536 * 0.5 us --- once per 32768 usec
	++countextender ;
	}
retval.w.lw = lastcnt1 = actcnt1 ;
retval.w.hw = countextender ;
SREG = oldstat ;
return retval.ul ;
}


#endif


// /////////////////////////////////////////////////////////////
