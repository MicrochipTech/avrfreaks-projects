/*
 * Comms.c
 *
 * Created: 5/21/2017 12:09:55 AM
 *  Author: Big Daddy
 */ 

#define _COMMS_

#include "project.h"
#include <asf.h>
#include <board.h>
#include <conf_board.h>


// ISR comes first because it makes the definition of the handler name
ISR(comms_int_handler,AVR32_USART0_IRQ,AVR32_INTC_INT1) // 
{
	int c;

	/*
	 * In the code line below, the interrupt priority level does not need to
	 * be explicitly masked as it is already because we are within the
	 * interrupt handler.
	 * The USART Rx interrupt flag is cleared by side effect when reading
	 * the received character.
	 * Waiting until the interrupt has actually been cleared is here useless
	 * as the call to usart_write_char will take enough time for this before
	 * the interrupt handler is left and the interrupt priority level is
	 * unmasked by the CPU.
	 */
	usart_read_char(USART, &c);
	if ((c > '/')&&(c < ';')) gn_DriveFlag = c - 0x30;

	if ((c == '>')||(c == '.')) {gn_DirFlag = 1;}
	if ((c == '<')||(c == ',')) {gn_DirFlag = 0;}
		
	// Print the received character to USART. This is a simple echo.
	usart_write_char(USART, c);
}



void Comms_Init(void)
{
	
	static const gpio_map_t USART_GPIO_MAP =
	{
		{USART_RX_PIN, USART_RX_FUNCTION},
		{USART_TX_PIN, USART_TX_FUNCTION}
	};

	// USART options.
	static const usart_options_t USART_OPTIONS =
	{
		.baudrate     = USART_BAUDRATE,
		.charlength   = 8,
		.paritytype   = USART_NO_PARITY,
		.stopbits     = USART_1_STOPBIT,
		.channelmode  = USART_NORMAL_CHMODE
	};

	// Assign GPIO to USART.
	gpio_enable_module(USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	// Initialize interrupt vectors.
	INTC_init_interrupts();

	/*
	 * Register the USART interrupt handler to the interrupt controller.
	 * usart_int_handler is the interrupt handler to register.
	 * EXAMPLE_USART_IRQ is the IRQ of the interrupt handler to register.
	 * AVR32_INTC_INT0 is the interrupt priority level to assign to the
	 * group of this IRQ.
	 */
	INTC_register_interrupt(&comms_int_handler, USART_IRQ, AVR32_INTC_INT3);
	
	sysclk_enable_pba_module(SYSCLK_USART0);
	stdio_serial_init(USART, &USART_OPTIONS);

	// Enable interrupts
	USART->ier = (  AVR32_USART_IER_RXRDY_MASK  );    // Receive byte
	//                    AVR32_USART_IER_TXRDY_MASK );    // Transmitter empty
	

}

