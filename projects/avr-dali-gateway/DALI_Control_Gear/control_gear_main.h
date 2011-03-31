#ifndef REFD_H
#define REFD_H

//_____ I N C L U D E S ____________________________________________________

#include "config.h"

//_____ M A C R O S ________________________________________________________

  /***  Timer1 Configuration Macros ***/

#define Dali_set_tccr1a()         ( TCCR1A = 0,\
                                  	TCCR0A = (T1_COMPARE_MATCH_OUTPUT_A_MODE<<COM1A0\
                                              | T1_COMPARE_MATCH_OUTPUT_B_MODE<<COM1B0\
                                              | T1_WAVEFORM_GENERATION_MODE_1<<WGM10) )
#define Dali_set_tccr1b()         ( TCCR1B = 0,\
                                    TCCR1B = (T1_CAPTURE_NOISE_CANCELLER<<ICNC1 \
                                            | T1_INPUT_CAPTURE_EDGE<<ICES1      \
                                            | T1_WAVEFORM_GENERATION_MODE_2<<WGM12 \
                                            | T1_TIMER_PRESCALER<<CS10) )
#define Dali_set_tccr1c()         ( TCCR1C = 0,\
                                    TCCR1C = (T1_FORCE_OUTPUT_COMPARE_A<<FOC1A \
                                            | T1_FORCE_OUTPUT_COMPARE_B<<FOC1B) )
#define Dali_set_t1_period()      ( OCR1AH = ((U8)T1_TOP_H),\
                                    OCR1AL = ((U8)T1_TOP_L) )
#define Dali_set_tifr1()          ( TIFR1 = 0,\
 	                                  TIFR1 = (T1_CLEAR_ICF1<<ICF1    \
                                           | T1_CLEAR_OCF1B<<OCF1B  \
                                           | T1_CLEAR_OCF1A<<OCF1A  \
                                           | T1_CLEAR_TOV1<<TOV1) )
#define Dali_set_timsk1()         ( TIMSK1 = 0,\
                                    TIMSK1 = (T1_INPUT_CAPTURE_INTERRUPT<<ICIE1 \
                                            | T1_ENABLE_OUTPUT_COMPARE_B_INTERRUPT<<OCIE1B \
                                            | T1_ENABLE_OUTPUT_COMPARE_A_INTERRUPT<<OCIE1A \
                                            | T1_ENABLE_T0_OVERFLOW_INTERRUPT<<TOIE1) ) 

  /*** Bargraph display ***/

#define   BAR_POWER(x)  (x>0 ? (PORTB = 0xff << (x>>5)+1) : (PORTB = 0xff))

//_____ D E F I N I T I O N S ______________________________________________

//_____ D E C L A R A T I O N S ____________________________________________

//_____ F U N C T I O N S __________________________________________________

void usr_init_ports(void);
void usr_update_output_level(U8);
U8 usr_check_lamp(void);
void usr_init_timer_1(void);
#endif
