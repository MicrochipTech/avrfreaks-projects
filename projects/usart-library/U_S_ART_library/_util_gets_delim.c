/*******************************************************************************
 * File: uart_util_gets_delim.c
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
uint8_t UART_gets_delim(uint8_t device,
		uint8_t str[],
		uint8_t delim,
		uint8_t maxlen)
{
	uint8_t len = maxlen;
	while (1)
	{
		while (!UART_getchar(device, str))
			;
		--len;
		if (!len || (*str == delim))
			return maxlen - len;
		++str;
	}
}

