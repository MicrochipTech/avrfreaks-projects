/********************************************************
 Name          : main.c
 Author        : Marcus Wagnborg
 Copyright     : Copy and paste whatever you like...
 Description   : EVK1100 template
 NOTE		   : In order to get this to work properly, connect
 				 the DB9 of your GPS to a null modem adapter, and connect
 				 that to a Male-Male dapter, and plug that into the UART1 DB9
 				 on the EVK1100.  Yeah its a pain, but you got to cross Tx/Rx
 				 and get the right male-female connection.
 				 
 				 Make sure the GPS is spitting out NMEA at 4800 baud.
 **********************************************************/

// ========= INCLUDES =============

#include "board.h"
#include "compiler.h"
#include "dip204.h"
#include "intc.h"
#include "gpio.h"
#include "pm.h"
#include "spi.h"
#include "usart.h"
#include "m_interrupts.h"
#include "definitions.h"
#include "myFunctions.h"

// ========= GLOBALS =============
unsigned char TimeOut = 0;
struct currentPositionStruct CurrentPosition;
struct gpsStruct gps;

void runRoundRobin(void)
{
	while(1)
	{
		// When we get a new position fix
		if (CurrentPosition.parseGPSBuff )
		{
			enable_usart_interrupts(EXAMPLE_USART, 0, 0); // disable gps interrupt
			parseGPSBuff();	
			
			if(gps.gotGGA && gps.gotVTG)
			{
				gps.parseDone = 1;
			}
			CurrentPosition.parseGPSBuff = FALSE;
		}

		if(gps.parseDone && gps.gotGGA && gps.gotVTG)
		{
			gps.parseDone = 0;
			gps.gotGGA = 0;
			gps.gotVTG = 0;

			displayGPS();
		}
	}

}

// ============================ M A I N =================================
int main(void)
{
  static const gpio_map_t DIP204_SPI_GPIO_MAP =
  {
    {DIP204_SPI_SCK_PIN,  DIP204_SPI_SCK_FUNCTION },  // SPI Clock.
    {DIP204_SPI_MISO_PIN, DIP204_SPI_MISO_FUNCTION},  // MISO.
    {DIP204_SPI_MOSI_PIN, DIP204_SPI_MOSI_FUNCTION},  // MOSI.
    {DIP204_SPI_NPCS_PIN, DIP204_SPI_NPCS_FUNCTION}   // Chip Select NPCS.
  };
  
  static const gpio_map_t USART_GPIO_MAP =
      {
        {EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
        {EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
      };

  // USART options.
  static const usart_options_t USART_OPTIONS =
  {
    .baudrate     = 4800,
    .charlength   = 8,
    .paritytype   = USART_NO_PARITY,
    .stopbits     = USART_1_STOPBIT,
    .channelmode  = USART_NORMAL_CHMODE
  };

  // Switch the CPU main clock to oscillator 0
  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);

  // Disable all interrupts.
  Disable_global_interrupt();

  // init the interrupts
  INTC_init_interrupts();

  INTC_register_interrupt(&compare_irq_handler, AVR32_CORE_COMPARE_IRQ, AVR32_INTC_INT0);
  INTC_register_interrupt(&uart_interrupt, AVR32_USART1_IRQ, AVR32_INTC_INT0);
  
  
  // Enable all interrupts.
  Enable_global_interrupt();

  // add the spi options driver structure for the LCD DIP204
  spi_options_t spiOptions =
  {
    .reg          = DIP204_SPI_NPCS,
    .baudrate     = 1000000,
    .bits         = 8,
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 1,
    .spi_mode     = 0,
    .fdiv         = 0,
    .modfdis      = 1
  };
  
  // Assign I/Os to SPI
  gpio_enable_module(DIP204_SPI_GPIO_MAP,
                     sizeof(DIP204_SPI_GPIO_MAP) / sizeof(DIP204_SPI_GPIO_MAP[0]));

  // Initialize as master
  spi_initMaster(DIP204_SPI, &spiOptions);

  // Set selection mode: variable_ps, pcs_decode, delay
  spi_selectionMode(DIP204_SPI, 0, 0, 0);

  // Enable SPI
  spi_enable(DIP204_SPI);

  // setup chip registers
  spi_setupChipReg(DIP204_SPI, &spiOptions, FOSC0);

  /* initialize LCD */
  dip204_init(backlight_IO, TRUE);
  
  // Assign GPIO to USART.
  gpio_enable_module(USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

  // Initialize USART in RS232 mode.
  usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, FOSC0);
  enable_usart_interrupts(EXAMPLE_USART, 0, 1);	// enable usart 1 interrupt
  
  // Display default message.
  dip204_clear_display();
  dip204_set_cursor_position(4,1);
  dip204_write_string("IMPORTANT!");
  dip204_set_cursor_position(1,2);
  dip204_write_string("Connect GPS to UART1");
  dip204_set_cursor_position(1,3);
  dip204_write_string("Please Read Comment");
  dip204_set_cursor_position(1,4);
  dip204_write_string("at top of main.c");  
  dip204_set_cursor_position(1,2);
  dip204_hide_cursor();

  delay_ms(10000);
  dip204_clear_display();
  dip204_set_cursor_position(1,1);
  dip204_write_string("Warming Up!");
  
  runRoundRobin();
  
  while(1);
}
