//**************************************************************************************
///	File Name	: \file zoError.h 
///	Description	: \brief Byte ring buffer used for error storage.
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
/// \defgroup zoError zoError.h : Byte ring buffers used for error storage.
/// 
/// \par Overview
/// This library has an identical implementation with the zoBuffer library. Naming is the
/// main difference. These 'special' ring buffers with a predefined fixed size of 8 bytes
/// are used to store errors that occur during execution of any zoAvrLib .c file. In this 
/// way the error handling code is more apparent to the reader. This implementation was
/// chosen because of the ability to safely handle errors that might be recognized in 
/// ISRs.See the error handling section in the documentation of zoAvrLib, or the way 
/// zoError functions are used in zoAvrLib source code for more info.
///
/// \par Usage example 
/// \code
/// #include "MyLibrary.h"
/// #include "zoError.h"
/// #include "zoTypes.h"
///
/// #define SOMETHING_WENT_WRONG_ERROR_CODE 0x01
///
/// ZO_ERROR MyError = ZO_ERROR_DEFAULTS;    //declare a global error buffer
///
/// void myLibraryInit(void)
/// {
/// 	zoErrorInit(&MyError);				//first thing to do is always  initialization
/// 	doThis();
/// 	doThat();
/// }
///
/// bool myFunction(void)
/// {
/// 	if( somethingWentWrong() )
/// 	{
/// 		zoErrorPut(&MyError, SOMETHING_WENT_WRONG_ERROR_CODE);
/// 		return false;
/// 	}
/// return true;
/// }
/// \endcode 
//****************************************************************************************
//@{

#ifndef ZO_ERROR_H
#define ZO_ERROR_H

#include "zoTypes.h"

//Types and defaults______________________________________________________________________
///Size of an error buffer
#define ZO_ERROR_STORAGE_SIZE 8

///Error buffer type
typedef volatile struct {
	volatile u08 	ctr;							///< count of bytes in the buffer 
	volatile u08* 	pIn;							///< pointer to next incoming byte
	volatile u08*	pOut;							///< pointer to next outgoing byte
	volatile u08	error[ZO_ERROR_STORAGE_SIZE];	///< byte storage
}ZO_ERROR;

///Default error buffer initializer
#define ZO_ERROR_DEFAULTS {0,0,0,{0,0,0,0,0,0,0,0}}

//Function declarations___________________________________________________________________
/*! \brief Initializes the error buffer. Always call this function before calling any 
	other function of this library.
    \param *p Pointer to the error buffer variable. */
void zoErrorInit(ZO_ERROR *p);

/*! \brief Gets the 'oldest' error code stored in the error buffer. 
    \param *p Pointer to the error buffer variable. */
u08 zoErrorGet(ZO_ERROR *p);

/*! \brief Puts an error code in the error buffer. 
    \param *p Pointer to the error buffer variable. 
	\param error The error code to be stored in the error buffer */
void zoErrorPut(ZO_ERROR *p,const u08 error);

/*! \brief Checks if the error buffer is empty. 
    \param *p Pointer to the error buffer variable.
    \return true if the error buffer is empty, false if it isn't */
bool zoErrorIsEmpty(ZO_ERROR *p);

///Unprotected inline version of zoErrorGet() for use in ISRs
u08 zoErrorGetIsr(ZO_ERROR *p);

///Unprotected inline version of zoErrorPut() for use in ISRs
void zoErrorPutIsr(ZO_ERROR *p,const u08 error);

#endif //ZO_ERROR_H
//@}
