/*******************************************************************************
 * File: uart_util_puts.c
 * Author: Artur Lipowski <LAL@pro.onet.pl>
 * Version: 3.0, May 27 2006
 * Copyright 2003-2006 Artur Lipowski
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *******************************************************************************/

#include <uart_util.h>

/*******************************************************************************
 *
 *
 *
 * *****************************************************************************/
void UART_puts(uint8_t device, const uint8_t str[])
{
    while(*str)
	{
		while(!UART_putchar(device, *str))
			;
		++str;
	}
}

