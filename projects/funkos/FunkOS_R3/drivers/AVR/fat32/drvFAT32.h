//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2010, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*!
    Contains the definitions for a FAT32 driver for FunkOS.  Currently, the
    driver is designed for single file-handle, read-only operation, but could
    be extended for use with multiple file handles, and Read/Write operations
*/
//---------------------------------------------------------------------------

#include "types.h"
#include "driver.h"

#ifndef __DRV_FAT32_H_
#define __DRV_FAT32_H_

//---------------------------------------------------------------------------
// Sector size - this is fundamental to the implementation to achieve a 
// minimum memory footprint
//---------------------------------------------------------------------------
#define FAT32_SECTOR_SIZE           (512)

//---------------------------------------------------------------------------
// Definitions used for reading the partition information off of a drive's MBR
//---------------------------------------------------------------------------
// The Drive's MBR is 512 bytes long - the first 446 are boot code, followed
// by 4 evenly-sized partition table entries, and lastly, a signature that
// is used to validate the MBR
//---------------------------------------------------------------------------
#define MBR_PARTITION_SIZE          (16)

#define MBR_PARTITION1_ADDR         (446)
#define MBR_PARTITION2_ADDR         (MBR_PARTITION1_ADDR + MBR_PARTITION_SIZE)
#define MBR_PARTITION3_ADDR         (MBR_PARTITION2_ADDR + MBR_PARTITION_SIZE)
#define MBR_PARTITION4_ADDR         (MBR_PARTITION3_ADDR + MBR_PARTITION_SIZE)

#define PARTITION_CODE_OFFSET       (4)
#define LBA_BEGIN_OFFSET            (8)
#define PARTITION_SECTOR_OFFSET_16	(32)
#define PARTITION_SECTOR_OFFSET_32	(12)


// There are 2 valid partition codes for FAT32 partitions
#define PARTITION_CODE_FAT32_A      (0x0B)
#define PARTITION_CODE_FAT32_B      (0x0C)

#define PARTITION_CODE_FAT16_A      (0x0E)
#define PARTITION_CODE_FAT16_B      (0x06)
// Signature for the MBR record
#define FAT32_SIGNATURE_1           (0x55)
#define FAT32_SIGNATURE_2           (0xAA)

//---------------------------------------------------------------------------
// Definitions used for reading the FAT32 Volume ID
//---------------------------------------------------------------------------
// The volume ID is the first sector in the FAT32 parititon.  This is the
// address that is pointed to by the entry in the MBR table
//---------------------------------------------------------------------------
#define BYTES_PER_SEC_OFFSET        (0x000B)		//!< Offset to the number of bytes per sector offset
#define SECTORS_PER_CLUSETER_OFFSET (0x000D)		//!< Offset to the number of sectors per cluster offset
#define RESERVED_SECTOR_OFFSET      (0x000E)		//!< Offset to the number of reserved sectors
#define FAT_COUNT_OFFSET            (0x0010)		//!< Offset to the number of FAT sectors count
#define SECTORS_PER_FAT_OFFSET      (0x0024)		//!< Offset to the number of sectors per FAT
#define ROOT_DIRECTORY_OFFSET       (0x002C)		//!< Offset to the location of the root directory
#define SIGNATURE_OFFSET            (0x01FE)		//!< Offset to the signature, which validates the block

#define SECTORS_PER_FAT_OFFSET_16   (0x0016)
#define SECTORS_PER_TRACK_16        (0x0018)
#define SECTORS_PER_PARTITION_16    (0x0020)
#define ROOT_ENTRIES_16             (0x0011)

//---------------------------------------------------------------------------
// Attribute definitions
//---------------------------------------------------------------------------
#define ATTRIBUTE_READONLY          (0x01)
#define ATTRIBUTE_HIDDEN            (0x02)
#define ATTRIBUTE_SYSTEM            (0x04)
#define ATTRIBUTE_DIRECTORY         (0x10)

#define ATTRIBUTE_LONG_FILE         (0x0F)

//---------------------------------------------------------------------------
// Definitions for directory records
//---------------------------------------------------------------------------
#define FOLDER_UNUSED               (0xE5)
#define FOLDER_END                  (0x00)

//---------------------------------------------------------------------------
// Definitions used for reading directory entries
//---------------------------------------------------------------------------
// Size of the directory record
#define DIR_RECORD_SIZE				(32)
#define FILE_RECORD_SIZE            (DIR_RECORD_SIZE)
#define RECORDS_PER_SECTOR          (16)

#define FILENAME_OFFSET				(0x0000)		//!< Offset to the filename in the block
#define ATTRIB_OFFSET				(0x000B)		//!< Offset to the attribute byte
#define CLUSTER_HIGH_OFFSET			(0x0014)		//!< Cluster address (high 16-bits)
#define CLUSTER_LOW_OFFSET			(0x001A)		//!< Cluster address (low 16-bits)
#define FILE_SIZE_OFFSET			(0x001C)		//!< Offset to the file size (max 32 bits)

//---------------------------------------------------------------------------
// 
typedef enum
{
    FAT32_STATE_UNINIT = 0,         //!< Uninitialized, media has not been mounted
    FAT32_STATE_MBR_READ,           //!< Drive inactive - MBR has been read, but no partition mounted
    FAT32_STATE_PARTITION_MOUNTED,  //!< Partition mounted, drive active
	FAT32_STATE_FILE_OPEN,			//!< A file is currently open
    FAT32_STATES                    //!< Count of FAT32_STATE enums
} FAT32_STATE;

//---------------------------------------------------------------------------
// Device driver commands
typedef enum
{
    FAT32_CMD_INIT_MEDIA = 0,
    FAT32_CMD_MOUNT_PARTITION,
    FAT32_CMD_READ_SECTOR,
	FAT32_CMD_WRITE_SECTOR,
    FAT32_CMD_READ_NEXT_DIR,
    FAT32_CMD_SEEK_BEGIN_DIR,
    FAT32_CMD_CHDIR,
    FAT32_CMD_CHDIR_BACK,
	FAT32_CMD_CHDIR_ROOT,
	FAT32_CMD_OPEN_FILE_READ,
	FAT32_CMD_OPEN_FILE_WRITE,
	FAT32_CMD_CLOSE_FILE,
} FAT32_COMMAND;

//---------------------------------------------------------------------------
// Error codes
typedef enum
{
	FAT32_RETURN_FAIL = 0,			//!< Catch-all error code
	FAT32_RETURN_OK,				//!< The operation was successful
	FAT32_READ_ERROR_NO_MEDIA,		//!< Failure on read - the media is not present
	FAT32_INIT_ERROR_NO_PARTITIONS,	//!< Failure on init - no FAT32 partitions are present
	FAT32_ERROR_WRONG_STATE,		//!< General failure - the driver is in the wronge state for the operation
	FAT32_ERROR_SECTOR_SIZE,		//!< Wrong sector size - we only support 512 byte sectors
    FAT32_ERROR_FOLDER_TOO_DEEP,    //!< Folder is too many subdirectories up
	FAT32_ERROR_EOF,                //!< End of file/directory reached.
    FAT32_ERROR_NOT_A_DIR,          //!< Cannot change directory to a file
    FAT32_ERROR_NOT_A_FILE,         //!< Cannot open a directory like a file
    FAT32_RECORD_UNUSED,            //!< This is an unused record
    FAT32_END_OF_DIRECTORY,         //!< The end-of-directory record is reached
    FAT32_NOT_SUPPORTED,            //!< Long file name record not supported.
} FAT32_RETURN_CODE;

//---------------------------------------------------------------------------
// Define 16 or 32-bit fat modes
typedef enum
{
    FAT_MODE_16,
    FAT_MODE_32
} FAT_MODE;

//---------------------------------------------------------------------------
// File system object
typedef struct
{
    UCHAR   aucFileName[12];    //!< 8.3 format filename

    ULONG   ulHeadFAT;          //!< pointer to the file's beginning FAT
    ULONG   ulCurrentFAT;       //!< pointer to the current FAT 

    ULONG   ulFileSize;         //!< Size of the file in bytes
    ULONG   ulReadBytes;        //!< Number of bytes read
	BOOL	bEOF;				//!< End of file flag
} FAT32_FILE_STRUCT;

//---------------------------------------------------------------------------
// Directory "stack" for containing the start of the parent directories
// These are cached for speed.
typedef struct
{
    UCHAR ucStackDepth;         //!< Current directory stack depth

    ULONG aulParentCluster[8];  //!< Parent cluster addresses
    ULONG aulParentFAT[8];      //!< Parent FAT values
} FAT32_DIR_STACK;

//---------------------------------------------------------------------------
// Device driver implementation using the FunkOS driver framework
typedef struct
{
    DRIVER_STRUCT stDriver;         //!< Must be first!!!

    ULONG   ulSectorsPerFAT;        //!< Number of sectors per File Allocation Table
    ULONG   ulFATBegin;             //!< Address of the first FAT cluster
	ULONG   ulFirstCluster;         //!< Cluster representing the first directory on the partition
    ULONG   ulRootBegin;         	//!< Address of the first Data cluster
    ULONG   ulClusterOffset;       //!< Number of clusters to subtract when calculating the secotor location

    USHORT  usNumReservedSectors;   //!< Number of reserved sectors

    UCHAR   ucSectorsPerCluster;    //!< Number of 512 byte sectors per cluster

    UCHAR   aucSectorData[FAT32_SECTOR_SIZE];     //!< buffer for a sector of data

    UCHAR   *szMountPoint;          //!< Mount point in the OS's filesystem

    UCHAR   ucValidPartitions;      //!< Bits representing the valid partitions on the media
    UCHAR   ucMountedPartition;     //!< Decimal representing the partition mounted in the filesystem

	UCHAR 	ucDirRecord;			//!< Index of the current directory record [0-15]
	UCHAR   ucCurrentSector;    	//!< Current sector in the cluster

    USHORT  usSectorReadAddr;       //!< Sector read pointer (file access)

    FAT_MODE eMode;                 //!< FAT mode (16 or 32)
	FAT32_STATE eState;				//!< State of the driver

    FAT32_FILE_STRUCT stFile;       //!< File handle to a file object
    FAT32_FILE_STRUCT stFolder;     //!< File handle for the current folder

} FAT32_DRIVER_STRUCT;

//---------------------------------------------------------------------------
void FAT32_Init(DRIVER_STRUCT *pstDriver_);
void FAT32_Start(DRIVER_STRUCT *pstDriver_);
void FAT32_Stop(DRIVER_STRUCT *pstDriver_);
USHORT FAT32_Control(DRIVER_STRUCT *pstDriver_, USHORT usID_, void *pvData_);
USHORT FAT32_Read(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_);
USHORT FAT32_Write(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_);

#endif



