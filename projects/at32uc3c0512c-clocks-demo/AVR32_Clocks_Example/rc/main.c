/**
 * \AVR32 Clocks Example
 *
 * \This project demonstrates the different types of clocks available on the AT32UC3C0512C chip.
 * \There are several layers of clock control from enabling the hardware in the SCIF, choosing your main clock source in the PM, and setting up clock divisors.
 */
#include <avr32/io.h>


#define LED0_PIN	AVR32_PIN_PA08

void init_led0();
void blink_led0();

int main (void)
{
	// Unlock the OSCCTRL0 register by writing its address and the key value to the SCIF unlock register
	AVR32_SCIF.unlock = 0xAA000000 + AVR32_SCIF_OSCCTRL;
	// Setup oscillator 0 for a crystal oscillator, automatic gain control, 128 RCSYS clocks of startup time, and enable oscillator 0
	AVR32_SCIF.oscctrl[0] = (AVR32_SCIF_OSCCTRL0_MODE_CRYSTAL) | (1 << AVR32_SCIF_OSCCTRL_AGC) | (1 << AVR32_SCIF_OSCCTRL0_STARTUP_128_RCOSC) | (1 << AVR32_SCIF_OSCCTRL_OSCEN);
	// Wait for the crystal to warm up
	while((AVR32_SCIF.pclksr & AVR32_SCIF_OSC0RDY_MASK) != AVR32_SCIF_OSC0RDY_MASK);
	
	init_led0();
	int counter=0;
	
	// Start with the 115 kHz RCSYS oscillator.
	// After 2 blinks of the LED move to the 8 MHz internal oscillator divided down to 32 kHz
	// Immediately start decreasing the clock divisor until there is no division
	// Finally move to the 16 MHz external crystal
	while(1)
	{			
		// Add a little delay to see the different clock speeds
		for(int i=0; i<20000; i++);
			
		blink_led0();
		
		if(counter == 4)
		{
			// Unlock the CPU clock select register
			AVR32_PM.unlock = 0xAA000000 + AVR32_PM_CPUSEL;
			// Set the CPU bus clock division factor to maximum (2^8)
			AVR32_PM.cpusel = (1 << AVR32_PM_CPUSEL_CPUDIV) + 7;
			
			// Unlock the main clock control register
			AVR32_PM.unlock = 0xAA000000 + AVR32_PM_MCCTRL;
			// Select the 8 MHz internal oscillator as the main clock source
			AVR32_PM.mcctrl = AVR32_PM_RCOSC8;
		}
		if((counter > 4) && (counter < 12))
		{
			// Unlock the CPU clock select register
			AVR32_PM.unlock = 0xAA000000 + AVR32_PM_CPUSEL;
			// Set the CPU bus clock division factor to maximum (2^8)
			AVR32_PM.cpusel = (1 << AVR32_PM_CPUSEL_CPUDIV) + (7 - (counter-4));
		}
		if(counter == 24)
		{
			// Switch to the 16 MHz crystal oscillator
			AVR32_PM.unlock = 0xAA000000 + AVR32_PM_MCCTRL;
			AVR32_PM.mcctrl = AVR32_PM_OSC0;
		}
		
		counter++;
	}
}

// This function initializes GPIO PA8 to drive an LED
void init_led0()
{
	// find the port the pin belongs to and create a variable for it
	volatile avr32_gpio_port_t *led0_gpio_port = &AVR32_GPIO.port[LED0_PIN/32];
	
	// Setup the port options ford LED0 (note that using a register name with 'c' following addresses the clear function of the register, an 's' for set, and a 't' for toggle)
	// Disable the output driver first to ensure there are no spurious outputs
	led0_gpio_port->oderc = 1 << LED0_PIN;
	// Enable the GPIO function of the pin
	led0_gpio_port->gpers = 1 << LED0_PIN;
	// Disable pullups
	led0_gpio_port->puerc = 1 << LED0_PIN;
	// Disable pulldowns
	led0_gpio_port->pderc = 1 << LED0_PIN;
	// Set the output driver to the lowest strength
	led0_gpio_port->odcr0c = 1 << LED0_PIN;
	// Set the output value to high to turn off the LED
	led0_gpio_port->ovrs = 1 << LED0_PIN;
	// Enable the output driver
	led0_gpio_port->oders = 1 << LED0_PIN;
}

// This function toggles GPIO PA8
void blink_led0()
{
	AVR32_GPIO.port[LED0_PIN/32].ovrt = 1 << LED0_PIN;
}