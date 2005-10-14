#include <avr\io.h>

#define WD_17ms   0
#define WD_34ms                          BV(WDP0)
#define WD_68ms               BV(WDP1)
#define WD_140ms  (           BV(WDP1) | BV(WDP0) )
#define WD_270ms   BV(WDP2)
#define WD_550ms  (BV(WDP2)            | BV(WDP0) )
#define WD_1100ms (BV(WDP2) | BV(WDP1)            ) 
#define WD_2200ms (BV(WDP2) | BV(WDP1) | BV(WDP0) )

void WDStart(uint8_t timeout);
void WDStop(void);

static inline void WDKick(void)  {asm("wdr");}
