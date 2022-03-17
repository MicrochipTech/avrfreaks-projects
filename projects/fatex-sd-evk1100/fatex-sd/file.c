/* This source file is part of the ATMEL AVR32-SoftwareFramework-1.2.1ES-AT32UC3A Release */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief FAT 12/16/32 Services.
 *
 * This file defines a useful set of functions for the file accesses on
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


//_____  I N C L U D E S ___________________________________________________
#include "conf_explorer.h"
#include "file.h"
#include "navigation.h"
#include LIB_MEM
#include LIB_CTRLACCESS


//_____ D E C L A R A T I O N S ____________________________________________

//! Use "FAT sector cache" to store a sector from a file (see file_putc(), file_getc(), file_read_buf(), file_write_buf())
extern   _MEM_TYPE_SLOW_   U8    fs_g_sector[ FS_CACHE_SIZE ];

static   void  file_load_segment_value( Fs_file_segment _MEM_TYPE_SLOW_ *segment );



//! This function checks if a file is selected
//!
//! @return    TRUE, a file is selected
//! @return    FALSE, otherwise
//!
Bool  file_ispresent( void )
{
   if( !fat_check_mount_select() )
      return FALSE;
   return fat_check_is_file();
}


//! This function opens the selected file
//!
//! @param     fopen_mode  option to open the file : <br>
//!                        FOPEN_MODE_R         R   access, flux pointer = 0, size not modify <br>
//!                        FOPEN_MODE_R_PLUS    R/W access, flux pointer = 0, size not modify <br>
//!                        FOPEN_MODE_W         W   access, flux pointer = 0, size = 0 <br>
//!                        FOPEN_MODE_W_PLUS    R/W access, flux pointer = 0, size = 0 <br>
//!                        FOPEN_MODE_APPEND    W   access, flux pointer = at the end, size not modify <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  file_open( U8 fopen_mode )
{
   if( !fat_check_mount_select_noopen())
      return FALSE;

   if( !fat_check_is_file())
      return FALSE;

   if(FOPEN_WRITE_ACCESS & fopen_mode)
   {
      if( !fat_check_nav_access_file( TRUE ) )
         return FALSE;
#if (FSFEATURE_WRITE == (FS_LEVEL_FEATURES & FSFEATURE_WRITE))
      if (FS_ATTR_READ_ONLY & fs_g_nav_entry.u8_attr)
      {
         fs_g_status = FS_ERR_READ_ONLY;  // File is read only
         return FALSE;
      }
      if( mem_wr_protect( fs_g_nav.u8_lun  ))
      {
         fs_g_status = FS_LUN_WP;  // Disk read only
         return FALSE;
      }
#else
      fs_g_status = FS_ERR_MODE_NOAVIALABLE;
      return FALSE;
#endif  // FS_LEVEL_FEATURES
   }
   else
   {
      if( !fat_check_nav_access_file( FALSE ) )
         return FALSE;
   }

   if(FOPEN_CLEAR_SIZE & fopen_mode)
   {
      fs_g_nav_entry.u32_size    = 0;     // The size is null
   }
   if(FOPEN_CLEAR_PTR & fopen_mode)
   {
      fs_g_nav_entry.u32_pos_in_file = 0; 
   }
   else
   {  // Go to at the end of file
      fs_g_nav_entry.u32_pos_in_file = fs_g_nav_entry.u32_size;
   }
   fs_g_nav_entry.u8_open_mode = fopen_mode;
   return TRUE;
}


//! This function stores the global segment variable in other variable
//!
//! @param     segment  Pointer on the variable to fill
//!
static void file_load_segment_value( Fs_file_segment _MEM_TYPE_SLOW_ *segment )
{
   segment->u8_lun = fs_g_nav.u8_lun;
   segment->u32_addr = fs_g_seg.u32_addr;
   segment->u16_size = fs_g_seg.u32_size_or_pos;
}


//! This function returns a segment (position & size) in a physical memory corresponding at the file
//!
//! @param     segment  Pointer on the segment structure: <br>
//!                     ->u32_size_or_pos    IN,   shall contains maximum number of sector to read in file (0 = unlimited) <br>
//!                     ->u32_size_or_pos    OUT,  containt the segment size (unit sector) <br>
//!                     ->other              IN,   ignored <br>
//!                     ->other              OUT,  contains the segment position <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! This routine is interesting to read a file via a DMA and avoid the file system decode
//! because this routine returns a physical memory segment without File System information.
//! Note: the file can be fragmented and you must call file_read() for each fragments.
//! @endverbatim
//!
Bool  file_read( Fs_file_segment _MEM_TYPE_SLOW_ *segment )
{
   U8 u8_nb_sector_truncated;

   if( !fat_check_mount_select_open())
      return FALSE;

   if(!(FOPEN_READ_ACCESS & fs_g_nav_entry.u8_open_mode))
   {
      fs_g_status = FS_ERR_WRITE_ONLY;
      return FALSE;
   }
   
   if ( file_eof() )
   {
      // End of the file
      fs_g_status = FS_ERR_EOF;
      return FALSE;
   }

   if( !fat_read_file(FS_CLUST_ACT_SEG))
   {
      if( FS_ERR_OUT_LIST == fs_g_status )
         fs_g_status = FS_ERR_EOF;  // translate the error
      return FALSE;
   }
   // If the segment is too large then truncate it
   if( (segment->u16_size != 0)                          // if no limit then no truncate
   &&  (segment->u16_size < fs_g_seg.u32_size_or_pos) )
   {
      u8_nb_sector_truncated   = fs_g_seg.u32_size_or_pos - segment->u16_size;
      fs_g_seg.u32_size_or_pos = segment->u16_size ;
   }else{
      u8_nb_sector_truncated = 0;
   }

   // Update file position
   fs_g_nav_entry.u32_pos_in_file += (U32)fs_g_seg.u32_size_or_pos * FS_512B;
   if( fs_g_nav_entry.u32_size < fs_g_nav_entry.u32_pos_in_file )
   {  
      // The segment is more larger then file
      // case possible: if the file don't use all cluster space
      // then compute sectors not used in last cluster of file cluster list
      U8 u8_nb_sector_not_used;
      
      // Compute the number of sector used in last cluster
      // remark: also the two first bytes of size is used, because the cluster size can't be more larger then 64KB
      u8_nb_sector_not_used = LSB1( fs_g_nav_entry.u32_size ) >> (FS_512B_SHIFT_BIT-8);
      if( 0 != (fs_g_nav_entry.u32_size & FS_512B_MASK) )
      {  // last sector of file isn't full, but it must been read
         u8_nb_sector_not_used++;
      }

      // Compute the number of sector not used in last cluster
      u8_nb_sector_not_used = fs_g_nav.u8_BPB_SecPerClus - (u8_nb_sector_not_used % fs_g_nav.u8_BPB_SecPerClus);
      // if all space of cluster isn't used, then it is wrong
      if( u8_nb_sector_not_used == fs_g_nav.u8_BPB_SecPerClus )
         u8_nb_sector_not_used = 0; // The file uses all last cluster space

      // Subtract this value a the file position and segment size
      u8_nb_sector_not_used -= u8_nb_sector_truncated;
      fs_g_seg.u32_size_or_pos -= u8_nb_sector_not_used;                                     // unit sector
      fs_g_nav_entry.u32_pos_in_file -= ((U16)u8_nb_sector_not_used) << FS_512B_SHIFT_BIT;   // unit byte
   }
   file_load_segment_value( segment );
   return TRUE;
}


//! This function copys in a buffer the file data corresponding at the current position
//!
//! @param     buffer         buffer to fill
//! @param     u16_buf_size   buffer size
//!
//! @return    number of byte read
//! @return    0, in case of error
//!
U16   file_read_buf( U8 _MEM_TYPE_SLOW_ *buffer , U16 u16_buf_size )
{
   _MEM_TYPE_FAST_ U16 u16_nb_read_tmp;
   _MEM_TYPE_FAST_ U16 u16_nb_read;
   _MEM_TYPE_FAST_ U16 u16_pos_in_sector;
   _MEM_TYPE_FAST_ U32 u32_byte_remaining;

   if( !fat_check_mount_select_open())
      return FALSE;

   if(!(FOPEN_READ_ACCESS & fs_g_nav_entry.u8_open_mode))
   {
      fs_g_status = FS_ERR_WRITE_ONLY;
      return FALSE;
   }
   
   u16_nb_read = 0;

   while( 0 != u16_buf_size )
   {
      if ( file_eof() )
      {
         fs_g_status = FS_ERR_EOF;
         return u16_nb_read;     // End of the file
      }
      u32_byte_remaining = fs_g_nav_entry.u32_size-fs_g_nav_entry.u32_pos_in_file;
      u16_pos_in_sector = fs_g_nav_entry.u32_pos_in_file % FS_512B;

      if( (0== u16_pos_in_sector)
      &&  (FS_512B <= u32_byte_remaining)
      &&  (FS_512B <= u16_buf_size))
      {
         // The file data sector can been directly transfer from memory to buffer (don't use internal cache)
         if( u16_buf_size <= u32_byte_remaining)
         {
            u16_nb_read_tmp = u16_buf_size;
         }else{
            u16_nb_read_tmp = u32_byte_remaining;
         }
         u16_nb_read_tmp = u16_nb_read_tmp / FS_512B;  // read a modulo sector size

         // Get following sector segment of file
         if( !fat_read_file(FS_CLUST_ACT_SEG))
         {
            if( FS_ERR_OUT_LIST == fs_g_status )
               fs_g_status = FS_ERR_EOF;  // translate the error
            return u16_nb_read;
         }
         // Truncate the segment size found if more larger than asked size
         if( u16_nb_read_tmp > fs_g_seg.u32_size_or_pos )
         {
            u16_nb_read_tmp = fs_g_seg.u32_size_or_pos;
         }else{
            fs_g_seg.u32_size_or_pos = u16_nb_read_tmp;
         }
         
         // Directly data tranfert from memory to buffer
         while( 0 != fs_g_seg.u32_size_or_pos )
         {
            if( CTRL_GOOD != memory_2_ram( fs_g_nav.u8_lun  , fs_g_seg.u32_addr, buffer))
            {
               fs_g_status = FS_ERR_HW;
               return u16_nb_read;
            }
            fs_g_seg.u32_size_or_pos--;
            fs_g_seg.u32_addr++;
            buffer += FS_512B;
         }
         // Translate from sector unit to byte unit
         u16_nb_read_tmp *= FS_512B;
      }
      else
      {
         // The file data can't been directly transfer from memory to buffer, the internal cache must be used
         
         // Tranfer data from memory to internal cache
         if( !fat_read_file( FS_CLUST_ACT_ONE ))
         {
            if( FS_ERR_OUT_LIST == fs_g_status )
            {  // Translate the error
               fs_g_status = FS_ERR_EOF;   // End of file
            }
            return u16_nb_read;
         }
         
         // Compute the number of data to transfer
         u16_nb_read_tmp = FS_512B - u16_pos_in_sector;  // The number is limited at sector size
         if( u16_nb_read_tmp > u32_byte_remaining )
            u16_nb_read_tmp = u32_byte_remaining;
         if( u16_nb_read_tmp > u16_buf_size )
            u16_nb_read_tmp = u16_buf_size;

         // Tranfer data from internal cache to buffer
         memcpy_ram2ram( buffer , &fs_g_sector[ u16_pos_in_sector ], u16_nb_read_tmp );
         buffer += u16_nb_read_tmp;
      }
      // Update positions
      fs_g_nav_entry.u32_pos_in_file   += u16_nb_read_tmp;
      u16_nb_read                      += u16_nb_read_tmp;
      u16_buf_size                     -= u16_nb_read_tmp;
   }
   return u16_nb_read;  // Buffer is full
}


//! This function returns the next byte of file
//!
//! @return    The byte readed
//! @return    EOF, in case of error or end of file
//!
U16   file_getc( void )
{
   U16   u16_byte;

   while(1)
   {
      if(!(FOPEN_READ_ACCESS & fs_g_nav_entry.u8_open_mode))
      {
         fs_g_status = FS_ERR_WRITE_ONLY;
         break;
      }
      if( fs_g_nav_entry.u32_size <= fs_g_nav_entry.u32_pos_in_file )
      {
         fs_g_status = FS_ERR_EOF;
         break;
      }
  
      if( !fat_read_file( FS_CLUST_ACT_ONE ))
      {
         if( FS_ERR_OUT_LIST == fs_g_status )
         {  // Translate the error
            fs_g_status = FS_ERR_EOF;   // End of file
         }
         break;
      }
      
      u16_byte = fs_g_sector[ fs_g_nav_entry.u32_pos_in_file & FS_512B_MASK ];
      fs_g_nav_entry.u32_pos_in_file++;
      return u16_byte;
   }
   return FS_EOF;   // No data readed
}


#if (FSFEATURE_WRITE == (FS_LEVEL_FEATURES & FSFEATURE_WRITE))
//! This function allocs and returns a segment (position & size) in a physical memory corresponding at the file
//!
//! @param     segment  Pointer on the segment structure: <br>
//!                     ->u32_size_or_pos    IN,   shall contains the maximum number of sector to write in file <br>
//!                     ->u32_size_or_pos    OUT,  contains the segment size (unit sector) <br>
//!                     ->other              IN,   ignored <br>
//!                     ->other              OUT,  contains the segment position <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! This routine is interesting to write a file via a DMA and avoid the file system decode
//! because this routine returns a physical memory segment without File System information.
//! Note: the file can be fragmented and you must call file_write() for each fragments.
//! @endverbatim
//!
Bool  file_write( Fs_file_segment _MEM_TYPE_SLOW_ *segment )
{
   if( !fat_check_mount_select_open())
      return FALSE;

   if(!(FOPEN_WRITE_ACCESS & fs_g_nav_entry.u8_open_mode))
   {
      fs_g_status = FS_ERR_READ_ONLY;
      return FALSE;
   }

   if( !fat_write_file( FS_CLUST_ACT_SEG , segment->u16_size ))
      return FALSE;

   // If the segment is too large then truncate it
   if( (segment->u16_size != 0)  // if not undefine limit
   &&  (segment->u16_size < fs_g_seg.u32_size_or_pos) )
   {
      fs_g_seg.u32_size_or_pos = segment->u16_size ;
   }
   
   // Update file position
   fs_g_nav_entry.u32_pos_in_file += ((U32)fs_g_seg.u32_size_or_pos * FS_512B);
   
   // Update size file
   if( fs_g_nav_entry.u32_pos_in_file > fs_g_nav_entry.u32_size )
   {
      fs_g_nav_entry.u32_size = fs_g_nav_entry.u32_pos_in_file;
   }
   file_load_segment_value( segment );
   return TRUE;
}


//! This function sets the end of file at the current position
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! This routine is usualy used after the last file_write() call.
//! The file_write() routine uses the sector unit (512B),
//! and you can set a specific byte size with a file_seek() call and fiel_set_eof() call.
//! @endverbatim
//!
Bool  file_set_eof( void )
{
   if( !fat_check_mount_select_open())
      return FALSE;

   if(!(FOPEN_WRITE_ACCESS & fs_g_nav_entry.u8_open_mode))
   {
      fs_g_status = FS_ERR_READ_ONLY;
      return FALSE;
   }

   // Update the file size
   fs_g_nav_entry.u32_size = fs_g_nav_entry.u32_pos_in_file;

   if( !fat_read_file( FS_CLUST_ACT_CLR ))
      return FALSE;

   return fat_cache_flush();
}


//! This function transfer a buffer to a file at the current file position
//!
//! @param     buffer         data buffer
//! @param     u16_buf_size   data size
//!
//! @return    number of byte write
//! @return    0, in case of error
//!
U16   file_write_buf( U8 _MEM_TYPE_SLOW_ *buffer , U16 u16_buf_size )
{
   _MEM_TYPE_FAST_ U16 u16_nb_write_tmp;
   _MEM_TYPE_FAST_ U16 u16_nb_write;
   _MEM_TYPE_FAST_ U16 u16_pos_in_sector;

   if( !fat_check_mount_select_open())
      return FALSE;

   if(!(FOPEN_WRITE_ACCESS & fs_g_nav_entry.u8_open_mode))
   {
      fs_g_status = FS_ERR_READ_ONLY;
      return FALSE;
   }

   u16_nb_write = 0;

   while( 0 != u16_buf_size )
   {
      // The file data sector can been directly transfer from buffer to memory (don't use internal cache)
      u16_pos_in_sector = fs_g_nav_entry.u32_pos_in_file % FS_512B;
      if( (0== u16_pos_in_sector)
      &&  (FS_512B <= u16_buf_size))
      {
         u16_nb_write_tmp = u16_buf_size / FS_512B;  // read a modulo sector size

         // Get and eventually alloc the following sector segment of file
         if( !fat_write_file( FS_CLUST_ACT_SEG , u16_nb_write_tmp ))
            return FALSE;
         // Truncate the segment found if more larger than asked size
         if( u16_nb_write_tmp < fs_g_seg.u32_size_or_pos)
         {
            fs_g_seg.u32_size_or_pos = u16_nb_write_tmp;
         }else{
            u16_nb_write_tmp = fs_g_seg.u32_size_or_pos;
         }
   
         // Directly data tranfert from buffer to memory
         while( 0 != fs_g_seg.u32_size_or_pos )
         {
            if( CTRL_GOOD != ram_2_memory( fs_g_nav.u8_lun  , fs_g_seg.u32_addr, buffer))
            {
               fs_g_status = FS_ERR_HW;
               return u16_nb_write;
            }
            fs_g_seg.u32_size_or_pos--;
            fs_g_seg.u32_addr++;
            buffer += FS_512B;
         }
         // Translate from sector unit to byte unit
         u16_nb_write_tmp *= FS_512B;
      }
      else
      {
         // The file data can't been directly transfer from buffer to memory, the internal cache must be used
         
         // Tranfer and eventually alloc a data sector from internal cache to memory
         if((fs_g_nav_entry.u32_pos_in_file == fs_g_nav_entry.u32_size)
         && (0==u16_pos_in_sector) )
         {
            // Eventually alloc one new sector for the file
            if( !fat_write_file( FS_CLUST_ACT_SEG  , 1 ))
               return FALSE;
            // Update the cache
            fs_gu32_addrsector = fs_g_seg.u32_addr;
            if( !fat_cache_read_sector( FALSE ))         // The memory is not readed because it is a new sector
               return FALSE;
         }else{
            // The sector must existed then alloc no necessary
            if( !fat_write_file( FS_CLUST_ACT_ONE  , 1 ))
               return FALSE;
         }

         // Flag internal cache modified
         fat_cache_sector_is_modify();
                
         // Compute the number of data to transfer
         u16_nb_write_tmp = FS_512B - u16_pos_in_sector; // The number is limited at sector size
         if( u16_nb_write_tmp > u16_buf_size )
            u16_nb_write_tmp = u16_buf_size;

         // Tranfer data from buffer to internal cache
         memcpy_ram2ram( &fs_g_sector[ u16_pos_in_sector ], buffer , u16_nb_write_tmp );
         buffer += u16_nb_write_tmp;
      }
      // Update positions
      fs_g_nav_entry.u32_pos_in_file+= u16_nb_write_tmp;
      u16_nb_write                  += u16_nb_write_tmp;
      u16_buf_size                  -= u16_nb_write_tmp;
      // Update file size
      if( fs_g_nav_entry.u32_pos_in_file > fs_g_nav_entry.u32_size )
      {
         fs_g_nav_entry.u32_size = fs_g_nav_entry.u32_pos_in_file;
      }
   }
   return u16_nb_write;  // All buffer is writed
}


//! This function writes a byte in the file
//!
//! @param     u8_byte    byte to write
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  file_putc( U8 u8_byte )
{
   if( !fat_check_mount_select_open())
      return FALSE;

   if(!(FOPEN_WRITE_ACCESS & fs_g_nav_entry.u8_open_mode))
   {
      fs_g_status = FS_ERR_READ_ONLY;
      return FALSE;
   }

   if( !fat_write_file( FS_CLUST_ACT_ONE  , 1 ))
      return FALSE;

   // Write the data in the internal cache
   fat_cache_sector_is_modify();
   fs_g_sector[ fs_g_nav_entry.u32_pos_in_file & FS_512B_MASK ]    = u8_byte;
   fs_g_nav_entry.u32_pos_in_file++;
   
   // Update the file size
   if( fs_g_nav_entry.u32_pos_in_file > fs_g_nav_entry.u32_size )
   {
      fs_g_nav_entry.u32_size = fs_g_nav_entry.u32_pos_in_file;
   }
   return TRUE;
}
#endif  // FS_LEVEL_FEATURES


//! This function returns the position in the file
//!
//! @return    Position in file
//!
U32   file_getpos( void )
{
   if( !fat_check_mount_select_open() )
      return 0;

   return fs_g_nav_entry.u32_pos_in_file;
}


//! This function changes the position in the file
//!
//! @param     u32_pos     number of byte to seek
//! @param     u8_whence   direction of seek <br>
//!                        FS_SEEK_SET   , start at the beginning and foward <br>
//!                        FS_SEEK_END   , start at the end of file and rewind <br>
//!                        FS_SEEK_CUR_RE, start at the current position and rewind <br>
//!                        FS_SEEK_CUR_FW, start at the current position and foward <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  file_seek( U32 u32_pos , U8 u8_whence )
{
   if( !fat_check_mount_select_open())
      return FALSE;

   switch(u8_whence)
   {
      case FS_SEEK_CUR_RE:
      if( fs_g_nav_entry.u32_pos_in_file < u32_pos )
      {  // Out of the limit
         fs_g_status = FS_ERR_BAD_POS;
         return FALSE;
      }
      // update the position
      fs_g_nav_entry.u32_pos_in_file -= u32_pos;
      break;

      case FS_SEEK_SET:
      if( fs_g_nav_entry.u32_size < u32_pos )
      {  // Out of the limit
         fs_g_status = FS_ERR_BAD_POS;
         return FALSE;
      }
      // update the position
      fs_g_nav_entry.u32_pos_in_file = u32_pos;
      break;

      case FS_SEEK_END:
      if( fs_g_nav_entry.u32_size < u32_pos )
      {  // Out of the limit
         fs_g_status = FS_ERR_BAD_POS;
         return FALSE;
      }
      // update the position
      fs_g_nav_entry.u32_pos_in_file = fs_g_nav_entry.u32_size - u32_pos;
      break;

      case FS_SEEK_CUR_FW:
      u32_pos += fs_g_nav_entry.u32_pos_in_file;
      if( fs_g_nav_entry.u32_size < u32_pos )
      {  // Out of the limit
         fs_g_status = FS_ERR_BAD_POS;
         return FALSE;
      }
      // update the position
      fs_g_nav_entry.u32_pos_in_file = u32_pos;
      break;
   }
   return TRUE;
}


//! This function checks the beginning of file 
//!
//! @return    1     the position is at the beginning of file
//! @return    0     the position isn't at the beginning of file
//! @return    FFh   error
//!
U8    file_bof( void )
{
   if( !fat_check_mount_select_open() )
      return 0xFF;

   return (0 == fs_g_nav_entry.u32_pos_in_file );
}


//! This function checks the end of file
//!
//! @return    1     the position is at the end of file
//! @return    0     the position isn't at the end of file
//! @return    FFh   error
//!
U8    file_eof( void )
{
   if( !fat_check_mount_select_open() )
      return 0xFF;
   return (fs_g_nav_entry.u32_size <= fs_g_nav_entry.u32_pos_in_file );
}


//! This function flushs the internal cache (file datas and file information)
//!
void  file_flush( void )
{
   U8 save_open_mode;
   save_open_mode = fs_g_nav_entry.u8_open_mode;
   file_close();
   fs_g_nav_entry.u8_open_mode = save_open_mode;
}


//! This function closes the file
//!
void  file_close( void )
{
   // If a file is opened, then close this one
   if( fat_check_mount_select_open() )
   {
      
#if (FSFEATURE_WRITE == (FS_LEVEL_FEATURES & FSFEATURE_WRITE))
      if( FOPEN_WRITE_ACCESS & fs_g_nav_entry.u8_open_mode )
      {
         // Write file information
         if( !fat_read_dir() )
            return;           // error
         fat_write_entry_file();
         fat_cache_flush();   // In case of error during writing data, flush the data before exit function
      }
#endif  // FS_LEVEL_FEATURES
      Fat_file_close;
   }
}

