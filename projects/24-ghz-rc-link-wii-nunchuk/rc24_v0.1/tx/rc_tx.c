/*================================================================================
	2.4 Ghz R/C Control
	rc_tx.c

	Copyright ©2007 Ajay Grewal

THIS SOFTWARE AND DOCUMENTATION IS PROVIDED "AS IS," AND COPYRIGHT HOLDER MAKES NO
REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO,
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR PURPOSE OR THAT THE
USE OF THE SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE ANY THIRD PARTY PATENTS,
COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.

COPYRIGHT HOLDER WILL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL OR
CONSEQUENTIAL DAMAGES ARISING OUT OF ANY USE OF THE SOFTWARE OR DOCUMENTATION.

You may freely copy and redistribute this software if no fee is charged for use,
copying or distribution.  Such redistributions must retain the above copyright
notice and disclaimer.
================================================================================*/
#include <stdint.h> 
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "rc_global.h"
#include "rc_timer.h"
#include "rc_rf.h"
#include "rc_serial.h"
#include "rc_hmi.h"
#include "rc_tx.h"

volatile uint8_t IRQSource = 0;

int main(void)
{	

#ifdef _DEBUG
	Serial_Init(RC_SERIAL_BAUD);
#endif

	// Initialize timer susbsystem
	Timer_Init();

	// Initialize RF (TX) communication subsystem
    RF_TX_Init();

	// Enable interrupts
	sei ();

	// Inilialize HMI subsystem
	HMI_Init();

#ifdef _DEBUG
	DDRB |= _BV(RC_DEBUG_PIN_1);
	PORTB &= ~(_BV(RC_DEBUG_PIN_1));

	DDRD |= _BV(RC_DEBUG_PIN_2);
	PORTD &= ~(_BV(RC_DEBUG_PIN_2));

	Serial_PrintStr("Initialized\r\n");
#endif

	// Run forrest run...
	RC_TX_Run();

	return (0);
}

void RC_TX_Run(void)
{
	uint8_t 	i,
				pulseCount,
				pBuf[RC_HMI_TOTAL_INPUTS];
	uint16_t 	hmiReadCount = RC_HMI_READCOUNT - 1;

	while ( 1 ) {

    	if (timerIRQ) {
    		if(++hmiReadCount == RC_HMI_READCOUNT) {
    			// Get data
    			HMI_GetInput(pBuf);
#ifdef _DEBUG
				for (i = 0; i < RC_HMI_NUM_CHANNELS; i++) {
					Serial_PrintDec(pBuf[i]);
					Serial_Write('\t');
				}
				Serial_PrintStr("\r\n");
#endif
				// Enable CE
				PORTB |= _BV(RC_RF_PIN_CE);

				L01_WR_TX_Pload(pBuf,RC_HMI_NUM_CHANNELS);

				// Disable CE
				PORTB &= ~_BV(RC_RF_PIN_CE);
			  
    			hmiReadCount = 0;
#ifdef _DEBUG
				PORTB &= ~_BV(RC_DEBUG_PIN_1);
				PORTD &= ~(_BV(RC_DEBUG_PIN_2));
#endif
    		}
			timerIRQ = 0;
        }

		if(nRfIRQ) {
			switch(nRfIRQ) {
    			case L01_MASK_RX_DR_FLAG:
    				L01_CLEAR_IRQ(L01_MASK_RX_DR_FLAG);
    			break;

    			case L01_MASK_TX_DS_FLAG:
    				L01_CLEAR_IRQ(L01_MASK_TX_DS_FLAG);
    			break;

    			case L01_MASK_MAX_RT_FLAG:
    				L01_CLEAR_IRQ(L01_MASK_MAX_RT_FLAG);
    			break;

    			default:
    			break;
			}
			nRfIRQ = 0;
		}
	}
}
