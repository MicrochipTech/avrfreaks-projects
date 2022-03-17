/* This header file is part of the ATMEL AVR32-SoftwareFramework-1.2.1ES-AT32UC3A Release */

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

#ifndef _FILE_H_
#define _FILE_H_

#include "fs_com.h"
#include "fat.h"

//_____ D E F I N I T I O N S ______________________________________________

//! \name file_seek() options parameter
//! @{
#define  FS_SEEK_SET       0x00  //!< start at the beginning
#define  FS_SEEK_END       0x01  //!< start at the end of file and rewind
#define  FS_SEEK_CUR_RE    0x02  //!< start at the current position and rewind
#define  FS_SEEK_CUR_FW    0x03  //!< start at the current position and foward
//! @}

//! \name Struture to define a physical memory segment
//! @{
typedef struct {
   U8   u8_lun;            //!< logical unit(drive) number
   U32  u32_addr;          //!< segment address (unit sector)
   U16  u16_size;          //!< segment size (unit sector), or IN parameter for file_write() and file_read() routines
} Fs_file_segment;
//! @}


//_____ D E C L A R A T I O N S ____________________________________________

//! This fonction check if a file is selected and it is not a directory
//!
//! @return    TRUE, a file is selected
//! @return    FALSE, otherwise
//!
Bool  file_ispresent( void );

//! This fonction open the file selected
//!
//! @param     fopen_mode  option to open file : <br>
//!                        FOPEN_MODE_R         R   access, flux pointer = 0, size not modify <br>
//!                        FOPEN_MODE_R_PLUS    R/W access, flux pointer = 0, size not modify <br>
//!                        FOPEN_MODE_W         W   access, flux pointer = 0, size = 0 <br>
//!                        FOPEN_MODE_W_PLUS    R/W access, flux pointer = 0, size = 0 <br>
//!                        FOPEN_MODE_APPEND    W   access, flux pointer = at the end, size not modify <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  file_open( U8 fopen_mode );

//! This fonction return a continue physical memory segment corresponding at a file to read
//!
//! @param     segment  Pointer on the segment structure: <br>
//!                     ->u32_size_or_pos    IN,   shall containt the maximum number of sector to read in file (0 = unlimited)
//!                     ->u32_size_or_pos    OUT,  containt the segment size (unit sector)
//!                     ->other              IN,   ignored
//!                     ->other              OUT,  containt the segment position
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! This routine is interesting to read a file via a DMA and avoid the file system decode
//! because this routine return a physical memory segment without File System data.
//! Note: the file can be fragmented and it can be necessary to call several time file_read().
//! @endverbatim
//!
Bool  file_read( Fs_file_segment _MEM_TYPE_SLOW_ *segment );

//! This fonction copy in data buffer the data file corresponding at the current position
//!
//! @param     buffer         buffer to fill
//! @param     u16_buf_size   buffer size
//!
//! @return    number of byte read
//! @return    0, in case of error
//!
U16   file_read_buf( U8 _MEM_TYPE_SLOW_ *buffer , U16 u16_buf_size );

//! This fonction return the next character at current file position
//!
//! @return    The byte reading
//! @return    EOF, in case of error or end of file
//!
U16   file_getc( void );

//! This fonction return a continue physical memory segment corresponding at a file to write
//!
//! @param     segment  Pointer on the segment structure: <br>
//!                     ->u32_size_or_pos    IN,   shall containt the maximum number of sector to write in file (0 = maximum)
//!                     ->u32_size_or_pos    OUT,  containt the segment size (unit sector)
//!                     ->other              IN,   ignored
//!                     ->other              OUT,  containt the segment position
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! This routine is interesting to write a file via a DMA and avoid the file system decode
//! because this routine return a physical memory segment without File System data.
//! Note: the file can be fragmented and it can be necessary to call several time file_write().
//! @endverbatim
//!
Bool  file_write( Fs_file_segment _MEM_TYPE_SLOW_ *segment );

//! This fonction set the end of file at the current position
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
//! @verbatim
//! This routine is use after the last file_write() call.
//! The file_write() routine work on sector (512B) unit,
//! and you can set a Byte size with file_seek() and this routine.
//! @endverbatim
//!
Bool  file_set_eof( void );

//! This fonction copy the data buffer in file at the current position
//!
//! @param     buffer         data buffer
//! @param     u16_buf_size   data size
//!
//! @return    number of byte write
//! @return    0, in case of error
//!
U16   file_write_buf( U8 _MEM_TYPE_SLOW_ *buffer , U16 u16_buf_size );

//! This fonction write a byte in the file at current position
//!
//! @param     u8_byte        byte to write in the file
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  file_putc( U8 u8_byte );

//! This fonction return the position in the file
//!
//! @return    Position in file
//!
U32   file_getpos( void );

//! This fonction change the position in file selected
//!
//! @param     u32_pos     number of byte to seek
//! @param     u8_whence   direction of seek <br>
//!                        FS_SEEK_SET   , start at the beginning and foward <br>
//!                        FS_SEEK_END   , start at the end of file and rewing <br>
//!                        FS_SEEK_CUR_RE, start at the current position and rewing <br>
//!                        FS_SEEK_CUR_FW, start at the current position and foward <br>
//!
//! @return    FALSE in case of error, see global value "fs_g_status" for more detail
//! @return    TRUE otherwise
//!
Bool  file_seek( U32 u32_pos , U8 u8_whence );

//! This fonction test if the position is at the beginning of file 
//!
//! @return    1     the position is at the beginning of file
//! @return    0     the position isn't at the beginning of file
//! @return    FFh   error
//!
U8    file_bof( void );

//! This fonction test if the position is at the end of file
//!
//! @return    1     the position is at the end of file
//! @return    0     the position isn't at the end of file
//! @return    FFh   error
//!
U8    file_eof( void );

//! This fonction flush the data include in file and information about file
//!
void  file_flush( void );

//! This fonction close the file
//!
void  file_close( void );


#endif  // _FILE_H_
