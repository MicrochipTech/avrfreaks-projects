/*================================================================================
	2.4 Ghz R/C Control
	rc_timer.h

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
#ifndef _RC_TIMER_H_
#define _RC_TIMER_H_

void Timer_Init(void);

extern volatile uint8_t timerIRQ;
#endif

