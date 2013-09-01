/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!

    \file   atomic.h

    \brief  Basic Atomic Operations
*/

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

#include "kerneltypes.h"
#include "mark3cfg.h"
#include "threadport.h"

#if KERNEL_USE_ATOMIC

/*!
 * \brief The Atomic class
 *
 * This utility class provides primatives for atomic operations - that is,
 * operations that are guaranteed to execute uninterrupted.  Basic atomic
 * primatives provided here include Set/Add/Delete for 8, 16, and 32-bit
 * integer types, as well as an atomic test-and-set.
 *
 */
class Atomic
{
public:
    /*!
     * \brief Set Set a variable to a given value in an uninterruptable operation
     * \param pucSource_ Pointer to a variable to set the value of
     * \param ucVal_ New value to set in the variable
     * \return Previously-set value
     */
    static K_UCHAR Set( K_UCHAR *pucSource_, K_UCHAR ucVal_ );
    static K_USHORT Set( K_USHORT *pusSource_, K_USHORT usVal_ );
    static K_ULONG Set( K_ULONG *pulSource_, K_ULONG ulVal_ );

    /*!
     * \brief Add Add a value to a variable in an uninterruptable operation
     * \param pucSource_ Pointer to a variable
     * \param ucVal_ Value to add to the variable
     * \return Previously-held value in pucSource_
     */
    static K_UCHAR Add( K_UCHAR *pucSource_, K_UCHAR ucVal_ );
    static K_USHORT Add( K_USHORT *pusSource_, K_USHORT usVal_ );
    static K_ULONG Add( K_ULONG *pulSource_, K_ULONG ulVal_ );

    /*!
     * \brief Sub Subtract a value from a variable in an uninterruptable operation
     * \param pucSource_ Pointer to a variable
     * \param ucVal_ Value to subtract from the variable
     * \return Previously-held value in pucSource_
     */
    static K_UCHAR Sub( K_UCHAR *pucSource_, K_UCHAR ucVal_ );
    static K_USHORT Sub( K_USHORT *pusSource_, K_USHORT usVal_ );
    static K_ULONG Sub( K_ULONG *pulSource_, K_ULONG ulVal_ );

    /*!
     * \brief TestAndSet Test to see if a variable is set, and set it if
     *        is not already set.  This is an uninterruptable operation.
     *
     *        If the value is false, set the variable to true, and return
     *        the previously-held value.
     *
     *        If the value is already true, return true.
     *
     * \param pbLock Pointer to a value to test against.  This will always
     *        be set to "true" at the end of a call to TestAndSet.
     *
     * \return true - Lock value was "true" on entry, false - Lock was set
     */
    static K_BOOL TestAndSet( K_BOOL *pbLock );
};

#endif // KERNEL_USE_ATOMIC

#endif //__ATOMIC_H__
