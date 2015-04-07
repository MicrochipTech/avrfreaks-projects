/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <ext_int_mega.h>
#include <tc.h>

bool toggle;
volatile bool run_once;

static void sw0_int_callback(void)
{
	// Set run_once only on SW0 pressed (inverted logic)
	if(!ioport_get_pin_level(SW0_GPIO)) run_once = 1;
}

//static void tcnt0_ovf_int_callback(void)
static void tcnt1_ovf_int_callback(void)
//static void tcnt2_ovf_int_callback(void)
{
	ioport_toggle_pin(LED0_GPIO);
}

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	sysclk_init();

	board_init();

	/* Insert application code here, after the board has been initialized. */
	
	run_once = 0;
	toggle = 1;
	
	/* Disable Global interrupt */
	cpu_irq_disable();
	
	/* Init TC Overflow counter */
	//tc0_write_count(0);
	tc1_write_count(0);
	//tc2_write_count(0);
	
	/* Enable TC clock before setup */
	//tc0_enable();
	tc1_enable();
	//tc2_enable();
	
	/* Set TC clock source */
	//tc0_write_clock_source(TC01_CLKSEL_PS_1024); // No clock no party...
	tc1_write_clock_source(TC01_CLKSEL_PS_8); // No clock no party...
	//tc2_write_clock_source(TC2_CLKSEL_PS_1024); // No clock no party...

	/* Set TC Overflow Interrupt CallBack Function */
	//tc0_set_ovf_interrupt_callback(tcnt0_ovf_int_callback);
	tc1_set_ovf_interrupt_callback(tcnt1_ovf_int_callback);
	//tc2_set_ovf_interrupt_callback(tcnt2_ovf_int_callback);
	
	/* Disable TC clock */
	//tc0_disable();
	tc1_disable();
	//tc2_disable();

    /* Set External Interrupt CallBack Function */
	ext_int_set_interrupt_callback(SW0_GPIO, sw0_int_callback);
	
	/* Enable the Ext Int */
	ext_int_pcint_init(SW0_GPIO);
	
	/* Enable Global interrupt */
	cpu_irq_enable();
	
	while (1) {
		if(run_once)
		{
			if(toggle)
			{
				/* Enable TC clock */
				//tc0_enable();
				tc1_enable();
				//tc2_enable();
			}
			else
			{
				/* Disable TC0 clock */
				//tc0_disable();
				tc1_disable();
				//tc2_disable();
				ioport_set_pin_level(LED0_GPIO, 0);
			}
			
			run_once = 0;
			toggle = !toggle;
		}
	}
}
