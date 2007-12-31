/*================================================================================
	2.4 Ghz R/C Control
	rc_rx.c

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
#include "rc_pwm.h"
#include "rc_rf.h"
#include "rc_rx.h"

#ifdef _DEBUG
#include "rc_serial.h"
#endif

int main(void)
{	
#ifdef _DEBUG
	// Initialize serial communication subsystem
	Serial_Init(RC_SERIAL_BAUD);
#endif

	// Inilialize Timer and PWM generation subsystem
	PWM_Init();

	// Initialize RF (RX) communication subsystem
	RF_RX_Init();

	// Enable interrupts
	sei ();

#ifdef _DEBUG
	DDRB |= _BV(RC_DEBUG_PIN_1);
	PORTB &= ~(_BV(RC_DEBUG_PIN_1));

	DDRD |= _BV(RC_DEBUG_PIN_2);
	PORTD &= ~(_BV(RC_DEBUG_PIN_2));

	Serial_PrintStr("Initialized\r\n");
#endif
	
	// Run forrest run...
	RC_RX_Run();

	return (0);
}

void RC_RX_Run(void)
{
	uint8_t 	i,
				plWidth,
				rxBuffer[RC_RF_RX_PLOAD_WIDTH];
	uint16_t 	failSafeCount = RC_RF_FAILSAFE_COUNT - 1;
	
	while( 1 ) {
		if(timerIRQ) {
        	if(++failSafeCount == RC_RF_FAILSAFE_COUNT) {
            	// Activate fail safe

    			// Cut off throttle
    			PWM_Update_Val(RC_CH_THROTTLE, 0);

    			// Set steering straight
    			PWM_Update_Val(RC_CH_STEERING, RC_PWM_DEFAULT_PWM);
			}
			timerIRQ = 0;
    	}

		if(nRfIRQ) {
    		switch(nRfIRQ) {

				case L01_MASK_RX_DR_FLAG:

    			// Disable RX mode
    			PORTB &= ~_BV(RC_RF_PIN_CE);

    			do {
        			// Receive payload
        			plWidth = L01_RD_RX_Pload(rxBuffer);

#ifdef _DEBUG
        			for (i = 0; i < plWidth; i++) {
        				Serial_PrintDec(rxBuffer[i]);
        				Serial_Write('\t');
        			}
        			Serial_PrintStr("\r\n");
#endif
        			// Update Steering
        			PWM_Update_Val(RC_CH_STEERING, rxBuffer[2]);

        			// Update throttle
        			PWM_Update_Val(RC_CH_THROTTLE, rxBuffer[1]);

        			//Reset fail safe timer
                   failSafeCount = 0;

    			} while ( !(L01_GET_FIFO() & L01_MASK_RX_FIFO_EMPTY) );

    			// Clear IRQ flag
    			L01_CLEAR_IRQ(L01_MASK_RX_DR_FLAG);

    			// Enable RX mode
    			PORTB |= _BV(RC_RF_PIN_CE);

				break;

				default:
				break;
			}
			nRfIRQ = 0;		
		}
	}
}
