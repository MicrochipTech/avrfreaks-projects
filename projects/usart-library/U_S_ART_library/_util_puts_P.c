/*******************************************************************************
 * File: uart_util_puts_P.c
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
void UART_puts_P(uint8_t device, PGM_P str)
{
	while (1)
	{
		uint8_t a_char = pgm_read_byte(str);
		if (!a_char) return; 
		while (!UART_putchar(device, a_char))
			;
		++str;
	}
}

