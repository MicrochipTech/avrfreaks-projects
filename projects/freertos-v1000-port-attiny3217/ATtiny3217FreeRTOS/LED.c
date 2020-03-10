/*
(C) 2018 Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software and
any derivatives exclusively with Microchip products. It is your responsibility
to comply with third party license terms applicable to your use of third party
software (including open source software) that may accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS,
IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES
OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER
RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF
THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWED
BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS
SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY
TO MICROCHIP FOR THIS SOFTWARE.
*/

#include <avr/io.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "LED.h"

/* Private Data Types */
typedef struct {
	PORT_t *port;
	uint8_t pin;
} led_t;

/* Private Data  define which pin to control */
static const led_t LEDs[] = {
    {.port = &PORTB, .pin = PIN4_bm}, // PB4 = LED on attiny3217 Xplained 
    {.port = &PORTB, .pin = PIN3_bm}, // PB3

};
static const uint8_t num_LEDs = sizeof(LEDs) / sizeof(led_t);

/* Private Function Declarations */
static void LED_on(uint8_t LED);
static void LED_off(uint8_t LED);
static void LED_toggle(uint8_t LED);
static void LED_set_state(uint8_t LED, bool state);

/* Global Function */
void tsk_LED(void *pvParams)
{
	LED_request_t LED_request; // LED_request.LED, .state
	while (1)
	{
		if (pdPASS == xQueueReceive(led_queue, &LED_request, portMAX_DELAY)) 
		{
			LED_set_state(LED_request.LED, LED_request.state);
			vTaskDelay(  pdMS_TO_TICKS(5) ); 
		}
	}
}

void tsk_status_LED(void *pvParams)
{
	uint8_t LED = *(uint8_t*)pvParams; // Cast it. 
	while (1) {
		LED_toggle((uint8_t) LED);
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}

void LEDs_init(void)
{
	for (uint8_t LED = 0; LED < num_LEDs; LED++) {
		led_t this_led = LEDs[LED];
		LED_off(LED);
		this_led.port->DIRSET = this_led.pin;
	}
}

/* Private Methods */
static void LED_on(uint8_t LED)
{
	led_t this_led        = LEDs[LED];
	this_led.port->OUTCLR = this_led.pin;
}

static void LED_off(uint8_t LED)
{
	led_t this_led        = LEDs[LED];
	this_led.port->OUTSET = this_led.pin;
}

static void LED_toggle(uint8_t LED)
{
	led_t this_led        = LEDs[LED];
	this_led.port->OUTTGL = this_led.pin;
}

static void LED_set_state(uint8_t LED, bool state)
{
	state ? LED_on(LED) : LED_off(LED);
}
