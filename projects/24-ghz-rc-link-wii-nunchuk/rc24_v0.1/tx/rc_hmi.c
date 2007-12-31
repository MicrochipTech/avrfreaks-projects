
/*================================================================================
	2.4 Ghz R/C Control
	rc_hmi.h

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
#include <stdlib.h> 
#include "rc_global.h"
#include "rc_twi.h"

uint8_t avgBuf[RC_HMI_NONCOND_INPUTS][RC_HMI_AVG_BUF_SIZE];
uint8_t avgBufIndex[RC_HMI_NONCOND_INPUTS];
					
void HMI_Init(void)
{
	uint8_t i,
			txBuffer[2];

	// Initialize TWI susbsystem
	twi_init();

	// Initialize Nunchuk
	txBuffer[0] = 0x40;
	txBuffer[1] = 0x00;
    twi_writeTo(0x52, txBuffer, 2, 1);

	//Initialize circular buffers used for smoothing/averaging
	for (i = 0; i < RC_HMI_NONCOND_INPUTS; i++) {
		avgBufIndex[i] = 0;
	}
}

void HMI_GetInput(uint8_t *rxBuffer)
{
	uint8_t 	i,
				j,
				byte;
	uint16_t	tmp;

    twi_readFrom(0x52, rxBuffer, 6);

	for (i = 0; i < 6; i++) {
		rxBuffer[i] = (rxBuffer[i] ^ 0x17) + 0x17;
	}
	// Joy X
	rxBuffer[0] = ((rxBuffer[0]-RC_HMI_JOY_X_BIAS)>>1) + 100;

	// Floor
	if(rxBuffer[0] < 105) {
		rxBuffer[0] = 100;
	}
	// Ceil
	if(rxBuffer[0] > 195) {
		rxBuffer[0] = 200;
	}
	// Remove neutral bias
	if(abs(rxBuffer[0] - 150) < RC_HMI_N_BIAS_SPAN) {
		rxBuffer[0] = 150;
	}

	// Joy Y
	rxBuffer[1] = ((rxBuffer[1]-RC_HMI_JOY_Y_BIAS)>>1) + 100;
	// Floor
	if(rxBuffer[1] < 105) {
		rxBuffer[1] = 100;
	}
	// Ceil
	if(rxBuffer[1] > 195) {
		rxBuffer[1] = 200;
	}
	// Remove neutral bias
	if(abs(rxBuffer[1] - 150) < RC_HMI_N_BIAS_SPAN) {
		rxBuffer[1] = 150;
	}

	// Smooth Acceleration-X, Acceleration-Y
	for (i = 0; i < RC_HMI_NONCOND_INPUTS; i++) {
	    tmp = rxBuffer[i+2] + 25;
    
		if ( tmp < 100 ) {
			tmp = 100;
		}
		if ( tmp > 200 ) {
			tmp = 200;
		}

		// Add new sample to the buffer
		avgBuf[i][avgBufIndex[i]++] = (uint8_t)tmp;
		if( avgBufIndex[i] == RC_HMI_AVG_BUF_SIZE) {
			avgBufIndex[i] = 0;
		}

		// Get averaged sample
		j = avgBufIndex[i];
		tmp = 0;
		do {
			tmp += avgBuf[i][j++];
			if(j == RC_HMI_AVG_BUF_SIZE) {
				j = 0;
			}
		} while (j != avgBufIndex[i]);
 
		rxBuffer[i+2] = tmp/RC_HMI_AVG_BUF_SIZE;
	}
	// Reverse Acc-X for correct orientation
	rxBuffer[2] = 300 - rxBuffer[2];
	
	// Z button
	rxBuffer[4] = rxBuffer[5] & 0x01;

	// C button
	rxBuffer[4] |= rxBuffer[5] & 0x02;
  
	// Send request for next sample
	byte = 0x00;
    twi_writeTo(0x52, &byte, 1, 1);
}
