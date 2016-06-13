#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "emmAVR.h"

uint32_t millis = 0;
uint32_t micros = 0;
uint8_t timecount = 0;

ISR( TIMER0_COMPA_vect )
{
  micros += 100;
  timecount++;
  if( timecount == 10 )
  {
    millis++;
    timecount = 0;
  }
}

void setTimer(void)
{
  // Possible values:
  // 16MHz - Prescaler F/64 - OCRA=250  -> 1ms
  // 16MHz - Prescaler F/64 - OCRA=25   -> 100us
  // 16MHz - Prescaler F/8  - OCRA=20   -> 10us
  //
  // Timer 0 Output Compare Register A
  OCR0A = 25;
  // Timer 0 Interrupt Mask Register
  TIMSK0 = _BV(OCIE0A);
  // Timer 0 Timer Control Register A : Clear Timer on Compare Match (CTC) mode (0 -> OCRA)
  TCCR0A = _BV(WGM01);
  // Timer 0 Timer Control Register B : Start Timer 0 with Prescaller F/64
  TCCR0B = (T0_Prescaller_64);
  // SET INTERRUPTS
  sei();
}

void resetTimer(void)
{
  // Timer Control Register A : Stop Timer 1
  TCCR0B = 0;
  // Clear Timer/Counter1
  TCNT0 = 0;
  // Clear "timecount" etc...
  timecount = 0;
  millis = 0;
  micros = 0;
  // Restart Timer
  setTimer();
}
