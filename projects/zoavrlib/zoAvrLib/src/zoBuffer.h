//**************************************************************************************
///	File Name	: \file zoBuffer.h 
///	Description	: \brief Ring byte buffer function library.
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
/// \defgroup zoBuffer zoBuffer.h : Byte ring buffers.
/// 
/// \par Overview
/// This library provides byte buffering functionality. The buffer is protected from
/// data corruption that may occur by the interference of ISR code, thus it can be 
/// declared at local or global scope with no worries.
///   
/// \par Usage example 
/// \code
/// #include "zoTypes.h"
/// #include "zoBuffer.h"
///
/// ZO_BUFFER MyBuffer = ZO_BUFFER_DEFAULTS;    //declare a global buffer
///
/// int main(void)
/// {
///     u08 c;
///       
///     zoBufferInit(&MyBuffer,10); 		//initialize the buffer to a size of ten bytes
///        
///     zoBufferPut(&MyBuffer,'i'); 		//put the char i into the buffer
/// 
///     if(!zoBufferIsFull())   			//check if the buffer is full; if not:
///         zoBufferPut(&MyBuffer,23);		//put the number 23 into the buffer
///
///     if(!zoBufferIsEmpty())  			//check if the buffer is empty; if not:
///         c = zoBufferGet(&MyBuffer); 	//get the 'oldest' byte stored in the buffer
/// 									    //thus 'i' is assigned to c.
/// }
/// \endcode 
//****************************************************************************************
//@{

#ifndef ZO_BUFFER_H
#define ZO_BUFFER_H

#include "zoTypes.h"

//Types and defaults______________________________________________________________________
///Buffer type
typedef volatile struct {
	volatile u08 size;	///< Size of the ring buffer
	volatile u08 ctr;	///< how many bytes are currently in the buffer
	u08* pIn;			///< where to put next incoming byte
	u08* pOut;			///< from where to take next outgoing byte
	u08 *data;			///< pointer to byte storage
}ZO_BUFFER;

///Default ZO_BUFFER variable initializer.
#define ZO_BUFFER_DEFAULTS {0,0,0,0,0}

//Function declarations___________________________________________________________________
/*! \brief Initializes a ZO_BUFFER variable. Always initialize a buffer variable with this
	function before calling any other function. 
    \param *p A pointer to the buffer variable.
    \param  size  The maximum number of bytes the buffer can store.
    \return success of memory allocation for the buffer */
bool zoBufferInit(ZO_BUFFER *p,u08 size);           

/*! \brief Stores a byte in the buffer. Check for a space in the buffer with 
             zoBufferIsFull() function. 
    \param *p Pointer to the buffer variable.
    \param  data  The byte to store.*/
void zoBufferPut(ZO_BUFFER *p,const u08 data);

/*! \brief Gets a byte from the buffer. Check if there are any data in the buffer with 
    the zoBufferIsEmpty() function.
    \param *p Pointer to the buffer variable.
    \return the 'oldest' byte stored last in the buffer. If the buffer is empty the byte
            returned is garbage.*/
u08  zoBufferGet(ZO_BUFFER *p);

/*! \brief Reads a byte in the buffer at specific index. The zoBufferRead() function does 
    not take the byte out of he buffer like the zoBufferGet() function does.
    \param *p Pointer to the buffer variable.
    \param  index  Where to read the byte in the buffer.Index is relative to the 'oldest' 
    byte stored in the buffer. Thus index=0 means the 'oldest' byte in the buffer, index=1 
    the next and so on. If index is greater than the buffer size the return byte is 
    garbage.
    \return the byte read. */
u08  zoBufferRead(ZO_BUFFER *p,u08 index);

/*! \brief Checks if the buffer is empty. 
    \param *p Pointer to the buffer variable.
    \return true if the buffer is empty, false if it isn't. */
bool zoBufferIsEmpty(ZO_BUFFER *p);

/*! \brief Checks if the buffer is full. 
    \param *p Pointer to the buffer variable.
    \return true if the buffer is full, false if it isn't */
bool zoBufferIsFull(ZO_BUFFER *p);

/*! \brief Flushes the buffer. All data in the buffer are discarded.
    \param *p Pointer to the buffer variable. */
void zoBufferFlush(ZO_BUFFER *p);

//!Inline unprotected version of zoBufferPut() for use in ISRs
void zoBufferPutIsr(ZO_BUFFER *p,const u08 data);

//!Inline unprotected version of zoBufferGet() for use in ISRs
u08  zoBufferGetIsr(ZO_BUFFER *p);

//!Inline unprotected version of zoBufferRead() for use in ISRs
u08	 zoBufferReadIsr(ZO_BUFFER *p,u08 index);

#endif //ZO_BUFFER_H
//@}
