/*
 * PositionSensors.c
 *
 * Created: 5/20/2017 4:36:04 PM
 *  Author: Big Daddy
 */ 

#define _POSITIONSENSORS_

#include "project.h"
#include <asf.h>
#include <board.h>
#include <conf_board.h>

// ISR comes first because it makes the definition of the handler name
	/*
	 * In the code line below, the interrupt priority level does not need to
	 * be explicitly masked as it is already because we are within the
	 * interrupt handler.
	 * scan for any edges, clear interrupt, get out of dodge
	 */


ISR( position_sensor_int_handler_G12, AVR32_GPIO_IRQ_0, AVR32_INTC_INT2 )
{
	LED_ON(RED_LED);
// 	if( gpio_get_pin_interrupt_flag( QC0 ) )
// 	{		// PD11 [gpio 107]  generated the interrupt.
// 		// Clear the interrupt flag of the pin PD11 is mapped to.
// 		gpio_clear_pin_interrupt_flag( QC0 );
// 	}
// 	if( gpio_get_pin_interrupt_flag( QC1 ) )
// 	{		// PD12 generated the interrupt.
// 		// Clear the interrupt flag
// 		gpio_clear_pin_interrupt_flag( QC1 );
// 	}
// 	if( gpio_get_pin_interrupt_flag( QC2 ) )
// 	{		// PD13 generated the interrupt.
// 		// Clear the interrupt flag
// 		gpio_clear_pin_interrupt_flag( QC2 );
// 	}
	if( gpio_get_pin_interrupt_flag( QC3 ) )
	{		// PD14 generated the interrupt.
		gn_PosDir[3] = gpio_get_pin_value(QD3);
		if ( gn_PosDir[3] ) {gn_PosCnt[3]++;}
		else {gn_PosCnt[3]--;}
		// Clear the interrupt flag
		gpio_clear_pin_interrupt_flag( QC3 );
	}
	//LED_OFF(RED_LED);
}

// ISR(position_sensor_int_handler_G14,AVR32_GPIO_IRQ_0,AVR32_INTC_INT2)
// {
// 	LED_ON(RED_LED);
// 
// 	if( gpio_get_pin_interrupt_flag( QC4 ) )
// 	{		// PD29 generated the interrupt.
// 		// Clear the interrupt flag
// 		gpio_clear_pin_interrupt_flag( QC4 );
// 	}
// }
// 
// ISR(position_sensor_int_handler_G13,AVR32_GPIO_IRQ_0,AVR32_INTC_INT2)
// {
// 	LED_ON(RED_LED);
// 
// 	if( gpio_get_pin_interrupt_flag( QC5 ) )
// 	{		// PD21 generated the interrupt.
// 		// Clear the interrupt flag
// 		gpio_clear_pin_interrupt_flag( QC5 );
// 	}
// }
// 


// initialize support for position sensors for each leg
void PositionSensorInit(void)
{

	// gpio Inputs: Direction sense on position sensors
	gpio_configure_pin(QD0, GPIO_DIR_INPUT);
	gpio_configure_pin(QD1, GPIO_DIR_INPUT);
	gpio_configure_pin(QD2, GPIO_DIR_INPUT);
	gpio_configure_pin(QD3, GPIO_DIR_INPUT);
	gpio_configure_pin(QD4, GPIO_DIR_INPUT);
	gpio_configure_pin(QD5, GPIO_DIR_INPUT);

	gpio_configure_pin(QC0, GPIO_DIR_INPUT|GPIO_PULL_DOWN);
	gpio_configure_pin(QC1, GPIO_DIR_INPUT|GPIO_PULL_DOWN);
	gpio_configure_pin(QC2, GPIO_DIR_INPUT|GPIO_PULL_DOWN);
	gpio_configure_pin(QC3, GPIO_DIR_INPUT|GPIO_PULL_DOWN);
	gpio_configure_pin(QC4, GPIO_DIR_INPUT|GPIO_PULL_DOWN);
	gpio_configure_pin(QC5, GPIO_DIR_INPUT|GPIO_PULL_DOWN);

	sysclk_enable_pba_module(SYSCLK_GPIO);
	
	// gpio inputs: Clocks for position sensor quadrature wheels [interrupt on rising edge]
// 	gpio_enable_pin_interrupt(QC0, GPIO_RISING_EDGE);
// 	gpio_enable_pin_interrupt(QC1, GPIO_RISING_EDGE);
// 	gpio_enable_pin_interrupt(QC2, GPIO_RISING_EDGE);
	gpio_enable_pin_interrupt(QC3, GPIO_RISING_EDGE);
// 	gpio_enable_pin_interrupt(QC4, GPIO_RISING_EDGE);
// 	gpio_enable_pin_interrupt(QC5, GPIO_RISING_EDGE);

	// Initialize interrupt vectors.
	Disable_global_interrupt();

	INTC_init_interrupts();

	INTC_register_interrupt( &position_sensor_int_handler_G12, (AVR32_GPIO_IRQ_0+(QC3/8)), AVR32_INTC_INT2); // group 13

// 	INTC_register_interrupt( &position_sensor_int_handler_G14, AVR32_GPIO_IRQ_0+(QC4/8), AVR32_INTC_INT2); // group 14
// 	INTC_register_interrupt( &position_sensor_int_handler_G13, AVR32_GPIO_IRQ_0+(QC5/8), AVR32_INTC_INT2); // group 13
	
}


