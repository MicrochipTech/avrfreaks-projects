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
    \file   nlfs.cpp
    \brief  Nice Little Filesystem (NLFS) implementation for Mark3
*/

#include "kerneltypes.h"
#include "nlfs.h"
#include "nlfs_file.h"
#include "memutil.h"
#include "nlfs_config.h"

//---------------------------------------------------------------------------
K_CHAR NLFS::Find_Last_Slash( const char *szPath_ )
{
    K_UCHAR ucLastSlash = 0;
    K_UCHAR i = 0;
    while (szPath_[i])
    {
        if (szPath_[i] == '/')
        {
            ucLastSlash = i;
        }
        i++;
    }
    return ucLastSlash;
}

//---------------------------------------------------------------------------
K_BOOL NLFS::File_Names_Match( const K_CHAR *szPath_, NLFS_Node_t *pstNode_)
{
    K_UCHAR ucLastSlash = Find_Last_Slash( szPath_ );
    K_UCHAR i;

    ucLastSlash++;
    for (i = 0; i < FILE_NAME_LENGTH; i++)
    {
        if (!szPath_[ucLastSlash+i] || !pstNode_->stFileNode.acFileName[i])
        {
            break;
        }
        if (szPath_[ucLastSlash+i] != pstNode_->stFileNode.acFileName[i])
        {
            return false;
        }
    }

    if (szPath_[ucLastSlash+i] != pstNode_->stFileNode.acFileName[i])
    {
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------
void NLFS::Print_File_Details( K_USHORT usNode_ )
{
    NLFS_Node_t stFileNode;
    Read_Node(usNode_, &stFileNode);

    DEBUG_PRINT(" Name       : %16s\n" , stFileNode.stFileNode.acFileName);
    DEBUG_PRINT(" Next Peer  : %d\n"   , stFileNode.stFileNode.usNextPeer);
    DEBUG_PRINT(" Prev Peer  : %d\n"   , stFileNode.stFileNode.usPrevPeer);
    DEBUG_PRINT(" User|Group : %d|%d\n", stFileNode.stFileNode.ucUser,
                                         stFileNode.stFileNode.ucGroup);

    DEBUG_PRINT(" Permissions: %04X\n" , stFileNode.stFileNode.usPerms);
    DEBUG_PRINT(" Parent     : %d\n"   , stFileNode.stFileNode.usParent);
    DEBUG_PRINT(" First Child: %d\n"   , stFileNode.stFileNode.usChild);
    DEBUG_PRINT(" Alloc Size : %d\n"   , stFileNode.stFileNode.ulAllocSize);
    DEBUG_PRINT(" File  Size : %d\n"   , stFileNode.stFileNode.ulFileSize);

    DEBUG_PRINT(" First Block: %d\n"   , stFileNode.stFileNode.ulFirstBlock);
    DEBUG_PRINT(" Last Block : %d\n"   , stFileNode.stFileNode.ulLastBlock);
}

//---------------------------------------------------------------------------
void NLFS::Print_Dir_Details( K_USHORT usNode_ )
{
    NLFS_Node_t stFileNode;
    Read_Node(usNode_, &stFileNode);

    DEBUG_PRINT(" Name       : %16s\n" , stFileNode.stFileNode.acFileName);
    DEBUG_PRINT(" Next Peer  : %d\n"   , stFileNode.stFileNode.usNextPeer);
    DEBUG_PRINT(" Prev Peer  : %d\n"   , stFileNode.stFileNode.usPrevPeer);
    DEBUG_PRINT(" User|Group : %d|%d\n", stFileNode.stFileNode.ucUser,
                                   stFileNode.stFileNode.ucGroup);
    DEBUG_PRINT(" Permissions: %04X\n"  , stFileNode.stFileNode.usPerms);
    DEBUG_PRINT(" Parent     : %d\n"   , stFileNode.stFileNode.usParent);
    DEBUG_PRINT(" First Child: %d\n"   , stFileNode.stFileNode.usChild);
}

//---------------------------------------------------------------------------
void NLFS::Print_Free_Details( K_USHORT usNode_ )
{
    NLFS_Node_t stFileNode;
    Read_Node(usNode_, &stFileNode);

    DEBUG_PRINT(" Next Free  : %d\n"    , stFileNode.stFileNode.usNextPeer );
}

//---------------------------------------------------------------------------
void NLFS::Print_Node_Details( K_USHORT usNode_ )
{
    NLFS_Node_t stTempNode;
    Read_Node(usNode_, &stTempNode);

    DEBUG_PRINT("\nNode: %d\n"
           " Node Type: ", usNode_);
    switch (stTempNode.eBlockType)
    {
        case NLFS_NODE_FREE:
            DEBUG_PRINT( "Free\n" );
            Print_Free_Details(usNode_);
            break;
        case NLFS_NODE_ROOT:
            DEBUG_PRINT( "Root Block\n" );
            break;
        case NLFS_NODE_FILE:
            DEBUG_PRINT( "File\n" );
            Print_File_Details(usNode_);
            break;
        case NLFS_NODE_DIR:
            DEBUG_PRINT( "Directory\n" );
            Print_Dir_Details(usNode_);
            break;
        default:
            break;
    }
}

//---------------------------------------------------------------------------
K_USHORT NLFS::Pop_Free_Node(void)
{
    K_USHORT usRetVal = m_stLocalRoot.usNextFreeNode;
    NLFS_Node_t stFileNode;

    if (INVALID_NODE == usRetVal)
    {
        return 0;
    }

    // Update Claimed node
    Read_Node(usRetVal, &stFileNode);
    m_stLocalRoot.usNextFreeNode = stFileNode.stFileNode.usNextPeer;
    stFileNode.stFileNode.usNextPeer = INVALID_NODE;
    DEBUG_PRINT("Node %d allocated, next free %d\n", usRetVal, m_stLocalRoot.usNextFreeNode);
    Write_Node(usRetVal, &stFileNode);

    //Update root node
    Read_Node(FS_CONFIG_BLOCK , &stFileNode);
    stFileNode.stRootNode.usNextFreeNode = m_stLocalRoot.usNextFreeNode;
    stFileNode.stRootNode.usNumFilesFree--;
    Write_Node(FS_CONFIG_BLOCK, &stFileNode);

    return usRetVal;
}

//---------------------------------------------------------------------------
void NLFS::Push_Free_Node(K_USHORT usNode_)
{
    NLFS_Node_t stFileNode;

    Read_Node(usNode_, &stFileNode);
    stFileNode.stFileNode.usNextPeer = m_stLocalRoot.usNextFreeNode;
    m_stLocalRoot.usNextFreeNode = usNode_;

    Write_Node(usNode_, &stFileNode);

    DEBUG_PRINT("Node %d freed\n", usNode_);

    //Update root node
    Read_Node(FS_CONFIG_BLOCK , &stFileNode);
    stFileNode.stRootNode.usNextFreeNode = m_stLocalRoot.usNextFreeNode;
    stFileNode.stRootNode.usNumFilesFree++;
    Write_Node(FS_CONFIG_BLOCK , &stFileNode);
}

//---------------------------------------------------------------------------
K_ULONG NLFS::Pop_Free_Block(void)
{
    K_ULONG ulRetVal = m_stLocalRoot.ulNextFreeBlock;
    NLFS_Block_t stFileBlock;
    NLFS_Node_t  stFileNode;

    if ((INVALID_BLOCK == ulRetVal) || (0 == m_stLocalRoot.ulNumBlocksFree))
    {
        DEBUG_PRINT("Out of data blocks\n");
        return 0;
    }

    Read_Block_Header(ulRetVal, &stFileBlock);

    m_stLocalRoot.ulNextFreeBlock = stFileBlock.ulNextBlock;
    m_stLocalRoot.ulNumBlocksFree--;
    stFileBlock.ulNextBlock = INVALID_BLOCK;

    Write_Block_Header(ulRetVal, &stFileBlock);

    Read_Node(FS_CONFIG_BLOCK , &stFileNode);

    stFileNode.stRootNode.ulNextFreeBlock = m_stLocalRoot.ulNextFreeBlock;
    stFileNode.stRootNode.ulNumBlocksFree--;

    Write_Node(FS_CONFIG_BLOCK , &stFileNode);

    DEBUG_PRINT("Allocated block %d, next free %d\n", ulRetVal, m_stLocalRoot.ulNextFreeBlock);
    return ulRetVal;
}

//---------------------------------------------------------------------------
void NLFS::Push_Free_Block(K_ULONG ulBlock_ )
{    
    NLFS_Block_t stFileBlock;
    NLFS_Node_t  stFileNode;

    Read_Block_Header(ulBlock_, &stFileBlock);

    stFileBlock.ulNextBlock = m_stLocalRoot.ulNextFreeBlock;
    m_stLocalRoot.ulNextFreeBlock = ulBlock_;

    Write_Block_Header(ulBlock_, &stFileBlock);

    Read_Node(FS_CONFIG_BLOCK , &stFileNode);
    stFileNode.stRootNode.ulNextFreeBlock = m_stLocalRoot.ulNextFreeBlock;
    stFileNode.stRootNode.ulNumBlocksFree++;
    Write_Node(FS_CONFIG_BLOCK , &stFileNode);

    DEBUG_PRINT("Block %d freed\n", ulBlock_);
}

//---------------------------------------------------------------------------
K_ULONG NLFS::Append_Block_To_Node(NLFS_Node_t *pstFile_ )
{
    K_ULONG ulBlock;
    NLFS_Block_t stFileBlock;

    // Allocate a new block
    ulBlock = Pop_Free_Block();
    if (ulBlock == INVALID_BLOCK)
    {
        return -1;
    }

    // Initialize the block
    DEBUG_PRINT("reading block header\n");
    Read_Block_Header(ulBlock, &stFileBlock);
    stFileBlock.ulNextBlock = INVALID_BLOCK;
    stFileBlock.uAllocated  = 1;

    DEBUG_PRINT("writing block header\n");
    Write_Block_Header(ulBlock, &stFileBlock);

    // Update the previous last-block links (if there is one)
    DEBUG_PRINT("updating previous block %d\n", pstFile_->stFileNode.ulLastBlock);
    if (pstFile_->stFileNode.ulLastBlock != INVALID_BLOCK)
    {
        Read_Block_Header(pstFile_->stFileNode.ulLastBlock, &stFileBlock);
        stFileBlock.ulNextBlock = ulBlock;
        Write_Block_Header(pstFile_->stFileNode.ulLastBlock, &stFileBlock);
    }
    else
    {
        DEBUG_PRINT("  previous block is invalid, setting as first\n");
        pstFile_->stFileNode.ulFirstBlock = ulBlock;
    }

    pstFile_->stFileNode.ulLastBlock = ulBlock;
    pstFile_->stFileNode.ulAllocSize += m_stLocalRoot.ulBlockSize;

    RootSync();

    return ulBlock;
}

//---------------------------------------------------------------------------
K_USHORT NLFS::Find_Parent_Dir(const K_CHAR *szPath_)
{
    int i, j;
    K_UCHAR ucLastSlash = 0;
    K_USHORT usRetVal;
    K_CHAR szTempName[FILE_NAME_LENGTH];
    NLFS_Node_t stFileNode;
    K_USHORT usTempPeer;

    Read_Node(FS_ROOT_BLOCK, &stFileNode );

    usRetVal = FS_ROOT_BLOCK;

    if (szPath_[0] != '/')
    {
        DEBUG_PRINT("Only fully-qualified paths are supported.  Bailing\n");
        return -1;
    }

    // Starting from the root fs_block (which is the mount point...)
    ucLastSlash = Find_Last_Slash(szPath_);

    // a) Search for each "/" if we've got more than one...
    if (0 == ucLastSlash)
    {
        return usRetVal;
    }

    usTempPeer = stFileNode.stFileNode.usChild;
    Read_Node(usTempPeer, &stFileNode );

    i = 1;
    while (szPath_[i] && i < ucLastSlash)
    {
        NLFS_Node_t stTempNode;
        K_BOOL bMatch = false;

        j = 0;
        MemUtil::SetMemory(szTempName, 0, FILE_NAME_LENGTH);

        while (szPath_[i] && (szPath_[i] != '/') && j < FILE_NAME_LENGTH)
        {
            szTempName[j] = szPath_[i];
            i++;
            j++;
        }
        DEBUG_PRINT("Checking %s\n", szTempName );
        if (j == FILE_NAME_LENGTH && szPath_[i] != '/')
        {
            DEBUG_PRINT("Directory name too long, invalid\n");
            return -1;
        }
        else if (szPath_[i] != '/')
        {
            i++;
            continue;
        }

        // Check to see if there's a valid peer with this name...
        while (INVALID_NODE != usTempPeer)
        {
            Read_Node(usTempPeer, &stTempNode);
            if (NLFS_NODE_DIR == stTempNode.eBlockType)
            {
                if (true == MemUtil::CompareStrings(stTempNode.stFileNode.acFileName, szTempName))
                {
                    bMatch = true;
                    break;
                }
            }
            usTempPeer = stTempNode.stFileNode.usNextPeer;
        }

        // Matched the folder name descend into the folder
        if (bMatch)
        {
            DEBUG_PRINT("Matched folder: %s, node %d\n", szTempName, usTempPeer);

            usRetVal = usTempPeer;

            usTempPeer = stTempNode.stFileNode.usChild;
            if (INVALID_NODE != usTempPeer)
            {
                DEBUG_PRINT("Entering subdirectory %d\n", usTempPeer);
                Read_Node(usTempPeer, &stFileNode);
            }
            else
            {
                break;
            }
        }
        // Failed to match the folder name, bail
        else
        {
            DEBUG_PRINT("Could not match folder name, bailing\n");
            usRetVal = -1;
            break;
        }

        if (i >= ucLastSlash)
        {
            break;
        }
        i++;
    }

    if (i == ucLastSlash)
    {
        // No more folders to traverse - we're successful.
        DEBUG_PRINT("Found root path for %s\n with node %d\n", szPath_, usRetVal);
        return usRetVal;
    }
    return INVALID_NODE;
}

//---------------------------------------------------------------------------
K_USHORT NLFS::Find_File(const K_CHAR *szPath_)
{
    NLFS_Node_t stTempNode;
    NLFS_Node_t stTempDir;

    K_USHORT usTempNode;

    K_USHORT usParentDir = Find_Parent_Dir(szPath_);

    if (INVALID_NODE == usParentDir)
    {
        DEBUG_PRINT("invalid root dir\n");
        return INVALID_NODE;
    }

    Read_Node(usParentDir, &stTempDir);

    if (INVALID_NODE == stTempDir.stFileNode.usChild)
    {
        return INVALID_NODE;
    }

    usTempNode = stTempDir.stFileNode.usChild;

    // See if there are matching child nodes
    while (INVALID_NODE != usTempNode)
    {
        Read_Node(usTempNode, &stTempNode);

        if (true == File_Names_Match(szPath_,&stTempNode ))
        {
            DEBUG_PRINT("matched file: %16s, node %d\n",
                   stTempNode.stFileNode.acFileName, usTempNode);
            return usTempNode;
        }

        usTempNode = stTempNode.stFileNode.usNextPeer;
    }
    DEBUG_PRINT("couldn't match file: %s\n", szPath_);
    return INVALID_NODE;
}

//---------------------------------------------------------------------------
void NLFS::Print(void)
{
    K_USHORT i;
    for (i = 0; i < m_stLocalRoot.usNumFiles; i++)
    {
        Print_Node_Details(i);
    }
}

//---------------------------------------------------------------------------
void NLFS::Set_Node_Name( NLFS_Node_t *pstFileNode_, const char *szPath_ )
{
    K_UCHAR i,j;
    K_UCHAR ucLastSlash = 0;

    // Search for the last "/", that's where we stop looking.
    i = 0;
    while (szPath_[i])
    {
        if (szPath_[i] == '/')
        {
            ucLastSlash = i;
        }
        i++;
    }

    // Parse out filename
    i = ucLastSlash + 1;
    j = 0;
    while (szPath_[i] && j < FILE_NAME_LENGTH)
    {     
        pstFileNode_->stFileNode.acFileName[j] = szPath_[i];
        j++;
        i++;
    }
    if (!szPath_[i]) // if no extension, we're done.
    {
        return;
    }
}

//---------------------------------------------------------------------------
K_USHORT NLFS::Create_File_i(const K_CHAR *szPath_, NLFS_Type_t eType_ )
{
    K_USHORT usNode;
    K_USHORT usRootNodes;

    NLFS_Node_t stFileNode;
    NLFS_Node_t stParentNode;
    NLFS_Node_t stPeerNode;

    // Tricky part - directory traversal
    usRootNodes = Find_Parent_Dir(szPath_);

    if (INVALID_NODE == usRootNodes)
    {
        DEBUG_PRINT("Unable to find path - bailing\n");
        return INVALID_NODE;
    }

    usNode = Pop_Free_Node();
    if (!usNode)
    {
        DEBUG_PRINT("Unable to allocate node.  Failing\n");
        return INVALID_NODE;
    }
    DEBUG_PRINT("New file using node %d\n", usNode);

    // File node allocated, do something with it...
    // Set the file's name and extension

    Read_Node(usNode, &stFileNode);

    // Set the file path
    Set_Node_Name(&stFileNode, szPath_);

    // Set block as in-use as a file
    stFileNode.eBlockType = eType_;

    // Zero-out the file
    stFileNode.stFileNode.ulFileSize = 0;

    // Set the default user and group, as well as perms
    stFileNode.stFileNode.ucUser   = 0;   //!! ToDo - set real user/group IDs
    stFileNode.stFileNode.ucGroup  = 0;
    stFileNode.stFileNode.usPerms  = PERM_U_ALL | PERM_G_ALL | PERM_O_ALL;

    stFileNode.stFileNode.usChild  = INVALID_NODE;
    stFileNode.stFileNode.usParent = usRootNodes;

    // Update the parent node.
    Read_Node(usRootNodes, &stParentNode);

    DEBUG_PRINT( "Parent's root child: %d\n", stParentNode.stFileNode.usChild );
    // Insert node at the beginning of the peer list
    if (INVALID_NODE != stParentNode.stFileNode.usChild)
    {
        stFileNode.stFileNode.usNextPeer = stParentNode.stFileNode.usChild;
        stFileNode.stFileNode.usPrevPeer = INVALID_NODE;

        // Update the peer node.
        Read_Node(stFileNode.stFileNode.usNextPeer , &stPeerNode);

        stPeerNode.stFileNode.usPrevPeer = usNode;
        stParentNode.stFileNode.usChild = usNode;

        DEBUG_PRINT("updating peer's prev: %d\n", stPeerNode.stFileNode.usPrevPeer);
        Write_Node(stFileNode.stFileNode.usNextPeer, &stPeerNode);
    }
    else
    {
        stParentNode.stFileNode.usChild = usNode;
        stFileNode.stFileNode.usNextPeer = INVALID_NODE;
        stFileNode.stFileNode.usPrevPeer = INVALID_NODE;
    }

    Write_Node(usNode, &stFileNode);
    Write_Node(usRootNodes, &stParentNode);

    RootSync();

    return usNode;
}

//---------------------------------------------------------------------------
K_USHORT NLFS::Create_File( const K_CHAR *szPath_ )
{

    if (INVALID_NODE != Find_File(szPath_))
    {
        DEBUG_PRINT("Create_File: File already exists\n");
        return INVALID_NODE;
    }

    return Create_File_i( szPath_, NLFS_NODE_FILE );
}

//---------------------------------------------------------------------------
K_USHORT NLFS::Create_Dir( const K_CHAR *szPath_ )
{
    if (INVALID_NODE != Find_File(szPath_))
    {
        DEBUG_PRINT("Create_Dir: Dir already exists!\n");
        return INVALID_NODE;
    }

    return Create_File_i(szPath_, NLFS_NODE_DIR );
}

//---------------------------------------------------------------------------
void NLFS::Cleanup_Node_Links(K_USHORT usNode_, NLFS_Node_t *pstNode_)
{
    DEBUG_PRINT("Cleanup_Node_Links: Entering\n");

    if (INVALID_NODE != pstNode_->stFileNode.usParent)
    {
        NLFS_Node_t stParent;
        DEBUG_PRINT("Cleanup_Node_Links: Parent Node: %d\n", pstNode_->stFileNode.usParent);
        Read_Node(pstNode_->stFileNode.usParent, &stParent);

        DEBUG_PRINT("0\n");
        if (stParent.stFileNode.usChild == usNode_)
        {
            DEBUG_PRINT("1\n");
            stParent.stFileNode.usChild = pstNode_->stFileNode.usNextPeer;
            Write_Node(pstNode_->stFileNode.usParent, &stParent);
            DEBUG_PRINT("2\n");
        }
    }

    DEBUG_PRINT("a\n");
    if ( (INVALID_NODE != pstNode_->stFileNode.usNextPeer) ||
         (INVALID_NODE != pstNode_->stFileNode.usPrevPeer) )
    {
        NLFS_Node_t stNextPeer;
        NLFS_Node_t stPrevPeer;

        DEBUG_PRINT("b\n");
        if (INVALID_NODE != pstNode_->stFileNode.usNextPeer)
        {
            DEBUG_PRINT("c\n");
            Read_Node(pstNode_->stFileNode.usNextPeer, &stNextPeer);
            DEBUG_PRINT("d\n");
        }

        if (INVALID_NODE != pstNode_->stFileNode.usPrevPeer)
        {
            DEBUG_PRINT("e\n");
            Read_Node(pstNode_->stFileNode.usPrevPeer, &stPrevPeer);
            DEBUG_PRINT("f\n");
        }

        if (INVALID_NODE != pstNode_->stFileNode.usNextPeer)
        {
            DEBUG_PRINT("g\n");
            stNextPeer.stFileNode.usPrevPeer = pstNode_->stFileNode.usPrevPeer;
            Write_Node(pstNode_->stFileNode.usNextPeer, &stNextPeer);
            DEBUG_PRINT("h\n");
        }

        if (INVALID_NODE != pstNode_->stFileNode.usPrevPeer)
        {
            DEBUG_PRINT("i\n");
            stPrevPeer.stFileNode.usNextPeer = pstNode_->stFileNode.usNextPeer;
            Write_Node(pstNode_->stFileNode.usPrevPeer, &stPrevPeer);
            DEBUG_PRINT("j\n");
        }
    }
    pstNode_->stFileNode.usParent = INVALID_NODE;
    pstNode_->stFileNode.usPrevPeer = INVALID_NODE;
    pstNode_->stFileNode.usNextPeer = INVALID_NODE;
}

//---------------------------------------------------------------------------
K_USHORT NLFS::Delete_Folder(const K_CHAR *szPath_)
{
    K_USHORT usNode = Find_File(szPath_);
    NLFS_Node_t stNode;

    if (INVALID_NODE == usNode)
    {
        DEBUG_PRINT("Delete_Folder: File not found!\n");
        return INVALID_NODE;
    }
    if (FS_ROOT_BLOCK == usNode || FS_CONFIG_BLOCK == usNode)
    {
        DEBUG_PRINT("Delete_Folder: Cannot delete root!\n");
        return INVALID_NODE;
    }

    Read_Node(usNode, &stNode);

    if (NLFS_NODE_FILE == stNode.eBlockType)
    {
        DEBUG_PRINT("Delete_Folder: Path is not a Folder (is it a file?)");
        return INVALID_NODE;
    }

    if (INVALID_NODE != stNode.stFileNode.usChild)
    {
        DEBUG_PRINT("Delete_Folder: Folder is not empty!");
        return INVALID_NODE;
    }

    Cleanup_Node_Links(usNode, &stNode);

    stNode.eBlockType = NLFS_NODE_FREE;

    Write_Node(usNode, &stNode);
    Push_Free_Node(usNode);

    RootSync();

    return usNode;
}

//---------------------------------------------------------------------------
K_USHORT NLFS::Delete_File( const K_CHAR *szPath_)
{
    K_USHORT usNode = Find_File(szPath_);
    K_ULONG ulCurr;
    K_ULONG ulPrev;
    NLFS_Node_t stNode;
    NLFS_Block_t stBlock;

    if (INVALID_NODE == usNode)
    {
        DEBUG_PRINT("Delete_File: File not found!\n");
        return INVALID_NODE;
    }
    if (FS_ROOT_BLOCK == usNode || FS_CONFIG_BLOCK == usNode)
    {
        DEBUG_PRINT("Delete_File: Cannot delete root!\n");
        return INVALID_NODE;
    }

    Read_Node(usNode, &stNode);

    if (NLFS_NODE_DIR == stNode.eBlockType)
    {
        DEBUG_PRINT("Delete_File: Path is not a file (is it a directory?)");
        return INVALID_NODE;
    }

    Cleanup_Node_Links(usNode, &stNode);
    ulCurr = stNode.stFileNode.ulFirstBlock;

    while (INVALID_BLOCK != ulCurr)
    {
        Read_Block_Header(ulCurr, &stBlock);

        ulPrev = ulCurr;
        ulCurr = stBlock.ulNextBlock;

        Push_Free_Block(ulPrev);
    }

    stNode.eBlockType = NLFS_NODE_FREE;

    Write_Node(usNode, &stNode);
    Push_Free_Node(usNode);

    RootSync();

    return usNode;
}

//---------------------------------------------------------------------------
void NLFS::Format(NLFS_Host_t *puHost_, K_ULONG ulTotalSize_, K_USHORT usNumFiles_, K_USHORT usDataBlockSize_)
{
    K_ULONG i;
    K_ULONG ulNumBlocks;

    NLFS_Node_t  stFileNode;
    NLFS_Block_t stFileBlock;

    // Compute number of data blocks (based on FS Size and the number of file blocks)
    ulTotalSize_ -= ((K_ULONG)usNumFiles_) * sizeof(stFileNode);
    ulNumBlocks = ulTotalSize_ / ((((K_ULONG)usDataBlockSize_) + (sizeof(stFileBlock) - 1) + 3 ) & ~3);

    DEBUG_PRINT("Number of blocks %d\n", ulNumBlocks);

    // Set up the local_pointer -> this is used for the low-level, platform-specific
    // bits, allowing the FS to be used on RAM buffers, EEPROM's, networks, etc.
    m_puHost = puHost_;

    // Set the local copies of the data block byte-offset, as well as the data-block size
    m_stLocalRoot.usNumFiles        = usNumFiles_;
    m_stLocalRoot.usNumFilesFree    = m_stLocalRoot.usNumFiles - 2;
    m_stLocalRoot.usNextFreeNode    = 2;

    m_stLocalRoot.ulNumBlocks       = ulNumBlocks;
    m_stLocalRoot.ulNumBlocksFree   = ulNumBlocks;
    m_stLocalRoot.ulNextFreeBlock   = 0;

    m_stLocalRoot.ulBlockSize       = ((((K_ULONG)usDataBlockSize_) + 3 ) & ~3 );
    m_stLocalRoot.ulBlockOffset     = (((K_ULONG)usNumFiles_) * sizeof(NLFS_Node_t));
    m_stLocalRoot.ulDataOffset      = m_stLocalRoot.ulBlockOffset
                                        + (((K_ULONG)ulNumBlocks) * sizeof(NLFS_Block_t));

    // Create root data block node
    MemUtil::CopyMemory(&(stFileNode.stRootNode), &m_stLocalRoot, sizeof(m_stLocalRoot));
    stFileNode.eBlockType = NLFS_NODE_ROOT;

    DEBUG_PRINT("Writing root node\n");
    Write_Node(0, &stFileNode);
    DEBUG_PRINT("Done\n");

    // Create root mount point (directory)
    MemUtil::SetMemory(&stFileNode, 0, sizeof(stFileNode));
    stFileNode.eBlockType = NLFS_NODE_DIR;

    stFileNode.stFileNode.acFileName[0] = '/';

    stFileNode.stFileNode.usNextPeer   = INVALID_NODE;
    stFileNode.stFileNode.usPrevPeer   = INVALID_NODE;
    stFileNode.stFileNode.ucGroup      = 0;
    stFileNode.stFileNode.ucUser       = 0;
    stFileNode.stFileNode.usPerms      = PERM_U_ALL | PERM_G_ALL | PERM_O_ALL;

    stFileNode.stFileNode.usParent     = INVALID_NODE;
    stFileNode.stFileNode.usChild      = INVALID_NODE;

    stFileNode.stFileNode.ulAllocSize  = 0;
    stFileNode.stFileNode.ulFileSize   = 0;

    stFileNode.stFileNode.ulFirstBlock = INVALID_BLOCK;
    stFileNode.stFileNode.ulLastBlock  = INVALID_BLOCK;

    DEBUG_PRINT("Writing mount point\n");
    Write_Node(1, &stFileNode);
    DEBUG_PRINT("Done\n");

    stFileNode.stFileNode.acFileName[0] = 0;
    // Format nodes
    for (i = 2; i < usNumFiles_; i++)
    {
        stFileNode.eBlockType = NLFS_NODE_FREE;
        if (i != usNumFiles_ - 1)
        {
            stFileNode.stFileNode.usNextPeer = (K_USHORT)(i + 1);            
        }
        else
        {
            stFileNode.stFileNode.usNextPeer = INVALID_NODE;
        }

        Write_Node(i, &stFileNode);
    }
    DEBUG_PRINT("File nodes formatted\n");

    // Format file blocks
    MemUtil::SetMemory(&stFileBlock, 0, sizeof(stFileBlock));

    DEBUG_PRINT("Writing file blocks\n");
    for (i = 0; i < ulNumBlocks; i++)
    {
        if (i == ulNumBlocks - 1)
        {
            stFileBlock.ulNextBlock = INVALID_BLOCK;
        }
        else
        {
            stFileBlock.ulNextBlock = i + 1;
        }

        Write_Block_Header(i, &stFileBlock);
    }    
}

//---------------------------------------------------------------------------
void NLFS::Mount(NLFS_Host_t *puHost_)
{
    NLFS_Node_t stRootNode;

    //!! Must set the host pointer first.
    m_puHost = puHost_;
    DEBUG_PRINT("Remounting FS %X - reading config node\n", puHost_);

    // Reload the root block into the local cache
    Read_Node(FS_CONFIG_BLOCK, &stRootNode);

    DEBUG_PRINT("Copying config node\n");
    MemUtil::CopyMemory(&m_stLocalRoot, &(stRootNode.stRootNode), sizeof(m_stLocalRoot));

    DEBUG_PRINT("Block Size", m_stLocalRoot.ulBlockSize );
    DEBUG_PRINT("Data Offset", m_stLocalRoot.ulDataOffset );
    DEBUG_PRINT("Block Offset", m_stLocalRoot.ulBlockOffset );
}

//---------------------------------------------------------------------------
void NLFS::RootSync()
{
    NLFS_Node_t stRootNode;

    MemUtil::CopyMemory(&(stRootNode.stRootNode), &m_stLocalRoot, sizeof(m_stLocalRoot));
    stRootNode.eBlockType = NLFS_NODE_ROOT;
    Write_Node(FS_CONFIG_BLOCK, &stRootNode);
}


//---------------------------------------------------------------------------
K_USHORT NLFS::GetFirstChild( K_USHORT usNode_ )
{
    NLFS_Node_t stTemp;
    if (!usNode_ || INVALID_NODE == usNode_)
    {
        return INVALID_NODE;
    }
    Read_Node(usNode_, &stTemp);

    if (stTemp.eBlockType != NLFS_NODE_DIR)
    {
        return INVALID_NODE;
    }

    return stTemp.stFileNode.usChild;
}

//---------------------------------------------------------------------------
K_USHORT NLFS::GetNextPeer( K_USHORT usNode_ )
{
    NLFS_Node_t stTemp;
    if (!usNode_ || INVALID_NODE == usNode_)
    {
        return INVALID_NODE;
    }
    Read_Node(usNode_, &stTemp);
    return stTemp.stFileNode.usNextPeer;
}

//---------------------------------------------------------------------------
K_BOOL NLFS::GetStat( K_USHORT usNode_, NLFS_File_Stat_t *pstStat_)
{
    NLFS_Node_t stTemp;
    if (!usNode_ || INVALID_NODE == usNode_)
    {
        return false;
    }
    Read_Node(usNode_, &stTemp);
    pstStat_->ulAllocSize = stTemp.stFileNode.ulAllocSize;
    pstStat_->ulFileSize = stTemp.stFileNode.ulFileSize;
    pstStat_->ucGroup = stTemp.stFileNode.ucGroup;
    pstStat_->ucUser = stTemp.stFileNode.ucUser;
    pstStat_->usPerms = stTemp.stFileNode.usPerms;
    MemUtil::CopyMemory(pstStat_->acFileName, stTemp.stFileNode.acFileName, 16);
    return true;
}

