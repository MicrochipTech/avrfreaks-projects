#ifndef TIMER_XM_V1
#define TIMER_XM_V1

#include "avr_compiler.h"
#include "cpu_xm_v1.h"

extern volatile bool timer_f;
void TimerInit(void);
void delay_ms(unsigned int);
#endif