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
#include "mark3cfg.h"
#include "kerneltypes.h"
#include "bitstream.h"

//---------------------------------------------------------------------------
void BitStreamer::Init(K_UCHAR *pucData_, K_USHORT usSize_)
{
    m_pucData = pucData_;
    m_ucBitIndex = 0;
    m_usByteIndex = 0;
    m_usSize = 0;
}

//---------------------------------------------------------------------------
void BitStreamer::AdvanceByte(void)
{
    if (m_ucBitIndex != 0)
    {
        m_ucBitIndex = 0;
        m_usByteIndex++;
    }
}

//---------------------------------------------------------------------------
K_UCHAR BitStreamer::ReadBits(K_UCHAR ucNumBits_)
{
    K_UCHAR ucDat = m_pucData[m_usByteIndex];
    K_UCHAR ucRet = 0;
    K_UCHAR ucIterBits;
    K_UCHAR ucShift = 0;

    while (ucNumBits_)
    {
        // Check to see whether or not the bitstream read will past the end
        // of the current byte's index.
        if (ucNumBits_ < (8 - m_ucBitIndex))
        {
            // If not, read everything all in one iteration
            ucIterBits = ucNumBits_;
        }
        else
        {
            // Otherwise, read what we can in this iteration.
            ucIterBits = (8 - m_ucBitIndex);
        }

        // Shift the bits to be read in this iteration to the lowest bits in the variable
        ucDat = ((ucDat >> m_ucBitIndex) & ((1 << ucIterBits) - 1));

        // Shift into the output variable in the correct index
        ucRet += (ucDat << ucShift);

        // Update the bit index (and byte index, if we've reached the end of the word)
        m_ucBitIndex += ucNumBits_;
        if (m_ucBitIndex == 8)
        {
            m_ucBitIndex = 0;
            m_usByteIndex++;
        }

        // Subtract the bits we've read from the total remaining
        ucNumBits_ -= ucIterBits;

        // If there are bits yet to be read
        if (ucNumBits_)
        {
            // Figure out how much to shift the current return value by
            ucShift += ucIterBits;

            // Read the next byte of data to use in the next iteration
            ucDat = m_pucData[m_usByteIndex];
        }
    }
    return ucRet;
}
