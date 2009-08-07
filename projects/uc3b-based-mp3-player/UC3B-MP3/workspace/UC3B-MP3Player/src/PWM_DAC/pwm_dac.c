
#include <stddef.h>

#include "compiler.h"
#include "pwm_dac.h"
#include "conf_player.h"

#include <avr32/io.h>
#include "intc.h"
#include "gpio.h"

#include "board.h"

extern unsigned short *outPtr;

volatile dac_output_params pwm_dac;

// Map of the PWM pins used by the PWM DAC.
static const gpio_map_t PWM_DAC_PWM_GPIO_MAP =
{
#ifdef PWM_DAC_MSB_R_CHANNEL
	// This board has a separate right channel.
	{PWM_DAC_LSB_R_PIN, PWM_DAC_LSB_R_FUNCTION},
	{PWM_DAC_MSB_R_PIN, PWM_DAC_MSB_R_FUNCTION},
#endif
	{PWM_DAC_LSB_PIN, PWM_DAC_LSB_FUNCTION},
	{PWM_DAC_MSB_PIN, PWM_DAC_MSB_FUNCTION}
};

__attribute__((__interrupt__))
static void pwm_dac_int_handler(void)
{
	AVR32_PWM.isr; // Clear interrupt status register.

	S16 ssample;
	U16 sample;

	if (pwm_dac.sample_buffer == outPtr)
	{
		/* There are no more samples ready for consumption. This
		 * means the decoder is slower than the dac, which is no good. */
		gpio_clr_gpio_pin(LED0_GPIO);
		return; // We have to wait for the next interrupt to update.
	}
	gpio_set_gpio_pin(LED0_GPIO);

	ssample = (S16)*pwm_dac.sample_buffer >> pwm_dac.volume_shift;
	sample = ((U16)ssample + 0x8000) & 0xffff;

	// Output 8 least significant bits to LSB-PWM:
	AVR32_PWM.channel[PWM_DAC_LSB_CHANNEL].cupd = (U8) sample;
	// Shift sample 8 bits to the right, and output the result to MSB-PWM:
	AVR32_PWM.channel[PWM_DAC_MSB_CHANNEL].cupd = (U8) (sample >> 8);

	if(pwm_dac.num_channels==2) {
		pwm_dac.sample_buffer++; // Next sample contains right channel.
	}

#ifdef PWM_DAC_MSB_R_CHANNEL
	// This board has a separate right channel.
	// Output right sample to right channel, or left again if mono.
	ssample = (S16)*pwm_dac.sample_buffer >> pwm_dac.volume_shift;
	sample = ((U16)ssample + 0x8000) & 0xffff;

	AVR32_PWM.channel[PWM_DAC_LSB_R_CHANNEL].cupd = (U8) sample;
	AVR32_PWM.channel[PWM_DAC_MSB_R_CHANNEL].cupd = (U8)(sample >> 8);
#endif

	pwm_dac.sample_buffer++;

	if(pwm_dac.sample_buffer >= (pwm_dac.sample_buffer_start + OUTBUF_SIZE-1)) {
		// Rewind the output buffer when we're at the end.
		pwm_dac.sample_buffer = pwm_dac.sample_buffer_start;
	}
}

void pwm_dac_start(U32 sample_rate_hz,
                   U8 num_channels,
                   U8 bits_per_sample,
                   U16 *sample_buffer,
                   U32 pba_hz)
{
	pwm_dac_stop(); // Stop / reset the dac.

	pwm_dac.sample_buffer_start = sample_buffer;
	pwm_dac.sample_buffer = sample_buffer;
	pwm_dac.num_channels		= num_channels;
	pwm_dac.bits_per_sample	= bits_per_sample;
	pwm_dac.flushing = FALSE;

	// Enable PWM module.
	gpio_enable_module(PWM_DAC_PWM_GPIO_MAP, sizeof(PWM_DAC_PWM_GPIO_MAP) / sizeof(PWM_DAC_PWM_GPIO_MAP[0]));

	AVR32_PWM.mr =
		(0 << AVR32_PWM_MR_PREA_OFFSET) | 	// No prescaling.
		(1 << AVR32_PWM_MR_DIVA_OFFSET) |	// Divide by 1.
		(0 << AVR32_PWM_MR_DIVB_OFFSET); 	// Clock B is not in use.

	AVR32_PWM.channel[PWM_DAC_MSB_CHANNEL].cprd = 0xff;
	AVR32_PWM.channel[PWM_DAC_LSB_CHANNEL].cprd = 0xff;
	AVR32_PWM.channel[PWM_DAC_MSB_CHANNEL].cmr =
		(AVR32_PWM_CPRE_CLKA << AVR32_PWM_CPRE_OFFSET) |	// Use clock A.
		(1 << AVR32_PWM_CALG_OFFSET) |						// Center aligned.
		(1 << AVR32_PWM_CPOL_OFFSET) |						// Period starts at low level.
		(0 << AVR32_PWM_CPD_OFFSET);						// Writing to cupd updates duty cycle.
	AVR32_PWM.channel[PWM_DAC_LSB_CHANNEL].cmr =
		(AVR32_PWM_CPRE_CLKA << AVR32_PWM_CPRE_OFFSET) |	// Use clock A.
		(1 << AVR32_PWM_CALG_OFFSET) |						// Center aligned.
		(1 << AVR32_PWM_CPOL_OFFSET) |						// Period starts at low level.
		(0 << AVR32_PWM_CPD_OFFSET);						// Writing to cupd updates duty cycle.

	// Separate channel for accurate timing:
	AVR32_PWM.channel[PWM_DAC_TIMING_CHANNEL].cprd = (int) (pba_hz/sample_rate_hz);
	AVR32_PWM.channel[PWM_DAC_TIMING_CHANNEL].cmr =
		(AVR32_PWM_CPRE_CLKA << AVR32_PWM_CPRE_OFFSET) |	// Use clock A.
		(0 << AVR32_PWM_CALG_OFFSET) |						// Left aligned.
		(0 << AVR32_PWM_CPOL_OFFSET) |						// Period starts at low level.
		(0 << AVR32_PWM_CPD_OFFSET);						// Writing to cupd updates duty cycle.

	// Enable interrupt routine:
	INTC_register_interrupt(pwm_dac_int_handler, AVR32_PWM_IRQ, AVR32_INTC_INT1);
	AVR32_PWM.ier = (1 << PWM_DAC_TIMING_CHANNEL);

#ifdef PWM_DAC_MSB_R_CHANNEL
	// This board has a separate right channel.
	AVR32_PWM.channel[PWM_DAC_MSB_R_CHANNEL].cprd = 0xff;
	AVR32_PWM.channel[PWM_DAC_LSB_R_CHANNEL].cprd = 0xff;
	AVR32_PWM.channel[PWM_DAC_MSB_R_CHANNEL].cmr =
		(AVR32_PWM_CPRE_CLKA << AVR32_PWM_CPRE_OFFSET) |	// Use clock A.
		(1 << AVR32_PWM_CALG_OFFSET) |						// Center aligned.
		(1 << AVR32_PWM_CPOL_OFFSET) |						// Period starts at low level.
		(0 << AVR32_PWM_CPD_OFFSET);						// Writing to cupd updates duty cycle.
	AVR32_PWM.channel[PWM_DAC_LSB_R_CHANNEL].cmr =
		(AVR32_PWM_CPRE_CLKA << AVR32_PWM_CPRE_OFFSET) |	// Use clock A.
		(1 << AVR32_PWM_CALG_OFFSET) |						// Center aligned.
		(1 << AVR32_PWM_CPOL_OFFSET) |						// Period starts at low level.
		(0 << AVR32_PWM_CPD_OFFSET);						// Writing to cupd updates duty cycle.

	// Enable PWMs.
	AVR32_PWM.ena = (1 << PWM_DAC_LSB_R_CHANNEL) | (1 << PWM_DAC_MSB_R_CHANNEL) | (1 << PWM_DAC_LSB_CHANNEL) | (1 << PWM_DAC_MSB_CHANNEL)| (1 << PWM_DAC_TIMING_CHANNEL);
#else
	// Enable PWMs.
	AVR32_PWM.ena = (1 << PWM_DAC_LSB_CHANNEL) | (1 << PWM_DAC_MSB_CHANNEL) | (1 << PWM_DAC_TIMING_CHANNEL);
#endif

}

void pwm_dac_increase_volume(void)
{
  if (pwm_dac.volume_shift) pwm_dac.volume_shift--;
}


void pwm_dac_decrease_volume(void)
{
  if (pwm_dac.volume_shift < 16) pwm_dac.volume_shift++;
}

void pwm_dac_stop(void)
{
  // Disable interrupt.
  AVR32_PWM.dis = (1 << PWM_DAC_TIMING_CHANNEL);

  // Free pins used for PWM.
  gpio_enable_gpio(PWM_DAC_PWM_GPIO_MAP,
                   sizeof(PWM_DAC_PWM_GPIO_MAP) / sizeof(PWM_DAC_PWM_GPIO_MAP[0]));
}
