
/****************************************************************************
 Title	:   C  file for the I2C FUNCTIONS library (i2c.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:	    9/3/2003 2:06:22 μμ
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/
/* Although i could had written it in the new style, i prefer the old one for compatibility sake. */

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1	   /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1	   /* This is for GCC 3.2 */
#endif

#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION                330        /* AVRGCC version for including the correct files  */
#endif

#if AVRGCC_VERSION == 330
#include <avr/io.h>
#else
#include <io.h>
#endif

#include "i2c.h"










 

#if !defined(outp)
#define outp(value,port)  outb(port,value)
#endif

#if !defined(inp)
#define inp(port)  inb(port)
#endif

#if defined(__AVR_ATmega103__)
#define DDRC   0
#define DDRF   0
#endif

#ifndef CONCAT1
#define CONCAT1(a, b) CONCAT2(a, b)
#endif

#ifndef CONCAT2
#define CONCAT2(a, b) a ## b
#endif

/* Register name forming */
#define I2C_SDA_OUT_REG   CONCAT1(PORT, I2C_SDA_PORT)
#define I2C_SCL_OUT_REG   CONCAT1(PORT, I2C_SCL_PORT)
#define I2C_SDA_DDR_REG   CONCAT1(DDR, I2C_SDA_PORT)
#define I2C_SCL_DDR_REG   CONCAT1(DDR, I2C_SCL_PORT)
#define I2C_SDA_PIN_REG   CONCAT1(PIN, I2C_SDA_PORT)
#define I2C_SCL_PIN_REG   CONCAT1(PIN, I2C_SCL_PORT)


/* 
   Conversion of milliseconds and HERTZ to the right value for the delay function. 
   Some tricks are performed in order to increase accuracy without reverting to floating numbers
*/

#if I2C_CLOCK_IN_HERTZ <= 0 
#undef  I2C_CLOCK_IN_HERTZ
#define I2C_CLOCK_IN_HERTZ   1
#warning "I2C_CLOCK_IN_HERTZ SET TO 1 HERTZ"
#endif

#if I2C_TIMEOUT_IN_MILLISECONDS <= 0
#undef  I2C_TIMEOUT_IN_MILLISECONDS 
#define I2C_TIMEOUT_IN_MILLISECONDS   1
#warning "I2C_TIMEOUT IS SET TO 1 MILLISECOND"
#endif    

#define I2C_CLOCK          CONCAT1(I2C_CLOCK_IN_HERTZ, L)
#define I2C_TIMEOUT        CONCAT1(I2C_TIMEOUT_IN_MILLISECONDS, L)

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



/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if   I2C_WILL_USE_A_TIMER == 1
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/


/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if defined(__AVR_ATmega103__) || defined(__AVR_ATmega128__) || \
    defined(__AVR_ATmega8535__) || defined(__AVR_ATmega8515__)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if I2C_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define I2C_TCNT            TCNT0
#define I2C_TCCR            TCCR0
#define I2C_TOIE            TOIE0
#define I2C_OCIE            OCIE0
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif I2C_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define I2C_TCNT            TCNT2
#define I2C_TCCR            TCCR2
#define I2C_TOIE            TOIE2
#define I2C_OCIE            OCIE2
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW2)

#endif

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || \
       defined(__AVR_ATmega64__)  
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if I2C_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     0
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define I2C_TCNT            TCNT0
#define I2C_TCCR            TCCR0
#define I2C_TOIE            TOIE0
#define I2C_OCIE            OCIE0
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif I2C_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define I2C_TCNT            TCNT2
#define I2C_TCCR            TCCR2
#define I2C_TOIE            TOIE2
#define I2C_OCIE            OCIE2
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW2)
                         
#endif

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_ATmega161__) || defined(__AVR_ATmega163__) || defined(__AVR_AT90S8515__) || \
       defined(__AVR_AT90S8535__) || defined(__AVR_AT90S4434__)  
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if I2C_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define I2C_TCNT            TCNT0
#define I2C_TCCR            TCCR0
#define I2C_TOIE            TOIE0
#if defined(__AVR_ATmega161__)
#define I2C_OCIE            OCIE0
#else
#define I2C_OCIE            0
#endif
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif I2C_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define I2C_TCNT            TCNT2
#define I2C_TCCR            TCCR2
#define I2C_TOIE            TOIE2
#define I2C_OCIE            OCIE2
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW2)
                         
#endif


/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_AT90S4433__)  || defined(__AVR_AT90S2343__) || defined(__AVR_AT90S2323__)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 1

#if I2C_USE_TIMER > 0
#undef I2C_USE_TIMER
#warning " THERE IS NOT SUCH AN 8 BIT TIMER. WILL USE TIMER 0 "
#define I2C_USE_TIMER 0
#endif

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define I2C_TCNT            TCNT0
#define I2C_TCCR            TCCR0
#define I2C_TOIE            TOIE0
#define I2C_OCIE            OCIE0
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/* TIMER SELECTION RELATED ERROR MESSAGES */
#if    I2C_WILL_USE_A_TIMER == 1 &&  NUMBER_OF_8_BIT_TIMERS == 2 

#if    I2C_USE_TIMER == 1 || I2C_USE_TIMER > 2 
#error "SELECT TIMER 0 OR 2 PLEASE! " 
#endif

#endif /* I2C_WILL_USE_A_TIMER == 1 &&  NUMBER_OF_8_BIT_TIMERS == 2  */



#define I2C_DELAY_ACCURATE          (((100000000/I2C_CLOCK)/(100000000/F_CPU))/2)

#if I2C_DELAY_ACCURATE <= 0
#error " I2C CLOCK TOO HIGH "

#elif I2C_DELAY_ACCURATE <= 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_1
#define TIMER_DIVISION_FACTOR     1

#elif I2C_DELAY_ACCURATE > 255

#if   TIMER_DIV_BY_8 > 0 && (I2C_DELAY_ACCURATE/8) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_8
#define TIMER_DIVISION_FACTOR     8

#elif TIMER_DIV_BY_32 > 0 && (I2C_DELAY_ACCURATE/32) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_32
#define TIMER_DIVISION_FACTOR     32

#elif TIMER_DIV_BY_64 > 0 && (I2C_DELAY_ACCURATE/64) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_64
#define TIMER_DIVISION_FACTOR     64

#elif TIMER_DIV_BY_128 > 0 && (I2C_DELAY_ACCURATE/128) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_128
#define TIMER_DIVISION_FACTOR     128

#elif TIMER_DIV_BY_256 > 0 && (I2C_DELAY_ACCURATE/256) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_256
#define TIMER_DIVISION_FACTOR     256

#elif TIMER_DIV_BY_1024 > 0 && (I2C_DELAY_ACCURATE/1024) < 255

#define I2C_TIMER_PRESCALER_BITS  TIMER_DIV_BY_1024
#define TIMER_DIVISION_FACTOR     1024

#endif  /* #if   TIMER_DIV_BY_8 > 0 && (I2C_DELAY_ACCURATE/8) < 255 */
#endif  /* #elif I2C_DELAY_ACCURATE > 255 */

#define I2C_DELAY  ( I2C_DELAY_ACCURATE/TIMER_DIVISION_FACTOR )
#define DELAY()    { outp(0,I2C_TCNT); while(inp(I2C_TCNT) < I2C_DELAY); }  

 

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif I2C_WILL_USE_A_TIMER == 0 /* #if   I2C_WILL_USE_A_TIMER == 1 */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#define I2C_LEVEL_CHANGE_OVERHEAD    1 /* The cyccles needed to change the level of a I2C pin. */

#define I2C_CLOCK_CYCLES_ACCURATE    ( ((1000000000/I2C_CLOCK)/(1000000000/(F_CPU*10)))/2 )
#define I2C_CLOCK_CYCLES_REAL        ( ((1000000000/I2C_CLOCK)/(1000000000/F_CPU))/2 )


#if  I2C_CLOCK_CYCLES_ACCURATE - (I2C_CLOCK_CYCLES_REAL*10) >= 5
#define I2C_CLOCK_CYCLES             ((I2C_CLOCK_CYCLES_REAL+1)-I2C_LEVEL_CHANGE_OVERHEAD)
#else
#define I2C_CLOCK_CYCLES             (I2C_CLOCK_CYCLES_REAL-I2C_LEVEL_CHANGE_OVERHEAD)
#endif


#if  I2C_CLOCK_CYCLES <= 0
#warning " I2C CLOCK TOO HIGH "
#define DELAY()                       { }

#elif  I2C_CLOCK_CYCLES == 1
#define DELAY()                      __asm__ volatile("nop")

#elif I2C_CLOCK_CYCLES == 2
#define DELAY()                      { __asm__ volatile("nop"); __asm__ volatile("nop"); }

#elif I2C_CLOCK_CYCLES == 3
#define DELAY()                      { __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                       __asm__ volatile("nop");                          \
                                     }

#elif I2C_CLOCK_CYCLES == 4
#define DELAY()                      { __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                     }

#elif  I2C_CLOCK_CYCLES >= 5 && I2C_CLOCK_CYCLES < (255*3)

#ifndef DELAY_C_ACCURATE
/* 3 cpu cycles per loop when a constant is passed.*/
#define DELAY_C_ACCURATE(x)      ({  __asm__ volatile (  "ldi r25,%0 \n\t"           \
                                                         "L_%=:      \n\t"           \
                                                         "dec r25    \n\t"           \
                                                         "brne L_%=  \n\t"           \
                                                         : /* NO OUTPUT */           \
                                                         : "g" ((unsigned char)(x))  \
		                                         : "r25"	             \
           	                                       );                            \
                                  })                                  

#endif              

#define I2C_DELAY_LOOPS              ( I2C_CLOCK_CYCLES/3)
#define I2C_DELAY_REMAINDER          ( I2C_CLOCK_CYCLES - (I2C_DELAY_LOOPS*3) )

#if   I2C_DELAY_REMAINDER <= 0
#define DELAY()                      DELAY_C_ACCURATE(I2C_DELAY_LOOPS)   
#elif I2C_DELAY_REMAINDER == 1
#define DELAY()                     { DELAY_C_ACCURATE(I2C_DELAY_LOOPS); __asm__ volatile("nop"); }  
#elif I2C_DELAY_REMAINDER == 2
#define DELAY()                     { DELAY_C_ACCURATE(I2C_DELAY_LOOPS); __asm__ volatile("nop"); \
                                      __asm__ volatile("nop");                                    \
                                    }    
#endif

#elif I2C_CLOCK_CYCLES >= (255*3)  

#define I2C_DELAY_LOOPS              ( (I2C_CLOCK_CYCLES-3)/6 )
#define I2C_DELAY_REMAINDER          ( I2C_CLOCK_CYCLES - (I2C_DELAY_LOOPS*6) )

#if   I2C_DELAY_REMAINDER <= 0
#define DELAY()                      DELAY_L_ACCURATE(I2C_DELAY_LOOPS)   
#elif I2C_DELAY_REMAINDER == 1
#define DELAY()                     { DELAY_L_ACCURATE(I2C_DELAY_LOOPS); __asm__ volatile("nop"); }  
#elif I2C_DELAY_REMAINDER == 2
#define DELAY()                     { DELAY_L_ACCURATE(I2C_DELAY_LOOPS); __asm__ volatile("nop"); \
                                      __asm__ volatile("nop");                                    \
                                    } 
#elif I2C_DELAY_REMAINDER == 3
#define DELAY()                     { DELAY_L_ACCURATE(I2C_DELAY_LOOPS); __asm__ volatile("nop"); \
                                      __asm__ volatile("nop"); __asm__ volatile("nop");           \
                                    }  
#elif I2C_DELAY_REMAINDER == 4
#define DELAY()                     { DELAY_L_ACCURATE(I2C_DELAY_LOOPS); __asm__ volatile("nop"); \
                                      __asm__ volatile("nop"); __asm__ volatile("nop");           \
                                      __asm__ volatile("nop");                                    \
                                    }    

#elif I2C_DELAY_REMAINDER == 5
#define DELAY()                     { DELAY_L_ACCURATE(I2C_DELAY_LOOPS); __asm__ volatile("nop"); \
                                      __asm__ volatile("nop"); __asm__ volatile("nop");           \
                                      __asm__ volatile("nop"); __asm__ volatile("nop");           \
                                      __asm__ volatile("nop");                                    \
                                    }
#elif I2C_DELAY_REMAINDER == 6
#define DELAY()                     { DELAY_L_ACCURATE(I2C_DELAY_LOOPS); __asm__ volatile("nop"); \
                                      __asm__ volatile("nop"); __asm__ volatile("nop");           \
                                      __asm__ volatile("nop"); __asm__ volatile("nop");           \
                                      __asm__ volatile("nop");  __asm__ volatile("nop");          \
                                    }
#elif I2C_DELAY_REMAINDER == 7
#define DELAY()                     { DELAY_L_ACCURATE(I2C_DELAY_LOOPS); __asm__ volatile("nop"); \
                                      __asm__ volatile("nop"); __asm__ volatile("nop");           \
                                      __asm__ volatile("nop"); __asm__ volatile("nop");           \
                                      __asm__ volatile("nop");  __asm__ volatile("nop");          \
                                      __asm__ volatile("nop");                                    \
                                    }                
   
#endif  /* #elif I2C_CLOCK_CYCLES >= (255*3) */



#endif  /* I2C_CLOCK_CYCLES <= 0 */


/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* #elif I2C_WILL_USE_A_TIMER == 0 */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/* SOME ERROR CHECKING WHEN USING MEGA 103 */

#if defined(__AVR_ATmega103__)

#if I2C_SDA_DDR_REG == 0 
#error "SORRY THE I2C SDA PIN PORT MUST! BE BIDIRECTIONAL"
#endif

#if I2C_SCL_DDR_REG == 0 
#error "SORRY THE I2C SCL PIN PORT MUST! BE BIDIRECTIONAL"
#endif

#endif /* #if defined(__AVR_ATmega103__) */
/*******************************************************************************************************/

/* Pin states */
#define SCL_1() 	  cbi(I2C_SCL_DDR_REG, SCL_PIN)
#define SCL_0() 	  sbi(I2C_SCL_DDR_REG, SCL_PIN)
#define SDA_1() 	  cbi(I2C_SDA_DDR_REG, SDA_PIN)
#define SDA_0() 	  sbi(I2C_SDA_DDR_REG, SDA_PIN)

#define RELEASE_I2C_BUS() { SDA_1(); SCL_1(); }
#define TOGGLE_SCL()      { SCL_1(); DELAY(); SCL_0(); DELAY(); }  


/* Global variables */
unsigned char i2c_address=1;
unsigned char address_flag=0;
#if I2C_ERROR_CHECK_ENABLE == 1
unsigned int  timeout=0;
static unsigned char i2c_error=0; 
#endif

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if I2C_ERROR_CHECK_ENABLE == 1
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

void i2c_start(void)
{

#if I2C_WILL_USE_A_TIMER == 1
        outp(I2C_TIMER_PRESCALER_BITS, I2C_TCCR);
#endif	

#if I2C_ISSUE_BLIND_START == 0
unsigned char x=0;
#endif

        i2c_error=I2C_NO_ERROR;
        /* Signal that a i2c start was issued. */
        address_flag=1;

        RELEASE_I2C_BUS();
	cbi(I2C_SDA_OUT_REG, SDA_PIN);
	cbi(I2C_SCL_OUT_REG, SCL_PIN);
        DELAY();

#if I2C_ISSUE_BLIND_START == 0

        
            while( bit_is_set(I2C_SCL_PIN_REG,SCL_PIN) && bit_is_set(I2C_SDA_PIN_REG,SDA_PIN) )
                { 
                   DELAY();
                   x++;
                   if(x >= (I2C_START_CHECK_CYCLES*2))
                    { 
                       SDA_0();
                       DELAY();
                       SCL_0();
                       DELAY();
                       return;
                    }                 
                }    

            /* If the I2C bus is occupied wait for a stop condition. */
            while(1)
                {
                    if(bit_is_clear(I2C_SDA_PIN_REG,SDA_PIN) && bit_is_set(I2C_SCL_PIN_REG,SCL_PIN))
                     {
                        while(bit_is_set(I2C_SCL_PIN_REG,SCL_PIN))
                            {
                               if(bit_is_set(I2C_SDA_PIN_REG,SDA_PIN))
                                {
                                   /* An extra check to exclude wrong stop condition detection */
                                   if(bit_is_set(I2C_SCL_PIN_REG,SCL_PIN))
                                    {
                                       DELAY();
                                       SDA_0();
                                       DELAY();
                                       SCL_0();
                                       DELAY();
                                       return;
                                    } 
                                }
               
                            }
                     } 
                }        
               
#elif I2C_ISSUE_BLIND_START == 1

        SDA_0();
        DELAY();
        SCL_0();
        DELAY(); 
#endif

return;
}
/*#################################################################################################*/

void i2c_stop(void)
{
#if I2C_ISSUE_BLIND_STOP == 0
unsigned char x=0;
unsigned int  timeout=0;
#endif

        i2c_error=I2C_NO_ERROR;

        /* Reset the i2c start issued, flag. */ 
        address_flag=0;

        RELEASE_I2C_BUS();
        DELAY();

#if I2C_ISSUE_BLIND_STOP == 0

        /* Make sure tha SCL is free so we can give the required pulses */
        while( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) )
            {    
               DELAY_MS(1);
               timeout++;
               if( timeout > I2C_TIMEOUT )
                {
                   timeout=0;
                   i2c_error=I2C_ERROR_DEVICE_BUSY;
                   return;
                }
            }   

        while( bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN) )
            {    
               DELAY_MS(1);
               timeout++;
               if( timeout > I2C_TIMEOUT )
                {
                   /*  If SDA is still low then give 9 pulses to fake a byte receive and Nack. */
                   /*  This way the bus might set free */  
                   for(x=0; x<9; x++) { TOGGLE_SCL(); }
                   if( bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN) )
                    {
                       timeout=0;
                       i2c_error=I2C_ERROR_DEVICE_BUSY;
                       return;
                    }
                }
            }   

#endif

        SCL_0();
	SDA_0();
  	DELAY();
	SCL_1();
	DELAY();
        SDA_1();
        DELAY();



return;
}
/*#################################################################################################*/

void i2c_put_byte(unsigned char data)
{

register unsigned char bit=0;
unsigned int           timeout=0;

i2c_error=I2C_NO_ERROR;

/* If a i2c start was issued just before this function then the data var must contain an i2c address. */
if(address_flag) { i2c_address=data; address_flag=0; }


/* Check that SDA line is high, otherwise the slave is holding it low and that means wait... */ 

	for(bit=0; bit<=7; bit++)
	  {
	      if( data & 0x80 ) { SDA_1(); } else { SDA_0(); }
              TOGGLE_SCL();
	      data = (data<<1);
	  }

/* Look for a proper AKNOWLEDGE sequence*/

/* Set SDA and SCL high */ 
	RELEASE_I2C_BUS();
        DELAY();

/* Now we must check for a SDA low state. The SDA line should be already low */		 
        if( bit_is_set(I2C_SDA_PIN_REG, SDA_PIN) )
         {    
            DELAY_MS(1);
            if( bit_is_set(I2C_SDA_PIN_REG, SDA_PIN) )
             {    
                i2c_error=I2C_ERROR_DEVICE_NOT_RESPONDING;
                return;
             }
         }

/* Check that SCL line is high, otherwise the slave is holding it low and that means wait... */ 
        while( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) )
            {    
               DELAY_MS(1);
               timeout++;
               if( timeout > I2C_TIMEOUT )
                {
                   timeout=0;
                   i2c_error=I2C_ERROR_DEVICE_BUSY;
                   return;
                }
            }   

/* Set SCL to low level */
	SCL_0();  
	DELAY();

/*
   If in write mode wait until the SDA line is back to high level.
   In read mode a situation might arise where after a start condition followed by the device
   address for receiving the first bit of the byte to be received is 0. In that case the SDA line
   would never get back to a high state and the program would timeout and issue a false error message.
   So it is wise to skip that check. 
*/
        if( (i2c_address&0x01)==0 )
         { 
            while( bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN) )
                {    
                  DELAY_MS(1);
                  timeout++;
                  if( timeout > I2C_TIMEOUT )
                   {
                      timeout=0;
                      i2c_error=I2C_ERROR_DEVICE_BUSY;
                      return;
                   }
                }    
         }

/* transaction complete. There were no ERRORS */
return;
}
/*#################################################################################################*/

unsigned char i2c_get_byte(unsigned char ack)
{
register unsigned char bit=0, data=0;
unsigned int           timeout=0;

i2c_error=I2C_NO_ERROR;

	RELEASE_I2C_BUS();
        DELAY();

        /* Make sure tha SCL is free so we can give the required pulses */
        while( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) )
            {    
               DELAY_MS(1);
               timeout++;
               if( timeout > I2C_TIMEOUT )
                {
                   timeout=0;
                   i2c_error=I2C_ERROR_DEVICE_BUSY;
                   return(0xFF);
                }
            }   
    
	for(bit=0; bit<=7; bit++)
	  {
	      SCL_1();
	      DELAY();
	      data = (data<<1);
	      if( bit_is_set(I2C_SDA_PIN_REG, SDA_PIN) ) { data++; }
	      SCL_0();
	      DELAY();
	  }

	/* Check that the slave has released the SDA line */    
        while( bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN) )
            {    
               DELAY_MS(1);
               timeout++;
               if( timeout > I2C_TIMEOUT )
                {
                   timeout=0;
                   i2c_error=I2C_ERROR_DEVICE_BUSY;
                   return(data);
                }
            }   

        /* if CONTINUE then send AKNOWLEDGE else if QUIT do not send AKNOWLEDGE (send Nack) */	 
	if(ack==I2C_CONTINUE) { SDA_0(); } else{ SDA_1(); } 
        DELAY();
        TOGGLE_SCL();    

return(data);
}
/*#################################################################################################*/

unsigned char  i2c_get_error(void)
{

return(i2c_error);
}

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif I2C_ERROR_CHECK_ENABLE == 0
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

void i2c_start(void)
{

#if I2C_WILL_USE_A_TIMER == 1
        outp(I2C_TIMER_PRESCALER_BITS, I2C_TCCR);
#endif	

        RELEASE_I2C_BUS();
	cbi(I2C_SDA_OUT_REG, SDA_PIN);
	cbi(I2C_SCL_OUT_REG, SCL_PIN);   
        while( bit_is_clear(I2C_SCL_PIN_REG,SCL_PIN) && bit_is_set(I2C_SDA_PIN_REG,SDA_PIN) );
        DELAY();  
	SDA_0();
	DELAY();
	SCL_0();
        DELAY();

/* Signal that a i2c start was issued. */
address_flag=1;

return;
}
/*#################################################################################################*/

void i2c_stop(void)
{

        SCL_0();
	SDA_0();
  	DELAY();
	SCL_1();
	DELAY();
        SDA_1();
        DELAY();

/* Reset the i2c start issued, flag. */  
address_flag=0;

return;
}
/*#################################################################################################*/

void i2c_put_byte(unsigned char data)
{

register unsigned char bit=0;

/* If a i2c start was issued just before this function then the data var must contain an i2c address. */
if(address_flag) { i2c_address=data; address_flag=0; }


	for(bit=0; bit<=7; bit++)
	  {
	      if( data & 0x80 ) { SDA_1(); } else { SDA_0(); }
              TOGGLE_SCL();
	      data = (data<<1);
	  }

        /* Check that SCL line is high, otherwise the slave is holding it low and that means wait... */  
        RELEASE_I2C_BUS();
        DELAY();
        while( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) ); 
       
        /* GIVE A PULSE TO STATE THAT AKNOWLEDGE HAS BEEN RECEIVED */
        SCL_0();
        DELAY();

/*
   If in write mode wait until the SDA line is back to high level.
   In read mode a situation might arise where after a start condition followed by the device
   address for receiving the first bit of the byte to be received is 0. In that case the SDA line
   would never get back to a high state and the program would loop forever.
   So it is wise to skip that check. 
*/
        if( (i2c_address&0x01)==0 ) { while( bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN) ); }

return;
}
/*#################################################################################################*/

unsigned char i2c_get_byte(unsigned char ack)
{

register unsigned char bit=0, data=0;

	RELEASE_I2C_BUS();
        DELAY();
        while( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) );

	for(bit=0; bit<=7; bit++)
	  {
	      SCL_1();
	      DELAY();
	      data = (data<<1);
	      if( bit_is_set(I2C_SDA_PIN_REG, SDA_PIN) ) { data++; }
	      SCL_0();
	      DELAY();
	  }

        /* if CONTINUE then send AKNOWLEDGE else if QUIT do not send AKNOWLEDGE (send Nack) */	 
	if(ack==I2C_CONTINUE) { SDA_0(); } else{ SDA_1(); } 
        DELAY();
        TOGGLE_SCL();
        

return(data);
}

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/



/*######################################################################################################*/
/*					   T H E   E N D						*/
/*######################################################################################################*/



