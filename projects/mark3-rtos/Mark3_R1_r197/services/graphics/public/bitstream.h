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

#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

//---------------------------------------------------------------------------
class BitStreamer
{
public:
    /*!
     * \brief Init
     *
     * Initialize the BitStreamer object prior to use
     *
     * \param pucData_ Pointer to raw data to be streamed
     * \param usSize_ Size of pucData_ in bytes
     */
    void Init(K_UCHAR *pucData_, K_USHORT usSize_);

    /*!
     * \brief AdvanceByte
     *
     * Advance byte index to the next full byte if the current bit index
     * is non-zero.  If the current bit index is zero, no action is taken.
     * This is used to byte-align 2-dimensional data, such as images.
     */
    void AdvanceByte(void);

    /*!
     * \brief ReadBits
     *
     * Read the next "n" bits from the stream, returning the result into an
     * 8-bit unsigned integer.
     *
     * \param ucNumBits_ Number of bits to read (less than 8)
     * \return Bits read as an 8-bit unsigned integer.
     */
    K_UCHAR ReadBits(K_UCHAR ucNumBits_);

private:
    K_UCHAR *m_pucData;     //!< Pointer to the data being streamed
    K_UCHAR m_ucBitIndex;   //!< Current "bit" index in the current byte
    K_USHORT m_usByteIndex;	//!< Current "byte" index in the stream
    K_USHORT m_usSize;      //!< Length of data (in bytes)
};

#endif // __BITSTREAM_H__
