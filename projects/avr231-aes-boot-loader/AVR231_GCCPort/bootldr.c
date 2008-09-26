//=============================================================================
// Copyright (C) 2003 Atmel Corporation
//
// File:			bootldr.c
// Compiler:		IAR Atmel AVR C/EC++ Compiler
// Output Size:
// Created:			4-Feb-2003	JP (Atmel Finland)
// Modified:
//
// Support Mail:	avr@atmel.com
//
// Description:		Start point of the program execution. Customize this
//					for your own needs. Currently, if switch SW2 is
//					pressed during reset, the boot loader is switched to
//					update mode i.e. ready to receive encrypted frames and
//					program the contents into flash or eeprom memory.
//
//					Before executing the application, the boot loader checks
//					the condition of the Application Section (if CRC_CHECK is
//					defined in 'bootldr.h' which is generated using 'create'
//					tool). If Application Section is not intact, it is not
//					executed.
//
// Other Info:
//=============================================================================

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "aesglobal.h"
#include "bootldr.h"
#include "loader.h"
#include "bus.h"
#include "crc.h"


//=============================================================================
// Starts here!
//=============================================================================

int main(void)
{
    // enable pullup - give port time to settle
    set_bit(PORTD, PD2);

    busInit();

    // Loop forever (loops only if Application Section is damaged)
    for (;;)
    {
        // Key pressed? Yes -> run the loader routine
        if (!(PIND & (1 << PD2)))
        {
            loader();
        }

#if defined(CRC_CHECK)
        // Check that the Application Section contents is undamaged
        // by calculating the CRC of the whole memory.
        {
			#ifndef RAMPZ
            	uint16_t p = 0x000000;
			#else
            	uint32_t p = 0x000000;
			#endif

			uint16_t crc = 0;

            do
            {
			#ifndef RAMPZ
				crc = CRC(crc, pgm_read_byte(p));
			#else
                crc = CRC(crc, pgm_read_byte_far(p));
			#endif
            }
            while (++p < MEM_SIZE);

            // Application Section damaged
            //   -> do not jump to Reset Vector of the Application Section
            if (crc)
            {
                DDRB = 0xff;
                for(;;)
                {
                    PORTB ^= 0xff;
                    _delay_ms(1000);
                }
            }
        }
#endif

        asm volatile ("jmp 0");
    }
}


