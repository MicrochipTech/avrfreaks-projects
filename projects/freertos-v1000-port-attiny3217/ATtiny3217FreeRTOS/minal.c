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
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "stream_buffer.h"

#include "terminal.h"
#include "clock_interface.h"

/* UART Defines */
#define BAUD_RATE (9600)

#define TERMINAL_HW USART0
#define TERMINAL_RX_VECTOR USART0_RXC_vect 
#define TERMINAL_TX_PORT PORTC
#define TERMINAL_TX_PIN PIN0_bm

/* Private Data */
static char tx_buff[50];
static char rx_buffer[3];

/* Static Method Declarations */
static void uart_init(void);
static bool uart_tx_is_ready(void);
static void uart_put_c(char c);
static void uart_put_s(char const *str);
static bool get_number(uint8_t *result);
static void report_timeout(void);
static void report_unknown_command(void);
static void report_time_set(void);

/* Global Methods */
void tsk_term_tx(void *pvParameters)
{
	uart_init();

	while (xMessageBufferReceive(terminal_tx_buffer, tx_buff, 50, portMAX_DELAY) > 0) {
		uart_put_s(tx_buff);
	}
}

void tsk_term_rx(void *pvParams)
{
	while (xStreamBufferReceive(terminal_rx_buffer, rx_buffer, 1, portMAX_DELAY) > 0) {
		switch (rx_buffer[0]) {
			uint8_t number;
		case 'h':
			if (!get_number(&number)) {
				report_timeout();
				break;
			}
			clock_new_hour(number);
			break;
		case 'm':
			if (!get_number(&number)) {
				report_timeout();
				break;
			}
			clock_new_minute(number);
			break;
		case 's':
			if (!get_number(&number)) {
				report_timeout();
				break;
			}
			clock_new_second(number);
			break;
		case 'e':
			clock_update_time();
			report_time_set();
			break;
		default:
			report_unknown_command();
			break;
		}
	}
}

/* Private Methods */
static bool get_number(uint8_t *result)
{
	xStreamBufferSetTriggerLevel(terminal_rx_buffer, 2);
	uint8_t num_bytes_received = xStreamBufferReceive(terminal_rx_buffer, rx_buffer, 2, pdMS_TO_TICKS(100));
	xStreamBufferSetTriggerLevel(terminal_rx_buffer, 1);

	/* Terminate string */
	rx_buffer[2] = 0;
	*result      = atoi(rx_buffer);

	return num_bytes_received == 2;
}

static const char time_set_msg[] = "Time Set\n";
static void       report_time_set(void)
{
	xMessageBufferSend(terminal_tx_buffer, time_set_msg, sizeof(time_set_msg), 0);
}

static const char timed_out_msg[] = "TIMED OUT\n";
static void       report_timeout(void)
{
	xMessageBufferSend(terminal_tx_buffer, timed_out_msg, sizeof(timed_out_msg), 0);
}

static const char unknown_command_msg[] = "UNKNOWN COMMAND\n";
static void       report_unknown_command(void)
{
	xMessageBufferSend(terminal_tx_buffer, unknown_command_msg, sizeof(unknown_command_msg), 0);
}

static uint8_t uart_get_c(void)
{
	return TERMINAL_HW.RXDATAL;
}

static void uart_init(void)
{
	TERMINAL_TX_PORT.DIRSET = TERMINAL_TX_PIN;

	TERMINAL_HW.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	TERMINAL_HW.BAUD  = (configCPU_CLOCK_HZ * 64UL) / (BAUD_RATE * 16UL);
	TERMINAL_HW.CTRLA = USART_RXCIE_bm;
}

static bool uart_tx_is_ready(void)
{
	return TERMINAL_HW.STATUS & USART_DREIF_bm;
}

void uart_put_c(char c)
{
	while (!uart_tx_is_ready())
		;
	TERMINAL_HW.TXDATAL = c;
}

void uart_put_s(char const *str)
{
	while (*str) {
		uart_put_c(*str);
		str++;
	}
}

/* Interrupt Service Routines */
ISR(TERMINAL_RX_VECTOR)
{
	BaseType_t xHigherPriorityTaskWoken;
	uint8_t    data = uart_get_c();

	xStreamBufferSendFromISR(terminal_rx_buffer, &data, sizeof(data), &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE) {
		taskYIELD_FROM_ISR();
	}
}
