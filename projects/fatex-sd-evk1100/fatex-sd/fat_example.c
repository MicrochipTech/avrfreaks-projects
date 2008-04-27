/* This source file is part of the ATMEL AVR32-SoftwareFramework-1.2.1ES-AT32UC3A Release */

/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief FAT access example application.
 *
 * This file gives an example of using the FAT access, through a shell.
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

/*! \page License
 * Copyright (c) 2007, Atmel Corporation All rights reserved.
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

/*! \mainpage
 * \section intro Introduction
 * <p>This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software. It also gives an example of the usage of the
 * FAT file system through the dataflash memory (AT45DBX) & SPI driver. </p>
 * <p>The example is based on a shell like interface on RS232, that will let you issue the following commands :
 * <ul>
 * <li>a:, b:... goto selected drive</li>
 * <li>mount drivename(a, b...): mount selected drive</li>
 * <li>cd dirname: change path to "dirname"</li>
 * <li>ls: display the content of the current directory</li>
 * <li>mkdir dirname: create "dirname"</li>
 * <li>cat filename: display "filename" content</li>
 * <li>touch filename: create "filename"</li>
 * <li>disk: get number of drives</li>
 * <li>append filename: append text to a file</li>
 * <li>mark: bookmark current directory</li>
 * <li>goto: goto bookmark</li>
 * <li>cp filename: copy filename to bookmark</li>
 * <li>df: get free space information</li>
 * <li>fat: get FAT type for current drive</li>
 * <li>rm filename: erase file or EMPTY directory</li>
 * <li>format drivename(a, b...): Format the selected disk (erase all data on it)</li>
 * <li>format32 drivename(a, b...): Force to format the selected disk in FAT32 (erase all data on it) [only possible if disk size allow it]</li>
 * <li>mv src dst: move file or directory</li>
 * <li>help: Display command helper</li>
 * </ul>
 * </p>
 *
 * Example to copy-paste a file: Bookmark the destination location directory using the command "mark". Then go to the source location of the wanted copied file. (the destination directory should be different from the source directory)
 * Type "cp thefileiwanttocopy.txt". Go back to the destination location, type "ls", you should see the copied file now.
 *
 * More information is available in the \subpage FileSystem section.
 *
 * \section files Main Files
 * - file.c : functions for file accesses
 * - file.h : headers for file accesses
 * - fat.c : functions for FAT accesses
 * - fat.h : headers for FAT accesses
 * - fat_unusual.c : add-ons functions for FAT accesses
 * - fs_com.h : structures and defines for file system accesses
 * - navigation.c : functions for navigators accesses
 * - navigation.h : headers for navigators accesses
 * - ctrl_access.c : functions for memory control access interfaces
 * - ctrl_access.h : headers for memory control access interfaces
 * - fat_example.c : FAT example
 *
 * \section cfgfiles Configuration Files
 * - conf_access.h: memory control configuration file for this example
 * - conf_at45dbx.h: dataflash AT45DB memory configuration file for this example
 * - conf_explorer.h: FAT explorer configuration for this example
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32 and IAR Systems compiler
 * for AVR32. Other compilers may or may not work.
 *
 * \section deviceinfo Device Info
 * All AVR32 devices with an SPI module can be used. This example has been tested
 * with the following setup:<BR>
 * - EVK1100 evaluation kit
 *
 * \section setupinfo Setup Information
 * <BR>CPU speed: <i> 12 MHz </i>
 * - Connect the USART_1 to your serial port via a standard RS-232 D-SUB9 cable
 * - Set the following settings in your terminal of choice: 57600 8N1
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 * Support and FAQ: http://support.atmel.no/
 */


//_____  I N C L U D E S ___________________________________________________

#include <string.h>
#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "print_funcs.h"
#include "ctrl_access.h"
#include "pm.h"
#include "gpio.h"
#include "usart.h"
#include "spi.h"
#include "fat.h"
#include "file.h"
#include "navigation.h"

//!!!
#include "conf_sd_mmc.h"
#include "pdca.h"
#include "intc.h"
// Dummy char table
const char dummy_data[] =
#include "dummy.h"
;
//!!!

//_____ M A C R O S ________________________________________________________

/*! \name Shell USART Configuration
 */
//! @{
#if BOARD == EVK1100
#  define SHL_USART               (&AVR32_USART1)
#  define SHL_USART_RX_PIN        AVR32_USART1_RXD_0_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART1_RXD_0_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART1_TXD_0_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART1_TXD_0_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#elif BOARD == EVK1101
#  define SHL_USART               (&AVR32_USART1)
#  define SHL_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define SHL_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define SHL_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define SHL_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define SHL_USART_BAUDRATE      57600
#endif
//! @}

/*! \name Shell Commands
 */
//! @{
#define CMD_NONE              0x00
#define CMD_MOUNT             0x01
#define CMD_LS                0x02
#define CMD_CD                0x03
#define CMD_CAT               0x04
#define CMD_HELP              0x05
#define CMD_MKDIR             0x06
#define CMD_TOUCH             0x07
#define CMD_RM                0x08
#define CMD_APPEND            0x09
#define CMD_NB_DRIVE          0x0A
#define CMD_SET_ID            0x0B
#define CMD_GOTO_ID           0x0C
#define CMD_CP                0x0D
#define CMD_DF                0x0E
#define CMD_MV                0x0F
#define CMD_FORMAT            0x10
#define CMD_FAT               0x11
#define CMD_FORMAT32          0x12
//! @}

/*! \name Special Char Values
 */
//! @{
#define CR                    '\r'
#define LF                    '\n'
#define CTRL_C                0x03
#define CTRL_Q                0x11
#define BKSPACE_CHAR          '\b'
#define ABORT_CHAR            CTRL_C
#define QUIT_APPEND           CTRL_Q
#define HISTORY_CHAR          '!'
//! @}

/*! \name String Values for Commands
 */
//! @{
#define STR_CD                "cd"
#define STR_MOUNT             "mount"
#define STR_CP                "cp"
#define STR_LS                "ls"
#define STR_RM                "rm"
#define STR_DF                "df"
#define STR_MKDIR             "mkdir"
#define STR_TOUCH             "touch"
#define STR_APPEND            "append"
#define STR_CAT               "cat"
#define STR_DISK              "disk"
#define STR_MARK              "mark"
#define STR_GOTO              "goto"
#define STR_MV                "mv"
#define STR_A                 "a:"
#define STR_B                 "b:"
#define STR_C                 "c:"
#define STR_D                 "d:"
#define STR_HELP              "help"
#define STR_FORMAT            "format"
#define STR_FORMAT32          "format32"
#define STR_FAT               "fat"
//! @}

/*! \name String Messages
 */
//! @{
#define MSG_PROMPT            "$>"
#define MSG_WELCOME           "\n" \
                              "-------------------------\n" \
                              "    ATMEL AVR32 Shell\n" \
                              "-------------------------\n"
#define MSG_ER_CMD_NOT_FOUND  "Command not found\n"
#define MSG_ER_MOUNT          "Unable to mount drive\n"
#define MSG_ER_DRIVE          "Drive does not exist\n"
#define MSG_ER_RM             "Can not erase; if the name is a directory, check it is empty\n"
#define MSG_ER_UNKNOWN_FILE   "Unknown file\n"
#define MSG_ER_MV             "Error during move\n"
#define MSG_ER_FORMAT         "Format fails\n"
#define MSG_APPEND_WELCOME    "\nSimple text editor, enter char to append, ^q to exit and save\n"
#define MSG_HELP              "Commands summary\n" \
                              " a:, b:... goto selected drive               mount disk(a, b...)\n" \
                              " cd dirname                                  ls\n" \
                              " mkdir dirname                               cat filename\n" \
                              " touch filename                              disk: get number of drives\n" \
                              " append filename                             goto: goto bookmark\n" \
                              " mark: bookmark current directory            df: get free space information\n" \
                              " cp filename: copy filename to bookmark      fat: get FAT type for current drive\n" \
                              " rm filename: erase file or EMPTY directory  format drivename, with drivename: a, b...\n" \
                              " mv src dst: move file or directory          format32 drivename, with drivename: a, b...\n" \
                              " help\n"
//! @}

//!!!
//! \brief PBA clock frequency (Hz)
#define PBA_HZ                FOSC0

//! \brief Number of bytes in the receive buffer when operating in slave mode
#define BUFFERSIZE            64

#if BOARD==EVK1100
#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI1_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI1_TX
#elif BOARD==EVK1101
#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI_TX
#else
#  error No known AVR32 board defined
#endif
#define AVR32_PDCA_CHANNEL_SPI_RX 0 // In the example we will use the pdca channel 0.
#define AVR32_PDCA_CHANNEL_SPI_TX 1 // In the example we will use the pdca channel 1.
//!!!

//_____ D E C L A R A T I O N S ____________________________________________
//! flag for a command presence
static Bool cmd;
//! command number
static U8   cmd_type;
//! flag for first ls : mount if set
static Bool first_ls;
//! string length
static U8   i_str = 0;

//! string for command
static char cmd_str[10 + 2 * MAX_FILE_PATH_LENGTH];
//! string for first arg
static char par_str1[MAX_FILE_PATH_LENGTH];
//! string for second arg
static char par_str2[MAX_FILE_PATH_LENGTH];

//! buffer for command line
static char str_buff[MAX_FILE_PATH_LENGTH];

//!!!
// PDCA Channel pointer
volatile avr32_pdca_channel_t* pdca_channelrx ;
volatile avr32_pdca_channel_t* pdca_channeltx ;

// Used to indicated an end of transfer
volatile Bool end_of_transfer;

// Large local ram_buffer for the example
volatile char ram_buffer[1000];
//!!!

//_____ D E F I N I T I O N S ______________________________________________

/*! \brief Decodes full command line into command type and arguments.
 *
 * This function allows to set the \ref cmd_type variable to the command type
 * decoded with its respective arguments \ref par_str1 and \ref par_str2.
 */
static void fat_example_parse_cmd(void)
{
  U8 i, j;

  // Get command type.
  for (i = 0; cmd_str[i] != ' ' && i < i_str; i++);

  if (i)
  {
    cmd = TRUE;
    // Save last byte
    j = cmd_str[i];
    // Reset vars
    cmd_str[i] = '\0';
    par_str1[0] = '\0';
    par_str2[0] = '\0';

    // Decode command type.
    if      (!strcmp(cmd_str, STR_CD      )) cmd_type = CMD_CD;
    else if (!strcmp(cmd_str, STR_MOUNT   )) cmd_type = CMD_MOUNT;
    else if (!strcmp(cmd_str, STR_FAT     )) cmd_type = CMD_FAT;
    else if (!strcmp(cmd_str, STR_CP      )) cmd_type = CMD_CP;
    else if (!strcmp(cmd_str, STR_LS      )) cmd_type = CMD_LS;
    else if (!strcmp(cmd_str, STR_RM      )) cmd_type = CMD_RM;
    else if (!strcmp(cmd_str, STR_DF      )) cmd_type = CMD_DF;
    else if (!strcmp(cmd_str, STR_MKDIR   )) cmd_type = CMD_MKDIR;
    else if (!strcmp(cmd_str, STR_TOUCH   )) cmd_type = CMD_TOUCH;
    else if (!strcmp(cmd_str, STR_APPEND  )) cmd_type = CMD_APPEND;
    else if (!strcmp(cmd_str, STR_CAT     )) cmd_type = CMD_CAT;
    else if (!strcmp(cmd_str, STR_DISK    )) cmd_type = CMD_NB_DRIVE;
    else if (!strcmp(cmd_str, STR_MARK    )) cmd_type = CMD_SET_ID;
    else if (!strcmp(cmd_str, STR_GOTO    )) cmd_type = CMD_GOTO_ID;
    else if (!strcmp(cmd_str, STR_MV      )) cmd_type = CMD_MV;
    else if (!strcmp(cmd_str, STR_A       )) cmd_type = CMD_MOUNT, par_str1[0] = 'a', par_str1[1] = '\0';
    else if (!strcmp(cmd_str, STR_B       )) cmd_type = CMD_MOUNT, par_str1[0] = 'b', par_str1[1] = '\0';
    else if (!strcmp(cmd_str, STR_C       )) cmd_type = CMD_MOUNT, par_str1[0] = 'c', par_str1[1] = '\0';
    else if (!strcmp(cmd_str, STR_D       )) cmd_type = CMD_MOUNT, par_str1[0] = 'd', par_str1[1] = '\0';
    else if (!strcmp(cmd_str, STR_HELP    )) cmd_type = CMD_HELP;
    else if (!strcmp(cmd_str, STR_FORMAT  )) cmd_type = CMD_FORMAT;
    else if (!strcmp(cmd_str, STR_FORMAT32)) cmd_type = CMD_FORMAT32;
    else
    {
      // error : command not found
      print(SHL_USART, MSG_ER_CMD_NOT_FOUND);
      cmd = FALSE;
    }
    // restore last byte
    cmd_str[i] = j;
  }
  // if command isn't found, display prompt
  if (!cmd)
  {
    print(SHL_USART, MSG_PROMPT);
    return;
  }

  // Get first arg (if any).
  if (++i < i_str)
  {
    j = 0;
    // remove " if used
    if (cmd_str[i] == '"')
    {
      i++;
      for (; cmd_str[i] != '"' && i < i_str; i++, j++)
      {
        par_str1[j] = cmd_str[i];
      }
      i++;
    }
    // get the arg directly
    else
    {
      for(; cmd_str[i] != ' ' && i < i_str; i++, j++)
      {
        par_str1[j] = cmd_str[i];
      }
    }
    // null terminated arg
    par_str1[j] = '\0';
  }

  // Get second arg (if any).
  if (++i < i_str)
  {
    j = 0;
    // remove " if used
    if (cmd_str[i] == '"')
    {
      i++;
      for (; cmd_str[i] != '"' && i < i_str; i++, j++)
      {
        par_str2[j] = cmd_str[i];
      }
      i++;
    }
    // get the arg directly
    else
    {
      for (; cmd_str[i] != ' ' && i < i_str; i++, j++)
      {
        par_str2[j] = cmd_str[i];
      }
    }
    // null terminated arg
    par_str2[j] = '\0';
  }
}


/*! \brief Gets the full command line on RS232 input to be interpreted.
 * The cmd_str variable is built with the user inputs.
 */
static void fat_example_build_cmd(void)
{
  int c;

  // If something new in the USART
  usart_reset_status(SHL_USART);
  if (usart_read_char(SHL_USART, &c) == USART_SUCCESS)
  {
    switch (c)
    {
    case CR:
      // Add LF.
      print_char(SHL_USART, LF);
      // Add NUL char.
      cmd_str[i_str] = '\0';
      // Decode the command.
      fat_example_parse_cmd();
      i_str = 0;
      break;
    // ^c abort cmd.
    case ABORT_CHAR:
      // Reset command length.
      i_str = 0;
      // Display prompt.
      print(SHL_USART, "\n" MSG_PROMPT);
      break;
    // Backspace.
    case BKSPACE_CHAR:
      if (i_str > 0)
      {
        // Replace last char.
        print(SHL_USART, "\b \b");
        // Decraese command length.
        i_str--;
      }
      break;
    default:
      // Echo.
      print_char(SHL_USART, c);
      // Append to cmd line.
      cmd_str[i_str++] = c;
      break;
    }
  }
}


/*! \brief Minimalist file editor to append char to a file.
 *
 * \note Hit ^q to exit and save file.
 */
static void fat_example_append_file(void)
{
  int c;

  print(SHL_USART, MSG_APPEND_WELCOME);

  // Wait for ^q to quit.
  while (TRUE)
  {
    // If something new in the USART
    usart_reset_status(SHL_USART);
    if (usart_read_char(SHL_USART, &c) == USART_SUCCESS)
    {
      // if this is not the quit char
      if (c != QUIT_APPEND)
      {
        // Echo the char.
        print_char(SHL_USART, c);
        // Add it to the file.
        file_putc(c);
        // if it is a carriage return.
        if (c == CR)
        {
          // Echo line feed.
          print_char(SHL_USART, LF);
          // Add line feed to the file.
          file_putc(LF);
        }
      }
      // Quit char received.
      else
      {
        // Exit the append function.
        break;
      }
    }
  }
}


/*! \brief Sets up USART for shell.
 *
 * \param pba_hz The current module frequency.
 */
static void init_shl_rs232(long pba_hz)
{
  // GPIO map for USART.
  static const gpio_map_t SHL_USART_GPIO_MAP =
  {
    {SHL_USART_RX_PIN, SHL_USART_RX_FUNCTION},
    {SHL_USART_TX_PIN, SHL_USART_TX_FUNCTION}
  };

  // Options for USART.
  static const usart_options_t SHL_USART_OPTIONS =
  {
    .baudrate = SHL_USART_BAUDRATE,
    .charlength = 8,
    .paritytype = USART_NO_PARITY,
    .stopbits = USART_1_STOPBIT,
    .channelmode = USART_NORMAL_CHMODE
  };

  // Set up GPIO for SHL_USART, size of the GPIO map is 2 here.
  gpio_enable_module(SHL_USART_GPIO_MAP,
                     sizeof(SHL_USART_GPIO_MAP) / sizeof(SHL_USART_GPIO_MAP[0]));

  // Initialize it in RS232 mode.
  usart_init_rs232(SHL_USART, &SHL_USART_OPTIONS, pba_hz);
}

//!!! stuff from sd_mmc_example.c
// Software wait
void wait()
{
  volatile int i;
  for(i = 0 ; i < 5000; i++);
}

// Software wait (longer)
void wait2()
{
  volatile long i;
  for(i = 0 ; i < 100000; i++);
}

/* interrupt handler to notify if the Data reception from flash is
 * over, in this case lunch the Memory(ram_buffer) to USART transfer and
 * disable interrupt*/
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void pdca_int_handler(void)
{
  // Disable all interrupts.
  Disable_global_interrupt();

  // Disable interrupt channel.
  pdca_disable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);

  sd_mmc_read_close_PDCA();//unselects the SD/MMC memory.
  wait();
  // Disable unnecessary channel
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_TX);
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_RX);

  // Enable all interrupts.
  Enable_global_interrupt();

  end_of_transfer = TRUE;
}


/*! \brief Initializes SD/MMC resources: GPIO, SPI and SD/MMC.
 */
static void sd_mmc_resources_init(void)
{
  // GPIO pins used for SD/MMC interface
  static const gpio_map_t SD_MMC_SPI_GPIO_MAP =
  {
    {SD_MMC_SPI_SCK_PIN,  SD_MMC_SPI_SCK_FUNCTION },  // SPI Clock.
    {SD_MMC_SPI_MISO_PIN, SD_MMC_SPI_MISO_FUNCTION},  // MISO.
    {SD_MMC_SPI_MOSI_PIN, SD_MMC_SPI_MOSI_FUNCTION},  // MOSI.
    {SD_MMC_SPI_NPCS_PIN, SD_MMC_SPI_NPCS_FUNCTION},  // Chip Select NPCS.
  };

  // SPI options.
  spi_options_t spiOptions =
  {
    .reg          = SD_MMC_SPI_NPCS,
    .baudrate     = SD_MMC_SPI_MASTER_SPEED,  // Defined in conf_sd_mmc.h.
    .bits         = SD_MMC_SPI_BITS,          // Defined in conf_sd_mmc.h.
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 1,
    .spi_mode     = 0,
    .fdiv         = 0,
    .modfdis      = 1
  };

  // Assign I/Os to SPI.
  gpio_enable_module(SD_MMC_SPI_GPIO_MAP,
                     sizeof(SD_MMC_SPI_GPIO_MAP) / sizeof(SD_MMC_SPI_GPIO_MAP[0]));

  // Initialize as master.
  spi_initMaster(SD_MMC_SPI, &spiOptions);

  // Set SPI selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(SD_MMC_SPI, 0, 0, 0);

  // Enable SPI module.
  spi_enable(SD_MMC_SPI);

  // Initialize SD/MMC driver with SPI clock (PBA).
  sd_mmc_init(spiOptions, PBA_HZ);
}


/*! \brief Initialize PDCA (Peripheral DMA Controller A) resources for the SPI transfer and start a dummy transfer
 */
void local_pdca_init(void)
{
  // this PDCA channel is used for data reception from the SPI
  pdca_channel_options_t pdca_options_SPI_RX ={ // pdca channel options

    .addr = ram_buffer,
    // memory address. We take here the address of the string dummy_data. This string is located in the file dummy.h

    .size = 512,                        // transfer counter: here the size of the string
    .r_addr = NULL,                     // next memory address after 1st transfer complete
    .r_size = 0,                        // next transfer counter not used here
    .pid = AVR32_PDCA_CHANNEL_USED_RX,  // select peripheral ID - data are on reception from SPI1 RX line
    .mode = PDCA_MODE_BYTE,             // select size of the transfer: 8,16,32 bits
  };

  // this channel is used to activate the clock of the SPI by sending a dummy variables
  pdca_channel_options_t pdca_options_SPI_TX ={ // pdca channel options

    .addr = (void *)&dummy_data,
    // memory address. We take here the address of the string dummy_data. This string is located in the file dummy.h

    .pid = AVR32_PDCA_CHANNEL_USED_TX,  // select peripheral ID - data are on reception from SPI1 RX line
    .size = 512,                        // transfer counter: here the size of the string
    .r_addr = NULL,                     // next memory address after 1st transfer complete
    .r_size = 0,                        // next transfer counter not used here
    .mode = PDCA_MODE_BYTE,             // select size of the transfer: 8,16,32 bits
  };

  // Init PDCA transmission channel
  pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_TX, &pdca_options_SPI_TX);

  // Init PDCA Reception channel
  pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_RX, &pdca_options_SPI_RX);

  //! \brief Enable pdca transfer interrupt when completed
  INTC_register_interrupt(&pdca_int_handler, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1);  // pdca_channel_spi1_RX = 0

}
//!!!

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
  U8 i, j;
  U16 file_size;
  Fs_index sav_index;
  static Fs_index mark_index;
  const char *part_type;
  
//!!!
  U8 sd_just_inserted;
//!!!

  // Switch to external oscillator 0.
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Initialize RS232 shell text output.
  init_shl_rs232(FOSC0);

//!!! stuff from sd_mmc_example.c
  // Initialize Interrupt Controller
  INTC_init_interrupts();

  // Initialize SD/MMC driver resources: GPIO, SPI and SD/MMC.
  sd_mmc_resources_init();

  // Enable all interrupts.
  Enable_global_interrupt();
//!!!  

  // reset vars
  cmd = FALSE;
  cmd_type = CMD_NONE;
  first_ls = TRUE;
  // reset all navigators
  nav_reset();

//!!!
  wait2();  //wait a bit for card to be ready
  
  if ( sd_mmc_mem_check() == OK )
  {
    sd_just_inserted = TRUE;	
  }
  else
  {
    sd_just_inserted = FALSE;
  }
//!!!
  
  // always loop
  while (TRUE)
  {

//!!! card inserted/removed code
    if ( sd_mmc_mem_check() != OK )
    {
      sd_just_inserted = TRUE;
      cmd = FALSE;
      cmd_type = CMD_NONE;
      first_ls = TRUE;
      // reset all navigators
      nav_reset();
      wait2();  //debounce	  
      print(SHL_USART, "\nCard removed!\n"); 	  
      while ( sd_mmc_mem_check() != OK );		  
    }

    if ( sd_just_inserted == TRUE )
    {
      print(SHL_USART, "\nCard inserted!\n"); 		
      wait2();  //debounce
		  
      // Save current nav position.
      sav_index = nav_getindex();
      // For all available drives :
      for (i = 0; i < nav_drive_nb(); i++)
      {
        // Select drive.
        nav_drive_set(i);
        // Try to mount.
        if (nav_partition_mount())
        {
          // Display memory name.
          print(SHL_USART, mem_name(i));
          // Display drive letter name.
          print(SHL_USART, " (");
          print_char(SHL_USART, 'A' + i);
          // Display partition space.
          print(SHL_USART, ":)\n"); 
          print(SHL_USART, "Partition Space: ");      
          print_ulong(SHL_USART, nav_partition_space() << FS_SHIFT_B_TO_SECTOR);
          print(SHL_USART, " Bytes\n");      
          // Display free space.
          print(SHL_USART, "     Free Space: ");      
          print_ulong(SHL_USART, nav_partition_freespace() << FS_SHIFT_B_TO_SECTOR);
          print(SHL_USART, " Bytes\n");
        }
      }
      // Restore nav position.
      nav_gotoindex(&sav_index);		
      sd_just_inserted = FALSE;

      // Display the prompt
      print(SHL_USART, "\n");    
      print(SHL_USART, MSG_PROMPT); 	  
    }
//!!!
	
    // While a usable user command on RS232 isn't received, build it
    if (!cmd)
    {
      fat_example_build_cmd();
    }
    // perform the command
    else
    {
      switch (cmd_type)
      {
      // this is a "mount" command
      case CMD_MOUNT:
        // Get drive number
        i = par_str1[0] - 'a';
        // If drive doesn't exist
        if (i >= nav_drive_nb())
        {
          // Display error message.
          print(SHL_USART, MSG_ER_DRIVE);
        }
        else
        {
          // Reset all navigators.
          nav_reset();
          // Select the desired drive.
          nav_drive_set(i);
          // Try to mount it.
          if (!nav_partition_mount())
          {
            // Display error message.
            print(SHL_USART, MSG_ER_MOUNT);
            // Retry to mount at next "ls".
            first_ls = TRUE;
          }
          else
          {
            // Clear flag, no need to remount at next "ls".
            first_ls = FALSE;
          }
        }
        break;
      // this is a "fat" information command
      case CMD_FAT:
        // Regarding the partition type :
        switch (nav_partition_type())
        {
        case FS_TYPE_FAT_12:
          // Display FAT12.
          part_type = "Drive uses FAT12\n";
          break;
        case FS_TYPE_FAT_16:
          // Display FAT16.
          part_type = "Drive uses FAT16\n";
          break;
        case FS_TYPE_FAT_32:
          // Display FAT32.
          part_type = "Drive uses FAT32\n";
          break;
        default:
          // Display error message.
          part_type = "Drive uses an unknown partition type\n";
          break;
        }
        print(SHL_USART, part_type);
        break;
      // this is a "ls" command
      case CMD_LS:
        // Check if params are correct or mount needed.
        if (nav_drive_get() >= nav_drive_nb() || first_ls)
        {
          first_ls = FALSE;
          // Reset navigators .
          nav_reset();
          // Use the last drive available as default.
          nav_drive_set(nav_drive_nb() - 1);
          // Mount it.
          nav_partition_mount();
        }
        // Get the volume name
        nav_dir_name((FS_STRING)str_buff, MAX_FILE_PATH_LENGTH);
        // Display general informations (drive letter and current path)
        print(SHL_USART, "\nVolume is ");
        print_char(SHL_USART, 'A' + nav_drive_get());
        print(SHL_USART, ":\nDir name is ");
        print(SHL_USART, str_buff);
        print_char(SHL_USART, LF);
        // Try to sort items by folders
        if (!nav_filelist_first(FS_DIR))
        {
          // Sort items by files
          nav_filelist_first(FS_FILE);
        }
        // Display items informations
        print(SHL_USART, "\tSize (Bytes)\tName\n");
        // reset filelist before to start the listing
        nav_filelist_reset();
        // While an item can be found
        while (nav_filelist_set(0, FS_FIND_NEXT))
        {
          // Get and display current item informations
          print(SHL_USART, (nav_file_isdir()) ? "Dir\t" : "   \t");
          print_ulong(SHL_USART, nav_file_lgt());
          print(SHL_USART, "\t\t");
          nav_file_name((FS_STRING)str_buff, MAX_FILE_PATH_LENGTH, FS_NAME_GET, TRUE);
          print(SHL_USART, str_buff);
          print_char(SHL_USART, LF);
        }
        // Display the files number
        print_ulong(SHL_USART, nav_filelist_nb(FS_FILE));
        print(SHL_USART, "  Files\n");
        // Display the folders number
        print_ulong(SHL_USART, nav_filelist_nb(FS_DIR));
        print(SHL_USART, "  Dir\n");
        break;
      // this is a "cd" command
      case CMD_CD:
        // get arg1 length
        i = strlen(par_str1);
        // Append the '/' char for the nav_setcwd to enter the chosen directory.
        if (par_str1[i - 1] != '/')
        {
          par_str1[i] = '/';
          par_str1[i + 1] = '\0';
        }
        // Try to to set navigator on arg1 folder.
        if (!nav_setcwd((FS_STRING)par_str1, TRUE, FALSE))
        {
          // Display error message.
          print(SHL_USART, MSG_ER_UNKNOWN_FILE);
        }
        break;
      // this is a "cat" command
      case CMD_CAT:
        // Try to to set navigator on arg1 file.
        if (!nav_setcwd((FS_STRING)par_str1, TRUE, FALSE))
        {
          // Display error message.
          print(SHL_USART, MSG_ER_UNKNOWN_FILE);
        }
        else
        {
          // Open the file.
          file_open(FOPEN_MODE_R);
          // While the end isn't reached
          while (!file_eof())
          {
          	// Display next char from file.
            print_char(SHL_USART, file_getc());
          }
          // Close the file.
          file_close();
          print_char(SHL_USART, LF);
        }
        break;
      // this is a "help" command
      case CMD_HELP:
        // Display help on USART
        print(SHL_USART, MSG_HELP);
        break;
      // this is a "mkdir" command
      case CMD_MKDIR:
        // Create the folder;
        nav_dir_make((FS_STRING)par_str1);
        break;
      // this is a "touch" command
      case CMD_TOUCH:
        // Create the file.
        nav_file_create((FS_STRING)par_str1);
        break;
      // this is a "rm" command
      case CMD_RM:
        // Save current nav position.
        sav_index = nav_getindex();
        // Try to to set navigator on arg1 folder or file.
        if (!nav_setcwd((FS_STRING)par_str1, TRUE, FALSE))
        {
          // Display error message.
          print(SHL_USART, MSG_ER_UNKNOWN_FILE);
        }
        // Try to delete the file
        else if (!nav_file_del(FALSE))
        {
          // Display error message.
          print(SHL_USART, MSG_ER_RM);
        }
        // Restore nav position.
        nav_gotoindex(&sav_index);
        break;
      // this is a "append"  command: Append a char to selected file.
      case CMD_APPEND:
        // Try to to set navigator on arg1 file.
        if (!nav_setcwd((FS_STRING)par_str1, TRUE, TRUE))
        {
          // Display error message.
          print(SHL_USART, MSG_ER_UNKNOWN_FILE);
        }
        else
        {
          // File exists, open it in append mode
          file_open(FOPEN_MODE_APPEND);
          // Append from USART
          fat_example_append_file();
          // Close the file
          file_close();
          // Display a line feed to user
          print_char(SHL_USART, LF);
        }
        break;
      // this is a "disk" command
      case CMD_NB_DRIVE:
        // Display number of drives.
        print(SHL_USART, "Nb Drive(s): ");
        print_char(SHL_USART, '0' + nav_drive_nb());
        print_char(SHL_USART, LF);
        break;
      // this is a "mark" command
      case CMD_SET_ID:
        // get marked index from current navigator location
        mark_index = nav_getindex();
        break;
      // this is a "goto" command
      case CMD_GOTO_ID:
        // set navigator to the marked index
        nav_gotoindex(&mark_index);
        break;
      // this is a "cp" command: Copy file to other location.
      case CMD_CP:
        // Try to set navigator on arg1 file.
        if (!nav_setcwd((FS_STRING)par_str1, TRUE, FALSE))
        {
          // Display error message.
          print(SHL_USART, MSG_ER_UNKNOWN_FILE);
        }
        else
        {
          // Get name of source to be used as same destination name.
          nav_file_name((FS_STRING)par_str1, MAX_FILE_PATH_LENGTH, FS_NAME_GET, TRUE);
          // Get file size.
          file_size = nav_file_lgtsector();
          // Mark source.
          nav_file_copy();
          // Save current source position.
          sav_index = nav_getindex();
          // Goto destination.
          nav_gotoindex(&mark_index);
          // Free space check.
          if (nav_partition_space() > file_size)
          {
            // Paste.
            nav_file_paste_start((FS_STRING)par_str1);
            // Restore previous nav position.
            nav_gotoindex(&sav_index);
            // Performs copy.
            while (nav_file_paste_state(FALSE) == COPY_BUSY);
          }
          // Restore previous nav position.
          nav_gotoindex(&sav_index);
        }
        break;
      // this is a "mv" command: Rename file.
      case CMD_MV:
        // Save current nav position.
        sav_index = nav_getindex();
        // Try to to set navigator on arg1 folder or file.
        if (!nav_setcwd((FS_STRING)par_str1, TRUE, FALSE))
        {
          // Display error message.
          print(SHL_USART, MSG_ER_UNKNOWN_FILE);
        }
        // Try to rename the file
        else if (!nav_file_rename((FS_STRING)par_str2))
        {
          // Display error message.
          print(SHL_USART, MSG_ER_MV);
        }
        // Restore nav position.
        nav_gotoindex(&sav_index);
        break;
      // this is a "df" command: Display free space information for all connected drives.
      case CMD_DF:
        // Save current nav position.
        sav_index = nav_getindex();
        // For all available drives :
        for (i = 0; i < nav_drive_nb(); i++)
        {
          // Select drive.
          nav_drive_set(i);
          // Try to mount.
          if (nav_partition_mount())
          {
            // Display memory name.
            print(SHL_USART, mem_name(i));
            // Display drive letter name.
            print(SHL_USART, " (");
            print_char(SHL_USART, 'A' + i);
            // Display partition space.
            print(SHL_USART, ":)\n");
            
        	//!!! modded output            
            print(SHL_USART, "Partition Space: ");      
            print_ulong(SHL_USART, nav_partition_space() << FS_SHIFT_B_TO_SECTOR);
            print(SHL_USART, " Bytes\n");      
            // Display free space.
            print(SHL_USART, "     Free Space: ");      
            print_ulong(SHL_USART, nav_partition_freespace() << FS_SHIFT_B_TO_SECTOR);
            print(SHL_USART, " Bytes\n");
            //!!!
          }
        }
        // Restore nav position.
        nav_gotoindex(&sav_index);
        break;
      // this is a "format" command : Format disk.
      case CMD_FORMAT:
        // Get disk number.
        i = par_str1[0] - 'a';
        // if drive number isn't valid
        if (i >= nav_drive_nb())
        {
          // Display error message.
          print(SHL_USART, MSG_ER_DRIVE);
        }
        else
        {
          // Get the current drive in the navigator.
          j = nav_drive_get();
          // Select drive to format.
          nav_drive_set(i);
          // If format fails.
          if (!fat_format(FS_FORMAT_DEFAULT))
          {
            // Display error message.
            print(SHL_USART, MSG_ER_FORMAT);
            // Return to the previous.
            nav_drive_set(j);
          }
          // Format succeds, if drives is the one we were navigating on
          else if (i == j)
          {
            // Reset the navigators.
            nav_reset();
            // Set current drive.
            nav_drive_set(j);
            // If partition mounting fails.
            if (!nav_partition_mount())
            {
              // Display error message.
              print(SHL_USART, MSG_ER_MOUNT);
              // this will be the first "ls"
              first_ls = TRUE;
            }
            else
            {
              // this won't be the first "ls", system is already mounted
              first_ls = FALSE;
            }
          }
          // Format succeds, restore previous navigator drive.
          else nav_drive_set(j);
        }
        break;
      // this is a "format32" command: Format disk as FAT 32 if possible.
      case CMD_FORMAT32:
        // Get disk number.
        i = par_str1[0] - 'a';
        // if drive number isn't valid
        if (i >= nav_drive_nb())
        {
          // Display error message.
          print(SHL_USART, MSG_ER_DRIVE);
        }
        else
        {
          // Get the current drive in the navigator.
          j = nav_drive_get();
          // Select drive to format.
          nav_drive_set(i);
          // If format fails.
          if (!fat_format(FS_FORMAT_FAT32))
          {
            // Display error message.
            print(SHL_USART, MSG_ER_FORMAT);
            // Return to the previous.
            nav_drive_set(j);
          }
          // Format succeds, if drives is the one we were navigating on
          else if (i == j)
          {
            // Reset the navigators.
            nav_reset();
            // Set current drive.
            nav_drive_set(j);
            // If partition mounting fails.
            if (!nav_partition_mount())
            {
              // Display error message.
              print(SHL_USART, MSG_ER_MOUNT);
              // this will be the first "ls"
              first_ls = TRUE;
            }
            else
            {
              // this won't be the first "ls", system is already mounted
              first_ls = FALSE;
            }
          }
          // Format succeds, restore previous navigator drive.
          else nav_drive_set(j);
        }
        break;
      // Unknown command.
      default:
        // Display error message.
        print(SHL_USART, MSG_ER_CMD_NOT_FOUND);
        break;
      }
      // Reset vars.
      cmd_type = CMD_NONE;
      cmd = FALSE;
      // Display prompt.
      print(SHL_USART, MSG_PROMPT);
    }
  }
}
