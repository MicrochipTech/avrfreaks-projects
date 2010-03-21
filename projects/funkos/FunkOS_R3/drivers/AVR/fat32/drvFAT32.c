//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
    Contains the definitions for a FAT32 driver for FunkOS.  Currently, the
    driver is designed for single file-handle, read-only operation, but could
    be extended for use with multiple file handles.
*/
//---------------------------------------------------------------------------

#include "types.h"
#include "driver.h"
#include "drvFAT32.h"

#define __WINDOWS_TEST
#ifdef __WINDOWS_TEST
    #include <stdio.h>
    static FILE *pfTest;
#endif

//---------------------------------------------------------------------------
static FAT32_RETURN_CODE FAT32_InitMedia(FAT32_DRIVER_STRUCT *pstDriver_);
static FAT32_RETURN_CODE FAT32_ReadSector(FAT32_DRIVER_STRUCT *pstDriver_, ULONG ulAddr_);
static UCHAR FAT32_RawReadByte(ULONG ulAddr_);

static FAT32_RETURN_CODE FAT32_MountPartition(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR ucPartition_);
static FAT32_RETURN_CODE FAT32_DirReadRecord(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR *aucRecord);
static FAT32_RETURN_CODE FAT32_ChangeDir(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR ucDirIdx_);
static FAT32_RETURN_CODE FAT32_OpenFile(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR ucFileIdx_);
static FAT32_RETURN_CODE FAT32_CloseFile(FAT32_DRIVER_STRUCT *pstDriver_);

static FAT32_RETURN_CODE FAT32_SeekBeginFolder(FAT32_DRIVER_STRUCT *pstDriver_);
static FAT32_RETURN_CODE FAT32_SeekRootFolder(FAT32_DRIVER_STRUCT *pstDriver_);
static FAT32_RETURN_CODE FAT32_ReadNextCluster(FAT32_DRIVER_STRUCT *pstDriver_, FAT32_FILE_STRUCT *pstFile_);

static ULONG FAT32_ReadFAT32(FAT32_DRIVER_STRUCT *pstDriver_, ULONG ulCurrentFAT_);
static USHORT FAT32_ReadFAT16(FAT32_DRIVER_STRUCT *pstDriver_, ULONG ulCurrentFAT_);

//---------------------------------------------------------------------------
/*!
    This function is part of the external device driver interface, and is used
    to initialize the FAT driver to a known-good state.  All states are reset
    and zero'd to allow other driver functions to run correctly.
    \fn void FAT32_Init(DRIVER_STRUCT *pstDriver_)
    \param pstDriver_ - pointer to the FAT driver struct (cast as a DRIVER_STRUCT object)
*/
void FAT32_Init(DRIVER_STRUCT *pstDriver_)
{
	// Initialize the data structure to a default, unmounted state
	FAT32_DRIVER_STRUCT *pstFAT = (FAT32_DRIVER_STRUCT *)pstDriver_;

    pstFAT->ulSectorsPerFAT = 0;
    pstFAT->ulFirstCluster = 0;
    pstFAT->ulFATBegin = 0;
    pstFAT->ulFirstCluster = 0;

	pstFAT->eState = FAT32_STATE_UNINIT;
    pstFAT->usNumReservedSectors = 0;
    pstFAT->ucSectorsPerCluster = 0;
    pstFAT->szMountPoint = "";

    pstFAT->ucValidPartitions = 0;
    pstFAT->ucMountedPartition = 0;

#define __WINDOWS_TEST
#ifdef __WINDOWS_TEST
    pfTest = NULL;
    pfTest = fopen("c:\\test16.img", "rb");
#endif

}

//---------------------------------------------------------------------------
/*!
    Part of the public device driver interface for the module.
    Opens the media, reads the MBR and determines if its FAT16/32 formatted
    and then loads the root directory, reading the first sector.
    \fn void FAT32_Start(DRIVER_STRUCT *pstDriver_)
    \param pstDriver_ - pointer to the FAT driver struct (cast as a DRIVER_STRUCT object)
*/
void FAT32_Start(DRIVER_STRUCT *pstDriver_)
{
	FAT32_DRIVER_STRUCT *pstFAT = (FAT32_DRIVER_STRUCT *)pstDriver_;
	FAT32_Control(pstDriver_, FAT32_CMD_INIT_MEDIA, NULL);
}

//---------------------------------------------------------------------------
/*!
    Part of the public device driver interface.  Null at this point - no
    unmounting/de-initialization is performed at this point.
    \fn void FAT32_Stop(DRIVER_STRUCT *pstDriver_)
    \param pstDriver_ - pointer to the FAT driver struct (cast as a DRIVER_STRUCT object)
*/
void FAT32_Stop(DRIVER_STRUCT *pstDriver_)
{
	FAT32_DRIVER_STRUCT *pstFAT = (FAT32_DRIVER_STRUCT *)pstDriver_;
}

//---------------------------------------------------------------------------
/*!
    Part of the external device driver implementation.  This is the main event
    dispatches for the FAT device driver function.
    \fn UCHAR FAT32_Control(DRIVER_STRUCT *pstDriver_, USHORT usID_, void *pvData_)
    \param pstDriver_ - pointer to the device driver
    \param usID_ - driver control event ID
    \param pvData_- pointer to the event data
    \return a USHORT containing an (enum)FAT32_RETURN_CODE from the handler function
*/
USHORT FAT32_Control(DRIVER_STRUCT *pstDriver_, USHORT usID_, void *pvData_)
{
	FAT32_DRIVER_STRUCT *pstFAT = (FAT32_DRIVER_STRUCT *)pstDriver_;
	FAT32_RETURN_CODE eReturn = FAT32_RETURN_OK;

	switch((FAT32_COMMAND)usID_)
	{
		case FAT32_CMD_INIT_MEDIA:
		{
			eReturn = FAT32_InitMedia(pstDriver_);
		}
			break;
		case FAT32_CMD_MOUNT_PARTITION:
		{
			UCHAR ucPartition = *((UCHAR*)pvData_);
			eReturn = FAT32_MountPartition(pstDriver_, ucPartition);
		}
			break;
		case FAT32_CMD_READ_SECTOR:
		{
			ULONG ulSector = *((ULONG*)pvData_);
			eReturn = FAT32_ReadSector(pstDriver_, ulSector);
		}
			break;
        case FAT32_CMD_READ_NEXT_DIR:
        {
            eReturn = FAT32_DirReadRecord(pstDriver_, pvData_);
        }
            break;
		case FAT32_CMD_WRITE_SECTOR:
		{
		}
			break;
		case FAT32_CMD_CHDIR:
		{
            UCHAR ucID;
            ucID = *((UCHAR*)pvData_);
            eReturn = FAT32_ChangeDir(pstDriver_, ucID);
		}
			break;
		case FAT32_CMD_CHDIR_ROOT:
		{
            eReturn = FAT32_SeekRootFolder(pstDriver_);
		}
			break;
        case FAT32_CMD_SEEK_BEGIN_DIR:
        {
            FAT32_SeekBeginFolder(pstDriver_);
        }
            break;
		case FAT32_CMD_OPEN_FILE_READ:
		{
            UCHAR ucID;
            ucID = *((UCHAR*)pvData_);
            eReturn = FAT32_OpenFile(pstDriver_, ucID);
		}
			break;
		case FAT32_CMD_OPEN_FILE_WRITE:
		{
		}
			break;
		case FAT32_CMD_CLOSE_FILE:
		{
            eReturn = FAT32_CloseFile(pstDriver_);
		}
			break;
		default:
			break;
	}
	return (UCHAR)eReturn;
}

//---------------------------------------------------------------------------
/*!
    Generic driver read function for the FAT driver.  Part of the public device
    driver interface.  This is used to read from the file's raw byte stream from
    the current file read pointer - only sequential access is provided by this
    function.
    \fn USHORT FAT32_Read(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_)
    \param pstDriver_ - pointer to the device driver structure (FAT32_DRIVER_STRUCT*) cast to (DEVICE_DRIVER_STRUCT*)
    \param pucData_ - pointer to the read buffer char array
    \param usLen_ - number of bytes to read from the device buffer
    \return USHORT - number of bytes actually read from the device
*/
USHORT FAT32_Read(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_)
{
    USHORT i = 0;
    UCHAR ucTemp;
    USHORT usRead = 0;

    FAT32_DRIVER_STRUCT *pstFAT = (FAT32_DRIVER_STRUCT*)pstDriver_;

    // Check to see if we can't read from the file
    if ((pstFAT->eState != FAT32_STATE_FILE_OPEN) || (pstFAT->stFile.bEOF == TRUE))
    {
        return 0;
    }

    // Read the data from the file, starting with the current
    for (i = 0; i < usLen_; i++, usRead++)
    {
        // check for end of file before we read
        if (pstFAT->stFile.ulReadBytes >= pstFAT->stFile.ulFileSize)
        {
            pstFAT->stFile.bEOF = TRUE;
            break;
        }

        // Check to see if we need to load the next sector
        if (pstFAT->usSectorReadAddr >=512)
        {
            pstFAT->usSectorReadAddr = 0;
            // Read the next sector...
            pstFAT->ucCurrentSector++;

            if (pstFAT->ucCurrentSector >= pstFAT->ucSectorsPerCluster)
            {
                // If we're at the end of the cluster, go to the next cluster
                // (read from the FAT), and read the first sector
                FAT32_ReadNextCluster(pstFAT, &(pstFAT->stFile));
            }
            else
            {
                // Read the next sector in the cluster
                ULONG ulClusterLBA;

                // Get the sector address
                ulClusterLBA =  pstFAT->ulFirstCluster +
                            ((pstFAT->stFile.ulCurrentFAT - pstFAT->ulClusterOffset) *
                            (ULONG)pstFAT->ucSectorsPerCluster) +
                            (ULONG)pstFAT->ucCurrentSector;

                FAT32_ReadSector(pstFAT, ulClusterLBA);
            }
        }
         // Read bytes from the currently loaded sector
        pucData_[i] = pstFAT->aucSectorData[pstFAT->usSectorReadAddr++];

        // Update the number of bytes read.
        pstFAT->stFile.ulReadBytes++;
    }

    // Return the number of bytes read this call
    return usRead;
}

//---------------------------------------------------------------------------
/*!
    Public device driver interface function for FAT file driver.  This is a
    stub since write operations are currently not supported.
    \fn USHORT FAT32_Write(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_)
    \param pstDriver_ - pointer to the FAT device driver struct
    \param pucData_ - pointer to the output write data stream
    \param usLen_ - length in bytes to write
    \return USHORT - number of bytes actually written
*/
USHORT FAT32_Write(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_)
{
	return 0;
}

//---------------------------------------------------------------------------
/*!
    Raw low-level interface to the filesystem - this is where the hardware
    level interface is hooked into the driver (SPI/UART/File,etc).  Reads a
    byte from the filesystem at the address specified.
    \fn static UCHAR FAT32_RawReadByte(ULONG ulAddr_)
    \param ulAddr_ - Raw address in the filesystem (4GB limit)
    \return UCHAR - return the read byte at the address
*/
static UCHAR FAT32_RawReadByte(ULONG ulAddr_)
{
	// This is the physical layer interface... could be SPI on an AVR


#ifdef __WINDOWS_TEST
    UCHAR ucTemp;
    fseek(pfTest, ulAddr_, SEEK_SET);
    fread(&ucTemp, 1, sizeof(UCHAR), pfTest);
    return ucTemp;
#endif
}

//---------------------------------------------------------------------------
/*!
    Initializes media containing FAT partitions.  This involves reading the
    partition table from the Master Boot Record and finding the location of
    all valid FAT16/32 partitions on the drive.
    \fn static FAT32_RETURN_CODE FAT32_InitMedia(FAT32_DRIVER_STRUCT *pstDriver_)
    \param pstDriver_ - pointer to the device
    \return FAT32_RETURN_CODE - FAT32_RETURN_OK on success, other error codes on failure
*/
static FAT32_RETURN_CODE FAT32_InitMedia(FAT32_DRIVER_STRUCT *pstDriver_)
{
	UCHAR aucPartitions[4];
	UCHAR i;

	if (pstDriver_->eState == FAT32_STATE_UNINIT)
	{
		// Read the master boot record (sector 0)
		FAT32_ReadSector(pstDriver_, 0);	//MBR is sector 0.

		// Get the list of partitions
		aucPartitions[0] = pstDriver_->aucSectorData[MBR_PARTITION1_ADDR + PARTITION_CODE_OFFSET];
		aucPartitions[1] = pstDriver_->aucSectorData[MBR_PARTITION2_ADDR + PARTITION_CODE_OFFSET];
		aucPartitions[2] = pstDriver_->aucSectorData[MBR_PARTITION3_ADDR + PARTITION_CODE_OFFSET];
		aucPartitions[3] = pstDriver_->aucSectorData[MBR_PARTITION4_ADDR + PARTITION_CODE_OFFSET];

		// Create the list of valid partitions (4 bits, each representing a partition)
		pstDriver_->ucValidPartitions = 0;
		for (i = 0; i < 4; i++)
		{
			if ((aucPartitions[i] == PARTITION_CODE_FAT32_A) ||
				(aucPartitions[i] == PARTITION_CODE_FAT32_B))
            {
				pstDriver_->ucValidPartitions |= (1 << i);
            }
            else if((aucPartitions[i] == PARTITION_CODE_FAT16_A) ||
                (aucPartitions[i] == PARTITION_CODE_FAT16_B))
			{
				pstDriver_->ucValidPartitions |= (1 << i);
			}
		}

		if (pstDriver_->ucValidPartitions == 0)
		{
            // Check to see if this is a bo
			return FAT32_INIT_ERROR_NO_PARTITIONS;
		}

		// MBR has been read now, switch states...
		pstDriver_->eState = FAT32_STATE_MBR_READ;

		return FAT32_RETURN_OK;
	}
	return FAT32_ERROR_WRONG_STATE;
}

//---------------------------------------------------------------------------
/*!
    Mounts the specified partition by index in the (0-3 in the specification).
    Reads the various pertinent information from the MBR array and the volumeID
    block for the FAT16/32 partition.
    \fn static FAT32_RETURN_CODE FAT32_MountPartition(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR ucPartition_)
    \param pstDriver_ - pointer to the FAT32 driver structure
    \param ucPartition_ - partition entry (0-3) of the driver (from the MBR)
    \return FAT32_RETURN_CODE - FAT32_RETURN_OK on success, other error codes on failure
*/
static FAT32_RETURN_CODE FAT32_MountPartition(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR ucPartition_)
{
	ULONG ulVolID;
	ULONG ulNumSectors;

	USHORT usOffset;
    USHORT usNumSectors16;

	ULONG ulTemp;
	USHORT usTemp;
	UCHAR ucTemp;

	if (pstDriver_->eState == FAT32_STATE_MBR_READ)
	{
		if (ucPartition_ >= 4)
		{
			return;	// only 4 valid partitions
		}

		// Get the address of the volume ID sector
		usOffset = MBR_PARTITION1_ADDR + LBA_BEGIN_OFFSET + (MBR_PARTITION_SIZE * ucPartition_);
		ulVolID = *((ULONG*)&(pstDriver_->aucSectorData[usOffset]));

   		usOffset = MBR_PARTITION1_ADDR + PARTITION_SECTOR_OFFSET_32 + (MBR_PARTITION_SIZE * ucPartition_);
		ulNumSectors = *((ULONG*)&(pstDriver_->aucSectorData[usOffset]));

		//Read the volume ID sector
		FAT32_ReadSector(pstDriver_, ulVolID);

		// OK, volume ID is loaded, now read the info from the sector
		usOffset = BYTES_PER_SEC_OFFSET;
		usTemp = *((USHORT*)&(pstDriver_->aucSectorData[usOffset]));
		if (usTemp != FAT32_SECTOR_SIZE)
		{
			// If the sector sizes don't match what we expect,
			// this is a critical error, and we should bail.
			return FAT32_ERROR_SECTOR_SIZE;
		}

		// Number of sectors per cluster
		usOffset = SECTORS_PER_CLUSETER_OFFSET;
		ucTemp = pstDriver_->aucSectorData[usOffset];
		pstDriver_->ucSectorsPerCluster = ucTemp;

		// Number of reserved sectors
		usOffset = RESERVED_SECTOR_OFFSET;
		usTemp = *((USHORT*)&(pstDriver_->aucSectorData[usOffset]));
		pstDriver_->usNumReservedSectors = usTemp;

        //Sectors per FAT
        usOffset = SECTORS_PER_FAT_OFFSET_16;
        usTemp = *((USHORT*)&(pstDriver_->aucSectorData[usOffset]));

        // This determines whether or not the device is FAT16 or FAT32
        if (usTemp != 0)
        {
            pstDriver_->eMode = FAT_MODE_16;
        }
        else
        {
            pstDriver_->eMode = FAT_MODE_32;
        }

        if (pstDriver_->eMode == FAT_MODE_16)
        {
             // Check the FAT16 number of sectors field
            usOffset = PARTITION_SECTOR_OFFSET_16;
            usNumSectors16 = *((USHORT*)&(pstDriver_->aucSectorData[usOffset]));

            //Sectors per FAT
            usOffset = SECTORS_PER_FAT_OFFSET_16;
            usTemp = *((USHORT*)&(pstDriver_->aucSectorData[usOffset]));
            pstDriver_->ulSectorsPerFAT = (ULONG)usTemp;

            //The root directory # of entries...
            usOffset = ROOT_ENTRIES_16;
            usTemp = *((USHORT*)&(pstDriver_->aucSectorData[usOffset]));

            // LBA of the first FAT sector
            pstDriver_->ulFATBegin = ulVolID + (ULONG)pstDriver_->usNumReservedSectors;

            //The root directory's first cluster
            pstDriver_->ulRootBegin = pstDriver_->ulFATBegin + (2 * pstDriver_->ulSectorsPerFAT);

            // LBA of the first Data cluster is the same as the root directory.
            pstDriver_->ulFirstCluster = pstDriver_->ulRootBegin + (2 * pstDriver_->ucSectorsPerCluster);

            pstDriver_->ulClusterOffset = 0;
        }
        else
        {
            //Sectors per FAT
            usOffset = SECTORS_PER_FAT_OFFSET;
            ulTemp = *((ULONG*)&(pstDriver_->aucSectorData[usOffset]));
            pstDriver_->ulSectorsPerFAT = ulTemp;

            //The root directory's first cluster
            usOffset = ROOT_DIRECTORY_OFFSET;
            ulTemp = *((ULONG*)&(pstDriver_->aucSectorData[usOffset]));
            pstDriver_->ulRootBegin = ulTemp;

            // LBA of the first FAT sector
            pstDriver_->ulFATBegin = ulVolID + (ULONG)pstDriver_->usNumReservedSectors;

            // LBA of the first Data cluster
            pstDriver_->ulFirstCluster = pstDriver_->ulFATBegin + (2 * pstDriver_->ulSectorsPerFAT);

            pstDriver_->ulClusterOffset = 2; // Always subtract 2 clusters in LBA w/Fat32
        }
		// Partition mounted successfully - set the drive state
		pstDriver_->eState = FAT32_STATE_PARTITION_MOUNTED;

		// Read the first sector of the root directory
		pstDriver_->ucDirRecord = 0;
		pstDriver_->ucCurrentSector = 0;

		// Initialize the directory struct to the root directory
		pstDriver_->stFolder.aucFileName[0] = '\0';

		pstDriver_->stFolder.ulHeadFAT = 2 ;
		pstDriver_->stFolder.ulCurrentFAT = 2;
        pstDriver_->stFolder.bEOF = FALSE;

        // Read the first sector of the root directory
        if (pstDriver_->eMode == FAT_MODE_16)
        {
            ulTemp = pstDriver_->ulRootBegin;
        }
        else
        {
            ulTemp = pstDriver_->ulFirstCluster;
        }

		if (FAT32_ReadSector(pstDriver_, ulTemp) == FAT32_RETURN_OK)
		{
			return FAT32_RETURN_OK;
		}
		return FAT32_RETURN_FAIL;
	}
	return FAT32_ERROR_WRONG_STATE;
}
//---------------------------------------------------------------------------
/*!
    Reads a 512-byte sector from the drive specified by the raw LBA index
    specified in the input paramter.  No caching is supplied, so only
    one sector is contained in the driver's internal buffer at any time.
    \fn static FAT32_RETURN_CODE FAT32_ReadSector(FAT32_DRIVER_STRUCT *pstDriver_, ULONG ulSector_)
    \param pstDriver_ - pointer to the FAT32 driver structure
    \param ulSector_ - the sector index to read (raw 512 byte sector index)
    \return FAT32_RETURN_CODE - FAT32_RETURN_OK on success, other error codes on failure
*/
static FAT32_RETURN_CODE FAT32_ReadSector(FAT32_DRIVER_STRUCT *pstDriver_, ULONG ulSector_)
{
	ULONG i;
	ULONG ulAddress = ulSector_ * FAT32_SECTOR_SIZE;

	for (i = 0; i < FAT32_SECTOR_SIZE; i++)
	{
		pstDriver_->aucSectorData[i] = FAT32_RawReadByte(ulAddress + i);
	}
	return FAT32_RETURN_OK;
}

//---------------------------------------------------------------------------
/*!
    Read the next 16-bit cluster chain ID from the FAT16 table for the current
    file/dir's cluster.
    \fn static USHORT FAT32_ReadFAT16(FAT32_DRIVER_STRUCT *pstDriver_, ULONG ulCurrentFAT_)
    \param pstDriver_ - pointer to the FAT driver structure
    \param ulCurrentFAT_ - the current cluster's FAT address
    \return USHORT - the cluster ID of the next block in the cluster chain
*/
static USHORT FAT32_ReadFAT16(FAT32_DRIVER_STRUCT *pstDriver_, ULONG ulCurrentFAT_)
{
	ULONG ulData;
	ULONG ulFATAddr;
	ULONG i;
    UCHAR ucTemp;

	// Find the address of the FAT relative to the index of the beginning of the
	// FAT table
	ulFATAddr = ((pstDriver_->ulFATBegin) * FAT32_SECTOR_SIZE) + (ulCurrentFAT_  * 2);
    ulData = 0;

	// 	Check that the partition is mounted at least....
	if (pstDriver_->eState >= FAT32_STATE_PARTITION_MOUNTED)
	{
		// Get and return the address of the next FAT block (cluster #)
		for (i = 0; i < 2; i ++)
		{
			ucTemp = FAT32_RawReadByte(ulFATAddr + i);
            ulData |= ((ULONG)(ucTemp) << 8) >> (8 - (8*i));
		}
		return ulData;
	}

	// return EOF otherwise...
	return 0xFFFF;
}

//---------------------------------------------------------------------------
/*!
    Read the next 32-bit cluster chain ID from the FAT32 table for the current
    file/dir's cluster.
    \fn static ULONG FAT32_ReadFAT32(FAT32_DRIVER_STRUCT *pstDriver_, ULONG ulCurrentFAT_)
    \param pstDriver_ - pointer to the FAT driver structure
    \param ulCurrentFAT_ - current cluster's FAT index
    \return ULONG - the cluster ID of the next block in the cluster chain
*/
static ULONG FAT32_ReadFAT32(FAT32_DRIVER_STRUCT *pstDriver_, ULONG ulCurrentFAT_)
{
	ULONG ulData;
	ULONG ulFATAddr;
	ULONG i;
    UCHAR ucTemp;

	// Find the address of the FAT relative to the index of the beginning of the
	// FAT table
	ulFATAddr = ((pstDriver_->ulFATBegin) * FAT32_SECTOR_SIZE) + (ulCurrentFAT_ * 4);
    ulData = 0;

	// 	Check that the partition is mounted at least....
	if (pstDriver_->eState >= FAT32_STATE_PARTITION_MOUNTED)
	{
		// Get and return the address of the next FAT block (cluster #)
		for (i = 0; i < 4; i ++)
		{
			ucTemp = FAT32_RawReadByte(ulFATAddr + i);
            ulData |= ((ULONG)(ucTemp) << 24) >> (24 - (8*i));
		}
		return ulData;
	}

	// return EOF otherwise...
	return 0xFFFFFFFF;
}

//---------------------------------------------------------------------------
/*!
    Read the next cluster in the file's cluster chain (traverse the FAT and read
    the first sector in the next read cluster).
    \fn static FAT32_RETURN_CODE FAT32_ReadNextCluster(FAT32_DRIVER_STRUCT *pstDriver_, FAT32_FILE_STRUCT *pstFile_)
    \param pstDriver_ - pointer to the FAT driver struct
    \param pstFile_ - pointer to the FAT file/folder structure object
    \return FAT32_RETURN_CODE - FAT32_RETURN_OK on success, other code on failure.
*/
static FAT32_RETURN_CODE FAT32_ReadNextCluster(FAT32_DRIVER_STRUCT *pstDriver_, FAT32_FILE_STRUCT *pstFile_)
{
	ULONG ulClusterLBA;
	ULONG ulNextFAT;

	if (pstFile_->bEOF == TRUE)	// End of file - start from the beginning again
	{
		pstFile_->ulCurrentFAT = pstFile_->ulHeadFAT;
	}
	else
	{
        if (pstDriver_->eMode == FAT_MODE_32)
        {
            // Read the next cluster from the FAT.
            ulNextFAT = FAT32_ReadFAT32(pstDriver_, pstFile_->ulCurrentFAT);
            pstFile_->ulCurrentFAT = ulNextFAT;
        }
        else
        {
            // Read the next cluster from the FAT.
            ulNextFAT = FAT32_ReadFAT16(pstDriver_, pstFile_->ulCurrentFAT);
            pstFile_->ulCurrentFAT = ulNextFAT;
        }
	}

    if (pstDriver_->eMode == FAT_MODE_32)
    {
        // Cluster # is 28-bits long...
        if ((ulNextFAT & 0x0FFFFFFF) >= 0x0FFFFFF0)	// If this is the last sector in the file
        {
            pstFile_->bEOF = TRUE;
            return FAT32_ERROR_EOF;
        }
    }
    else
    {
        // Cluster # is 28-bits long...
        if (ulNextFAT >= 0xFFF0)	// If this is the last sector in the file
        {
            pstFile_->bEOF = TRUE;
            return FAT32_ERROR_EOF;
        }
    }

	// Calculate the LBA of the next cluster, and read the first
	// sector of that cluster.
    ulClusterLBA =  pstDriver_->ulFirstCluster +
            ((ulNextFAT - pstDriver_->ulClusterOffset) *
            pstDriver_->ucSectorsPerCluster);

	pstDriver_->ucCurrentSector = 0;

	FAT32_ReadSector(pstDriver_, ulClusterLBA);
	return FAT32_RETURN_OK;
}
//---------------------------------------------------------------------------
/*!
    Read a 32-byte directory record (index 0-15) from the current directory
    sector.  Fails if the entry is not a valid file or folder (Long filename, blank
    sector, or end-of-directory record)
    \fn static FAT32_RETURN_CODE FAT32_DirReadRecord(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR *aucRecord_)
    \param pstDriver_ - pointer to the FAT device driver
    \param aucRecord_ - char array that will receive the 32-byte directory entry
    \return FAT32_RETURN_CODE - FAT32_RETURN_OK on success, other on fail
*/
static FAT32_RETURN_CODE FAT32_DirReadRecord(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR *aucRecord_)
{
	USHORT i;
	USHORT usOffset = (USHORT)(pstDriver_->ucDirRecord) * DIR_RECORD_SIZE;
	FAT32_RETURN_CODE eReturn = FAT32_RETURN_OK;

    // Can only read a directory if the partition is mounted
	if (pstDriver_->eState != FAT32_STATE_PARTITION_MOUNTED)
	{
		return FAT32_ERROR_WRONG_STATE;
	}

    // If the folder has been fully read, return EOF.
    if (pstDriver_->stFolder.bEOF == TRUE)
    {
        return FAT32_ERROR_EOF;
    }

    // Check for invalid entries...
    if (pstDriver_->aucSectorData[usOffset] == FOLDER_UNUSED)
    {
        // This record is unused, ignore.
        eReturn = FAT32_RECORD_UNUSED;
    }
    else if (pstDriver_->aucSectorData[usOffset] == FOLDER_END)
    {
        // This record tells us the end of directory has been reached
        eReturn = FAT32_END_OF_DIRECTORY;
        pstDriver_->stFolder.bEOF = TRUE;
    }
    else if ((pstDriver_->aucSectorData[usOffset + ATTRIB_OFFSET] & ATTRIBUTE_LONG_FILE) == ATTRIBUTE_LONG_FILE)
    {
        // We only support 8.3 filenames.
        eReturn = FAT32_NOT_SUPPORTED;
    }
    else    // A valid entry
    {
        // Copy out the directory record
        for (i = 0; i < DIR_RECORD_SIZE; i++)
        {
            aucRecord_[i] = pstDriver_->aucSectorData[usOffset + i];
        }
    }

	// Increment the directory record index
	pstDriver_->ucDirRecord++;
	if (pstDriver_->ucDirRecord	>= RECORDS_PER_SECTOR)
	{
		pstDriver_->ucDirRecord = 0;
		pstDriver_->ucCurrentSector++;

		if (pstDriver_->ucCurrentSector >= pstDriver_->ucSectorsPerCluster)
		{
			// If we're at the end of the cluster, go to the next cluster
			// (read from the FAT), and read the first sector
			eReturn = FAT32_ReadNextCluster(pstDriver_, &(pstDriver_->stFolder));
		}
		else
		{
			// Read the next sector in the cluster
			ULONG ulClusterLBA;

			// Get the sector address
			ulClusterLBA =  pstDriver_->ulFirstCluster +
						((pstDriver_->stFolder.ulCurrentFAT - pstDriver_->ulClusterOffset) *
						(ULONG)pstDriver_->ucSectorsPerCluster) +
						(ULONG)pstDriver_->ucCurrentSector;

			eReturn = FAT32_ReadSector(pstDriver_, ulClusterLBA);
		}
	}

	return eReturn;
}
//---------------------------------------------------------------------------
/*!
    Loads the first sector of the specified directory.
    \fn static FAT32_RETURN_CODE FAT32_ChangeDir(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR ucDirIdx_)
    \param pstDriver_ - pointer to the FAT device driver
    \param ucDirIdx_ - index of the directory record in the current sector (0-15)
    \return FAT32_RETURN_CODE - FAT32_RETURN_OK on success, fail otherwise
*/
static FAT32_RETURN_CODE FAT32_ChangeDir(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR ucDirIdx_)
{
    USHORT usHigh;
    USHORT usLow;
    ULONG ulDirLBA;
    UCHAR ucAttrib;
    ULONG ulSector;

	if (pstDriver_->eState != FAT32_STATE_PARTITION_MOUNTED)
	{
		return FAT32_ERROR_WRONG_STATE;
	}

    if (ucDirIdx_ > RECORDS_PER_SECTOR)
    {
        return FAT32_RETURN_FAIL;
    }

    // Make sure that this is a directory
    ucAttrib = pstDriver_->aucSectorData[(ucDirIdx_ * FILE_RECORD_SIZE) + ATTRIB_OFFSET];
    if ((ucAttrib & ATTRIBUTE_DIRECTORY) != ATTRIBUTE_DIRECTORY)
    {
        return FAT32_ERROR_NOT_A_DIR;
    }

    // Read the information @ the current sector's file index...
    usHigh = *(USHORT*)(&pstDriver_->aucSectorData[(ucDirIdx_ * FILE_RECORD_SIZE) + CLUSTER_HIGH_OFFSET]);
    usLow = *(USHORT*)(&pstDriver_->aucSectorData[(ucDirIdx_ * FILE_RECORD_SIZE) + CLUSTER_LOW_OFFSET]);

    ulDirLBA = ((ULONG)usHigh) << 16;
    ulDirLBA |= ((ULONG)usLow);

    pstDriver_->stFolder.ulCurrentFAT = ulDirLBA;
    pstDriver_->stFolder.ulHeadFAT = ulDirLBA;
    pstDriver_->stFolder.bEOF = FALSE;
    pstDriver_->stFolder.ulReadBytes = *(ULONG*)(&pstDriver_->aucSectorData[(ucDirIdx_ * FILE_RECORD_SIZE) + FILE_SIZE_OFFSET]);

    // Read the first sector of the new directory
    if (pstDriver_->stFolder.ulCurrentFAT == 0)
    {
        pstDriver_->stFolder.ulCurrentFAT = 2;
        pstDriver_->stFolder.ulHeadFAT = 2;
    }

    ulSector =  pstDriver_->ulFirstCluster +
                ((pstDriver_->stFolder.ulCurrentFAT - pstDriver_->ulClusterOffset) *
                (ULONG)pstDriver_->ucSectorsPerCluster);

    pstDriver_->ucCurrentSector = 0;
    pstDriver_->ucDirRecord = 0;

    FAT32_ReadSector(pstDriver_, ulSector);

    return FAT32_RETURN_OK;
}
//---------------------------------------------------------------------------
/*!
    Loads the first sector of the specified file, and enters the file state.
    Since only 1 file/folder can be open at a time, state checking verifies that
    this operation is allowed.
    \fn static FAT32_RETURN_CODE FAT32_OpenFile(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR ucFileIdx_)
    \param pstDriver_
    \param ucFileIdx_
    \return FAT32_RETURN_CODE
*/
static FAT32_RETURN_CODE FAT32_OpenFile(FAT32_DRIVER_STRUCT *pstDriver_, UCHAR ucFileIdx_)
{
    USHORT usHigh;
    USHORT usLow;
    ULONG ulDirLBA;
    UCHAR ucAttrib;
    ULONG ulSector;

	if (pstDriver_->eState != FAT32_STATE_PARTITION_MOUNTED)
	{
		return FAT32_ERROR_WRONG_STATE;
	}

    if (ucFileIdx_ > RECORDS_PER_SECTOR)
    {
        return FAT32_RETURN_FAIL;
    }

    // Make sure that this is a directory
    ucAttrib = pstDriver_->aucSectorData[(ucFileIdx_ * FILE_RECORD_SIZE) + ATTRIB_OFFSET];
    if ((ucAttrib & ATTRIBUTE_DIRECTORY) == ATTRIBUTE_DIRECTORY)
    {
        return FAT32_ERROR_NOT_A_FILE;
    }

    // Read the information @ the current sector's file index...
    usHigh = *(USHORT*)(&pstDriver_->aucSectorData[(ucFileIdx_ * FILE_RECORD_SIZE) + CLUSTER_HIGH_OFFSET]);
    usLow = *(USHORT*)(&pstDriver_->aucSectorData[(ucFileIdx_ * FILE_RECORD_SIZE) + CLUSTER_LOW_OFFSET]);

    ulDirLBA = ((ULONG)usHigh) << 16;
    ulDirLBA |= ((ULONG)usLow);

    pstDriver_->stFile.ulCurrentFAT = ulDirLBA;
    pstDriver_->stFile.ulHeadFAT = ulDirLBA;
    pstDriver_->stFile.bEOF = FALSE;
    pstDriver_->stFile.ulFileSize = *(ULONG*)(&pstDriver_->aucSectorData[(ucFileIdx_ * FILE_RECORD_SIZE) + FILE_SIZE_OFFSET]);
    pstDriver_->stFile.ulReadBytes = 0;

    // Read the first sector of the new file
    ulSector =  pstDriver_->ulFirstCluster +
                    ((ulDirLBA-pstDriver_->ulClusterOffset) * (ULONG)pstDriver_->ucSectorsPerCluster);

    pstDriver_->ucCurrentSector = 0;
    pstDriver_->usSectorReadAddr = 0;
    pstDriver_->ucDirRecord = 0;

    FAT32_ReadSector(pstDriver_, ulSector);

    // Change to the FILE state - cannot access directories now.
    pstDriver_->eState = FAT32_STATE_FILE_OPEN;

    return FAT32_RETURN_OK;
}
//---------------------------------------------------------------------------
/*!
    Closes the driver's currently open file stream, returning the driver to the
    directory state.
    \fn static FAT32_RETURN_CODE FAT32_CloseFile(FAT32_DRIVER_STRUCT *pstDriver_)
    \param pstDriver_ - pointer to the device driver
    \return FAT32_RETURN_CODE - FAT32_RETURN_OK on success, other on fail
*/
static FAT32_RETURN_CODE FAT32_CloseFile(FAT32_DRIVER_STRUCT *pstDriver_)
{
    ULONG ulSector;
 	if (pstDriver_->eState != FAT32_STATE_FILE_OPEN)
	{
		return FAT32_ERROR_WRONG_STATE;
	}

    // Read the first sector of the new directory
    ulSector =  pstDriver_->ulFirstCluster +
                ((pstDriver_->stFolder.ulHeadFAT - pstDriver_->ulClusterOffset) *
                (ULONG)pstDriver_->ucSectorsPerCluster);

    pstDriver_->ucCurrentSector = 0;
    pstDriver_->ucDirRecord = 0;

    FAT32_ReadSector(pstDriver_, ulSector);

    pstDriver_->eState = FAT32_STATE_PARTITION_MOUNTED;

    return FAT32_RETURN_OK;
}

//---------------------------------------------------------------------------
/*!
    Seeks to the first sector of the current directory listing, and reloads it.
    This effectively allows the directory contents to be read from start to
    finish.
    \fn static FAT32_RETURN_CODE FAT32_SeekBeginFolder(FAT32_DRIVER_STRUCT *pstDriver_)
    \param pstDriver_ - pointer to the fat driver struct
    \return FAT32_RETURN_CODE
*/
static FAT32_RETURN_CODE FAT32_SeekBeginFolder(FAT32_DRIVER_STRUCT *pstDriver_)
{
    ULONG ulSector;

	if (pstDriver_->eState != FAT32_STATE_PARTITION_MOUNTED)
	{
		return FAT32_ERROR_WRONG_STATE;
	}

    // Seek to the beginning of the current directory (relist)
    pstDriver_->stFolder.ulCurrentFAT = pstDriver_->stFolder.ulHeadFAT;
    pstDriver_->stFolder.bEOF = FALSE;

    // Read the first sector of the new directory
    ulSector =  pstDriver_->ulFirstCluster +
                ((pstDriver_->stFolder.ulCurrentFAT - 0) * //pstDriver_->ulClusterOffset) *
                (ULONG)pstDriver_->ucSectorsPerCluster);

    pstDriver_->ucCurrentSector = 0;
    pstDriver_->ucDirRecord = 0;

    FAT32_ReadSector(pstDriver_, ulSector);

    return FAT32_RETURN_OK;
}

//---------------------------------------------------------------------------
/*!
    Change directory to the root folder in the partition.  Uses the root folder
    sector index stored in the driver structure.
    \fn static FAT32_RETURN_CODE FAT32_SeekRootFolder(FAT32_DRIVER_STRUCT *pstDriver_)
    \param pstDriver_
    \return FAT32_RETURN_CODE
*/
//---------------------------------------------------------------------------
static FAT32_RETURN_CODE FAT32_SeekRootFolder(FAT32_DRIVER_STRUCT *pstDriver_)
{
    ULONG ulSector;

	if (pstDriver_->eState != FAT32_STATE_PARTITION_MOUNTED)
	{
		return FAT32_ERROR_WRONG_STATE;
	}

    // Seek to the beginning of the current directory (relist)
    pstDriver_->stFolder.ulCurrentFAT = 2;
    pstDriver_->stFolder.ulHeadFAT = 2;
    pstDriver_->stFolder.bEOF = FALSE;

    if (pstDriver_->eMode == FAT_MODE_16)
    {
        // Read the first sector of the new directory
        ulSector = pstDriver_->ulRootBegin;
    }
    else
    {
        ulSector = pstDriver_->ulFirstCluster;
    }

    pstDriver_->ucCurrentSector = 0;
    pstDriver_->ucDirRecord = 0;

    FAT32_ReadSector(pstDriver_, ulSector);

    return FAT32_RETURN_OK;
}

