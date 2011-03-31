#ifndef CONFIG_H
#define CONFIG_H

//_____  I N C L U D E S ___________________________________________________

#include "common/lib_mcu/compiler.h"
#include "common/lib_mcu/mcu.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

#define FOSC		                  8000000  //!< Oscillator Frequency : 8MHz

  /***  TIMER 1 Configuration ***/

#define USE_TIMER_1

#define T1_COMPARE_MATCH_OUTPUT_A_MODE    1
//!<0: OCR0A disconnected  1: Toggle OCR0A  2: Clear OCR0A  3: Set OCR0A
#define T1_COMPARE_MATCH_OUTPUT_B_MODE    0
//!<0: OCR0A disconnected  1: Toggle OCR0A  2: Clear OCR0A  3: Set OCR0A
#define T1_WAVEFORM_GENERATION_MODE_1     2
//!<0: Normal   1:PWM Phase Correct  2:CTC  3:Fast PWM   

#define T1_CAPTURE_NOISE_CANCELLER        0
//!<0: disabled                1: enabled
#define T1_INPUT_CAPTURE_EDGE             0
//!<0: falling edge on ICPn    1: rising edge
#define T1_WAVEFORM_GENERATION_MODE_2     1
//!<0: see datasheet
#define T1_TIMER_PRESCALER                3
//!<0: no clock  1: 1  2: 8  3: 64   4: 256  5: 1024 
//! 6: output clock on falling edge 7: output clock on rising edge

#define T1_FORCE_OUTPUT_COMPARE_A         0
//!<0: no force output compare  1: force output compare
#define T1_FORCE_OUTPUT_COMPARE_B         0
//!<0: no force output compare  1: force output compare

#define T1_ACTUAL_PRESCALER               64
//see T1_TIMER_PRESCALER definition and set T1_ACTUAL_PRESCALER to actual value

#ifdef FOSC    
  #define T1_TOP                          ((U16)(FOSC/(1000.*T1_ACTUAL_PRESCALER)))
//this value is calculated to match every 1ms
  #define T1_TOP_H                        ((U8)(T1_TOP>>8))
  #define T1_TOP_L                        ((U8)T1_TOP)
#else 
  #error(FOSC must be defined. See "dali_config.h")
#endif

#define T1_INPUT_CAPTURE_INTERRUPT              0
//!<0: interrupt disabled  1: interrupt enabled
#define T1_ENABLE_OUTPUT_COMPARE_B_INTERRUPT    0
//!<0: interrupt disabled  1: interrupt enabled
#define T1_ENABLE_OUTPUT_COMPARE_A_INTERRUPT    1
//!<0: interrupt disabled  1: interrupt enabled
#define T1_ENABLE_T0_OVERFLOW_INTERRUPT         0
//!<0: interrupt disabled  1: interrupt enabled

#define T1_CLEAR_ICF1                     1
//!<0: -   1: clear output compare A interrupt flag
#define T1_CLEAR_OCF1B                    1
//!<0: -   1: clear output compare A interrupt flag
#define T1_CLEAR_OCF1A                    1
//!<0: -   1: clear output compare B interrupt flag
#define T1_CLEAR_TOV1                     1
//!<0: -   1: clear timer 0 overflow interrupt flag

#endif  // CONFIG_H
