/* This source file is part of the ATMEL AVR32-SoftwareFramework-1.3.0-AT32UC3B Release */
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


/*_____ I N C L U D E S ____________________________________________________*/

#include "conf_access.h"


#if SD_MMC_MEM == ENABLE

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "spi.h"
#include "conf_sd_mmc.h"
#include "sd_mmc.h"


/*_____ M A C R O S ________________________________________________________*/

#define        NO_SUPPORT_USB_PING_PONG                     // defines if USB endpoints do not support ping pong mode


/*_____ D E F I N I T I O N ________________________________________________*/

static U32  gl_ptr_mem;                       // Memory data pointer
//static U8   sector_buf[MMC_SECTOR_SIZE];  // Sector buffer

Bool  sd_mmc_init_done = FALSE;
U8   r1;
U16  r2;

          U8   csd[16];                     // stores the Card Specific Data
volatile  U32  capacity;                    // stores the capacity in bytes
volatile  U32  sd_mmc_last_block_address;   // stores the address of the last block (sector)
          U16  erase_group_size;            // stores the number of blocks concerned by an erase command
          U8   card_type;                   // stores SD_CARD or MMC_CARD type card


          //U8   data_mem[513]; // data buffer
#if      (SD_MMC_READ_CID == ENABLED)
          U8   cid[16];
#endif


/*_____ D E C L A R A T I O N ______________________________________________*/

//!
//! @brief This function initializes the SD/MMC controller.
//!
//!
//! @return bit
//!   The memory is ready     -> OK (always)
Bool sd_mmc_internal_init(void)
{
  U32 retry;
  
  for(retry=0; retry<20; retry++)
	  spi_write(SD_MMC_SPI,0xFF);            // write dummy byte

  // RESET THE MEMORY CARD
  sd_mmc_init_done = FALSE;
  card_type = SD_CARD;
  retry = 0;
  do
  {
    // reset card.
    r1 = sd_mmc_send_command(MMC_GO_IDLE_STATE, 0);
    // do retry counter
    retry++;
    if(retry > 0xff)
      return KO;
  }
  while(r1 != 0x01);   // check memory enters idle_state
  
  spi_write(SD_MMC_SPI,0xFF);
  spi_write(SD_MMC_SPI,0xFF);

  // CONTINUE INTERNAL INITIALIZATION OF THE CARD
  // Continue sending CMD1 while memory card is in idle state
  retry = 0;
  do
  {
     // initializing card for operation
	 r1 = sd_mmc_send_command(SD_APP_CMD55,0);
	 spi_write(SD_MMC_SPI,0xFF);
     r1 = sd_mmc_send_command(SD_SEND_OP_COND_ACMD, 0);
     spi_write(SD_MMC_SPI,0xFF);
     // do retry counter
     retry++;
     if(retry == 500000)    // measured approx. 500 on several cards
        return KO;
  }
  while (r1);

  r1 = sd_mmc_send_command(MMC_CRC_ON_OFF, 0);  // disable CRC (should be already initialized on SPI init)
  spi_write(SD_MMC_SPI,0xFF);            // write dummy byte

  // SET BLOCK LENGTH TO 512 BYTES
  r1 = sd_mmc_send_command(MMC_SET_BLOCKLEN, 512);
  spi_write(SD_MMC_SPI,0xFF);            // write dummy byte
  if (r1 != 0x00)
    return KO;    // card unsupported if block length of 512b is not accepted

  // GET CARD SPECIFIC DATA
  if (KO ==  sd_mmc_get_csd(csd))
    return KO;

  // GET CARD CAPACITY and NUMBER OF SECTORS
  sd_mmc_get_capacity();

  // GET CARD IDENTIFICATION DATA IF REQUIRED
#if (SD_MMC_READ_CID == ENABLED)
  if (KO ==  sd_mmc_get_cid(cid))
    return KO;
#endif

  sd_mmc_init_done = TRUE;

  return(OK);
}

//!
//! @brief This function initializes the SD/MMC controller & the SPI bus(over which the SD_MMC is controlled).
//!
//!
//! @return bit
//!   The memory is ready     -> OK (always)
Bool sd_mmc_init(spi_options_t spiOptions, unsigned int pba_hz)
{
  // Setup SPI registers according to spiOptions.
  spi_setupChipReg(SD_MMC_SPI, &spiOptions, pba_hz);

  // Initialize the SD/MMC controller.
  return sd_mmc_internal_init();
}

//!
//! @brief This function sends a command WITH NO DATA STATE to the SD/MMC and waits for R1 response
//!        This function also selects and unselects the memory => should be used only for single command transmission
//!
//! @param  command   command to send (see sd_mmc.h for command list)
//! @param  arg       argument of the command
//!
//! @return U8
//!         R1 response (R1 == 0xFF if time out error)
U8 sd_mmc_send_command(U8 command, U32 arg)
{
  spi_selectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);    // select SD_MMC
  r1 = sd_mmc_command(command, arg);
  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
  return r1;
}

//!
//! @brief This function sends a command WITH DATA STATE to the SD/MMC and waits for R1 response
//!        The memory /CS signal is not affected so this function can be used to send a commande during a large transmission
//!
//! @param  command   command to send (see sd_mmc.h for command list)
//! @param  arg       argument of the command
//!
//! @return U8
//!         R1 response (R1 == 0xFF time out error)
U8 sd_mmc_command(U8 command, U32 arg)
{
U8 retry;

  spi_write(SD_MMC_SPI,0xFF);            // write dummy byte
  spi_write(SD_MMC_SPI,command | 0x40);  // send command
  spi_write(SD_MMC_SPI,arg>>24);         // send parameter
  spi_write(SD_MMC_SPI,arg>>16);
  spi_write(SD_MMC_SPI,arg>>8 );
  spi_write(SD_MMC_SPI,arg    );
  if(command == 8)
	  spi_write(SD_MMC_SPI,0x86); // CRC for CMD8
  else
	  spi_write(SD_MMC_SPI,0x95); // CRC for CMD0
                                  // after, the CRC is ignored
  // end command
  // wait for response
  // if more than 8 retries, card has timed-out and return the received 0xFF
  retry = 0;
  r1    = 0xFF;
  while((r1 = sd_mmc_send_and_read(0xFF)) == 0xFF)
  {
    retry++;
    if(retry > 10) break;
  }
  return r1;
}



//!
//! @brief This function sends a byte over SPI and returns the byte read from the slave.
//!
//!
//! @param  data_to_send   byte to send over SPI
//!
//! @return U8
//!   Byte read from the slave
U8 sd_mmc_send_and_read(U8 data_to_send)
{
   unsigned short data_read;
   spi_write(SD_MMC_SPI,data_to_send);
   if( SPI_ERROR_TIMEOUT == spi_read(SD_MMC_SPI,&data_read) )
     return 0xFF;
   return (data_read);
}



//!
//! @brief This function reads the CSD (Card Specific Data) of the memory card
//!
//! @param  buffer to fill
//!
//! @return bit
//!         OK / KO
Bool sd_mmc_get_csd(U8 *buffer)
{
U8 retry;
unsigned short data_read;
  // wait for MMC not busy
  if (KO == sd_mmc_wait_not_busy())
    return KO;

  spi_selectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);    // select SD_MMC
  // issue command
  r1 = sd_mmc_command(MMC_SEND_CSD, 0);
  // check for valid response
  if(r1 != 0x00)
  {
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
    sd_mmc_init_done = FALSE;
    return KO;
  }
  // wait for block start
  retry = 0;
  while((r1 = sd_mmc_send_and_read(0xFF)) != MMC_STARTBLOCK_READ)
  {
    if (retry > 8)
    {
      spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
      return KO;
    }
    retry++;
  }
  for (retry = 0; retry <16; retry++)
  {
   spi_write(SD_MMC_SPI,0xFF);
   spi_read(SD_MMC_SPI,&data_read);
    buffer[retry] = data_read;
  }
   spi_write(SD_MMC_SPI,0xFF);   // load CRC (not used)
   spi_write(SD_MMC_SPI,0xFF);
   spi_write(SD_MMC_SPI,0xFF);   // give clock again to end transaction
  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
  return OK;
}


//!
//! @brief This function reads the CID (Card Identification Data) of the memory card
//!
//! @param  buffer to fill
//!
//! @return bit
//!         OK / KO
Bool sd_mmc_get_cid(U8 *buffer)
{
U8 retry;
unsigned short data_read;
  // wait for MMC not busy
  if (KO == sd_mmc_wait_not_busy())
    return KO;

  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // select SD_MMC
  // issue command
  r1 = sd_mmc_command(MMC_SEND_CID, 0);
  // check for valid response
  if(r1 != 0x00)
  {
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
    sd_mmc_init_done = FALSE;
    return KO;
  }
  // wait for data block start
  retry = 0;
  while((r2 = sd_mmc_send_and_read(0xFF)) != MMC_STARTBLOCK_READ)
  {
    if (retry > 8)
    {
      spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
      return KO;
    }
    retry++;
  }
  // store valid data block
  for (retry = 0; retry <16; retry++)
  {
   spi_write(SD_MMC_SPI,0xFF);
   spi_read(SD_MMC_SPI,&data_read);
   buffer[retry] = data_read;
  }
  spi_write(SD_MMC_SPI,0xFF);   // load CRC (not used)
  spi_write(SD_MMC_SPI,0xFF);
  spi_write(SD_MMC_SPI,0xFF);   // give clock again to end transaction
  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
  return OK;
}



//!
//! @brief This function extracts structure information from CSD array
//! and compute the number of blocks of the memory card (stored in global U32 sd_mmc_last_block_address),
//! its capacity in bytes (stored in global U32 capacity)
//! and the block group size for an erase operation
//! Here is defined the position of required fields in CSD array :
//! READ_BL_LEN :
//!         [83:80] == data[5] && 0x0f
//! C_SIZE :
//!         [73:72] == data[6] && 0x03
//!         [71:64] == data[7]
//!         [63:62] == data[8] && 0xc0
//! C_SIZE_MULT :
//!         [49:48] == data[9] && 0x03
//!         [47]    == data[10] && 0x80
//! ERASE_GRP_SIZE (MMC card only) :
//!         [46:42] == data[10] && 0x7c
//! ERASE_GRP_MULT (MMC card only) :
//!         [41:40] == data[10] && 0x03
//!         [39:37] == data[11] && 0xe0
//! SECTOR_SIZE (SD card only) :
//!         [45:40] == data[10] && 0x3F
//!         [39]    == data[11] && 0x80
//!
//! @return bit
//!         OK
void sd_mmc_get_capacity(void)
{
  U16 c_size;
  U8  c_size_mult;
  U8  read_bl_len;
  U8  erase_grp_size;
  U8  erase_grp_mult;

  // extract variables from CSD array
  c_size      = ((csd[6] & 0x03) << 10) + (csd[7] << 2) + ((csd[8] & 0xC0) >> 6);
  c_size_mult = ((csd[9] & 0x03) << 1) + ((csd[10] & 0x80) >> 7);
  read_bl_len = csd[5] & 0x0F;
  if (card_type == MMC_CARD)
  {
    erase_grp_size = ((csd[10] & 0x7C) >> 2);
    erase_grp_mult = ((csd[10] & 0x03) << 3) | ((csd[11] & 0xE0) >> 5);
  }
  else
  {
    erase_grp_size = ((csd[10] & 0x3F) << 1) + ((csd[11] & 0x80) >> 7);
    erase_grp_mult = 0;
  }

  // compute last block addr
  sd_mmc_last_block_address = ((U32)(c_size + 1) * (U32)((1 << (c_size_mult + 2)))) - 1;
  if (read_bl_len > 9)  // 9 means 2^9 = 512b
    sd_mmc_last_block_address <<= (read_bl_len - 9);

  // compute card capacity in bytes
  capacity = (1 << read_bl_len) * (sd_mmc_last_block_address + 1);

  // compute block group size for erase operation
  erase_group_size = (erase_grp_size + 1) * (erase_grp_mult + 1);
}



//!
//! @brief    This function reads the STATUS regsiter of the memory card
//!           After a read the error flags are automatically cleared
//!
//! @return bit
//!           The open succeeded      -> OK
Bool     sd_mmc_get_status(void)
{
  U8 retry, spireg;

  // wait for MMC not busy
  if (KO == sd_mmc_wait_not_busy())
    return KO;

  spi_selectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);    // select SD_MMC

  // send command
  spi_write(SD_MMC_SPI,MMC_SEND_STATUS | 0x40);  // send command
  spi_write(SD_MMC_SPI,0);                       // send parameter
  spi_write(SD_MMC_SPI,0);
  spi_write(SD_MMC_SPI,0);
  spi_write(SD_MMC_SPI,0);
  spi_write(SD_MMC_SPI,0x95);            // correct CRC for first command in SPI (CMD0)
                                  // after, the CRC is ignored
  // end command
  // wait for response
  // if more than 8 retries, card has timed-out and return the received 0xFF
  retry = 0;
  r2 = 0xFFFF;
  spireg = 0xFF;
  while((spireg = sd_mmc_send_and_read(0xFF)) == 0xFF)
  {
    retry++;
    if(retry > 10)
    {
      spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);
      return KO;
    }
  }
  r2 = ((U16)(spireg) << 8) + sd_mmc_send_and_read(0xFF);    // first byte is MSb

  spi_write(SD_MMC_SPI,0xFF);   // give clock again to end transaction
  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC

  return OK;
}


//!
//! @brief This function waits until the SD/MMC is not busy.
//!
//! @return bit
//!          OK when card is not busy
Bool sd_mmc_wait_not_busy(void)
{
  U16 retry;

  // Select the SD_MMC memory gl_ptr_mem points to
  spi_selectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);
  retry = 0;
  while((r1 = sd_mmc_send_and_read(0xFF)) != 0xFF)
  {
    retry++;
    if (retry == 50000)
    {
      spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);
      return KO;
    }
  }
  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);
  return OK;
}



//!
//! @brief This function check the presence of a memory card
//!     - if the card was already initialized (removal test), the host send a CRC_OFF command (CMD59) and check the answer
//!     - if the card was not already initialized (insertion test), the host send a CMD0 reset command and check the answer
//!
//! @return bit
//!   The memory is present (OK)
//!   The memory does not respond (disconnected) (KO)
Bool sd_mmc_check_presence(void)
{
  U16 retry;

  retry = 0;
  if (sd_mmc_init_done == FALSE)
  {
    // If memory is not initialized, try to initialize it (CMD0)
    // If no valid response, there is no card
    while ((r1 = sd_mmc_send_command(MMC_GO_IDLE_STATE, 0)) != 0x01)
    {
  spi_write(SD_MMC_SPI,0xFF);            // write dummy byte
      retry++;
      if (retry > 10)
        return KO;
    }
    return OK;
  }
  else
  {
    if ((r1 = sd_mmc_send_command(MMC_CRC_ON_OFF,0)) == 0x00)
      return OK;
    sd_mmc_init_done = FALSE;
    return KO;
  }
}


//!
//! @brief This function performs a memory check on the SD_MMC.
//!
//!
//! @return bit
//!   The memory is ready     -> OK
//!   The memory check failed -> KO
Bool sd_mmc_mem_check(void)
{
  if (sd_mmc_check_presence() == OK)
  {
    if (sd_mmc_init_done == FALSE)
    {
      sd_mmc_internal_init();
    }
    if (sd_mmc_init_done == TRUE)
      return OK;
    else
      return KO;
  }
  return KO;
}



//!
//! @brief This function checks if the card is password-locked
//!        Old versions of MMC card don't support this feature !
//!        For a MMC, "lock protection" is featured from v2.1 release !
//!          => see CSD[0]<5:2> bits to know the version : 0x0=1.x, 0x1=1.4, 0x2=2.x, 0x3=3.x, 0x4=4.0
//!
//! @return bit
//!   Password protected         -> OK
//!   NOT password protected     -> KO (or card not initialized)
Bool is_sd_mmc_write_pwd_locked(void)
{
  if (card_type == MMC_CARD)
  {
    if (((csd[0] >> 2) & 0x0F) < 2) // lock feature is not present on the card since the MMC is v1.x released !
      return KO;
  }
  if (KO == sd_mmc_get_status())    // get STATUS response
    return KO;
  if ((r2&0x0001) != 0)             // check "card is locked" flag in R2 response
    return OK;

  return KO;
}


//!
//! @brief This function manages locking operations for the SD/MMC card (password protection)
//!         - Once the card is locked, the only commands allowed are UNLOCK and FORCED_ERASE
//!         - Once the card is unlocked, the commands allowed are all the others
//!         - Before setting a new password (SET_PWD), the current one must be cleared (RESET_PWD)
//!         - If card contains a password (PWDSLEN != 0), the card will automatically be locked at start-up
//!
//!    /!\  Take care that old versions of MMC cards don't support this feature !
//!         For a MMC, "lock protection" is featured only from v2.1 release !
//!           => see CSD[0]<5:2> bits to know the version : 0x0=1.x, 0x1=1.4, 0x2=2.x, 0x3=3.x, 0x4=4.0
//!         Moreover the OP_FORCED_ERASE command can also have no effect on some cards !
//!
//! @param operation
//!           OP_LOCK           -> to lock the card (the current pasword must be specified)
//!           OP_UNLOCK         -> to unlock the card (the current password must be specified)
//!           OP_RESET_PWD      -> to clear the current password (the current password must be specified)
//!           OP_SET_PWD        -> to set a new password to the card (the old password must have been cleared first)
//!           OP_FORCED_ERASE   -> to erase completely the card and the password (no password needed)
//! @param pwd_lg
//!           Password length
//! @param pwd
//!           Pointer on the password (char array) to send
//!
//! @return bit
//!   Operation succeeded       -> OK
//!   Operation failed          -> KO
Bool sd_mmc_lock_operation(U8 operation, U8 pwd_lg, U8 * pwd)
{
  Bool status = OK;
  U8 retry;

  // check parameters validity
  if ((operation != OP_FORCED_ERASE) && (pwd_lg == 0))  // password length must be > 0
    return KO;

  // wait card not busy
  if (sd_mmc_wait_not_busy() == KO)
    return KO;

  // set block length
  if (operation == OP_FORCED_ERASE)
    r1 = sd_mmc_send_command(MMC_SET_BLOCKLEN, 1);   // CMD
  else
    r1 = sd_mmc_send_command(MMC_SET_BLOCKLEN, pwd_lg+2);   // CMD + PWDSLEN + PWD
  spi_write(SD_MMC_SPI,0xFF);            // write dummy byte
  spi_write(SD_MMC_SPI,0xFF);            // write dummy byte
  spi_write(SD_MMC_SPI,0xFF);            // write dummy byte
  if (r1 != 0x00)
    return KO;

  // send the lock command to the card
  spi_selectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);    // select SD_MMC

  // issue command
  r1 = sd_mmc_command(MMC_LOCK_UNLOCK, 0);

  // check for valid response
  if(r1 != 0x00)
  {
    status = KO;
  }
  // send dummy
  spi_write(SD_MMC_SPI,0xFF);   // give clock again to end transaction

  // send data start token
  spi_write(SD_MMC_SPI,MMC_STARTBLOCK_WRITE);
  // write data
  spi_write(SD_MMC_SPI,operation);
  if (operation != OP_FORCED_ERASE)
  {
  spi_write(SD_MMC_SPI,pwd_lg);
    for(retry=0 ; retry<pwd_lg ; retry++)
    {
  spi_write(SD_MMC_SPI,*(pwd+retry));
    }
  }
  spi_write(SD_MMC_SPI,0xFF);    // send CRC (field required but value ignored)
  spi_write(SD_MMC_SPI,0xFF);

  // check data response token
  retry = 0;
  r1 = sd_mmc_send_and_read(0xFF);
  if ((r1 & MMC_DR_MASK) != MMC_DR_ACCEPT)
    status = KO;

  spi_write(SD_MMC_SPI,0xFF);    // dummy byte
  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);

  // wait card not busy
  if (operation == OP_FORCED_ERASE)
    retry = 100;
  else
    retry = 10;
  while (sd_mmc_wait_not_busy() == KO)
  {
    retry--;
    if (retry == 0)
    {
      status = KO;
      break;
    }
  }

  // get and check status of the operation
  if (KO == sd_mmc_get_status())    // get STATUS response
    status = KO;
  if ((r2&0x0002) != 0)   // check "lock/unlock cmd failed" flag in R2 response
    status = KO;

  // set original block length
  r1 = sd_mmc_send_command(MMC_SET_BLOCKLEN, 512);
  if (r1 != 0x00)
    status = KO;

  return status;
}



//!
//! @brief This function opens a SD_MMC memory in read mode at a given sector address (not byte address)
//!
//! @param  pos   Sector address
//!
//! @return bit
//!   The open succeeded      -> OK
Bool sd_mmc_read_open (U32 pos)
{
  // Set the global memory ptr at a Byte address.
  gl_ptr_mem = pos << 9;        // gl_ptr_mem = pos * 512

  // wait for MMC not busy
  return sd_mmc_wait_not_busy();
}


//!
//! @brief This function unselects the current SD_MMC memory.
//!
void sd_mmc_read_close (void)
{

}


//!
//! @brief This function opens a SD_MMC memory in write mode at a given sector
//! address.
//!
//! NOTE: If page buffer > 512 bytes, page content is first loaded in buffer to
//! be partially updated by write_byte or write64 functions.
//!
//! @param  pos   Sector address
//!
//! @return bit
//!   The open succeeded      -> OK
Bool sd_mmc_write_open (U32 pos)
{
  // Set the global memory ptr at a Byte address.
  gl_ptr_mem = pos << 9;                    // gl_ptr_mem = pos * 512

  // wait for MMC not busy
  return sd_mmc_wait_not_busy();
}


//!
//! @brief This function fills the end of the logical sector (512B) and launch
//! page programming.
//!
void sd_mmc_write_close (void)
{

}

//!
//! @brief This function allow to read multiple sectors
//!
//! @param  nb_sector   the number of sector to read
//! @return bit
//!   The read succeeded      -> OK
Bool sd_mmc_read_multiple_sector(U16 nb_sector)
{
	return KO;
	/*
  while (nb_sector--)
  {
    // Read the next sector
    sd_mmc_read_sector_to_ram(sector_buf);
    sd_mmc_read_multiple_sector_callback(sector_buf);
  }

  return OK;
  */
}

//!
//! @brief This function allow to write multiple sectors
//!
//! @param  nb_sector   the number of sector to write
//! @return bit
//!   The write succeeded      -> OK
Bool sd_mmc_write_multiple_sector(U16 nb_sector)
{
	return KO;
	/*
  while (nb_sector--)
  {
    // Write the next sector
    sd_mmc_write_multiple_sector_callback(sector_buf);
    sd_mmc_write_sector_from_ram(sector_buf);
  }

  return OK;
  */
}

//! @brief  This function erase a group of sectors
//!        NOTE : Erasing operation concerns only groups of sectors and not one sector only
//!               The global variable "erase_group_size" (extracted from CSD) contains the sector group size boundary
//!               User specifies the addresses of the first group and the last group to erase (several contiguous groups can be selected for erase)
//!               An misaligned address will not generate an error since the memory card ignore the LSbs of the address
//!               Some examples (with "erase_group_size" = 0x20 = group boundary) :
//!                 - adr_start=0x100 and adr_end=0x100, all the sectors from 0x100 up to 0x11F will be erased
//!                 - adr_start=0x90 and adr_end=0x100, all the sectors from 0x80 up to 0x11F will be erased (0x90 interpreted as 0x80)
//!                 - adr_start=0x80 and adr_end=0x146, all the sectors from 0x80 up to 0x15F will be erased
//!               This function just initiates a transmission, user may get status register to check that operation has succeeded
//!               After an erase, a MMC card contains bits at 0, and SD can contains bits 0 or 1 (according to field DATA_STAT_AFTER_ERASE in the CSD)
//!
//! @param adr_start         address of 1st group   (sector address, not byte address)
//! @param adr_end           address of last group  (sector address, not byte address)
//!
//! @return bit
//!   The erase operation succeeded (has been started)  -> OK
//!   The erase operation failed (not started)  -> KO
Bool sd_mmc_erase_sector_group(U32 adr_start, U32 adr_end)
{
  U8 cmd;

  // wait for MMC not busy
  if (KO == sd_mmc_wait_not_busy())
    return KO;

  spi_selectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);    // select SD_MMC

  // send address of 1st group
  if (card_type == MMC_CARD)
  { cmd = MMC_TAG_ERASE_GROUP_START; }
  else
  { cmd = SD_TAG_WR_ERASE_GROUP_START; }
  if ((r1 = sd_mmc_command(cmd,(adr_start << 9))) != 0)
  {
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);
    return KO;
  }
    spi_write(SD_MMC_SPI,0xFF);

  // send address of last group
  if (card_type == MMC_CARD)
  { cmd = MMC_TAG_ERASE_GROUP_END; }
  else
  { cmd = SD_TAG_WR_ERASE_GROUP_END; }
  if ((r1 = sd_mmc_command(cmd,(adr_end << 9))) != 0)
  {
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);
    return KO;
  }
  spi_write(SD_MMC_SPI,0xFF);

  // send erase command
  if ((r1 = sd_mmc_command(MMC_ERASE,0)) != 0)
  {
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);
    return KO;
  }
  spi_write(SD_MMC_SPI,0xFF);

  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);

  return OK;
}


//! Stop PDCA transfer
//! @brief This function closes a PDCA read transfer
//! page programming.
//!
void sd_mmc_read_close_PDCA (void)
{

  // load 16-bit CRC (ignored)
  spi_write(SD_MMC_SPI,0xFF);
  spi_write(SD_MMC_SPI,0xFF);

  // continue delivering some clock cycles
  spi_write(SD_MMC_SPI,0xFF);
  spi_write(SD_MMC_SPI,0xFF);

  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
  //print_dbg("SD_MMC is unselected\n");

 /* if(!sd_mmc_wait_not_busy())
  print_dbg("SD_MMC is busy\n");*/
//  Trace("\nSD_MMC is unselected");
}



//! Reading using PDCA
//! @brief This function opens a SD_MMC memory in read mode at a given sector
//! address.
//!
//! NOTE: If page buffer > 512 bytes, page content is first loaded in buffer to
//! be partially updated by write_byte or write64 functions.
//!
//! @param  pos   Sector address
//!
//! @return bit
//!   The open succeeded      -> OK
//!/
Bool sd_mmc_read_open_PDCA (U32 pos)
{
  U16 read_time_out;

  // Set the global memory ptr at a Byte address.
  gl_ptr_mem = pos << 9;                    // gl_ptr_mem = pos * 512

  // wait for MMC not busy
  if (KO == sd_mmc_wait_not_busy())
    return KO;


  spi_selectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);          // select SD_MMC
  // issue command
  r1 = sd_mmc_command(MMC_READ_SINGLE_BLOCK, gl_ptr_mem);

  // check for valid response
  if (r1 != 0x00)
  {
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
   // print_dbg("\n****** could not open SD_MMC for single block read\n");
   // Trace("\n****** could not open SD_MMC for single block read\n");
    return KO;
  }

  // wait for token (may be a datablock start token OR a data error token !)
  read_time_out = 30000;
  while((r1 = sd_mmc_send_and_read(0xFF)) == 0xFF)
  {
     read_time_out--;
     if (read_time_out == 0)   // TIME-OUT
     {
       spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS); // unselect SD_MMC
   //    print_dbg("**** datablock start error or token error \n");
   // Trace("**** data error token SD_MMC\n");
       return KO;
     }
  }

  // check token
  if (r1 != MMC_STARTBLOCK_READ)
  {
    spi_write(SD_MMC_SPI,0xFF);
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
    //print_dbg("******bad SD_MMC token \n");
   //  Trace("******token SD_MMC\n");
    return KO;
  }
 // print_dbg("********* SD_MMC opened successfuly\n");
 // Trace("********* SD_MMC opened successfuly\n");
  return OK;   // Read done.
}


//! @brief This function read one MMC sector and load it into a ram buffer
//!
//!         DATA FLOW is: SD/MMC => RAM
//!
//!
//! NOTE:
//!   - First call (if sequential read) must be preceded by a call to the sd_mmc_read_open() function
//!
//! @param ram         pointer to ram buffer
//!
//! @return bit
//!   The read succeeded   -> OK
//!   The read failed (bad address, etc.)  -> KO
//!/
Bool sd_mmc_read_sector_to_ram(void *ram)
{
  U8 *_ram = ram;
  U16  i;
  U16  read_time_out;
  unsigned short data_read;
  // wait for MMC not busy
  if (KO == sd_mmc_wait_not_busy())
    return KO;

  spi_selectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);    // select SD_MMC
  // issue command
  r1 = sd_mmc_command(MMC_READ_SINGLE_BLOCK, gl_ptr_mem);

  // check for valid response
  if (r1 != 0x00)
  {
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
    return KO;
  }

  // wait for token (may be a datablock start token OR a data error token !)
  read_time_out = 30000;
  while((r1 = sd_mmc_send_and_read(0xFF)) == 0xFF)
  {
     read_time_out--;
     if (read_time_out == 0)   // TIME-OUT
     {
       spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS); // unselect SD_MMC
       return KO;
     }
  }

  // check token
  if (r1 != MMC_STARTBLOCK_READ)
  {
    spi_write(SD_MMC_SPI,0xFF);
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
    return KO;
  }

  // store datablock
  for(i=0;i<MMC_SECTOR_SIZE;i++)
  {
    spi_write(SD_MMC_SPI,0xFF);
    spi_read(SD_MMC_SPI,&data_read);
    *_ram++=data_read;
  }
  gl_ptr_mem += 512;     // Update the memory pointer.

  // load 16-bit CRC (ignored)
  spi_write(SD_MMC_SPI,0xFF);
  spi_write(SD_MMC_SPI,0xFF);

  // continue delivering some clock cycles
  spi_write(SD_MMC_SPI,0xFF);
  spi_write(SD_MMC_SPI,0xFF);

  // release chip select
  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC

  return OK;   // Read done.
}


//! @brief This function writes one MMC sector from a ram buffer
//!
//!         DATA FLOW is: RAM => SD/MMC
//!
//!
//! NOTE (please read) :
//!   - First call (if sequential write) must be preceded by a call to the sd_mmc_write_open() function
//!   - An address error will not detected here, but with the call of sd_mmc_get_status() function
//!   - The program exits the functions with the memory card busy !
//!
//! @param ram         pointer to ram buffer
//!
//! @return bit
//!   The write succeeded   -> OK
//!   The write failed      -> KO
//!
Bool sd_mmc_write_sector_from_ram(const void *ram)
{
  const U8 *_ram = ram;
  U16 i;

  // wait for MMC not busy
  if (KO == sd_mmc_wait_not_busy())
    return KO;

  spi_selectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);    // select SD_MMC
  // issue command
  r1 = sd_mmc_command(MMC_WRITE_BLOCK, gl_ptr_mem);
  // check for valid response
  if(r1 != 0x00)
  {
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);
    return KO;
  }
  // send dummy
    spi_write(SD_MMC_SPI,0xFF);   // give clock again to end transaction

  // send data start token
    spi_write(SD_MMC_SPI,MMC_STARTBLOCK_WRITE);
  // write data
  for(i=0;i<MMC_SECTOR_SIZE;i++)
  {
    spi_write(SD_MMC_SPI,*_ram++);
  }

    spi_write(SD_MMC_SPI,0xFF);    // send CRC (field required but value ignored)
    spi_write(SD_MMC_SPI,0xFF);

  // read data response token
  r1 = sd_mmc_send_and_read(0xFF);
  if( (r1&MMC_DR_MASK) != MMC_DR_ACCEPT)
  {
    spi_write(SD_MMC_SPI,0xFF);    // send dummy bytes
    spi_write(SD_MMC_SPI,0xFF);
    spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);
     return KO;
//     return r1;             // return ERROR byte
  }

    spi_write(SD_MMC_SPI,0xFF);    // send dummy bytes
    spi_write(SD_MMC_SPI,0xFF);

  // release chip select
  spi_unselectChip(SD_MMC_SPI, SD_MMC_SPI_NPCS);  // unselect SD_MMC
  gl_ptr_mem += 512;        // Update the memory pointer.

  // wait card not busy after last programming operation
  i=0;
  while (KO == sd_mmc_wait_not_busy())
  {
    i++;
    if (i == 10)
      return KO;
  }

  return OK;                  // Write done
}


#endif  // SD_MMC_MEM == ENABLE
