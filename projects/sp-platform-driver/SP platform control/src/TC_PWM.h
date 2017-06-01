/*
 * TC_PWM.h
 *
 * Created: 5/20/2017 8:10:27 PM
 *  Author: Big Daddy
 */ 


#ifndef TC_PWM_H_
#define TC_PWM_H_

#define PWM_FREQ 37000
#define PWM_PERIOD_VALUE ((BOARD_OSC0_HZ / 2 )/ PWM_FREQ) // 2 is the divider from TC_CLOCK_SOURCE_TC2

// timer counter
#define TC0_0 0
#define TC0_1 1
#define TC0_2 3

#define MTR0 0
#define MTR1 1
#define MTR2 2
#define MTR3 3
#define MTR4 4
#define MTR5 5

#define PWM0_PIN AVR32_TC0_B1_2_PIN
#define PWM0_FUNCTION AVR32_TC0_B1_2_FUNCTION
#define PWM1_PIN AVR32_TC0_A2_3_PIN
#define PWM1_FUNCTION AVR32_TC0_A2_3_FUNCTION
#define PWM2_PIN AVR32_TC0_B0_2_PIN
#define PWM2_FUNCTION AVR32_TC0_B0_2_FUNCTION
#define PWM3_PIN AVR32_TC0_A1_2_PIN
#define PWM3_FUNCTION AVR32_TC0_A1_2_FUNCTION
#define PWM4_PIN AVR32_TC0_B2_2_PIN
#define PWM4_FUNCTION AVR32_TC0_B2_2_FUNCTION
#define PWM5_PIN AVR32_TC0_A0_2_PIN
#define PWM5_FUNCTION AVR32_TC0_A0_2_FUNCTION

// local or extern?
#ifdef _TC_PWM_
#define GLOBAL
#define _I(x) x
#else // end #ifdef _TC_PWM_
#define GLOBAL extern
#define _I(x)
#endif // end #ifndef _TC_PWM_

GLOBAL volatile avr32_tc_t *tc0 _I(= &AVR32_TC0);

GLOBAL void TC_PWM_Init(void);

GLOBAL void init_tc_PWM_channel(unsigned int channel);
GLOBAL void set_pwm_duty_cycle(U8 pwm, U8 dutycycle);

// cleanup
#undef _I
#undef GLOBAL

#endif /* TC_PWM_H_ */