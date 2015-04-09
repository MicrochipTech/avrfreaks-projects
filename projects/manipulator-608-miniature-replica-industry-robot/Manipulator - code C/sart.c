/*
 * usart.c
 *
 * Created: 2013-01-17 21:39:25
 *  Author: tmf
 */ 

/**************************
 * --->>  Includes  <<--- *
 **************************/

#include "usart.h"
#include <avr/pgmspace.h>
#include "hd44780.h"
#include <util/delay.h>
#include "wdt_driver.h"
#include "Globals_Defs.h"


/***************************
 * --->>  Functions  <<--- *
 ***************************/

bool usart_set_baudrate(USART_t * const usart, uint32 baud, uint32 cpu_hz)
{
	int8 exp;
	uint32 div;
	uint32 limit;
	uint32 ratio;
	uint32 min_rate;
	uint32 max_rate;

	/*
	 * Check if the hardware supports the given baud rate
	 */
	/* 8 = (2^0) * 8 * (2^0) = (2^BSCALE_MIN) * 8 * (BSEL_MIN) */
	max_rate = cpu_hz / 8;
	/* 4194304 = (2^7) * 8 * (2^12) = (2^BSCALE_MAX) * 8 * (BSEL_MAX+1) */
	min_rate = cpu_hz / 4194304;

	if (!((usart)->CTRLB & USART_CLK2X_bm)) {
		max_rate /= 2;
		min_rate /= 2;
	}

	if ((baud > max_rate) || (baud < min_rate)) {
		return false;
	}

	/* Check if double speed is enabled. */
	if (!((usart)->CTRLB & USART_CLK2X_bm)) {
		baud *= 2;
	}

	/* Find the lowest possible exponent. */
	limit = 0xfffU >> 4;
	ratio = cpu_hz / baud;

	for (exp = -7; exp < 7; exp++) {
		if (ratio < limit) {
			break;
		}

		limit <<= 1;

		if (exp < -3) {
			limit |= 1;
		}
	}

	/*
	 * Depending on the value of exp, scale either the input frequency or
	 * the target baud rate. By always scaling upwards, we never introduce
	 * any additional inaccuracy.
	 *
	 * We are including the final divide-by-8 (aka. right-shift-by-3) in
	 * this operation as it ensures that we never exceeed 2**32 at any
	 * point.
	 *
	 * The formula for calculating BSEL is slightly different when exp is
	 * negative than it is when exp is positive.
	 */
	if (exp < 0) {
		/* We are supposed to subtract 1, then apply BSCALE. We want to
		 * apply BSCALE first, so we need to turn everything inside the
		 * parenthesis into a single fractional expression.
		 */
		cpu_hz -= 8 * baud;

		/* If we end up with a left-shift after taking the final
		 * divide-by-8 into account, do the shift before the divide.
		 * Otherwise, left-shift the denominator instead (effectively
		 * resulting in an overall right shift.)
		 */
		if (exp <= -3) {
			div = ((cpu_hz << (-exp - 3)) + baud / 2) / baud;
		} else {
			baud <<= exp + 3;
			div = (cpu_hz + baud / 2) / baud;
		}
	} else {
		/* We will always do a right shift in this case, but we need to
		 * shift three extra positions because of the divide-by-8.
		 */
		baud <<= exp + 3;
		div = (cpu_hz + baud / 2) / baud - 1;
	}

	(usart)->BAUDCTRLB = (uint8)(((div >> 8) & 0X0F) | (exp << 4));
	(usart)->BAUDCTRLA = (uint8)div;

	return true;
}

void USART_putchar(USART_t * const usart, char ch)
{
	while(!(usart->STATUS & USART_DREIF_bm));
	usart->DATA=ch;
}

void USART_send(USART_t * const usart, const char *txt)
{
	while(*txt)
	{
		USART_putchar(usart, *txt);
		++txt;
	}
}

void USART_send_F(USART_t * const usart, const char *txt)
{
	char tmp;
	while((tmp=pgm_read_byte(txt)))
	{
		USART_putchar(usart, tmp);
		++txt;
	}
}

void USART_send_block(USART_t * const usart, const uint8 *block, uint8 size)
{
	while(size--)
	{
		USART_putchar(usart, *block);
		++block;
	}
}

void USART_send_block_F(USART_t * const usart, const uint8 *block, uint8 size)
{
	while(size--)
	{
		USART_putchar(usart, pgm_read_byte(block));
		++block;
	}
}

void USART_send_int(USART_t* const usart, const uint16 data)
{
	USART_putchar(usart, (data));
	while(!(usart->STATUS & USART_DREIF_bm));
	USART_putchar(usart, data>>8);
}

void USART_MergingData(void)
{
	uint16 static usart_data = 0;

	if (usart_i == 2)
	{
		usart_i = 0;
	}
	if ( usart_i == 0)
	{
		usart_data = USARTC1.DATA;
	}
	else if (usart_i == 1)
	{
		if (buf_index == RING_BUFFER_LENGTH) /* Very simple ring buffer */
		{
			buf_index = 0;
		}
		usart_data += (USARTC1.DATA<<8);
		if (RxBuf[buf_index] == 0)
		{
/**********************************************************************
 * 						 EMERGENCY STOP 							  *
 **********************************************************************/
			if (usart_data == 65530)
			{
				TCE0.CCABUF = 0;
				TCE0.CCBBUF = 0;
				TCE0.CCCBUF = 0;
				TCE0.CCDBUF = 0;
				TCF0.CCABUF = 0;
				TCF0.CCBBUF = 0;
				TCF0.CCCBUF = 0;
				Pid[0].PozRef = Pid[0].ActPoz = Pid[0].PositionBuffor = A; /* Rewrite all position variables in PID regulator */
				Pid[1].PozRef = Pid[1].ActPoz = Pid[1].PositionBuffor = B;
				Pid[2].PozRef = Pid[2].ActPoz = Pid[2].PositionBuffor = C;
				Pid[3].PozRef = Pid[3].ActPoz = Pid[3].PositionBuffor = D;
				Pid[4].PozRef = Pid[4].ActPoz = Pid[4].PositionBuffor = E;
				Pid[5].PozRef = Pid[5].ActPoz = Pid[5].PositionBuffor = F;
				M1_STOP;
				M2_STOP;
				M3_STOP;
				M4_STOP;
				M5_STOP;
				M6_STOP;
				M7_STOP;
				BUZZER_HORN_ENABLE;
				ERROR_LED_ON;
				LcdClear();
				LcdWrite("     ERROR    ");
				LcdGoto(0,1);
				LcdWrite("  RESET DEVICE");
				while (1)
				{
					WDT_Reset(); /*Feed dog*/
				}
			}
			RxBuf[buf_index] = usart_data;
			buf_index ++;
		}
		else
		{
			Error();
		}
	}
	usart_i ++;
}

void Error(void)
{
	while (1)
	{
		USART_send_int(&USARTC1, 666);
	}
}

