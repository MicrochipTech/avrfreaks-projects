/* This source file is part of the ATMEL AVR32-SoftwareFramework-1.3.0-AT32UC3B Release */
/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief FAT 12/16/32 Services.
 *
 * This file defines a useful set of functions for file navigation.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 *****************************************************************************/

/* Copyright (C) 2006-2008, Atmel Corporation All rights reserved.
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

//_____  I N C L U D E S ___________________________________________________
#include "conf_explorer.h"
#include "navigation.h"
#include "file.h"
#include LIB_CTRLACCESS


//_____ M A C R O S ________________________________________________________

#define ID_STREAM_ERR 0

//_____ D E F I N I T I O N S ______________________________________________

#if (FSFEATURE_WRITE == (FS_LEVEL_FEATURES & FSFEATURE_WRITE))
//! This variable store the current ID transfer used by the internal copy-paste routines
   _MEM_TYPE_SLOW_ U8 g_id_trans_memtomem = ID_STREAM_ERR;
#endif

#if (FS_NB_NAVIGATOR > 1)
//! This one containt the current navigator selected
   _MEM_TYPE_SLOW_ U8 fs_g_u8_nav_selected;
#endif

//_____ D E C L A R A T I O N S ____________________________________________

//**********************************************************************
//************************ String format select ************************
#if( (FS_ASCII == ENABLED) && (FS_UNICODE == ENABLED))
//! This function selects the UNICODE mode for all routines with FS_STRING parameter 
//!
//! @verbatim
//! If you have enabled the FS_ASCII AND FS_UNICODE define
//! then FS_STRING parameter can be a ASCII or UNICODE string.
//! @endverbatim
//!
void  nav_string_unicode( void )
{
   g_b_unicode = TRUE;
}
//! This function selects the ASCII mode for all routines with FS_STRING parameter 
//!
//! @verbatim
//! If you have enabled the FS_ASCII AND FS_UNICODE define
//! then FS_STRING parameter can be a ASCII or UNICODE string.
//! @endverbatim
//!
void  nav_string_ascii( void )
{
   g_b_unicode = FALSE;
}
#endif

//! This function selects the LENGTH string mode for all routines with FS_STRING parameter 
//!
//! @verbatim
//! In LENGTH string mode when you call a routine with FS_STRING parameter
//! only the size (16bits, unit ASCII or UNICODE) is returned in the first 16bits of string array.
//! @endverbatim
//!
void  nav_string_length_enable( void )
{
   g_b_string_length = TRUE;
}
//! This function deselects the LENGTH string mode for all routines with FS_STRING parameter 
//!
//! @verbatim
//! In LENGTH string mode when you call a routine with FS_STRING parameter
//! only the size (16bits, unit ASCII or UNICODE) is returned in the first 16bits of string array.
//! @endverbatim
//!
void  nav_string_length_disable( void )
{
   g_b_string_length = FALSE;
}

//**********************************************************************
//********************** To optimize speed access **********************

//! This function disables the disk check before each actions on disk
//!
//! @verbatim
//! By default, between each read/write access a check disk (test unit ready) is sended at device.
//! This check can reduce the speed access on specific disk.
//! @endverbatim
//!
void  nav_checkdisk_disable( void )
{
   g_b_no_check_disk = TRUE;
}

//! This function enables the disk check before each actions on disk
//!
//! @verbatim
//! By default, between each read/write access a check disk (test unit ready) is sended at device.
//! This check can reduce the speed access on specific disk.
//! @endverbatim
//!
void  nav_checkdisk_enable( void )
{
   g_b_no_check_disk = FALSE;
}


//**********************************************************************
//************** Initialise or Stop navigation module ****************** 


//! This function resets ALL navigations to init file system core
//!
//! @verbatim
//! Call this at the program startup or before a new session (e.g. USB Device exit)
//! @endverbatim
//!
void  nav_reset( void )
{
#if ( (FS_ASCII   == ENABLED) && (FS_UNICODE == ENABLED))
   g_b_unicode = TRUE;
#endif
   g_b_string_length = FALSE;
   g_b_no_check_disk = FALSE;

   fat_cache_reset();
   fat_cache_clusterlist_reset();

#if (FS_NB_NAVIGATOR > 1)
   {
   U8 i;
   // Reset variables of each navigators
   for( i=0 ; i!=FS_NB_NAVIGATOR ; i++ )
   {
      nav_select(i);
      fs_g_nav_fast.u8_type_fat = FS_TYPE_FAT_UNM; // By default the fat isn't mounted
      fs_g_nav.u8_lun = 0xFF;                      // By default don't select a drive
#if (FS_MULTI_PARTITION  ==  ENABLED)
      fs_g_nav.u8_partition=0;                     // By default select the first partition
#endif
      Fat_file_close;                              // By default no file is opened
   }
   // By default select the navigator 0
   fs_g_u8_nav_selected = 0;
   }
#else
   fs_g_nav_fast.u8_type_fat = FS_TYPE_FAT_UNM;    // By default the fat isn't mounted
   fs_g_nav.u8_lun = 0xFF;                         // By default don't select a drive
#  if (FS_MULTI_PARTITION  ==  ENABLED)
   fs_g_nav.u8_partition=0;                        // By default select the first partition
#  endif
   Fat_file_close;                                 // By default no file is opened
#endif // (FS_NB_NAVIGATOR > 1)
}


//! This function selects the navigation to use
//!
//! @param     u8_idnav    navigator identifier to select (0 to FS_NB_NAVIGATOR-1)
//!
//! @return    FALSE if ID navigator don't exist
//! @return    TRUE otherwise
//!
Bool  nav_select( U8 u8_idnav )
{
   if( FS_NB_NAVIGATOR <= u8_idnav )
   {
      fs_g_status = FS_ERR_BAD_NAV;             // The navigator doesn't exist
      return FALSE;
   }
#if (FS_NB_NAVIGATOR > 1)
   if( fs_g_u8_nav_selected != u8_idnav )
   {
      fat_invert_nav( fs_g_u8_nav_selected );   // Deselect previous navigator = Select default navigator
      fat_invert_nav( u8_idnav );               // Select new navigator
      fs_g_u8_nav_selected = u8_idnav;
   }
#endif
   return TRUE;
}


//! This function returns the navigation identifier used
//!
//! @return    u8_idnav    navigator identifier selected
//!
U8    nav_get( void )
{
#if (FS_NB_NAVIGATOR > 1)
   return fs_g_u8_nav_selected;
#else
   return 0;
#endif
}


//! This function copys the navigator information to another navigator
//!
//! @param     u8_idnav       navigator identifier where the main navigator will be copied
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! Use this routine to select quickly the same file in another navigator
//! @endverbatim
//!
Bool  nav_copy( U8 u8_idnav )
{
#if (FS_NB_NAVIGATOR > 1)
   if( fs_g_u8_nav_selected == u8_idnav )
      return FALSE;                    // It is the source and destination is the same navigator
   if( 0 == u8_idnav )
      u8_idnav = fs_g_u8_nav_selected; // the default navigator is invert with the current navigator
   if( 0 == u8_idnav)
      return FALSE;                    // It is the source and destination is the same navigator
   fat_copy_nav( u8_idnav );
   return TRUE;
#else
   u8_idnav++;
   return FALSE;                       // Copy impossible because only one navigator is available
#endif
}


//**********************************************************************
//********************* Drive navigation functions *********************


//! This function returns the number of devices availabled
//!
//! @return    number of devices, 0 = NO DEVICE AVAILABLED
//!
//! @verbatim
//! This value may be dynamic because it depends of memory drivers (e.g. Mass Storage disk on USB host mode)
//! @endverbatim
//!
U8    nav_drive_nb( void )
{
   return get_nb_lun(); // Number of devices = Number of lun
}


//! This function selects a drive in navigator but don't mount the disk partition
//!
//! @param     u8_number    device number (0 to nav_drive_nb()-1 )
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_drive_set( U8 u8_number )
{
   if ( !fat_check_noopen() )
      return FALSE;

   if (u8_number >= get_nb_lun() )
   {
      fs_g_status = FS_ERR_END_OF_DRIVE;   // The drive number is bad
      return FALSE;
   }

   if ( fs_g_nav.u8_lun == u8_number)
      return TRUE;   // It is the same drive number
      
   // Go to the device
   fs_g_nav.u8_lun = u8_number;
   fs_g_nav_fast.u8_type_fat = FS_TYPE_FAT_UNM;
#if (FS_MULTI_PARTITION  ==  ENABLED)
   fs_g_nav.u8_partition=0;   // by default select the first partition
#endif
   return TRUE;
}


//! This function returns the selected drive number
//!
//! @return    0 to nav_drive_nb()-1
//! @return    0xFF in case of no drive selected
//!
U8    nav_drive_get( void )
{
#if (FS_MULTI_PARTITION  ==  ENABLED)
   if(0xFF == fs_g_nav.u8_lun)
      return 0xFF;
   return ((fs_g_nav.u8_lun*4) + fs_g_nav.u8_partition); // Maximum 4 partitions per device
#else
   return (fs_g_nav.u8_lun);
#endif
}


//! This function returns the selected drive letter
//!
//! @return    'A','B',...
//! @return    'X', in case of no drive selected
//!
U8    nav_drive_getname( void )
{
   if(0xFF == fs_g_nav.u8_lun)
      return 'X';
#if (FS_MULTI_PARTITION  ==  ENABLED)
   return ('A' + (fs_g_nav.u8_lun*4) + fs_g_nav.u8_partition); // Maximum 4 partitions per device
#else
   return ('A' + fs_g_nav.u8_lun);
#endif
}


#if (FSFEATURE_WRITE_COMPLET == (FS_LEVEL_FEATURES & FSFEATURE_WRITE_COMPLET))
//! This function formats the current drive (=disk)
//!
//! @param     u8_fat_type    Select the format type<br>
//!            FS_FORMAT_DEFAULT, The system chooses the better FAT format <br>
//!            FS_FORMAT_FAT, The FAT12 or FAT16 is used to format the drive, if possible (disk space <2GB) <br>
//!            FS_FORMAT_FAT32, The FAT32 is used to format the drive, if possible (disk space >32MB) <br>
//!            FS_FORMAT_NOMBR_FLAG, if you don't want a MRB on the disk then add this flag (e.g. specific partition structure on a CD support)
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! WARNING: This routine can't format a multi-partiton, if the disk contains a multi-partition
//! then this one is erased and replaced by a single partition on ALL disk space.
//! @endverbatim
//!
Bool  nav_drive_format( U8 u8_fat_type )
{
   if ( !fat_check_noopen() )
      return FALSE;
   if ( !fat_check_nav_access_disk() )
      return FALSE;
   if ( !fat_format( u8_fat_type ) )
      return FALSE;
   return fat_mount();
}
#endif  // FS_LEVEL_FEATURES


//**********************************************************************
//******************* Partition navigation functions ******************* 


#if (FS_MULTI_PARTITION  ==  ENABLED)
//! This function returns the number of partitions present on drive
//!
//! @return    u8_number   number of partitions
//!
U8    nav_partition_nb( void )
{
   return fat_get_nbpartition();
}


//! This function selects a partition on drive
//!
//! @param     partition_number     partition number (0 to 3)
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_partition_set( U8 partition_number )
{
   if ( fs_g_nav.u8_partition == partition_number)
      return TRUE;   // It is the same
      
   if ( !fat_check_noopen() )
      return FALSE;

   // Go to partition
   fs_g_nav.u8_partition = partition_number;
   fs_g_nav_fast.u8_type_fat = FS_TYPE_FAT_UNM;
   return TRUE;
}
#endif


//! This function mounts the selected partition 
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
//! @verbatim
//! If the FS_MULTI_PARTITION option is disabled
//! then the mount routine selects the first partition supported by file system. <br>
//! After mount, the file list contains files and directories of ROOT directory 
//! @endverbatim
//!
Bool  nav_partition_mount( void )
{
   if ( !fat_check_noopen() )
      return FALSE;

   if( FS_TYPE_FAT_UNM != fs_g_nav_fast.u8_type_fat)
   {
      // Already mounted
      // Go to root directory
      fs_g_nav.u32_cluster_sel_dir   = 0;
      // No file is selected by default
      fat_clear_entry_info_and_ptr();
      return TRUE;
   }

   return fat_mount();
}


//! This function gives the partition type
//!
//! @return partition type: FS_TYPE_FAT_12, FS_TYPE_FAT_16, FS_TYPE_FAT_32
//! @return FS_TYPE_FAT_UNM, in case of error or unknow format
//!
U8    nav_partition_type( void )
{
   fat_check_device();
   if( FS_TYPE_FAT_UNM == fs_g_nav_fast.u8_type_fat)
   {
      nav_partition_mount();
   }
   return fs_g_nav_fast.u8_type_fat;
}


//! This function reads or writes the serial number on the selected partition 
//!
//! @param     b_action    to select the action <br>
//!                        FS_SN_READ to read serial number <br>
//!                        FS_SN_WRITE to write serial number <br>
//! @param     a_u8_sn     pointer on an array (4 bytes) <br>
//!                        if FS_SN_READ, then the array is used to store the serial number <br>
//!                        if FS_SN_WRITE, then the array is used to give the new serial number <br>
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
Bool  nav_partition_serialnumber( Bool b_action , U8 _MEM_TYPE_SLOW_ *a_u8_sn )
{
   if( !fat_check_mount())
      return FALSE;
      
   return fat_serialnumber( b_action , a_u8_sn );
}


//! This function reads or writes the label of selected partition
//!
//! @param     b_action    to select the action <br>
//!                        FS_LABEL_READ to read label <br>
//!                        FS_LABEL_WRITE to write label <br>
//! @param     sz_label    pointer on a ASCII string (11 chars + NULL terminator =12 bytes) <br>
//!                        if FS_LABEL_READ, then the string is used to store label <br>
//!                        if FS_LABEL_WRITE, then the string is used to give the new label <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_partition_label( Bool b_action , FS_STRING sz_label )
{
   _MEM_TYPE_SLOW_   Fs_index index;
   Bool status = TRUE;
         
   index = nav_getindex();    // Save current position

   // Go to root dir
   if( nav_dir_root())
   {
      // Find field label
      fs_g_nav_fast.u16_entry_pos_sel_file = 0; // Go to first entry
      while( 1 )
      {
         if ( !fat_read_dir())
         {
            status = FALSE;
            break;               // error
         } 
         if( fat_entry_label( FS_LABEL_READ , NULL ) )
            break;
         if( FS_ERR_ENTRY_EMPTY == fs_g_status )
            break;
         fs_g_nav_fast.u16_entry_pos_sel_file++;
      }
      if( TRUE == status )
      {
         // Find OK
         if( FS_LABEL_READ == b_action )
         {
            // Read field label
            if( !fat_entry_label( FS_LABEL_READ , sz_label ) )
               sz_label[0]=0;    // empty name
         }else{
            // Change field label
            status = fat_entry_label( FS_LABEL_WRITE , sz_label );
         }
      }  
   }
   nav_gotoindex( &index );   // Restore the position
   return status;
}


//! This function returns partition total space
//!
//! @return    number of sectors
//! @return    0, in case of error
//!
//! @verbatim
//! You shall mounted the partition before call this routine
//! @endverbatim
//!
U32   nav_partition_space( void )
{
   fat_check_device();
   if (FS_TYPE_FAT_UNM == fs_g_nav_fast.u8_type_fat)
      return 0;
   return (fs_g_nav.u32_CountofCluster * fs_g_nav.u8_BPB_SecPerClus);
}


//! This function returns the partition cluster size
//!
//! @return    cluster size (unit sector)
//!
U8    nav_partition_cluster_size( void )
{
   return fs_g_nav.u8_BPB_SecPerClus;
}

//! This function returns the partition free space
//!
//! @return    number of free sectors
//! @return    0 in case of error or full partition
//!
//! @verbatim
//! You shall mounted the partition before call this routine
//! @endverbatim
//!
U32   nav_partition_freespace( void )
{
   fat_check_device();
   if (FS_TYPE_FAT_UNM == fs_g_nav_fast.u8_type_fat)
      return 0;
   return fat_getfreespace();
}


//! This function returns the partition space free in percent
//!
//! @return    percent of free space (0% to 100%)
//! @return    0% in case of error or full partition
//!
//! @verbatim
//! To speed up the compute, the resultat have an error delta of 1%
//! @endverbatim
//!
U8    nav_partition_freespace_percent( void )
{
   fat_check_device();
   if (FS_TYPE_FAT_UNM == fs_g_nav_fast.u8_type_fat)
      return 0;
   return fat_getfreespace_percent();
}


//**********************************************************************
//****************** File list navigation functions ******************** 


//! This function resets the selection pointer, so "no file selected" in file list
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
Bool  nav_filelist_reset( void )
{
   if ( !fat_check_mount_noopen())
      return FALSE;

   // No file selected and reset navigation
   fat_clear_entry_info_and_ptr();
   return TRUE;
}


//! This function checks if a file is selected
//!
//! @return    TRUE  if a file is selected
//! @return    FALSE if no file is selected
//!
Bool  nav_filelist_validpos( void )
{
   return fat_check_mount_select_noopen();
}


//! This function checks if no file is open
//!
//! @return    TRUE  if no file is open
//! @return    FALSE if a file is open
//!
Bool  nav_filelist_fileisnotopen( void )
{
   return fat_check_noopen();
}


//! This function moves the selection pointer in file list
//!
//! @param     u16_nb      numbers of file to jump before stopping action <br>
//!                        0, stop at the first file found <br>
//!                        1, stop at the second file found <br>
//!
//! @param     b_direction search direction <br>
//!                        FS_FIND_NEXT, move to next file or directory
//!                        FS_FIND_PREV, move to previous file or directory
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
//! @verbatim
//! Note: if no file is selected then nav_filelist_set( 0 , FS_NEXT ) goes to the first entry of the file list.
//! @endverbatim
//!
Bool  nav_filelist_set( U16 u16_nb , Bool b_direction )
{
   U16   u16_ptr_save_entry;
   U16   u16_save_pos_sel_file;
   Bool  b_save_entry_type;
   Bool  b_find_last_entry = FALSE;

   if ( !fat_check_mount_noopen())
      return FALSE;
      
   // Save the current selection
   u16_ptr_save_entry      = fs_g_nav_fast.u16_entry_pos_sel_file;
   u16_save_pos_sel_file   = fs_g_nav.u16_pos_sel_file;
   b_save_entry_type       = fs_g_nav.b_mode_nav;

   // Loop in directory
   while( 1 )
   {
      if(( FS_FIND_NEXT == b_direction )
      || ( b_find_last_entry ) )
      {
         if ( FS_END_FIND == fs_g_nav_fast.u16_entry_pos_sel_file )
         {
            // Too many files in directory (case impossible)
            fs_g_status = FS_ERR_FS;
            break;
         }
         fs_g_nav_fast.u16_entry_pos_sel_file++;      // Update entry position
      }
      else
      {
         if ( FS_NO_SEL == fs_g_nav_fast.u16_entry_pos_sel_file )
         {
            // No selected file then previous action impossible
            fs_g_status = FS_ERR_NO_FIND;
            break;
         }
         if ( 0 == fs_g_nav_fast.u16_entry_pos_sel_file )
         {
            // beginning of directory
            if ( FS_DIR == fs_g_nav.b_mode_nav )
            {
               // End of directory scan, then no previous action possible
               fs_g_status = FS_ERR_NO_FIND;
               break;
            }
            // End of file scan, then find last directory
            b_find_last_entry = TRUE;
         }else{
            fs_g_nav_fast.u16_entry_pos_sel_file--;   // Update entry position
         }
      }

      if( !fat_read_dir())
      {
         if( FS_ERR_OUT_LIST != fs_g_status )
            break; // Error
      }else{
         if ( fat_entry_check( fs_g_nav.b_mode_nav ) )
         {
           // HERE, the file entry match with the type seached
           
           if( b_find_last_entry )
             continue;  // The search of last directory is on going then continue the search
   
           // Update position in directory
           if ( FS_FIND_NEXT == b_direction )
              fs_g_nav.u16_pos_sel_file++;
           else
              fs_g_nav.u16_pos_sel_file--;
   
           if (0 == u16_nb)
           {
              // It is the end of move then update file information
              fat_get_entry_info();
              return TRUE;         // NB FILE FIND
           }
           u16_nb--;
           continue;
         }
      }

      // Here error, check type of error
      if(( FS_ERR_ENTRY_EMPTY == fs_g_status )
      || ( FS_ERR_OUT_LIST    == fs_g_status ) )
      {
         // Here, end of the directory
         if( b_find_last_entry )
         {
            // Re enable the previous command at the end of directory to find the last directory entry
            b_find_last_entry = FALSE;
            fs_g_nav.b_mode_nav = FS_DIR;
            continue;
         }
         // Here, a next action is on going
         if ( FS_FILE == fs_g_nav.b_mode_nav )
         {
            // End of next file action then end of next action
            fs_g_status = FS_ERR_NO_FIND; // No file found
            break;                        // end of search
         }else{
            // End of next dir action then starts the next file action at the beginning of directory
            fs_g_nav_fast.u16_entry_pos_sel_file = 0xFFFF;
            fs_g_nav.b_mode_nav = FS_FILE;
         }
      }
   }  // end of loop while(1)

   fs_g_nav.b_mode_nav                    = b_save_entry_type;
   fs_g_nav_fast.u16_entry_pos_sel_file   = u16_ptr_save_entry;
   fs_g_nav.u16_pos_sel_file              = u16_save_pos_sel_file;
   return FALSE;
}


//! This function returns the position of selected file in file list
//!
//! @return    position of selected file (0 is the first position)
//! @return    FS_NO_SEL, in case of no file selected
//!
U16   nav_filelist_get( void )
{
   return fs_g_nav.u16_pos_sel_file;
}


//! This function goes at a position in file list
//!
//! @param     u16_newpos     new position to select (0 is the first position)
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_filelist_goto( U16 u16_newpos )
{
   U16 u16_current_pos;
   u16_current_pos = nav_filelist_get();
   if( u16_newpos < (u16_current_pos/2) )
   {
      // Restart at the beginning of list to accelerate the search
      if( !nav_filelist_reset() )
         return FALSE;      
      u16_current_pos = FS_NO_SEL;
   }
   if (FS_NO_SEL == u16_current_pos)
   {
      return nav_filelist_set( u16_newpos, FS_FIND_NEXT );
   }
   else
   {
      if (u16_newpos < u16_current_pos)
      {
         return nav_filelist_set( u16_current_pos -u16_newpos -1 , FS_FIND_PREV );
      }
      if (u16_newpos > u16_current_pos)
      {
         return nav_filelist_set( u16_newpos -u16_current_pos - 1 , FS_FIND_NEXT );
      }
   }
   return TRUE;
}


//! This function searchs a file name in file list 
//!
//! @param     sz_name        name to search (UNICODE or ASCII) <br>
//!                           It must be terminate by NULL or '*' value
//! @param     b_match_case   FALSE to ignore the case
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! This function starts a search at the next position of the current in file list 
//! @endverbatim
//!
Bool  nav_filelist_findname( const FS_STRING sz_name , Bool b_match_case )
{
   while( 1 )
   {
      if ( !nav_filelist_set( 0, FS_FIND_NEXT ))
         return FALSE;
      if ( nav_file_name( sz_name , 0 , FS_NAME_CHECK , b_match_case ))
         return TRUE;
   }
}


//! This function checks the end of file list
//!
//! @return    FALSE, NO end of file list
//! @return    TRUE, in case of end of list or error
//!
Bool  nav_filelist_eol( void )
{
   if( FALSE == nav_filelist_set( 0 , FS_FIND_NEXT ) )
      return TRUE;   // End of list or error (remark: the position haven't changed)
   
   if( 0 != fs_g_nav.u16_pos_sel_file )
   {
      // Go to previous position
      nav_filelist_set( 0 , FS_FIND_PREV );
   }else{
      // No select file before, then reset position
      fs_g_nav_fast.u16_entry_pos_sel_file= FS_NO_SEL;
      fs_g_nav.u16_pos_sel_file           = FS_NO_SEL;
      fs_g_nav.b_mode_nav                 = FS_DIR;
   }
   return FALSE;
}


//! This function checks the beginning of file list
//!
//! @return    FALSE, it is not the beginning of file list
//! @return    TRUE, in case of the file selected is the first file, or in case of error
//!
Bool  nav_filelist_bol( void )
{
   if( FALSE == nav_filelist_set( 0 , FS_FIND_PREV ) )
      return TRUE;   // End of list or error (remark: the position haven't changed)
   // Go to previous position
   nav_filelist_set( 0 , FS_FIND_NEXT );
   return FALSE;
}


//! This function checks the presence of files or directories in file list
//!
//! @param     b_type   FS_DIR  to check the directory presence <br>
//!                     FS_FILE to check the file presence <br>
//!
//! @return    TRUE, in case of a file or a directory exists
//! @return    FALSE, in case of no file or no directory exists, or error
//!
Bool  nav_filelist_exist( Bool b_type )
{
   Bool status;
   U16   u16_save_position;
   
   // Save current position
   u16_save_position = fs_g_nav.u16_pos_sel_file;
   // Go to first file or directory
   status = nav_filelist_first( b_type );
   // Restore previous position
   nav_filelist_reset();
   if ( u16_save_position != FS_NO_SEL )
   {  
      nav_filelist_set( u16_save_position , FS_FIND_NEXT );
   }
   return status;   
}


//! This function computes the number of files or directories in file list
//!
//! @param     b_type   FS_DIR  to compute the number of directories <br>
//!                     FS_FILE to compute the number of files <br>
//!
//! @return    number of files or directories in file list
//!
U16   nav_filelist_nb( Bool b_type )
{
   U16   u16_save_position;
   U16   u16_save_number_dir;
   U16   u16_save_number_file;
   
   // Save current position
   u16_save_position = fs_g_nav.u16_pos_sel_file;
   // Reset position
   if ( !nav_filelist_reset())
      return 0;
   // Scan all directory
   u16_save_number_dir  = 0;
   u16_save_number_file = 0;
   while( nav_filelist_set( 0 , FS_FIND_NEXT ) )
   {
      if( FS_FILE == fs_g_nav.b_mode_nav )
         u16_save_number_file++;    // It is a file
      else
         u16_save_number_dir++;     // It is a directory
   }
   // Restore previous position
   nav_filelist_reset();
   if ( u16_save_position != FS_NO_SEL )
   {
      nav_filelist_set( u16_save_position , FS_FIND_NEXT );
   }
   // Return the value asked
   if( FS_FILE == b_type )
      return u16_save_number_file;
   else
      return u16_save_number_dir;
}


//! This function goes to at the first file or directory in file list
//!
//! @param     b_type   FS_DIR  to go at the first directory <br>
//!                     FS_FILE to go at the first file <br>
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
Bool  nav_filelist_first( Bool b_type )
{
   // Reset position
   if ( !nav_filelist_reset())
      return FALSE;
   // Find the first file or directory
   while( nav_filelist_set( 0 , FS_FIND_NEXT ) )
   {
      if( b_type == fs_g_nav.b_mode_nav )
         return TRUE;   // First file or directory found
   }
   fs_g_status = FS_ERR_NO_FIND;
   return FALSE;
}


//! This function goes to at the last file or directory in file list
//!
//! @param     b_type   FS_DIR  to go at the last directory <br>
//!                     FS_FILE to go at the last file <br>
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
Bool  nav_filelist_last( Bool b_type )
{
   U16 u16_nb;

   // Get number of file or directory
   u16_nb = nav_filelist_nb( b_type  );
   if( 0 == u16_nb )
   {
      fs_g_status = FS_ERR_NO_FIND;
      return FALSE;  // NO FILE FOUND
   }
   // Go to the first file or directory
   if ( !nav_filelist_first( b_type ))
      return FALSE;
   // If there are more one file or directory, then go to at the last of list
   if( 1 == u16_nb )
      return TRUE;
   u16_nb -= 2;
   return nav_filelist_set( u16_nb , FS_FIND_NEXT );
}


//**********************************************************************
//************************ Index functions *****************************


//! This function returns a small index on the selected file
//!
//! @return    It is a small index structure with information on selected file (disk, partition, dir, file/dir selected)
//!
//! @verbatim
//! This routine is interresting to save a file position in small variable.
//! This pointer allow to reinit a navigator quickly with nav_gotoindex() routine.
//! @endverbatim
//!
Fs_index nav_getindex( void )
{
   Fs_index index;

   // Fill index structure
   index.u8_lun                  = fs_g_nav.u8_lun;
#if (FS_MULTI_PARTITION  ==  ENABLED)
   index.u8_partition            = fs_g_nav.u8_partition;
#endif
   index.u32_cluster_sel_dir     = fs_g_nav.u32_cluster_sel_dir;
   index.u16_entry_pos_sel_file  = fs_g_nav_fast.u16_entry_pos_sel_file;
   return index;
}


//! This function selects a file in the navigator via a file index
//!
//! @param     index       structure with information about file to select (disk, partition, dir, file/dir selected )
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! This routine allow to reinit a navigator quickly via a file index (disk, partition, dir, file/dir selected )
//! To get a file index, you shall used the routine nav_getindex().
//! @endverbatim
//!
Bool  nav_gotoindex( const Fs_index _MEM_TYPE_SLOW_ *index )
{
   // Select the drive and partition corresponding at file index
   if( !nav_drive_set( index->u8_lun ))
      return FALSE;
#if (FS_MULTI_PARTITION  ==  ENABLED)
   if( !nav_partition_set(index->u8_partition))
      return FALSE;
#endif
   if( !nav_partition_mount())
      return FALSE;

   // Select the directory corresponding at file index
   fs_g_nav.u32_cluster_sel_dir   = index->u32_cluster_sel_dir;

   // Search the file position corresponding at file index
   if ( !nav_filelist_reset())
      return FALSE;
   while( fs_g_nav_fast.u16_entry_pos_sel_file != index->u16_entry_pos_sel_file )
   {
      if( !nav_filelist_set( 0 , FS_FIND_NEXT ) )
      {
         nav_filelist_reset();
         return FALSE;
      }
   }
   return TRUE;
}


//**********************************************************************
//************************ Directory functions *************************


//! This function initializes the file list on the root directory
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
Bool  nav_dir_root( void )
{
   return nav_partition_mount();
}


//! This function enters in the selected directory in file list
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
//! @verbatim
//! After this routine the file list changes and contains the files and directories of the new directory.
//! By default no file is selected.
//! @endverbatim
//!
Bool  nav_dir_cd( void )
{
   if ( !fat_check_mount_select_noopen())
      return FALSE;
      
   // The current selection, is it a directory ?
   if ( !fat_entry_is_dir())
      return FALSE;

   // Select the current directory
   fs_g_nav.u32_cluster_sel_dir = fs_g_nav_entry.u32_cluster;

   // Reset file list
   if( FALSE == nav_filelist_reset())
      return FALSE;
   return TRUE;
}


//! This function goes to the parent directory
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
//! @verbatim
//! After, the file list changes and contains the files and directories of the new directory.
//! By default, the file selected in file list is the previous (children) directory.
//! @endverbatim
//!
Bool  nav_dir_gotoparent( void )
{
   U32 u32_cluster_old_dir;

   if (!fat_check_mount_noopen())
      return FALSE;
   
   if (0 == fs_g_nav.u32_cluster_sel_dir)
   {
      fs_g_status = FS_ERR_IS_ROOT;        // There aren't parent
      return FALSE;
   }

   // Select and read information about directory ".."
   fs_g_nav_fast.u16_entry_pos_sel_file = 1;
   if ( !fat_read_dir())
      return FALSE;
   fat_get_entry_info();
   // Save the children directory cluster
   u32_cluster_old_dir = fs_g_nav.u32_cluster_sel_dir;
   
   // Select the parent directory via information present in the current directory ".."
   fs_g_nav.u32_cluster_sel_dir = fs_g_nav_entry.u32_cluster;

   // Select the children directory in new directory (=parent directory)
   if( FALSE == nav_filelist_reset())
      return FALSE;
   while( nav_filelist_set( 0 , FS_FIND_NEXT ) )
   {
      if (fs_g_nav_entry.u32_cluster == u32_cluster_old_dir)
         return TRUE;         // It is the children directory
   }
   fs_g_status = FS_ERR_FS;
   return FALSE;
}


//! This function returns the directory name corresponding at the file list
//!
//! @param     sz_path        string to store the name (ASCII or UNICODE )
//! @param     u8_size_max    string size (unit ASCII or UNICODE )
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_dir_name( FS_STRING sz_path  , U8 u8_size_max  )
{
   _MEM_TYPE_SLOW_   Fs_index index;
   U8 u8_i, u8_character;
   Bool status = FALSE;

   if ( !fat_check_mount_noopen())
      return FALSE;

   index = nav_getindex();       // Save current position
   if ( nav_dir_gotoparent() )   // Go to parent directory (this routine select the children directory)
   {
      status = nav_file_name( sz_path  , u8_size_max , FS_NAME_GET , FALSE  );
   }
   else
   {
      // No parent directory, then it is the root directory
      if( g_b_string_length )
      {
         ((FS_STR_UNICODE)sz_path )[0] = 3;  // 3 chars for path "x:"
         status = TRUE;
      }else
      
      if ( FS_ERR_IS_ROOT == fs_g_status )
      {
         // Create a device name
         for( u8_i = 0 ; u8_i<3 ; u8_i++ )
         {
            switch( u8_i )
            {
               case 0:
               u8_character = nav_drive_getname();    // Letter
               break;
               case 1:
               u8_character = ':';            			// ":"
               break;
               case 2:
               u8_character = 0;              			// end of string
               break;
            }
            if( Is_unicode )
            {
               ((FS_STR_UNICODE)sz_path )[0] = u8_character;
            }else{
               sz_path [0] = u8_character;
            }
            sz_path  += (Is_unicode? 2 : 1 );
         }
         status = TRUE;
      }
   }
   nav_gotoindex( &index );      // Restore the position
   return status;
}


#if (FSFEATURE_WRITE_COMPLET == (FS_LEVEL_FEATURES & FSFEATURE_WRITE_COMPLET))
//! This function creates a directory in the directory corresponding at file list
//!
//! @param     sz_name     directory name (ASCII or UNICODE )
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_dir_make( const FS_STRING sz_name  )
{
   if ( !fat_check_mount_noopen())
      return FALSE;

   // Create an entry file
   if ( !nav_file_create( sz_name ))
      return FALSE;
      
   // Allocate one cluster for the new directory
   MSB0(fs_g_seg.u32_addr)=0xFF;    // It is a new cluster list
   fs_g_seg.u32_size_or_pos = 1;    // Only one sector (= one cluster)
   if ( !fat_allocfreespace())
      return FALSE;

   // Save information about the new directory
   fs_g_nav_entry.u32_cluster = fs_g_seg.u32_addr; // First cluster of the directory returned by alloc_free_space
   fs_g_nav_entry.u32_size    = 0;                 // The directory size is null
   fs_g_nav_entry.u8_attr     = FS_ATTR_DIRECTORY; // Directory attribut
 
   // Initialize the values in the new directory
   if ( !fat_initialize_dir())
      return FALSE;

   // Write directory information in her entry file
   if ( !fat_read_dir())
      return FALSE;
   fat_write_entry_file();
   
   // Go to position of new directory (it is the last directory)
   return nav_filelist_last( FS_DIR );
}
#endif  // FS_LEVEL_FEATURES


//! This function returns the full path of the selection
//!
//! @param     sz_path              string to store the path (ASCII or UNICODE )
//! @param     u8_size_path         string size (unit ASCII or UNICODE )
//! @param     b_view_file_select   TRUE, to include in path the selected file name
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_getcwd( FS_STRING sz_path  , U8 u8_size_path , Bool b_view_file_select  )
{
   _MEM_TYPE_SLOW_   Fs_index index;
   _MEM_TYPE_SLOW_   U16 u16_lgt=0;    // Only used if LENGTH string mode enabled
   U8 u8_save_size_path=0, u8_i;
   Bool status = FALSE;                // Error by default

   if ( !fat_check_mount_noopen())
      return FALSE;

   index = nav_getindex();             // Save current position

   if( g_b_string_length )
   { 
      u16_lgt = 1;   // 1 for NULL terminate
   }
   else
   {
      u8_save_size_path = u8_size_path;
      // Set NULL terminator at the end of path
      u8_size_path--;
      if( Is_unicode )
      {
         ((FS_STR_UNICODE)sz_path )[u8_size_path] = 0;
      }else{
         sz_path [u8_size_path]   = 0;
      }
   }

   // For each directory of path
   while( 1 )
   {
      if( b_view_file_select )
      {
         // Write the selected file or directory in path
         b_view_file_select = FALSE;
         if( !fat_check_select() )
            continue;   // No selected file or directory, then restart loop to go at the first directory of path
      }
      else
      {
         // Go to parent directory and selects the children directory
         if( !nav_dir_gotoparent() )
            break;
      }
   
      // Read name of selected file or directory
      if( !nav_file_name( sz_path  , u8_size_path , FS_NAME_GET, FALSE  ))
         break;

      if( g_b_string_length )
      {
         // Update total length
         u16_lgt += ((FS_STR_UNICODE)sz_path )[0]; // 0 = -1 to remove NULL terminated and +1 for '\\'
         continue;
      }
      
      // Compute the size of name
      u8_i = 0;
      while( 1 )
      {
         if( Is_unicode )
         {
            if( 0 == ((FS_STR_UNICODE)sz_path )[u8_i])
               break;
         }else{
            if( 0 == sz_path [u8_i])
               break;
         }
         u8_i++;
      }
      
      // Check the string size
      if( (u8_i+1) == u8_size_path )
      {
         fs_g_status = FS_ERR_BUFFER_FULL;   // The path string is full
         break;
      }
      
      // Move the name at the end of path
      while( 0 != u8_i )
      {
         u8_i--;
         u8_size_path--;
         if( Is_unicode )
         {
            ((FS_STR_UNICODE)sz_path )[u8_size_path] = ((FS_STR_UNICODE)sz_path )[u8_i];
         }else{
            sz_path [u8_size_path]   = sz_path [u8_i];
         }
      }
      // Set '\' char before the name
      u8_size_path--;
      if( Is_unicode )
      {
         ((FS_STR_UNICODE)sz_path )[u8_size_path] = '\\';
      }else{
         sz_path [u8_size_path]   = '\\';
      }
   }

   if ( FS_ERR_IS_ROOT == fs_g_status )
   {
      if( g_b_string_length )
      {
         // Update and write total length
         ((FS_STR_UNICODE)sz_path )[0] = u16_lgt +2 ; //+2 for "x:"
         status = TRUE;
      }
      else
      {
         // End of path then add device name
         if( 2 > u8_size_path )
         {
            fs_g_status = FS_ERR_BUFFER_FULL;   // The path string is full
         }
         else
         {
            // Create a device name
            if( Is_unicode )
            {
               ((FS_STR_UNICODE)sz_path )[0] = nav_drive_getname(); 	// Letter
               ((FS_STR_UNICODE)sz_path )[1] = ':';             		// ":"
            }else{
               sz_path [0] = nav_drive_getname(); 	// Letter
               sz_path [1] = ':';             		   // ":"
            }
      
            // The path is stored at the end, then move the path at the beginning
            u8_i = 2;
            while( u8_save_size_path != u8_size_path )
            {
               if( Is_unicode )
               {
                  ((FS_STR_UNICODE)sz_path )[u8_i] = ((FS_STR_UNICODE)sz_path )[u8_size_path];
               }else{
                  sz_path [u8_i] = sz_path [u8_size_path];
               }
               u8_i++;
               u8_size_path++;
            }
            status = TRUE;
         }
      }
   } // else Error system
   
   nav_gotoindex( &index );   // Restore the position
   return status;
}


//! This function selects a disk position via a path
//!
//! @param     sz_path           path string (ASCII or UNICODE )
//! @param     b_match_case      FALSE to ignore the case
//! @param     b_create          TRUE, if path no exists then create it <br>
//!                              FALSE, no create path <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! The syntact "./../../file_name" is supported.
//! With syntact "./dir_parent/directory_name"  the file list corresponding at "dir_parent" and "directory_name" is selected.
//! With syntact "./dir_parent/directory_name/" the file list corresponding at "directory_name" and no file is selected.
//! @endverbatim
//!
Bool  nav_setcwd( FS_STRING sz_path , Bool b_match_case , Bool b_create )
{
   _MEM_TYPE_SLOW_   Fs_index index;
   FS_STRING sz_save_path = 0;
   Bool b_create_name = FALSE;

   if ( !fat_check_noopen())
      return FALSE;

   index = nav_getindex();             // Save current position

   // Check syntact "\path..."
   if( (( Is_unicode) && (('\\'  == ((FS_STR_UNICODE)sz_path )[0]) || ('/'  == ((FS_STR_UNICODE)sz_path )[0])) )
   ||  ((!Is_unicode) && (('\\'  == sz_path [0]) || ('/'  == sz_path [0])) ) )
   {
      // Go to the root of current drive
      if( !nav_dir_root())
         goto nav_setcwd_fail;
      sz_path  += (Is_unicode? 2 : 1 );
   }else
   
   // Check syntact "x:\path..."
   if( (( Is_unicode) && (( ':'  == ((FS_STR_UNICODE)sz_path )[1] ) && (('\\'  == ((FS_STR_UNICODE)sz_path )[2] ) || ('/'  == ((FS_STR_UNICODE)sz_path )[2]))) )
   ||  ((!Is_unicode) && (( ':'  == sz_path [1] ) && (('\\'  == sz_path [2] ) || ('/'  == sz_path [2]))) ) )
   {
      // Go to the drive
      if( Is_unicode )
      {
         if( !nav_drive_set( ((FS_STR_UNICODE)sz_path )[0]-'A' ) )
            goto nav_setcwd_fail;
      }else{
         if( !nav_drive_set( sz_path [0]-'A' ) )
            goto nav_setcwd_fail;
      }
      if( !nav_partition_mount())
         goto nav_setcwd_fail;
      sz_path  += 3*(Is_unicode? 2 : 1 );
   }else

   // Check syntact ".\path..."
   if( (( Is_unicode) && (( '.'  == ((FS_STR_UNICODE)sz_path )[0] ) && (('\\'  == ((FS_STR_UNICODE)sz_path )[1] ) || ('/'  == ((FS_STR_UNICODE)sz_path )[1] ))) )
   ||  ((!Is_unicode) && (( '.'  == sz_path [0] ) && (('\\'  == sz_path [1] ) || ('/'  == sz_path [1] ))) ) )
   {
      // Search in current directory
      sz_path  += 2*(Is_unicode? 2 : 1 );
   }else
   
   {
      // Check syntact "..\..\path..."
      if( Is_unicode )
      {
         while(( '.'  == ((FS_STR_UNICODE)sz_path )[0] )
         &&    ( '.'  == ((FS_STR_UNICODE)sz_path )[1] )
         &&    (('\\'  == ((FS_STR_UNICODE)sz_path )[2]) || ('/'  == ((FS_STR_UNICODE)sz_path )[2]) || (0  == ((FS_STR_UNICODE)sz_path )[2])) )
         {
            // Go to parent directory
            if( !nav_dir_gotoparent() )
               goto nav_setcwd_fail;
            sz_path  += (2*2); // jump ".."
            if( 0 != ((FS_STR_UNICODE)sz_path )[0])
               sz_path  += (2*1); // jump "/"
         }
      }else{
         while(( '.'  == sz_path [0] )
         &&    ( '.'  == sz_path [1] )
         &&    (('\\'  == sz_path [2]) || ('/'  == sz_path [2]) || (0  == sz_path [2])) )
         {
         // Go to parent directory
         if( !nav_dir_gotoparent() )
            goto nav_setcwd_fail;
            sz_path  += 2; // jump ".."
            if( 0 != sz_path [0])
               sz_path  +=1; // jump "/"
         }
      }
   }

   // Reset list to start the search at the beginning
   if( !nav_filelist_reset())
      goto nav_setcwd_fail;

   while( 1 )
   {
      if( (( Is_unicode) && ( 0 == ((FS_STR_UNICODE)sz_path )[0] ) )
      ||  ((!Is_unicode) && ( 0 == sz_path [0] ) ) )
      {
         return TRUE;   // path (without file) is found or create
      }
      if( !nav_filelist_findname( sz_path  , b_match_case  ))
      {
         // The file or directory is not found
         if( !b_create )
            goto nav_setcwd_fail;   // don't creat the directory then exit
         // Set flag to create the directory
         b_create_name = TRUE;
         sz_save_path = sz_path;
      }
      
      while( 1 )
      {
         sz_path  += (Is_unicode? 2 : 1 );
         if( (( Is_unicode) && ( 0 == ((FS_STR_UNICODE)sz_path )[0] ) )
         ||  ((!Is_unicode) && ( 0 == sz_path [0] ) ) )
         {
            // Is it the last name of path and it is a file
            if( b_create_name )
            {
               // The file must be create
               nav_file_create( sz_save_path );
            }
            break;   // The file include in path is found or created, then end of set_cwd
         }

         if( (( Is_unicode) && (('\\' == ((FS_STR_UNICODE)sz_path )[0] ) || ('/' == ((FS_STR_UNICODE)sz_path )[0] )) )
         ||  ((!Is_unicode) && (('\\' == sz_path [0] ) || ('/' == sz_path [0] )) ) )
         {
            // Is it a folder name
            if( b_create_name )
            {
               // The folder don't exist and it must be created
               nav_dir_make( sz_save_path );
            }
            if( !fat_entry_is_dir() )
               goto nav_setcwd_fail;
            // jump '\'
            sz_path  += (Is_unicode? 2 : 1 );
            if( !nav_dir_cd())
               goto nav_setcwd_fail;
            break;
         }
      }
      
   }
      
nav_setcwd_fail:
   nav_gotoindex( &index );   // Restore the position
   return FALSE;
}




//**********************************************************************
//*********************** File control functions ***********************


//! This function returns the name of selected file
//!
//! @param     sz_name        string to store the name file (ASCII or UNICODE ) <br>
//! @param     u8_size_max    string size (unit ASCII or UNICODE )
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
Bool nav_file_getname( FS_STRING sz_name, U8 u8_size_max )
{
   return nav_file_name( sz_name, u8_size_max, FS_NAME_GET, FALSE );
}


//! This function returns the name of selected file or checks the string with the name of selected file 
//!
//! @param     b_mode         action mode: <br>
//!                           FS_NAME_GET    to get the name of selected file <br>
//!                           FS_NAME_CHECK  to check the name of selected file <br>
//! @param     sz_name        if FS_NAME_GET    then string to store the file name (ASCII or UNICODE ) <br>
//!                           if FS_NAME_CHECK  then string to match with file name (ASCII or UNICODE),
//!                                             it must be terminated by NULL or '*' value <br>
//! @param     b_match_case   FALSE, ignore the case (only used in "FS_NAME_CHECK" action)
//!
//! @param     u8_size_max    string size (unit ASCII or UNICODE ), only used in "FS_NAME_GET" action
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
Bool  nav_file_name( FS_STRING sz_name , U8 u8_size_max , Bool b_mode , Bool b_match_case  )
{
   _MEM_TYPE_SLOW_   U16 u16_lgt;   // Only used if LENGTH string mode enabled
   U16  u16_ptr_save_entry;
   Bool  b_readshortname = FALSE;

   if ( !fat_check_mount_select())
      return FALSE;

   // Check if the string size is not 0
   if( (FS_NAME_GET == b_mode)
   &&  (0 == u8_size_max) )
   {
      return TRUE;
   }

   // Save the current entry position
   u16_ptr_save_entry = fs_g_nav_fast.u16_entry_pos_sel_file;
   // if it is the beginning of the directory
   if ( 0 == fs_g_nav_fast.u16_entry_pos_sel_file )
   {
      b_readshortname = TRUE;                   // It isn't possibled to have a long name
   }
   else
   {
      fs_g_nav_fast.u16_entry_pos_sel_file--;   // Initialize entry position to search the first long name entry
   }

   // Loop in directory entry
   u16_lgt = 0;
   while( 1 )
   {
      if ( !fat_read_dir())
         break; // error

      if ( b_readshortname )
      { 
         // No long name present then read short name
         return fat_entry_shortname( sz_name , u8_size_max , b_mode  );
      }

      // Check or read the part of long file name in this entry
      if ( fat_entry_longname( sz_name , u8_size_max , b_mode , b_match_case  ))
      {
         if( g_b_string_length )
         {
            ((FS_STR_UNICODE)sz_name )[0] += u16_lgt;
         }
         fs_g_nav_fast.u16_entry_pos_sel_file = u16_ptr_save_entry;
         return TRUE;
      }
      
      if ( FS_NO_LAST_LFN_ENTRY != fs_g_status )
      {
         // Go to the main entry file (=short name entry) 
         fs_g_nav_fast.u16_entry_pos_sel_file = u16_ptr_save_entry;

         if ( FS_ERR_ENTRY_BAD == fs_g_status )
         {
            // It isn't a long name entry then there aren't long file name
            b_readshortname = TRUE;   // It is mandatory to use the short name
            continue;                 // restart the loop
         }
         // here, it is a error system or the string don't match with the file name 
         break;
      }
      if( g_b_string_length )
      {
         u16_lgt += FS_SIZE_LFN_ENTRY;
      }
      else
      {
         // Increment the string to store the next part of file name
         sz_name += FS_SIZE_LFN_ENTRY * (Is_unicode? 2 : 1 );
         u8_size_max -= FS_SIZE_LFN_ENTRY;
      }
      fs_g_nav_fast.u16_entry_pos_sel_file--;   // Go to the next part of long file name

   }  // end of loop while(1)
   return FALSE;
}


//! This function returns the size of selected file (unit byte)
//!
//! @return    Size of selected file (unit byte)
//!
U32   nav_file_lgt( void )
{
   return fs_g_nav_entry.u32_size;
}


//! This function returns the size of selected file (unit sector)
//!
//! @return    Size of selected file (unit 512B)
//!
U16   nav_file_lgtsector( void )
{
   return (fs_g_nav_entry.u32_size >> FS_512B_SHIFT_BIT);
}


//! This function checks the write protection of disk and the attribut "read only" of selected file
//!
//! @return    FALSE, it is possible to modify the selected file
//! @return    TRUE, in other case
//!
Bool  nav_file_isreadonly( void )
{
   if( !fat_check_mount_select() )
      return TRUE;   // No file selected
   if( mem_wr_protect( fs_g_nav.u8_lun ) )
      return TRUE;   // Disk protected
   return (0!=(FS_ATTR_READ_ONLY & fs_g_nav_entry.u8_attr));   // Check attribut "read only"
}


//! This function returns the type of selected file
//!
//! @return    TRUE, it is a directory
//! @return    FALSE, in other case
//!
Bool  nav_file_isdir( void )
{
   return fat_entry_is_dir();
}


//! This function checks the extension of selected file
//!
//! @param     sz_filterext   extension filter (ASCII format, e.g.: "txt" or "txt,d*,wk" )
//!
//! @return    TRUE, the file extension match with extension filter
//! @return    FALSE, in other case
//!
Bool  nav_file_checkext( const FS_STRING sz_filterext )
{
   if ( fat_check_mount_select() )
   {
      // Read selected entry (=short name entry) in directory
      if ( fat_read_dir())
      {
         // Check the extension with filter
         if ( fat_entry_checkext( (FS_STRING) sz_filterext ) )
            return TRUE;
      }
   }
   return FALSE;
}


//! This function returns the date of selected file
//!
//! @param     type_date      FS_DATE_LAST_WRITE,  to get the date of last write access <br>
//!                           FS_DATE_CREATION,    to get the date of file creation
//! @param     sz_date        ASCCI string (>17B) to store the information about date <br>
//!                           "YYYYMMDDHHMMSSMS" = year, month, day, hour, minute, seconde, miliseconde
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_file_dateget( FS_STRING sz_date , Bool type_date )
{
   if ( !fat_check_mount_select())
      return FALSE;

   // Read selected entry in directory
   if ( !fat_read_dir())
      return FALSE;
   fat_get_date( sz_date , type_date );
   return TRUE;
}


//! This function returns the attribut of selected file
//!
//! @return    attribut of selected file, see masks "FS_ATTR_" in fs_com.h file.
//!
U8    nav_file_attributget( void )
{
   return fs_g_nav_entry.u8_attr;
}



#if (FSFEATURE_WRITE_COMPLET == (FS_LEVEL_FEATURES & FSFEATURE_WRITE_COMPLET))
//! This function changes the date of selected file
//!
//! @param     type_date      FS_DATE_LAST_WRITE,  to get the date of last write access <br>
//!                           FS_DATE_CREATION,    to get the date of file creation
//! @param     sz_date        ASCCI string contains the date to write<br>
//!                           "YYYYMMDDHHMMSSMS" = year, month, day, hour, minute, seconde, miliseconde
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_file_dateset( const FS_STRING sz_date , Bool type_date )
{
   if ( !fat_check_mount_select())
      return TRUE;

   // Read selected entry in directory
   if ( !fat_read_dir())
      return FALSE;
   fat_set_date( sz_date , type_date );
   return fat_cache_flush();  // To write all data and check write access before exit function
}
#endif  // FS_LEVEL_FEATURES


#if (FSFEATURE_WRITE_COMPLET == (FS_LEVEL_FEATURES & FSFEATURE_WRITE_COMPLET))
//! This function changes the attribut of selected file
//!
//! @param   u8_attribut   value to write on selected file, see masks "FS_ATTR_" in fs_com.h file.
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
Bool  nav_file_attributset( U8 u8_attribut )
{
   if ( !fat_check_mount_select())
      return FALSE;

   // Read selected entry in directory
   if ( !fat_read_dir())
      return FALSE;

   // Write the new attribut
   fs_g_nav_entry.u8_attr &= (~(FS_ATTR_READ_ONLY|FS_ATTR_HIDDEN|FS_ATTR_SYSTEM|FS_ATTR_ARCHIVE));
   fs_g_nav_entry.u8_attr |= u8_attribut & (FS_ATTR_READ_ONLY|FS_ATTR_HIDDEN|FS_ATTR_SYSTEM|FS_ATTR_ARCHIVE);
   fat_write_entry_file();
   return fat_cache_flush();  // To write all data and check write access before exit function
}
#endif  // FS_LEVEL_FEATURES


#if (FSFEATURE_WRITE == (FS_LEVEL_FEATURES & FSFEATURE_WRITE))
//! This function deletes the selected file or directory
//!
//! @param     b_only_empty      TRUE, delete the directory only if empty <br>
//!                              FALSE, delete directories and files include in selected directory <br>
//!                              If the selection is not a directory then this param is ignored.
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_file_del( Bool b_only_empty )
{
   U8 u8_folder_level = 0xFF;

   if ( !fat_check_mount_select_noopen())
      return FALSE;

   if( 0xFF == u8_folder_level )  // to remove a eventually compile warning
     goto nav_file_del_test_dir_or_file;
     
   // loop to scan and delete ALL folders and ALL files
   while(1)
   {
      while(1)
      {
         if( nav_filelist_set( 0 , FS_FIND_NEXT ) )
         {
            // Directory no empty
            if( b_only_empty )
            {
               fs_g_status = FS_ERR_DIR_NOT_EMPTY;      // Erase only the empty directory
               return FALSE;
            }
            break; // Exit loop to delete files or directories present
         }
         // HERE, directory empty
         
         // Go to parent directory and this one select the children directory
         if( !nav_dir_gotoparent() )
            return FALSE;
         
         // Delete children directory name and her cluster list
         if ( !fat_delete_file( TRUE ))
            return FALSE;

         if( 0 == u8_folder_level )
         {
            // All directory tree is deleted
            return TRUE; //********* END OF DEL TREE **************
         }
         u8_folder_level--;

      } // end of second while (1)
      
nav_file_del_test_dir_or_file:
      if( nav_file_isdir())
      {
         // here, a directory is found and is selected
         if( !nav_dir_cd())
            return FALSE;
         u8_folder_level++;
      }
      else
      {
         // here, a file is found and is selected
         if( !fat_check_nav_access_file( TRUE ) )
            return FALSE;
         // delete file entry name and cluster list
         if ( !fat_delete_file( TRUE ))
            return FALSE;
         if( 0xFF == u8_folder_level )
            break;   // only one file to delete
      } // if dir OR file
   } // end of first while(1)
   
   // Reset selection
   nav_filelist_reset();
   return fat_cache_flush();  // To write all data and check write access before exit function
}
#endif  // FS_LEVEL_FEATURES


#if (FSFEATURE_WRITE_COMPLET == (FS_LEVEL_FEATURES & FSFEATURE_WRITE_COMPLET))
//! This function renames the selected directory or file
//!
//! @param     sz_name     new name (ASCII or UNICODE )
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  nav_file_rename( const FS_STRING sz_name  )
{
   U16 u16_save_entry_pos;
   Bool b_save_entry_type;
   U8 u8_attr;
   U32 u32_cluster;
   U32 u32_size;

   if ( !fat_check_mount_select_noopen())
      return FALSE;

   if( !fat_check_nav_access_file( TRUE ) )
      return FALSE;
      
   // Note: in case of error, create the new name before delete the current name
   
   // Save information about current name poisition
   u16_save_entry_pos = fs_g_nav_fast.u16_entry_pos_sel_file;
   b_save_entry_type  = fs_g_nav.b_mode_nav;
   // Save information about file
   u8_attr           = fs_g_nav_entry.u8_attr;
   u32_cluster       = fs_g_nav_entry.u32_cluster;
   u32_size          = fs_g_nav_entry.u32_size;
      
   // Create a name
   if ( !nav_file_create( sz_name  ))
      return FALSE; // error   
   // Restore information about file or directory on the new name entry
   if ( !fat_read_dir())
      return FALSE;
   fs_g_nav_entry.u8_attr = u8_attr;
   fs_g_nav_entry.u32_cluster = u32_cluster;
   fs_g_nav_entry.u32_size = u32_size;
   fat_write_entry_file();
   
   // Delete old entry name
   fs_g_nav_fast.u16_entry_pos_sel_file = u16_save_entry_pos; // go to old entry name
   if ( !fat_delete_file(FALSE) )
      return FALSE;
   if ( !fat_cache_flush() )
      return FALSE;

   // Go to at the position of the new name entry (it is the last file or directory )
   return nav_filelist_last( b_save_entry_type );
}
#endif  // FS_LEVEL_FEATURES

#if (FSFEATURE_WRITE == (FS_LEVEL_FEATURES & FSFEATURE_WRITE))

//! This function creates a file with NULL size and NULL attribut
//!
//! @param     sz_name     file name to create (ASCII or UNICODE )
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! If you ues this routine to create a file, then you must called file_open() to open this new file
//! @endverbatim
//!
Bool  nav_file_create( const FS_STRING sz_name  )
{
   // Check if the name already exists
   if (!nav_filelist_reset())
      return FALSE;
   if (nav_filelist_findname(sz_name , FALSE))
   {
      fs_g_status = FS_ERR_FILE_EXIST;
      return FALSE;  // File exist -> it is not possible to create this name
   }
   // FYC: here, the selection is at the end of the list
   // Create name entrys
   if ( !fat_create_entry_file_name( sz_name ))
      return FALSE; // error
   // By default the information about the new file is NULL
   fs_g_nav_entry.u32_cluster = 0;     // No first cluster
   fs_g_nav_entry.u32_size    = 0;     // The size is null
   fs_g_nav_entry.u8_attr     = 0;     // Attribut is a file

   // It is the last FILE of the list
   fs_g_nav.u16_pos_sel_file++;
   fs_g_nav.b_mode_nav = FS_FILE;
   return fat_cache_flush();
}


//! This function updates the COPY navigator with the selected file
//!
//! @return  FALSE in case of error, see global value "fs_g_status" for more detail
//! @return  TRUE otherwise
//!
//! @verbatim
//! If you use the COPY navigator after this routine then the copy information is lost (see FS_NAV_ID_COPYFILE in conf_explorer.h).
//! @endverbatim
//!
Bool  nav_file_copy( void )
{
   if( nav_file_isdir() )
   {
      fs_g_status = FS_ERR_COPY_DIR; // Impossible to copy a directory
      return FALSE;
   }
   // In "copy file" navigator select the file
   nav_copy( FS_NAV_ID_COPYFILE );
   return TRUE;
}


//! This function pastes the selected file in COPY navigator in the file list of the current navigator
//!
//! @param     sz_name     file name of the new file (ASCII or UNICODE )
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! After this routine, you shall called nav_file_paste_state() to run and way the copy
//! @endverbatim
//!
Bool  nav_file_paste_start( const FS_STRING sz_name  )
{
   U8 nav_id_save;
   Bool status;

   if( ID_STREAM_ERR != g_id_trans_memtomem )
   {
     
      fs_g_status = FS_ERR_COPY_RUNNING;  // A copy action is always running
      return FALSE;
   }

   // Create the file to paste
   if( !nav_file_create( sz_name  ) )
      return FALSE;
   // Open the file in write mode with size 0
   if( !file_open( FOPEN_MODE_W_PLUS ))
      return FALSE;

   // Open the file to copy
   nav_id_save = nav_get();      
   nav_select( FS_NAV_ID_COPYFILE );
   status = file_open(FOPEN_MODE_R);
   nav_select( nav_id_save );

   // If error then close "paste file"
   if( !status )
   {
      file_close();
      return FALSE;
   }else{
      // Signal start copy
      g_id_trans_memtomem = ID_STREAM_ERR-1;
      return TRUE;
   }
}


//! This function executes the copy file
//!
//! @param     b_stop      set TRUE to stop copy action
//!
//! @return    copy status <br>
//!            COPY_BUSY,     copy running
//!            COPY_FAIL,     copy fail
//!            COPY_FINISH,   copy finish
//!
U8    nav_file_paste_state( Bool b_stop )
{
	/*
   _MEM_TYPE_SLOW_ Fs_file_segment segment_src;
   _MEM_TYPE_SLOW_ Fs_file_segment segment_dest;

   Ctrl_status status_stream;
   U8 status_copy;
   U8 nav_id_save;

   nav_id_save = nav_get();

   // Check, if the copy is running
   if( ID_STREAM_ERR == g_id_trans_memtomem )
      return COPY_FAIL;

   if( b_stop )
   {
      status_copy = COPY_FAIL;
   }
   else
   {
      if( (ID_STREAM_ERR-1) != g_id_trans_memtomem )
      {
         // It isn't the beginning of copy action, then check current stream
         status_stream = stream_state( g_id_trans_memtomem );
         switch( status_stream )  
         {
         case CTRL_BUSY:
            status_copy = COPY_BUSY;
            break;
         case CTRL_GOOD:
            status_copy = COPY_FINISH;
            break;
         case CTRL_FAIL:
         default:
            status_copy = COPY_FAIL;
            break;
         }
      }else{
         status_copy = COPY_FINISH;
      }
   
      // Compute the new segment to copy
      if( COPY_FINISH == status_copy )
      {
         stream_stop( g_id_trans_memtomem );

         // Check eof source file
         nav_select( FS_NAV_ID_COPYFILE );
         if( 0 == file_eof() )
         {
            status_copy = COPY_BUSY;
            segment_src.u16_size = 0xFFFF; // Get the maximum segment supported by navigation (U16)
            if( !file_read( &segment_src ))
            {
               status_copy = COPY_FAIL;
            }
         }
         nav_select( nav_id_save );
   
         // Check destination file
         if( COPY_BUSY == status_copy )
         {
            segment_dest.u16_size = segment_src.u16_size; // Ask the segment no more larger than source segment
            if( !file_write( &segment_dest ))
            {
               status_copy = COPY_FAIL;
            }
         }

         // Start new segment copy
         if( COPY_BUSY == status_copy )
         {
            // Compute a minimal segment
            if( segment_src.u16_size > segment_dest.u16_size )
            {
               // Reposition source file
               nav_select( FS_NAV_ID_COPYFILE );
               if( !file_seek( (U32)(segment_src.u16_size - segment_dest.u16_size)*FS_512B , FS_SEEK_CUR_RE ))
               {
                  status_copy = COPY_FAIL;
               }
               nav_select( nav_id_save );
               segment_src.u16_size = segment_dest.u16_size; // Update source to start a correct transfer
            }
         }
         if( COPY_BUSY == status_copy )
         {
            g_id_trans_memtomem = stream_mem_to_mem( segment_src.u8_lun , segment_src.u32_addr , segment_dest.u8_lun , segment_dest.u32_addr , segment_src.u16_size );               
            if( ID_STREAM_ERR == g_id_trans_memtomem )
                  status_copy = COPY_FAIL;
         }
      }
   }

   // Check end of copy
   if( COPY_BUSY != status_copy )
   {
      U32 u32_size_exact;

      // Stop copy
      stream_stop( g_id_trans_memtomem );
      g_id_trans_memtomem = ID_STREAM_ERR;

      // Get exact size and close the source file
      nav_select( FS_NAV_ID_COPYFILE );
      u32_size_exact = nav_file_lgt();
      file_close();
      nav_select( nav_id_save );

      // If no error then set the exact size on the destination file
      if( COPY_FINISH == status_copy )
      {
         if( !file_seek( u32_size_exact , FS_SEEK_SET ))
         {
            status_copy = COPY_FAIL;
         }else{
            if( !file_set_eof() )
            {
               status_copy = COPY_FAIL;
            }
         }
      }
      file_close();
      // If error then delete the destination file
      if( COPY_FAIL == status_copy )
      {
         nav_file_del( TRUE );
      }
   }
   return status_copy;
  */
	return COPY_FAIL;
}
#endif  // FS_LEVEL_FEATURES
