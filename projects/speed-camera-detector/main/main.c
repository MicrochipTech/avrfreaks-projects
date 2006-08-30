/**
 * \GPIO_Example
 * 
 * \This example shows how to manually configure a GPIO pin as an output.
 * \This project was written for AVR Studio 5 and the AT32UC3C-EK development board
 *
 * \GPIO pins on the UC3 devices are not arranged in software by the port groups (A, B, C...) shown on the datasheet.
 * \Instead the GPIO pins are packed into GPIO Ports by their GPIO number. The GPIO numbers start at 0 for PORTA and count upwards including pins that don't actually exist.
 * \For example, on the AT32UC3C0512C, PORTA30 and PORTA31 don't exist...the count starts at 0 for PORTA0 and increases to 29 for PORTA29. PORTB0 is assigned 32, though, because
 * \PORTA30 and PORTA31 are included in the count. All of this is handled for you by including the avr32/io.h file and using the AVR32_PIN_Pxxx defines for your GPIO pin.
 */

#include <delay.h>
#include <avr32/io.h>

#define LED0_PIN	AVR32_PIN_PA08
#define LED1_PIN	AVR32_PIN_PD23
#define LED2_PIN	AVR32_PIN_PC13
#define LED3_PIN	AVR32_PIN_PD22
#define RCSYS_HZ	115000

int main (void)
{
	delay_init(RCSYS_HZ);

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
	// Set the output value to low
	led0_gpio_port->ovrc = 1 << LED0_PIN;
	// Enable the output driver
	led0_gpio_port->oders = 1 << LED0_PIN;
	
	// Setup the port options for LED1
	volatile avr32_gpio_port_t *led1_gpio_port = &AVR32_GPIO.port[LED1_PIN/32];
	led1_gpio_port->oderc = 1 << LED1_PIN;
	led1_gpio_port->gpers = 1 << LED1_PIN;
	led1_gpio_port->puerc = 1 << LED1_PIN;
	led1_gpio_port->pderc = 1 << LED1_PIN;
	led1_gpio_port->odcr0c = 1 << LED1_PIN;
	led1_gpio_port->ovrs = 1 << LED1_PIN;
	led1_gpio_port->oders = 1 << LED1_PIN;
	
	// Setup the port options for LED2
	volatile avr32_gpio_port_t *led2_gpio_port = &AVR32_GPIO.port[LED2_PIN/32];
	led2_gpio_port->oderc = 1 << LED2_PIN;
	led2_gpio_port->gpers = 1 << LED2_PIN;
	led2_gpio_port->puerc = 1 << LED2_PIN;
	led2_gpio_port->pderc = 1 << LED2_PIN;
	led2_gpio_port->odcr0c = 1 << LED2_PIN;
	led2_gpio_port->ovrs = 1 << LED2_PIN;
	led2_gpio_port->oders = 1 << LED2_PIN;
	
	// Setup the port options for LED3
	volatile avr32_gpio_port_t *led3_gpio_port = &AVR32_GPIO.port[LED3_PIN/32];
	led3_gpio_port->oderc = 1 << LED3_PIN;
	led3_gpio_port->gpers = 1 << LED3_PIN;
	led3_gpio_port->puerc = 1 << LED3_PIN;
	led3_gpio_port->pderc = 1 << LED3_PIN;
	led3_gpio_port->odcr0c = 1 << LED3_PIN;
	led3_gpio_port->ovrs = 1 << LED3_PIN;
	led3_gpio_port->oders = 1 << LED3_PIN;
	
	uint16_t led_mask = 0x0001;
	bool up = true;
	
	while(1)
	{
		// If LED0 is lit, pause a bit longer for effect
		if((led0_gpio_port->pvr & (1 << LED0_PIN)) != (1 << LED0_PIN))
		{
			delay_ms(900);
		}

		delay_ms(100);
		// Clear all the LEDs
		led0_gpio_port->ovrs = 1 << LED0_PIN;
		led1_gpio_port->ovrs = 1 << LED1_PIN;
		led2_gpio_port->ovrs = 1 << LED2_PIN;
		led3_gpio_port->ovrs = 1 << LED3_PIN;
		
		// Set the LEDs indicated by led_mask
		led0_gpio_port->ovrc = (led_mask & 0x0001) << LED0_PIN;
		led1_gpio_port->ovrc = ((led_mask>>4) & 0x0001) << LED1_PIN;
		led2_gpio_port->ovrc = ((led_mask>>8) & 0x0001) << LED2_PIN;
		led3_gpio_port->ovrc = ((led_mask>>12) & 0x0001) << LED3_PIN;
		
		if(up)
		{
			led_mask = led_mask << 4;
			if(led_mask == 0x1000)
			{
				up = false;
			}	
		}
		else
		{
			led_mask = led_mask >> 4;
			if(led_mask == 0x0001)
			{
				up = true;
			}
		}
	}
}
