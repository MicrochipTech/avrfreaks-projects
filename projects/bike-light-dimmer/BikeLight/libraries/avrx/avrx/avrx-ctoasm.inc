/*
 C to ASM

*/
#ifndef __AVRX_CTOASM__
#define __AVRX_CTOASM__

#ifdef __IAR_SYSTEMS_ASM__

#  define p1l     R16
#  define p1h     R17
#  define p2l     R20
#  define p2h     R21
#  define r1l     R16
#  define r1h     R17

#  define tmp0    R18
#  define tmp1    R19
#  define tmp2    R22
#  define tmp3    R23
#  define tmp4    R2
#  define tmp5    R3

#else   /* avr-gcc */

#  define p1l     R24
#  define p1h     R25
#  define p2l     R22
#  define p2h     R23
#  define r1l     R24
#  define r1h     R25

#  define tmp0    R18
#  define tmp1    R19
#  define tmp2    R20
#  define tmp3    R21
#  define tmp4    R26   /* X */
#  define tmp5    R27   /* X */

#endif

#endif /* __AVRX_CTOASM__ */
