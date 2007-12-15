/////////////////////////////////////////////////////////////////////////////// 
//	Miscellanous HAL-like routines/defines
//	Version 1.0
//		(c) 2009-2011 sashman@gmail.com
//		Portions inspired by an unknown author on the internets.
/////////////////////////////////////////////////////////////////////////////// 
#ifndef MISC_H
#define MISC_H
///////////////////////////////////////////////////////////////////////////////
#define clear_bit(reg,bit)          (reg&=~(_BV(bit)))
#define set_bit(reg,bit)            (reg|=(_BV(bit)))
///////////////////////////////////////////////////////////////////////////////
#define MIN(A,B) (((A)>(B))?(B):(A))	
#define MAX(A,B) (((A)>(B))?(A):(B))
//don't do MIN(x++,y) unless sure of the consequences
///////////////////////////////////////////////////////////////////////////////
// helper defines
#define PORT_(port) PORT ## port 
#define DDR_(port)  DDR  ## port 
#define PIN_(port)  PIN  ## port 
#define PORT(port) PORT_(port) 
#define DDR(port)  DDR_(port) 
#define PIN(port)  PIN_(port)
#define sbp(P,BIT)   (set_bit(PORT(P), BIT))
#define cbp(P,BIT) (clear_bit(PORT(P), BIT))
///////////////////////////////////////////////////////////////////////////////
// usage:
// #define FOO_PORT A
// #define FOO_PIN	1
//...
//	out(FOO);	// sets pin for output in DDR register. Equivalent to
//				// DDRA |= _BV(DDA1);
//	set(FOO);	// enables pullup	/ logical one
//	clear(FOO);	// enables pulldown / logical zero
///////////////////////////////////////////////////////////////////////////////
#define set(WHAT)   (sbp(WHAT ## _PORT, WHAT ## _PIN))
#define clear(WHAT) (cbp(WHAT ## _PORT, WHAT ## _PIN))

#define out(WHAT) (set_bit(DDR(WHAT ## _PORT), WHAT ## _PIN))
#define in(WHAT)(clear_bit(DDR(WHAT ## _PORT), WHAT ## _PIN))
///////////////////////////////////////////////////////////////////////////////
#define OUT(P,B)	(set_bit(DDR(P), (DD ## P ## B)))
#define IN(P,B) 	(clear_bit(DDR(P), (DD ## P ## B)))
///////////////////////////////////////////////////////////////////////////////
#endif
///////////////////////////////////////////////////////////////////////////////
