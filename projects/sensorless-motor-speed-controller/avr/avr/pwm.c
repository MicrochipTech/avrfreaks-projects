// very simple PWM driver using  OC2 and one arbitary direction pin
// usefull for a simple single-fet with relay reversal motor
// or D to A conversion using rc filter etc
#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "adc.h"
#include "fail.h"
#include "string.h"
#include "monorail.h"
#include "command.h"
#include "pwm.h"
#include "avr.h"

// pin 20 is PORTD6
#define PWMDIR_PORT PORTD
#define PWMDIR_DDR DDRD
#define PWMDIR_PIN PIND
#define PWMDIR_BIT 6

#define GOFORWARD sbi(PORTD,PWMDIR_BIT)
#define GOREVERSE cbi(PORTD,PWMDIR_BIT)

#define FORWARD bit_is_set(PWMDIR_PIN,PWMDIR_BIT)
#define REVERSE bit_is_clear(PWMDIR_PIN,PWMDIR_BIT)

// pin 21 is speed PWM output
#define SPEED OCR2

//#define PWMENABLE

void init_pwm()
{
   //   port D PWM motor output
   PWMDIR_DDR|=_BV(PWMDIR_BIT);  // direction
   sbi(DDRD,7);   // speed
   // timer2 PWM mode
   // TCCR2=(1<<COM21)+(1<<CS20)+T2_CK1024;
   OCR2=0x7F;  // 50%
   // fast PWM mode, PWM compare toggle, direct clocking no prescaler
   TCCR2= (1<<WGM21) + (1<<WGM20) + (1<<COM21) + T2_CK256;
   sbi(TIMSK,TOIE2);  // enable timer 2 overflow interrupt enable
}

void pwm_set(uint8_t speed)
{
   SPEED=speed;
   if(speed==0){
		  cbi(TCCR2,COM21);  // disconnect PWM signal when off
		  return;
	       }
   TCCR2|=~_BV(COM21);  // connect PWM signal
}

void pwm_dir(int8_t dir)
{
   if(dir==0){
		SPEED=0;
		cbi(TCCR2,COM21);  // disconnect PWM signal when off
		return;
	     }
   if(dir>0){
	       PORTD&=~_BV(PWMDIR_BIT);
	       TCCR2|=~_BV(COM21);  // connect PWM signal
	    }

   if(dir<0){
	       PORTD|=_BV(PWMDIR_BIT);
	       TCCR2|=~_BV(COM21);  // connect PWM signal
	    }
   return;
}

void pwm_print()
{
   puts("speed=");
   put_i16(SPEED);
   put_cr();
   return;
}

//rollover time for 32bit us clock is

#ifdef PWMENABLE
SIGNAL (SIG_OVERFLOW2)
{
//static inline rail_interrupt();
}
#endif

