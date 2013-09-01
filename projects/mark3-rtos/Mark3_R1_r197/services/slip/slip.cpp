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
    \file slip.cpp
    \brief Serial Line IP framing code.
*/

#include "kerneltypes.h"
#include "slip.h"
#include "driver.h"

//---------------------------------------------------------------------------
#define FRAMING_BYTE            (192)   //!< Byte indicating end-of-frame
#define FRAMING_ENC_BYTE        (219)   //!< Byte used to indicate substitution
#define FRAMING_SUB_BYTE        (220)   //!< Byte to substitute for framing byte
#define FRAMING_SUB_ENC_BYTE    (221)   //!< Byte to substitute for the substitute-byte

//---------------------------------------------------------------------------
#define ACchar                (69)    //!< Acknowledgement character
#define NACchar               (96)    //!< Non-acknowledgement character

//---------------------------------------------------------------------------
K_USHORT Slip::EncodeByte( K_UCHAR ucChar_, K_UCHAR *aucBuf_ )
{
    K_USHORT usLen = 1;
    switch (ucChar_)
    {
        case FRAMING_BYTE:
            aucBuf_[0] = FRAMING_ENC_BYTE;
            aucBuf_[1] = FRAMING_SUB_BYTE;
            usLen = 2;
            break;
        case FRAMING_ENC_BYTE:
            aucBuf_[0] = FRAMING_ENC_BYTE;
            aucBuf_[1] = FRAMING_SUB_ENC_BYTE;
            usLen = 2;
            break;
        default:
            aucBuf_[0] = ucChar_;
    }
    return usLen;
}

//---------------------------------------------------------------------------
K_USHORT Slip::DecodeByte( K_UCHAR *ucChar_, const K_UCHAR *aucBuf_ )
{
    K_USHORT usLen = 1;
    
    if (aucBuf_[0] == FRAMING_ENC_BYTE)
    {
        if(aucBuf_[1] == FRAMING_SUB_BYTE)
        {
            *ucChar_ = FRAMING_BYTE;
            usLen = 2;
        }
        else if(aucBuf_[1] == FRAMING_SUB_ENC_BYTE)
        {
            *ucChar_ = FRAMING_ENC_BYTE;
            usLen = 2;
        }
        else
        {
            *ucChar_ = 0;
            usLen = 0;
        }
    }
    else if (aucBuf_[0] == FRAMING_BYTE)
    {
        usLen = 0;
        *ucChar_ = 0;
    }
    else
    {
        *ucChar_ = aucBuf_[0];
    }
    return usLen;
}

//---------------------------------------------------------------------------
void Slip::WriteByte( K_UCHAR ucData_)
{
    K_USHORT usSize = 0;
    K_USHORT usIdx = 0;
    K_UCHAR aucBuf[2];
    usSize = EncodeByte(ucData_, aucBuf);
    while (usIdx < usSize)
    {
        usIdx += m_pclDriver->Write(usSize, &aucBuf[usIdx]);    
    }
}

//---------------------------------------------------------------------------
K_USHORT Slip::ReadData(K_UCHAR *pucChannel_, K_CHAR *aucBuf_, K_USHORT usLen_)
{
    K_USHORT usReadCount;
    K_UCHAR ucTempCount;
    K_USHORT usValid = 0;
    K_USHORT usCRC;
    K_USHORT usCRC_Calc = 0;
    K_USHORT usLen;
    K_UCHAR *pucSrc = (K_UCHAR*)aucBuf_;
    K_UCHAR *pucDst = (K_UCHAR*)aucBuf_;
    
    usReadCount = m_pclDriver->Read(usLen_, (K_UCHAR*)aucBuf_);

    while (usReadCount)
    {
        K_UCHAR ucRead;
        ucTempCount = DecodeByte(&ucRead, pucSrc);
    
        *pucDst = ucRead;
        
        // Encountered a FRAMING_BYTE - end of message
        if (!ucTempCount) 
        {
            break;
        }
        
        // Add to the CRC
        usCRC_Calc += ucRead;
        
        // Adjust iterators, source, and destination pointers.
        usReadCount -= ucTempCount;
        pucSrc += ucTempCount;
        pucDst++;
        usValid++;
    }

    // Ensure we have enough data to try a match.
    if (usValid < 5) {
        return 0;
    }
    
    usCRC_Calc -= aucBuf_[usValid-2];
    usCRC_Calc -= aucBuf_[usValid-1];
    
    usLen = ((K_USHORT)aucBuf_[1]) << 8;
    usLen += ((K_USHORT)aucBuf_[2]);
    usCRC = ((K_USHORT)aucBuf_[usValid-2]) << 8;
    usCRC += ((K_USHORT)aucBuf_[usValid-1]);
    
    if (usCRC != usCRC_Calc)
    {
        return 0;
    }
    
    *pucChannel_ = aucBuf_[0];

    return usLen;
}

//---------------------------------------------------------------------------
void Slip::WriteData(K_UCHAR ucChannel_, const K_CHAR *aucBuf_, K_USHORT usLen_)
{
    K_UCHAR aucTmp[2];    
    K_USHORT usCRC = 0;

    // Lightweight protocol built on-top of SLIP.
    // 1) Channel ID (8-bit)
    // 2) Data Size (16-bit)
    // 3) Data blob
    // 4) CRC16 (16-bit)
    aucTmp[0] = FRAMING_BYTE;
    while( !m_pclDriver->Write(1, aucTmp) ) {}
                    
    if (!usLen_)    // Read to end-of-line (\0)
    {
        K_UCHAR *pucBuf = (K_UCHAR*)aucBuf_;
        while (*pucBuf != '\0')
        {
            usLen_++;
            pucBuf++;
        }                
    }
    
    WriteByte(ucChannel_);
    usCRC = ucChannel_;

    WriteByte((K_UCHAR)(usLen_ >> 8));
    usCRC += (usLen_ >> 8);

    WriteByte((K_UCHAR)(usLen_ & 0x00FF));
    usCRC += (usLen_ & 0x00FF);
        
    while (usLen_--)
    {
        WriteByte(*aucBuf_);
        usCRC += (K_USHORT)*aucBuf_;
        aucBuf_++;    
    }
    
    WriteByte((K_UCHAR)(usCRC >> 8));
    WriteByte((K_UCHAR)(usCRC & 0x00FF));
    
    aucTmp[0] = FRAMING_BYTE;
    while( !m_pclDriver->Write(1, aucTmp) ) {}
}

//---------------------------------------------------------------------------
void Slip::SendAck()
{
    WriteByte(ACchar);
}

//---------------------------------------------------------------------------
void Slip::SendNack()
{
    WriteByte(NACchar);
}

//---------------------------------------------------------------------------
void Slip::WriteVector(K_UCHAR ucChannel_, SlipDataVector *astData_, K_USHORT usLen_)
{
    K_UCHAR aucTmp[2];    
    K_USHORT usCRC = 0;
    K_UCHAR i, j;
    K_USHORT usTotalLen = 0;
    
    // Calculate the total length of all message fragments
    for (i = 0; i < usLen_; i++)
    {        
        usTotalLen += astData_[i].ucSize;
    }    
    
    // Send a FRAMING_BYTE to start framing a message
    aucTmp[0] = FRAMING_BYTE;
    while( !m_pclDriver->Write(1, aucTmp) ) {}
    
    // Write a the channel
    WriteByte(ucChannel_);
    usCRC = ucChannel_;

    // Write the length
    WriteByte((K_UCHAR)(usTotalLen >> 8));
    usCRC += (usTotalLen >> 8);

    WriteByte((K_UCHAR)(usTotalLen & 0x00FF));    
    usCRC += (usTotalLen & 0x00FF);
        
    // Write the message fragments
    for (i = 0; i < usLen_; i++)
    {
        K_UCHAR *aucBuf = astData_[i].pucData;
        for (j = 0; j < astData_[i].ucSize; j++ )
        {
            WriteByte(*aucBuf);
            usCRC += (K_USHORT)*aucBuf;
            aucBuf++;
        }        
    }
    
    // Write the CRC 
    WriteByte((K_UCHAR)(usCRC >> 8));
    WriteByte((K_UCHAR)(usCRC & 0x00FF));
    
    // Write the end-of-message
    aucTmp[0] = FRAMING_BYTE;
    while( !m_pclDriver->Write(1, aucTmp) ) {}
}
