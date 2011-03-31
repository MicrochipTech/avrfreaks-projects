/*H**************************************************************************
* $RCSfile: compiler.h,v $
*----------------------------------------------------------------------------
* Copyright (c) 2003 Atmel.
*----------------------------------------------------------------------------
* RELEASE:      $Name:  $
* FILE_REV:     $Revision: 1.13.2.23 $
* FILE_CVSID:   $Id: compiler.h,v 1.13.2.23 2006/03/13 18:38:02 rletendu Exp $
*----------------------------------------------------------------------------
* PURPOSE:
* This file redefines dedicated IAR AVR
* keywords in order to ensure that any source file can be processed by
* these compilers.
*****************************************************************************/

#ifndef _COMPILER_H_
#define _COMPILER_H_

/*_____ I N C L U D E S ____________________________________________________*/


/*_____ D E C L A R A T I O N S ____________________________________________*/
#define LITTLE_ENDIAN

#ifndef ASM_INCLUDE // define ASM_INCLUDE in your a51 source code before include of .h file
typedef float               Float16;

typedef unsigned char       U8 ;
typedef unsigned short      U16;
typedef unsigned long       U32;
typedef signed char         S8 ;
typedef short               S16;
typedef long                S32;
#if (defined __C51__)
typedef bit                 Bool;    // Shall be used with _MEM_TYPE_BIT_ to optimize the memory.
#else
typedef unsigned char       Bool;
#endif

typedef U8                  Status;
typedef Bool                Status_bool;
#define PASS 0
#define FAIL 1



#if (defined __C51__)
#  define _MEM_TYPE_BIT_              bdata  // Used for bit accesses
#  define _MEM_TYPE_FAST_              data
#  define _MEM_TYPE_MEDFAST_          idata
#  define _MEM_TYPE_MEDSLOW_          pdata
#  define _MEM_TYPE_SLOW_             xdata
#else
#  define _MEM_TYPE_BIT_
#  define _MEM_TYPE_FAST_
#  define _MEM_TYPE_MEDFAST_
#  define _MEM_TYPE_MEDSLOW_
#  define _MEM_TYPE_SLOW_
#endif

typedef unsigned char       Uchar;


typedef unsigned char       Uint8;
typedef unsigned int        Uint16;
typedef unsigned long int   Uint32;

typedef char                Int8;
typedef int                 Int16;
typedef long int            Int32;

typedef unsigned char       Byte;
typedef unsigned int        Word;
typedef unsigned long int   DWord;

typedef union
{
  Uint32 dw; // l changed in dw (double word) because l is used for signed long...
  Uint16 w[2];
  Uint8  b[4];
} Union32;

typedef union
{
  Uint16 w;
  Uint8  b[2];
} Union16;

#ifdef __IAR_SYSTEMS_ICC__
typedef char     bit;
typedef int      p_uart_ptchar;
typedef int      r_uart_ptchar;
#endif
#ifdef __CODEVISIONAVR__
typedef char     bit;
typedef int      p_uart_ptchar;
typedef char     r_uart_ptchar;
#endif
#if !defined(__IAR_SYSTEMS_ICC__) && !defined(___ICC__)
typedef char      p_uart_ptchar;
typedef char      r_uart_ptchar;
#endif

#endif

/**********************************************************************************/
/* codevision COMPILER (__CODEVISIONAVR__)                                                 */
/**********************************************************************************/
#ifdef __ICC__
#define _ConstType_	lit
#define _MemType_
#define _GenericType_ __generic
#define code lit
#define xdata
#define idata
#define data
#endif
/**********************************************************************************/
/* IAR COMPILER (__IAR_SYSTEMS_ICC__)                                             */
/**********************************************************************************/
#ifdef __IAR_SYSTEMS_ICC__
#include "inavr.h"
#define _ConstType_	__flash
#define _MemType_
#define _GenericType_ __generic
#define code __flash
#define xdata
#define idata
#define data
#define At(x) @ x
#define pdata
#define bdata

#define Enable_interrupt() __enable_interrupt()
#define Disable_interrupt() __disable_interrupt()
#endif


/* General purpose defines */
/*#define _ConstType_	__farflash
#define _MemType_
#define _GenericType_ __generic
#define code __farflash
#define xdata
#define idata
#define data*/




/*_____ M A C R O S ________________________________________________________*/
/* little-big endian management */
#define INTEL_ALIGNMENT     LITTLE_ENDIAN
#define MOTOROLA_ALIGNMENT  BIG_ENDIAN

// U16/U32 endian handlers
#ifdef LITTLE_ENDIAN     // => 16bit: (LSB,MSB), 32bit: (LSW,MSW) or (LSB0,LSB1,LSB2,LSB3) or (MSB3,MSB2,MSB1,MSB0)
#  define MSB(u16)        (((U8* )&u16)[1])
#  define LSB(u16)        (((U8* )&u16)[0])
#  define MSW(u32)        (((U16*)&u32)[1])
#  define LSW(u32)        (((U16*)&u32)[0])
#  define MSB0(u32)       (((U8* )&u32)[3])
#  define MSB1(u32)       (((U8* )&u32)[2])
#  define MSB2(u32)       (((U8* )&u32)[1])
#  define MSB3(u32)       (((U8* )&u32)[0])
#  define LSB0(u32)       MSB3(u32)
#  define LSB1(u32)       MSB2(u32)
#  define LSB2(u32)       MSB1(u32)
#  define LSB3(u32)       MSB0(u32)
#else // BIG_ENDIAN         => 16bit: (MSB,LSB), 32bit: (MSW,LSW) or (LSB3,LSB2,LSB1,LSB0) or (MSB0,MSB1,MSB2,MSB3)
#  define MSB(u16)        (((U8* )&u16)[0])
#  define LSB(u16)        (((U8* )&u16)[1])
#  define MSW(u32)        (((U16*)&u32)[0])
#  define LSW(u32)        (((U16*)&u32)[1])
#  define MSB0(u32)       (((U8* )&u32)[0])
#  define MSB1(u32)       (((U8* )&u32)[1])
#  define MSB2(u32)       (((U8* )&u32)[2])
#  define MSB3(u32)       (((U8* )&u32)[3])
#  define LSB0(u32)       MSB3(u32)
#  define LSB1(u32)       MSB2(u32)
#  define LSB2(u32)       MSB1(u32)
#  define LSB3(u32)       MSB0(u32)
#endif

// Endian converters
#define Le16(b)                        \
   (  ((U16)(     (b) &   0xFF) << 8)  \
   |  (     ((U16)(b) & 0xFF00) >> 8)  \
   )
#define Le32(b)                             \
   (  ((U32)(     (b) &       0xFF) << 24)  \
   |  ((U32)((U16)(b) &     0xFF00) <<  8)  \
   |  (     ((U32)(b) &   0xFF0000) >>  8)  \
   |  (     ((U32)(b) & 0xFF000000) >> 24)  \
   )

// host to network conversion: used for Intel HEX format, TCP/IP, ...
// Convert a 16-bit value from host-byte order to network-byte order
// Standard Unix, POSIX 1003.1g (draft)

#ifdef LITTLE_ENDIAN
#  define htons(a)    Le16(a)
#define ntohs(a)    htons(a)
#  define htonl(a)    Le32(a)
#define ntohl(a)    htonl(a)
#else
#define htons(a)    (a)
#define ntohs(a)    (a)
#define htonl(a)    (a)
#define ntohl(a)    (a)
#endif


// Constants
#define ENABLE   1
#define ENABLED  1
#define DISABLED 0
#define DISABLE  0
#define FALSE   (0==1)
#define TRUE    (1==1)

#define KO      0
#define OK      1
#define OFF     0
#define ON      1
#define NULL    0
#ifndef ASM_INCLUDE // define ASM_INCLUDE in your a51 source code before include of .h file
#define CLR     0
#define SET     1
#endif

/* Bit and bytes manipulations */
#define LOW(U16)                ((Uchar)U16)
#define HIGH(U16)               ((Uchar)(U16>>8))
#define TST_BIT_X(addrx,mask)   (*addrx & mask)
#define SET_BIT_X(addrx,mask)   (*addrx = (*addrx | mask))
#define CLR_BIT_X(addrx,mask)   (*addrx = (*addrx & ~mask))
#define OUT_X(addrx,value)      (*addrx = value)
#define IN_X(addrx)             (*addrx)

#  define Max(a, b)            ( (a)>(b) ? (a) : (b) )       // Take the max between a and b
#  define Min(a, b)            ( (a)<(b) ? (a) : (b) )       // Take the min between a and b

// Align on the upper value <val> on a <n> boundary
// i.e. Upper(0, 4)= 4
//      Upper(1, 4)= 4
//      Upper(2, 4)= 4
//      Upper(3, 4)= 4
//      Upper(4, 4)= 8
//      ../..
#  define Upper(val, n)        ( ((val)+(n)) & ~((n)-1) )

// Align up <val> on a <n> boundary
// i.e. Align_up(0, 4)= 0
//      Align_up(1, 4)= 4
//      Align_up(2, 4)= 4
//      Align_up(3, 4)= 4
//      Align_up(4, 4)= 4
//      ../..
#  define Align_up(val, n)     ( ((val)+(n)-1) & ~((n)-1) )

// Align down <val> on a <n> boundary
// i.e. Align_down(0, 4)= 0
//      Align_down(1, 4)= 0
//      Align_down(2, 4)= 0
//      Align_down(3, 4)= 0
//      Align_down(4, 4)= 4
//      ../..
#  define Align_down(val, n)   (  (val)        & ~((n)-1) )

/*M**************************************************************************
* NAME: Long_call
*----------------------------------------------------------------------------
* PARAMS:
* addr: address of the routine to call
*----------------------------------------------------------------------------
* PURPOSE:
* Call the routine at address addr: generate an Assembly LCALL addr opcode.
*----------------------------------------------------------------------------
* EXAMPLE:
* Long_call(0); // Software reset (if no IT used before)
*----------------------------------------------------------------------------
* NOTE:
* May be used as a long jump opcode in some special cases
*****************************************************************************/
#define Long_call(addr)         ((*(void (_ConstType_*)(void))(addr))())

/* {For Langdoc} */

/***********************************************************
 SET_SFR_BIT macro
  parameters
    sfr_reg : defined value in include file for sfr register
    bit_pos : defined value B_XX in include file for particular
              bit of sfr register
    bit_val : CLR / SET
************************************************************/
#define SET_SFR_BIT(sfr_reg, bit_pos, bit_val) { sfr_reg &= ~(1<<(bit_pos)); sfr_reg |= ((bit_val)<<(bit_pos));}

/***********************************************************
 bit_is_clear macro
  parameters
    PORT     : defined value in include file for sfr register
    POSITION : defined value in include file for particular
              bit of sfr register
  example : if (bit_is_clear(PORTB,PORTB3)) ...
************************************************************/
#define bit_is_clear(PORT,POSITION) ((PORT & (1<<POSITION)) == 0 )

/***********************************************************
 bit_is_set macro
  parameters
    PORT     : defined value in include file for sfr register
    POSITION : defined value in include file for particular
              bit of sfr register
  example : if (bit_is_set(PORTB,PORTB3)) ...
************************************************************/
#define bit_is_set(PORT,POSITION) ((PORT & (1<<POSITION)) != 0 )



/******************************************************************************/
/* IAR COMPILER                                                               */
/******************************************************************************/

#define __IOMACRO_H

#define TID_GUARD(proc) ((__TID__ & 0x7FF0) != ((90 << 8) | ((proc) << 4)))


/*----------------------------------------------------------------------------*/
#ifdef __IAR_SYSTEMS_ASM__
/*----------------------------------------------------------------------------*/

/* Byte sized SFRs */
#define SFR_B_BITS(_NAME,_ADDR,_A,_B,_C,_D,_E,_F,_G,_H)\
	sfrb	_NAME = _ADDR
#define SFR_B_BITS_EXT(_NAME,_ADDR,_A,_B,_C,_D,_E,_F,_G,_H)\
	sfrb	_NAME = _ADDR
#define SFR_B2_BITS(_NAME1,_NAME2,_ADDR,_A,_B,_C,_D,_E,_F,_G,_H)\
	ASMSFRB2 _NAME1, _NAME2, _ADDR

ASMSFRB2 MACRO
	sfrb	\1 = \3
	sfrb	\2 = \3
	ENDM


/* Word sized SFRs, needs to be expanded into an assembler macro first. */
#define SFR_W_BITS(_NAME, _ADDR, _A,_B,_C,_D,_E,_F,_G,_H, _I,_J,_K,_L,_M,_N,_O,_P)\
	ASMSFRW	_NAME, _ADDR

#define SFR_W_BITS_EXT(_NAME, _ADDR, _A,_B,_C,_D,_E,_F,_G,_H, _I,_J,_K,_L,_M,_N,_O,_P)\
	ASMSFRW	_NAME, _ADDR

ASMSFRW	MACRO
	sfrw	\1  = \2
	sfrb	\1L = (\2+0)
	sfrb	\1H = (\2+1)
	ENDM

#endif /* __IAR_SYSTEMS_ASM__ */

/*----------------------------------------------------------------------------*/
#ifdef __ICCAVR__
/*----------------------------------------------------------------------------*/
#define __BYTEBITS(_NAME,_A,_B,_C,_D,_E,_F,_G,_H) \
unsigned char _NAME ## _ ## _A:1, \
              _NAME ## _ ## _B:1, \
              _NAME ## _ ## _C:1, \
              _NAME ## _ ## _D:1, \
              _NAME ## _ ## _E:1, \
              _NAME ## _ ## _F:1, \
              _NAME ## _ ## _G:1, \
              _NAME ## _ ## _H:1;

#define SFR_B_BITS(_NAME, _ADDR, _A,_B,_C,_D,_E,_F,_G,_H) \
    __io union { \
      unsigned char   _NAME;           /* The sfrb as 1 byte */ \
      struct {                        /* The sfrb as 8 bits */ \
        __BYTEBITS(_NAME, _A,_B,_C,_D,_E,_F,_G,_H) \
      };  \
    } @ _ADDR;
#define SFR_B2_BITS(_NAME1, _NAME2, _ADDR, _A,_B,_C,_D,_E,_F,_G,_H) \
    __io union { \
      unsigned char   _NAME1;           /* The sfrb as 1 byte */ \
      unsigned char   _NAME2;           /* The sfrb as 1 byte */ \
      struct {                        /* The sfrb as 8 bits */ \
        __BYTEBITS(_NAME1, _A,_B,_C,_D,_E,_F,_G,_H) \
      };  \
      struct {                        /* The sfrb as 8 bits */ \
        __BYTEBITS(_NAME2, _A,_B,_C,_D,_E,_F,_G,_H) \
      };  \
    } @ _ADDR;
#define SFR_B_BITS_EXT(_NAME, _ADDR, _A,_B,_C,_D,_E,_F,_G,_H) \
    __tiny __no_init volatile union { \
      unsigned char   _NAME;           /* The sfrb as 1 byte */ \
      struct {                        /* The sfrb as 8 bits */ \
        __BYTEBITS(_NAME, _A,_B,_C,_D,_E,_F,_G,_H) \
      };  \
    } @ _ADDR;

#define SFR_W_BITS(_NAME, _ADDR, _A,_B,_C,_D,_E,_F,_G,_H, _I,_J,_K,_L,_M,_N,_O,_P) \
    __io union { \
      unsigned short  _NAME;  /* The sfrw as 1 short */ \
      struct {                /* The sfrw as 16 bits */ \
        __BYTEBITS(_NAME, _A,_B,_C,_D,_E,_F,_G,_H)   /* Bit names defined by user */  \
        __BYTEBITS(_NAME, _I,_J,_K,_L,_M,_N,_O,_P)   /* Bit names defined by user */  \
      };  \
      struct { /* The sfrw as 2 bytes */ \
        unsigned char _NAME ## L; \
        unsigned char _NAME ## H; \
      };  \
      struct {                          /* The sfrw as 2 x 8 bits */ \
        __BYTEBITS(_NAME ## L, Bit0,Bit1,Bit2,Bit3,Bit4,Bit5,Bit6,Bit7)  /* Bit names hard coded to 0-7 */ \
        __BYTEBITS(_NAME ## H, Bit0,Bit1,Bit2,Bit3,Bit4,Bit5,Bit6,Bit7)  /* Bit names hard coded to 0-7 */ \
      };  \
    } @ _ADDR;

#define SFR_W_BITS_EXT(_NAME, _ADDR, _A,_B,_C,_D,_E,_F,_G,_H, _I,_J,_K,_L,_M,_N,_O,_P) \
   __io union { \
    unsigned short  _NAME;   \
     struct {                /* The sfrw_ext as 16 bits */ \
        __BYTEBITS(_NAME, _A,_B,_C,_D,_E,_F,_G,_H)   /* Bit names defined by user */  \
        __BYTEBITS(_NAME, _I,_J,_K,_L,_M,_N,_O,_P)   /* Bit names defined by user */  \
      };  \
      struct { /* The sfrw _ext as 2 bytes */ \
        unsigned char _NAME ## L; \
        unsigned char _NAME ## H; \
      };  \
      struct {                          /* The sfrw_ext as 2 x 8 bits */ \
        __BYTEBITS(_NAME ## L, Bit0,Bit1,Bit2,Bit3,Bit4,Bit5,Bit6,Bit7)  /* Bit names hard coded to 0-7 */ \
        __BYTEBITS(_NAME ## H, Bit0,Bit1,Bit2,Bit3,Bit4,Bit5,Bit6,Bit7)  /* Bit names hard coded to 0-7 */ \
      };  \
    } @ _ADDR;

#endif

#define SFR_B(_NAME, _ADDR) SFR_B_BITS(_NAME, _ADDR, \
                                    Bit0,Bit1,Bit2,Bit3,Bit4,Bit5,Bit6,Bit7)
#define SFR_B2(_NAME1, _NAME2, _ADDR) SFR_B2_BITS(_NAME1, _NAME2, _ADDR, \
                                    Bit0,Bit1,Bit2,Bit3,Bit4,Bit5,Bit6,Bit7)
#define SFR_B_EXT(_NAME, _ADDR) SFR_B_BITS_EXT(_NAME, _ADDR, \
                                    Bit0,Bit1,Bit2,Bit3,Bit4,Bit5,Bit6,Bit7)

#define SFR_W(_NAME, _ADDR)  SFR_W_BITS(_NAME, _ADDR, \
                                    Bit0,Bit1,Bit2,Bit3,Bit4,Bit5,Bit6,Bit7, \
                                    Bit8,Bit9,Bit10,Bit11,Bit12,Bit13,Bit14,Bit15)

#define SFR_W_EXT(_NAME, _ADDR)  SFR_W_BITS_EXT(_NAME, _ADDR, \
                                    Bit0,Bit1,Bit2,Bit3,Bit4,Bit5,Bit6,Bit7, \
                                    Bit8,Bit9,Bit10,Bit11,Bit12,Bit13,Bit14,Bit15)

/******************************************************************************/
/* GCC COMPILER                                                               */
/******************************************************************************/
   #ifdef AVRGCC
#define _ConstType_	__flash
#define _MemType_
#define _GenericType_ __generic
#define code PROGMEM
#define xdata
#define idata
#define data
#define At(x) @ x
#define pdata
#define bdata
#define bit	U8
#define bool U8
   //#include <avr/sfr_defs.h>
   #include <avr/interrupt.h>
   #include <avr/pgmspace.h>
   #define Enable_interrupt() sei()
   #define Disable_interrupt() cli()

   #endif
#endif /* _COMPILER_H_ */

