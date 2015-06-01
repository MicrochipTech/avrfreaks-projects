/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief 3D Cube Demo example use with 3D lib
 * This is an example of a 3D cube containing the logo AVR32
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with GPIO.
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

/*!
 * \mainpage
 * \section section1 Description
 *   This application is a 3D Implementation on UC3B \n
 *   It runs on an EVK1101 board with the add-on board. \n
 *   The main source files are :
 *
 *          - \ref application.c        Main loop
 *          - \ref 3dengine.c           3D Graphic Engine
 *          - \ref lcd_nokia.c          Low Level Layer for LCD display
 *
 *
 * \section section2 Configuration
 * The configuration of the application is stored in different files :
 *
 *          - \ref conf_3Dcube.h              Contains the configuration of the application
 *
 * \section section3 Main Peripheral Use
 *   - SPI is used for LCD Display
 *
 * \section section4 User Manual
 *   - See the associated application note available at http://www.atmel.com
 *
 * \subsection subsection1 List of Material
 *   - EVK1101 (UC3B) + EVK1101 Extension Board.
 *
 * \section section5 Supported Compiler
 *  - AVR32-GCC 4.2.2 - atmel 1.0.4 with AVR32 GNU TOOLCHAIN 2 beta 5.
 *  - IAR C/C++ Compiler for Atmel AVR32 2.21B/W32 (2.21.2.3)
 */

//_____  I N C L U D E S ___________________________________________________
#include <string.h>
#include <math.h>
#include "board.h"
#include "asf.h"

#include "CONF/evk1101_extension_board.h"

#include "lcd_nokia.h"

#include "3dengine.h"
#include "raster.h"
#include "CONF/conf_3Dcube.h"
#include "picture_objects.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

//_____ D E C L A R A T I O N S ____________________________________________
volatile avr32_spi_t *spi = LCD_NOKIA_SPI;  //!< SPI Instance declaration
volatile avr32_pm_t* pm = &AVR32_PM;        //!< PM  Instance declaration

int rotation_axes_cpt = 1;               //!< Rotation Axes Speed Reference
extern int Face_to_prints[3];               //!< Faces Reference
extern int Number_Face_to_print;            //!< Number of Face to Display

char RASTER[SCR_WIDE * SCR_HEIGHT];           //!< LCD Display Screen Content
unsigned char mycolor = White;                //!< Current Color


/*! \brief Initializes the MCU system clocks.
 */
static void init_sys_clocks(void)
{

volatile avr32_pm_t* pm = &AVR32_PM;

  pm_switch_to_osc0(pm, FOSC0, OSC0_STARTUP);  // Switch main clock to Osc0.

  /* Setup PLL0 on Osc0, mul=3 ,no divisor, lockcount=16, ie. 12Mhzx8 = 96MHz output */
  /*void pm_pll_setup(volatile avr32_pm_t* pm,
                  unsigned int pll,
                  unsigned int mul,
                  unsigned int div,
                  unsigned int osc,
                  unsigned int lockcount) {
   */
  pm_pll_setup(pm,
               0,   // use PLL0
               11,   // MUL=7 in the formula
               1,   // DIV=1 in the formula
               0,   // Sel Osc0/PLL0 or Osc1/PLL1
               16); // lockcount in main clock for the PLL wait lock

  /*
   This function will set a PLL option.
   *pm Base address of the Power Manager (i.e. &AVR32_PM)
   pll PLL number 0
   pll_freq Set to 1 for VCO frequency range 80-180MHz, set to 0 for VCO frequency range 160-240Mhz.
   pll_div2 Divide the PLL output frequency by 2 (this settings does not change the FVCO value)
   pll_wbwdisable 1 Disable the Wide-Bandith Mode (Wide-Bandwith mode allow a faster startup time and out-of-lock time). 0 to enable the Wide-Bandith Mode.
  */
  /* PLL output VCO frequency is 96MHz. We divide it by 2 with the pll_div2=1. This enable to get later main clock to 48MHz */
  pm_pll_set_option(pm, 0, 1, 1, 0);

  /* Enable PLL0 */
  /*
    void pm_pll_enable(volatile avr32_pm_t* pm,
                  unsigned int pll) {
  */
  pm_pll_enable(pm,0);

  /* Wait for PLL0 locked */
  pm_wait_for_pll0_locked(pm) ;

  /* Divide PBA clock by 2 from main clock (PBA clock = 48MHz/2 = 24MHz).
     Pheripheral Bus A clock divisor enable = 1
     Pheripheral Bus A select = 0
     Pheripheral Bus B clock divisor enable = 0
     Pheripheral Bus B select = 0
     High Speed Bus clock divisor enable = 0
     High Speed Bus select = 0
  */
  pm_cksel(pm, 1, 0, 0, 0, 0, 0);

  // Set one wait-state (WS) for flash controller. 0 WS access is up to 30MHz for HSB/CPU clock.
  // As we want to have 48MHz on HSB/CPU clock, we need to set 1 WS on flash controller.
  flashc_set_wait_state(1);

  pm_switch_to_clock(pm, AVR32_PM_MCSEL_PLL0); /* Switch main clock to 48MHz */
}


/*! \brief Initializes LCD_NOKIA resources: GPIO, SPI
 */
void lcd_nokia_resources_init(void);
void lcd_nokia_resources_init(void)
{
  static const gpio_map_t LCD_NOKIA_SPI_GPIO_MAP =
  {
    {LCD_NOKIA_SPI_SCK_PIN,  LCD_NOKIA_SPI_SCK_FUNCTION },  // SPI Clock.
    {LCD_NOKIA_SPI_MISO_PIN, LCD_NOKIA_SPI_MISO_FUNCTION},  // MISO.
    {LCD_NOKIA_SPI_MOSI_PIN, LCD_NOKIA_SPI_MOSI_FUNCTION},  // MOSI.
    {LCD_NOKIA_SPI_NPCS_PIN, LCD_NOKIA_SPI_NPCS_FUNCTION}   // Chip Select NPCS.
  };

  // SPI options.
  spi_options_t spiOptions =
  {
    .reg          = LCD_NOKIA_SPI_NPCS,
    .baudrate     = SPI_MASTER_SPEED,
    .bits         = 9,
    .spck_delay   = 0,
    .trans_delay  = 0,
    .stay_act     = 1,
    .spi_mode     = 0,
    .modfdis      = 1
  };

  // Assign I/Os to SPI.
  gpio_enable_module(LCD_NOKIA_SPI_GPIO_MAP,
                     sizeof(LCD_NOKIA_SPI_GPIO_MAP) / sizeof(LCD_NOKIA_SPI_GPIO_MAP[0]));

  // Initialize as master.
  spi_initMaster(LCD_NOKIA_SPI, &spiOptions);

  // Set selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(LCD_NOKIA_SPI, 0, 0, 0);

  // Enable SPI.
  spi_enable(LCD_NOKIA_SPI);

  // Initialize LCD with SPI clock Osc0.
  spi_setupChipReg(LCD_NOKIA_SPI, &spiOptions, FPBA_HZ);
}


int main(void)
{

	init_sys_clocks();
	
	delay_init(66000000);
	
  int i = 0;
  int teta = 0;
  int MYTSFMATR[9];

  // First init of the rotation matrix
  mrot_C8_FULL_FFIX(MYTSFMATR, teta, 0x5A82, 0x5A82, 0);

  // Initialize LCD_NOKIA resources: GPIO, SPI
  lcd_nokia_resources_init();


	// Controller initialization routine for LCD controller
	lcd_nokia_init();
 
  reset_Raster(RASTER);

  for (;;) {
        gpio_tgl_gpio_pin(AVR32_PIN_PX12);

        //modify rotation axes
      	switch (rotation_axes_cpt)
      	{
      		// Rotation Axes on x,y
          case 0:
              mrot_C8_FULL_FFIX(MYTSFMATR, teta, 0x5A82, 0x5A82, 0);
      		break;
      		// Rotation Axes on y,-z
          case 1 :
              mrot_C8_FULL_FFIX(MYTSFMATR, teta, 0, 0x5A82,-0x5A82);
      		break;
          // Rotation Axes on y,z
          case 2 :
              mrot_C8_FULL_FFIX(MYTSFMATR, teta, 0,0x5A82 , 0x5A82);
          break;
          // Rotation Axes on -x,y,z
          case 3 :
              mrot_C8_FULL_FFIX(MYTSFMATR, teta, -0x49E6, 0x49E6, 0x49E6);
          break;
          // Rotation Axes on x,z
          case 4 :
              mrot_C8_FULL_FFIX(MYTSFMATR, teta, 0x5A82, 0, 0x5A82);
          break;
          // Rotation Axes on x,-y,-z
          case 5 :
              mrot_C8_FULL_FFIX(MYTSFMATR, teta, 0x49E6, -0x49E6, -0x49E6);
          break;
          // Rotation Axes on -z
          case 6 :
              mrot_C8_FULL_FFIX(MYTSFMATR, teta, 0, 0, 0x8000);
          break;
          // Default : It is the Identity Matrix
          default:
            MYTSFMATR[0] = 1;MYTSFMATR[1] = 0;MYTSFMATR[2] = 0;
            MYTSFMATR[3] = 0;MYTSFMATR[4] = 1;MYTSFMATR[5] = 0;
            MYTSFMATR[6] = 0;MYTSFMATR[7] = 0;MYTSFMATR[8] = 1;
      		break;
        }
        
		// Teta Update: Update step between every rotation axes
        // Bigger is the step and Faster is the rotation
        teta +++;
        if(teta>=240)
        {
              teta=0;
        }

        // Display Screen
        SEND_RASTER_BUF(RASTER);

        // Clear Display Buffer for next compute step
        reset_Raster(RASTER);

        // Compute 3D Transformation with last MATRIX value
        // Input buffer : centers / Output buffer : tmpcenters
        transforme_C8_FFIX((OBJ3D_C8_FFIX *)&centers, (OBJ3D_C8_FFIX *)&tmpcenters, (int *)MYTSFMATR);

        // Compute zBuffer Transformation in order to optimize display
        update_faces();

        for( i = 0; i < Number_Face_to_print; i++)
        {
          // Test if it is a face to display so a Picture
          if (cube_logoavr32[Face_to_prints[i]])
          {
              transforme_C8_FFIX(cube_logoavr32[Face_to_prints[i]], tmpcube_logoavr32[Face_to_prints[i]], MYTSFMATR);
              draw_image_raster_bmp(tmpcube_logoavr32[Face_to_prints[i]], AVR32_COLOR, RASTER);
          }
          // Else Display Cube Transformation
          transforme_C8_FFIX((OBJ3D_C8_FFIX *)&of[Face_to_prints[i]], (OBJ3D_C8_FFIX *)&tmpof[Face_to_prints[i]], (int *)MYTSFMATR);
          draw_image_raster(&tmpof[Face_to_prints[i]], MESH_COLOR, RASTER);
         }

    }
}
