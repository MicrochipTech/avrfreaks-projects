
#include <math.h>

#define NPOLY 6

float sin22p5( float x )	//  0.0 <= x <= 45.0 degrees expressed as 0...1.0
{
static float apolys[6]= 
	{
      0.0000000991797373F ,   // apolys[0]
      0.7853934175074619F ,   // apolys[1]
      0.0000476319890092F ,   // apolys[2]
     -0.0809275910626443F ,   // apolys[3]
      0.0003064938782596F ,   // apolys[4]
      0.0022868429710194F     // apolys[5]
	} ;
float y ;
uint i ;

y = apolys[NPOLY-1] ;
for ( i = NPOLY-1 ; i ; )
	{
	--i ;
	y *= x ;
	y += apolys[i] ;
	}

return y ;
}



float cos22p5( float x )	//  0.0 <= x <= 45.0 degrees expressed as 0...1.0
{
static float apolyc[6]= 	// calculated and verified by qqsin.cpp
	{
      1.0000002593768582F ,   // apolyc[0]
     -0.0000128513146372F ,   // apolyc[1]
     -0.3082901664733330F ,   // apolyc[2]
     -0.0005533289585200F ,   // apolyc[3]
      0.0169103635060012F ,   // apolyc[4]
     -0.0009472414122206F     // apolyc[5]
	} ;
float y ;
uint i ;

y = apolyc[NPOLY-1] ;
for ( i = NPOLY-1 ; i ; )
	{
	--i ;
	y *= x ;
	y += apolyc[i] ;
	}

return y ;
}





#ifdef AVR

inline float fneg( float x )
{
asm( "subi	%D0,0x80\n" 
: "=%d" (x) 			// Output
: "d" (x)				// Input
:  );	// Clobber
return x;
}
#else
inline double fneg( double x )
{
return -x ;
}
#endif


// This sine routine splits the circle in 8 parts, each quadrant in two parts
float sin( float x )	// x as radians
{
// requires for hr1 = sin(-10.0) ; 4519 cycles =282 usec
float w, r ;
uint32_t u ;
bool neg ;
uint8_t segment ;
float y ;

neg = false ;
r = x * 0.15915494309189533576888376337251F ; 	// 1.0/(2.0*cPI) ;
if ( r < 0.0F ) 
	{
	neg = true ;
	r = fneg(r) ;
	}
u = (uint32_t)r ;
w = r - u ;					// 0...360 degrees = 0.0.99999 
w = fmulpow2( w, 3 );			// w = w * 8.0F ;
segment = (uint8_t)(uint32_t)w ;		// 0...7

switch ( segment )
	{
case 0 :  y =  sin22p5( w           ); break ;
case 1 :  y =  cos22p5( 2.0F - w    ); break ;
case 2 :  y =  cos22p5( w    - 2.0F ); break ;
case 3 :  y =  sin22p5( 4.0F - w    ); break ;
case 4 :  y = -sin22p5( w    - 4.0F ); break ;
case 5 :  y = -cos22p5( 6.0F - w    ); break ;
case 6 :  y = -cos22p5( w    - 6.0F ); break ;
default : y = -sin22p5( 8.0F - w    ); break ;
	}
if ( neg ) y = fneg(y) ;
return y ;
}


float cos( float x )	// x as radians
{
return sin( (float)(x+1.5707963267948966192313216916398F) ) ;	// PI / 2.0
}





float tan( float x )	// x as radians
{
return sin( x ) / cos( x ) ;
}




