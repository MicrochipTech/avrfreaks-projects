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

#include "keyboard.h"

/* Private Data Types */
typedef enum { KEY_UNCHANGED, KEY_UP, KEY_DOWN } sw_edge_t;

typedef struct {
	PORT_t *port;
	uint8_t pin;
	uint8_t state;
} key_t;

/* Private Data */
static key_t keys[] = {
    {.port = &PORTB, .pin = 2},

};
static const uint8_t num_keys = sizeof(keys) / sizeof(key_t);

/* Private Function Declaration */
static void      keys_init(void);
static bool      key_get_state(uint8_t key_num);
static sw_edge_t key_get_edge(uint8_t key_num);

/* Global Function */
void tsk_keyboard(void *pvParams)
{
	keys_init();

	while (1) {
		for (uint8_t key_num = 0; key_num < num_keys; key_num++) {
			sw_edge_t edge = key_get_edge(key_num);
			if (edge != KEY_UNCHANGED) {
				key_msg_t key_msg;
				key_msg.key   = key_num;
				key_msg.state = (edge == KEY_DOWN ? 1 : 0);
				xQueueSendToBack(key_queue, &key_msg, 0);
			}
		}

		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/* Private Methods */
static void keys_init(void)
{
	for (uint8_t key = 0; key < num_keys; key++) {
		key_t   this_key = keys[key];
		PORT_t *port     = this_key.port;
		uint8_t pin      = this_key.pin;

		/* Read initial state */
		this_key.state = key_get_state(key);

		/* Enable pull-up on all key pins */
		(&(port->PIN0CTRL))[pin] = PORT_PULLUPEN_bm;
	}
}

static bool key_get_state(uint8_t key_num)
{
	key_t   this_key = keys[key_num];
	PORT_t *port     = this_key.port;
	uint8_t pin_bm   = (1 << this_key.pin);

	return !(port->IN & pin_bm);
}

static sw_edge_t key_get_edge(uint8_t key_num)
{
	bool      key_state = key_get_state(key_num);
	sw_edge_t ret_val   = KEY_UNCHANGED;

	if (key_state != keys[key_num].state) {
		ret_val = key_state ? KEY_DOWN : KEY_UP;
	}
	keys[key_num].state = key_state;

	return ret_val;
}
