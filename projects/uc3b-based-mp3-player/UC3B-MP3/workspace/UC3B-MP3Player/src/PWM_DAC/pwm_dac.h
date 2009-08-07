
#ifndef _PWM_DAC_H_
#define _PWM_DAC_H_

#include <stddef.h>

#include "board.h"
#include "compiler.h"

#if BOARD==EVK1101
#	define PWM_DAC_LSB_CHANNEL		6
#	define PWM_DAC_LSB_PIN			AVR32_PWM_6_0_PIN
#	define PWM_DAC_LSB_FUNCTION		AVR32_PWM_6_0_FUNCTION
#	define PWM_DAC_MSB_CHANNEL		4
#	define PWM_DAC_MSB_PIN			AVR32_PWM_4_1_PIN
#	define PWM_DAC_MSB_FUNCTION		AVR32_PWM_4_1_FUNCTION
#	define PWM_DAC_TIMING_CHANNEL	3
#elif BOARD==MP3BOARD
#	define PWM_DAC_LSB_CHANNEL		1
#	define PWM_DAC_LSB_PIN			AVR32_PWM_1_0_PIN
#	define PWM_DAC_LSB_FUNCTION		AVR32_PWM_1_0_FUNCTION
#	define PWM_DAC_MSB_CHANNEL		0
#	define PWM_DAC_MSB_PIN			AVR32_PWM_0_0_PIN
#	define PWM_DAC_MSB_FUNCTION		AVR32_PWM_0_0_FUNCTION
#	define PWM_DAC_LSB_R_CHANNEL	6
#	define PWM_DAC_LSB_R_PIN		AVR32_PWM_6_0_PIN
#	define PWM_DAC_LSB_R_FUNCTION	AVR32_PWM_6_0_FUNCTION
#	define PWM_DAC_MSB_R_CHANNEL	2
#	define PWM_DAC_MSB_R_PIN		AVR32_PWM_2_0_PIN
#	define PWM_DAC_MSB_R_FUNCTION	AVR32_PWM_2_0_FUNCTION
#	define PWM_DAC_TIMING_CHANNEL	3
#endif

typedef struct {
  U8 num_channels;
  U8 bits_per_sample;
  U16 *sample_buffer_start;
  volatile U16 *sample_buffer;
  U8 volume_shift;
  Bool flushing;
} dac_output_params;

extern volatile dac_output_params pwm_dac;

/*! \brief Starts the DAC.
 */
extern void pwm_dac_start(U32 sample_rate_hz,
                          U8 num_channels,
                          U8 bits_per_sample,
                          U16 *sample_buffer,
                          U32 pba_hz);

/*! \brief Increases the output volume of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern void pwm_dac_increase_volume(void);

/*! \brief Decreases the output volume of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern void pwm_dac_decrease_volume(void);

/*! \brief Stops the DAC.
 */
extern void pwm_dac_stop(void);

#endif  // _PWM_DAC_H_
