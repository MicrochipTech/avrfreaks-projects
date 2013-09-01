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

    \file   memutil.h

    \brief  Utility class containing memory, string, and conversion routines.
*/

#ifndef __MEMUTIL_H__
#define __MEMUTIL_H__

#include "kerneltypes.h"
#include "mark3cfg.h"
#include "kernel_debug.h"

//---------------------------------------------------------------------------
/*!
    \brief Token descriptor struct format
*/
typedef struct
{
    const K_CHAR *pcToken;  //!< Pointer to the beginning of the token string
    K_UCHAR ucLen;          //!< Length of the token (in bytes)
} Token_t;

//---------------------------------------------------------------------------
/*!
    \brief String and Memory manipulation class.

    Utility method class implementing common memory and string manipulation
    functions, without relying on an external standard library implementation
    which might not be available on some toolchains, may be closed source,
    or may not be thread-safe.
*/
class MemUtil
{

public:

    //-----------------------------------------------------------------------
    /*!
        Convert an 8-bit unsigned binary value as a hexadecimal string.

        \fn static void DecimalToHex( K_UCHAR ucData_, char *szText_ )

        \param ucData_ Value to convert into a string
        \param szText_ Destination string buffer (3 bytes minimum)
    */
    static void DecimalToHex( K_UCHAR ucData_, char *szText_ );
    static void DecimalToHex( K_USHORT usData_, char *szText_ );
    static void DecimalToHex( K_ULONG ulData_, char *szText_ );

    //-----------------------------------------------------------------------
    /*!
        Convert an 8-bit unsigned binary value as a decimal string.

        \fn static void DecimalToString( K_UCHAR ucData_, char *szText_ )

        \param ucData_ Value to convert into a string
        \param szText_ Destination string buffer (4 bytes minimum)
    */
    static void DecimalToString( K_UCHAR ucData_, char *szText_ );
    static void DecimalToString( K_USHORT usData_, char *szText_ );
    static void DecimalToString( K_ULONG ulData_, char *szText_ );

    //-----------------------------------------------------------------------
    /*!
        Compute the 8-bit addative checksum of a memory buffer.

        \fn static K_USHORT Checksum8( const void *pvSrc_, K_USHORT usLen_ )

        \param pvSrc_ Memory buffer to compute a 8-bit checksum of.
        \param usLen_ Length of the buffer in bytes.
        \return 8-bit checksum of the memory block.
    */
    static K_UCHAR  Checksum8( const void *pvSrc_, K_USHORT usLen_ );

    //-----------------------------------------------------------------------
    /*!
        Compute the 16-bit addative checksum of a memory buffer.

        \fn static K_USHORT Checksum16( const void *pvSrc_, K_USHORT usLen_ )

        \param pvSrc_ Memory buffer to compute a 16-bit checksum of.
        \param usLen_ Length of the buffer in bytes.
        \return 16-bit checksum of the memory block.
    */
    static K_USHORT Checksum16( const void *pvSrc_, K_USHORT usLen_ );

    //-----------------------------------------------------------------------
    /*!
        Compute the length of a string in bytes.

        \fn static K_USHORT StringLength( const char *szStr_ )

        \param szStr_ Pointer to the zero-terminated string to calculate the length of
        \return length of the string (in bytes), not including the 0-terminator.

    */
    static K_USHORT StringLength( const char *szStr_ );

    //-----------------------------------------------------------------------
    /*!
        Compare the contents of two zero-terminated string buffers to eachother.

        \fn static bool CompareStrings( const char *szStr1_, const char *szStr2_ )

        \param szStr1_ First string to compare
        \param szStr2_ Second string to compare
        \return true if strings match, false otherwise.
    */
    static bool CompareStrings( const char *szStr1_, const char *szStr2_ );

    //-----------------------------------------------------------------------
    /*!
        Copy one buffer in memory into another.

        \fn static void CopyMemory( void *pvDst_, const void *pvSrc_, K_USHORT usLen_ )

        \param pvDst_ Pointer to the destination buffer
        \param pvSrc_ Pointer to the source buffer
        \param usLen_ Number of bytes to copy from source to destination
    */
    static void CopyMemory( void *pvDst_, const void *pvSrc_, K_USHORT usLen_ );

    //-----------------------------------------------------------------------
    /*!
        Copy a string from one buffer into another.

        \fn static void CopyString( char *szDst_, const char *szSrc_ )

        \param szDst_ Pointer to the buffer to copy into
        \param szSrc_ Pointer to the buffer to copy data from
    */
    static void CopyString( char *szDst_, const char *szSrc_ );

    //-----------------------------------------------------------------------
    /*!
        Search for the presence of one string as a substring within another.

        \fn static K_SHORT StringSearch( const char *szBuffer_, const char *szPattern_ )

        \param szBuffer_ Buffer to search for pattern within
        \param szPattern_ Pattern to search for in the buffer
        \return Index of the first instance of the pattern in the buffer, or -1 on no match.
    */
    static K_SHORT StringSearch( const char *szBuffer_, const char *szPattern_ );

    //-----------------------------------------------------------------------
    /*!
        Compare the contents of two memory buffers to eachother

        \fn static bool CompareMemory( void *pvMem1_, void *pvMem2_, K_USHORT usLen_ )

        \param pvMem1_ First buffer to compare
        \param pvMem2_ Second buffer to compare
        \param usLen_ Length of buffer (in bytes) to compare

        \return true if the buffers match, false if they do not.
    */
    static bool CompareMemory( const void *pvMem1_, const void *pvMem2_, K_USHORT usLen_ );

    //-----------------------------------------------------------------------
    /*!
        Initialize a buffer of memory to a specified 8-bit pattern

        \fn static void SetMemory( void *pvDst_, K_UCHAR ucVal_, K_USHORT usLen_ )

        \param pvDst_ Destination buffer to set
        \param ucVal_ 8-bit pattern to initialize each byte of destination with
        \param usLen_ Length of the buffer (in bytes) to initialize
    */
    static void SetMemory( void *pvDst_, K_UCHAR ucVal_, K_USHORT usLen_ );

    //-----------------------------------------------------------------------
    /*!
       \brief Tokenize Function to tokenize a string based on a space delimeter.  This is a
              non-destructive function, which populates a Token_t descriptor array.

       \param szBuffer_ String to tokenize
       \param pastTokens_ Pointer to the array of token descriptors
       \param ucMaxTokens_ Maximum number of tokens to parse (i.e. size of pastTokens_)
       \return Count of tokens parsed
    */
    static K_UCHAR Tokenize( const char *szBuffer_, Token_t *pastTokens_, K_UCHAR ucMaxTokens_);    
};


#endif //__MEMUTIL_H__




