/*-------------------------------------------------------------------------
;			automatic baud rate detection
;-------------------------------------------------------------------------
;
; recognize any byte,
; which contain 1 * bit time low, followed by 4 * bit times low
;                      ____    __    __ __             ____
;e.g. recognize 0x0D:      |__|  |__|  |  |__|__|__|__|
;                          0  1  2  3     5           9
;                                1*T               4*T
*/
#include "main.h"

#define	TOLERANCE	10U


u16 abaud ( void )
{
  u8 time_h;
  u16 time;
  s16 time0;
  u8 i;

  time = 0;
  for(;;){
    time0 = time<<2;			// previous time
    time_h = 0;
    TCCR1 = 0;				// stop T1
    TIFR = 0xFF;			// clear overflow flag
    TCNT1 = 0;                          // clear T1
    while( get_rxd() == 1 );		// wait until RXD = 0
    TCCR1 = 1<<CS10;			// start T1 with XTAL/1
    do{
      if( (i = TIFR) & 1<<TOV1 ){
	TIFR = i;                       // clear T1 overflow flag
	if( ++time_h == 0 )		// count overflow
	  TCCR1 = 0;            	// stop T1 = no more overflows
      }
    }while( get_rxd() == 0 );		// wait until RXD = 1
    TCCR1 = 0;				// stop T1
    if( TIFR & 1<<TOV1 )
      ++time_h;				// count overflow
    time = (time_h<<8) + TCNT1;		// make 16 bit timer value
    if( time_h == 0 )			// to short or to long ?
      continue;
    time0 -= time;                      // 4 * t0 - t1
    do{
      time0 >>= 1;			// relative error
    }while( time_h >>= 1 );
    if( time0 + TOLERANCE <= 2*TOLERANCE )	// inside tolerance
      break;
  }
  return (time + 2) >> 2;		// 1 * bit time
}
