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
    \file   nlfs_eeprom.cpp
    \brief  RAM-based Nice Little Filesystem (NLFS) driver
*/

#include "nlfs.h"
#include "nlfs_eeprom.h"
#include "memutil.h"
#include "nlfs_config.h"

#include <avr/io.h>
#include <avr/eeprom.h>

//---------------------------------------------------------------------------
void NLFS_EEPROM::Read_Node( K_USHORT usNode_, NLFS_Node_t *pstFileNode_)
{
    NLFS_Node_t *pstFileNode =  (NLFS_Node_t*)(m_puHost->kaData
                                                    + (usNode_ * sizeof(NLFS_Node_t)));

    eeprom_read_block((void*)pstFileNode_, (const void*)pstFileNode, sizeof(NLFS_Node_t) );
}

//---------------------------------------------------------------------------
void NLFS_EEPROM::Write_Node(K_USHORT usNode_, NLFS_Node_t *pstFileNode_)
{
    NLFS_Node_t *pstFileNode =  (NLFS_Node_t*)(m_puHost->kaData
                                                    + (usNode_ * sizeof(NLFS_Node_t)));

    eeprom_write_block((void*)pstFileNode_, (void*)pstFileNode, sizeof(NLFS_Node_t) );
}

//---------------------------------------------------------------------------
void NLFS_EEPROM::Read_Block_Header(K_ULONG ulBlock_, NLFS_Block_t *pstFileBlock_)
{
    NLFS_Block_t *pstFileBlock =  (NLFS_Block_t*)(m_puHost->kaData
                                                    + m_stLocalRoot.ulBlockOffset
                                                    + (ulBlock_ * sizeof(NLFS_Block_t)));

    eeprom_read_block((void*)pstFileBlock_, (const void*)pstFileBlock, sizeof(NLFS_Block_t) );
}

//---------------------------------------------------------------------------
void NLFS_EEPROM::Write_Block_Header(K_ULONG ulBlock_, NLFS_Block_t *pstFileBlock_)
{
    NLFS_Block_t *pstFileBlock =  (NLFS_Block_t*)(m_puHost->kaData
                                                    + m_stLocalRoot.ulBlockOffset
                                                    + (ulBlock_ * sizeof(NLFS_Block_t)));

    eeprom_write_block((void*)pstFileBlock_, (void*)pstFileBlock, sizeof(NLFS_Block_t) );
}

//---------------------------------------------------------------------------
void NLFS_EEPROM::Read_Block(K_ULONG ulBlock_, K_ULONG ulOffset_, void *pvData_, K_ULONG ulLen_)
{
    const void* pvAddr = (const void*)( m_puHost->kaData
                            + m_stLocalRoot.ulDataOffset
                            + ulOffset_
                            + (ulBlock_ * m_stLocalRoot.ulBlockSize) );

    eeprom_read_block(pvData_, pvAddr, (size_t)ulLen_ );
}

//---------------------------------------------------------------------------
void NLFS_EEPROM::Write_Block(K_ULONG ulBlock_, K_ULONG ulOffset_, void *pvData_, K_ULONG ulLen_)
{
    void* pvAddr = (void*)( m_puHost->kaData
                            + m_stLocalRoot.ulDataOffset
                            + ulOffset_
                            + (ulBlock_ * m_stLocalRoot.ulBlockSize) );

    eeprom_write_block(pvData_, pvAddr, (size_t)ulLen_ );
}
