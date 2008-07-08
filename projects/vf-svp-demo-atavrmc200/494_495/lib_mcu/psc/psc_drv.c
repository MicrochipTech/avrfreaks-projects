//! @file psc_drv.c,v
//!
//! Copyright (c) 2004 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the low level functions for the PSC
//!
//! @version 1.5 pwm3-ac-ctrl-motor-0_0_6 $Id: psc_drv.c,v 1.5 2005/09/23 13:45:16 ebouin Exp $
//!
//! @todo
//! @bug

//_____  I N C L U D E S ___________________________________________________
#include "config.h"
#include "lib_mcu/psc/psc_drv.h"
#include "lib_mcu/pll/pll_drv.h"

//_____ M A C R O S ________________________________________________________


//_____ P R I V A T E    D E C L A R A T I O N _____________________________


//_____ D E F I N I T I O N ________________________________________________


//_____ D E C L A R A T I O N ______________________________________________

#ifdef USE_PSC0 //!< this define is set on config.h file
//! Configures the PSC0 accordingly to the PSC0 Define Configuration values.
//!
//! @param None.
//!
//! @pre  - Define the configuration in config.h file
//!
//! @post - Set the psc0 comparison values
//!       - Start the PSC0 thanks to Start_psc0() macro
void init_psc0(void)
{
#   if (PSC0_PLL_FREQUENCY == 64)
       Start_pll_64_mega();
#   elif (PSC0_PLL_FREQUENCY == 32)
       Start_pll_32_mega();
#   elif (PSC0_PLL_FREQUENCY == 0)
#   else
#      error (PSC0_PLL_FREQUENCY should be 0, 32, or 64... See config.h file)
#   endif

#   if (PSC0_OUTPUT_SELECTION == 3)
       Enable_both_psc0_outputs();
#   elif (PSC0_OUTPUT_SELECTION == 2)
       Enable_pscout01();
#   elif (PSC0_OUTPUT_SELECTION == 1)
       Enable_pscout00();
#   elif (PSC0_OUTPUT_SELECTION == 0)
       Disable_both_psc0_outputs();
#   else
#      error (PSC0_OUTPUT_SELECTION should be 0, 1, 2, or 2... See config.h file)
#   endif

#   if (PSC0_ACTIVE_STATE == 1)
       Psc0_outputs_active_high();
#   elif (PSC0_ACTIVE_STATE == 0)
       Psc0_outputs_active_low();
#   else
#      error (PSC0_ACTIVE_STATE should be 0, or 1... See config.h file)
#   endif

#   if (PSC0_RAMP_MODE == 4)
       Psc0_in_4_ramps_mode();
#   elif (PSC0_RAMP_MODE == 2)
       Psc0_in_2_ramps_mode();
#   elif (PSC0_RAMP_MODE == 1)
       Psc0_in_1_ramp_mode();
#   elif (PSC0_RAMP_MODE == 0)
       Psc0_in_centered_aligne_mode();
#   else
#      error (PSC0_RAMP_MODE should be 0, 1, 3, or 4... See config.h file)
#   endif

#   if (PSC0_FIFTY_PERCENT_MODE == 1)
       Enable_psc0_fifty_percent_mode();
#   elif (PSC0_FIFTY_PERCENT_MODE == 0)
       Disable_psc0_fifty_percent_mode();
#   else
#      error (PSC0_FIFTY_PERCENT_MODE should be 0, or 1)
#   endif

#   if (PSC0_SYMETRICAL_FLANK_WIDTH_MODULATION == 1)
       Psc0_symetrical_flank_width_modulation();
#   elif (PSC0_SYMETRICAL_FLANK_WIDTH_MODULATION == 0)
       Psc0_end_of_cycle_flank_width_modulation();
#   else
#      error (PSC0_SYMETRICAL_FLANK_WIDTH_MODULATION should be 0, or 1... See config.h file)
#   endif

#   if (PSC0_AUTOLOCK_MODE == 1)
       Enable_psc0_autolock_mode();
#   elif (PSC0_AUTOLOCK_MODE == 0)
       Disable_psc0_autolock_mode();
#   else
#      error (PSC0_AUTOLOCK_MODE should be 0, or 1... See config.h file)
#   endif

#   if (PSC0_PRESCALER_DIVIDER == 64)
       Divide_psc0_input_clock_by_64();
#   elif (PSC0_PRESCALER_DIVIDER == 16)
       Divide_psc0_input_clock_by_16();
#   elif (PSC0_PRESCALER_DIVIDER == 4)
       Divide_psc0_input_clock_by_4();
#   elif (PSC0_PRESCALER_DIVIDER == 0)
       Disable_psc0_prescaler();
#   else
#      error (PSC0_PRESCALER_DIVIDER should be 0, 4, 16, or 64... See config.h file)
#   endif

#   if (PSC0_PLL_FREQUENCY == 64)
       Wait_pll_ready();
       Psc0_use_pll_clock();
#   elif (PSC0_PLL_FREQUENCY == 32)
       Wait_pll_ready();
       Psc0_use_pll_clock();
#   elif (PSC0_PLL_FREQUENCY == 0)
       Psc0_use_io_clock();
#   else
#      error (PSC0_PLL_FREQUENCY should be 0, 32, or 64... See config.h file)
#   endif
}


//! Update all the PSC0 comparison values accordingly to the four values
//! passed as Psc_comparison_values typedef.
//!
//! @param Comparison values via U16.
//!   psc0_comparison_values->deadtime0 (=) U16 deadtime0
//!   psc0_comparison_values->ontime0   (=) U16 ontime0
//!   psc0_comparison_values->deadtime1 (=) U16 deadtime1
//!   psc0_comparison_values->ontime1   (=) U16 ontime1
//!
void update_psc0 ( Psc_comparison_values *psc0_comparison_values )
{
    Lock_psc0_compare_values();
    OCR0SAH = HIGH(psc0_comparison_values->deadtime0);
    OCR0SAL = LOW (psc0_comparison_values->deadtime0);
    OCR0RAH = HIGH(psc0_comparison_values->ontime0);
    OCR0RAL = LOW (psc0_comparison_values->ontime0);
    OCR0SBH = HIGH(psc0_comparison_values->deadtime1);
    OCR0SBL = LOW (psc0_comparison_values->deadtime1);
    OCR0RBH = HIGH(psc0_comparison_values->ontime1);
    OCR0RBL = LOW (psc0_comparison_values->ontime1);
    Update_psc0_compare_values();
}

//! Update half the PSC0 comparison values accordingly to the two values
//! passed as Psc_fifty_percent_comparison_values typedef.
//! This function is usefull if the PSC0 is configured in fifty percent mode
//!
//! @param Comparison values via U16.
//!   psc0_fifty_percent_comparison_values->deadtime1 (=) U16 deadtime1
//!   psc0_fifty_percent_comparison_values->ontime1   (=) U16 ontime1
//!
void update_psc0_fifty ( Psc_fifty_percent_comparison_values *psc0_fifty_percent_comparison_values )
{
    Lock_psc0_compare_values();
    OCR0SBH = HIGH(psc0_fifty_percent_comparison_values->deadtime1);
    OCR0SBL = LOW (psc0_fifty_percent_comparison_values->deadtime1);
    OCR0RBH = HIGH(psc0_fifty_percent_comparison_values->ontime1);
    OCR0RBL = LOW (psc0_fifty_percent_comparison_values->ontime1);
    Update_psc0_compare_values();
}
#endif // USE_PFC0


#ifdef USE_PSC1 //!< this define is set on config.h file
//! Configures the PSC1 accordingly to the PSC1 Define Configuration values.
//!
//! @param None.
//!
//! @pre  - Define the configuration in config.h file
//!
//! @post - Set the psc1 comparison values
//!       - Start the PSC1 thanks to Start_psc1() macro
void init_psc1(void)
{
#   if (PSC1_PLL_FREQUENCY == 64)
       Start_pll_64_mega();
#   elif (PSC1_PLL_FREQUENCY == 32)
       Start_pll_32_mega();
#   elif (PSC1_PLL_FREQUENCY == 0)
#   else
#      error (PSC1_PLL_FREQUENCY should be 0, 32, or 64... See config.h file)
#   endif

#   if (PSC1_OUTPUT_SELECTION == 3)
       Enable_both_psc1_outputs();
#   elif (PSC1_OUTPUT_SELECTION == 2)
       Enable_pscout11();
#   elif (PSC1_OUTPUT_SELECTION == 1)
       Enable_pscout10();
#   elif (PSC1_OUTPUT_SELECTION == 0)
       Disable_both_psc1_outputs();
#   else
#      error (PSC1_OUTPUT_SELECTION should be 0, 1, 2, or 2... See config.h file)
#   endif

#   if (PSC1_ACTIVE_STATE == 1)
       Psc1_outputs_active_high();
#   elif (PSC1_ACTIVE_STATE == 0)
       Psc1_outputs_active_low();
#   else
#      error (PSC1_ACTIVE_STATE should be 0, or 1... See config.h file)
#   endif

#   if (PSC1_RAMP_MODE == 4)
       Psc1_in_4_ramps_mode();
#   elif (PSC1_RAMP_MODE == 2)
       Psc1_in_2_ramps_mode();
#   elif (PSC1_RAMP_MODE == 1)
       Psc1_in_1_ramp_mode();
#   elif (PSC1_RAMP_MODE == 0)
       Psc1_in_centered_aligned_mode();
#   else
#      error (PSC1_RAMP_MODE should be 0, 1, 3, or 4... See config.h file)
#   endif

#   if (PSC1_FIFTY_PERCENT_MODE == 1)
       Enable_psc1_fifty_percent_mode();
#   elif (PSC1_FIFTY_PERCENT_MODE == 0)
       Disable_psc1_fifty_percent_mode();
#   else
#      error (PSC1_FIFTY_PERCENT_MODE should be 0, or 1)
#   endif

#   if (PSC1_SYMETRICAL_FLANK_WIDTH_MODULATION == 1)
       Psc1_symetrical_flank_width_modulation();
#   elif (PSC1_SYMETRICAL_FLANK_WIDTH_MODULATION == 0)
       Psc1_end_of_cycle_flank_width_modulation();
#   else
#      error (PSC1_SYMETRICAL_FLANK_WIDTH_MODULATION should be 0, or 1... See config.h file)
#   endif

#   if (PSC1_AUTOLOCK_MODE == 1)
       Enable_psc1_autolock_mode();
#   elif (PSC1_AUTOLOCK_MODE == 0)
       Disable_psc1_autolock_mode();
#   else
#      error (PSC1_AUTOLOCK_MODE should be 0, or 1... See config.h file)
#   endif

#   if (PSC1_PRESCALER_DIVIDER == 64)
       Divide_psc1_input_clock_by_64();
#   elif (PSC1_PRESCALER_DIVIDER == 16)
       Divide_psc1_input_clock_by_16();
#   elif (PSC1_PRESCALER_DIVIDER == 4)
       Divide_psc1_input_clock_by_4();
#   elif (PSC1_PRESCALER_DIVIDER == 0)
       Disable_psc1_prescaler();
#   else
#      error (PSC1_PRESCALER_DIVIDER should be 0, 4, 16, or 64... See config.h file)
#   endif

#   if (PSC1_PLL_FREQUENCY == 64)
       Wait_pll_ready();
       Psc1_use_pll_clock();
#   elif (PSC1_PLL_FREQUENCY == 32)
       Wait_pll_ready();
       Psc1_use_pll_clock();
#   elif (PSC1_PLL_FREQUENCY == 0)
       Psc1_use_io_clock();
#   else
#      error (PSC1_PLL_FREQUENCY should be 0, 32, or 64... See config.h file)
#   endif
}

//! Update all the PSC1 comparison values accordingly to the four values
//! passed as Psc_comparison_values typedef.
//!
//! @param Comparison values via U16.
//!   psc1_comparison_values->deadtime0 (=) U16 deadtime0
//!   psc1_comparison_values->ontime0   (=) U16 ontime0
//!   psc1_comparison_values->deadtime1 (=) U16 deadtime1
//!   psc1_comparison_values->ontime1   (=) U16 ontime1
//!
void update_psc1 ( Psc_comparison_values *psc1_comparison_values )
{
    Lock_psc1_compare_values();
    OCR1SAH = HIGH(psc1_comparison_values->deadtime0);
    OCR1SAL = LOW (psc1_comparison_values->deadtime0);
    OCR1RAH = HIGH(psc1_comparison_values->ontime0);
    OCR1RAL = LOW (psc1_comparison_values->ontime0);
    OCR1SBH = HIGH(psc1_comparison_values->deadtime1);
    OCR1SBL = LOW (psc1_comparison_values->deadtime1);
    OCR1RBH = HIGH(psc1_comparison_values->ontime1);
    OCR1RBL = LOW (psc1_comparison_values->ontime1);
    Update_psc1_compare_values();
}

//! Update half the PSC1 comparison values accordingly to the two values
//! passed as Psc_fifty_percent_comparison_values typedef.
//! This function is usefull if the PSC1 is configured in fifty percent mode
//!
//! @param Comparison values via U16.
//!   psc1_fifty_percent_comparison_values->deadtime1 (=) U16 deadtime1
//!   psc1_fifty_percent_comparison_values->ontime1   (=) U16 ontime1
//!
void update_psc1_fifty ( Psc_fifty_percent_comparison_values *psc1_fifty_percent_comparison_values )
{
    Lock_psc1_compare_values();
    OCR1SBH = HIGH(psc1_fifty_percent_comparison_values->deadtime1);
    OCR1SBL = LOW (psc1_fifty_percent_comparison_values->deadtime1);
    OCR1RBH = HIGH(psc1_fifty_percent_comparison_values->ontime1);
    OCR1RBL = LOW (psc1_fifty_percent_comparison_values->ontime1);
    Update_psc1_compare_values();
}
#endif // USE_PSC1

#ifdef USE_PSC2 //!< this define is set on config.h file
//! Configures the PSC2 accordingly to the PSC2 Define Configuration values.
//!
//! @param None.
//!
//! @pre  - Define the configuration in config.h file
//!
//! @post - Set the psc2 comparison values
//!       - Start the PSC2 thanks to Start_psc2() macro
void init_psc2(void)
{
#   if (PSC2_PLL_FREQUENCY == 64)
       Start_pll_64_mega();
#   elif (PSC2_PLL_FREQUENCY == 32)
       Start_pll_32_mega();
#   elif (PSC2_PLL_FREQUENCY == 0)
#   else
#      error (PSC2_PLL_FREQUENCY should be 0, 32, or 64... See config.h file)
#   endif

#   if (PSC2_OUTPUT_SELECTION == 3)
       Enable_both_psc2_outputs();
#   elif (PSC2_OUTPUT_SELECTION == 2)
       Enable_pscout21();
#   elif (PSC2_OUTPUT_SELECTION == 1)
       Enable_pscout20();
#   elif (PSC2_OUTPUT_SELECTION == 0)
       Disable_both_psc2_outputs();
#   else
#      error (PSC2_OUTPUT_SELECTION should be 0, 1, 2, or 2... See config.h file)
#   endif

#   if (PSC2_ACTIVE_STATE == 1)
       Psc2_outputs_active_high();
#   elif (PSC2_ACTIVE_STATE == 0)
       Psc2_outputs_active_low();
#   else
#      error (PSC2_ACTIVE_STATE should be 0, or 1... See config.h file)
#   endif

#   if (PSC2_RAMP_MODE == 4)
       Psc2_in_4_ramps_mode();
#   elif (PSC2_RAMP_MODE == 2)
       Psc2_in_2_ramps_mode();
#   elif (PSC2_RAMP_MODE == 1)
       Psc2_in_1_ramp_mode();
#   elif (PSC2_RAMP_MODE == 0)
       Psc2_in_centered_aligned_mode();
#   else
#      error (PSC2_RAMP_MODE should be 0, 1, 3, or 4... See config.h file)
#   endif

#   if (PSC2_FIFTY_PERCENT_MODE == 1)
       Enable_psc2_fifty_percent_mode();
#   elif (PSC2_FIFTY_PERCENT_MODE == 0)
       Disable_psc2_fifty_percent_mode();
#   else
#      error (PSC2_FIFTY_PERCENT_MODE should be 0, or 1)
#   endif

#   if (PSC2_SYMETRICAL_FLANK_WIDTH_MODULATION == 1)
       Psc2_symetrical_flank_width_modulation();
#   elif (PSC2_SYMETRICAL_FLANK_WIDTH_MODULATION == 0)
       Psc2_end_of_cycle_flank_width_modulation();
#   else
#      error (PSC2_SYMETRICAL_FLANK_WIDTH_MODULATION should be 0, or 1... See config.h file)
#   endif

#   if (PSC2_AUTOLOCK_MODE == 1)
       Enable_psc2_autolock_mode();
#   elif (PSC2_AUTOLOCK_MODE == 0)
       Disable_psc2_autolock_mode();
#   else
#      error (PSC2_AUTOLOCK_MODE should be 0, or 1... See config.h file)
#   endif

#   if (PSC2_PRESCALER_DIVIDER == 64)
       Divide_psc2_input_clock_by_64();
#   elif (PSC2_PRESCALER_DIVIDER == 16)
       Divide_psc2_input_clock_by_16();
#   elif (PSC2_PRESCALER_DIVIDER == 4)
       Divide_psc2_input_clock_by_4();
#   elif (PSC2_PRESCALER_DIVIDER == 0)
       Disable_psc2_prescaler();
#   else
#      error (PSC2_PRESCALER_DIVIDER should be 0, 4, 16, or 64... See config.h file)
#   endif

#   if (PSC2_PLL_FREQUENCY == 64)
       Wait_pll_ready();
       Psc2_use_pll_clock();
#   elif (PSC2_PLL_FREQUENCY == 32)
       Wait_pll_ready();
       Psc2_use_pll_clock();
#   elif (PSC2_PLL_FREQUENCY == 0)
       Psc2_use_io_clock();
#   else
#      error (PSC2_PLL_FREQUENCY should be 0, 32, or 64... See config.h file)
#   endif
}

//! Update all the PSC2 comparison values accordingly to the four values
//! passed as Psc_comparison_values typedef.
//!
//! @param Comparison values via U16.
//!   psc2_comparison_values->deadtime0 (=) U16 deadtime0
//!   psc2_comparison_values->ontime0   (=) U16 ontime0
//!   psc2_comparison_values->deadtime1 (=) U16 deadtime1
//!   psc2_comparison_values->ontime1   (=) U16 ontime1
//!
void update_psc2 ( Psc_comparison_values *psc2_comparison_values )
{
    Lock_psc2_compare_values();
    OCR2SAH = HIGH(psc2_comparison_values->deadtime0);
    OCR2SAL = LOW (psc2_comparison_values->deadtime0);
    OCR2RAH = HIGH(psc2_comparison_values->ontime0);
    OCR2RAL = LOW (psc2_comparison_values->ontime0);
    OCR2SBH = HIGH(psc2_comparison_values->deadtime1);
    OCR2SBL = LOW (psc2_comparison_values->deadtime1);
    OCR2RBH = HIGH(psc2_comparison_values->ontime1);
    OCR2RBL = LOW (psc2_comparison_values->ontime1);
    Update_psc2_compare_values();
}

//! Update half the PSC2 comparison values accordingly to the two values
//! passed as Psc_fifty_percent_comparison_values typedef.
//! This function is usefull if the PSC2 is configured in fifty percent mode
//!
//! @param Comparison values via U16.
//!   psc2_fifty_percent_comparison_values->deadtime1 (=) U16 deadtime1
//!   psc2_fifty_percent_comparison_values->ontime1   (=) U16 ontime1
//!
void update_psc2_fifty ( Psc_fifty_percent_comparison_values *psc2_fifty_percent_comparison_values )
{
    Lock_psc2_compare_values();
    OCR2SBH = HIGH(psc2_fifty_percent_comparison_values->deadtime1);
    OCR2SBL = LOW (psc2_fifty_percent_comparison_values->deadtime1);
    OCR2RBH = HIGH(psc2_fifty_percent_comparison_values->ontime1);
    OCR2RBL = LOW (psc2_fifty_percent_comparison_values->ontime1);
    Update_psc2_compare_values();
}
#endif // USE_PSC2
