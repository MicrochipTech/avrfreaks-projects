#ifndef __heimath_h__
#define __heimath_h__
// Herbert's (hjherbert@t-online.de) integer math header and prototypes

#include <heport.h>		// defintion of uint32_t ...

uint32_t gcd(uint32_t x, uint32_t y);	// greatest common divisor
uint32_t scm(uint32_t x, uint32_t y);	// smallest common multiple

uint16_t lsqrt(uint32_t x);			// Square root of a long integer

extern uint32_t primelist[] ;		// list of prime values, end = 0

// factorize a number
int primfact(				// returns 0 if O.K.
		uint32_t x,			// this number
		uint32_t *pprimfact,	// save there
		uint maxprimfact,	// size of array
		uint *pnprimfact		// save actually found count of factors
		);

int16_t swmuldiv( int16_t x, int16_t y, int16_t d ) ;
uint16_t uwmuldiv( uint16_t x, uint16_t y, uint16_t d ) ;

extern "C" {
uint32_t mul_uw_uw( uint16_t x , uint16_t y );	// special optimized for ATMEL
int32_t mul_sw_sw( int16_t x , int16_t y );	// special optimized for ATMEL
 } ;

#define uchar_MAX 255
#define schar_MAX 127
#define schar_MIN -128

#define sword_MIN (int16_t)0x8000
#define sword_MAX (int16_t)0x7FFF
#define uword_MAX (uint16_t)0xFFFF


#if INTSIZE == 16
const sint sint_MAX = 0x7FFF ;
const uint uint_MAX = 0xFFFF ;
#else
// const sint sint_MAX = 0x7FFFFFFF ;
#define sint_MAX (sint)(0x7FFFFFFF)
#define uint_MAX (uint)(0xFFFFFFFF)
#endif

#define slong_MIN (int32_t )0x80000000L
#define slong_MAX (int32_t )0x7FFFFFFFL
#define ulong_MAX (uint32_t )0xFFFFFFFFL




#endif
