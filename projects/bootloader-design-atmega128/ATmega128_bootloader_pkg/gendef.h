
/*****************************************************************************************
*
*   gendef.h
*
*   GENERIC DEFINITIONS FOR EMBEDDED C APPLICATIONS
*
*/
#ifndef _GENDEF_H_
#define _GENDEF_H_


#ifndef TRUE
#define FALSE    0
#define TRUE     (!FALSE)
#endif

#ifndef NULL
#define NULL     ((void *) 0)
#endif

#define  until(exp)  while(!(exp))   /* Usage:  do { ... } until (exp);  */

/*
*   Some of these typedefs may need to be changed so that the data sizes implied by the
*   names of the generic types, int8, int16, etc, are correct for the target processor.
*   Use uint16 and uint32 in preference to ushort and ulong (resp) in new projects.
*/
typedef signed char         int8;
typedef unsigned char       uint8,  uchar;

typedef signed short        int16;
typedef unsigned short      uint16, ushort;

typedef signed long         int32;
typedef unsigned long       uint32, ulong;

#ifndef bool
typedef unsigned char       bool;
#endif

typedef void (* pfnvoid)(void);     // pointer to void function


/*
*   Macros for byte/word/register bit manipulation.
*   NB: TEST_BIT(...) evaluates to *any* non-zero value to indicate TRUE.
*/
#define  TEST_BIT(entity, bitmask)   ((entity) & (bitmask))
#define  SET_BIT(entity, bitmask)    ((entity) |= (bitmask))
#define  CLEAR_BIT(entity, bitmask)  ((entity) &= ~(bitmask))

#define  BIT_0   0x01     /*** BIT MASKS ***/
#define  BIT_1   0x02
#define  BIT_2   0x04
#define  BIT_3   0x08
#define  BIT_4   0x10
#define  BIT_5   0x20
#define  BIT_6   0x40
#define  BIT_7   0x80
#define  BIT_8   0x0100
#define  BIT_9   0x0200
#define  BIT_10  0x0400
#define  BIT_11  0x0800
#define  BIT_12  0x1000
#define  BIT_13  0x2000
#define  BIT_14  0x4000
#define  BIT_15  0x8000

/*
*   Other useful macros...
*/
#ifdef  LITTLE_ENDIAN   /* If bytes of unsigned word are in little-endian order... */
#define SWAP(w)   (w)   /* ... no change  */
#else                   /* If big-endian ... swap order of bytes */
#define SWAP(w)   ((((w) & 0xFF) << 8) | (((w) >> 8) & 0xFF))
#endif

#define HI_BYTE(w)  (((w) >> 8) & 0xFF)   /* Extract high-order byte from unsigned word */
#define LO_BYTE(w)  ((w) & 0xFF)          /* Extract low-order byte from unsigned word */

#define LESSER_OF(arg1,arg2) ((arg1)<=(arg2)?(arg1):(arg2))

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))


/***** Commonly used symbollic constants *****/

#define  ERROR   (-1)
#define  FAIL    (-1)
#define  SUCCESS   0
#define  OFF       0
#define  ON        1
#define  DISABLE   0
#define  ENABLE    1

#define  NUL             0        /* Not to be confused with NULL */
#define  SPACE          32

#define  ASCII_NUL       0
#define  ASCII_ACK       6
#define  ASCII_BS        8        /* Ctrl+H, Backspace */
#define  ASCII_HT        9
#define  ASCII_TAB       9
#define  ASCII_LF       10
#define  ASCII_CR       13
#define  ASCII_DC2      18        /* Ctrl+R, Device Control 2 */
#define  ASCII_NAK      21
#define  ASCII_CAN      24        /* Ctrl+X, Cancel line */
#define  ASCII_ESC      27
#define  ASCII_SP       32


#endif  /*  _GENDEF_H_  */
