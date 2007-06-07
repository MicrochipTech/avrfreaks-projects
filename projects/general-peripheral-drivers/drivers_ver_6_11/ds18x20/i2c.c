
/****************************************************************************
 Title	:   C  file for the I2C FUNCTIONS library (i2c.c)
 Author:    Chris efstathiou hendrix@vivodinet.gr
 Date:	    7/Apr/2007 
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.
*****************************************************************************/
/* Although i could had written it in the new style, i prefer the old one for compatibility sake. */

#if !defined(AVRGCC_VERSION)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) > 303
#define AVRGCC_VERSION      304        /* AVRGCC version for including the correct files  */
#elif ((__GNUC__ * 100) + __GNUC_MINOR__) >= 300  &&  ((__GNUC__ * 100) + __GNUC_MINOR__) <= 303
#define AVRGCC_VERSION      303        /* AVRGCC version for including the correct files  */
#else 
#define AVRGCC_VERSION   300    
#endif
#endif


#if AVRGCC_VERSION >= 303
#include <avr/io.h>
#else
#include <io.h>
#endif

#include "i2c.h"

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

#if I2C_TIMEOUT <= 0
#undef  I2C_TIMEOUT 
#define I2C_TIMEOUT   10
#warning "I2C_TIMEOUT IS SET TO 10 CYCLES"
#endif    

#define I2C_CLOCK          CONCAT1(I2C_CLOCK_IN_HERTZ, L)

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
#define DELAY()    { I2C_TCNT = 0; while(I2C_TCNT < I2C_DELAY); }  

 

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

#elif  I2C_CLOCK_CYCLES >= 5 && I2C_CLOCK_CYCLES <= (240*3)

 

#define I2C_DELAY_LOOPS              ( I2C_CLOCK_CYCLES/3)
#define I2C_DELAY_REMAINDER          ( I2C_CLOCK_CYCLES - (I2C_DELAY_LOOPS*3) )

#if   I2C_DELAY_REMAINDER <= 0
#define DELAY()                      delay_c(I2C_DELAY_LOOPS)   
#elif I2C_DELAY_REMAINDER == 1
#define DELAY()                     { delay_c(I2C_DELAY_LOOPS); __asm__ volatile("nop"); }  
#elif I2C_DELAY_REMAINDER == 2
#define DELAY()                     { delay_c(I2C_DELAY_LOOPS); __asm__ volatile("nop"); \
                                      __asm__ volatile("nop");                                    \
                                    }    
#endif

#elif I2C_CLOCK_CYCLES > (240*3)  

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

/* Pin states */

#define SCL_1() 	  I2C_SCL_DDR_REG &= (~(1<<SCL_PIN))
#define SCL_0() 	  I2C_SCL_DDR_REG |= (1<<SCL_PIN)
#define SDA_1() 	  I2C_SDA_DDR_REG &= (~(1<<SDA_PIN))
#define SDA_0() 	  I2C_SDA_DDR_REG |= (1<<SDA_PIN)

#define RELEASE_I2C_BUS() { asm("cli"); SDA_1(); SCL_1(); asm("sei"); }
#define TOGGLE_SCL()      { SCL_1(); DELAY(); SCL_0(); DELAY(); }  


/* Global variables */
static unsigned char i2c_error=0; 


static inline void delay_c(uint8_t __count) __attribute__((always_inline));

void  delay_c(uint8_t __count)
{
	__asm__ volatile (
		"1: dec %0" "\n\t"
		"brne 1b"
		: "=r" (__count)
		: "0" (__count)
	);
}


unsigned char i2c_get_error(void)
{

return(i2c_error);
}


void i2c_start(void)
{
unsigned int timeout = 0;

#if I2C_WILL_USE_A_TIMER == 1
        I2C_TCCR = I2C_TIMER_PRESCALER_BITS;
#endif	

        RELEASE_I2C_BUS();
	I2C_SDA_DDR_REG &= (~(1<<SDA_PIN));
        I2C_SCL_DDR_REG &= (~(1<<SCL_PIN));
        while( bit_is_clear(I2C_SCL_PIN_REG,SCL_PIN) || bit_is_clear(I2C_SDA_PIN_REG,SDA_PIN) )
            {
               	DELAY();
                timeout++;
                if(timeout > I2C_TIMEOUT ) { timeout=0; i2c_error=2; return; }
             }	
        DELAY();  
	SDA_0();
	DELAY();
	SCL_0();
        DELAY();

return;
}
/*#################################################################################################*/

void i2c_stop(void)
{
unsigned int timeout = 0;

#if I2C_WILL_USE_A_TIMER == 1
        I2C_TCCR = I2C_TIMER_PRESCALER_BITS;
#endif	

        SCL_0();
	SDA_0();
  	DELAY();
	SCL_1();
	DELAY();
        SDA_1();
        DELAY();
        while( bit_is_clear(I2C_SCL_PIN_REG,SCL_PIN) || bit_is_clear(I2C_SDA_PIN_REG,SDA_PIN) )
            {
               	DELAY();
              	DELAY();
                timeout++;
                if(timeout > I2C_TIMEOUT ) { i2c_error=1; }
             }	


return;
}
/*#################################################################################################*/

void i2c_put_byte(unsigned char data)
{
unsigned char bit=0;
unsigned int timeout = 0;

#if I2C_WILL_USE_A_TIMER == 1
        I2C_TCCR = I2C_TIMER_PRESCALER_BITS;
#endif	


	for(bit=0; bit<=7; bit++)
	  {
	      if( data & 0x80 ) { SDA_1(); } else { SDA_0(); }
              TOGGLE_SCL();
	      data = (data<<1);
	  }

        /* Now set SDA to high and wait for the aknowledge. */  
        SDA_1();
        DELAY();
 
       /* Wait for the SDA line to go low (Ack) with a timeout. */
        while( bit_is_set(I2C_SDA_PIN_REG, SDA_PIN) )
            {    
               DELAY();
               DELAY();
               timeout++;
               if( timeout > I2C_TIMEOUT ) { timeout=0; i2c_error=1; return; }
            }   
        
        /* GIVE A CLOCK PULSE TO STATE THAT AKNOWLEDGE HAS BEEN RECEIVED */
        TOGGLE_SCL();


return;

}
/*#################################################################################################*/

unsigned char i2c_get_byte(unsigned char ack)
{

register unsigned char bit=0, data=0;
unsigned int timeout = 0;

#if I2C_WILL_USE_A_TIMER == 1
        I2C_TCCR = I2C_TIMER_PRESCALER_BITS;
#endif	


	RELEASE_I2C_BUS();
        DELAY();
        while( bit_is_clear(I2C_SCL_PIN_REG, SCL_PIN) )
            {    
               DELAY();
               DELAY();
               timeout++;
               if( timeout > I2C_TIMEOUT ) { timeout=0; i2c_error=1; return(0);  }
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

        /* if CONTINUE then send AKNOWLEDGE else if QUIT do not send AKNOWLEDGE (send Nack) */	 
	if(ack==I2C_CONTINUE) { SDA_0(); } else{ SDA_1(); } 
        DELAY();
        TOGGLE_SCL();
        

return(data);
}




/*######################################################################################################*/
/*					   T H E   E N D						*/
/*######################################################################################################*/



