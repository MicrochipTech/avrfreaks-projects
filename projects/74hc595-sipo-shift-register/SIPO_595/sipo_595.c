//*****************************************************************************
//
// Work File	: sipo_595.c
// Description	: Driver for 595 shift register
//				  Implementations for sipo_595.h
//
// Author       : Ivan Karežiæ
// Created      : 2011-03-03
// Revised      : 2011-03-03
// Version      : 1.0
// Target MCU	: Atmel AVR
// Uses         : avr-libc sipo_595.h
//
// This code is distributed under the GNU Public License
// which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "sipo_595.h"

void sipo_clr(){
	SRCLR_PORT &= ~SRCLR_PIN;
	RCLK_PORT &= ~RCLK_PIN;
	RCLK_PORT |= RCLK_PIN;
	RCLK_PORT &= ~RCLK_PIN;
	SRCLR_PORT |= SRCLR_PIN;
}

void sipo_init()
{
	// Set SIPO ports as output
	SIPO1_DDR |= (SRCLR_PIN | SRCLK_PIN);
	SIPO2_DDR |= (RCLK_PIN | SI_PIN);
	// Clear all registers
	sipo_clr();
}

void sipo_sendbyte(uint8_t* sipo_buffer)
{
	uint8_t i=SPIO_COUNT,j,msb_value;
	while(i>0)
	{
		--i;
		for (j=0;j<8;++j){
			msb_value=(sipo_buffer[i]&0x80);
			if(msb_value==0)
			{
				SI_PORT &= ~SI_PIN;				// DATA = 0
			}
			else
			{
				SI_PORT |= SI_PIN;				// DATA = 1
			}
			sipo_buffer[i]=sipo_buffer[i]<<1;	// Shift bits of current byte one step to the left

			// First register shift clock cycle
			SRCLK_PORT &= ~SRCLK_PIN;			// CLK = 0
			SRCLK_PORT |= SRCLK_PIN;			// CLK = 1
		}
	}

	// Latch clock cycle (copy data from first register to second)
	RCLK_PORT &= ~RCLK_PIN;						// RCLK = 0
	RCLK_PORT |= RCLK_PIN;						// RCLK = 1

	// Set data pin to low
	SI_PORT &= ~SI_PIN;
}
