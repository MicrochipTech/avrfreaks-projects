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

Installed tools.

Atmel Studio 7 (Version: 7.0.2397 - )
© 2019 Microchip Technology, Inc.
All rights reserved.


OS Version: Microsoft Windows NT 6.2.9200.0
Platform: Win32NT


Installed Packages: Shell VSIX manifest - 7.0
Shell VSIX manifest
Version: 7.0
Package GUID: e874ffe4-fbe3-4624-9a17-61014ede02d0
Company: Atmel Corporation


Installed Packages: Atmel Start - 1.0.162.0
Atmel Start
Version: 1.0.162.0
Package GUID: F8853255-9C7B-4DC2-8E0F-64D9324AEB0E
Company: Atmel


Installed Packages: Atmel Software Framework - 3.47.0.1648
ASF
Version: 3.47.0
Package GUID: 4CE20911-D794-4550-8B94-6C66A93228B8
Company: Atmel
HelpUrl: http://asf.atmel.com/3.47.0
Release Description: ASF - 3.47.0 Release

ASF
Version: 3.46.0
Package GUID: 4CE20911-D794-4550-8B94-6C66A93228B8
Company: Atmel
HelpUrl: http://asf.atmel.com/3.46.0
Release Description: ASF - 3.46.0 Release

ASF
Version: 3.45.0
Package GUID: 4CE20911-D794-4550-8B94-6C66A93228B8
Company: Atmel
HelpUrl: http://asf.atmel.com/3.45.0
Release Description: ASF - 3.45.0 Release

ASF
Version: 3.44.1
Package GUID: 4CE20911-D794-4550-8B94-6C66A93228B8
Company: Atmel
HelpUrl: http://asf.atmel.com/3.44.1
Release Description: ASF - 3.44.1 Release

ASF
Version: 3.44.0
Package GUID: 4CE20911-D794-4550-8B94-6C66A93228B8
Company: Atmel
HelpUrl: http://asf.atmel.com/3.44.0
Release Description: ASF - 3.44.0 Release

ASF
Version: 3.43.0
Package GUID: 4CE20911-D794-4550-8B94-6C66A93228B8
Company: Atmel
HelpUrl: http://asf.atmel.com/3.43.0
Release Description: ASF - 3.43.0 Release

ASF
Version: 3.42.0
Package GUID: 4CE20911-D794-4550-8B94-6C66A93228B8
Company: Atmel
HelpUrl: http://asf.atmel.com/3.42.0
Release Description: ASF - 3.42.0 Release

ASF
Version: 3.41.0
Package GUID: 4CE20911-D794-4550-8B94-6C66A93228B8
Company: Atmel
HelpUrl: http://asf.atmel.com/3.41.0
Release Description: ASF - 3.41.0 Release

ASF
Version: 3.40.0
Package GUID: 4CE20911-D794-4550-8B94-6C66A93228B8
Company: Atmel
HelpUrl: http://asf.atmel.com/3.40.0
Release Description: ASF - 3.40.0 Release

ASF
Version: 3.39.0
Package GUID: 4CE20911-D794-4550-8B94-6C66A93228B8
Company: Atmel
HelpUrl: http://asf.atmel.com/3.39.0
Release Description: ASF - 3.39.0 Release



Installed Packages: LiveWatch - 2.0.65
LiveWatch
Version: 2.0.65
Package GUID: 7DF6DCFD-2BCA-41C7-9C0E-1B7F606B008E
Company: Atmel


Installed Packages: GdbConsole - 7.0.188
GdbConsole
Version: 7.0.188
Package GUID: 49258291-0FED-4501-881B-6BAA91BEBCA8
Company: Atmel


Installed Packages: Atmel Kits - 7.0.129
Atmel Kits
Version: 7.0.129
Package GUID: 6F4B8FE4-C464-4916-8B43-AC92431C1CDF
Company: Atmel


Installed Packages: AtmelToolchainProvider - 7.0.908
AtmelToolchainProvider
Version: 7.0.908
Package GUID: AtmelToolchainProvider.Atmel.10EF9C74-D8DA-4872-85F5-D8BB3101E245
Company: Atmel


Installed Packages: Data Visualizer Extension - 2.20.735
Data Visualizer Extension
Version: 2.20.735
Package GUID: 25dc067d-df31-4e22-be7f-cc6a77ccc7f3
Company: Atmel


Installed Packages: Microchip Gallery - 8.1.0
Microchip Gallery
Version: 8.1.0
Package GUID: AtmelStudio7ExtensionManager
Company: Microchip


Installed Packages: Visual Assist for Atmel Studio - 10.9.2332.2
Visual Assist for Atmel Studio
Version: 10.9.2332.2
Package GUID: 7997A33C-B154-4b75-B2AC658CD58C9510
Company: Whole Tomato Software

This code is built based on the app note: 
http://ww1.microchip.com/downloads/en/Appnotes/FreeRTOS-on-megaAVR0series-DS00003007A.pdf
Using the START tool:
https://start.atmel.com/#examples/ATmega4809/FreeRTOS

*/

#include <avr/io.h>
#include <stdio.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stream_buffer.h"
#include "message_buffer.h"
#include "semphr.h"

/* Tasks */
#include "keyboard.h"
#include "LED.h"

/* Services */

/* Private data */

/* Private Function Prototypes */
static void system_init();

/* Function Prototypes */
void tsk_main(void *pvParams);

/* Global Data */
QueueHandle_t         led_queue;

int main(void)
{
    system_init();
	LEDs_init();

	/* Initialize global inter-task communication infrastructure */
	led_queue          = xQueueCreate(10, 1);

	/* Task Registration and creation */
	xTaskCreate(tsk_main, "main", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(tsk_LED, "LEDs", configMINIMAL_STACK_SIZE*2, NULL, 2, NULL);
	uint8_t LED_tsk_status = 1; // LED or pin to toggle in the tsk_status task
	xTaskCreate(tsk_status_LED, "stsLed", configMINIMAL_STACK_SIZE, (void*) &LED_tsk_status, 3, NULL);
	vTaskStartScheduler();
	return 0;
}

void tsk_main(void *pvParams)
{
	key_msg_t key_msg;		
	key_msg.key = 0;
	while (1)
	{
		key_msg.state = !key_msg.state; // Toggle on off
		/* Control LED */
		xQueueSendToBack(led_queue, &key_msg, portMAX_DELAY); // send - throttle in receiver.
	}
}

/* Private Methods */
static void system_init()
{
	/* Set clock prescaler to div2 */
	_PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PEN_bm);
}
