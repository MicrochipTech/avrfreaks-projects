/****************************************************************************
 Title  :   S file for the SOFTWARE UART MINIMUM FUNCTIONS  (softuart.s)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      03/0ct/2002
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

#if   AVRGCC_VERSION == 330
#include <avr/io.h>
#elif AVRGCC_VERSION == 320
#include <io.h>
#else
#include <io-avr.h>
#endif

#include "suart_min.cfg"




#define SUART_RX_PORT_IS_IO    1
#define SUART_TX_PORT_IS_IO    1

/* MACRO DEFINITIONS */
#ifndef CONCAT1
#define CONCAT1(a, b) CONCAT2(a, b)
#endif
#ifndef CONCAT2
#define CONCAT2(a, b) a ## b
#endif

/* REGISTER NAME FORMING */
#define SUART_RX_OUT_REG    CONCAT1(PORT, SOFT_UART_RX_PORT)
#define SUART_RX_DDR_REG    CONCAT1(DDR,  SOFT_UART_RX_PORT)
#define SUART_RX_PIN_REG    CONCAT1(PIN,  SOFT_UART_RX_PORT)

#define SUART_TX_OUT_REG    CONCAT1(PORT, SOFT_UART_TX_PORT)
#define SUART_TX_DDR_REG    CONCAT1(DDR,  SOFT_UART_TX_PORT)
#define SUART_TX_PIN_REG    CONCAT1(PIN,  SOFT_UART_TX_PORT)

#if SUART_TIMEOUT_ENABLED == 1

#define SUART_TIMEOUT_LOOPS          (SUART_TIMEOUT_TIME_MS*(F_CPU/8000) )

#if SUART_TIMEOUT_LOOPS <= 0 

#undef SUART_TIMEOUT_LOOPS
#define SUART_TIMEOUT_LOOPS   1

#endif

#endif  /* #if SUART_TIMEOUT_ENABLED == 1 */

#if   SUART_STOP_BIT_DETECTION == 0 
#define  SUART_AVAILABLE_CPU_CYCLES  (((F_CPU/SUART_BAUDRATE)*SUART_STOP_BITS)+((F_CPU/SUART_BAUDRATE)/2) ) 
#elif SUART_STOP_BIT_DETECTION == 1
#define  SUART_AVAILABLE_CPU_CYCLES  ((F_CPU/SUART_BAUDRATE)*SUART_STOP_BITS ) 
#elif SUART_STOP_BIT_DETECTION == 2
#define  SUART_AVAILABLE_CPU_CYCLES  ((F_CPU/SUART_BAUDRATE)/2 )
#endif

                                                       

/* BAUDRATE TIMING SETTINGS */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if   SUART_WILL_USE_A_TIMER == 0 
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#define SUART_CORRECTION_FACTOR        8
#define SUART_DELAY_OVERHEAD           3

#define TEMP_SUART_DELAY_ACCURATE      ( ((F_CPU/(SUART_BAUDRATE/10))-(SUART_CORRECTION_FACTOR*10))/6 )  
#define TEMP_SUART_DELAY_REAL          ( ((F_CPU/SUART_BAUDRATE)-SUART_CORRECTION_FACTOR)/6 )

#if   ( TEMP_SUART_DELAY_ACCURATE - (TEMP_SUART_DELAY_REAL*10)  ) == 0  

#define SUART_DELAY1  (TEMP_SUART_DELAY_REAL-(SUART_DELAY_OVERHEAD/3))  
#define SUART_DELAY2  TEMP_SUART_DELAY_REAL  
#define SUART_DELAY3  TEMP_SUART_DELAY_REAL 

#elif   ( TEMP_SUART_DELAY_ACCURATE - (TEMP_SUART_DELAY_REAL*10)  ) >= 1 && \
        ( TEMP_SUART_DELAY_ACCURATE - (TEMP_SUART_DELAY_REAL*10)  ) <= 3 

#define SUART_DELAY1  ((TEMP_SUART_DELAY_REAL+1)-(SUART_DELAY_OVERHEAD/3))   
#define SUART_DELAY2  TEMP_SUART_DELAY_REAL 
#define SUART_DELAY3  TEMP_SUART_DELAY_REAL 

#elif   ( TEMP_SUART_DELAY_ACCURATE - (TEMP_SUART_DELAY_REAL*10)  ) >= 4 && \
        ( TEMP_SUART_DELAY_ACCURATE - (TEMP_SUART_DELAY_REAL*10)  ) <= 6 

#define SUART_DELAY1  ((TEMP_SUART_DELAY_REAL+1)-(SUART_DELAY_OVERHEAD/3))   
#define SUART_DELAY2  (TEMP_SUART_DELAY_REAL+1) 
#define SUART_DELAY3  TEMP_SUART_DELAY_REAL 

#elif   ( TEMP_SUART_DELAY_ACCURATE - (TEMP_SUART_DELAY_REAL*10)  ) >= 7 && \
        ( TEMP_SUART_DELAY_ACCURATE - (TEMP_SUART_DELAY_REAL*10)  ) <= 9 

#define SUART_DELAY1  ((TEMP_SUART_DELAY_REAL+1)-(SUART_DELAY_OVERHEAD/3))   
#define SUART_DELAY2  (TEMP_SUART_DELAY_REAL+1) 
#define SUART_DELAY3  (TEMP_SUART_DELAY_REAL+1) 


#endif



/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_WILL_USE_A_TIMER == 1  
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if defined(__AVR_ATmega103__) || defined(__AVR_ATmega128__) || \
    defined(__AVR_ATmega8535__) || defined(__AVR_ATmega8515__)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if SUART_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define SUART_TCNT          TCNT0
#define SUART_TCCR          TCCR0
#define SUART_TOIE          TOIE0
#define SUART_OCIE          OCIE0

#elif SUART_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define SUART_TCNT          TCNT2
#define SUART_TCCR          TCCR2
#define SUART_TOIE          TOIE2
#define SUART_OCIE          OCIE2


#endif

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || \
       defined(__AVR_ATmega64__) || defined(__AVR_ATmega323__) 
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if SUART_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     0
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define SUART_TCNT          TCNT0
#define SUART_TCCR          TCCR0
#define SUART_TOIE          TOIE0
#define SUART_OCIE          OCIE0

#elif SUART_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define SUART_TCNT          TCNT2
#define SUART_TCCR          TCCR2
#define SUART_TOIE          TOIE2
#define SUART_OCIE          OCIE2

                         
#endif

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_ATmega161__) || defined(__AVR_ATmega163__) || defined(__AVR_AT90S8515__) || \
       defined(__AVR_AT90S8535__) || defined(__AVR_AT90S4434__)  
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if SUART_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define SUART_TCNT          TCNT0
#define SUART_TCCR          TCCR0
#define SUART_TOIE          TOIE0
#if defined(__AVR_ATmega161__)
#define SUART_OCIE          OCIE0
#else
#define SUART_OCIE          0
#endif

#elif SUART_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define SUART_TCNT           TCNT2
#define SUART_TCCR           TCCR2
#define SUART_TOIE           TOIE2
#define SUART_OCIE           OCIE2

                         
#endif


/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_AT90S4433__) 
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 1

#if SUART_USE_TIMER > 0
#undef SUART_USE_TIMER
#warning " THERE IS NOT SUCH AN 8 BIT TIMER. WILL USE TIMER 0 "
#define SUART_USE_TIMER 0
#endif

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define SUART_TCNT          TCNT0
#define SUART_TCCR          TCCR0
#define SUART_TOIE          TOIE0
#define SUART_OCIE          OCIE0

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif defined(TCNT0)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if SUART_USE_TIMER > 0
#undef SUART_USE_TIMER
#warning " THERE IS NOT SUCH AN 8 BIT TIMER. WILL USE TIMER 0 "
#define SUART_USE_TIMER 0
#endif

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define SUART_TCNT          TCNT0
#define SUART_TCCR          TCCR0
#define SUART_TOIE          TOIE0
#define SUART_OCIE          0

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define SUART_CORRECTION_FACTOR              16
#define SUART_DELAY_OVERHEAD                 0

#define SUART_BAUDRATE_DELAY                 ( F_CPU/SUART_BAUDRATE )


#if     SUART_BAUDRATE_DELAY <= 255
#define PRESCALER_VALUE                       TIMER_DIV_BY_1
#define TIMER_DIVISION_FACTOR                 1

#elif   SUART_BAUDRATE_DELAY <= (255*8)
#define PRESCALER_VALUE                       TIMER_DIV_BY_8
#define TIMER_DIVISION_FACTOR                 8

#elif   SUART_BAUDRATE_DELAY <= (255*32) && TIMER0_DIV_BY_32 > 0
#define PRESCALER_VALUE                       TIMER_DIV_BY_32
#define TIMER_DIVISION_FACTOR                 32

#elif   SUART_BAUDRATE_DELAY <= (255*64) && TIMER_DIV_BY_64 >  0
#define PRESCALER_VALUE                       TIMER_DIV_BY_64
#define TIMER_DIVISION_FACTOR                 64

#elif   SUART_BAUDRATE_DELAY <= (255*128) && TIMER_DIV_BY_128 > 0
#define PRESCALER_VALUE                       TIMER_DIV_BY_128
#define TIMER_DIVISION_FACTOR                 128

#elif   SUART_BAUDRATE_DELAY <= (255*255)
#define PRESCALER_VALUE                       TIMER_DIV_BY_256
#define TIMER_DIVISION_FACTOR                 256

#elif   SUART_BAUDRATE_DELAY <= (255*1024)
#define PRESCALER_VALUE                       TIMER_DIV_BY_1024
#define TIMER_DIVISION_FACTOR                 1024

#else 
#error " BAUDRATE TOO LOW "

#endif

#define TIME_1_BIT      ( (SUART_BAUDRATE_DELAY-SUART_CORRECTION_FACTOR)/TIMER_DIVISION_FACTOR )
#define TIME_HALF_BIT   ( ((SUART_BAUDRATE_DELAY-SUART_DELAY_OVERHEAD)/2)/TIMER_DIVISION_FACTOR )


/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/   
#endif  /* #elif SUART_WILL_USE_A_TIMER == 1 */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/





/*********************************************************************************************************/

#if    SUART_STOP_BIT_DETECTION > 2
#error " SUART_STOP_BIT_DETECTION CAN BE 0,1 OR 2 "
#endif

/*********************************************************************************************************/

#if defined(__AVR_ATmega103__)

#undef DDRC   
#undef DDRF   
#undef PINC   
#undef PORTF  

#define DDRC   0
#define DDRF   0
#define PINC   0
#define PORTF  0

#if SUART_RX_PIN_REG == 0 
#error " THE SUART RX PIN IS OUTPUT ONLY "
#endif

#if SUART_TX_OUT_REG == 0 
#error " THE SUART TX PIN IS INPUT ONLY "
#endif

#if SUART_RX_PIN_REG == PINF
#undef  SUART_RX_PORT_IS_IO
#define SUART_RX_PORT_IS_IO    0
#endif

#if SUART_TX_OUT_REG == PORTC
#undef  SUART_TX_PORT_IS_IO
#define SUART_TX_PORT_IS_IO    0
#endif

#endif

/*********************************************************************************************************/

#if   SUART_WILL_USE_A_TIMER == 0 && SUART_DYNAMIC_BAUD_CHANGE == 0

#if    SUART_DELAY1 <=2 || SUART_DELAY2 <= 3 || SUART_DELAY3 <= 3 
#error " BAUDRATE TOO HIGH "
#elif  SUART_DELAY1 > 255 || SUART_DELAY2 > 255 || SUART_DELAY3 > 255
#error " BAUDRATE TOO LOW USE A TIMER "
#endif

#elif SUART_WILL_USE_A_TIMER == 1  && SUART_DYNAMIC_BAUD_CHANGE == 0

#if   TIME_HALF_BIT <= 0 || TIME_1_BIT <= 0 || SUART_BAUDRATE_DELAY < (SUART_CORRECTION_FACTOR*2)
#error " BAUDRATE TOO HIGH "
#endif

#endif /* #elif SUART_WILL_USE_A_TIMER == 1  && SUART_DYNAMIC_BAUD_CHANGE == 0 */

/*********************************************************************************************************/

#if SREG > 0X3F

#define S_TX_DDR_REG  SUART_TX_DDR_REG - 0X20
#define S_TX_PORT     SUART_TX_OUT_REG - 0X20
#define S_RX_DDR_REG  SUART_RX_DDR_REG - 0X20
#define S_RX_PIN_REG  SUART_RX_PIN_REG - 0X20
#define S_INT_REG     SUART_INT_REG    - 0X20
#define AVR_TCNT      SUART_TCNT       - 0X20
#define AVR_TCCR      SUART_TCCR       - 0X20

#else

#define S_TX_DDR_REG  SUART_TX_DDR_REG
#define S_TX_PORT     SUART_TX_OUT_REG
#define S_RX_DDR_REG  SUART_RX_DDR_REG
#define S_RX_PIN_REG  SUART_RX_PIN_REG
#define S_INT_REG     SUART_INT_REG
#define AVR_TCNT      SUART_TCNT 
#define AVR_TCCR      SUART_TCCR       

#endif



/* DO NOT CHANGE ANYTHING BELOW THIS LINE !!! */
#define          bitcnt  r31                     /* bit counter */
#define          temp1   r30                     /* temporary storage register */
#define          temp2   r27
#define          Txbyte  r24                     /* r24,r25 are used for passing parameters */
#define          Rxbyte  r24                     /* r24,r25 are used for returning a value in gcc */

/*********************************************************************************************/
                 .global suart_putc
                 .func suart_putc

suart_putc:

#if SUART_TX_PORT_IS_IO == 1
                 sbi      S_TX_DDR_REG, SUART_TX_PIN    /* set Tx pin as aoutput */
#endif
#if SUART_WILL_USE_A_TIMER == 1
                 ldi      temp1, PRESCALER_VALUE 
                 out      AVR_TCCR, temp1  
#endif  
                 ldi      bitcnt, 9+SUART_STOP_BITS 
                 com      Txbyte                        /* Invert Txbyte  */
                 sec                                    /* Set carry (Start bit) */

#if    SUART_USE_RS232_LOGIC == 0 
suart_putchar0:  brcc     suart_putchar1                /* If carry is set   */
#elif  SUART_USE_RS232_LOGIC == 1
suart_putchar0:  brcs     suart_putchar1                /* If carry is clear */
#endif 

                 cbi      S_TX_PORT, SUART_TX_PIN       /* send a '0'   */
                 rjmp     suart_putchar2                /* else         */

suart_putchar1:  sbi      S_TX_PORT, SUART_TX_PIN       /* send a '1'   */
                 nop

#if SUART_WILL_USE_A_TIMER == 0  
suart_putchar2:  ldi      temp1, SUART_DELAY2           /* 0,5 bit delay ((3*b)+8)*1/F_CPU  */
SUART_tx_delay1: dec      temp1
                 brne     SUART_tx_delay1

                 ldi      temp1, SUART_DELAY3          /* another 0,5 bit delay ((3*b)+8)*1/F_CPU  */
SUART_tx_delay2: dec      temp1                        /* making a total of 1 bit */
                 brne     SUART_tx_delay2

#elif SUART_WILL_USE_A_TIMER == 1  

suart_putchar2:  clr      temp1                        /* 1 bit delay loop using a timer */
                 out      AVR_TCNT, temp1
                                                  
                 ldi      temp2, TIME_1_BIT 
                                 
suart_tx_delay1: in       temp1, AVR_TCNT
                 cp       temp1, temp2
                 brlo     suart_tx_delay1

#endif
                 lsr      Txbyte                       /* Get next bit  */
                 dec      bitcnt                       /* decrement bitcount */
                 brne     suart_putchar0               /* If not all bit sent (if not 0) send next */
                                                       /* else */
                 ret                                   /* return from function  */

.endfunc
/***************************************************************************************************/
                 .global suart_getc
                 .func suart_getc

suart_getc:      

#if SUART_RX_PORT_IS_IO == 1                 
                 cbi     S_RX_DDR_REG, SUART_RX_PIN    /* set port Rxd bit as input */
#endif                 

#if SUART_WILL_USE_A_TIMER == 1
                 ldi      temp1, PRESCALER_VALUE 
                 out      AVR_TCCR, temp1
#endif  

#if    SUART_TIMEOUT_ENABLED == 1
                 ldi  r24, 0
                 sts  suart_timeout_flag, r24
                 ldi  r24, lo8(SUART_TIMEOUT_LOOPS)
                 ldi  r25, hi8(SUART_TIMEOUT_LOOPS)
                 ldi  r26, hlo8(SUART_TIMEOUT_LOOPS)
                 ldi  r27, hhi8(SUART_TIMEOUT_LOOPS)
#endif

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if    SUART_USE_RS232_LOGIC == 0
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
 
#if SUART_STOP_BIT_DETECTION == 0 
suart_wait_high: sbis    S_RX_PIN_REG, SUART_RX_PIN     /* Wait for low-high-low transition  */
                 rjmp    suart_wait_high                /* or for a high-low transition      */
#endif

suart_wait_low:
#if    SUART_TIMEOUT_ENABLED == 1
                 subi    r24, lo8(-(-1))
                 sbci    r25, hi8(-(-1))  
                 sbci    r26, hlo8(-(-1)) 
                 sbci    r27, hhi8(-(-1)) 
                 breq    suart_timeout1
#endif
                 sbic    S_RX_PIN_REG, SUART_RX_PIN       
                 rjmp    suart_wait_low                 /* Wait for start bit */

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_USE_RS232_LOGIC == 1 
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if SUART_STOP_BIT_DETECTION == 0 
suart_wait_low:  sbic    S_RX_PIN_REG, SUART_RX_PIN     /* Wait for low-high-low transition  */
                 rjmp    suart_wait_low                 /* or for a high-low transition      */
#endif

suart_wait_high:
#if    SUART_TIMEOUT_ENABLED == 1
                 subi    r24, lo8(-(-1))
                 sbci    r25, hi8(-(-1))  
                 sbci    r26, hlo8(-(-1)) 
                 sbci    r27, hhi8(-(-1)) 
                 breq    suart_timeout1
#endif
                 sbis    S_RX_PIN_REG, SUART_RX_PIN       
                 rjmp    suart_wait_high                /* Wait for start bit */

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* #if SUART_STOP_BIT_DETECTION == 0  */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
                 
                 ldi     bitcnt, 8                

#if SUART_WILL_USE_A_TIMER == 0  
                 ldi     temp1, SUART_DELAY1            /* 0,5 bit to be counted only once (start bit) */
SUART_rx_delay1: dec     temp1
                 brne    SUART_rx_delay1

getchar2:        ldi     temp1, SUART_DELAY2            /* another 0,5 bit delay ((3*b)+8)*1/F_CPU */
SUART_rx_delay2: dec     temp1
                 brne    SUART_rx_delay2

                 ldi     temp1, SUART_DELAY3            /* another 0,5 bit delay  making 1 bit in total */
SUART_rx_delay3: dec     temp1
                 brne    SUART_rx_delay3

#elif SUART_WILL_USE_A_TIMER == 1  

                 clr     temp1                          /* 1/2 bit delay loop to be counted only once */
                 out     AVR_TCNT, temp1                /* using a timer  */
/* Dead time start(TIMER IS ALREADY COUNTING). */                        
                 ldi     temp2, TIME_HALF_BIT     
/***************   Dead time end   ************/                                                       
suart_rx_delay1: in      temp1, AVR_TCNT
                 cp      temp1, temp2
                 brlo    suart_rx_delay1
            
getchar2:        clr     temp1                           /* 1 bit delay loop using a timer */
                 out     AVR_TCNT, temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
                 ldi     temp2, TIME_1_BIT 
/***************   Dead time end   ************/                               
suart_rx_delay2: in      temp1, AVR_TCNT
                 cp      temp1, temp2
                 brlo    suart_rx_delay2

#endif

                 clc                                     /* clear carry */
#if    SUART_USE_RS232_LOGIC == 0                  
                 sbic    S_RX_PIN_REG, SUART_RX_PIN      /* If rx pin is high */             
#elif  SUART_USE_RS232_LOGIC == 1
                 sbis    S_RX_PIN_REG, SUART_RX_PIN      /* If rx pin is low  */             
#endif
                                                         /* set cary */
                 sec                                      
                                                         /* else  */
                 ror     Rxbyte                          /* shift bit into Rxbyte  */
                 dec     bitcnt                          /* If bit is stop bit  */
                 breq    getchar3                        /* byte is received */
                 rjmp    getchar2                        /* else go get next bit */

getchar3:        
#if SUART_WILL_USE_A_TIMER == 0 && SUART_STOP_BIT_DETECTION == 2 
                 ldi     bitcnt, SUART_STOP_BITS
              
getchar4:        ldi     temp1, SUART_DELAY2       
SUART_rx_delay4: dec     temp1
                 brne    SUART_rx_delay4

                 ldi     temp1, SUART_DELAY3      
SUART_rx_delay5: dec     temp1
                 brne    SUART_rx_delay5

                 dec     bitcnt
                 brne    getchar4

#elif SUART_WILL_USE_A_TIMER == 1 && SUART_STOP_BIT_DETECTION == 2  
                 ldi     bitcnt, SUART_STOP_BITS
            
getchar4:        clr     temp1                    
                 out     AVR_TCNT, temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
                 ldi     temp2, TIME_1_BIT 
/***************   Dead time end   ************/                               
suart_rx_delay4: in      temp1, AVR_TCNT
                 cp      temp1, temp2
                 brlo    suart_rx_delay4

                 dec     bitcnt
                 brne    getchar4
#endif


getchar5: 
#if   SUART_STOP_BIT_DETECTION == 1 && SUART_USE_RS232_LOGIC == 0 

exit_suart_1:    sbis    S_RX_PIN_REG, SUART_RX_PIN      /* Wait for high level            */
                 rjmp    exit_suart_1                    /* which should be the stop bit   */

#elif SUART_STOP_BIT_DETECTION == 1 && SUART_USE_RS232_LOGIC == 1

exit_suart_1:    sbic    S_RX_PIN_REG, SUART_RX_PIN      /* Wait for low level             */
                 rjmp    exit_suart_1                    /* which should be the stop bit   */

#endif
                 ret                                     /* return from function */

suart_timeout1:  ldi     Rxbyte, 1
                 sts     suart_timeout_flag, Rxbyte
                 ldi     Rxbyte, RX_TERMINATION_CHAR
                 ret

.endfunc
/***************************************************************************************************/
/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/

