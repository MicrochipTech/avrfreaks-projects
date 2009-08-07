/*
 * sd_reader.c
 * This file reads files from the SD-card using PDCA.
 *
 */

#include "pm.h"
#include "gpio.h"
#include "spi.h"
#include "conf_sd_mmc.h"
#include "sd_mmc.h"
#include "pdca.h"
#include "intc.h"

#include "board.h"
#include "conf_player.h"

#if BOARD==EVK1100
#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI1_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI1_TX
#elif BOARD==EVK1101
#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI_TX
#elif BOARD==MP3BOARD
#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI_TX
#else
#  error No known AVR32 board defined
#endif
#define AVR32_PDCA_CHANNEL_SPI_RX 2 // pdca channel 2.
#define AVR32_PDCA_CHANNEL_SPI_TX 3 // pdca channel 3.

// Dummy char table
const char dummy_data[] =
#include "dummy.h"
;

// PDCA channel pointers.
volatile avr32_pdca_channel_t* pdca_channelrx ;
volatile avr32_pdca_channel_t* pdca_channeltx ;

// GPIO pins used for SD/MMC interface
static const gpio_map_t SD_MMC_SPI_GPIO_MAP =
{
	{SD_MMC_SPI_SCK_PIN, SD_MMC_SPI_SCK_FUNCTION},	// SPI Clock.
	{SD_MMC_SPI_MISO_PIN, SD_MMC_SPI_MISO_FUNCTION},// MISO.
	{SD_MMC_SPI_MOSI_PIN, SD_MMC_SPI_MOSI_FUNCTION},// MOSI.
	{SD_MMC_SPI_NPCS_PIN, SD_MMC_SPI_NPCS_FUNCTION}	// Chip Select NPCS.
};

extern void CheckAndRead();
extern volatile unsigned char *writePtr;
extern volatile int bytesLeft;

volatile Bool in_transfer;

// Called whenever a sector has been read.
__attribute__((__interrupt__))
static void pdca_int_handler(void)
{
  // Disable interrupt.
  pdca_disable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);

  // Disable transfer.
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_TX);
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_RX);

  sd_mmc_read_close_PDCA();

  bytesLeft += 512;
  writePtr += 512;
  in_transfer = FALSE;

  CheckAndRead(); // Start a new transfer if room in the buffer.
}

void local_pdca_init(void)
{
  // Reception channel:
  pdca_channel_options_t pdca_options_SPI_RX = {
    .addr = NULL,						// We don't want to receive anything now.
    .size = 0,
    .r_addr = NULL,
    .r_size = 0,
    .pid = AVR32_PDCA_CHANNEL_USED_RX,	// Peripheral ID.
    .mode = 8							// Tranfer size; 8,16 or 32 bits.
  };

  // Transmision channel:
  pdca_channel_options_t pdca_options_SPI_TX = {
    .addr = (void *)&dummy_data,		// We send 0xff all the time while receiving.
    .size = 512,						// Sector size is always 512 bytes.
    .r_addr = (void *)&dummy_data,		// More dummy data.
    .r_size = 512,
    .pid = AVR32_PDCA_CHANNEL_USED_TX,	// Peripheral ID.
    .mode = 8							// Tranfer size; 8,16 or 32 bits.
  };

  pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_TX, &pdca_options_SPI_TX);
  pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_RX, &pdca_options_SPI_RX);

  // Enable PDCA interrupt:
  INTC_register_interrupt(&pdca_int_handler, AVR32_PDCA_IRQ_2, AVR32_INTC_INT1);
}

static void sd_mmc_resources_init(void)
{
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

void sd_reader_get_sector(U32 addr)
{
	in_transfer = TRUE;

    pdca_load_channel(AVR32_PDCA_CHANNEL_SPI_RX, writePtr, 512);
    pdca_load_channel(AVR32_PDCA_CHANNEL_SPI_TX, (void *)&dummy_data, 512);

    if(sd_mmc_read_open_PDCA(addr) == OK) // Select the correct sector.
    {
      spi_write(SD_MMC_SPI,0xFF);

      // Run the interrupt routine when the transfer is complete.
      pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);

      // Start the transfer.
      pdca_enable(AVR32_PDCA_CHANNEL_SPI_RX); // Enable RX PDCA transfer first,
      pdca_enable(AVR32_PDCA_CHANNEL_SPI_TX); // then TX PDCA transfer.
    }
}

void sd_reader_init()
{
	in_transfer = FALSE;

	gpio_enable_pin_pull_up(AVR32_PIN_PA14);
	gpio_enable_pin_pull_up(AVR32_PIN_PA15);
	gpio_enable_pin_pull_up(AVR32_PIN_PA16);
	gpio_enable_pin_pull_up(AVR32_PIN_PA25);
	gpio_enable_pin_pull_up(AVR32_PIN_PA27);

	//while(gpio_get_pin_value(SD_MMC_CARD_DETECT_PIN)); // Wait for card-detect trigger.

	// Initialize SD/MMC driver resources: GPIO, SPI and SD/MMC.
	sd_mmc_resources_init();

	// Initialize PDCA.
	local_pdca_init();

	while ( sd_mmc_mem_check() != OK );
}
