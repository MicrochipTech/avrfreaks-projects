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

#include "rand_lfsr.h"

//---------------------------------------------------------------------------
#define POLY_MASK_32 0xB4BCD35C
#define POLY_MASK_31 0x7A5BC2E3

//---------------------------------------------------------------------------
PseudoRandom::PseudoRandom()
{
    m_ulLFSR1 = 0xABCDE;
    m_ulLFSR2 = 0x23456789;
}

//---------------------------------------------------------------------------
void PseudoRandom::Seed( K_ULONG ulSeed1_, K_ULONG ulSeed2_ )
{
    m_ulLFSR1 = ulSeed1_;
    m_ulLFSR2 = ulSeed2_;
}

//---------------------------------------------------------------------------
K_USHORT PseudoRandom::GetRandom()
{
    Shift(&m_ulLFSR2, POLY_MASK_32);
    return (K_USHORT)( Shift(&m_ulLFSR2, POLY_MASK_32) ^
                       Shift(&m_ulLFSR1, POLY_MASK_31) );
}

//---------------------------------------------------------------------------
K_ULONG PseudoRandom::Shift( K_ULONG *pulLFSR_, K_ULONG ulMask_ )
{
    K_ULONG ulFeedback;

    ulFeedback = *pulLFSR_ & 1;
    *pulLFSR_ >>= 1;
    if (ulFeedback)
    {
        *pulLFSR_ ^= ulMask_;
    }
    return *pulLFSR_;
}
