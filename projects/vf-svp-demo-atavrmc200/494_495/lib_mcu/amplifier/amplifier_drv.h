//! @file amplifier_drv.h,v
//!
//! Copyright (c) 2004 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the low level macros and definition for the
//! amplifiers
//!
//! @version 1.4 pwm3-ac-ctrl-motor-0_0_6
//!
//! @todo
//! @bug

#ifndef AMPLIFIER_DRV_H
#define AMPLIFIER_DRV_H

//_____ I N C L U D E S ____________________________________________________

//! @defgroup Amplifiers_modules Amplifiers Modules
//! Amplifiers Modules
//! @{
//_____ M A C R O S ________________________________________________________

   //! @defgroup Amplifiers_macros Amplifiers Macros
   //! Low level macros that permit the use of the Amplifiers of the product.
   //! @{

      //! @defgroup Amplifier0_macros Amplifier 0 Macros
      //! Low level macros that permit the use of the Amplifier 0 of the product.
      //! @{

   //! *** CONFIGURE THE AMPLIFIER 0 ***
   //! ****** AMP0CSR
   //! 7   AMP0EN    : 1 Enable the amplifier
   //! 6   AMP0IS    : 1 Shunt the amplifier input
   //! 5:4 AMP0G1:0  : Amplifier 0 gain selction bits
   //!               : 00 gain  5
   //!               : 01 gain 10 
   //!               : 10 gain 20 
   //!               : 11 gain 40 
   //! 3:2 Reserved  : Not used
   //! 1:0 AMP0TS1:0 : Amplifier 0 Trigger Source Bits
   //!               : 00 ADCclk/8
   //!               : 01 PSC0 
   //!               : 10 PSC1
   //!               : 11 PSC2
   //! ****** END AMP0CSR

         //! @defgroup Turn_on_amplifier0 Turn on the Amplifier 0
         //! Turn on the Amplifier 0
         //! @{
#define Enable_amp0()                         (AMP0CSR |= (1<<AMP0EN))  
#define Is_amp0_enabled()                     (AMP0CSR &  (1<<AMP0EN))
         //! @}

         //! @defgroup Amp0_input_shunt Amplifier0 Input shunt
         //! Configure the Amplifier 0 shunt Option
         //! @{
#define Enable_amp0_input_shunt()             (AMP0CSR |=  (1<<AMP0IS))
#define Disable_amp0_input_shunt()            (AMP0CSR &= ~(1<<AMP0IS)) 
         //! @}

         //! @defgroup Amplifier0_gain_configuration Amplifier 0 Gain Configuration
         //! Configure the value of the gain of the Amplifier 0
         //! @{
//! Amplifier 0 Gain  5 Configuration
#define Set_amp0_gain05()                                                      \
           (AMP0CSR &= ~(  (1<<AMP0G1) | (1<<AMP0G0) ) ) 
//! Amplifier 0 Gain 10 Configuration
#define Set_amp0_gain10()                                                     \
           (AMP0CSR &= ~(1<<AMP0G1),                                          \
            AMP0CSR |=  (1<<AMP0G0) )
//! Amplifier 0 Gain 20 Configuration
#define Set_amp0_gain20()                                                     \
           (AMP0CSR |=  (1<<AMP0G1),                                          \
            AMP0CSR &= ~(1<<AMP0G0) )
//! Amplifier 0 Gain 40 Configuration
#define Set_amp0_gain40()                                                     \
           (AMP0CSR |=  ((1<<AMP0G1) | (1<<AMP0G0)) ) 
         //! @}

         //! @defgroup Amplifier0_clock_selection Amplifier 0 Clock Selection
         //! Select the clock which will be used to clock the amplfier
         //! @{
//! Amplifier 0 clocked by ADCclk/8
#define Amp0_internal_clock()                                                 \
           (AMP0CSR &= ~(  (1<<AMP0TS1) | (1<<AMP0TS0) ) ) 
//! Amplifier 0 clocked by PSC0 trig signal
#define Amp0_psc0_clock()                                                     \
           (AMP0CSR &= ~(1<<AMP0TS1),                                         \
            AMP0CSR |=  (1<<AMP0TS0) )
//! Amplifier 0 clocked by PSC1 trig signal
#define Amp0_psc1_clock()                                                     \
           (AMP0CSR |=  (1<<AMP0TS1),                                         \
            AMP0CSR &= ~(1<<AMP0TS0) )
//! Amplifier 0 clocked by PSC2 trig signal
#define Amp0_psc2_clock()                                                     \
           (AMP0CSR |=  ((1<<AMP0TS1) | (1<<AMP0TS0)) ) 
         //! @}


         //! @defgroup Turn_off_amplifier0 Turn off the Amplifier 0
         //! Turn off the Amplifier 0
         //! @{
#define Disable_amp0()                         (AMP0CSR &= ~(1<<AMP0EN) )  
         //! @}

      //! @}

      //! @defgroup Amplifier1_macros Amplifier 1 Macros
      //! Low level macros that permit the use of the Amplifier 1 of the product.
      //! @{

   //! *** CONFIGURE THE AMPLIFIER 1 ***
   //! ****** AMP1CSR
   //! 7   AMP1EN    : 1 Enable the amplifier
   //! 6   AMP1IS    : 1 Shunt the amplifier input
   //! 5:4 AMP1G1:0  : Amplifier 1 gain selction bits
   //!               : 00 gain  5
   //!               : 01 gain 10 
   //!               : 10 gain 20 
   //!               : 11 gain 40 
   //! 3:2 Reserved  : Not used
   //! 1:0 AMP1TS1:0 : Amplifier 1 Trigger Source Bits
   //!               : 00 ADCclk/8
   //!               : 01 PSC0 
   //!               : 10 PSC1
   //!               : 11 PSC2
   //! ****** END AMP1CSR

         //! @defgroup Turn_on_amplifier1 Turn on the Amplifier 1
         //! Turn on the Amplifier 1
         //! @{
#define Enable_amp1()                         (AMP1CSR |= (1<<AMP1EN))  
#define Is_amp1_enabled()                     (AMP1CSR &  (1<<AMP1EN))
         //! @}

         //! @defgroup Amp1_input_shunt Amplifier1 Input shunt
         //! Configure the Amplifier 1 shunt Option
         //! @{
#define Enable_amp1_input_shunt()             (AMP1CSR |=  (1<<AMP1IS))
#define Disable_amp1_input_shunt()            (AMP1CSR &= ~(1<<AMP1IS)) 
         //! @}

         //! @defgroup Amplifier1_gain_configuration Amplifier 1 Gain Configuration
         //! Configure the value of the gain of the Amplifier 1
         //! @{
//! Amplifier 1 Gain  5 Configuration
#define Set_amp1_gain05()                                                      \
           (AMP1CSR &= ~(  (1<<AMP1G1) | (1<<AMP1G0) ) ) 
//! Amplifier 1 Gain 10 Configuration
#define Set_amp1_gain10()                                                     \
           (AMP1CSR &= ~(1<<AMP1G1),                                          \
            AMP1CSR |=  (1<<AMP1G0) )
//! Amplifier 1 Gain 20 Configuration
#define Set_amp1_gain20()                                                     \
           (AMP1CSR |=  (1<<AMP1G1),                                          \
            AMP1CSR &= ~(1<<AMP1G0) )
//! Amplifier 0 Gain 40 Configuration
#define Set_amp1_gain40()                                                     \
           (AMP1CSR |=  ((1<<AMP1G1) | (1<<AMP1G0)) ) 
         //! @}

         //! @defgroup Amplifier1_clock_selection Amplifier 1 Clock Selection
         //! Select the clock which will be used to clock the amplfier
         //! @{
//! Amplifier 1 clocked by ADCclk/8
#define Amp1_internal_clock()                                                 \
           (AMP1CSR &= ~(  (1<<AMP1TS1) | (1<<AMP1TS0) ) ) 
//! Amplifier 1 clocked by PSC0 trig signal
#define Amp1_psc0_clock()                                                     \
           (AMP1CSR &= ~(1<<AMP1TS1),                                         \
            AMP1CSR |=  (1<<AMP1TS0) )
//! Amplifier 1 clocked by PSC1 trig signal
#define Amp1_psc1_clock()                                                     \
           (AMP1CSR |=  (1<<AMP1TS1),                                         \
            AMP1CSR &= ~(1<<AMP1TS0) )
//! Amplifier 1 clocked by PSC2 trig signal
#define Amp1_psc2_clock()                                                     \
           (AMP1CSR |=  ((1<<AMP1TS1) | (1<<AMP1TS0)) ) 
         //! @}


         //! @defgroup Turn_off_amplifier1 Turn off the Amplifier 1
         //! Turn off the Amplifier 1
         //! @{
#define Disable_amp1()                         (AMP1CSR &= ~(1<<AMP1EN) )  
         //! @}

      //! @}

   //! @}


//_____ D E F I N I T I O N S ______________________________________________

//_____ F U N C T I O N S __________________________________________________
   //! @defgroup Amplifiers_low_level_functions Amplifiers Low Level Functions
   //! Amplifiers Low Level Functions
   //! @{

//! Configures the Amplifiers accordingly to the Amplifiers Define Configuration
//! values,
//! Take care that you have to select the ports which will be converted as
//! analog inputs thanks to the DIDR0 and DIDR1 registers.
//!
#ifdef USE_AMP0 //!< this define is set on config.h file
      //! @defgroup Amplifier0_low_level_function Amplifier 0 Low Level Functions
      //! Amplifier 0 Low Level Functions
      //! @{
void init_amp0(void);
      //! @}
#endif //USE_AMP0

#ifdef USE_AMP1 //!< this define is set on config.h file
      //! @defgroup Amplifier1_low_level_function Amplifier 1 Low Level Functions
      //! Amplifier 1 Low Level Functions
      //! @{
void init_amp1(void);
      //! @}
#endif //USE_AMP1

   //! @}

//! @}
#endif  // AMPLIFIER_DRV_H

