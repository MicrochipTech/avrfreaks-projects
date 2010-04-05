//**************************************************************************************
///	File Name	: \file zoTypes.h 
///	Description	: \brief Types used in zoAvrLib
//	Created		: 21/01/2010
//	Target MCU	: AVR
//	Author		: Sissakis Giannis
//  email		: info@01mech.com
//
//  Copyright (C) 2010 Zero One Mechatronics LP
//
//  This program is free software: you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation, 
//  version 3 of the License or any later version. This program is distributed in the hope
//  that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public 
//  License for more details: <http://www.gnu.org/licenses/>
//  
/// \ingroup soft 
/// \defgroup zoTypes zoTypes.h : Types commonly used in zoAvrLib 
/// 
/// \par Overview
/// This file contains type definitions for the types used by zoAvrLib. 
///   
//****************************************************************************************
//@{

#ifndef ZO_TYPES_H
#define ZO_TYPES_H

#include <stdint.h>

//common defines__________________________________________________________________________

#ifndef FALSE
/// gcc style false definition
#define FALSE	0
#endif

#ifndef TRUE
/// gcc style true definition
#define TRUE	(-1)
#endif

#ifndef NULL
/// gcc null definition
#define NULL 	0
#endif

#ifndef false
/// microsoft style false definition
#define false	0
#endif

#ifndef true
/// microsoft style true definition
#define true	(-1)
#endif

#ifndef null
/// microsoft style null definition
#define null 	0
#endif
  
//common datatype definitions_____________________________________________________________
typedef int8_t  	bool;	///< boolean
typedef uint8_t	u08;	///< unsigned 8-bit integer
typedef int8_t  	s08;	///< signed 8-bit integer
typedef uint16_t 	u16;	///< unsigned 16-bit integer
typedef int16_t	s16;	///< signed 16-bit integer
typedef uint32_t	u32;	///< unsigned 32-bit integer
typedef int32_t 	s32;	///< signed 32-bit integer
typedef uint64_t	u64;	///< unsigned 64-bit integer
typedef int64_t 	s64;	///< signed 64-bit integer

//minimums/maximums_______________________________________________________________________
//maximum value that can be held by unsigned data types (8,16,32bits)
#define MAX_U08	255			///< maximum value for unsigned 8-bit integer
#define MAX_U16	65535		///< maximum value for unsigned 16-bit integer
#define MAX_U32	4294967295	///< maximum value for unsigned 32-bit integer

//maximum values that can be held by signed data types (8,16,32bits)
#define MIN_S08	-128		///< minimum value for unsigned 8-bit integer
#define MAX_S08	127			///< maximum value for unsigned 8-bit integer
#define MIN_S16	-32768		///< minimum value for unsigned 8-bit integer
#define MAX_S16	32767		///< maximum value for unsigned 8-bit integer
#define MIN_S32	-2147483648	///< minimum value for unsigned 8-bit integer
#define MAX_S32	2147483647	///< maximum value for unsigned 8-bit integer

//Usefull unions of types with byte arrays________________________________________________
///union of two bytes with 16-bit unsigned integer
typedef union {
	u08 byte[2];			///< byte array
	u16 all;				///< unsigned 16-bit integer
}U16;

///union of two bytes with 16-bit signed integer
typedef union {
	u08 byte[2];			///< byte array
	s16 all;				///< signed 16-bit integer
}S16;

///union of four bytes with 32-bit unsigned integer
typedef union {
	u08 byte[4];			///< byte array
	u32 all;				///< unsigned 32-bit integer
}U32;

///union of four bytes with 32-bit signed integer
typedef union {
	u08 byte[4];			///< byte array
	s32 all;				///< signed 32-bit integer
}S32;

///union of eight bytes with 64-bit unsigned integer
typedef union {
	u08 byte[8];			///< byte array
	u64 all;				///< unsigned 64-bit integer
}U64;

///union of eight bytes with 64-bit signed integer
typedef union {
	u08 byte[8];			///< byte array
	s64 all;				///< signed 64-bit integer
}S64;

#endif /*ZO_TYPES_H*/
//@}
