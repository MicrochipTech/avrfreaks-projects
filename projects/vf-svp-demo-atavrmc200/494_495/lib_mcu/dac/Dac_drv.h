//! @file Dac_drv.h,v
//!
//! Copyright (c) 2004 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the low level macros and definition for the DAC
//!
//! @version 1.2 pwm3-ac-ctrl-motor-0_0_6
//!
//! @todo
//! @bug

#ifndef DAC_DRV_H
#define DAC_DRV_H

//_____ I N C L U D E S ____________________________________________________


//! @defgroup DAC_module DAC Module
//! DAC Module
//! @{
//_____ M A C R O S ________________________________________________________

   //! @defgroup DAC_macros DAC Macros
   //! Low level macros that permit the use of the DAC of the product.
   //! @{

      //! @defgroup Turn_on_dac Turn on the DAC
      //! Turn on the DAC
      //! @{
#define Enable_dac()                         (DACON |= (1<<DAEN))
      //! @}

      //! @defgroup Turn_off_dac Turn Off the DAC
      //! Turn Off the DAC
      //! @{
#define Disable_dac()                        (DACON &= ~(1<<DAEN))
      //! @}

      //! @defgroup DAC_alignement_configuration DAC Alignement Configuration
      //! Configure the input data alignement
      //! @{
#define Right_adjust_dac_input()             (DACON  &= ~(1<<DALA))
#define Left_adjust_dac_input()              (DACON  |=  (1<<DALA))
      //! @}


      //! @defgroup DAC_output_configuration DAC Output Configuration
      //! Switch on or off the DAC output
      //! @{
#define Enable_dac_output()                  (DACON  |=  (1<<DAOE))
#define Disable_dac_output()                 (DACON  &= ~(1<<DAOE))
      //! @}

      //! @defgroup DAC_set_input_value DAC Set Input Value
      //! Setart the conversion input conversion value
      //! @{
#define Set_dac_8_bits(dac_in)               (DACH = (U8)(dac_in))
#define Set_dac_10_bits(dac_in)              (DACL = LOW((U16)(dac_in)), DACH = HIGH((U16)(dac_in)))
      //! @}
/* from IREENA */
#define Right_adjust_dac_result()            (DACON  &= ~(1<<DALA))
#define Left_adjust_dac_result()             (DACON  |=  (1<<DALA))

#define Enable_internal_vref()               (ADMUX  |=  ((1<<REFS1)|(1<<REFS0)) )
#define Enable_external_vref()               (ADMUX  &= ~((1<<REFS1)|(1<<REFS0)) )
#define Enable_vcc_vref()                    (ADMUX  &= ~(1<<REFS1), ADMUX |= (1<<REFS0) )

#define Output_dac_enable()                   (DACON |= (1<<DAOE))
#define Output_dac_disabled()                 (DACON &= ~(1<<DAOE))
/* end from IREENA */

//_____ D E F I N I T I O N S ______________________________________________

//_____ F U N C T I O N S __________________________________________________
   //! @defgroup DAC_low_level_functions DAC Low Level Fucntions
   //! DAC Low Level Functions
   //! @{

//! Configures the DAC accordingly to the DAC Define Configuration values.
void init_dac(void);

   //! @}

//! @}

#endif  // DAC_DRV_H
