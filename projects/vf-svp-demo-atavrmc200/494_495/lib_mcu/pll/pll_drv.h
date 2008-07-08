///! @file pll_drv.h,v
//!
//! Copyright (c) 2004 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the low level macros and definition for the PLL
//!
//! @version 1.3 pwm3-ac-ctrl-motor-0_0_6
//!
//! @todo
//! @bug

#ifndef PLL_DRV_H
#define PLL_DRV_H

//_____ I N C L U D E S ____________________________________________________

//! @defgroup PLL_module PLL Module 
//! PLL Module
//! @{
//_____ M A C R O S ________________________________________________________

   //! @defgroup PLL_macros PLL Macros 
   //! These functions allow to control the PLL
   //! @{

#define Start_pll_64_mega()                           (PLLCSR = 0x06)            //!< Start the PLL at 64MHz
#define Start_pll_32_mega()                           (PLLCSR = 0x02)            //!< Start the PLL at 32MHz
//! Start the PLL at the parameter frequency (only 64 or 32 MHz)
#define Start_pll(frequency)                                                  \
           (PLLCSR = ( (((frequency>>6)&1)<<2) | (1<<1) ) )
#define Is_pll_ready()                                (PLLCSR & (1<<PLOCK) )    //!< Return 1 if the PLL is ready
#define Wait_pll_ready()                              while (!(PLLCSR & (1<<PLOCK)))
#define Stop_pll()                                    (PLLCSR = 0x00)            //!< Stop the PLL

   //! @}        

//_____ D E F I N I T I O N S ______________________________________________

//_____ F U N C T I O N S __________________________________________________

//! @}        
#endif  // PLL_DRV_H
