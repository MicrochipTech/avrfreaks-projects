/*
 * ADC.c
 *
 * Created: 5/26/2017 7:21:14 PM
 *  Author: Big Daddy
 */ 

#define _ADC_

#include "project.h"
#include <asf.h>
#include <board.h>
#include <conf_board.h>


// ISR comes first because it makes the definition of the handler name
// ISR(ADCIFA_interrupt_handler, AVR32_ADCIFA_IRQ_GROUP, ADC_INTERRUPT_PRIORITY)
// {
// 	/* get sequence */
// 	LED_ON(RED_LED);
// 	ADCIFA_read_resx_sequencer_0(0);
// }



/*! \brief Initializes the ADC.
 */
void ADCIFA_init(void)
{
	adcifa_sequencer_opt_t adcifa_sequence_opt;
	adcifa_sequencer_conversion_opt_t adcifa_sequence_conversion_opt[ ADC_CHANNELS ];
	adcifa_opt_t adc_config_t;

	adcifa_window_monitor_opt_t adcifa_window_monitor_0_opt;

//	unsigned char volatile window1;

	/* Assign and enable GPIO pins to the ADC function. */
	gpio_enable_module(ADCIFA_GPIO_MAP, sizeof(ADCIFA_GPIO_MAP) / sizeof(ADCIFA_GPIO_MAP[0]));

	/* Configure the ADC for the application*/
	adc_config_t.frequency                = 10000;
	adc_config_t.reference_source         = ADCIFA_REF1V;
	adc_config_t.sample_and_hold_disable  = false;
	adc_config_t.single_sequencer_mode    = true;
	adc_config_t.free_running_mode_enable = true;
  	adc_config_t.sleep_mode_enable        = false;
    adc_config_t.mux_settle_more_time     = false;

	/* Get ADCIFA Factory Configuration */
	adcifa_get_calibration_data(&AVR32_ADCIFA, &adc_config_t);

	/* Calibrate offset first*/
	adcifa_calibrate_offset(&AVR32_ADCIFA, &adc_config_t, sysclk_get_cpu_hz());

	/* Configure ADCIFA core */
	adcifa_configure( &AVR32_ADCIFA, &adc_config_t, sysclk_get_cpu_hz() );

	/* ADCIFA sequencer 0 configuration structure*/
	adcifa_sequence_opt.convnb              = ADC_CHANNELS;
	adcifa_sequence_opt.resolution          = ADCIFA_SRES_12B;
	adcifa_sequence_opt.trigger_selection   = ADCIFA_TRGSEL_CONTINUOUS;
	adcifa_sequence_opt.start_of_conversion = ADCIFA_SOCB_SINGLECONV;
	adcifa_sequence_opt.sh_mode             = ADCIFA_SH_MODE_OVERSAMP;
	adcifa_sequence_opt.half_word_adjustment= ADCIFA_HWLA_NOADJ;
	adcifa_sequence_opt.software_acknowledge= ADCIFA_SA_NO_EOS_SOFTACK;

	/* ADCIFA conversions for sequencer 0*/
	adcifa_sequence_conversion_opt[0].channel_p = P8V192sns_INP;
	adcifa_sequence_conversion_opt[0].channel_n = P8V192sns_INN;
	adcifa_sequence_conversion_opt[0].gain      = ADCIFA_SHG_1;

	/* Window Monitor 0 Configuration */
	/* Window Mode 2 -> Active (If Result Register > Low Threshold) */
	adcifa_window_monitor_0_opt.mode           = ADCIFA_WINDOW_MODE_NONE;
	/* First channel in the configured sequence */
	adcifa_window_monitor_0_opt.source_index   = 0;
	/* Low Threshold Equivalent ADC counts */
	adcifa_window_monitor_0_opt.low_threshold  = 0x1FF;
	/* High Threshold Equivalent ADC counts */
	adcifa_window_monitor_0_opt.high_threshold = 0;

//	irq_initialize_vectors();

	/*
	 * Register the ADCIFA interrupt handler
	 */
// 	irq_register_handler(&ADCIFA_interrupt_handler, AVR32_ADCIFA_SEQUENCER0_IRQ, ADC_INTERRUPT_PRIORITY);
// 
// 	/* Enable seq0 complete Interrupt */
// 	adcifa_enable_interrupt(&AVR32_ADCIFA, AVR32_ADCIFA_IER_SEOC0_MASK  );
// 
	/* Configure ADCIFA sequencer 0 */
	adcifa_configure_sequencer(&AVR32_ADCIFA, 0, &adcifa_sequence_opt, adcifa_sequence_conversion_opt);

	/* Start ADCIFA sequencer 0 */
	adcifa_start_sequencer(&AVR32_ADCIFA, 0);

}

