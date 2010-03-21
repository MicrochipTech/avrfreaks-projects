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

#define WORD		UCHAR

#endif

