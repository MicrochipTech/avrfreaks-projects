/*
** Soyuz-TRX Project
** IF1-AGC Controller
** Testing software
** Target board: ATtiny84, ADG711, 74HC595, AD8369, AD8351, AD8310
** (c) kinkajou, 2008..2011
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/eeprom.h>

#include <math.h>

#include "defines.h"


void hc595(uint8_t byte)
{
	uint8_t i, d = byte;
	for (i=0; i<8; i++)
	{
		CLR_BIT(PORTA, AMPL_SCK);
		OVR_BIT(PORTB, AMPL_DATA, ((d & 0x80) != 0));
		SET_BIT(PORTA, AMPL_SCK);
		d <<= 1;
	}
	SET_BIT(PORTA, AMPL_STORE);
	CLR_BIT(PORTA, AMPL_STORE);
}

void AD8369_SetGain(uint8_t Gain)
{
	uint8_t GainCode = Gain & 0x0F;
	uint8_t VGA_B[] = 
	{
		GAIN_0, GAIN_1, GAIN_2, GAIN_3,
		GAIN_4, GAIN_5, GAIN_6, GAIN_7,
		GAIN_8, GAIN_9, GAIN_10, GAIN_11,
		GAIN_12, GAIN_13, GAIN_14, GAIN_15
	};
	hc595(FGAPWUP | LOGPWUP | VGAPWUP | VGA_B[GainCode] | nVGAENB);
	hc595(FGAPWUP | LOGPWUP | VGAPWUP | VGA_B[GainCode] ); // negative pulse on nVGAENB
	hc595(FGAPWUP | LOGPWUP | VGAPWUP | VGA_B[GainCode] | nVGAENB);
}


void led_on(void)
{
	hc595(VGA_B0);
}

void led_off(void)
{
	hc595(0);
}

volatile uint8_t dummy = 1, dummy2 = 1;;

void led_tgl(void)
{
	if (dummy2)
	{
		led_on();
		dummy2 = 0;
	}
	else
	{
		led_off();
		dummy2 = 1;
	}
}

void ProcessData (uint8_t Data)
{
	if (dummy == 1)
	{
		AD8369_SetGain(13); // VGA on
		dummy = 0;
	}
	else
	{
		hc595(FGAPWUP | LOGPWUP ); // VGA off
		dummy = 1;
	}
}

void I2C_SoftwareSlave_Init();

uint8_t eeprom_stored_cfg[2] __attribute__ ((section (".eeprom"))) = { 6, ~13 };

int main(void)
{
	uint8_t Gain = 0;
	uint8_t B0, B1;


	DDRB	|= (1<<AMPL_DATA);
	PORTB	= 0x00;
	DDRA	|= ((1<<AMPL_SCK) | (1<<AMPL_STORE) | (1<<AMPL_nRESET));
	PORTA	|= (1<<AMPL_nRESET);

	hc595(0);

	I2C_SoftwareSlave_Init();
	sei();

	/*
	while (!eeprom_is_ready()) asm ("nop");
	B0 = eeprom_read_byte (&eeprom_stored_cfg[0]);
	while (!eeprom_is_ready()) asm ("nop");
	B1 = eeprom_read_byte (&eeprom_stored_cfg[1]);
	if (B0 != ~B1)
	{
		while (!eeprom_is_ready()) asm ("nop");
		eeprom_write_byte (&eeprom_stored_cfg[0], 13);
		while (!eeprom_is_ready()) asm ("nop");
		eeprom_write_byte (&eeprom_stored_cfg[1], ~13);
	}
	*/
	while (!eeprom_is_ready()) asm ("nop");
	Gain = eeprom_read_byte (&eeprom_stored_cfg[0]);
	//AD8369_SetGain(Gain); // -> maximum
	AD8369_SetGain(13);

	for (;;)
	{
		//AD8369_SetGain(Gain);
		//_delay_ms(3000);
		//Gain++;
		//if (Gain == 16) { Gain = 0; }

	};

	return 0;
}
