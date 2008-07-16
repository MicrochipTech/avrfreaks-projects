#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <avr/interrupt.h>
//#include <avr/signal.h>

#ifndef F_CPU
	#define F_CPU F_OSC
#endif

#define CLOCK_TCNT  65535 - (F_CPU / 1024);

#define CLOCK_HOUR 0
#define CLOCK_MIN  1
#define CLOCK_SEC  2
extern unsigned char clock[3];
extern volatile unsigned char clock_new_flag;
extern volatile unsigned char clock_timeout_timer;
void clock_init(void);
void clock_do(void);

//clear given flag:
#define CLOCK_CLEAR_FLAG(f) clock_new_flag &= ~(1<<(f))
#define CLOCK_TEST_FLAG(f)  (clock_new_flag & (1<<(f)))


#endif

