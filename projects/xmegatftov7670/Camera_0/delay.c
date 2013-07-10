#include "delay.h"

void delay_ms(unsigned int delay)
{
 while(delay--) delay_us(1000);
} 