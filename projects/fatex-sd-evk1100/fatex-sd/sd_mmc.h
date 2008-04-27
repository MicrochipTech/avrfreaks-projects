/* This header file is part of the ATMEL AVR32-SoftwareFramework-1.2.1ES-AT32UC3A Release */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief SD/MMC card driver using SPI interface .
 *
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with an SPI module can be used.
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


#ifndef _SD_MMC_H_
#define _SD_MMC_H_


/*_____ I N C L U D E S ____________________________________________________*/

#include "conf_access.h"

#if SD_MMC_MEM == DISABLE
  #error sd_mmc.h is #included although SD_MMC_MEM is disabled
#endif


#include "compiler.h"
#include "spi.h"


/*_____ M A C R O S ________________________________________________________*/

#ifndef MMC_SECTOR_SIZE
#define MMC_SECTOR_SIZE                   512   //default sector size is 512 bytes
#endif

#define byte_adr_of_block(ad)             (ad<<9)   // returns the first byte address of a specified sector/block number (512bytes/block)

/*_____ D E F I N I T I O N ________________________________________________*/

// Card identification
#define MMC_CARD                          0
#define SD_CARD                           1

// Lock operations
#define OP_UNLOCK                         0x00
#define OP_LOCK                           0x04
#define OP_RESET_PWD                      0x02
#define OP_SET_PWD                        0x01
#define OP_FORCED_ERASE                   0x08

// MMC commands (taken from MMC reference)
#define MMC_GO_IDLE_STATE                 0    ///< initialize card to SPI-type access
#define MMC_SEND_OP_COND                  1    ///< set card operational mode
#define MMC_CMD2                          2               ///< illegal in SPI mode !
#define MMC_SEND_CSD                      9    ///< get card's CSD
#define MMC_SEND_CID                      10    ///< get card's CID
#define MMC_SEND_STATUS                   13
#define MMC_SET_BLOCKLEN                  16    ///< Set number of bytes to transfer per block
#define MMC_READ_SINGLE_BLOCK             17    ///< read a block
#define MMC_WRITE_BLOCK                   24    ///< write a block
#define MMC_PROGRAM_CSD                   27
#define MMC_SET_WRITE_PROT                28
#define MMC_CLR_WRITE_PROT                29
#define MMC_SEND_WRITE_PROT               30
#define SD_TAG_WR_ERASE_GROUP_START       32
#define SD_TAG_WR_ERASE_GROUP_END         33
#define MMC_TAG_SECTOR_START              32
#define MMC_TAG_SECTOR_END                33
#define MMC_UNTAG_SECTOR                  34
#define MMC_TAG_ERASE_GROUP_START         35    ///< Sets beginning of erase group (mass erase)
#define MMC_TAG_ERASE_GROUP_END           36    ///< Sets end of erase group (mass erase)
#define MMC_UNTAG_ERASE_GROUP             37    ///< Untag (unset) erase group (mass erase)
#define MMC_ERASE                         38    ///< Perform block/mass erase
#define SD_SEND_OP_COND_ACMD              41              ///< Same as MMC_SEND_OP_COND but specific to SD (must be preceeded by CMD55)
#define MMC_LOCK_UNLOCK                   42              ///< To start a lock/unlock/pwd operation
#define SD_APP_CMD55                      55              ///< Use before any specific command (type ACMD)
#define MMC_CRC_ON_OFF                    59    ///< Turns CRC check on/off
// R1 Response bit-defines
#define MMC_R1_BUSY                       0x80  ///< R1 response: bit indicates card is busy
#define MMC_R1_PARAMETER                  0x40
#define MMC_R1_ADDRESS                    0x20
#define MMC_R1_ERASE_SEQ                  0x10
#define MMC_R1_COM_CRC                    0x08
#define MMC_R1_ILLEGAL_COM                0x04
#define MMC_R1_ERASE_RESET                0x02
#define MMC_R1_IDLE_STATE                 0x01
// Data Start tokens
#define MMC_STARTBLOCK_READ               0xFE  ///< when received from card, indicates that a block of data will follow
#define MMC_STARTBLOCK_WRITE              0xFE  ///< when sent to card, indicates that a block of data will follow
#define MMC_STARTBLOCK_MWRITE             0xFC
// Data Stop tokens
#define MMC_STOPTRAN_WRITE                0xFD
// Data Error Token values
#define MMC_DE_MASK                       0x1F
#define MMC_DE_ERROR                      0x01
#define MMC_DE_CC_ERROR                   0x02
#define MMC_DE_ECC_FAIL                   0x04
#define MMC_DE_OUT_OF_RANGE               0x04
#define MMC_DE_CARD_LOCKED                0x04
// Data Response Token values
#define MMC_DR_MASK                       0x1F
#define MMC_DR_ACCEPT                     0x05
#define MMC_DR_REJECT_CRC                 0x0B
#define MMC_DR_REJECT_WRITE_ERROR         0x0D



/*_____ D E C L A R A T I O N ______________________________________________*/

//! Low-level functions (basic management)
extern Bool sd_mmc_internal_init(void);
extern Bool sd_mmc_init(spi_options_t spiOptions, unsigned int pba_hz);             // initializes the SD/MMC card (reset, init, analyse)
extern Bool sd_mmc_check_presence(void);    // check the presence of the card
extern Bool sd_mmc_mem_check(void);         // check the presence of the card, and initialize if inserted
extern Bool sd_mmc_wait_not_busy (void);    // wait for the card to be not busy (exits with timeout)
extern Bool sd_mmc_get_csd(U8 *);           // stores the CSD of the card into csd[16]
extern Bool sd_mmc_get_cid(U8 *);           // stores the CID of the card into cid[16]
extern void sd_mmc_get_capacity(void);      // extract parameters from CSD and compute capacity, last block adress, erase group size
extern Bool sd_mmc_get_status(void);        // read the status register of the card (R2 response)
extern U8   sd_mmc_send_and_read(U8);       // send a byte on SPI and returns the received byte
extern U8   sd_mmc_send_command(U8, U32);   // send a single command + argument (R1 response expected and returned), with memory select then unselect
extern U8   sd_mmc_command(U8, U32);        // send a command + argument (R1 response expected and returned), without memory selct/unselect

//! Protection functions (optionnal)
extern Bool is_sd_mmc_write_pwd_locked(void);     // check if the lock protection on the card is featured and enabled
extern Bool sd_mmc_lock_operation(U8, U8, U8 *);  // use this function to lock/unlock the card or modify password

//! Functions for preparing block read/write
extern Bool sd_mmc_read_open (U32);         // to call before first access to a random page
extern void sd_mmc_read_close (void);
extern Bool sd_mmc_write_open (U32);        // to call before first access to a random page
extern void sd_mmc_write_close (void);

//! Funtions to link USB DEVICE flow with MMC
extern Bool sd_mmc_write_sector (U16);      // write a 512b sector from USB buffer
extern Bool sd_mmc_read_sector (U16);       // reads a 512b sector to an USB buffer
extern Bool sd_mmc_read_multiple_sector(U16 nb_sector);
extern Bool sd_mmc_write_multiple_sector(U16 nb_sector);
extern void sd_mmc_read_multiple_sector_callback(const void *psector);
extern void sd_mmc_write_multiple_sector_callback(void *psector);


/*
//! Funtions to link USB HOST flow with MMC
bit     sd_mmc_host_write_sector (U16);
bit     sd_mmc_host_read_sector (U16);
*/

//! Functions to read/write one sector (512btes) with ram buffer pointer
extern Bool sd_mmc_read_sector_to_ram(void *ram);     // reads a data block and send it to a buffer (512b)
extern Bool sd_mmc_write_sector_from_ram(const void *ram);  // writes a data block from a buffer (512b)
extern Bool sd_mmc_erase_sector_group(U32, U32);    // erase a group of sectors defined by start and end address (details in sd_mmc.c)


//!functions used to make a trasnfer from SD_MMC to RAM using the PDCA
//Max reading size is block
extern Bool sd_mmc_read_open_PDCA (U32);         // to call before first access to a random page
extern void sd_mmc_read_close_PDCA (void);       // unselect the memory


extern U8           csd[16];                    // stores the Card Specific Data
extern volatile U32 capacity;                   // stores the capacity in bytes
extern volatile U32 sd_mmc_last_block_address;
extern  U16         erase_group_size;
extern  U8          r1;
extern  U16         r2;
extern  U8          card_type;                   // stores SD_CARD or MMC_CARD type card
extern  Bool        sd_mmc_init_done;


#endif  // _SD_MMC_H_
