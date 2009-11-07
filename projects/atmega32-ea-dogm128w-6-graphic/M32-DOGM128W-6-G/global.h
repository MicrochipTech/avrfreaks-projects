/*

	global function


*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include <avr/pgmspace.h>
#include "delay.h"


#define FALSE	0
#define TRUE	-1
 
// datatype definitions macros
typedef unsigned char  u08;
typedef   signed char  s08;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned long  u32;
typedef   signed long  s32;
typedef unsigned long long u64;
typedef   signed long long s64;

// maximum value that can be held
// by unsigned data types (8,16,32bits)
#define MAX_U08	255
#define MAX_U16	65535
#define MAX_U32	4294967295

// maximum values that can be held
// by signed data types (8,16,32bits)
#define MIN_S08	-128
#define MAX_S08	127
#define MIN_S16	-32768
#define MAX_S16	32767
#define MIN_S32	-2147483648
#define MAX_S32	2147483647

	// more type redefinitions
typedef unsigned char   BOOL;
typedef unsigned char	BYTE;
typedef unsigned int	WORD;
typedef unsigned long	DWORD;
typedef unsigned char	UCHAR;
typedef unsigned int	UINT;
typedef unsigned short  USHORT;
typedef unsigned long	ULONG;

typedef char			CHAR;
typedef int				INT;
typedef long			LONG;


#define	outb(addr, data)	addr = (data)
#define	inb(addr)			(addr)
#define	outw(addr, data)	addr = (data)
#define	inw(addr)			(addr)
#define BV(bit)			(1<<(bit))
#define cbi(reg,bit)	reg &= ~(BV(bit))
#define sbi(reg,bit)	reg |= (BV(bit))
#define cli()			__asm__ __volatile__ ("cli" ::)
#define sei()			__asm__ __volatile__ ("sei" ::)
// port address helpers
#define DDR(x) ((x)-1)    // address of data direction register of port x
#define PIN(x) ((x)-2)    // address of input register of port x
// MIN/MAX/ABS macros
#define MIN(a,b)			((a<b)?(a):(b))
#define MAX(a,b)			((a>b)?(a):(b))
#define ABS(x)				((x>0)?(x):(-x))
// constants
#define PI		3.14159265359
#define CLOCK	8000000


#define CYCLES_PER_US ((CLOCK+500000)/1000000) 	// cpu cycles per microsecond
