/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2014 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file rand_lfsr.h
    \brief Linear Feedback Shift Register based Pseudo-Random Number Generator
*/

#ifndef __RAND_LFSR_H__
#define __RAND_LFSR_H_

#include "kerneltypes.h"

//---------------------------------------------------------------------------
/*!
 * \brief The PseudoRandom class
 *
 * Pseudro-Random Number Generator based on a 32-bit linear-feedback
 * shift register.  For reference, the implementation is based on the
 * Maxim IC application note "AN4000"
 *
 * http://www.maximintegrated.com/app-notes/index.mvp/id/4400
 *
 */

//---------------------------------------------------------------------------
class PseudoRandom
{
public:
    /*!
     * \brief PseudoRandom
     *
     * Default constructor
     */
    PseudoRandom();

    /*!
     * \brief Seed
     *
     * Initialize the LFSR registers in the object, seeding the random
     * number generator.
     *
     * \param ulSeed1_ Seed value for LFSR register 1
     * \param ulSeed2_ Seed value for LFSR register 2
     */
    void Seed( K_ULONG ulSeed1_, K_ULONG ulSeed2_ );

    /*!
     * \brief GetRandom
     *
     * Return a 16-bit random number.
     *
     * \return 16-bit random number
     */
    K_USHORT GetRandom();

private:

    /*!
     * \brief Shift
     *
     * Deep magic to drive the the LFSR state machine
     *
     * \param pulLFSR_ Pointer to the register to drive
     * \param ulMask_ Bitmask to apply as an XOR
     * \return new value in the LFSR register
     */
    K_ULONG Shift( K_ULONG *pulLFSR_, K_ULONG ulMask_ );

    K_ULONG m_ulLFSR1;
    K_ULONG m_ulLFSR2;

};

#endif

