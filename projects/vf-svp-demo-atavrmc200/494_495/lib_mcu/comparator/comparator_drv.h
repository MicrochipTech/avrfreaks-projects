//! @file comparator_drv.h,v
//!
//! Copyright (c) 2004 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the low level macros and definition for the
//! comparators
//!
//! @version 1.3 pwm3-ac-ctrl-motor-0_0_6
//!
//! @todo
//! @bug

#ifndef COMPARATOR_DRV_H
#define COMPARATOR_DRV_H

//_____ I N C L U D E S ____________________________________________________

//! @defgroup Comparators_modules Comparators Modules
//! Comparators Modules
//! @{
//_____ M A C R O S ________________________________________________________

   //! @defgroup Comparators_macros Comparators Macros
   //! Low level macros that permit the use of the Comparators of the product.
   //! @{

      //! @defgroup Comparator0_macros Comparator 0 Macros
      //! Low level macros that permit the use of the Comparator 0 of the product.
      //! @{

   //! *** CONFIGURE THE COMPARATOR 0 ***
   //! ****** AC0CON
   //! 7   AC0EN     : 1 Enable the comparator
   //! 6   AC0IE     : 1 Analog Comparator Interrupt Enable
   //! 5:4 AC0IS1:0  : Analog comparator Interrupt selection
   //!               : 00 interrupt on output toggle
   //!               : 01 reserved
   //!               : 10 interrupt on output falling edge
   //!               : 11 interrupt on output rising edge
   //! 3 Reserved    : Not used
   //! 2:0 AC0M2:0   : Comparator 0 Multiplexer register
   //!               : 000 Vref/6.40V 
   //!               : 001 Vref/3.20V
   //!               : 010 Vref/2.13V
   //!               : 011 Vref/1.60V
   //!               : 100 Analog Comparator Negative Input pin (ACMPM pin)
   //!               : 101 DAC result
   //!               : 110 reserved
   //!               : 111 reserved
   //! ****** END AC0CON

         //! @defgroup Turn_on_comparator0 Turn on the Comparator 0
         //! Turn on the Comparator 0
         //! @{
#define Enable_comparator0()                          (AC0CON |= (1<<AC0EN) )  
         //! @}

         //! @defgroup Comparator0_interrupt_enable Comparator 0 Interrupt Enable
         //! Enable or Disable Comparator 0 Interrupt
         //! @{
#define Enable_comparator0_interrupt()                (AC0CON |=  (1<<AC0IE) )
#define Disable_comparator0_interrupt()               (AC0CON &= ~(1<<AC0IE) ) 
         //! @}

         //! @defgroup Comparator0_gain_configuration Comparator 0 Gain Configuration
         //! Configure the value of the gain of the Comparator 0
         //! @{
         //! Comparator 0 interrupt on output toggle
#define Comparator0_it_on_output_toggle()                                     \
           (AC0CON &= ~(  (1<<AC0IS1) | (1<<AC0IS0) ) ) 
//! Comparator 0 interrupt on_output falling edge
#define Comparator0_it_on_output_falling_edge()                               \
           (AC0CON |=  (1<<AC0IS1),                                           \
            AC0CON &= ~(1<<AC0IS0) )
//! Comparator 0 interrupt on_output rising edge
#define Comparator0_it_on_output_rising_edge()                                \
           (AC0CON |=  ((1<<AC0IS1) | (1<<AC0IS0)) ) 
         //! @}

         //! @defgroup Comparator0_negative_input_selection Comparator 0 Negative Input Selection
         //! Select the Negative Input signal of the Comparator 0
         //! @{
//! Set the negative input comparator 0 as Vref/6.40V
#define Set_comparator0_ni_vref_6_40()                                        \
           (AC0CON &= ~( (1<<AC0M2) | (1<<AC0M1) | (1<<AC0M0) ) ) 
//! Set Vref/3.20V as the negative input comparator 0
#define Set_comparator0_ni_vref_3_20()                                        \
           (AC0CON &= ~( (1<<AC0M2) | (1<<AC0M1) |  (0<<AC0M0) ),             \
            AC0CON |=  ( (0<<AC0M2) | (0<<AC0M1) |  (1<<AC0M0) ) ) 
//! Set Vref/2.13V as the negative input comparator 0
#define Set_comparator0_ni_vref_2_13()                                        \
           (AC0CON &= ~( (1<<AC0M2) | (0<<AC0M1) |  (1<<AC0M0) ),             \
            AC0CON |=  ( (0<<AC0M2) | (1<<AC0M1) |  (0<<AC0M0) ) ) 
//! Set Vref/1.60V as the negative input comparator 0
#define Set_comparator0_ni_vref_1_60()                                        \
           (AC0CON &= ~( (1<<AC0M2) | (0<<AC0M1) |  (0<<AC0M0) ),             \
            AC0CON |=  ( (0<<AC0M2) | (1<<AC0M1) |  (1<<AC0M0) ) ) 
//! Set the ACMPM pin as the negative input comparator 0
#define Set_comparator0_ni_acmp()                                             \
           (AC0CON &= ~( (0<<AC0M2) | (1<<AC0M1) |  (1<<AC0M0) ),             \
            AC0CON |=  ( (1<<AC0M2) | (0<<AC0M1) |  (0<<AC0M0) ) ) 
//! Set the DAC output as the negative input comparator 0
#define Set_comparator0_ni_dac()                                              \
           (AC0CON &= ~( (0<<AC0M2) | (1<<AC0M1) |  (0<<AC0M0) ),             \
            AC0CON |=  ( (1<<AC0M2) | (0<<AC0M1) |  (1<<AC0M0) ) ) 
         //! @}

         //! @defgroup Turn_off_comparator0 Turn off the Comparator 0
         //! Turn off the Comparator 0
         //! @{
#define Disable_comparator0()                         (AC0CON &= ~(1<<AC0EN) )  
         //! @}

      //! @}


      //! @defgroup Comparator1_macros Comparator 1 Macros
      //! Low level macros that permit the use of the Comparator 1 of the product.
      //! @{

   //! *** CONFIGURE THE COMPARATOR 1 ***
   //! ****** AC1CON
   //! 7   AC1EN     : 1 Enable the comparator
   //! 6   AC1IE     : 1 Analog Comparator Interrupt Enable
   //! 5:4 AC1IS1:0  : Analog comparator Interrupt selection
   //!               : 00 interrupt on output toggle
   //!               : 01 reserved
   //!               : 10 interrupt on output falling edge
   //!               : 11 interrupt on output rising edge
   //! 3   AC1IC     : 1 Interrupt Capture Enable
   //! 2:0 AC1M2:0   : Comparator 1 Multiplexer register
   //!               : 000 Vref/6.40V 
   //!               : 001 Vref/3.20V
   //!               : 010 Vref/2.13V
   //!               : 011 Vref/1.60V
   //!               : 100 Analog Comparator Negative Input pin (ACMPM pin)
   //!               : 101 DAC result
   //!               : 110 reserved
   //!               : 111 reserved
   //! ****** END AC1CON

         //! @defgroup Turn_on_comparator1 Turn on the Comparator 1
         //! Turn on the Comparator 1
         //! @{
#define Enable_comparator1()                          (AC1CON |= (1<<AC1EN) )  
         //! @}

         //! @defgroup Comparator1_interrupt_enable Comparator 1 Interrupt Enable
         //! Enable or Disable Comparator 1 Interrupt
         //! @{
#define Enable_comparator1_interrupt()                (AC1CON |=  (1<<AC1IE) )
#define Disable_comparator1_interrupt()               (AC1CON &= ~(1<<AC1IE) ) 
         //! @}

         //! @defgroup Comparator1_gain_configuration Comparator 1 Gain Configuration
         //! Configure the value of the gain of the Comparator 1
         //! @{
//! Comparator 1 interrupt on output toggle
#define Comparator1_it_on_output_toggle()                                     \
           (AC1CON &= ~(  (1<<AC1IS1) | (1<<AC1IS0) ) ) 
//! Comparator 1 interrupt on_output falling edge
#define Comparator1_it_on_output_falling_edge()                               \
           (AC1CON |=  (1<<AC1IS1),                                           \
            AC1CON &= ~(1<<AC1IS0) )
//! Comparator 1 interrupt on_output rising edge
#define Comparator1_it_on_output_rising_edge()                                \
           (AC1CON |=  ((1<<AC1IS1) | (1<<AC1IS0)) ) 
         //! @}

         //! @defgroup Comparator1_interrupt_capture Comparator 1 Interrupt Capture
         //! Enable or Disable input capture of the Timer/Counter 1 on analog comparator
         //! event
         //! @{
#define Enable_comparator1_interrupt_capture()        (AC1CON |=  (1<<AC1ICE) )
#define Disable_comparator1_interrupt_capture()       (AC1CON &= ~(1<<AC1ICE) ) 
         //! @}

         //! @defgroup Comparator1_negative_input_selection Comparator 1 Negative Input Selection
         //! Select the Negative Input signal of the Comparator 1
         //! @{
//! Set the negative input comparator 1 as Vref/6.40V
#define Set_comparator1_ni_vref_6_40()                                        \
           (AC1CON &= ~( (1<<AC1M2) | (1<<AC1M1) | (1<<AC1M0) ) ) 
//! Set Vref/3.20V as the negative input comparator 1
#define Set_comparator1_ni_vref_3_20()                                        \
           (AC1CON &= ~( (1<<AC1M2) | (1<<AC1M1) |  (0<<AC1M0) ),             \
            AC1CON |=  ( (0<<AC1M2) | (0<<AC1M1) |  (1<<AC1M0) ) ) 
//! Set Vref/2.13V as the negative input comparator 1
#define Set_comparator1_ni_vref_2_13()                                        \
           (AC1CON &= ~( (1<<AC1M2) | (0<<AC1M1) |  (1<<AC1M0) ),             \
            AC1CON |=  ( (0<<AC1M2) | (1<<AC1M1) |  (0<<AC1M0) ) ) 
//! Set Vref/1.60V as the negative input comparator 1
#define Set_comparator1_ni_vref_1_60()                                        \
           (AC1CON &= ~( (1<<AC1M2) | (0<<AC1M1) |  (0<<AC1M0) ),             \
            AC1CON |=  ( (0<<AC1M2) | (1<<AC1M1) |  (1<<AC1M0) ) ) 
//! Set the ACMPM pin as the negative input comparator 1
#define Set_comparator1_ni_acmp()                                             \
           (AC1CON &= ~( (0<<AC1M2) | (1<<AC1M1) |  (1<<AC1M0) ),             \
            AC1CON |=  ( (1<<AC1M2) | (0<<AC1M1) |  (0<<AC1M0) ) ) 
//! Set the DAC output as the negative input comparator 1
#define Set_comparator1_ni_dac()                                              \
           (AC1CON &= ~( (0<<AC1M2) | (1<<AC1M1) |  (0<<AC1M0) ),             \
            AC1CON |=  ( (1<<AC1M2) | (0<<AC1M1) |  (1<<AC1M0) ) ) 
         //! @}

         //! @defgroup Turn_off_comparator1 Turn off the Comparator 1
         //! Turn off the Comparator 1
         //! @{
#define Disable_comparator1()                         (AC1CON &= ~(1<<AC1EN) )  
         //! @}

      //! @}

      //! @defgroup Comparator2_macros Comparator 2 Macros
      //! Low level macros that permit the use of the Comparator 2 of the product.
      //! @{

   //! *** CONFIGURE THE COMPARATOR 2 ***
   //! ****** AC2CON
   //! 7   AC2EN     : 1 Enable the comparator
   //! 6   AC2IE     : 1 Analog Comparator Interrupt Enable
   //! 5:4 AC2IS1:0  : Analog comparator Interrupt selection
   //!               : 00 interrupt on output toggle
   //!               : 01 reserved
   //!               : 10 interrupt on output falling edge
   //!               : 11 interrupt on output rising edge
   //! 3 Reserved    : Not used
   //! 2:0 AC2M2:0   : Comparator 1 Multiplexer register
   //!               : 000 Vref/6.40V 
   //!               : 001 Vref/3.20V
   //!               : 010 Vref/2.13V
   //!               : 011 Vref/1.60V
   //!               : 100 Analog Comparator Negative Input pin (ACMPM pin)
   //!               : 101 DAC result
   //!               : 110 reserved
   //!               : 111 reserved
   //! ****** END AC2CON

         //! @defgroup Turn_on_comparator2 Turn on the Comparator 2
         //! Turn on the Comparator 2
         //! @{
#define Enable_comparator2()                          (AC2CON |= (1<<AC2EN) )  
         //! @}

         //! @defgroup Comparator2_interrupt_enable Comparator 2 Interrupt Enable
         //! Enable or Disable Comparator 2 Interrupt
         //! @{
#define Enable_comparator2_interrupt()                (AC2CON |=  (1<<AC2IE) )
#define Disable_comparator2_interrupt()               (AC2CON &= ~(1<<AC2IE) ) 
         //! @}

         //! @defgroup Comparator2_gain_configuration Comparator 2 Gain Configuration
         //! Configure the value of the gain of the Comparator 2
         //! @{
//! Comparator 2 interrupt on output toggle
#define Comparator2_it_on_output_toggle()                                     \
           (AC2CON &= ~(  (1<<AC2IS1) | (1<<AC2IS0) ) ) 
//! Comparator 2 interrupt on_output falling edge
#define Comparator2_it_on_output_falling_edge()                               \
           (AC2CON |=  (1<<AC2IS1),                                           \
            AC2CON &= ~(1<<AC2IS0) )
//! Comparator 2 interrupt on_output rising edge
#define Comparator2_it_on_output_rising_edge()                                \
           (AC2CON |=  ((1<<AC2IS1) | (1<<AC2IS0)) ) 
         //! @}

         //! @defgroup Comparator2_negative_input_selection Comparator 2 Negative Input Selection
         //! Select the Negative Input signal of the Comparator 2
         //! @{
//! Set the negative input comparator 2 as Vref/6.40V
#define Set_comparator2_ni_vref_6_40()                                        \
           (AC2CON &= ~( (1<<AC2M2) | (1<<AC2M1) | (1<<AC2M0) ) ) 
//! Set Vref/3.20V as the negative input comparator 2
#define Set_comparator2_ni_vref_3_20()                                        \
           (AC2CON &= ~( (1<<AC2M2) | (1<<AC2M1) |  (0<<AC2M0) ),             \
            AC2CON |=  ( (0<<AC2M2) | (0<<AC2M1) |  (1<<AC2M0) ) ) 
//! Set Vref/2.13V as the negative input comparator 2
#define Set_comparator2_ni_vref_2_13()                                        \
           (AC2CON &= ~( (1<<AC2M2) | (0<<AC2M1) |  (1<<AC2M0) ),             \
            AC2CON |=  ( (0<<AC2M2) | (1<<AC2M1) |  (0<<AC2M0) ) ) 
//! Set Vref/1.60V as the negative input comparator 2
#define Set_comparator2_ni_vref_1_60()                                        \
           (AC2CON &= ~( (1<<AC2M2) | (0<<AC2M1) |  (0<<AC2M0) ),             \
            AC2CON |=  ( (0<<AC2M2) | (1<<AC2M1) |  (1<<AC2M0) ) ) 
//! Set the ACMPM pin as the negative input comparator 2
#define Set_comparator2_ni_acmp()                                             \
           (AC2CON &= ~( (0<<AC2M2) | (1<<AC2M1) |  (1<<AC2M0) ),             \
            AC2CON |=  ( (1<<AC2M2) | (0<<AC2M1) |  (0<<AC2M0) ) ) 
//! Set the DAC output as the negative input comparator 2
#define Set_comparator2_ni_dac()                                              \
           (AC2CON &= ~( (0<<AC2M2) | (1<<AC2M1) |  (0<<AC2M0) ),             \
            AC2CON |=  ( (1<<AC2M2) | (0<<AC2M1) |  (1<<AC2M0) ) ) 
         //! @}

         //! @defgroup Turn_off_comparator2 Turn off the Comparator 2
         //! Turn off the Comparator 2
         //! @{
#define Disable_comparator2()                         (AC2CON &= ~(1<<AC2EN) )  
         //! @}

      //! @}

   //! @}


//_____ D E F I N I T I O N S ______________________________________________

//_____ F U N C T I O N S __________________________________________________
   //! @defgroup Comparators_low_level_functions Comparators Low Level Functions
   //! Comparators Low Level Functions
   //! @{

//! Configures the Comparators accordingly to the Comparators Define Configuration
//! values,
//! Take care that you have to select the Analog comparator inputs as
//! analog inputs thanks to the DIDR0 and DIDR1 registers.
//!

      //! @defgroup Comparator0_low_level_functions Comparator 0 Low Level Functions
      //! Comparator 0 Low Level Functions
      //! @{
void init_comparator0(void);
      //! @}

      //! @defgroup Comparator1_low_level_functions Comparator 1 Low Level Functions
      //! Comparator 1 Low Level Functions
      //! @{
void init_comparator1(void);
      //! @}

      //! @defgroup Comparator2_low_level_functions Comparator 2 Low Level Functions
      //! Comparator 0 Low Level Functions
      //! @{
void init_comparator2(void);
      //! @}

   //! @}

//! @}
#endif  // COMPARATOR_DRV_H

