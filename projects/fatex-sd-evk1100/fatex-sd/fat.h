/* This header file is part of the ATMEL AVR32-SoftwareFramework-1.2.1ES-AT32UC3A Release */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief FAT services
 *
 * This file is the header for FAT services
 *
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 *                       The example is written for UC3 and EVK1100.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 *****************************************************************************/

/* Copyright (c) 2007, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FAT_H_
#define _FAT_H_

#include "fs_com.h"

//! @verbatim
//! - File system vocabulary :
//! MBR : Master Boot Record (constains four PE)
//! PE  : Partition Entry (constains a location informations about PBR)
//! PBR : Partition Boot Record
//! BPB : BIOS Parameter Block (see Hardware White Paper FAT)
//! PBR = BPB
//! FAT : File Allocation Table
//! @endverbatim


#undef _GLOBEXT_
#if (defined _fat_c_)
#  define _GLOBEXT_
#else
#  define _GLOBEXT_ extern
#endif

//_____ I N C L U D E S ____________________________________________________


//_____ M A C R O S ________________________________________________________


// To optimize the code
#if (FS_FAT_12 == ENABLED)
#  define   Is_fat12    (FS_TYPE_FAT_12 == fs_g_nav_fast.u8_type_fat)
#else
#  define   Is_fat12    (0)
#endif
#if (FS_FAT_16 == ENABLED)
#  define   Is_fat16    (FS_TYPE_FAT_16 == fs_g_nav_fast.u8_type_fat)
#else
#  define   Is_fat16    (0)
#endif
#if (FS_FAT_32 == ENABLED)
#  define   Is_fat32    (FS_TYPE_FAT_32 == fs_g_nav_fast.u8_type_fat)
#else
#  define   Is_fat32    (0)
#endif

// Tor optimize the code
#if ( (FS_ASCII   == ENABLED) && (FS_UNICODE == ENABLED))
#  define   Is_unicode  (g_b_unicode)
#elif (FS_ASCII   == ENABLED)
#  define   Is_unicode  (0)
#elif (FS_UNICODE == ENABLED)
#  define   Is_unicode  (1)
#else
#  error You must define FS_ASCII or/and FS_UNICODE enable in conf_explorer.h
#endif


//_____ D E F I N I T I O N S ______________________________________________


//**** Definitions corresponding at the FAT norm ****

//! Position (unit byte) in the MBR of a partition entry
#define  FS_MBR_OFFSET_PART_ENTRY( num )  ((U16)((U16)(0x1BE)+(0x10 * num)))  // Partition entry num (0 to 4)


//! \name Macro to access at fields in BPB sector (only used in fat_mount() function)
//! The name prefixed by "BPB_" are defined in "Hardware White Paper FAT"
//! @{
#define  LOW_16_BPB_BytsPerSec   fs_g_sector[11]
#define  HIGH_16_BPB_BytsPerSec  fs_g_sector[12]
#define  U8_BPB_SecPerClus       fs_g_sector[13]
#define  LOW_16_BPB_RootEntCnt   fs_g_sector[17]
#define  HIGH_16_BPB_RootEntCnt  fs_g_sector[18]
#define  LOW_16_BPB_FATSz16      fs_g_sector[22]
#define  HIGH_16_BPB_FATSz16     fs_g_sector[23]
#define  LOW0_32_BPB_FATSz32     fs_g_sector[36]
#define  LOW1_32_BPB_FATSz32     fs_g_sector[37]
#define  LOW2_32_BPB_FATSz32     fs_g_sector[38]
#define  LOW3_32_BPB_FATSz32     fs_g_sector[39]
#define  LOW_16_BPB_TotSec16     fs_g_sector[19]
#define  HIGH_16_BPB_TotSec16    fs_g_sector[20]
#define  LOW0_32_BPB_TotSec32    fs_g_sector[32]
#define  LOW1_32_BPB_TotSec32    fs_g_sector[33]
#define  LOW2_32_BPB_TotSec32    fs_g_sector[34]
#define  LOW3_32_BPB_TotSec32    fs_g_sector[35]
#define  LOW_16_BPB_ResvSecCnt   fs_g_sector[14]
#define  HIGH_16_BPB_ResvSecCnt  fs_g_sector[15]
#define  U8_BPB_NumFATs          fs_g_sector[16]
#define  LOW0_32_BPB_RootClus    fs_g_sector[44]
#define  LOW1_32_BPB_RootClus    fs_g_sector[45]
#define  LOW2_32_BPB_RootClus    fs_g_sector[46]
#define  LOW3_32_BPB_RootClus    fs_g_sector[47]
//! @}


//! \name Constante used to sign a MBR or PBR sectors
//! @{
#define  FS_BR_SIGNATURE_LOW     0x55
#define  FS_BR_SIGNATURE_HIGH    0xAA
//! @}


//! \name Constants used in MBR sector
//! @{
#define  FS_PART_BOOTABLE           0x80
#define  FS_PART_NO_BOOTABLE        0x00
/*
   Partition  Fdisk                                               D�marrage dans
   Type       Rapports      Taille                 Type de FAT    version
   -----------------------------------------------------------------------------
   01         PRI DOS       0-15 Mo                12 bits        MS-DOS 2.0
   04         PRI DOS       16-32 Mo               16 bits        MS-DOS 3.0
   05         EXT DOS       0-2 Go                 n/a            MS-DOS 3.3
   06         PRI DOS       32 Mo-2 Go             16 bits        MS-DOS 4.0
   0E         PRI DOS       32 Mo-2 Go             16 bits        Windows 95
   0F         EXT DOS       0-2 Go                 n/a            Windows 95
   0B         PRI DOS       512 Mo - 2 t�raoctets  32 bits        OSR2
   0C         EXT DOS       512 Mo - 2 t�raoctets  32 bits        OSR2
*/
#define  FS_PART_TYPE_FAT12         0x01
#define  FS_PART_TYPE_FAT16_INF32M  0x04
#define  FS_PART_TYPE_FAT16_SUP32M  0x06
#define  FS_PART_TYPE_FAT16_SUP32M_BIS  0x0E
#define  FS_PART_TYPE_FAT32         0x0B
#define  FS_PART_TYPE_FAT32_BIS     0x0C
#define  FS_PART_REMOVE_MEDIA       0xF0     // removal media
#define  FS_PART_NO_REMOVE_MEDIA    0xF8     // no removal media
#define  FS_PART_HARD_DISK          0x81     // hard disk
#define  FS_BOOT_SIGN               0x29     // Boot signature
//! @}


//! \name Maximum of FAT cluster
//! @{
#define  FS_FAT12_MAX_CLUSTERS      4085     // Maximum of cluster for FAT 12
#define  FS_FAT16_MAX_CLUSTERS      65525    // Maximum of cluster for FAT 16
//! @}


//! \name Constants used in the first byte of file entry
//! @{
#define  FS_ENTRY_END               0x00     // end of directory
#define  FS_ENTRY_DEL               0xE5     // deleted entry
#define  FS_ENTRY_LFN_LAST          0x40     // mask to detect the last long name entry
//! @}


//! \name Constantes used to manage the file entry
//! @{
#define  FS_SIZE_FILE_ENTRY         32       // Size of the file entry
#define  FS_SHIFT_B_TO_FILE_ENTRY    5       // Shift a unit byte to unit entry file (32,<<5) to unit sector 512B (512,>>9)
#define  FS_SIZE_LFN_ENTRY          13       // Size of name stored in the file entry "long file name" (unit UNICODE = 2bytes)
#define  FS_SIZE_SFNAME             11       // Size of name stored in the file entry "short file name" (unit byte)
#define  FS_SIZE_SFNAME_WITHOUT_EXT  8       // Size of name (without extension) stored in the file entry "short file name" (unit byte)
#define  FS_SIZE_SFNAME_EXT_ONLY     3       // Size of extension name stored in the file entry "short file name" (unit byte)
//! @}


//! \name LIMITATIONS OF FILE SYSTEM
//! @{
#define  FS_NB_FAT                  2        // This file system managed only 2 FAT
//! @}


//**** Definitions of function configurations

//! \name The unit sector of 512B is many used in file System stack
//! @{
#define  FS_512B                    512
#define  FS_512B_MASK               (512-1)
#define  FS_512B_SHIFT_BIT          9        // Shift a unit byte to unit sector (512,>>9)
#define  FS_SIZE_OF_SECTOR          FS_512B           // For compliance with old FileSystem module
#define  FS_MASK_SIZE_OF_SECTOR     FS_512B_MASK      // For compliance with old FileSystem module
#define  FS_SHIFT_B_TO_SECTOR       FS_512B_SHIFT_BIT // For compliance with old FileSystem module
#define  FS_CACHE_SIZE              512      // Cache size used by module (unit 512B)
//! @}

//! Signal that sector cache is not valid
#define  FS_BUF_SECTOR_EMPTY        0xFF


//! \name Status of the fat_checkcluster() function
//! @{
#define  FS_CLUS_OK                 0        // Value correct
#define  FS_CLUS_BAD                1        // Value bad
#define  FS_CLUS_END                2        // It is the end of cluster list
//! @}


//! \name Options of the fat_cluster_list() function
//! @{
#define  FS_CLUST_ACT_SEG           0x01     // Get the sector address and size of the cluster list
#define  FS_CLUST_ACT_ONE           0x02     // Get the sector address of the cluster list
#define  FS_CLUST_ACT_CLR           0x03     // Clear the cluster list
//! @}


//! \name Options of the fat_cluster_val() function
#define  FS_CLUST_VAL_READ          FALSE      // Mode read
#define  FS_CLUST_VAL_WRITE         TRUE       // Mode write
#define  FS_CLUST_VAL_EOL           0x0FFFFFFF // Value to signal the end of cluster list
//! @}


//! \name Structures used to store the information about File System mount
//! @{

//! Union to define a root directory
typedef union
{
   U32  u32_cluster;                   //!< For FAT32, the root directory is a cluster list
   struct                             
   {                                  
      U16  u16_pos;                    //!< Offset between the beginning of FAT and the beginning of root dir (unit 512B)
      U16  u16_size;                   //!< Size of root (unit 512B)
   } seg;                              //!< For FAT 12 & 16, it is a segment (no cluster list)
} Fs_rootdir;

//! Struture to save the variables frequently used by file system mounted
typedef struct
{
   U8    u8_lun;                       //!< Number of logical driver
#if (FS_MULTI_PARTITION == ENABLED)
   U8    u8_partition;                 //!< Number of partition - 1 (0 or 1)
#endif
   U8    u8_BPB_SecPerClus;            //!< Cluster size (unit 512B)
   // The pointers start at beginning of the memory, and unit = 512B
   U16   u16_fat_size;                 //!< Size of one FAT (unit 512B)
   U32   u32_CountofCluster;           //!< Number of cluster (include the two reserved cluster)
   U32   u32_ptr_fat;                  //!< FAT address (unit 512B)
   U32   u32_offset_data;              //!< Offset between the beginning of FAT and the first cluster (unit 512B)
   Fs_rootdir     rootdir;             //!< Root directory informations
   U32   u32_cluster_sel_dir;          //!< First cluster number of selected directory (0 for the root directory)
   U16   u16_pos_sel_file;             //!< File position in the file list (only used by navigation functions)
   U16   u16_pos_filterlist;           //!< File position in the file list filtered (only for nav_filterlist functions)
   FS_STRING sz_filterext;             //!< pointer on extension filter to use in nav_filterlist functions
   U8    b_mode_nav;                   //!< Navigation step ( FS_FILE or FS_DIR )
#ifdef NAVIGATION_MODE_FLAT
   U8    u8_flat_dir_level;            //!< Directory level of the current dir in flat list
   U16   u16_flat_pos_offset;          //!< Offset in flat list of the directory
#endif
} Fs_management;

//! Struture to save the variables very frequently used by file system mounted
typedef struct
{
   U8    u8_type_fat;                  //!< FAT type (default = no mounted = FS_TYPE_FAT_UNM)
   U16   u16_entry_pos_sel_file;       //!< Entry file position in directory (unit = FS_SIZE_FILE_ENTRY) (see value FS_NO_SEL & FS_END_FIND)
} Fs_management_fast;

//! Struture to save the frequently variables of file system mounted
typedef struct
{
   U8    u8_open_mode;                 //!< open mode of selected file
   U8    u8_txt_format;                //!< format of text used in selected file (only for reader_txt module)
   U8    u8_attr;                      //!< Attribut of the selected file
   U32   u32_cluster;                  //!< First cluster of the selected file
   U32   u32_size;                     //!< Size of selected file (unit Bytes)
   U32   u32_pos_in_file;              //!< Current position in file (unit Bytes)
} Fs_management_entry;
//! @}


//! \name Main sructures
//! @{

//! Struture to define a segment
typedef struct {
   U32  u32_addr;                      //!< segment address (unit 512B), or cluster number
   U32  u32_size_or_pos;               //!< segment size (unit 512B), or position in cluster list (unit 512B)
} Fs_segment;

//! Struture to store cluster information
typedef struct st_fs_cluster
{
   U32   u32_pos;                      //!< cluster position
   U32   u32_val;                      //!< cluster value
} Fs_cluster;

//! @}


//! Struture to store the cluster list cache
typedef struct {
   U8    u8_level_use;                 //!< Cache level, 0 for the last used and up to FS_NB_CACHE_CLUSLIST-1 for the old access (ignore if FS_NB_CACHE_CLUSLIST=1)
   U8    u8_lun;                       //!< LUN of cluster list
   U32   u32_cluster;                  //!< First cluster of cluster list
   U32   u32_start;                    //!< Start position in the cluster list (unit 512B)
   U32   u32_addr;                     //!< Address corresponding at the position "start" in cluster list
   U32   u32_size;                     //!< Cluster list size
} Fs_clusterlist_cache;


//! Struture to store the information about sector cache (=last sector read or write on disk)
typedef struct {
   U8    u8_lun;                       //!< LUN of sector
   U32   u32_addr;                     //!< Sector address (unit 512B)
   U8    u8_modify;                    //!< Cache status
                                       //!< if the sector is a sector from a cluster list THEN
   U32   u32_clusterlist_start;        //!< first cluster of cluster list
   U32   u32_clusterlist_pos;          //!< position in cluster list (unit 512B)
} Fs_sector_cache;


//**** Definition of value used by the STRUCTURES of communication

//! \name FAT type ID, used in "Fs_management_fast.u8_type_fat"
//! @{
#define  FS_TYPE_FAT_UNM   0           //!< Partition not mounted
#define  FS_TYPE_FAT_12    1
#define  FS_TYPE_FAT_16    2
#define  FS_TYPE_FAT_32    3
//! @}


//! \name Value used in "Fs_management_fast.u16_entry_pos_sel_file"
//! @{
#define  FS_NO_SEL         0xFFFF      //!< Signal that a file entry isn't selected
#define  FS_END_FIND       0xFFFE      //!< Signal that a file entry is the last file entry accessibled by system
//! @}


//! \name Macro to check the file open mode
//! @{
#define  Fat_file_is_open     (fs_g_nav_entry.u8_open_mode !=0 )
#define  Fat_file_isnot_open  (fs_g_nav_entry.u8_open_mode ==0 )
#define  Fat_file_close       (fs_g_nav_entry.u8_open_mode  =0 )
//! @}


//_____ D E C L A R A T I O N S ____________________________________________

//**** Global file system variables

//! Variables to select string format (initialised in nav_reset())
_GLOBEXT_                     Bool                 g_b_unicode;
//! Variables to select LENGTH string mode (initialised in nav_reset())
_GLOBEXT_                     Bool                 g_b_string_length;

//! Variables to enable/disable the disk check before each action on disk
_GLOBEXT_                     Bool                 g_b_no_check_disk;

//! \name Variables initialised in drive_mount()
//! @{
_GLOBEXT_   _MEM_TYPE_SLOW_   Fs_management        fs_g_nav;
_GLOBEXT_   _MEM_TYPE_FAST_   Fs_management_fast   fs_g_nav_fast;
_GLOBEXT_   _MEM_TYPE_SLOW_   Fs_management_entry  fs_g_nav_entry;
//! @}

//! Variable frequently used by many function (optimization, no parameter in function)
_GLOBEXT_   _MEM_TYPE_FAST_   Fs_segment           fs_g_seg;

//! To take time in functions: fat_getfreespace, fat_cluster_list, fat_cluster_val, fat_checkcluster
_GLOBEXT_   _MEM_TYPE_FAST_   Fs_cluster           fs_g_cluster;

#if (FS_LEVEL_FEATURES > FSFEATURE_READ)
//! \name Variables used to update the second FAT zone
//! @{
_GLOBEXT_   _MEM_TYPE_SLOW_   U16                  fs_g_u16_first_mod_fat; //!< Offset (unit 512B) in fat of the first sector (unit 512B)
_GLOBEXT_   _MEM_TYPE_SLOW_   U16                  fs_g_u16_last_mod_fat;  //!< Offset (unit 512B) in fat of the last sector (unit 512B)
//! @}
#endif  // FS_LEVEL_FEATURES

//! \name Variables used to manage the sector cache
//! @{
_GLOBEXT_   _MEM_TYPE_SLOW_   U8                   fs_g_sector[ FS_CACHE_SIZE ];   
_GLOBEXT_   _MEM_TYPE_SLOW_   Fs_sector_cache      fs_g_sectorcache;
_GLOBEXT_   _MEM_TYPE_SLOW_   U32                  fs_gu32_addrsector;     //!< Store the address of futur cache (unit 512B)
typedef U8  _MEM_TYPE_SLOW_   * PTR_CACHE;
//!}@




//! \name Functions to verify navigator state
//! @{
Bool        fat_check_device              ( void );
Bool        fat_check_mount               ( void );
Bool        fat_check_noopen              ( void );
Bool        fat_check_open                ( void );
Bool        fat_check_select              ( void );
Bool        fat_check_mount_noopen        ( void );
Bool        fat_check_mount_select_noopen ( void );
Bool        fat_check_mount_select_open   ( void );
Bool        fat_check_mount_select        ( void );
Bool        fat_check_is_file             ( void );
//! @}

//! This function returns the number of partition present on selected drive
U8          fat_get_nbpartition           ( void );

//! This function mounts a partition 
Bool        fat_mount                     ( void );

//! This function formats the drive
Bool        fat_format                    ( U8 u8_fat_type );

//! This function reads or writes a serial number
Bool        fat_serialnumber              ( Bool b_action , U8 _MEM_TYPE_SLOW_ *a_u8_sn );


//! \name Functions to compute free space on a partition
//! @{
U32         fat_getfreespace              ( void );
U8          fat_getfreespace_percent      ( void );
Bool        fat_write_fat32_FSInfo        ( U32 u32_nb_free_cluster );
U32         fat_read_fat32_FSInfo         ( void );
//! @}


//! \name Functions to manage the cluster list
//! @{
Bool        fat_cluster_list              ( U8 opt_action );
void        fat_cache_clusterlist_reset   ( void );
Bool        fat_cluster_val               ( Bool b_mode );
Bool        fat_cluster_readnext          ( void );
U8          fat_checkcluster              ( void );
Bool        fat_allocfreespace            ( void );
void        fat_clear_info_fat_mod        ( void );
Bool        fat_clear_cluster             ( void );
Bool        fat_update_fat2               ( void );
//! @}


//! \name Functions to read or to write a file or a directory
//! @{
Bool        fat_read_file                 ( U8 mode );
Bool        fat_write_file                ( U8 mode , U32 u32_nb_sector_write );
Bool        fat_read_dir                  ( void );
Bool        fat_initialize_dir            ( void );
//! @}


//! \name Functions to manage the entry field (fat.c)
//! @{
Bool        fat_entry_check               ( Bool b_type );
Bool        fat_entry_checkext            ( FS_STRING sz_filter );
void        fat_get_entry_info            ( void );
Bool        fat_entry_is_dir              ( void );
void        fat_clear_entry_info_and_ptr  ( void );
void        fat_write_entry_file          ( void );
Bool        fat_entry_shortname           ( FS_STRING sz_name , U8 u8_size_max , Bool b_mode );
Bool        fat_entry_longname            ( FS_STRING sz_name , U8 u8_size_max , Bool b_mode , Bool b_match_case );
Bool        fat_check_eof_name            ( U16 character );
PTR_CACHE   fat_get_ptr_entry             ( void );
//! @}


//! \name Functions to manage the entry field (fat_unusual.c)
//! @{
Bool        fat_create_entry_file_name    ( FS_STRING sz_name );
void        fat_get_date                  ( FS_STRING sz_date , Bool type_date );
void        fat_set_date                  ( const FS_STRING sz_date , Bool type_date );
Bool        fat_delete_file               ( Bool b_cluster_list );
Bool        fat_entry_label               ( Bool b_action , FS_STRING sz_label );
//! @}


//! \name Functions to manage the cache
//! @{
Bool        fat_cache_read_sector         ( Bool b_load );
void        fat_cache_reset               ( void );
void        fat_cache_clear               ( void );
void        fat_cache_sector_is_modify    ( void );
Bool        fat_cache_flush               ( void );
//! @}


//! \name Functions to control access disk
//! @{
#if (FS_NB_NAVIGATOR > 1)
   Bool     fat_check_nav_access_disk     ( void );
   Bool     fat_check_nav_access_file     ( Bool mode );
#else
# define    fat_check_nav_access_disk(b)  (TRUE)            //! In case of one navigator, function not used
# define    fat_check_nav_access_file(b)  (TRUE)            //! In case of one navigator, function not used
#endif
//! @}


//! \name Functions to manage navigator switch
//! @{
#if (FS_NB_NAVIGATOR > 1)
   void     fat_invert_nav                ( U8 u8_idnav );
   void     fat_copy_nav                  ( U8 u8_idnav );
#else
# define    fat_invert_nav                ( arg )  (arg++)  //! In case of one navigator, function not used
# define    fat_copy_nav                  ( arg )  (arg++)  //! In case of one navigator, function not used
#endif
//! @}

#endif  // _FAT_H_

