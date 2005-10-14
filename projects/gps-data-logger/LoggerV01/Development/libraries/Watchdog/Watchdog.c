#include "watchdog.h"
#include <avr\interrupt.h>
#include <avr\sfr_defs.h>
#include <inttypes.h>


void WDStart(uint8_t timeout)
{
	register uint8_t setVal = timeout | (1<<WDE);
	cli();
	WDTCR = (1<<WDE) | (1<<WDCE);
	WDTCR = setVal;
	sei();

}
/* now an inline, to save stack/speed (see header)
void WDKick(void)
{
	asm("wdr");
}
*/
void WDStop(void)
{
	cli();
	WDTCR = (1<<WDCE) | (1<<WDE);
	WDTCR = 0;
	sei();
}