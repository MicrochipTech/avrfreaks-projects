/*
  inttypes.h

  Description:
  Macros, typedefs and other type-related information for the application.
*/


#ifndef INTTYPES_H
#define INTTYPES_H


/* MACROS */


/* "The COSMIC TRUTHS!" */
#define FALSE (0)
#define TRUE  (!FALSE)



/*
  MAXxBITS:
  Macros defining maximum bit lengths of various integer types as
  follows:

  MAXCBITS: char
  MAXSBITS: short
  MAXLBITS: long
  MAXIBITS: int

  In each case, it is assumed that sizeof() evaluates to the number of
  8-bit bytes for a given type.  So the result is multiplied by 8
  to evaluate the bit size.
*/
#define MAXCBITS (8*(sizeof(char)))
#define MAXSBITS (8*(sizeof(short)))
#define MAXLBITS (8*(sizeof(long)))
#define MAXIBITS (8*(sizeof(int)))


/* TYPEDEFS */
typedef          long long int64;
typedef          long  int32;
typedef          short int16;
typedef          char  int8;

typedef unsigned long long uint64;
typedef unsigned long  uint32;
typedef unsigned short uint16;
typedef unsigned char  uint8;

typedef unsigned int   uint;

#endif /* INTTYPES_H */
