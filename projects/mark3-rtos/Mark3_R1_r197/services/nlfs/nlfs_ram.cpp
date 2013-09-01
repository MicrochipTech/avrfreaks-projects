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
    \file   nlfs_ram.cpp
    \brief  RAM-based Nice Little Filesystem (NLFS) driver
*/

#include "nlfs.h"
#include "nlfs_ram.h"
#include "memutil.h"
#include "nlfs_config.h"

//---------------------------------------------------------------------------
void NLFS_RAM::Read_Node( K_USHORT usNode_, NLFS_Node_t *pstFileNode_)
{
    NLFS_Node_t *pstFileNode =  (NLFS_Node_t*)(m_puHost->kaData
                                                    + (usNode_ * sizeof(NLFS_Node_t)));

    MemUtil::CopyMemory(pstFileNode_, pstFileNode, sizeof(NLFS_Node_t));
}

//---------------------------------------------------------------------------
void NLFS_RAM::Write_Node(K_USHORT usNode_, NLFS_Node_t *pstFileNode_)
{
    NLFS_Node_t *pstFileNode =  (NLFS_Node_t*)(m_puHost->kaData
                                                    + (usNode_ * sizeof(NLFS_Node_t)));

    MemUtil::CopyMemory(pstFileNode, pstFileNode_, sizeof(NLFS_Node_t));
}

//---------------------------------------------------------------------------
void NLFS_RAM::Read_Block_Header(K_ULONG ulBlock_, NLFS_Block_t *pstFileBlock_)
{
    NLFS_Block_t *pstFileBlock =  (NLFS_Block_t*)(m_puHost->kaData
                                                    + m_stLocalRoot.ulBlockOffset
                                                    + (ulBlock_ * sizeof(NLFS_Block_t)));

    MemUtil::CopyMemory(pstFileBlock_, pstFileBlock, sizeof(NLFS_Block_t));
}

//---------------------------------------------------------------------------
void NLFS_RAM::Write_Block_Header(K_ULONG ulBlock_, NLFS_Block_t *pstFileBlock_)
{
    NLFS_Block_t *pstFileBlock =  (NLFS_Block_t*)(m_puHost->kaData
                                                    + m_stLocalRoot.ulBlockOffset
                                                    + (ulBlock_ * sizeof(NLFS_Block_t)));

    MemUtil::CopyMemory(pstFileBlock, pstFileBlock_, sizeof(NLFS_Block_t));
}

//---------------------------------------------------------------------------
void NLFS_RAM::Read_Block(K_ULONG ulBlock_, K_ULONG ulOffset_, void *pvData_, K_ULONG ulLen_)
{
    void *pvSrc_ = (void*)( m_puHost->kaData
                            + m_stLocalRoot.ulDataOffset
                            + ulOffset_
                            + (ulBlock_ * m_stLocalRoot.ulBlockSize) );
    MemUtil::CopyMemory(pvData_, pvSrc_, (K_USHORT)ulLen_);    
}

//---------------------------------------------------------------------------
void NLFS_RAM::Write_Block(K_ULONG ulBlock_, K_ULONG ulOffset_, void *pvData_, K_ULONG ulLen_)
{
    void *pvDst_ = (void*)( m_puHost->kaData
                            + m_stLocalRoot.ulDataOffset
                            + ulOffset_
                            + (ulBlock_ * m_stLocalRoot.ulBlockSize) );
    MemUtil::CopyMemory(pvDst_, pvData_, (K_USHORT)ulLen_);    
}
