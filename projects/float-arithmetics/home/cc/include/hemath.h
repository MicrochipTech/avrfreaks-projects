#ifndef __hemath_h__
#define __hemath_h__
// #include <math.h>

const double cPI = 3.141592653589793 ;
const double cPI2 = 2.0 * cPI ;
const double cPI05 = 0.5 * cPI ;
const double cE = 2.718281828459045 ;
const double cLOG2 = 0.69314718055994529 ;

#define nearPI05(x) ( ( x > cPI05-0.00000001 ) && ( x < cPI05+0.00000001 ) )
#define sqr(x) ((x)*(x))
extern double powdd( double base, double exponent );

const double c_dINVALID = 3.40282e+038 ;
const float	 c_fINVALID = 3.40282e+038F ;


#define float_MAX 340.282346E36F
#define float_MIN -340.282346E36F

#ifdef AVR
extern float fmulpow2( float x, sint i );
#else
inline float fmulpow2( float x, sint i )
{
if ( i > 0 ) return x * ( 1 << i );
if ( i < 0 ) return x / ( 1 << i );
return x ;
}
#endif




#endif // #ifndef __hemath_h__

