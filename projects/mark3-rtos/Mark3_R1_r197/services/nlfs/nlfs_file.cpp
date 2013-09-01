/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2013 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file nlfs_file.cpp
    \brief Nice Little Filesystem - File Access Class
*/

#include "kerneltypes.h"
#include "memutil.h"
#include "nlfs_file.h"
#include "nlfs.h"
#include "nlfs_config.h"

//----------------------------------------------------------------------------
int NLFS_File::Open(NLFS *pclFS_, const K_CHAR *szPath_, NLFS_File_Mode_t eMode_)
{
    K_USHORT usNode;
    usNode = pclFS_->Find_File(szPath_);

    if (INVALID_NODE == usNode)
    {
        DEBUG_PRINT("file does not exist in path\n");
        if (eMode_ & NLFS_FILE_CREATE)
        {
            DEBUG_PRINT("Attempt to create\n");
            usNode = pclFS_->Create_File(szPath_);
            if (INVALID_NODE == usNode)
            {
                DEBUG_PRINT("unable to create node in path\n");
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }

    DEBUG_PRINT("Current Node: %d\n", usNode);

    m_pclFileSystem = pclFS_;
    m_pclFileSystem->Read_Node(usNode, &m_stNode);

    m_usFile = usNode;

    if (eMode_ & NLFS_FILE_APPEND)
    {
        if (!(eMode_ & NLFS_FILE_WRITE))
        {
            DEBUG_PRINT("Open file for append in read-only mode?  Why!\n");
            return -1;
        }
        if (-1 == Seek(m_stNode.stFileNode.ulFileSize))
        {
            DEBUG_PRINT("file open failed - error seeking to EOF for append\n");
            return -1;
        }

    }
    else if (eMode_ & NLFS_FILE_TRUNCATE)
    {
        if (!(eMode_ & NLFS_FILE_WRITE))
        {
            DEBUG_PRINT("Truncate file in read-only mode?  Why!\n");
            return -1;
        }

        K_ULONG ulCurr = m_stNode.stFileNode.ulFirstBlock;
        K_ULONG ulPrev = ulCurr;

        // Go through and clear all blocks allocated to the file
        while (INVALID_BLOCK != ulCurr)
        {
            NLFS_Block_t stBlock;
            pclFS_->Read_Block_Header(ulCurr, &stBlock);

            ulPrev = ulCurr;
            ulCurr = stBlock.ulNextBlock;

            pclFS_->Push_Free_Block(ulPrev);
        }

        m_ulOffset = 0;
        m_ulCurrentBlock = m_stNode.stFileNode.ulFirstBlock;
    }
    else
    {
        // Open file to beginning of file, regardless of mode.
        m_ulOffset = 0;
        m_ulCurrentBlock = m_stNode.stFileNode.ulFirstBlock;
    }

    m_ucFlags = eMode_;

    DEBUG_PRINT("Current Block: %d\n", m_ulCurrentBlock);
    DEBUG_PRINT("file open OK\n");
    return 0;
}

//----------------------------------------------------------------------------
int NLFS_File::Seek(K_ULONG ulOffset_)
{
    NLFS_Block_t stBlock;
    m_ulCurrentBlock = m_stNode.stFileNode.ulFirstBlock;
    m_ulOffset = ulOffset_;

    if (INVALID_NODE == m_usFile)
    {
        DEBUG_PRINT("Error - invalid file");
        return -1;
    }

    if (INVALID_BLOCK == m_ulCurrentBlock)
    {
        DEBUG_PRINT("Invalid block\n");
        m_ulOffset = 0;
        return -1;
    }

    m_pclFileSystem->Read_Block_Header(m_ulCurrentBlock, &stBlock);

    while (ulOffset_ >= m_pclFileSystem->GetBlockSize())
    {
        ulOffset_ -= m_pclFileSystem->GetBlockSize();
        m_ulCurrentBlock = stBlock.ulNextBlock;
        if ((ulOffset_) && (INVALID_BLOCK == m_ulCurrentBlock))
        {
            m_ulCurrentBlock = m_stNode.stFileNode.ulFirstBlock;
            m_ulOffset = 0;
            return -1;
        }
        m_pclFileSystem->Read_Block_Header(m_ulCurrentBlock, &stBlock);
    }

    m_ulOffset = ulOffset_;
    return 0;
}

//----------------------------------------------------------------------------
int NLFS_File::Read(void *pvBuf_, K_ULONG ulLen_)
{
    K_ULONG ulBytesLeft;
    K_ULONG ulOffset;
    K_ULONG ulRead = 0;
    K_BOOL bBail = false;

    K_CHAR *szCharBuf = (K_CHAR*)pvBuf_;

    if (INVALID_NODE == m_usFile)
    {
        DEBUG_PRINT("Error - invalid file");
        return -1;
    }

    if (!(NLFS_FILE_READ & m_ucFlags))
    {
        DEBUG_PRINT("Error - file not open for read\n");
        return -1;
    }

    DEBUG_PRINT("Reading: %d bytes from file\n", ulLen_);
    while (ulLen_ && !bBail)
    {
        ulOffset = m_ulOffset & (m_pclFileSystem->GetBlockSize() - 1);
        ulBytesLeft = m_pclFileSystem->GetBlockSize() - ulOffset;
        if (ulBytesLeft > ulLen_)
        {
            ulBytesLeft = ulLen_;
        }
        if (m_ulOffset + ulBytesLeft >= m_stNode.stFileNode.ulFileSize)
        {
            ulBytesLeft = m_stNode.stFileNode.ulFileSize - m_ulOffset;
            bBail = true;
        }

        DEBUG_PRINT( "%d bytes left in block, %d len, %x block\n", ulBytesLeft, ulLen_, m_ulCurrentBlock);
        if (ulBytesLeft && ulLen_ && (INVALID_BLOCK != m_ulCurrentBlock))
        {
            m_pclFileSystem->Read_Block(m_ulCurrentBlock, ulOffset, (void*)szCharBuf, ulBytesLeft );

            ulRead += ulBytesLeft;
            ulLen_ -= ulBytesLeft;
            szCharBuf += ulBytesLeft;
            m_ulOffset += ulBytesLeft;
            DEBUG_PRINT( "%d bytes to go\n", ulLen_);
        }
        if (ulLen_)
        {
            DEBUG_PRINT("reading next node\n");
            NLFS_Block_t stBlock;
            m_pclFileSystem->Read_Block_Header(m_ulCurrentBlock, &stBlock);
            m_ulCurrentBlock = stBlock.ulNextBlock;
        }

        if (INVALID_BLOCK == m_ulCurrentBlock)
        {
            break;
        }

    }
    DEBUG_PRINT("Return :%d bytes read\n", ulRead);
    return ulRead;
}

//----------------------------------------------------------------------------
int NLFS_File::Write(void *pvBuf_, K_ULONG ulLen_)
{
    K_ULONG ulBytesLeft;
    K_ULONG ulOffset;
    K_ULONG ulWritten = 0;
    K_CHAR *szCharBuf = (K_CHAR*)pvBuf_;

    if (INVALID_NODE == m_usFile)
    {
        DEBUG_PRINT("Error - invalid file");
        return -1;
    }

    if (!(NLFS_FILE_WRITE & m_ucFlags))
    {
        DEBUG_PRINT("Error - file not open for write\n");
        return -1;
    }

    DEBUG_PRINT("writing: %d bytes to file\n", ulLen_);
    while (ulLen_)
    {
        ulOffset = m_ulOffset & (m_pclFileSystem->GetBlockSize() - 1);
        ulBytesLeft = m_pclFileSystem->GetBlockSize() - ulOffset;
        if (ulBytesLeft > ulLen_)
        {
            ulBytesLeft = ulLen_;
        }
        if (ulBytesLeft && ulLen_ && (INVALID_BLOCK != m_ulCurrentBlock))
        {
            m_pclFileSystem->Write_Block(m_ulCurrentBlock, ulOffset, (void*)szCharBuf, ulBytesLeft );
            ulWritten += ulBytesLeft;
            ulLen_ -= ulBytesLeft;
            szCharBuf += ulBytesLeft;
            m_stNode.stFileNode.ulFileSize += ulBytesLeft;
            m_ulOffset += ulBytesLeft;
            DEBUG_PRINT( "%d bytes to go\n", ulLen_);
        }
        if (!ulLen_)
        {
            m_pclFileSystem->Write_Node(m_usFile, &m_stNode);
        }
        else
        {
            DEBUG_PRINT("appending\n");
            m_ulCurrentBlock = m_pclFileSystem->Append_Block_To_Node(&m_stNode);
        }

        DEBUG_PRINT("writing node to file\n");
        m_pclFileSystem->Write_Node(m_usFile, &m_stNode);
    }
    return ulWritten;
}

//----------------------------------------------------------------------------
int NLFS_File::Close(void)
{
    m_usFile = INVALID_NODE;
    m_ulCurrentBlock = INVALID_BLOCK;
    m_ulOffset = 0;
    m_ucFlags = 0;
    return 0;
}
