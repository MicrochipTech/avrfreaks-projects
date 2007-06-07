
/****************************************************************************
 Title  :   C  file for the SRF08 FUNCTIONS library (srf08.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      13/Jul/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1     /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1     /* This is for GCC 3.2 */
#endif

#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION       330        /* AVRGCC version for including the correct files  */
#endif

#if AVRGCC_VERSION == 330

#include <avr/io.h>
#include <avr/signal.h>

#else

#include <io.h>
#include <sig-avr.h>

#endif

#include "i2c.h"
#include "lm75.h"














#if !defined(outp)
#define outp(value,port)  outb(port,value)
#endif

#if !defined(inp)
#define inp(port)  inb(port)
#endif

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if   LM75_WILL_USE_A_TIMER == 1
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/


/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if defined(__AVR_ATmega103__) || defined(__AVR_ATmega128__) || \
    defined(__AVR_ATmega8535__) || defined(__AVR_ATmega8515__)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if LM75_USE_8_BIT_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define TIMER_TCNT          TCNT0
#define TIMER_TCCR          TCCR0
#define TIMER_TOIE          TOIE0
#define TIMER_OCIE          OCIE0
#define TIMER_TIMSK         TIMSK
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif LM75_USE_8_BIT_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define TIMER_TCNT          TCNT2
#define TIMER_TCCR          TCCR2
#define TIMER_TOIE          TOIE2
#define TIMER_OCIE          OCIE2
#define TIMER_TIMSK         TIMSK
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW2)

#endif

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || \
       defined(__AVR_ATmega64__)  
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if LM75_USE_8_BIT_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     0
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define TIMER_TCNT          TCNT0
#define TIMER_TCCR          TCCR0
#define TIMER_TOIE          TOIE0
#define TIMER_OCIE          OCIE0
#define TIMER_TIMSK         TIMSK
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif LM75_USE_8_BIT_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define TIMER_TCNT          TCNT2
#define TIMER_TCCR          TCCR2
#define TIMER_TOIE          TOIE2
#define TIMER_OCIE          OCIE2
#define TIMER_TIMSK         TIMSK
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW2)
                         
#endif

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_ATmega161__) || defined(__AVR_ATmega163__) || defined(__AVR_AT90S8515__) || \
       defined(__AVR_AT90S8535__) || defined(__AVR_AT90S4434__)  
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if LM75_USE_8_BIT_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#if defined(__AVR_ATmega161__)
#define TIMER_OCIE          OCIE0
#else
#define TIMER_OCIE          0
#endif

#define TIMER_TCNT          TCNT0
#define TIMER_TCCR          TCCR0
#define TIMER_TOIE          TOIE0
#define TIMER_TIMSK         TIMSK
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif LM75_USE_8_BIT_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define TIMER_TCNT          TCNT2
#define TIMER_TCCR          TCCR2
#define TIMER_TOIE          TOIE2
#define TIMER_OCIE          OCIE2
#define TIMER_TIMSK         TIMSK
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW2)
                         
#endif


/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_AT90S4433__)  || defined(__AVR_AT90S2343__) || defined(__AVR_AT90S2323__)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 1

#if LM75_USE_8_BIT_TIMER > 0
#undef LM75_USE_8_BIT_TIMER
#warning " THERE IS NOT SUCH AN 8 BIT TIMER. WILL USE TIMER 0 "
#define LM75_USE_8_BIT_TIMER 0
#endif

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define TIMER_TCNT          TCNT0
#define TIMER_TCCR          TCCR0
#define TIMER_TOIE          TOIE0
#define TIMER_OCIE          OCIE0
#define TIMER_TIMSK         TIMSK
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define TIMER_VALUE           (((LM75_CONVERSION_TIME_MS*1000000)/(1000000000/F_CPU))/256)
#define TIMER_PRESCALER       1
#define TIMER_PRESCALER_BITS  TIMER_DIV_BY_1      



/* ERROR MESSAGES */
#if TIMER_VALUE > 65535 
#error " LM75 DELAY TOO LONG "
#endif

#if    LM75_WILL_USE_A_TIMER == 1 &&  NUMBER_OF_8_BIT_TIMERS == 2 

#if    LM75_USE_8_BIT_TIMER == 1 || LM75_USE_8_BIT_TIMER > 2 
#error "SELECT TIMER 0 OR 2 PLEASE! " 
#endif

#endif

#elif  LM75_WILL_USE_A_TIMER == 0

#ifndef DELAY_L_ACCURATE
/* 6 cpu cycles per loop + 3 overhead when a constant is passed. */
#define DELAY_L_ACCURATE(x)  ({ unsigned long number_of_loops=(unsigned long)x;   \
                                __asm__ volatile ( "L_%=: \n\t"                   \
                                                   "subi %A0,lo8(-(-1)) \n\t"     \
                                                   "sbci %B0,hi8(-(-1)) \n\t"     \
                                                   "sbci %C0,hlo8(-(-1)) \n\t"    \
                                                   "sbci %D0,hhi8(-(-1)) \n\t"    \
                                                   "brne L_%= \n\t"               \
                                                   : /* NO OUTPUT */              \
                                                   : "w" (number_of_loops)        \
                                                 );                               \
                             })                                      




#define TIME_L1_MS      ( 1*(F_CPU/6000) )     /* 1 MILLISECOND  */

#define TIME_L100_US    ( 1*(F_CPU/60000) )    /* 100 MICROSECONDS  */

#define DELAY_MS(ms)    DELAY_L_ACCURATE( (TIME_L1_MS*ms) )

#define DELAY_L_US(us)  DELAY_L_ACCURATE( (TIME_L100_US*(us/100))+1 )

#endif /* #ifndef DELAY_L_ACCURATE */

#endif /* #elif  LM75_WILL_USE_A_TIMER == 0 */







/* Global Variables */
static unsigned char   address=LM75_UNIT_0;

#if  LM75_WILL_USE_A_TIMER == 1
volatile unsigned char lm75_read_temp_lock=0;
unsigned int           timer_loops=0;
#endif

volatile union int_union  {
                             volatile unsigned char byte[2];
                             volatile unsigned int  integer;
                           }i2c;




/*#################################################################################################*/

void lm75_select_unit(unsigned char lm75_address)
{

/* Make the new i2c address the active one. */
address=lm75_address;

return;
}
/*#################################################################################################*/

signed char lm75_get_temp(void)
{
static signed char lm75_temp=0;



#if  LM75_WILL_USE_A_TIMER == 1
asm("sei");
outp((1<<TIMER_TOIE), TIMER_TIMSK);
outp(TIMER_PRESCALER_BITS, TIMER_TCCR);


if(lm75_read_temp_lock == 0)
 {
    i2c_start_rx(address);
    lm75_temp=i2c_get_byte(I2C_CONTINUE);
    i2c.byte[0]=i2c_get_byte(I2C_QUIT);
    i2c_stop();
    lm75_read_temp_lock=1;
    outp(0, TIMER_TCNT);
    timer_loops=0;
 }
#elif  LM75_WILL_USE_A_TIMER == 0

    DELAY_MS(LM75_CONVERSION_TIME_MS);
    i2c_start_rx(address);
    lm75_temp=i2c_get_byte(I2C_CONTINUE);
    i2c.byte[0]=i2c_get_byte(I2C_QUIT);
    i2c_stop();
    
#endif
    
return(lm75_temp);
}


signed int lm75_get_reg(unsigned char reg_number)
{

    i2c_start_tx(address);
    i2c_put_byte(reg_number);
    i2c_start_rx(address);

    if(reg_number != 1)
      {
      	i2c.byte[1]=i2c_get_byte(I2C_CONTINUE);
        i2c.byte[0]=i2c_get_byte(I2C_QUIT);
      }
    else{ i2c.byte[0]=i2c_get_byte(I2C_QUIT); i2c.byte[1]=0; }

    i2c_stop();


return(i2c.integer);
}

void lm75_set_tos(signed char tos_temp)
{

    i2c_start_tx(address);
    i2c_put_byte(LM75_TOS_REG);


    i2c_put_byte(tos_temp);       
    i2c_put_byte(0);

    i2c_stop();


return;
}

void lm75_set_hyst(signed char hyst_temp)
{


    i2c_start_tx(address);
    i2c_put_byte(LM75_HYST_REG);

    i2c_put_byte(hyst_temp);       
    i2c_put_byte(0);

    i2c_stop();


return;
}

void lm75_configure(unsigned char config_byte)
{


    i2c_start_tx(address);
    i2c_put_byte(LM75_CONFIG_REG);

    i2c_put_byte(config_byte);       

    i2c_stop();


return;
}

#if  LM75_WILL_USE_A_TIMER == 1
/* INTERRUPT SERVICE ROUTINE */
TIMER_SIGNAL()
{
timer_loops++;
if(timer_loops >= TIMER_VALUE) { lm75_read_temp_lock=0; }


return;
}
#endif

/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

