/*******************************************************************************
 * File: uart_util_gets.c
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
 *******************************************************************************/
void UART_gets(uint8_t device, uint8_t str[], uint8_t maxlen)
{
	while (maxlen)
	{
		while (0 == UART_getchar(device, str))
			;
		++str;
		--maxlen;
	}
}


