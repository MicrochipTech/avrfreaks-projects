/* This source file is part of the ATMEL AVR32-SoftwareFramework-1.2.1ES-AT32UC3A Release */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief FAT 12/16/32 Services.
 *
 * This file defines a useful set of functions for the FAT accesses on
 * AVR32 devices.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
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

#define _fat_c_

//_____  I N C L U D E S ___________________________________________________
#include "conf_explorer.h"
#include "fs_com.h"
#include "fat.h"
#include LIB_MEM
#include LIB_CTRLACCESS


//_____ D E F I N I T I O N S ______________________________________________


//! \name Store navigator datas no selected
//! @{
#if (FS_NB_NAVIGATOR > 1)
_MEM_TYPE_SLOW_     Fs_management       fs_g_navext[FS_NB_NAVIGATOR-1];
_MEM_TYPE_SLOW_     Fs_management_fast  fs_g_navext_fast[FS_NB_NAVIGATOR-1];
_MEM_TYPE_SLOW_     Fs_management_entry fs_g_navext_entry[FS_NB_NAVIGATOR-1];
#endif
//! @}

//! \name Variables to manage cluster list caches
//! @{
_MEM_TYPE_SLOW_     Fs_clusterlist_cache fs_g_cache_clusterlist[FS_NB_CACHE_CLUSLIST];
#if (FS_NB_CACHE_CLUSLIST>1)
   _MEM_TYPE_SLOW_     U8  fs_g_u8_current_cache;
#else
#  define fs_g_u8_current_cache  0
#endif
//! @}

//_____ D E C L A R A T I O N S ____________________________________________


void  fat_cache_clusterlist_update_start  ( void );
void  fat_cache_clusterlist_update_finish ( void );
Bool  fat_cache_clusterlist_update_read   ( void );
void  fat_cache_clusterlist_update_select ( void );



//! This function checks device state
//!
//! @return    TRUE  device ready
//! @return    FALSE otherwise
//!
//! @verbatim
//! This function updates all navigator datas when the device state change.
//! @endverbatim
//!
Bool  fat_check_device( void )
{
   U8 retry=0;
   U8 i;
   Ctrl_status status;

   // Possibility to ignore the disk check. Used to take time during multi read/write access
   if( g_b_no_check_disk )
      return TRUE;

   if( 0xFF == fs_g_nav.u8_lun )
   {
      fs_g_status = FS_ERR_HW;
      return FALSE;                                // No device selected
   }

   for( retry=0 ; retry<100 ; retry++ )
   {
      // Check device
      status = mem_test_unit_ready( fs_g_nav.u8_lun );
      if( CTRL_GOOD       == status )
         return TRUE;                              // drive ready
      
      //* HERE error or state change
      // Clean all navigator datas which use this device
      fs_g_nav_fast.u8_type_fat = FS_TYPE_FAT_UNM; // By default the fat isn't mounted
      Fat_file_close;                              // By default the file is not open
#if (FS_NB_NAVIGATOR > 1)
      for( i=0 ; i!=(FS_NB_NAVIGATOR-1) ; i++ )
      {
         if( fs_g_nav.u8_lun == fs_g_navext[i].u8_lun )
         {
            fs_g_navext_fast[i].u8_type_fat     = FS_TYPE_FAT_UNM;   // By default the fat isn't mounted
            fs_g_navext_entry[i].u8_open_mode   = 0;                 // By default the file is not open
         }
      }
#endif
      // If the internal cache corresponding at device then clean it
      if( fs_g_nav.u8_lun == fs_g_sectorcache.u8_lun )
      {
         fat_cache_reset();
      }
      fat_cache_clusterlist_reset();
      
      fs_g_status = FS_ERR_HW;                     // By default HW error
      if( CTRL_BUSY == status )
         continue;                                 // If device busy then retry
      
      if( CTRL_NO_PRESENT == status )
         fs_g_status = FS_ERR_HW_NO_PRESENT;       // Update error flag
      break;                                       // FAIL or NOT PRESENT = fatal error = no retry
   }
   return FALSE;
}


//! This function checks if the partition is mounted
//!
//! @return    TRUE  partition mounted
//! @return    FALSE otherwise
//!
Bool  fat_check_mount( void )
{
   if( !fat_check_device() )
      return FALSE;
   if (FS_TYPE_FAT_UNM == fs_g_nav_fast.u8_type_fat)
   {
      fs_g_status = FS_ERR_NO_MOUNT;
      return FALSE;
   }
   return TRUE;
}


//! This function checks if a file is not opened on current navigator
//!
//! @return    TRUE  no file opened
//! @return    FALSE otherwise
//!
Bool  fat_check_noopen( void )
{
   if( !fat_check_device() )
      return TRUE;
   if (FS_TYPE_FAT_UNM == fs_g_nav_fast.u8_type_fat)
      return TRUE;
   if( Fat_file_is_open )
   {
      fs_g_status = FS_ERR_TOO_FILE_OPEN;  // The navigation have already open a file
      return FALSE;
   }
   return TRUE;
}


//! This function checks if a file is opened on current navigator
//!
//! @return    TRUE  a file is opened
//! @return    FALSE otherwise
//!
Bool  fat_check_open( void )
{
   if( Fat_file_isnot_open )
   {
      fs_g_status = FS_ERR_FILE_NO_OPEN;
      return FALSE;
   }
   return TRUE;
}


//! This function checks if a file is selected on current navigator
//!
//! @return    TRUE  a file is selected
//! @return    FALSE otherwise
//!
Bool  fat_check_select( void )
{
   if (FS_NO_SEL == fs_g_nav_fast.u16_entry_pos_sel_file)
   {
      fs_g_status = FS_ERR_NO_FILE_SEL;
      return FALSE;
   }
   return TRUE;
}


//! This function checks if the partition is mounted and no file is opened
//!
//! @return    TRUE  partition mounted and no file is opened
//! @return    FALSE otherwise
//!
Bool  fat_check_mount_noopen( void )
{
   if( !fat_check_mount() )
      return FALSE;
   return fat_check_noopen();
}


//! This function checks if the partition is mounted and if no file is opened and a file is selected
//!
//! @return    TRUE  partition mounted and no file is opened and a file is selected
//! @return    FALSE otherwise
//!
Bool  fat_check_mount_select_noopen( void )
{
   if( !fat_check_mount() )
      return FALSE;
   if( !fat_check_select() )
      return FALSE;
   return fat_check_noopen();
}


//! This function checks if the partition is mounted and if a file is opened
//!
//! @return    TRUE  partition mounted and a file is opened
//! @return    FALSE otherwise
//!
Bool  fat_check_mount_select_open( void )
{
   if( !fat_check_mount() )
      return FALSE;
   if( !fat_check_select() )
      return FALSE;
   return fat_check_open();
}


//! This function checks if the partition is mounted and if a file is selected
//!
//! @return    TRUE  partition mounted and a file is selected
//! @return    FALSE otherwise
//!
Bool  fat_check_mount_select( void )
{
   if( !fat_check_mount() )
      return FALSE;
   return fat_check_select();
}


//! This function checks if the selected file entry is a file and not a directory
//!
//! @return    TRUE  It is a file and not a directory
//! @return    FALSE otherwise
//!
Bool  fat_check_is_file( void )
{
   if( Fat_is_not_a_file )
   {
      fs_g_status = FS_ERR_NO_FILE;   // It isn't a file, it is a directory or a volume id
      return FALSE;
   }
   return TRUE;
}


#if (FS_MULTI_PARTITION  ==  ENABLED)
//! This function returns the number of partition on current drive
//!
//! @return    u8_number   number of partition
//!
U8    fat_get_nbpartition( void )
{
   if( !fat_check_device() )
      return 0;
      
#warning this routine contains bug, rework it
   // Read the first sector of drive
   fs_gu32_addrsector = 0;
   if( !fat_cache_read_sector( TRUE ))
      return FALSE;
   
   // Check PBR or MBR signature
   if ( (fs_g_sector[510] != FS_BR_SIGNATURE_LOW  )
   &&   (fs_g_sector[511] != FS_BR_SIGNATURE_HIGH ) )
   {
      // No MBR
      // The sector, is it a PBR ?
      if ( (fs_g_sector[0] == 0xEB) &&          // PBR Byte 0
           (fs_g_sector[2] == 0x90) &&          // PBR Byte 2
           ((fs_g_sector[21] & 0xF0) == 0xF0) ) // PBR Byte 21 : Media byte
      {  
         return 1;   // No MBR but PBR exist then only one partition
      } else {
         return 0;   // No MBR and no PBR then no partition found
      }
   }

   number_part = 0;
   while( 1 )
   {
      // The first sector must be a MBR, then check the partition entry in the MBR
      if ( ((fs_g_sector[FS_MBR_OFFSET_PART_ENTRY(number_part)+0] != FS_PARTITION_ACTIVE) &&
            (fs_g_sector[FS_MBR_OFFSET_PART_ENTRY(number_part)+0] != 0x00))
      ||    (fs_g_sector[FS_MBR_OFFSET_PART_ENTRY(number_part)+4] == 0x00) )
      {
         break;
      }
      number_part++;
   }
   return number_part;
}
#endif


// To clean the code 
#if (FS_NB_CACHE_CLUSLIST==1)
#  define u8_i                   0
#endif

//! This function gets or clears a cluster list
//!
//! @param     opt_action        Choose action on the cluster list <br>
//!            FS_CLUST_ACT_SEG  Get continue memory segment corresponding at cluster list <br>
//!            FS_CLUST_ACT_ONE  Get only one memory sector (512B) corresponding at cluster list <br>
//!            FS_CLUST_ACT_CLR  Clear the cluster list <br>
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
//! @verbatim
//! Global variables used
//! IN : 
//!   fs_g_seg.u32_addr          The first cluster of the cluster list
//!   fs_g_seg.u32_size_or_pos   Start position in the cluster list (unit 512B)
//! OUT:
//!   fs_g_seg.u32_addr          The memory segment address corresponding at the beginning of cluster list (only for action FS_CLUST_ACT_SEG & FS_CLUST_ACT_ONE)
//!   fs_g_seg.u32_size_or_pos   The memory segment size corresponding at cluster list readed or cleared (unit 512B)
//! @endverbatim
//!
Bool  fat_cluster_list( U8 opt_action )
{
   _MEM_TYPE_FAST_ U32 u32_tmp;
   _MEM_TYPE_FAST_ U8 u8_cluster_status;
   
   fs_g_status = FS_ERR_FS;      // By default system error
   
   if(  Is_fat32
   &&  (FS_CLUST_ACT_CLR == opt_action) )
   {
      // Clear free space information storage in FAT32
      if( !fat_write_fat32_FSInfo( 0xFFFFFFFF ))
         return FALSE;
   }
   
   if ( 0 == fs_g_seg.u32_addr )
   {  
      // Cluster list of root directory
      if( FS_CLUST_ACT_CLR == opt_action )
         return FALSE;           // Impossible to erase ROOT DIR

      if ( Is_fat12 || Is_fat16 )
      {  
         // For a FAT 12 & 16, the root dir isn't a cluster list
         // Check the position
         if ( fs_g_seg.u32_size_or_pos < fs_g_nav.rootdir.seg.u16_size )
         {
            // Compute the start address and the size
            fs_g_seg.u32_addr = fs_g_nav.u32_ptr_fat + fs_g_nav.rootdir.seg.u16_pos + fs_g_seg.u32_size_or_pos;
            fs_g_seg.u32_size_or_pos = fs_g_nav.rootdir.seg.u16_size - fs_g_seg.u32_size_or_pos;
            return TRUE;
         } else {
            fs_g_status = FS_ERR_OUT_LIST;
            return FALSE;        // Position outside the root area
         }
      }
      if ( Is_fat32 )
      {
         // For FAT 32, the root is a cluster list and the first cluster is reading during the mount
         fs_g_cluster.u32_pos = fs_g_nav.rootdir.u32_cluster;
      }
   } else {
      // It is the first cluster of a cluster list
      fs_g_cluster.u32_pos = fs_g_seg.u32_addr;
   }

   // Management of cluster list caches
   if( FS_CLUST_ACT_CLR != opt_action )
   {
      if( fat_cache_clusterlist_update_read() )
         return TRUE;            // Segment found in cache
      // Segment not found & cache ready to update
   }else{
      fat_cache_clusterlist_reset();   // It is a clear action then clear cluster list caches
#if (FS_LEVEL_FEATURES > FSFEATURE_READ)
      fat_clear_info_fat_mod();        // Init cache on fat modification range
#endif  // FS_LEVEL_FEATURES
   }

   // Init loop with a start segment no found
   MSB0( fs_g_seg.u32_addr ) = 0xFF;

   //**** Loop to read the cluster list
   while ( 1 )
   {
      if ( fs_g_seg.u32_size_or_pos < fs_g_nav.u8_BPB_SecPerClus )
      {
         // The segment starts in this cluster
         // Compute the sector address of this cluster
         fs_g_seg.u32_addr = ((fs_g_cluster.u32_pos - 2) * fs_g_nav.u8_BPB_SecPerClus)
                           + fs_g_nav.u32_ptr_fat + fs_g_nav.u32_offset_data + fs_g_seg.u32_size_or_pos;

         if ( FS_CLUST_ACT_ONE == opt_action )
         {  
            // Compute the maximum size
            fs_g_seg.u32_size_or_pos = fs_g_nav.u8_BPB_SecPerClus-fs_g_seg.u32_size_or_pos;
            fat_cache_clusterlist_update_finish();
            // Send a size of one sector
            fs_g_seg.u32_size_or_pos = 1;
            return TRUE;
         }
         // Update the segment size
         fs_g_seg.u32_size_or_pos = fs_g_nav.u8_BPB_SecPerClus - LSB0( fs_g_seg.u32_size_or_pos );

         // Take time, during read cluster list on FAT 16 & 32
         if( (FS_CLUST_ACT_SEG == opt_action)
         &&  (!Is_fat12) )
         {
            // Init loop with the current cluster
            u32_tmp = fs_g_cluster.u32_pos;
            if( !fat_cluster_val( FS_CLUST_VAL_READ ))
               return FALSE;
            // Read cluster list, while this one is continue
            while(1)
            {
               if ( (++fs_g_cluster.u32_pos) != fs_g_cluster.u32_val )
               {
                  fs_g_cluster.u32_pos--;                   // Recompute previous value
                  u32_tmp = fs_g_cluster.u32_pos - u32_tmp; // Compute the size of cluster list
                  fs_g_seg.u32_size_or_pos += u32_tmp * fs_g_nav.u8_BPB_SecPerClus;
                  break;
               }
               if( !fat_cluster_readnext() )
                  return FALSE;
            }
         }
      }
      // Get the cluster value
      if( !fat_cluster_val( FS_CLUST_VAL_READ ))
         return FALSE;

      // Read and check the status of the new cluster
      u8_cluster_status = fat_checkcluster();
      if (FS_CLUS_BAD == u8_cluster_status)
         return FALSE; // error, end of cluster list
   
      if (0xFF == MSB0(fs_g_seg.u32_addr))
      {
         // The beginning of the segment isn't found
         if (FS_CLUS_END == u8_cluster_status)
         {
            u32_tmp = fs_g_seg.u32_size_or_pos;       // Save number of sector remaining
            
            // Compute the sector address of this last cluster to take time during a futur request with the same cluster list
            fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_start -= fs_g_seg.u32_size_or_pos;
            fs_g_seg.u32_addr = ((fs_g_cluster.u32_pos - 2) * fs_g_nav.u8_BPB_SecPerClus)
                              + fs_g_nav.u32_ptr_fat + fs_g_nav.u32_offset_data;
            fs_g_seg.u32_size_or_pos = fs_g_nav.u8_BPB_SecPerClus;
            if (FS_CLUST_ACT_CLR != opt_action)
               fat_cache_clusterlist_update_finish();
            
            // The position is outside the cluster list
            fs_g_seg.u32_addr = fs_g_cluster.u32_pos; // Send the last cluster value
            fs_g_seg.u32_size_or_pos = u32_tmp;       // Restore number of sector remaining
            fs_g_status = FS_ERR_OUT_LIST;
            return FALSE; 
         }
         // Good cluster then continue
         fs_g_seg.u32_size_or_pos -= fs_g_nav.u8_BPB_SecPerClus;
#if (FS_LEVEL_FEATURES > FSFEATURE_READ)
         if (FS_CLUST_ACT_CLR == opt_action)
         {
            if( fs_g_seg.u32_size_or_pos == 0)
            {
               // At cluster position, set the flag end of cluster list
               fs_g_seg.u32_addr = fs_g_cluster.u32_val; // Save the next cluster
               fs_g_cluster.u32_val = FS_CLUST_VAL_EOL;
               if( !fat_cluster_val( FS_CLUST_VAL_WRITE ))
                  return FALSE;
               fs_g_cluster.u32_val = fs_g_seg.u32_addr; // Resotre the next cluster
               // !!!! It isn't necessary to reinit MSB0( fs_g_seg.u32_addr ) to 0xFF,
               // !!!! fs_g_seg.u32_addr will be modified at the beginning of main loop
            }
         }
#endif  // FS_LEVEL_FEATURES
      }
      else
      {
         // The beginning of segment is found
         if (FS_CLUST_ACT_SEG == opt_action)
         {
            if ( (fs_g_cluster.u32_pos+1) != fs_g_cluster.u32_val )
            {
               // The cluster is not a continue cluster or a invalid cluster
               fat_cache_clusterlist_update_finish();
               return TRUE;                              // End of segment
            }
         }
#if (FS_LEVEL_FEATURES > FSFEATURE_READ)
         if (FS_CLUST_ACT_CLR == opt_action)
         {
            //** Clear cluster position
            fs_g_seg.u32_addr = fs_g_cluster.u32_val;    // Save the next cluster
            fs_g_cluster.u32_val = 0;                    // by default free cluster
            // If it is the first cluster (fs_g_seg.u32_size_or_pos <= fs_g_nav.u8_BPB_SecPerClus)
            // and doesn't start at the beginning of cluster (fs_g_seg.u32_size_or_pos != fs_g_nav.u8_BPB_SecPerClus)
            if (fs_g_seg.u32_size_or_pos < fs_g_nav.u8_BPB_SecPerClus)
            {
               fs_g_cluster.u32_val = FS_CLUST_VAL_EOL;  // End of cluster list allocated
            }
            if( !fat_cluster_val( FS_CLUST_VAL_WRITE ))
               return FALSE;
            fs_g_cluster.u32_val = fs_g_seg.u32_addr;    // Resotre the next cluster
            // !!!! It isn't necessary to reinit MSB0( fs_g_seg.u32_addr ) at 0xFF,
            // !!!! because it isn't possible that MSB0( fs_g_cluster.val ) = 0xFF.
         }
#endif  // FS_LEVEL_FEATURES

         // Check the end of cluster list
         if (FS_CLUS_END == u8_cluster_status)
         {
#if (FS_LEVEL_FEATURES > FSFEATURE_READ)
            if (FS_CLUST_ACT_CLR == opt_action)
            {
               return fat_update_fat2();
            }
#endif  // FS_LEVEL_FEATURES
            fat_cache_clusterlist_update_finish();
            return TRUE; // End of segment
         }

         // Update the segment size
         fs_g_seg.u32_size_or_pos += fs_g_nav.u8_BPB_SecPerClus;
      }
      // HERE, Continue to read the cluster list
      // The next cluster is the value of previous cluster
      fs_g_cluster.u32_pos = fs_g_cluster.u32_val;
   }  // End of main loop
}


//! \name Position of the current cluster in the FAT <br>
//! Global variable used to take time with routines fat_cluster_readnext() and fat_cluster_val()
_MEM_TYPE_FAST_ U16   fs_g_u16_pos_fat;


//! This function returns or modifys a cluster value in FAT
//!
//! @param     b_mode   FALSE, to read a cluster value <br>
//!                     TRUE,  to write a cluster value
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! Global variables used
//! IN : 
//!   fs_g_cluster.u32_pos    cluster number to read or write
//!   fs_g_cluster.u32_val    value to write
//! OUT:
//!   fs_g_cluster.u32_val    value readed
//!   fs_g_u16_pos_fat        position in FAT of the cluster to read or write
//!                           value init in case of the fat_cluster_readnext() routine is used after
//! @endverbatim
//!
Bool  fat_cluster_val( Bool b_mode )
{
   _MEM_TYPE_FAST_ U16   u16_offset_fat;
   _MEM_TYPE_FAST_ U8    u8_data1, u8_data2;
#define  u8_data3    (LSB(u16_offset_fat)) // Manual overlay
#define  u8_data4    (MSB(u16_offset_fat)) // Manual overlay
   _MEM_TYPE_FAST_ PTR_CACHE u8_ptr_cluster;

   //**** Compute the cluster position in FAT (sector address & position in sector)
   if ( Is_fat32 )
   {  
      // FAT 32
      // Optimization of -> u16_offset_fat = fs_g_cluster.pos * 4 / FS_CACHE_SIZE;
      // Optimization of -> u16_offset_fat = fs_g_cluster.pos / 128
      u16_offset_fat = fs_g_cluster.u32_pos >> (8-1);            
      
      // Optimization of -> fs_g_u16_pos_fat = (fs_g_cluster.u32_pos * 4) % FS_CACHE_SIZE;
      // Optimization of -> fs_g_u16_pos_fat = (fs_g_cluster.u32_pos % 128) * 4
      fs_g_u16_pos_fat = ((U16)(LSB0(fs_g_cluster.u32_pos) & 0x7F))<< 2;
   }
   else if ( Is_fat16 )
   {  
      // FAT 16
      // Optimization of -> u16_offset_fat = fs_g_cluster.u32_pos * 2 / FS_CACHE_SIZE = fs_g_cluster.u32_pos / 256;
      u16_offset_fat = LSB1(fs_g_cluster.u32_pos);
      // Optimization of -> fs_g_u16_pos_fat = (fs_g_cluster.u32_pos * 2) % FS_CACHE_SIZE;
      // Optimization of -> fs_g_u16_pos_fat = (fs_g_cluster.u32_pos % 256) * 2
      fs_g_u16_pos_fat = ((U16)LSB0(fs_g_cluster.u32_pos)) <<1;   
   }
   else if ( Is_fat12 )
   {
      // FAT 12
      // Optimization of -> fs_g_u16_pos_fat = fs_g_cluster.u32_pos + (fs_g_cluster.u32_pos/ 2)
      fs_g_u16_pos_fat = (U16)fs_g_cluster.u32_pos + ((U16)fs_g_cluster.u32_pos >>1);
      // Optimization of -> u16_offset_fat = fs_g_cluster.u32_pos / FS_CACHE_SIZE
      u16_offset_fat = MSB(fs_g_u16_pos_fat) >> 1;
      // Optimization of -> fs_g_u16_pos_fat = fs_g_u16_pos_fat % FS_CACHE_SIZE
      MSB( fs_g_u16_pos_fat ) &= 0x01;
   }

#if (FS_LEVEL_FEATURES > FSFEATURE_READ)
   if (b_mode)
   {
      // Update information about FAT modification
      if( fs_g_u16_first_mod_fat > u16_offset_fat )
      {
         fs_g_u16_first_mod_fat = u16_offset_fat;
      }
      if( fs_g_u16_last_mod_fat < u16_offset_fat )
      {
         fs_g_u16_last_mod_fat = u16_offset_fat;
      }
      if ( Is_fat12 )
      {  // A cluster may be stored on two sectors
         if( fs_g_u16_pos_fat == (FS_CACHE_SIZE-1) )
         {  // Count the next FAT sector
            if( fs_g_u16_last_mod_fat < (u16_offset_fat+1) )
            {
               fs_g_u16_last_mod_fat = (u16_offset_fat+1);
            }
         }
      }
   }
#endif  // FS_LEVEL_FEATURES

   //**** Read cluster sector in FAT
   fs_gu32_addrsector = fs_g_nav.u32_ptr_fat + u16_offset_fat;   // Computed logical sector address
   if( !fat_cache_read_sector( TRUE ))
      return FALSE;
      
   // Read cluster information
   u8_ptr_cluster = &fs_g_sector[fs_g_u16_pos_fat];
   u8_data1 = u8_ptr_cluster[0];
   // Remark: if (fs_g_u16_pos_fat+1)=512 then it isn't a mistake, because this value will be erase in next lines
   u8_data2 = u8_ptr_cluster[1];
   u8_data3 = u8_ptr_cluster[2];
   u8_data4 = u8_ptr_cluster[3];
   
   if ( Is_fat12 )
   {   // A cluster may be stored on two sectors
      if(  fs_g_u16_pos_fat == (FS_CACHE_SIZE-1) )
      {  // Go to next sector
         fs_gu32_addrsector++;
         if( !fat_cache_read_sector( TRUE ))
           return FALSE;
         u8_data2 = fs_g_sector[0];
      }
   }

   if (FALSE == b_mode)
   {
      //**** Read the cluster value
      LSB0( fs_g_cluster.u32_val ) = u8_data1;  // FAT 12,16,32
      LSB1( fs_g_cluster.u32_val ) = u8_data2;  // FAT 12,16,32

      if ( Is_fat32 )
      {  // FAT 32
         LSB2( fs_g_cluster.u32_val ) = u8_data3;
         LSB3( fs_g_cluster.u32_val ) = u8_data4 & 0x0F; // The high 4 bits are reserved
      }
      else
      {  // FAT 12 & 16 don't use the high bytes
         LSB2( fs_g_cluster.u32_val ) = 0;
         LSB3( fs_g_cluster.u32_val ) = 0;

         // FAT 12 translate 16bits value to 12bits
         if ( Is_fat12 )
         {
            if ( 0x01 & LSB0(fs_g_cluster.u32_pos) )
            {  // Readed cluster is ODD
               LSB0( fs_g_cluster.u32_val ) = (LSB1( fs_g_cluster.u32_val ) <<4 ) + (LSB0( fs_g_cluster.u32_val ) >>4 );
               LSB1( fs_g_cluster.u32_val ) =  LSB1( fs_g_cluster.u32_val ) >>4 ;
            }
            else
            {  // Readed cluster is EVEN
               LSB1( fs_g_cluster.u32_val ) &= 0x0F;
            }
         }
      }
   } else {
#if (FS_LEVEL_FEATURES > FSFEATURE_READ)
      //**** Write the cluster value
      if ( Is_fat12 )
      {
         // FAT 12, translate cluster value
         if ( 0x01 & LSB0(fs_g_cluster.u32_pos) )
         {  // Cluster writing is ODD
            u8_data1 = (u8_data1 & 0x0F) + (LSB0( fs_g_cluster.u32_val )<<4);
            u8_data2 = (LSB1( fs_g_cluster.u32_val )<<4) + (LSB0( fs_g_cluster.u32_val )>>4) ;
         } else {
            // Cluster writing is EVEN
            u8_data1 = LSB0( fs_g_cluster.u32_val );
            u8_data2 = (u8_data2 & 0xF0) + (LSB1( fs_g_cluster.u32_val ) & 0x0F) ;
         }

         // A cluster may be stored on two sectors
         if( fs_g_u16_pos_fat == (FS_CACHE_SIZE-1) )
         {  
            fs_g_sector[0] = u8_data2;
            fat_cache_sector_is_modify();
            // Go to previous sector
            fs_gu32_addrsector--;
            if( !fat_cache_read_sector( TRUE ))
              return FALSE;
            // Modify the previous sector
            fs_g_sector[ FS_CACHE_SIZE-1 ] = u8_data1;
            fat_cache_sector_is_modify();
            return TRUE;
         }
      }
      else
      {
         // FAT 16 & 32
         u8_data1 = LSB0( fs_g_cluster.u32_val );
         u8_data2 = LSB1( fs_g_cluster.u32_val );
         if ( Is_fat32 )
         {  // FAT 32
            u8_ptr_cluster[2] = LSB2( fs_g_cluster.u32_val );
            u8_ptr_cluster[3] = LSB3( fs_g_cluster.u32_val ) + (u8_data4 & 0xF0); // The high 4 bits are reserved
         }
      }
      // Here for FAT 32, 16 & 12 (only if the cluster values are in the same sector)
      u8_ptr_cluster[0] = u8_data1;
      u8_ptr_cluster[1] = u8_data2;
      fat_cache_sector_is_modify();
#else
      fs_g_status = FS_ERR_COMMAND;
      return FALSE;
#endif  // FS_LEVEL_FEATURES
   }

   return TRUE;
#undef  u8_data3    // end of Manual overlay
#undef  u8_data4    // end of Manual overlay
}


//! This function is optimized to read a continue cluster list on FAT16 and FAT32
//!
//! Read global value "fs_g_status" in case of error :
//!          FS_ERR_HW            Hardware driver error
//!          FS_LUN_WP            Drive is read only
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! Global variables used
//! IN : 
//!   fs_g_u16_pos_fat        previous cluster position in FAT
//! OUT:
//!   fs_g_u16_pos_fat        readed cluster position in FAT
//!   fs_g_cluster.u32_val    value of cluster readed
//! @endverbatim
//!
Bool  fat_cluster_readnext( void )
{
   // Compute the next cluster position in FAT
   if ( Is_fat32 )
   {  
      fs_g_u16_pos_fat += 4;
   }else{
      // Is_fat16
      fs_g_u16_pos_fat += 2;
   }
   
   // Check if next cluster is in internal cache
   if( FS_CACHE_SIZE == fs_g_u16_pos_fat )
   {
      // Update cache
      fs_g_u16_pos_fat = 0;
      fs_gu32_addrsector++;
      if( !fat_cache_read_sector( TRUE ))
         return FALSE;
   }

   //**** Read the cluster value
   LSB0( fs_g_cluster.u32_val ) = fs_g_sector[fs_g_u16_pos_fat+0];  // FAT 16,32
   LSB1( fs_g_cluster.u32_val ) = fs_g_sector[fs_g_u16_pos_fat+1];  // FAT 16,32

   if ( Is_fat32 )
   {  // FAT 32
      LSB2( fs_g_cluster.u32_val ) = fs_g_sector[fs_g_u16_pos_fat+2];
      LSB3( fs_g_cluster.u32_val ) = fs_g_sector[fs_g_u16_pos_fat+3];
   }
   return TRUE;
}


//! This function checks the cluster value
//!
//! @return    value status <br>
//!            FS_CLUS_OK        Value correct <br>
//!            FS_CLUS_BAD       Value bad <br>
//!            FS_CLUS_END       It is a end of list <br>
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_cluster.u32_val       value to check
//! @endverbatim
//!
U8    fat_checkcluster( void )
{
   // Cluster bad if (FAT12 > 0x0FF6) (FAT16 > 0xFFF6) (FAT32 > 0x0FFFFFF6) or cluster null
   if ( Is_fat32 )
   {
      if (  (0x0F != LSB3(fs_g_cluster.u32_val))
      ||    (0xFF != LSB2(fs_g_cluster.u32_val))
      ||    (0xFF != LSB1(fs_g_cluster.u32_val)) )
      {
         return FS_CLUS_OK;
      }
   }
   else if ( Is_fat16 )
   {
      if (0xFF != LSB1(fs_g_cluster.u32_val))
         return FS_CLUS_OK;
   }
   else if ( Is_fat12 )
   {
      if (0x0F != LSB1(fs_g_cluster.u32_val))
         return FS_CLUS_OK;
   }

   if (LSB0(fs_g_cluster.u32_val) == 0xFF)
      return FS_CLUS_END;
   if (LSB0(fs_g_cluster.u32_val) > 0xF6 )
      return FS_CLUS_BAD;

   return FS_CLUS_OK;
}


//! \name Internal functions to manage cluster list caches
//! @{

//! This function resets the cluster list caches
//!
void  fat_cache_clusterlist_reset( void )
{
#if (FS_NB_CACHE_CLUSLIST>1)
   U8 u8_i;
   fs_g_u8_current_cache=0;
   for( u8_i=0; u8_i<FS_NB_CACHE_CLUSLIST; u8_i++ )
#endif
   {
      fs_g_cache_clusterlist[u8_i].u8_lun = 0xFF;
      fs_g_cache_clusterlist[u8_i].u8_level_use = 0xFF;
   }
}

//! This function initializes a cache in cluster list caches
//!
void  fat_cache_clusterlist_update_start( void )
{
#if (FS_NB_CACHE_CLUSLIST>1)
   // Get the OLD cache (=max level used)
   U8 u8_i;
   for( u8_i=0; u8_i<(FS_NB_CACHE_CLUSLIST-1); u8_i++ ) // FS_NB_CACHE_CLUSLIST-1, in case of error
   {
      if( (FS_NB_CACHE_CLUSLIST-2) < fs_g_cache_clusterlist[u8_i].u8_level_use )
        break;
   }
   fs_g_u8_current_cache = u8_i;
#endif
   fs_g_cache_clusterlist[fs_g_u8_current_cache].u8_lun       = 0xFF;                     // unvalid cache
   fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_cluster  = fs_g_cluster.u32_pos;
   fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_start    = fs_g_seg.u32_size_or_pos;
}


//! This function updates a cache of cluster list caches
//!
void  fat_cache_clusterlist_update_finish( void )
{
   U8 u8_cluster_offset = fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_start % fs_g_nav.u8_BPB_SecPerClus;
   fs_g_cache_clusterlist[fs_g_u8_current_cache].u8_lun       = fs_g_nav.u8_lun;          // valid cache
   fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_start   -= u8_cluster_offset;
   fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_addr     = fs_g_seg.u32_addr - u8_cluster_offset;
   fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_size     = fs_g_seg.u32_size_or_pos + u8_cluster_offset;

#if (FS_NB_CACHE_CLUSLIST>1)  
   // Update the "level used" of cache
   fat_cache_clusterlist_update_select();
#endif
}

  
#if (FS_NB_CACHE_CLUSLIST>1)  
//! This function signals that a cache is used
//!
void  fat_cache_clusterlist_update_select( void )
{
   U8 u8_i;
   U8 u8_level_to_update;
  
   u8_level_to_update = fs_g_cache_clusterlist[  fs_g_u8_current_cache  ].u8_level_use;
   for( u8_i=0; u8_i<FS_NB_CACHE_CLUSLIST; u8_i++ )
   {
      if( u8_level_to_update > fs_g_cache_clusterlist[u8_i].u8_level_use )
        fs_g_cache_clusterlist[u8_i].u8_level_use++;
   }
   fs_g_cache_clusterlist[  fs_g_u8_current_cache  ].u8_level_use = 0;
}
#endif


//! This function searchs a cluster list in cluster list caches
//!
//! @return    TRUE  cluster list found and global variable fs_g_seg updated
//! @return    FALSE no found in cluster list caches
//!
Bool  fat_cache_clusterlist_update_read( void )
{
   U32 u32_tmp;
#if (FS_NB_CACHE_CLUSLIST>1)
   U8 u8_i;
   for( u8_i=0; u8_i<FS_NB_CACHE_CLUSLIST; u8_i++ )
#endif
   {
      if(fs_g_cache_clusterlist[u8_i].u8_lun == fs_g_nav.u8_lun )
      {
         if( fs_g_cache_clusterlist[u8_i].u32_cluster == fs_g_cluster.u32_pos )
         {
            if( fs_g_cache_clusterlist[u8_i].u32_start <= fs_g_seg.u32_size_or_pos )
            {
               // The segment research is in or after the cache
               if( fs_g_cache_clusterlist[u8_i].u32_size  > (fs_g_seg.u32_size_or_pos-fs_g_cache_clusterlist[u8_i].u32_start) )
               {
                  //** The segment research is in cache, then compute the segment infos
                  fs_g_seg.u32_size_or_pos -= fs_g_cache_clusterlist[u8_i].u32_start;
                  fs_g_seg.u32_addr = fs_g_cache_clusterlist[u8_i].u32_addr + fs_g_seg.u32_size_or_pos;
                  fs_g_seg.u32_size_or_pos = fs_g_cache_clusterlist[u8_i].u32_size - fs_g_seg.u32_size_or_pos;
#if (FS_NB_CACHE_CLUSLIST>1)
                  fs_g_u8_current_cache = u8_i;
                  fat_cache_clusterlist_update_select();
#endif
                  return TRUE;   // the segment is in cluster list cache
               }else{
                  //** It is after the cache then get cache information and continue to read the cluster list in FAT
#if (FS_NB_CACHE_CLUSLIST>1)
                  // Store the resultat in this cache
                  fs_g_u8_current_cache = u8_i;
#endif
                  fs_g_cache_clusterlist[fs_g_u8_current_cache].u8_lun       = 0xFF;   // unvalid cache
                  // fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_cluster  = fs_g_cluster.u32_pos;  // It is the same cluster start
                  
                  // Get cache information to take time during the next FAT access
                  // Compute the cluster number corresponding at the last cluster of the cluster list cache
                  fs_g_cluster.u32_pos     = ((fs_g_cache_clusterlist[u8_i].u32_addr -fs_g_nav.u32_ptr_fat - fs_g_nav.u32_offset_data + fs_g_cache_clusterlist[u8_i].u32_size -1)
                                             / fs_g_nav.u8_BPB_SecPerClus) +2;
                  u32_tmp  = fs_g_seg.u32_size_or_pos;                                 // save position ask
                  // Compute the position of the end of cluster list cache, and decrement the position asked
                  fs_g_seg.u32_size_or_pos-= ((fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_start + fs_g_cache_clusterlist[u8_i].u32_size -1)
                                             / fs_g_nav.u8_BPB_SecPerClus)
                                             * fs_g_nav.u8_BPB_SecPerClus;
                  fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_start = u32_tmp;   // Update cache with the position asked
                  return FALSE;                                                        // The segment isn't in cluster list cache
               }
            }
         }
      }
   }
   // No found in cache then read FAT and store the resultat in cache
   fat_cache_clusterlist_update_start();
   return FALSE;
}

// To clean the code 
#if (FS_NB_CACHE_CLUSLIST==1)
#  undef u8_i
#endif

//! @}

//! This function gets or clears a cluster list at the current position in the selected file
//!
//! @param     mode              Choose action <br>
//!            FS_CLUST_ACT_SEG  Get memory segment corresponding at the position in selected file <br>
//!            FS_CLUST_ACT_ONE  Store in internal cache the sector corresponding at the position in selected file <br>
//!            FS_CLUST_ACT_CLR  Clear the cluster list corresponding at the position in selected file <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_nav_entry.u32_cluster       First cluster of selected file
//!   fs_g_nav_entry.u32_pos_in_file   Position in file (unit byte)
//! @endverbatim
//!
Bool  fat_read_file( U8 mode )
{
   U32   u32_sector_pos;

   // Compute sector position
   u32_sector_pos = fs_g_nav_entry.u32_pos_in_file >> FS_512B_SHIFT_BIT;
   
   if(FS_CLUST_ACT_ONE  == mode)
   {
      if( (fs_g_sectorcache.u8_lun                 == fs_g_nav.u8_lun )
      &&  (fs_g_sectorcache.u32_clusterlist_start  == fs_g_nav_entry.u32_cluster )
      &&  (fs_g_sectorcache.u32_clusterlist_pos    == u32_sector_pos ) )
      {
         return TRUE;      // The internal cache contains the sector ascked
      }
   }
   else
   {
      if( FS_CLUST_ACT_CLR == mode )
      {
         // Clear cluster list
         if( 0 == fs_g_nav_entry.u32_cluster )
            return TRUE;   // No cluster list is linked with the file, then no clear is necessary
            
         if(0 != (fs_g_nav_entry.u32_pos_in_file & FS_512B_MASK) )
         {
            // The actual sector is used, then start clear on the next sector
            u32_sector_pos++;
         }
      }
   }

   // Get the segment which start at the current position
   fs_g_seg.u32_addr = fs_g_nav_entry.u32_cluster;
   fs_g_seg.u32_size_or_pos = u32_sector_pos;
   if( FS_CLUST_ACT_ONE != mode )
   {
      if( fat_cluster_list( mode ) )
         return TRUE;      // Get or clear segment OK
   }
   else
   {
      if( fat_cluster_list( FS_CLUST_ACT_SEG ) )   // Read all segment
      {
         // Read the sector corresponding at the position file (= first sector of segment)
         fs_gu32_addrsector = fs_g_seg.u32_addr ;
         if( fat_cache_read_sector( TRUE ) )
         {
            fs_g_sectorcache.u32_clusterlist_start  = fs_g_nav_entry.u32_cluster;
            fs_g_sectorcache.u32_clusterlist_pos    = u32_sector_pos;
            return TRUE;
         }
      }
   }
   if( (FS_CLUST_ACT_CLR == mode       )
   &&  (FS_ERR_OUT_LIST  == fs_g_status) )
   {
      // It is possible to clear nothing
      return TRUE;
   }
   return FALSE;
}


#if (FSFEATURE_WRITE == (FS_LEVEL_FEATURES & FSFEATURE_WRITE))
//! This function gets and eventually allocs a cluster list at the current position in the selected file 
//!
//! @param     mode                 Choose action <br>
//!            FS_CLUST_ACT_SEG     Get and eventuelly alloc a cluster list <br>
//!            FS_CLUST_ACT_ONE     Get and eventually alloc a cluster list for one sector, and load this sector in internal cache <br>
//! @param     u32_nb_sector_write  maximum number of sector to get and eventually to alloc for the selected file (ignored if mode = FS_CLUST_ACT_ONE)
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_nav_entry.u32_cluster       First cluster of selected file
//!   fs_g_nav_entry.u32_pos_in_file   Position in the file (unit byte)
//! @endverbatim
//!
Bool  fat_write_file( U8 mode , U32 u32_nb_sector_write )
{
   if( 0 == fs_g_nav_entry.u32_cluster )
   {
      // File don't have a cluster list, then alloc the first cluster list of the file
      MSB0(fs_g_seg.u32_addr)    = 0xFF;     // It is a new cluster list
      // Update cluster list caches
      // fs_g_cluster.u32_pos    = ?         // To fill after alloc
      fs_g_seg.u32_size_or_pos   = 0;
      fat_cache_clusterlist_update_start();
   }
   else
   {
      if( fat_read_file( mode ) )
         return TRUE;      // A segment is availabled (no alloc necessary)

      if( FS_ERR_OUT_LIST != fs_g_status )
      {
         return FALSE;     // Error system
      }
      // fat_read_file is outsize the list then the current cluster list cache contains the last cluster

      // Initialize cluster list caches before alloc routine
      fs_g_cache_clusterlist[fs_g_u8_current_cache].u8_lun       = 0xFF;                     // unvalid cache
      // fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_cluster  = fs_g_cluster.u32_pos;  // it is the same
      fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_start += fs_g_nav.u8_BPB_SecPerClus; // Position of next cluster (the first new)
   }
   
   // Alloc a cluster list
   if( FS_CLUST_ACT_SEG == mode )
   {
      fs_g_seg.u32_size_or_pos = u32_nb_sector_write;
   }else{
      fs_g_seg.u32_size_or_pos = 1;                                                          // only one sector
   }

   //note: fs_g_seg.u32_addr is already initialized with the last cluster value (see fat_cluster_list())
   if( !fat_allocfreespace())
      return FALSE;
   //note: fs_g_seg.u32_addr is the first cluster of the cluster list allocated by alloc_free_space()
   //note: fs_g_seg.u32_size_or_pos = number of sectors remaining

   if( 0 == fs_g_nav_entry.u32_cluster )
   {
      // It is the first cluster list of file, then update following values in cluster list cache
      // fs_g_seg.u32_addr = already contzins the first cluster of the file (see alloc_free_space())
      fs_g_cache_clusterlist[fs_g_u8_current_cache].u32_cluster = fs_g_seg.u32_addr;
      // Update file entry
      fs_g_nav_entry.u32_cluster = fs_g_seg.u32_addr;
   }
   
   // Update cluster list cache
   if( FS_CLUST_ACT_SEG == mode )
   {
      fs_g_seg.u32_size_or_pos = u32_nb_sector_write - fs_g_seg.u32_size_or_pos;
   }else{
      fs_g_seg.u32_size_or_pos = 1 - fs_g_seg.u32_size_or_pos;
   }
   fs_g_seg.u32_addr = ((fs_g_seg.u32_addr - 2) * fs_g_nav.u8_BPB_SecPerClus)
                     + fs_g_nav.u32_ptr_fat + fs_g_nav.u32_offset_data;
   fat_cache_clusterlist_update_finish();

   return fat_read_file( mode );    // load the new cluster list
}
#endif  // FS_LEVEL_FEATURES

//! This function fill the internal cache with a sector from current directory
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_nav.u32_cluster_sel_dir           First cluster of current directory
//!   fs_g_nav_fast.u16_entry_pos_sel_file   Position in directory (unit entry)
//! @endverbatim
//!
Bool  fat_read_dir( void )
{
   U32 u32_cluster_pos;

   // Compute the cluster list position corresponding of the current entry
   u32_cluster_pos = fs_g_nav_fast.u16_entry_pos_sel_file >> (FS_512B_SHIFT_BIT - FS_SHIFT_B_TO_FILE_ENTRY);

   if( (fs_g_sectorcache.u8_lun                 == fs_g_nav.u8_lun )
   &&  (fs_g_sectorcache.u32_clusterlist_start  == fs_g_nav.u32_cluster_sel_dir )
   &&  (fs_g_sectorcache.u32_clusterlist_pos    == u32_cluster_pos ) )
   {
         return TRUE;      // The internal cache contains the sector ascked
   }

   // Get sector address corresponding at cluster list position
   fs_g_seg.u32_addr = fs_g_nav.u32_cluster_sel_dir;
   fs_g_seg.u32_size_or_pos = u32_cluster_pos;
   if( fat_cluster_list( FS_CLUST_ACT_ONE ) )
   {
      // Read the sector
      fs_gu32_addrsector = fs_g_seg.u32_addr;
      if( fat_cache_read_sector( TRUE ) )
      {
         // Update information about internal sector cache
         fs_g_sectorcache.u32_clusterlist_start  = fs_g_nav.u32_cluster_sel_dir;
         fs_g_sectorcache.u32_clusterlist_pos    = u32_cluster_pos;
         return TRUE;
      }
   }
   return FALSE;
}



//! This function checks the entry
//!
//! @param     b_type   entry type to compare (FS_FILE or FS_DIR)
//!
//! @return    TRUE,    the entry is a short entry and correspond to b_type
//! @return    FALSE,   otherwise
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_sector       The directory sector corresponding at the current position
//!   fs_g_nav_fast.u16_entry_pos_sel_file    Position in directory of the entry file (unit entry)
//! @endverbatim
//!
Bool  fat_entry_check( Bool b_type ) 
{
   PTR_CACHE u8_ptr_entry;
   U8 u8_first_byte, u8_seconde_byte;
   U8 u8_attribut;

   u8_ptr_entry = fat_get_ptr_entry();

   u8_first_byte = u8_ptr_entry[0];
   if ( FS_ENTRY_END == u8_first_byte )
   {
      fs_g_status = FS_ERR_ENTRY_EMPTY;   // end of directory
      return FALSE;
   }
   fs_g_status = FS_ERR_ENTRY_BAD;        // by default BAD ENTRY
   if ( FS_ENTRY_DEL == u8_first_byte )      { return FALSE;   } // entry deleted
   if (   '.'  == u8_first_byte )            { return FALSE;   } // current dir "."
   u8_seconde_byte = u8_ptr_entry[1];
   if ( ('.'  == u8_first_byte)
   &&   ('.'  == u8_seconde_byte) )          { return FALSE;   } // current dir ".."

   // Check attribut
   u8_attribut = u8_ptr_entry[11];
   if ( FS_ATTR_VOLUME_ID & u8_attribut )    { return FALSE;   } // volume id
   // Optimization, this line isn't necessary because the next test control this case
   // if ( FS_ATTR_LFN_ENTRY == *u8_ptr_entry) { return FALSE;   } // long file name

   // Check entry type
   if( FS_ATTR_DIRECTORY & u8_attribut )
   {
      return (FS_DIR == b_type);
   }else{
      return (FS_FILE == b_type);
   }
}


//! This function checks the file extension
//!
//! @param     sz_filter      extension filter is a ASCII string (ex: "mp3,w*" )
//!
//! @return    TRUE, the file name have a good extension
//! @return    FALSE, otherwise
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_sector       The directory sector corresponding at the current position
//!   fs_g_nav_fast.u16_entry_pos_sel_file    Position in directory of the entry file (unit entry)
//! @endverbatim
//!
Bool  fat_entry_checkext( FS_STRING sz_filter )
{
   PTR_CACHE u8_ptr_entry;
   U8 u8_i, u8_filter_char, u8_entry_char;
   
   u8_ptr_entry = fat_get_ptr_entry();
   
   // Compare the extension with filter
   for( u8_i=0 ; u8_i<3 ; u8_i++)
   {
      u8_filter_char = *sz_filter;
      if ('*' == u8_filter_char)
         break; // All extension is good
         
      u8_entry_char = u8_ptr_entry[8+u8_i]; 
      
      // Compare the extension filter to extension file (this one ignore the case)
      if( (u8_filter_char!=  u8_entry_char     )
      &&  (u8_filter_char!= (u8_entry_char+('a'-'A'))) )
      {
         if ( (',' == u8_filter_char)
         ||   ( 0  == u8_filter_char) )
         {  
           // It is the end of filter
           if (' ' == u8_entry_char)
              break; // it is the end of extension file -> extension good
         }
         // here, bad extension

         // Search the next filter
         while( ',' != u8_filter_char )
         {
            if (0  == u8_filter_char)
            {
               return FALSE;   // it is the last filter
            }
            sz_filter++;
            u8_filter_char = *sz_filter;
         }
         u8_i = 0xFF;          // restart loop compare
      }
      sz_filter++; // go to next char of filter
   }

   return TRUE; // It is a good extension
}


//! This function reads information about selected file
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_sector       The directory sector corresponding at the current position
//!   fs_g_nav_fast.u16_entry_pos_sel_file    Position in directory of the entry file (unit entry)
//! OUT:
//!   fs_g_nav_entry. u32_cluster, u8_attr, u32_size
//! @endverbatim
//!
void  fat_get_entry_info( void )
{
   PTR_CACHE ptr_entry;

   ptr_entry = fat_get_ptr_entry();

   // Get attribut
   ptr_entry+= 11;
   fs_g_nav_entry.u8_attr = ptr_entry[0];

   // Get the first cluster of the file cluster list
   ptr_entry += (20-11);
   LSB2(fs_g_nav_entry.u32_cluster) = ptr_entry[0];
   LSB3(fs_g_nav_entry.u32_cluster) = ptr_entry[1];
   ptr_entry += (26-20);
   LSB0(fs_g_nav_entry.u32_cluster) = ptr_entry[0];
   LSB1(fs_g_nav_entry.u32_cluster) = ptr_entry[1];

   // Get the size of file
   ptr_entry += (28-26);
   LSB0(fs_g_nav_entry.u32_size) = ptr_entry[0];
   LSB1(fs_g_nav_entry.u32_size) = ptr_entry[1];
   LSB2(fs_g_nav_entry.u32_size) = ptr_entry[2];
   LSB3(fs_g_nav_entry.u32_size) = ptr_entry[3];
}


//! This function checks if the entry file is a directory
//!
//! @return    TRUE,    this entry is a directory
//! @return    FALSE,   otherwise
//!
Bool  fat_entry_is_dir(void)
{
   fs_g_status = FS_ERR_NO_DIR;
   return (FS_ATTR_DIRECTORY & fs_g_nav_entry.u8_attr);
}


//! This function resets the selection pointers
//!
void  fat_clear_entry_info_and_ptr( void )
{
   fs_g_nav_fast.u16_entry_pos_sel_file= FS_NO_SEL;
   fs_g_nav.u16_pos_sel_file           = FS_NO_SEL;
   fs_g_nav.b_mode_nav                 = FS_DIR;
   fs_g_nav_entry.u8_attr     = 0;
   fs_g_nav_entry.u32_cluster = 0;
   fs_g_nav_entry.u32_size    = 0;
   Fat_file_close;
}


#if (FSFEATURE_WRITE == (FS_LEVEL_FEATURES & FSFEATURE_WRITE))
//! This function writes the information about selected file
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_sector       The directory sector corresponding at the current position
//!   fs_g_nav_fast.u16_entry_pos_sel_file    Position in directory of the entry file (unit entry)
//! OUT:
//!   fs_g_sector    Updated
//! @endverbatim
//!
void  fat_write_entry_file( void )
{
   PTR_CACHE ptr_entry;

   fat_cache_sector_is_modify();
   ptr_entry = fat_get_ptr_entry();

   if( !(FS_ATTR_DIRECTORY | fs_g_nav_entry.u8_attr))
   {
      if( 0 == fs_g_nav_entry.u32_size )
         fs_g_nav_entry.u32_cluster = 0;
   }

   //! Write the attribut
   ptr_entry+= 11;
   ptr_entry[0] = fs_g_nav_entry.u8_attr;

   // Write the first cluster of file cluster list
   ptr_entry += (20-11);
   ptr_entry[0] = LSB2(fs_g_nav_entry.u32_cluster);
   ptr_entry[1] = LSB3(fs_g_nav_entry.u32_cluster);
   ptr_entry += (26-20);
   ptr_entry[0] = LSB0(fs_g_nav_entry.u32_cluster);
   ptr_entry[1] = LSB1(fs_g_nav_entry.u32_cluster);

   //! Write the size of file
   ptr_entry += (28-26);
   ptr_entry[0] = LSB0(fs_g_nav_entry.u32_size);
   ptr_entry[1] = LSB1(fs_g_nav_entry.u32_size);
   ptr_entry[2] = LSB2(fs_g_nav_entry.u32_size);
   ptr_entry[3] = LSB3(fs_g_nav_entry.u32_size);
}
#endif  // FS_LEVEL_FEATURES


//! This function returns or compares the short name entry
//!
//! @param     b_mode         action mode: <br>
//!                           FS_NAME_GET       to get the short name of selected file <br>
//!                           FS_NAME_CHECK     to compare the short name of selected file <br>
//! @param     sz_name        if FS_NAME_GET    then buffer to store the short name file (ASCII or UNICODE ) <br>
//!                           if FS_NAME_CHECK  then name to compare with short name (ASCII or UNICODE),
//!                                             it must be terminate by NULL or '*' value <br>
//! @param     u8_size_max    buffer size (unit ASCII or UNICODE ) (ignored in "FS_NAME_CHECK" mode)
//!
//! @return    FALSE, in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE, the name is correct or read OK
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_sector       The directory sector corresponding at the current position
//!   fs_g_nav_fast.u16_entry_pos_sel_file    Position in directory of the entry file (unit entry)
//! @endverbatim
//!
Bool  fat_entry_shortname( FS_STRING sz_name , U8 u8_size_max , Bool b_mode )
{
   Bool b_extension_nostart = TRUE;
   U8 u8_pos_name;
   U8 u8_entry_char, u8_szname_char;
   PTR_CACHE ptr_entry;
   U8 u8_pos_entry;

   fs_g_status = FS_ERR_NAME_INCORRECT;  // by default the name don't corresponding at filter name

   u8_pos_name = 0;
   u8_pos_entry = 0;
   ptr_entry = fat_get_ptr_entry();

   // for each characters of short name
   while( 1 )
   {  
      if( FS_SIZE_SFNAME == u8_pos_entry )
      {
         u8_entry_char = 0;   // end of name
      }
      else
      {
         u8_entry_char = ptr_entry[ u8_pos_entry ];
         if( ((FS_SIZE_SFNAME_WITHOUT_EXT == u8_pos_entry) && b_extension_nostart)  // end of name and '.' character no writed
         ||  ( ' ' == u8_entry_char) )
         {  
            // end of name or extension
            if( (FS_SIZE_SFNAME_WITHOUT_EXT >= u8_pos_entry)         // End of name without extension
            &&  (' ' != ptr_entry[ FS_SIZE_SFNAME_WITHOUT_EXT ]) )   // extension exists
            {
               // go to extension position
               b_extension_nostart = FALSE;
               u8_pos_entry = FS_SIZE_SFNAME_WITHOUT_EXT-1;
               u8_entry_char = '.';
            }
            else
            {
               u8_entry_char = 0;                                    // end of name
            }
         }
      }

      if( FS_NAME_GET == b_mode )
      {
         if( !g_b_string_length )
         {
            if(u8_pos_name >= (u8_size_max-1))
               u8_entry_char = 0;                                    // buffer full then force end of string
         
            if( ('A'<=u8_entry_char) && (u8_entry_char<='Z'))
               u8_entry_char += ('a'-'A');                           // display short name in down case
               
            if( Is_unicode )
            {
               ((FS_STR_UNICODE)sz_name)[0] = u8_entry_char;
            }else{
               sz_name[0] = u8_entry_char;
            }
         }
      }
      else
      {
         // Compare the name
         if( Is_unicode 
         && (0 != MSB(((FS_STR_UNICODE)sz_name)[0])) )
         {
            // The UNICODE is not possibled in short name
            return FALSE;
         }
        
         if( Is_unicode )
         {
            u8_szname_char = ((FS_STR_UNICODE)sz_name)[0];
         }else{
            u8_szname_char = sz_name[0];
         }
         if ('*' == u8_szname_char)
         {  // end of filter name which autorise all next character
            return TRUE;   //*** The name is correct ***
         }
        
         if( (0 != u8_entry_char) || (('\\' != u8_szname_char) && ('/' != u8_szname_char)) )
         {
            if((u8_szname_char != u8_entry_char)
            && (u8_szname_char != (u8_entry_char+('a'-'A'))) )  // no case sensitive
               return FALSE;  // short name not equal
         }
      }

      // For each characters
      if (0 == u8_entry_char)
      {
         if( g_b_string_length )
         {
            ((FS_STR_UNICODE)sz_name)[0] = u8_pos_name+1;      // Get length name
         }
         return TRUE;   // End of test correct or end of get name
      }
      if( !g_b_string_length )
      {
         sz_name += (Is_unicode? 2 : 1 );
      }
      u8_pos_name++;
      u8_pos_entry++;
   }   
}


//! This function returns or compares the long name entry
//!
//! @param     b_mode         action mode: <br>
//!                           FS_NAME_GET       to get the long name of selected file <br>
//!                           FS_NAME_CHECK     to compare the long name of selected file <br>
//! @param     sz_name        if FS_NAME_GET    then buffer to store the long name file (ASCII or UNICODE ) <br>
//!                           if FS_NAME_CHECK  then name to compare with long name (ASCII or UNICODE),
//!                                             it must be terminate by NULL or '*' value <br>
//!
//! @param     b_match_case   FALSE, ignore the case (only used in "FS_NAME_CHECK" action mode)
//! @param     u8_size_max    buffer size (unit ASCII or UNICODE ) (ignored in "FS_NAME_CHECK" mode)
//!
//! @return    FALSE is not the end of long name, or in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE, the name is correct or read is finish
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_sector       The directory sector corresponding at the current position
//!   fs_g_nav_fast.u16_entry_pos_sel_file    Position in directory of the entry file (unit entry)
//! @endverbatim
//!
Bool  fat_entry_longname( FS_STRING sz_name , U8 u8_size_max , Bool b_mode , Bool b_match_case )
{
   U8 u8_pos_name;
   PTR_CACHE ptr_entry;
   U16 u16_unicode_entry;
   U16 u16_unicode_szname;
   
   ptr_entry = fat_get_ptr_entry();

   if( (FS_ENTRY_END == *ptr_entry )            // end of directory
   ||  (FS_ENTRY_DEL == *ptr_entry )            // entry deleted
   ||  (FS_ATTR_LFN_ENTRY != ptr_entry[11]) )   // no long name
   {
      fs_g_status = FS_ERR_ENTRY_BAD;
      return FALSE;
   }

   if( g_b_string_length )
   {
      if ( 0 == (FS_ENTRY_LFN_LAST & *ptr_entry))
      {
         // no necessary -> ((FS_STR_UNICODE)sz_name)[0] = FS_SIZE_LFN_ENTRY;
         fs_g_status = FS_NO_LAST_LFN_ENTRY;
         return FALSE;                          // Other entry long name
      }
   }

   ptr_entry++;                                 // The long name start at offset 1 of the entry file

   u8_pos_name=0;
   while( 1 )
   {
      LSB(u16_unicode_entry) = ptr_entry[0];
      MSB(u16_unicode_entry) = ptr_entry[1];
      if( FS_NAME_GET == b_mode )
      {
         if( !g_b_string_length )
         {
            // Check the end of buffer
            if( u8_pos_name>=(u8_size_max-1) )
            {
               // Write end of string
               if( Is_unicode )
               {
                  ((FS_STR_UNICODE)sz_name)[0] = 0;
               }else{
                  sz_name[0] = 0;
               }
               return TRUE;                     // the buffer is full
            }
            // Read and store the long name
            if( Is_unicode )
            {
               ((FS_STR_UNICODE)sz_name)[0] = u16_unicode_entry;
            }else{
               sz_name[0] = (U8)u16_unicode_entry;
            }
         }
      }
      else
      {
         if( Is_unicode )
         {
            u16_unicode_szname = ((FS_STR_UNICODE)sz_name)[0];
         }else{
            u16_unicode_szname = sz_name[0];
         }
         // Check the name
         if( '*' == u16_unicode_szname )
         {  // end of filter name which autorise all next character
            return TRUE;   //*** The name is correct ***
         }
         
         if( ((0 != u16_unicode_entry ) || (( '\\' != u16_unicode_szname) && ( '/' != u16_unicode_szname)) )
         &&  ((u16_unicode_szname != (u16_unicode_entry+('a'-'A'))) || b_match_case)
         &&  ((u16_unicode_szname != (u16_unicode_entry-('a'-'A'))) || b_match_case)
         &&  (u16_unicode_szname != u16_unicode_entry) )
         {
           fs_g_status = FS_ERR_NAME_INCORRECT; //  The name don't corresponding at filter name
           return FALSE;
         }
      }

      if( 0 == u16_unicode_entry)
      {
         if( g_b_string_length )
         {
            ((FS_STR_UNICODE)sz_name)[0] = u8_pos_name+1;
         }
         return TRUE;                           // Last long name entry
      }
      if( 4 == u8_pos_name )
         ptr_entry += 3;                        // Go to second character

      if( 10 == u8_pos_name )
         ptr_entry += 2;                        // Go to third character

      if( 12 == u8_pos_name )
      {  // End of entry long name
         ptr_entry -= (FS_SIZE_FILE_ENTRY-2);   // Go to the first byte of the file entry
         if ( 0 == (FS_ENTRY_LFN_LAST & ptr_entry[0]))
         {
            fs_g_status = FS_NO_LAST_LFN_ENTRY;
            return FALSE;                       // Other long name entry is present
         }
         else
         {  // It is the last long name entry
            // then it is the end of name
            if( (FS_NAME_GET == b_mode) && g_b_string_length )
            {
               ((FS_STR_UNICODE)sz_name)[0] = 14;
               return TRUE;  
            }
            sz_name += (Is_unicode? 2 : 1 );
            if( FS_NAME_GET == b_mode )
            {
               // Write end of string UNICODE
               if( Is_unicode )
               {
                  ((FS_STR_UNICODE)sz_name)[0] = 0;
               }else{
                  sz_name[0] = 0;
               }
               return TRUE;  
            }
            else
            { 
               // if it is the end of filter
               if( Is_unicode )
               {
                  u16_unicode_szname = ((FS_STR_UNICODE)sz_name)[0];
               }else{
                  u16_unicode_szname = sz_name[0];
               }
               return fat_check_eof_name(u16_unicode_szname);
            }
         }
      }

      if( !g_b_string_length )
      {
         sz_name += (Is_unicode? 2 : 1 );
      }
      u8_pos_name++;
      ptr_entry+=2;
   }
}


//! Check end of name
//!
//! @param     character   value of character to check
//!
//! @return    TRUE, it is a character to signal a end of name (0,'\\','/')
//! @return    FALSE, otherwise
//!
Bool  fat_check_eof_name( U16 character )
{
   return (('\0'==character)||('\\'==character)||('/'==character));
}


//! This function returns a cache pointer on the current entry
//!
//! @return a pointer on the internal cache
//!
PTR_CACHE fat_get_ptr_entry( void )
{
   return &fs_g_sector[(fs_g_nav_fast.u16_entry_pos_sel_file * FS_SIZE_FILE_ENTRY) & FS_512B_MASK];
}


//! This function loads a memory sector in internal cache sector
//!
//! @param     b_load   TRUE,  load the cache with the memory sector corresponding <br>
//!                     FALSE, Don't change the sector cache but change the memory address of cache <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! Global variable used
//! IN : 
//!   fs_g_nav.u8_lun      drive number to read
//!   fs_gu32_addrsector   address to read (unit sector)
//! @endverbatim
//!
Bool  fat_cache_read_sector( Bool b_load )
{
   // Check if the sector asked is the same in cache
   if( (fs_g_sectorcache.u8_lun     == fs_g_nav.u8_lun )
   &&  (fs_g_sectorcache.u32_addr   == fs_gu32_addrsector ) )
   {
      return TRUE;
   }

   // Write previous cache before fill cache with a new sector
   if( !fat_cache_flush())
      return FALSE;
   
   // Delete informations about the caches
   fat_cache_reset();
   
   // Init sector cache
   fs_g_sectorcache.u32_addr = fs_gu32_addrsector;
   if( b_load )
   {
      // Load the sector from memory
      if( CTRL_GOOD != memory_2_ram( fs_g_nav.u8_lun  , fs_g_sectorcache.u32_addr, fs_g_sector))
      {
         fs_g_status = FS_ERR_HW;
         return FALSE;
      }
   }
   // Valid sector cache
   fs_g_sectorcache.u8_lun = fs_g_nav.u8_lun;
   return TRUE;
}


//! This function resets the sector cache
//!
void  fat_cache_reset( void )
{
   fs_g_sectorcache.u8_lun                = FS_BUF_SECTOR_EMPTY;
   fs_g_sectorcache.u8_modify             = FALSE;
   fs_g_sectorcache.u32_clusterlist_start = 0xFFFFFFFF;
}


#if (FS_LEVEL_FEATURES > FSFEATURE_READ)
//! This function clears the sector cache
//! 
void  fat_cache_clear( void )
{
   memset( fs_g_sector , 0 , FS_CACHE_SIZE );
}


//! This function sets a flag to signal that sector cache is modified
//!
void  fat_cache_sector_is_modify( void )
{
   fs_g_sectorcache.u8_modify = TRUE;
}
#endif  // FS_LEVEL_FEATURES


//! This function flushs the sector cache on the memory if necessary
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  fat_cache_flush( void )
{
   // If the cache is modified, then write the sector cache on the device
   if ( TRUE == fs_g_sectorcache.u8_modify )
   {      
      fs_g_sectorcache.u8_modify = FALSE; // Always clear, although an error occur
      if( mem_wr_protect( fs_g_sectorcache.u8_lun  ))
      {
         fs_g_status = FS_LUN_WP;
         return FALSE;
      }
      if (CTRL_GOOD != ram_2_memory( fs_g_sectorcache.u8_lun , fs_g_sectorcache.u32_addr , fs_g_sector ))
      {
         fs_g_status = FS_ERR_HW;
         return FALSE;
      }
   }
   return TRUE;
}



#if (FS_NB_NAVIGATOR > 1)
//! This function checks write access
//!
//! @return    TRUE,    write access on disk possibled
//! @return    FALSE,   File open then write access not possibled
//!
Bool  fat_check_nav_access_disk( void )
{
   U8 i;

   // For each navigators
   for( i=0 ; i!=(FS_NB_NAVIGATOR-1) ; i++ )
   {
      // Disk mounted ?
      if( FS_TYPE_FAT_UNM != fs_g_navext_fast[i].u8_type_fat )
      // Is it the same disk ?
      if( fs_g_nav.u8_lun == fs_g_navext[i].u8_lun )
      // Is it access file ?
      if( fs_g_navext_entry[i].u8_open_mode!=0 )
      {
         fs_g_status = FS_ERR_FILE_OPEN;
         return FALSE;  // File opened then write access not possibled
      }
   }
   return TRUE;
}


//! This function checks all access at current file
//!
//! @param     mode  TRUE,  check to write access <br>
//!                  FALSE, check to read access <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail : <br>
//!            mode TRUE,  File opened then write access not possibles <br>
//!            mode FALSE, File opened in write mode then read access not possibles <br>
//! @return    TRUE, access file possibles
//!
Bool  fat_check_nav_access_file( Bool mode )
{
   U8 i;

   // For each navigators
   for( i=0 ; i!=(FS_NB_NAVIGATOR-1) ; i++ )
   {
      // Disk mounted ?
      if( FS_TYPE_FAT_UNM != fs_g_navext_fast[i].u8_type_fat )
      // Is it the same disk ?
      if( fs_g_nav.u8_lun == fs_g_navext[i].u8_lun )
#if (FS_MULTI_PARTITION == ENABLED)
      // Is it the same partition ?
      if( fs_g_nav.u8_partition == fs_g_navext[i].u8_partition )
#endif
      // Is it the same directory ?
      if( fs_g_nav.u32_cluster_sel_dir == fs_g_navext[i].u32_cluster_sel_dir )
      // Is it the same file ?
      if( fs_g_nav_fast.u16_entry_pos_sel_file == fs_g_navext_fast[i].u16_entry_pos_sel_file )
      {
         if( mode )
         {
            // Is it open ?
            if( fs_g_navext_entry[i].u8_open_mode!=0 )
            {
               fs_g_status = FS_ERR_FILE_OPEN;
               return FALSE;  // File opened then write access not possibled
            }
         }
         else
         {
            // Is it open in write mode ?
            if( fs_g_navext_entry[i].u8_open_mode & FOPEN_WRITE_ACCESS )
            {
               fs_g_status = FS_ERR_FILE_OPEN_WR;
               return FALSE;  // File opened in write mode then read access not possibled
            }
         }
      }
   }
   return TRUE;
}


//! This function inverts the current navigation with another
//!
//! @param     u8_idnav    Id navigator to invert
//!
void  fat_invert_nav( U8 u8_idnav )
{
   _MEM_TYPE_SLOW_ U8 Temp[Max(Max(sizeof(Fs_management),sizeof(Fs_management_entry)),sizeof(Fs_management_fast))];

   if( u8_idnav == 0 )
      return;
   u8_idnav--;

   memcpy_ram2ram(Temp,                              (U8*)&fs_g_nav,                     sizeof(Fs_management));
   memcpy_ram2ram((U8*)&fs_g_nav,                    (U8*)&fs_g_navext[u8_idnav],        sizeof(Fs_management));
   memcpy_ram2ram((U8*)&fs_g_navext[u8_idnav],       Temp,                               sizeof(Fs_management));

   memcpy_ram2ram(Temp,                              (U8*)&fs_g_nav_entry,               sizeof(Fs_management_entry));
   memcpy_ram2ram((U8*)&fs_g_nav_entry,              (U8*)&fs_g_navext_entry[u8_idnav],  sizeof(Fs_management_entry));
   memcpy_ram2ram((U8*)&fs_g_navext_entry[u8_idnav], Temp,                               sizeof(Fs_management_entry));

   memcpy_ram2ram(Temp,                              (U8*)&fs_g_nav_fast,                sizeof(Fs_management_fast));
   memcpy_ram2ram((U8*)&fs_g_nav_fast,               (U8*)&fs_g_navext_fast[u8_idnav],   sizeof(Fs_management_fast));
   memcpy_ram2ram((U8*)&fs_g_navext_fast[u8_idnav],  Temp,                               sizeof(Fs_management_fast));
}


//! This function copys the main navigator to another navigator
//!
//! @param     u8_idnav    Id navigator to fill
//!
void  fat_copy_nav( U8 u8_idnav )
{
   if( 0 != u8_idnav)
   {
      u8_idnav--;
      memcpy_ram2ram((U8*)&fs_g_navext[u8_idnav],       (U8*)&fs_g_nav       , sizeof(Fs_management) );
      memcpy_ram2ram((U8*)&fs_g_navext_entry[u8_idnav], (U8*)&fs_g_nav_entry , sizeof(Fs_management_entry) );
      memcpy_ram2ram((U8*)&fs_g_navext_fast[u8_idnav],  (U8*)&fs_g_nav_fast  , sizeof(Fs_management_fast) );
      fs_g_navext_entry[u8_idnav].u8_open_mode=0;   // Clear open file flag
   }
}

#endif

// To clean the code 
#if (FS_NB_CACHE_CLUSLIST==1)
#  undef fs_g_u8_current_cache
#endif

