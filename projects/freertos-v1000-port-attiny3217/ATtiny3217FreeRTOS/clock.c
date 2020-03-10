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
#include <stdio.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "oled.h"

#include "clock.h"
#include "clock_interface.h"

/* Private Data Types */
typedef struct {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
} time_t;

/* Private Data */
static time_t time = {.hours = 0, .minutes = 0, .seconds = 0};
static char   time_str[9];
static time_t new_time;
static bool   should_update_time = false;

/* Private Function Declarations */
static void time_tick(volatile time_t *time);

/* Global Methods */
void tsk_clk(void *pvParams)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while (1) {
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));

		if (should_update_time) {
			should_update_time = false;
			time               = new_time;
		} else {
			time_tick(&time);
		}

		sprintf(time_str, "%02u:%02u:%02u", time.hours, time.minutes, time.seconds);
		if (xSemaphoreTake(oled_semaphore, pdMS_TO_TICKS(500)) == pdTRUE) {
			oled_write(time_str, 0);
			xSemaphoreGive(oled_semaphore);
		}
	}
}

void clock_new_hour(uint8_t hours)
{
	new_time.hours = hours % 24;
}

void clock_new_minute(uint8_t minutes)
{
	new_time.minutes = minutes % 60;
}

void clock_new_second(uint8_t seconds)
{
	new_time.seconds = seconds % 60;
}

void clock_update_time(void)
{
	should_update_time = true;
}

/* Private Methods */
static void time_tick(volatile time_t *time)
{
	time->seconds++;
	if (time->seconds >= 60) {
		time->seconds = 0;
		time->minutes++;
	}

	if (time->minutes >= 60) {
		time->minutes = 0;
		time->hours++;
	}

	if (time->hours >= 24) {
		time->hours = 0;
	}
}
