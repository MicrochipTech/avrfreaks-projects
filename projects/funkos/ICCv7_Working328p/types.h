//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
#ifndef __TYPES_H_
#define __TYPES_H_

#define LONG	long
#define ULONG	unsigned long
#define SHORT	short
#define USHORT	unsigned short
#define BYTE	char
#define CHAR	char
#define UCHAR	unsigned char
#define INT		int
#define FLOAT	float
#define DOUBLE	double

#ifndef BOOL
	#define BOOL	BYTE
#endif
#ifndef TRUE
	#define TRUE	1
#endif
#ifndef FALSE
	#define FALSE	0
#endif

#ifndef NULL
	#define NULL	((void*)0)
#endif

//!! WORD definition is platform specific.  Architecture symbol must be defined in the build
#ifdef ARCH_ARM_C3
	#define WORD		ULONG
#endif

#ifdef ARCH_AVR
	#define WORD		UCHAR
#endif

#ifdef ARCH_MSP430
	#define WORD		USHORT
#endif

#endif

