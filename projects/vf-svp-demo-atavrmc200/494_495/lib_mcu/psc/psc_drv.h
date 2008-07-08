//! @file psc_drv.h,v
//!
//! Copyright (c) 2004 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the low level macros and definition for the PSC use
//!
//! @version 1.6 pwm3-ac-ctrl-motor-0_0_6
//!
//! @todo
//! @bug

#ifndef PSC_DRV_H
#define PSC_DRV_H

//_____  I N C L U D E S ___________________________________________________
#include "lib_mcu/pll/pll_drv.h"

//! @defgroup PSC_modules PSC Modules
//! PSC Modules
//! @{

//_____ M A C R O S ________________________________________________________

   //! @defgroup PSC0_macros PSC0 Macros
   //! PSC0 Macros
   //! @{

      //! @defgroup PSC0_output_configuration PSC0 Output Configuration
      //! Select the PSC0 Outputs
      //! @{
#define Enable_pscout00()                             (PSOC0 |=  (1<<POEN0A) )  //!< Enable PSC0 Waveform Generator A
#define Disable_pscout00()                            (PSOC0 &= ~(1<<POEN0A) )  //!< Disable PSC0 Waveform Generator A

#define Enable_pscout01()                             (PSOC0 |=  (1<<POEN0B) )  //!< Enable PSC0 Waveform Generator B
#define Disable_pscout01()                            (PSOC0 &= ~(1<<POEN0B) )  //!< Disable PSC0 Waveform Generator B
//! Enable Both PSC0 Waveform Generator A and B
#define Enable_both_psc0_outputs()                                            \
           (PSOC0 |=  ((1<<POEN0A) | (1<<POEN0B)))
//! Disable Both PSC0 Waveform Generator A and B
#define Disable_both_psc0_outputs()                                           \
           (PSOC0 &= ~((1<<POEN0A) | (1<<POEN0B)))

#define Psc0_outputs_active_high()                    (PCNF0 |=  (1<<POP0) )    //!< PSC0 outputs are active low
#define Psc0_outputs_active_low()                     (PCNF0 &= ~(1<<POP0) )    //!< PSC0 outputs are active high
      //! @}

      //! @defgroup SPC0_mode_control_module PSC0 Mode Control
      //! PSC0C Mode ontrol
      //! @{
#define Enable_psc0_fifty_percent_mode()              (PCNF0 |=  (1<<PFIFTY0) )  //!< PSC0 is in 50% mode: Only OCR0RBH/L and OCR0SBH/L are used. They are duplicated in OCR0R/SAH/L during the update of OCR0BH/L
#define Disable_psc0_fifty_percent_mode()             (PCNF0 &= ~(1<<PFIFTY0) )  //!< OCR0R/SAH/L and OCR0R/SBH/L are fully independant

#define Enable_psc0_autolock_mode()                   (PCNF0 |=  (1<<PALOCK0) )  //!< OCR0R/SAH/L and OCR0R/SBH/L can be written without disturbing the PSC cycle. The update of these registers will be proceed at the end of the PSC cycle if the OCR0RB has been last written
#define Disable_psc0_autolock_mode()                  (PCNF0 &= ~(1<<PALOCK0) )  //!< The update will be procced according to PLOCK0 bit

#define Lock_psc0_compare_values()                    (PCNF0 |=  (1<<PLOCK0) )   //!< Take care that the lock is active only if you have disabled the autolock mode
#define Update_psc0_compare_values()                  (PCNF0 &= ~(1<<PLOCK0) )   //!< The compare registers will be updated with the content of OCR0ARH/L and OCR0BRH
      //! @}

      //! @defgroup PSC0_clock_control PSC0 Clock Control
      //! PSC0 Clock Control
      //! @{

         //! @defgroup PSC0_clock_selection PSC0 Clock Selection
         //! PSC0 Clock Selection
         //! @{
//! Connect the PSC0 input clock to the PLL
#define Psc0_use_pll_clock()                          (PCNF0 |= (1<<PCLKSEL0) )
//! Connect the PSC0 input clock to the I/O clock
#define Psc0_use_io_clock()                           (PCNF0 &= ~(1<<PCLKSEL0) )
//! Start the PLL at 64MHz and connect it to PSC0
#define Psc0_use_64_mega_pll_clock()                                          \
          (Start_pll_64_mega(),                                               \
           Wait_pll_ready(),                                                  \
           Psc0_use_pll_clock() )
//! Start the PLL at 32MHz and connect it to PSC0
#define Psc0_use_32_mega_pll_clock()                                          \
          (Start_pll_32_mega(),                                               \
           Wait_pll_ready(),                                                  \
           Psc0_use_pll_clock() )
         //! @}

         //! @defgroup PSC0_prescaler_control PSC0 Prescaler Control
         //! PSC0 Prescaler Control
         //! @{
//! No PSC0prescaler
#define Disable_psc0_prescaler()                                              \
           (PCTL0 &= ~(  (1<<PPRE01) | (1<<PPRE00) ) )
//! PSC0 clock is internally divided by 4
#define Divide_psc0_input_clock_by_4()                                        \
           (PCTL0 &= ~(1<<PPRE01),                                            \
            PCTL0 |=  (1<<PPRE00) )
//! PSC0 clock is internally divided by 16
#define Divide_psc0_input_clock_by_16()                                       \
           (PCTL0 |=  (1<<PPRE01),                                            \
            PCTL0 &= ~(1<<PPRE00) )
//! PSC0 clock is internally divided by 64
#define Divide_psc0_input_clock_by_64()                                       \
           (PCTL0 |=  ((1<<PPRE01) | (1<<PPRE00)) )
         //! @}

      //! @}

      //! @defgroup PSC0_ramp_mode_selection PSC0 Ramp Mode Selection
      //! PSC0 Ramp Mode Selection
      //! @{
      //! PSC0 is configured in one ramp mode, it means that the internal counter
      //! counts from 0 up to OCR0RB
#define Psc0_in_1_ramp_mode()                                                 \
           (PCNF0 &= ~(  (1<<PMODE01) | (1<<PMODE00) ) )
//! PSC0 is configured in two ramp mode, it means that the internal counter
//! counts from 0 up to OCR0RA then from 0 to OCR0RB
#define Psc0_in_2_ramps_mode()                                                \
           (PCNF0 &= ~(1<<PMODE01),                                           \
            PCNF0 |=  (1<<PMODE00) )
//! PSC0 is configured in for ramp mode, it means that the internal counter
//! counts from 0 up to OCR0SA then from 0 to OCR0RA then from 0 to OCR0SB then from 0 to OCR0RB
#define Psc0_in_4_ramps_mode()                                                \
           (PCNF0 |=  (1<<PMODE01),                                           \
            PCNF0 &= ~(1<<PMODE00) )
//! PSC0 is configured in for ramp mode, it means that the internal counter
//! counts from 0 up to OCR0RB then from OCR0RB downto 0
#define Psc0_in_centered_aligned_mode()                                       \
           (PCNF0 |=  (  (1<<PMODE01) | (1<<PMODE00) ) )
      //! @}

      //! @defgroup PSC0_flank_width_modulation_configuration PSC0 Flank Width Modulation Configuration
      //! Select PSC0 Flank Width Modulation Configuration
      //! @{
#define Psc0_symetrical_flank_width_modulation()      (PCTL0 |=  (1<<PBFM0) )     //!< PSC0 Flank width modulation operates on both OCR0RA and OCR0RB
#define Psc0_end_of_cycle_flank_width_modulation()    (PCTL0 &= ~(1<<PBFM0) )     //!< PSC0 Flank width modulation operates only on OCR0RB
      //! @}

      //! @defgroup PSC0_adc_amplifier_trigger_configuration PSC0 ADC Amplifier Trigger Configuration
      //! Select the PSC0 trigger configuration for the ADC or the amplifier
      //! @{
//! The PSC0 synchronization signal is sent to the ADC/Amplifier on waveform generator A leading edge
#define Psc0_synchro_on_waveform_generator_a_leading_edge()                   \
           (PSCO0 &= ~(  (1<<PSYNC01) | (1<<PSYNC00) ) )
//! The PSC0 synchronization signal is sent to the ADC/Amplifier on waveform generator A trailing edge
#define Psc0_synchro_on_waveform_generator_a_trailing_edge()                  \
           (PSCO0 &= ~(1<<PSYNC01),                                           \
            PSCO0 |=  (1<<PSYNC00) )
//! The PSC0 synchronization signal is sent to the ADC/Amplifier on waveform generator B leading edge
#define Psc0_synchro_on_waveform_generator_b_leading_edge()                   \
           (PSCO0 |=  (1<<PSYNC01),                                           \
            PSCO0 &= ~(1<<PSYNC00) )
//! The PSC0 synchronization signal is sent to the ADC/Amplifier on waveform generator B trailing edge
#define Psc0_synchro_on_waveform_generator_b_trailing_edge()                  \
           (PSCO0 |=  (  (1<<PSYNC01) | (1<<PSYNC00) ) )
      //! @}

      //! @defgroup PSC0_run_control PSC0 Run Control
      //! Turn On and Off the PSC0
      //! @{
#define Start_psc0()                                  (PCTL0 |=  (1<<PRUN0) )     //!< Turn On PSC0
#define Stop_psc0()                                   (PCTL0 &= ~(1<<PRUN0) )     //!< Turn Off PSC0
//! Turn Off PSC0 and the end of cycle
#define Psc0_complete_waveform_and_stop()             (PCTL0 |=  (1<<PCCYC0), \
                                                       PCTL0 &= ~(1<<PRUN0) )
#define Is_psc0_started()                             (PCTL0 & (1<<PRUN0) )
      //! @}

      //! @defgroup PSC0_fault_mode_configuration PSC0 Fault Mode Configuration
      //! PSC0 Fault Mode Configuration
      //! @{
#define Enable_pscout00_fault_mode()                  (PCTL0 |=  (1<<PAOC0A) )    //!< Fault input select to block A can act directly to PSCOUT00 output
#define Disable_pscout00_fault_mode()                 (PCTL0 &= ~(1<<PAOC0A) )    //!< No fault or retrigger management on PSCOUT00

#define Enable_pscout01_fault_mode()                  (PCTL0 |=  (1<<PAOC0B) )    //!< Fault input select to block A can act directly to PSCOUT00 output
#define Disable_pscout01_fault_mode()                 (PCTL0 &= ~(1<<PAOC0B) )    //!< No fault or retrigger management on PSCOUT01

#define Enable_psc0_autorun()                         (PCTL0 |=  (1<<PARUN0) )    //!< Setting PRUN2 in PCTL2 register or setting both PARUN2 in PCTL2 register and PRUN1 in PCTL1 register will make the two PSC start simultaneously
#define Disable_psc0_autorun()                        (PCTL0 &= ~(1<<PARUN0) )    //!< No synchronization between PSC2 and PSC0 exists
      //! @}

      //! @defgroup PSC0_interrupt_configuration PSC0 Interrupt Configuration
      //! PSC0 interrupt configuration
      //! @{
#define Enable_psc0_synchro_error_interrupt()         (PIM0 |=  (1<<PSEIE0) )     //!< An interrupt is generated when the PSEI0 bit is set
#define Disable_psc0_synchro_error_interrupt()        (PIM0 &= ~(1<<PSEIE0) )     //!< No interrupt is generated when the PSEI0 bit is set

#define Enable_psc0_external_event_a_interrupt()      (PIM0 |=  (1<<PEVE0A) )     //!< An external event which can generate a capture from retrigger/fault block A generates an interrupt
#define Disable_psc0_external_event_a_interrupt()     (PIM0 &= ~(1<<PEVE0A) )     //!< An external event which can generate a capture from retrigger/fault block A doesn't generate any interrupt

#define Enable_psc0_external_event_b_interrupt()      (PIM0 |=  (1<<PEVE0B) )     //!< An external event which can generate a capture from retrigger/fault block B generates an interrupt
#define Disable_psc0_external_event_b_interrupt()     (PIM0 &= ~(1<<PEVE0B) )     //!< An external event which can generate a capture from retrigger/fault block B doesn't generate any interrupt

#define Enable_psc0_end_of_cycle_interrupt()          (PIM0 |=  (1<<PEOPE0) )     //!< An interrupt is generated when PSC0 reaches the end of the whole cycle
#define Disable_psc0_end_of_cycle_interrupt()         (PIM0 &= ~(1<<PEOPE0) )     //!< No interrupt is generated when PSC0 reaches the end of the whole cycle
      //! @}

      //! @defgroup PSC0_interrupt_flag_control PSC0 Interrupt Flag Control
      //! PSC0 interrupt flag control
      //! @{
#define Is_psc0_synchro_error_interrupt_flag_set()    (PIFR0 & (1<<PSEI0) )       //!< Return 1 if the PSEI0 bit in PIFR0 is set
#define Clear_psc0_synchro_error_interrupt_flag()     (PIFR0 &= ~(1<<PSEI0) )     //!< Clear PSEI0 bit in PIFR0 register

#define Is_psc0_external_event_a_interrupt_flag_set() (PIFR0 & (1<<PEV0A) )       //!< Return 1 if the PEV0A bit in PIFR0 is set
#define Clear_psc0_external_event_a_interrupt_flag()  (PIFR0 &= ~(1<<PEV0A) )     //!< Clear PEV0A bit in PIFR0 register

#define Is_psc0_external_event_b_interrupt_flag_set() (PIFR0 & (1<<PEV0B) )       //!< Return 1 if the PEV0B bit in PIFR0 is set
#define Clear_psc0_external_event_b_interrupt_flag()  (PIFR0 &= ~(1<<PEV0B) )     //!< Clear PEV0B bit in PIFR0 register

#define Is_psc0_end_of_cycle_interrupt_flag_set()     (PIFR0 & (1<<PEOP2) )       //!< Return 1 if the PE0P2 bit in PIFR0 is set
#define Clear_psc0_end_of_cycle_interrupt_flag()      (PIFR0 &= ~(1<<PEOP2) )     //!< Clear PEOP2 bit in PIFR0 register
      //! @}

      //! @defgroup PSC0_comparison_value_initilization PSC0 Comparison Value Initialization
      //! This section allows to fully initilaize the comprison values
      //! @{
//! Init all PSC0 comparison values
#define Init_psc0_all_compare_values(PSC0_DEADTIME0, PSC0_ONTIME0, PSC0_DEADTIME1, PSC0_ONTIME1)  \
           (OCR0SAH = HIGH((U16)PSC0_DEADTIME0),                              \
            OCR0SAL = LOW ((U16)PSC0_DEADTIME0),                              \
            OCR0RAH = HIGH((U16)PSC0_ONTIME0),                                \
            OCR0RAL = LOW ((U16)PSC0_ONTIME0),                                \
            OCR0SBH = HIGH((U16)PSC0_DEADTIME1),                              \
            OCR0SBL = LOW ((U16)PSC0_DEADTIME1),                              \
            OCR0RBH = HIGH((U16)PSC0_ONTIME1),                                \
            OCR0RBL = LOW ((U16)PSC0_ONTIME1)   )
//! Init PSC0 fifty percent comparison values (usefull for 50% mode)
#define Init_psc0_fifty_percent_compare_values(PSC0_DEADTIME1, PSC0_ONTIME1)              \
           (OCR0SBH = HIGH((U16)PSC0_DEADTIME1),                              \
            OCR0SBL = LOW ((U16)PSC0_DEADTIME1),                              \
            OCR0RBH = HIGH((U16)PSC0_ONTIME1),                                \
            OCR0RBL = LOW ((U16)PSC0_ONTIME1)   )
      //! @}

   //! @}


   //! @defgroup PSC1_macros PSC1 Macros
   //! PSC1 Macros
   //! @{

      //! @defgroup PSC1_output_configuration PSC1 Output Configuration
      //! Select the PSC1 Outputs
      //! @{
#define Enable_pscout10()                             (PSOC1 |=  (1<<POEN1A) )  //!< Enable PSC1 Waveform Generator A
#define Disable_pscout10()                            (PSOC1 &= ~(1<<POEN1A) )  //!< Disable PSC1 Waveform Generator A

#define Enable_pscout11()                             (PSOC1 |=  (1<<POEN1B) )  //!< Enable PSC1 Waveform Generator B
#define Disable_pscout11()                            (PSOC1 &= ~(1<<POEN1B) )  //!< Disable PSC1 Waveform Generator B
//! Enable Both PSC1 Waveform Generator A and B
#define Enable_both_psc1_outputs()                                            \
           (PSOC1 |=  ((1<<POEN1A) | (1<<POEN1B)))
//! Disable Both PSC1 Waveform Generator A and B
#define Disable_both_psc1_outputs()                                           \
           (PSOC1 &= ~((1<<POEN1A) | (1<<POEN1B)))

#define Psc1_outputs_active_high()                    (PCNF1 |=  (1<<POP1) )    //!< PSC1 outputs are active low
#define Psc1_outputs_active_low()                     (PCNF1 &= ~(1<<POP1) )    //!< PSC1 outputs are active high
      //! @}

      //! @defgroup PSC1_mode_control_module PSC1 Mode Control
      //! PSC1 Mode Control
      //! @{
#define Enable_psc1_fifty_percent_mode()              (PCNF1 |=  (1<<PFIFTY1) )  //!< PSC1 is in 50% mode: Only OCR1RBH/L and OCR1SBH/L are used. They are duplicated in OCR1R/SAH/L during the update of OCR1BH/L
#define Disable_psc1_fifty_percent_mode()             (PCNF1 &= ~(1<<PFIFTY1) )  //!< OCR1R/SAH/L and OCR1R/SBH/L are fully independant

#define Enable_psc1_autolock_mode()                   (PCNF1 |=  (1<<PALOCK1) )  //!< OCR1R/SAH/L and OCR1R/SBH/L can be written without disturbing the PSC cycle. The update of these registers will be proceed at the end of the PSC cycle if the OCR1RB has been last written
#define Disable_psc1_autolock_mode()                  (PCNF1 &= ~(1<<PALOCK1) )  //!< The update will be procced according to PLOCK1 bit

#define Lock_psc1_compare_values()                    (PCNF1 |=  (1<<PLOCK1) )   //!< Take care that the lock is active only if you have disabled the autolock mode
#define Update_psc1_compare_values()                  (PCNF1 &= ~(1<<PLOCK1) )   //!< The compare registers will be updated with the content of OCR1AH/L and OCR1BH
      //! @}

      //! @defgroup PSC1_clock_control PSC1 Clock Control
      //! PSC1 Clock Control
      //! @{

         //! @defgroup PSC1_clock_selection PSC1 Clock Selection
         //! PSC1 Clock Selection
         //! @{
//! Connect the PSC1 input clock to the PLL
#define Psc1_use_pll_clock()                          (PCNF1 |= (1<<PCLKSEL0) )
//! Connect the PSC1 input clock to the I/O clock
#define Psc1_use_io_clock()                           (PCNF1 &= ~(1<<PCLKSEL0) )
//! Start the PLL at 64MHz and connect it to PSC1
#define Psc1_use_64_mega_pll_clock()                                          \
          (Start_pll_64_mega(),                                               \
           Wait_pll_ready(),                                                  \
           Psc1_use_pll_clock() )
//! Start the PLL at 32MHz and connect it to PSC1
#define Psc1_use_32_mega_pll_clock()                                          \
          (Start_pll_32_mega(),                                               \
           Wait_pll_ready(),                                                  \
           Psc1_use_pll_clock() )
         //! @}

         //! @defgroup PSC1_prescaler_control PSC1 Prescaler Control
         //! PSC1 Prescaler Control
         //! @{
//! No PSC1prescaler
#define Disable_psc1_prescaler()                                              \
           (PCTL1 &= ~(  (1<<PPRE11) | (1<<PPRE10) ) )
//! PSC1 clock is internally divided by 4
#define Divide_psc1_input_clock_by_4()                                        \
           (PCTL1 &= ~(1<<PPRE11),                                            \
            PCTL1 |=  (1<<PPRE10) )
//! PSC1 clock is internally divided by 16
#define Divide_psc1_input_clock_by_16()                                       \
           (PCTL1 |=  (1<<PPRE11),                                            \
            PCTL1 &= ~(1<<PPRE10) )
//! PSC1 clock is internally divided by 64
#define Divide_psc1_input_clock_by_64()                                       \
           (PCTL1 |=  ((1<<PPRE11) | (1<<PPRE10)) )
         //! @}

      //! @}

      //! @defgroup PSC1_ramp_mode_selection PSC1 Ramp Mode Selection
      //! PSC1 Ramp Mode Selection
      //! @{
      //! PSC1 is configured in one ramp mode, it means that the internal counter
      //! counts from 0 up to OCR1RB
#define Psc1_in_1_ramp_mode()                                                 \
           (PCNF1 &= ~(  (1<<PMODE11) | (1<<PMODE10) ) )
//! PSC1 is configured in two ramp mode, it means that the internal counter
//! counts from 0 up to OCR1RA then from 0 to OCR1RB
#define Psc1_in_2_ramps_mode()                                                \
           (PCNF1 &= ~(1<<PMODE11),                                           \
            PCNF1 |=  (1<<PMODE10) )
//! PSC1 is configured in for ramp mode, it means that the internal counter
//! counts from 0 up to OCR1SA then from 0 to OCR1RA then from 0 to OCR1SB then from 0 to OCR1RB
#define Psc1_in_4_ramps_mode()                                                \
           (PCNF1 |=  (1<<PMODE11),                                           \
            PCNF1 &= ~(1<<PMODE10) )
//! PSC1 is configured in for ramp mode, it means that the internal counter
//! counts from 0 up to OCR1RB then from OCR1RB downto 0
#define Psc1_in_centered_aligned_mode()                                       \
           (PCNF1 |=  (  (1<<PMODE11) | (1<<PMODE10) ) )
      //! @}

      //! @defgroup PSC1_flank_width_modulation_configuration PSC1 Flank Width Modulation Configuration
      //! Select PSC1 Flank Width Modulation Configuration
      //! @{
#define Psc1_symetrical_flank_width_modulation()      (PCTL1 |=  (1<<PBFM1) )     //!< PSC1 Flank width modulation operates on both OCR1RA and OCR1RB
#define Psc1_end_of_cycle_flank_width_modulation()    (PCTL1 &= ~(1<<PBFM1) )     //!< PSC1 Flank width modulation operates only on OCR1RB
      //! @}

      //! @defgroup PSC1_adc_amplifier_trigger_configuration PSC1 ADC Amplifier Trigger Configuration
      //! Select the PSC1 trigger configuration for the ADC or the amplifier
      //! @{
//! The PSC1 synchronization signal is sent to the ADC/Amplifier on waveform generator A leading edge
#define Psc1_synchro_on_waveform_generator_a_leading_edge()                   \
           (PSCO1 &= ~(  (1<<PSYNC11) | (1<<PSYNC10) ) )
//! The PSC1 synchronization signal is sent to the ADC/Amplifier on waveform generator A trailing edge
#define Psc1_synchro_on_waveform_generator_a_trailing_edge()                  \
           (PSCO1 &= ~(1<<PSYNC11),                                           \
            PSCO1 |=  (1<<PSYNC10) )
//! The PSC1 synchronization signal is sent to the ADC/Amplifier on waveform generator B leading edge
#define Psc1_synchro_on_waveform_generator_b_leading_edge()                   \
           (PSCO1 |=  (1<<PSYNC11),                                           \
            PSCO1 &= ~(1<<PSYNC10) )
//! The PSC1 synchronization signal is sent to the ADC/Amplifier on waveform generator B trailing edge
#define Psc1_synchro_on_waveform_generator_b_trailing_edge()                  \
           (PSCO1 |=  (  (1<<PSYNC11) | (1<<PSYNC10) ) )
      //! @}

      //! @defgroup PSC1_run_control PSC1 Run Control
      //! Turn On and Off the PSC1
      //! @{
#define Start_psc1()                                  (PCTL1 |=  (1<<PRUN1) )     //!< Turn On PSC1
#define Stop_psc1()                                   (PCTL1 &= ~(1<<PRUN1) )     //!< Turn Off PSC1
//! Turn Off PSC1 and the end of cycle
#define Psc1_complete_waveform_and_stop()             (PCTL1 |=  (1<<PCCYC1), \
                                                       PCTL1 &= ~(1<<PRUN1) )
#define Is_psc1_started()                             (PCTL1 & (1<<PRUN1) )
      //! @}

      //! @defgroup PSC1_fault_mode_configuration PSC1 Fault Mode Configuration
      //! PSC1 Fault Mode Configuration
      //! @{
#define Enable_pscout10_fault_mode()                  (PCTL1 |=  (1<<PAOC1A) )    //!< Fault input select to block A can act directly to PSCOUT00 output
#define Disable_pscout10_fault_mode()                 (PCTL1 &= ~(1<<PAOC1A) )    //!< No fault or retrigger management on PSCOUT00

#define Enable_pscout11_fault_mode()                  (PCTL1 |=  (1<<PAOC1B) )    //!< Fault input select to block A can act directly to PSCOUT00 output
#define Disable_pscout11_fault_mode()                 (PCTL1 &= ~(1<<PAOC1B) )    //!< No fault or retrigger management on PSCOUT01

#define Enable_psc1_autorun()                         (PCTL1 |=  (1<<PARUN1) )    //!< Setting PRUN0 in PCTL0 register or setting both PARUN0 in PCTL0 register and PRUN2 in PCTL2 register will make the two PSC start simultaneously
#define Disable_psc1_autorun()                        (PCTL1 &= ~(1<<PARUN1) )    //!< No synchronization between PSC2 and PSC1 exists
      //! @}

      //! @defgroup PSC1_interrupt_configuration PSC1 Interrupt Configuration
      //! PSC1 interrupt configuration
      //! @{
#define Enable_psc1_synchro_error_interrupt()         (PIM1 |=  (1<<PSEIE1) )     //!< An interrupt is generated when the PSEI0 bit is set
#define Disable_psc1_synchro_error_interrupt()        (PIM1 &= ~(1<<PSEIE1) )     //!< No interrupt is generated when the PSEI0 bit is set

#define Enable_psc1_external_event_a_interrupt()      (PIM1 |=  (1<<PEVE1A) )     //!< An external event which can generate a capture from retrigger/fault block A generates an interrupt
#define Disable_psc1_external_event_a_interrupt()     (PIM1 &= ~(1<<PEVE1A) )     //!< An external event which can generate a capture from retrigger/fault block A doesn't generate any interrupt

#define Enable_psc1_external_event_b_interrupt()      (PIM1 |=  (1<<PEVE1B) )     //!< An external event which can generate a capture from retrigger/fault block B generates an interrupt
#define Disable_psc1_external_event_b_interrupt()     (PIM1 &= ~(1<<PEVE1B) )     //!< An external event which can generate a capture from retrigger/fault block B doesn't generate any interrupt

#define Enable_psc1_end_of_cycle_interrupt()          (PIM1 |=  (1<<PEVE1A) )     //!< An interrupt is generated when PSC1 reaches the end of the whole cycle
#define Disable_psc1_end_of_cycle_interrupt()         (PIM1 &= ~(1<<PEVE1A) )     //!< No interrupt is generated when PSC1 reaches the end of the whole cycle
      //! @}

      //! @defgroup PSC1_interrupt_flag_control PSC1 Interrupt Flag Control
      //! PSC1 interrupt flag control
      //! @{
#define Is_psc1_synchro_error_interrupt_flag_set()    (PIFR1 & (1<<PSEI0) )       //!< Return 1 if the PSEI0 bit in PIFR1 is set
#define Clear_psc1_synchro_error_interrupt_flag()     (PIFR1 &= ~(1<<PSEI0) )     //!< Clear PSEI0 bit in PIFR1 register

#define Is_psc1_external_event_a_interrupt_flag_set() (PIFR1 & (1<<PEV0A) )       //!< Return 1 if the PEV0A bit in PIFR1 is set
#define Clear_psc1_external_event_a_interrupt_flag()  (PIFR1 &= ~(1<<PEV0A) )     //!< Clear PEV0A bit in PIFR1 register

#define Is_psc1_external_event_b_interrupt_flag_set() (PIFR1 & (1<<PEV0B) )       //!< Return 1 if the PEV0B bit in PIFR1 is set
#define Clear_psc1_external_event_b_interrupt_flag()  (PIFR1 &= ~(1<<PEV0B) )     //!< Clear PEV0B bit in PIFR1 register

#define Is_psc1_end_of_cycle_interrupt_flag_set()     (PIFR1 & (1<<PEOP1) )       //!< Return 1 if the PEOP1 bit in PIFR1 is set
#define Clear_psc1_end_of_cycle_interrupt_flag()      (PIFR1 &= ~(1<<PEOP1) )     //!< Clear PEOP1 bit in PIFR1 register
      //! @}

      //! @defgroup PSC1_comparison_value_initilization PSC1 Comparison Value Initialization
      //! This section allows to fully initilaize the comprison values
      //! @{
//! Init all PSC1 comparison values
#define Init_psc1_all_compare_values(PSC1_DEADTIME0, PSC1_ONTIME0, PSC1_DEADTIME1, PSC1_ONTIME1)  \
           (OCR1SAH = HIGH((U16)PSC1_DEADTIME0),                              \
            OCR1SAL = LOW ((U16)PSC1_DEADTIME0),                              \
            OCR1RAH = HIGH((U16)PSC1_ONTIME0),                                \
            OCR1RAL = LOW ((U16)PSC1_ONTIME0),                                \
            OCR1SBH = HIGH((U16)PSC1_DEADTIME1),                              \
            OCR1SBL = LOW ((U16)PSC1_DEADTIME1),                              \
            OCR1RBH = HIGH((U16)PSC1_ONTIME1),                                \
            OCR1RBL = LOW ((U16)PSC1_ONTIME1)   )
//! Init PSC1 fifty percent comparison values (usefull for 50% mode)
#define Init_psc1_fifty_percent_compare_values(PSC1_DEADTIME1, PSC1_ONTIME1)              \
           (OCR1SBH = HIGH((U16)PSC1_DEADTIME1),                              \
            OCR1SBL = LOW ((U16)PSC1_DEADTIME1),                              \
            OCR1RBH = HIGH((U16)PSC1_ONTIME1),                                \
            OCR1RBL = LOW ((U16)PSC1_ONTIME1)   )
      //! @}
   //! @}


   //! @defgroup PSC2_macros PSC2 Macros
   //! PSC2 Macros
   //! @{

      //! @defgroup PSC2_output_configuration PSC2 Output Configuration
      //! Select the PSC2 Outputs
      //! @{
#define Enable_pscout20()                             (PSOC2 |=  (1<<POEN2A) )  //!< Enable PSC2 Waveform Generator A
#define Disable_pscout20()                            (PSOC2 &= ~(1<<POEN2A) )  //!< Disable PSC2 Waveform Generator A

#define Enable_pscout21()                             (PSOC2 |=  (1<<POEN2B) )  //!< Enable PSC2 Waveform Generator B
#define Disable_pscout21()                            (PSOC2 &= ~(1<<POEN2B) )  //!< Disable PSC2 Waveform Generator B
//! Enable Both PSC2 Waveform Generator A and B
#define Enable_both_psc2_outputs()                                            \
           (PSOC2 |=  ((1<<POEN2A) | (1<<POEN2B)))
//! Disable Both PSC2 Waveform Generator A and B
#define Disable_both_psc2_outputs()                                           \
           (PSOC2 &= ~((1<<POEN2A) | (1<<POEN2B)))

#define Psc2_outputs_active_high()                    (PCNF2 |=  (1<<POP2) )    //!< PSC2 outputs are active low
#define Psc2_outputs_active_low()                     (PCNF2 &= ~(1<<POP2) )    //!< PSC2 outputs are active high
      //! @}

      //! @defgroup PSC2_mode_control_module PSC2 Mode Control
      //! PSC2 Mode Control
      //! @{
#define Enable_psc2_fifty_percent_mode()              (PCNF2 |=  (1<<PFIFTY2) )  //!< PSC2 is in 50% mode: Only OCR2RBH/L and OCR2SBH/L are used. They are duplicated in OCR2R/SAH/L during the update of OCR2BH/L
#define Disable_psc2_fifty_percent_mode()             (PCNF2 &= ~(1<<PFIFTY2) )  //!< OCR2R/SAH/L and OCR2R/SBH/L are fully independant

#define Enable_psc2_autolock_mode()                   (PCNF2 |=  (1<<PALOCK2) )  //!< OCR2R/SAH/L and OCR2R/SBH/L can be written without disturbing the PSC cycle. The update of these registers will be proceed at the end of the PSC cycle if the OCR2RB has been last written
#define Disable_psc2_autolock_mode()                  (PCNF2 &= ~(1<<PALOCK2) )  //!< The update will be procced according to PLOCK2 bit

#define Lock_psc2_compare_values()                    (PCNF2 |=  (1<<PLOCK2) )   //!< Take care that the lock is active only if you have disabled the autolock mode
#define Update_psc2_compare_values()                  (PCNF2 &= ~(1<<PLOCK2) )   //!< The compare registers will be updated with the content of OCR2AH/L and OCR2BH
      //! @}

      //! @defgroup PSC2_clock_control PSC2 Clock Control
      //! PSC2 Clock Control
      //! @{

         //! @defgroup PSC2_clock_selection PSC2 Clock Selection
         //! PSC2 Clock Selection
         //! @{
//! Connect the PSC2 input clock to the PLL
#define Psc2_use_pll_clock()                          (PCNF2 |= (1<<PCLKSEL0) )
//! Connect the PSC2 input clock to the I/O clock
#define Psc2_use_io_clock()                           (PCNF2 &= ~(1<<PCLKSEL0) )
//! Start the PLL at 64MHz and connect it to PSC2
#define Psc2_use_64_mega_pll_clock()                                          \
          (Start_pll_64_mega(),                                               \
           Wait_pll_ready(),                                                  \
           Psc2_use_pll_clock() )
//! Start the PLL at 32MHz and connect it to PSC2
#define Psc2_use_32_mega_pll_clock()                                          \
          (Start_pll_32_mega(),                                               \
           Wait_pll_ready(),                                                  \
           Psc2_use_pll_clock() )
         //! @}

         //! @defgroup PSC2_prescaler_control PSC2 Prescaler Control
         //! PSC2 Prescaler Control
         //! @{
//! No PSC2prescaler
#define Disable_psc2_prescaler()                                              \
           (PCTL2 &= ~(  (1<<PPRE21) | (1<<PPRE20) ) )
//! PSC2 clock is internally divided by 4
#define Divide_psc2_input_clock_by_4()                                        \
           (PCTL2 &= ~(1<<PPRE21),                                            \
            PCTL2 |=  (1<<PPRE20) )
//! PSC2 clock is internally divided by 16
#define Divide_psc2_input_clock_by_16()                                       \
           (PCTL2 |=  (1<<PPRE21),                                            \
            PCTL2 &= ~(1<<PPRE20) )
//! PSC2 clock is internally divided by 64
#define Divide_psc2_input_clock_by_64()                                       \
           (PCTL2 |=  ((1<<PPRE21) | (1<<PPRE20)) )
         //! @}

      //! @}

      //! @defgroup PSC2_ramp_mode_selection PSC2 Ramp Mode Selection
      //! PSC2 Ramp Mode Selection
      //! @{
      //! PSC2 is configured in one ramp mode, it means that the internal counter
      //! counts from 0 up to OCR2RB
#define Psc2_in_1_ramp_mode()                                                 \
           (PCNF2 &= ~(  (1<<PMODE21) | (1<<PMODE20) ) )
//! PSC2 is configured in two ramp mode, it means that the internal counter
//! counts from 0 up to OCR2RA then from 0 to OCR2RB
#define Psc2_in_2_ramps_mode()                                                \
           (PCNF2 &= ~(1<<PMODE21),                                           \
            PCNF2 |=  (1<<PMODE20) )
//! PSC2 is configured in for ramp mode, it means that the internal counter
//! counts from 0 up to OCR2SA then from 0 to OCR2RA then from 0 to OCR2SB then from 0 to OCR2RB
#define Psc2_in_4_ramps_mode()                                                \
           (PCNF2 |=  (1<<PMODE21),                                           \
            PCNF2 &= ~(1<<PMODE20) )
//! PSC2 is configured in for ramp mode, it means that the internal counter
//! counts from 0 up to OCR2RB then from OCR2RB downto 0
#define Psc2_in_centered_aligned_mode()                                       \
           (PCNF2 |=  (  (1<<PMODE21) | (1<<PMODE20) ) )
      //! @}

      //! @defgroup PSC2_flank_width_modulation_configuration PSC2 Flank Width Modulation Configuration
      //! Select PSC2 Flank Width Modulation Configuration
      //! @{
#define Psc2_symetrical_flank_width_modulation()      (PCTL2 |=  (1<<PBFM2) )     //!< PSC2 Flank width modulation operates on both OCR2RA and OCR2RB
#define Psc2_end_of_cycle_flank_width_modulation()    (PCTL2 &= ~(1<<PBFM2) )     //!< PSC2 Flank width modulation operates only on OCR2RB
      //! @}

      //! @defgroup PSC2_adc_amplifier_trigger_configuration PSC2 ADC Amplifier Trigger Configuration
      //! Select the PSC2 trigger configuration for the ADC or the amplifier
      //! @{
//! The PSC2 synchronization signal is sent to the ADC/Amplifier on waveform generator A leading edge
#define Psc2_synchro_on_waveform_generator_a_leading_edge()                   \
           (PSCO2  &= ~(  (1<<PSYNC21) | (1<<PSYNC20) ) )
//! The PSC2 synchronization signal is sent to the ADC/Amplifier on waveform generator A trailing edge
#define Psc2_synchro_on_waveform_generator_a_trailing_edge()                  \
           (PSCO2 &= ~(1<<PSYNC21),                                           \
            PSCO2 |=  (1<<PSYNC20) )
//! The PSC2 synchronization signal is sent to the ADC/Amplifier on waveform generator B leading edge
#define Psc2_synchro_on_waveform_generator_b_leading_edge()                   \
           (PSCO2 |=  (1<<PSYNC21),                                           \
            PSCO2 &= ~(1<<PSYNC20) )
//! The PSC2 synchronization signal is sent to the ADC/Amplifier on waveform generator B trailing edge
#define Psc2_synchro_on_waveform_generator_b_trailing_edge()                  \
           (PSCO2 |=  (  (1<<PSYNC21) | (1<<PSYNC20) ) )
      //! @}

      //! @defgroup PSC2_run_control PSC2 Run Control
      //! Turn On and Off the PSC2
      //! @{
#define Start_psc2()                                  (PCTL2 |=  (1<<PRUN2) )     //!< Turn On PSC2
#define Stop_psc2()                                   (PCTL2 &= ~(1<<PRUN2) )     //!< Turn Off PSC2
//! Turn Off PSC2 and the end of cycle
#define Psc2_complete_waveform_and_stop()             (PCTL2 |=  (1<<PCCYC2), \
                                                       PCTL2 &= ~(1<<PRUN2) )
#define Is_psc2_started()                             (PCTL2 & (1<<PRUN2) )
      //! @}

      //! @defgroup PSC2_fault_mode_configuration PSC2 Fault Mode Configuration
      //! PSC2 Fault Mode Configuration
      //! @{
#define Enable_pscout20_fault_mode()                  (PCTL2 |=  (1<<PAOC2A) )    //!< Fault input select to block A can act directly to PSCOUT00 output
#define Disable_pscout20_fault_mode()                 (PCTL2 &= ~(1<<PAOC2A) )    //!< No fault or retrigger management on PSCOUT00

#define Enable_pscout21_fault_mode()                  (PCTL2 |=  (1<<PAOC2B) )    //!< Fault input select to block A can act directly to PSCOUT00 output
#define Disable_pscout21_fault_mode()                 (PCTL2 &= ~(1<<PAOC2B) )    //!< No fault or retrigger management on PSCOUT01

#define Enable_psc2_autorun()                         (PCTL2 |=  (1<<PARUN2) )    //!< Setting PRUN1 in PCTL1 register or setting both PARUN1 in PCTL1 register and PRUN0 in PCTL0 register will make the two PSC start simultaneously
#define Disable_psc2_autorun()                        (PCTL2 &= ~(1<<PARUN2) )    //!< No synchronization between PSC2 and PSC2 exists
      //! @}

      //! @defgroup PSC2_interrupt_configuration PSC2 Interrupt Configuration
      //! PSC2 interrupt configuration
      //! @{
#define Enable_psc2_synchro_error_interrupt()         (PIM2 |=  (1<<PSEIE2) )     //!< An interrupt is generated when the PSEI0 bit is set
#define Disable_psc2_synchro_error_interrupt()        (PIM2 &= ~(1<<PSEIE2) )     //!< No interrupt is generated when the PSEI0 bit is set

#define Enable_psc2_external_event_a_interrupt()      (PIM2 |=  (1<<PEVE2A) )     //!< An external event which can generate a capture from retrigger/fault block A generates an interrupt
#define Disable_psc2_external_event_a_interrupt()     (PIM2 &= ~(1<<PEVE2A) )     //!< An external event which can generate a capture from retrigger/fault block A doesn't generate any interrupt

#define Enable_psc2_external_event_b_interrupt()      (PIM2 |=  (1<<PEVE2B) )     //!< An external event which can generate a capture from retrigger/fault block B generates an interrupt
#define Disable_psc2_external_event_b_interrupt()     (PIM2 &= ~(1<<PEVE2B) )     //!< An external event which can generate a capture from retrigger/fault block B doesn't generate any interrupt

#define Enable_psc2_end_of_cycle_interrupt()          (PIM2 |=  (1<<PEVE2A) )     //!< An interrupt is generated when PSC2 reaches the end of the whole cycle
#define Disable_psc2_end_of_cycle_interrupt()         (PIM2 &= ~(1<<PEVE2A) )     //!< No interrupt is generated when PSC2 reaches the end of the whole cycle
      //! @}

      //! @defgroup PSC2_interrupt_flag_control PSC2 Interrupt Flag Control
      //! PSC2 interrupt flag control
      //! @{
#define Is_psc2_synchro_error_interrupt_flag_set()    (PIFR2 & (1<<PSEI0) )       //!< Return 1 if the PSEI0 bit in PIFR2 is set
#define Clear_psc2_synchro_error_interrupt_flag()     (PIFR2 &= ~(1<<PSEI0) )     //!< Clear PSEI0 bit in PIFR2 register

#define Is_psc2_external_event_a_interrupt_flag_set() (PIFR2 & (1<<PEV0A) )       //!< Return 1 if the PEV0A bit in PIFR2 is set
#define Clear_psc2_external_event_a_interrupt_flag()  (PIFR2 &= ~(1<<PEV0A) )     //!< Clear PEV0A bit in PIFR2 register

#define Is_psc2_external_event_b_interrupt_flag_set() (PIFR2 & (1<<PEV0B) )       //!< Return 1 if the PEV0B bit in PIFR2 is set
#define Clear_psc2_external_event_b_interrupt_flag()  (PIFR2 &= ~(1<<PEV0B) )     //!< Clear PEV0B bit in PIFR2 register

#define Is_psc2_end_of_cycle_interrupt_flag_set()     (PIFR2 & (1<<PEOP2) )       //!< Return 1 if the PE2P2 bit in PIFR2 is set
#define Clear_psc2_end_of_cycle_interrupt_flag()           (PIFR2 &= ~(1<<PEOP2) )     //!< Clear PEOP2 bit in PIFR2 register
      //! @}

      //! @defgroup PSC2_comparison_value_initilization PSC2 Comparison Value Initialization
      //! This section allows to fully initilaize the comprison values
      //! @{
//! Init all PSC2 comparison values
#define Init_psc2_all_compare_values(PSC2_DEADTIME0, PSC2_ONTIME0, PSC2_DEADTIME1, PSC2_ONTIME1)  \
           (OCR2SAH = HIGH((U16)PSC2_DEADTIME0),                              \
            OCR2SAL = LOW ((U16)PSC2_DEADTIME0),                              \
            OCR2RAH = HIGH((U16)PSC2_ONTIME0),                                \
            OCR2RAL = LOW ((U16)PSC2_ONTIME0),                                \
            OCR2SBH = HIGH((U16)PSC2_DEADTIME1),                              \
            OCR2SBL = LOW ((U16)PSC2_DEADTIME1),                              \
            OCR2RBH = HIGH((U16)PSC2_ONTIME1),                                \
            OCR2RBL = LOW ((U16)PSC2_ONTIME1)   )
//! Init PSC2 fifty percent comparison values (usefull for 50% mode)
#define Init_psc2_fifty_percent_compare_values(PSC2_DEADTIME1,PSC2_ONTIME1)              \
           (OCR2SBH = HIGH((U16)PSC2_DEADTIME1),                              \
            OCR2SBL = LOW ((U16)PSC2_DEADTIME1),                              \
            OCR2RBH = HIGH((U16)PSC2_ONTIME1),                                \
            OCR2RBL = LOW ((U16)PSC2_ONTIME1)   )
   //! @}


//_____ D E F I N I T I O N S ______________________________________________
   //! @defgroup PSC_structure_definitions PSC Structure Definitions
   //! These definitions shall be used by the PSC functions
   //! @{
typedef struct st_psc_comparison_values {
   U16 deadtime0;
   U16 ontime0;
   U16 deadtime1;
   U16 ontime1;
}Psc_comparison_values;

typedef struct st_psc_fifty_percent_comparison_values {
   U16 deadtime1;
   U16 ontime1;
}Psc_fifty_percent_comparison_values;
   //! @}

//_____ F U N C T I O N S __________________________________________________
   //! @defgroup PSC0_low_level_functions PSC0 Low Level Fucntions
   //! This section allows to fully configure the PSC0 mode and to start it with the wanted Comparison values
   //! @{
//! Configures the PSC0 accordingly to the PSC0 Define Configuration values,
//! Then Init the PSC0 comparison values and start it
void init_psc0         (void);
//! Update all the PSC0 comparison values accordingly to the four values
//! passed as U16 parameters
void update_psc0       ( Psc_comparison_values *psc0_comparison_values );
//! Update half the PSC0 comparison values accordingly to the two values
//! passed as U16 parameters
void update_psc0_fifty ( Psc_fifty_percent_comparison_values *psc0_fifty_percent_comparison_values );
   //! @}

   //! @defgroup PSC1_low_level_functions PSC1 Low Level Fucntions
   //! This section allows to fully configure the PSC1 mode and to start it with the wanted Comparison values
   //! @{
//! Configures the PSC1 accordingly to the PSC1 Define Configuration values,
//! Then Init the PSC1 comparison values and start it
void init_psc1         (void);
//! Update all the PSC1 comparison values accordingly to the four values
//! passed as U16 parameters
void update_psc1       ( Psc_comparison_values *psc1_comparison_values );
//! Update half the PSC1 comparison values accordingly to the two values
//! passed as U16 parameters
void update_psc1_fifty ( Psc_fifty_percent_comparison_values *psc1_fifty_percent_comparison_values );
   //! @}

   //! @defgroup PSC2_low_level_functions PSC2 Low Level Fucntions
   //! This section allows to fully configure the PSC2 mode and to start it with the wanted Comparison values
   //! @{
//! Configures the PSC2 accordingly to the PSC2 Define Configuration values,
//! Then Init the PSC2 comparison values and start it
void init_psc2         (void);
//! Update all the PSC2 comparison values accordingly to the four values
//! passed as U16 parameters
void update_psc2       ( Psc_comparison_values *psc2_comparison_values );
//! Update half the PSC2 comparison values accordingly to the two values
//! passed as U16 parameters
void update_psc2_fifty ( Psc_fifty_percent_comparison_values *psc2_fifty_percent_comparison_values );
   //! @}

//! @}
#endif  // PSC_DRV_H
