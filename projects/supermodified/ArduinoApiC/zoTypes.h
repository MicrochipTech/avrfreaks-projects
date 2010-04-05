#ifndef ZO_TYPES_H
#define ZO_TYPES_H

//****************************************************************************************
//	File Name	: zoTypes.h 
//	Description	: type definitions
//	Created		: 25.1.2010
//	Target MCU	: Atmel AVR
//	Author		: Sissakis Giannis
//  email		: info@01mech.com
//
//  Copyright (C) 2010 Zero One Mechatronics LP
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************************************

#include <stdint.h>

#define FALSE	0
#define TRUE	-1
  
#ifndef NULL
#define NULL 0
#endif

//common datatype definitions macros
typedef int8_t  	bool;
typedef uint8_t		u08;
typedef int8_t  	s08;
typedef uint16_t 	u16;
typedef int16_t		s16;
typedef uint32_t	u32;
typedef int32_t 	s32;
typedef uint64_t	u64;
typedef int64_t 	s64;

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

typedef union {
	u08 byte[2];
	u16 all;
}U16;

typedef union {
	u08 byte[2];
	s16 all;
}S16;

typedef union {
	u08 byte[4];
	u32 all;
}U32;

typedef union {
	u08 byte[4];
	s32 all;
}S32;

typedef union {
	u08 byte[8];
	u64 all;
}U64;

typedef union {
	u08 byte[8];
	s64 all;
}S64;

#endif /*ZO_TYPES_H*/
