/**
 * \AVR32 USART1 Example
 *
 * \This project demonstrates serial communication over the USART1 on the AT32UC3C0512C chip.
 * \This project is specific to the AT32UC3C-EK development board, but can be adapted to other hardware
 * \RX and TX are present on header J25 of the UC3C-EK and a serial to RS-232 converter must be used if connecting to a PC
 */

#include <Uc3c_ek.h>
#include <avr32/io.h>
#include "usart.h"
#include "gpio.h"

#define SUCCESS 0;
#define FAILURE -1;

/* used for printing strings */
void print(volatile struct avr32_usart_t * usart, char *str);

int main( void )
{
	// Unlock the OSCCTRL0 register by writing its address and the key value to the SCIF unlock register
	AVR32_SCIF.unlock = 0xAA000000 + AVR32_SCIF_OSCCTRL;
	// Setup oscillator 0 for a crystal oscillator, automatic gain control, 128 RCSYS clocks of startup time, and enable oscillator 0
	AVR32_SCIF.oscctrl[0] = (AVR32_SCIF_OSCCTRL0_MODE_CRYSTAL) | (1 << AVR32_SCIF_OSCCTRL_AGC) | (1 << AVR32_SCIF_OSCCTRL0_STARTUP_128_RCOSC) | (1 << AVR32_SCIF_OSCCTRL_OSCEN);
	// Wait for the crystal to warm up
	while((AVR32_SCIF.pclksr & AVR32_SCIF_OSC0RDY_MASK) != AVR32_SCIF_OSC0RDY_MASK);
	
	// Switch to the 16 MHz crystal oscillator
	AVR32_PM.unlock = 0xAA000000 + AVR32_PM_MCCTRL;
	AVR32_PM.mcctrl = AVR32_PM_OSC0;
	
	// Setup pio for USART
	static const gpio_map_t usart_piomap = {				   \
		{AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION}, \
		{AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION}   \
	};
  
	unsigned long cpu_hz = 16000000;
	static const usart_options_t opt = {\
		//// Set options for the USART
		.baudrate = 57600,\
		.charlength = 8,\
		.paritytype = USART_NO_PARITY,\
		.stopbits = USART_1_STOPBIT,\
		.channelmode = USART_NORMAL_CHMODE\
	};	

	volatile struct avr32_usart_t *usart = &AVR32_USART1;

	gpio_enable_module(usart_piomap, sizeof(usart_piomap)/sizeof(usart_piomap[0]));
	
	// Initialize it in RS232 mode
	if(usart_init_rs232(usart, &opt, cpu_hz) != USART_SUCCESS)
		return FAILURE;

	while(1)
	{
		print(usart, "This is AVR32 saying hello from the AT32UC3C-EK!\n");
	}  

	return SUCCESS;
}


/*! \brief Print a string of characters to an usart
 *
 * \param *usart The usart to write to
 * \param *str The string of characters
 *
 * \return;
 */
void print(volatile struct avr32_usart_t * usart, char *str)
{
	while (*str != '\0')
		usart_putchar(usart, *str++);
}