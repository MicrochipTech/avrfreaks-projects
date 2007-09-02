/* Lutzies Macros.
   All you need which doesn't need bread
   */

#ifndef __LUTZMACRO
#define __LUTZMACRO   

#define swap(x) asm volatile("swap %0" : "=r" ((x)) : "0" ((x)));

typedef unsigned char u08;
typedef unsigned char  u8;
typedef   signed char  s8;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned long  u32;
typedef   signed long  s32;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong; // max: 18 446 744 073 709 551 616

#define max(x,y) ((x)>(y)?(x):(y))
#define min(x,y) ((x)>(y)?(y):(x)) 

#endif
