//**************************************************************************************
///	File Name	: \file zoString.h 
///	Description	: \brief Conversion of strings to integers and vice versa.
//	Created		: 21/01/2010
//	Target MCU	: all
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
/// \defgroup zoString zoString.h : Conversion of strings to integers and vice versa.
/// 
/// \par Overview
/// This library provides functionality for converting strings to any kind of integer and
///	vice versa. The string-integer union types declared in zoTypes.h are utilized to 
/// produce fast and efficient code.
///   
/// \par Usage example 
/// \code
/// #include "zoTypes.h"
/// #include "zoString.h"
/// #include "zoUart.h"
///
/// int main(void)
/// {
/// 	u08 str[10];							//declare a byte array
/// 	u64 ageOfTheEarthInYears = 4540000000;	//the age of the earth in years!
/// 	zoUartInit();							//initialize uart to 115200,8,N,1
/// 											//refer to the uart documentation
///
/// 	//send ageOfTheEarthInYears over the uart byte by byte (not digit by digit)
/// 	zoUartPutData(u64ToStr(ageOfTheEarthInYears,str),8);
/// }
/// \endcode 
//****************************************************************************************
//@{

#ifndef ZO_STRING_H
#define ZO_STRING_H

#include "zoTypes.h"

/*! \brief Converts (or rather interprets) an array of two bytes to an unsigned 16-bit
	integer
    \param *str Pointer to the byte array. 
	\return The unsigned 16-bit integer*/
inline u16 strToU16(u08* str)
{
	U16 *num;
	num = (U16*)str;
	return num->all;
}

/*! \brief Converts an array of two bytes to a signed 16-bit integer
    \param *str Pointer to the byte array. 
	\return The signed 16-bit integer*/
inline s16 strToS16(u08* str)
{
	S16* num;
	num = (S16*)str;
	return num->all;
}

/*! \brief Converts an array of four bytes to an unsigned 32-bit integer
    \param *str Pointer to the byte array. 
	\return The unsigned 32-bit integer*/
inline u32 strToU32(u08* str)
{
	U32 *num;
	num = (U32*)str;
	return num->all;
}

/*! \brief Converts an array of four bytes to a signed 32-bit integer
    \param *str Pointer to the byte array. 
	\return The signed 32-bit integer*/
inline s32 strToS32(u08* str)
{
	S32 *num;
	num = (S32*)str;
	return num->all;
}

/*! \brief Converts an array of eight bytes to an unsigned 64-bit integer
    \param *str Pointer to the byte array. 
	\return The unsigned 64-bit integer*/
inline u64 strToU64(u08* str)
{
	U64 *num;
	num = (U64*)str;
	return num->all;
}

/*! \brief Converts an array of eight bytes to a signed 64-bit integer
    \param *str Pointer to the byte array. 
	\return The signed 64-bit integer*/
inline s64 strToS64(u08* str)
{
	S64 *num;
	num = (S64*)str;
	return num->all;
}

/*! \brief Converts an unsigned 16-bit integer to an array of two bytes
    \param data The unsigned 16-bit integer to convert.
	\param *str Pointer to the user provided byte array. 
	\return Pointer to the byte array. */
inline u08* u16ToStr(u16 data, u08* str)
{
	U16* num;
	num = (U16*)str;
	num->all = data;
	return str;
}

/*! \brief Converts a signed 16-bit integer to an array of two bytes
    \param data The signed 16-bit integer to convert.
	\param *str Pointer to the user provided byte array. 
	\return Pointer to the byte array. */
inline u08* s16ToStr(s16 data, u08* str)
{
	S16* num;
	num = (S16*)str;
	num->all = data;
	return str;
}

/*! \brief Converts an unsigned 32-bit integer to an array of four bytes
    \param data The unsigned 32-bit integer to convert.
	\param *str Pointer to the user provided byte array. 
	\return Pointer to the byte array. */
inline u08* u32ToStr(u32 data, u08* str)
{
	U32* num;
	num = (U32*)str;
	num->all = data;
	return str;
}

/*! \brief Converts a signed 32-bit integer to an array of four bytes
    \param data The signed 32-bit integer to convert.
	\param *str Pointer to the user provided byte array. 
	\return Pointer to the byte array. */
inline u08* s32ToStr(s32 data, u08* str)
{
	S32* num;
	num = (S32*)str;
	num->all = data;
	return str;
}

/*! \brief Converts an unsigned 64-bit integer to an array of eight bytes
    \param data The unsigned 64-bit integer to convert.
	\param *str Pointer to the user provided byte array. 
	\return Pointer to the byte array. */
inline u08* u64ToStr(u64 data, u08* str)
{
	U64* num;
	num = (U64*)str;
	num->all = data;
	return str;
}

/*! \brief Converts a signed 64-bit integer to an array of eight bytes
    \param data The signed 64-bit integer to convert.
	\param *str Pointer to the user provided byte array. 
	\return Pointer to the byte array. */
inline u08* s64ToStr(s64 data, u08* str)
{
	S64* num;
	num = (S64*)str;
	num->all = data;
	return str;
}

#endif //ZO_STRING_H
//@}