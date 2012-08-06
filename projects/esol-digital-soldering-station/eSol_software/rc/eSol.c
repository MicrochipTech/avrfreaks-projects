#include <avr/interrupt.h>
#include <stdbool.h>
#include "eSol.h"
#include "os.h"
#include "port.h"
#include "adc.h"


int main (void)
{
  gvPortInit();
  gvDisplayInit();

  ADC_vInit();
  OS_vInit();

  OS_vStartFreeRunTimer();

  sei();

  while (1)
  {
    if (OS_u8Cnt1ms >= OS_1ms_RUN_TIMER)
    {
      OS_vCyclicTask1ms();
      OS_u8Cnt1ms = 0;
    }

    if (OS_u16Cnt100ms >= OS_100ms_RUN_TIMER)
    {
      OS_vCyclicTask100ms();
      OS_u16Cnt100ms = 0;
    }
    if (OS_u16Cnt200ms >= OS_200ms_RUN_TIMER)
    {
      OS_vCyclicTask200ms();
      OS_u16Cnt200ms = 0;
    }
    if (OS_u16Cnt500ms >= OS_500ms_RUN_TIMER)
    {
      OS_vCyclicTask500ms();
      OS_u16Cnt500ms = 0;
    }

    OS_vBackgroundTask();
  }
  return (1);
}
