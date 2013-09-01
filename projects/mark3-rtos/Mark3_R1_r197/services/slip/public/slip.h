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
    \file slip.h

    Serial Line IP framing code.  Also includes code to frame data in 
    FunkenSlip format for use with SlipTerm on a host PC.  
    
    FunkenSlip uses SLIP-framed messages with a pre-defined packet format 
    as follows:
    
    [ Channel ][ Size ][ Data Buffer ][ CRC8 ]
    
    Channel is 1 byte, indicating the type of data carried in the message

    Size is 2 bytes, indicating the length of the binary blob that follows

    Data Buffer is n bytes, and contains the raw packet data.

    CRC16 is 2 byte, Providing an error detection mechanism
*/

#include "kerneltypes.h"
#include "driver.h"

#ifndef __SLIP_H__
#define __SLIP_H__

//---------------------------------------------------------------------------
typedef enum
{
    SLIP_CHANNEL_TERMINAL = 0,    //!< ASCII text mode terminal
    SLIP_CHANNEL_UNISCOPE,        //!< Uniscope VM command channel
    SLIP_CHANNEL_NVM,            //!< Non-volatile memory configuration
    SLIP_CHANNEL_RESET,            //!< Channel used to reset the device...
    SLIP_CHANNEL_GRAPHICS,      //!< Encoded drawing commands    
    SLIP_CHANNEL_HID,            //!< HID commands
//---    
    SLIP_CHANNEL_COUNT
} SlipChannel;

//---------------------------------------------------------------------------
/*!
    Data structure used for vector-based SLIP data transmission.  Allows for
    building and transmitting complex data structures without having to copy
    data into intermediate buffers.
*/
typedef struct  
{
    K_UCHAR ucSize;             //!< Size of the data buffer
    K_UCHAR *pucData;           //!< Pointer to the data buffer
}SlipDataVector;

//---------------------------------------------------------------------------
/*!
    Object used to frame communications over an abstract device using
    the serial-line internet protocol (SLIP).
*/
class Slip
{
public:
    /*!
        Set the driver to attach to this object.
        
        \param pclDriver_ Pointer to the driver to attach
    */
    void SetDriver( Driver *pclDriver_ ){ m_pclDriver = pclDriver_; }
    
    /*!
        Return the pointer to the driver attached to this object
        
        \return Pointer to the driver attached
    */
    Driver *GetDriver() { return m_pclDriver; }

    /*!
        \fn K_USHORT EncodeByte( K_UCHAR ucChar_, K_UCHAR *aucBuf_ )    
        
        Encode a single byte into a stream, returning the size of
        the encoded value (either 1 or 2 bytes).
    
        \param ucChar_ Character to encode
        \param aucBuf_ Buffer to encode into
        
        \return # bytes read
    */
    static K_USHORT EncodeByte( K_UCHAR ucChar_, K_UCHAR *aucBuf_ );

    /*!
        \fn K_USHORT DecodeByte( K_UCHAR *ucChar_, const K_UCHAR *aucBuf_ )
        
        Decode a byte from a stream into a specified value
        
        Returns    the number of bytes from the source array that were processed,
        (1 or 2), or 0 if an end-of-packet (192) was encountered.
        
        
        \param ucChar_ Destination K_CHAR
        \param aucBuf_ Source buffer
        
        \return # bytes read, or 0 on terminating character (192)
    */
    static K_USHORT DecodeByte( K_UCHAR *ucChar_, const K_UCHAR *aucBuf_ );
    
    /*!
        \fn void WriteData( K_UCHAR ucChannel_, const K_CHAR *aucBuf_, K_UCHAR ucLen_ )
        
        Write a packet of data in the FunkenSlip format.
        
        Returns    the number of bytes from the source array that were processed,
        (1 or 2), or 0 if an end-of-packet (192) was encountered.
        
        \param ucChannel_ Channel to encode the packet to
        \param aucBuf_ Payload to encode
        \param usLen_ Length of payload data
    */
    void WriteData( K_UCHAR ucChannel_, const K_CHAR *aucBuf_, K_USHORT usLen_ );

    /*!
        \fn K_USHORT ReadData( K_UCHAR *pucChannel_, K_CHAR *aucBuf_, K_USHORT usLen_ )
        
        Read a packet from a specified device, parse, and copy to a specified 
        output buffer.
        
        \param pucChannel_ Pointer to a uchar that stores the message channel
        \param aucBuf_ Buffer where the message will be decoded
        \param usLen_ Length of the buffer to decode
        
        \return data bytes read, 0 on failure.
    */
    K_USHORT ReadData( K_UCHAR *pucChannel_, K_CHAR *aucBuf_, K_USHORT usLen_ );

    /*!
        \fn void WriteVector( K_UCHAR ucChannel_, SlipDataVector_t *astData, K_USHORT usLen_ )
        
        Write a single message composed of multiple data-vector fragments.  
        
        Allows for transmitting complex data structures without requiring buffering.
        This operation is zero-copy.
        
        \param ucChannel_ Message channel
        \param astData_ Pointer to the data vector 
        \param usLen_ Number of elements in the data vector
    */
    void WriteVector( K_UCHAR ucChannel_, SlipDataVector *astData_, K_USHORT usLen_ );
    
    /*!
        \fn void SendAck()        
        
        Send an acknowledgement character to the host
    */
    void SendAck();

    /*!
        \fn void SendNack()
        
        Send a negative-acknowledgement character to the host
    */
    void SendNack();
    
private:
    void WriteByte(K_UCHAR ucData_);    
    Driver *m_pclDriver;    
};

#endif
