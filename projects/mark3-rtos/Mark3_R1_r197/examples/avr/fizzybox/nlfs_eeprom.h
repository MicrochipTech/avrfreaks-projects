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
    \file   nlfs_eeprom.h
    \brief  RAM-based Nice Little Filesystem (NLFS) driver
*/

#ifndef __NLFS_EEPROM_H
#define __NLFS_EEPROM_H

#include "nlfs.h"

/*!
 * \brief The NLFS_EEPROM class
 *
 * This class implements an NLFS filesystem in an EEPROM buffer.  In this case, the
 * host pointer passed into the "format" call is a pointer to the locally-
 * allocated buffer in which the filesystem lives.
 */
class NLFS_EEPROM : public NLFS
{
private:

    /*!
     * \brief Read_Node is an implementation-specific method used to read a
     *        file node from physical storage into a local data struture.
     * \param [in] usNode_ - File node index
     * \param [out] pstNode_ - Pointer to the file node object to read into
     */
    virtual void Read_Node(K_USHORT usNode_, NLFS_Node_t *pstNode_);

    /*!
     * \brief Write_Node is an implementation-specific method used to write a
     *        file node from a local structure back to the physical storage.
     * \param [in] usNode_ - File node index
     * \param [in] pstNode_ - Pointer to the file node object to write from
     */
    virtual void Write_Node(K_USHORT usNode_, NLFS_Node_t *pstNode_);

    /*!
     * \brief Read_Block_Header is an implementation-specific method used to read
     *        a file block header from physical storage into a local struct.
     * \param [in] ulBlock_ - data block index
     * \param [out] pstBlock_ - block header structure to read into
     */
    virtual void Read_Block_Header(K_ULONG ulBlock_, NLFS_Block_t *pstBlock_);

    /*!
     * \brief Write_Block_Header is an implementation-specific method used to write
     *        a file block header back to physical storage from a local struct
     * \param [in] ulBlock_ - data block index
     * \param [in] pstFileBlock_ - pointer to the local data structure to write from
     */
    virtual void Write_Block_Header(K_ULONG ulBlock_, NLFS_Block_t *pstFileBlock_);

    /*!
     * \brief Read_Block is an implementation-specific method used to read raw file
     *        data from physical storage into a local buffer.
     *
     * \param [in] ulBlock_ - filesystem block ID corresponding to the file
     * \param [in] ulOffset_ - offset (in bytes) from the beginning of the block
     * \param [out] pvData_ - output buffer to read into
     * \param [in] ulLen_ - length of data to read (in bytes)
     */
    virtual void Read_Block(K_ULONG ulBlock_, K_ULONG ulOffset_, void *pvData_, K_ULONG ulLen_);

    /*!
     * \brief Write_Block is an implementation-specific method used to write a
     *        piece of file data to its data block in the underlying physical
     *        storage.
     *
     * \param [in] ulBlock_ - filesystem block ID corresponding to the file
     * \param [in] ulOffset_ - offset (in bytes) from the beginning of the block
     * \param [in] pvData_ - data buffer to write to disk
     * \param [in] ulLen_ - length of data to write (in bytes)
     */
    void Write_Block(K_ULONG ulBlock_, K_ULONG ulOffset_, void *pvData_, K_ULONG ulLen_);

};

#endif // NLFS_RAM_H
