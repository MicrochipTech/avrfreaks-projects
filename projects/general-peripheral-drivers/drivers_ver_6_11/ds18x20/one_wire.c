
/*********************************************************************************************************
 Title  :   C include file for the ONE_WIRE FUNCTIONS library (one_wire.c)
 Author:    Chris efstathiou  
 E-mail:    hendrix@vivodinet.gr
 Homepage:  ........................
 Date:      18/Apr/2007 
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE but without any warranty of any kind.
*********************************************************************************************************/

/* 
    Although i could had written it using the new style, i prefer the old one for compatibility's sake.
*/

/********************************************************************************************************/
/*                                   PREPROCESSOR DIRECTIVES                                            */
/********************************************************************************************************/

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

#include "one_wire.h"


/* Register name forming */
#ifndef CONCAT1
#define CONCAT1(a, b) CONCAT2(a, b)
#endif

#ifndef CONCAT2
#define CONCAT2(a, b) a ## b
#endif

#define ONE_WIRE_OUT_REG   CONCAT1(PORT, ONE_WIRE_PORT)
#define ONE_WIRE_DDR_REG   CONCAT1(DDR,  ONE_WIRE_PORT)
#define ONE_WIRE_PIN_REG   CONCAT1(PIN,  ONE_WIRE_PORT)

/*******************************************************************************************************/

#define ROM_CODE_BYTES           (OW_TOTAL_R0M_CODE_SIZE-OW_ROM_CODE_CRC_SIZE)   
#define TOTAL_ROM_SIZE           OW_TOTAL_R0M_CODE_SIZE

/* One wire absolute minimum and maximum limits that are used for timing */
#define OW_MAX_TIMEOUT_US                  4000
#define OW_TYP_TIMEOUT_US                  4000
#define OW_MIN_TIMEOUT_US                  3850

#define OW_MAX_ALARM_TIME_US               3840
#define OW_TYP_ALARM_TIME_US               2000
#define OW_MIN_ALARM_TIME_US               960

#define OW_MAX_RESET_PULSE_WIDTH_US        960
#define OW_TYP_RESET_PULSE_WIDTH_US        750  /* That is from my experience */
#define OW_MIN_RESET_PULSE_WIDTH_US        480

#define OW_MAX_WAIT_FOR_PRESENSE_PULSE_US  60
#define OW_TYP_WAIT_FOR_PRESENSE_PULSE_US  30
#define OW_MIN_WAIT_FOR_PRESENSE_PULSE_US  15

#define OW_MAX_PRESENSE_PULSE_WIDTH_US     240
#define OW_TYP_PRESENSE_PULSE_WIDTH_US     120
#define OW_MIN_PRESENSE_PULSE_WIDTH_US     60

#define OW_MAX_WRITE_SLOT_DURATION_US      120
#define OW_TYP_WRITE_SLOT_DURATION_US      90   /* That is from my experience */
#define OW_MIN_WRITE_SLOT_DURATION_US      60

#define OW_MAX_READ_SLOT_DURATION_US       120
#define OW_TYP_READ_SLOT_DURATION_US       90   /* That is from my experience */
#define OW_MIN_READ_SLOT_DURATION_US       60

#define OW_MAX_READ_POSITION_TIME_US       15
#define OW_TYP_READ_POSITION_TIME_US       14   /* That is from my experience */
#define OW_MIN_READ_POSITION_TIME_US       5    /* That is from my experience */

#define OW_MAX_RECOVERY_TIME_US            15
#define OW_TYP_RECOVERY_TIME_US            1    /* That is from my experience for short cables */
#define OW_MIN_RECOVERY_TIME_US            1

#if ONE_WIRE_DEFAULT_TIMING == 1
#warning "DEFAULT TIMING VALUES ARE USED!"

#undef ONE_WIRE_RECOVERY_TIME_US           /* The time for the pullup resistor to get the line high */
#undef ONE_WIRE_READ_POSITION_US           /* When to sample the DQ line for rx, after DQ going low */
#undef ONE_WIRE_WRITE_SLOT_TIME_US         /* The total time of the WRITE SLOT  */ 
#undef ONE_WIRE_READ_SLOT_TIME_US          /* The total time of the READ SLOT  */ 
#undef ONE_WIRE_RESET_TIME_US              /* The total reset pulse time */
#undef ONE_WIRE_WAIT_FOR_PRESENSE_US       /* The max time to wait for a presence pulse after reset */
#undef ONE_WIRE_PRESENSE_DURATION_US       /* The minimum presense pulse width in order to be valid. */
#undef ONE_WIRE_ALARM_TIME_US              /* The required presense pulse duration for a valid alarm */
#undef ONE_WIRE_TIMEOUT_US                 /* After that time elapses a fault is detected */

#define ONE_WIRE_RECOVERY_TIME_US          OW_TYP_RECOVERY_TIME_US
#define ONE_WIRE_READ_POSITION_US          OW_TYP_READ_POSITION_TIME_US
#define ONE_WIRE_WRITE_SLOT_TIME_US        OW_TYP_WRITE_SLOT_DURATION_US
#define ONE_WIRE_READ_SLOT_TIME_US         OW_TYP_READ_SLOT_DURATION_US
#define ONE_WIRE_RESET_TIME_US             OW_TYP_RESET_PULSE_WIDTH_US
#define ONE_WIRE_WAIT_FOR_PRESENSE_US      OW_TYP_WAIT_FOR_PRESENSE_PULSE_US  
#define ONE_WIRE_PRESENSE_DURATION_US      OW_MIN_PRESENSE_PULSE_WIDTH_US
#define ONE_WIRE_ALARM_TIME_US             OW_TYP_ALARM_TIME_US
#define ONE_WIRE_TIMEOUT_US                OW_TYP_TIMEOUT_US

#endif

/* 1 WIRE BUS TIMING ERROR CHECKING */  
/*******************************************************************************************************/

#if ONE_WIRE_TIMEOUT_US < OW_MIN_TIMEOUT_US 
#warning "ONE_WIRE_TIMEOUT_US OUT OF SPECS"
#endif
/*******************************************************************************************************/

#if (ONE_WIRE_ALARM_TIME_US > OW_MAX_ALARM_TIME_US)||(ONE_WIRE_ALARM_TIME_US < OW_MIN_ALARM_TIME_US) 
#warning "ONE_WIRE_ALARM_TIME_US OUT OF SPECS"
#endif
/*******************************************************************************************************/

#if (((((ONE_WIRE_RESET_TIME_US)*1000L)/(1000000000/F_CPU))-1)/4) <= 0 
#warning "ONE_WIRE_RESET_TIME_US SET TO MINIMUM"
#undef  ONE_WIRE_RESET_TIME_US_US 
#define ONE_WIRE_RESET_TIME_US 480
#endif

#if ONE_WIRE_RESET_TIME_US < OW_MIN_RESET_PULSE_WIDTH_US
#warning "ONE_WIRE_RESET_TIME_US TOO SMALL"
#endif

#if ONE_WIRE_RESET_TIME_US > OW_MAX_RESET_PULSE_WIDTH_US
#warning "ONE_WIRE_RESET_TIME_US OUT OF SPECS"
#warning "1 WIRE RESET TIME MIGHT CAUSE HARDWARE RESETS"
#endif
/*******************************************************************************************************/

#if (((((ONE_WIRE_WAIT_FOR_PRESENSE_US)*1000L)/(1000000000/F_CPU))-1)/4) <= 0 
#warning "ONE_WIRE_WAIT_FOR_PRESENSE_US SET TO MINIMUM"
#undef  ONE_WIRE_WAIT_FOR_PRESENSE_US 
#define ONE_WIRE_WAIT_FOR_PRESENSE_US   OW_MIN_WAIT_FOR_PRESENSE_PULSE_US 
#endif

#if ONE_WIRE_WAIT_FOR_PRESENSE_US < OW_MIN_WAIT_FOR_PRESENSE_PULSE_US
#warning "ONE_WIRE_WAIT_FOR_PRESENSE_US TOO SMALL"
#endif

#if ONE_WIRE_WAIT_FOR_PRESENSE_US > OW_MAX_WAIT_FOR_PRESENSE_PULSE_US
#warning "ONE_WIRE_WAIT_FOR_PRESENSE_US OUT OF SPECS"
#endif
/*******************************************************************************************************/

#if (((((ONE_WIRE_PRESENSE_DURATION_US)*1000L)/(1000000000/F_CPU))-1)/4) <= 0 
#warning "ONE_WIRE_PRESENSE_DURATION_US SET TO MINIMUM"
#undef  ONE_WIRE_PRESENSE_DURATION_US 
#define ONE_WIRE_PRESENSE_DURATION_US   OW_MIN_PRESENSE_PULSE_WIDTH_US 
#endif

#if ONE_WIRE_PRESENSE_DURATION_US > OW_MAX_PRESENSE_PULSE_WIDTH_US
#warning "ONE_WIRE_PRESENSE_DURATION_US OUT OF SPECS "
#endif
/*******************************************************************************************************/

#if (((((ONE_WIRE_WRITE_SLOT_TIME_US)*1000L)/(1000000000/F_CPU))-1)/4) <= 0 
#warning "ONE_WIRE_WRITE_SLOT_TIME_US SET TO MINIMUM"
#undef  ONE_WIRE_WRITE_SLOT_TIME_US 
#define ONE_WIRE_WRITE_SLOT_TIME_US   OW_MIN_WRITE_SLOT_DURATION_US 
#endif

#if ONE_WIRE_WRITE_SLOT_TIME_US > 120 || ONE_WIRE_WRITE_SLOT_TIME_US < 60
#warning "ONE_WIRE_WRITE_SLOT_TIME_US OUT OF SPECS"
#endif
/*******************************************************************************************************/

#if (((((ONE_WIRE_READ_SLOT_TIME_US)*1000L)/(1000000000/F_CPU))-1)/4) <= 0 
#warning "ONE_WIRE_READ_SLOT_TIME_US SET TO MINIMUM "
#undef  ONE_WIRE_READ_SLOT_TIME_US 
#define ONE_WIRE_READ_SLOT_TIME_US   OW_MIN_READ_SLOT_DURATION_US 
#endif

#if ONE_WIRE_READ_SLOT_TIME_US > 120 || ONE_WIRE_READ_SLOT_TIME_US < 60
#warning "ONE_WIRE_READ_SLOT_TIME_US OUT OF SPECS"
#endif
/*******************************************************************************************************/

#if  ONE_WIRE_RECOVERY_TIME_US > OW_MAX_RECOVERY_TIME_US 
#warning "ONE_WIRE_RECOVERY_TIME_US OUT OF SPECS"
#endif

#if  ONE_WIRE_RECOVERY_TIME_US < OW_MIN_RECOVERY_TIME_US 
#warning "ONE_WIRE_RECOVERY_TIME_US TOO SMALL"
#endif
/*******************************************************************************************************/

#if ONE_WIRE_READ_POSITION_US > OW_MAX_READ_POSITION_TIME_US
#warning "1 WIRE READ_POSITION OUTSIDE READ WINDOW"
#endif

#if ONE_WIRE_READ_POSITION_US < OW_MIN_READ_POSITION_TIME_US
#warning "ONE_WIRE_READ_POSITION_US TOO SMALL"
#endif
/*******************************************************************************************************/

/* CRC8 checking disabled message */
#if OW_CALCULATE_CRC8_NEEDED == 0
#warning "CRC8 CHECKING IS DISABLED"
#endif

#if ONE_WIRE_DYNAMIC_IO_CHANGE == 1
#define OW_DQ_CHANGE_OVERHEAD_CYCLES    12
#define OW_DQ_TEST_OVERHEAD_CYCLES      (OW_DQ_CHANGE_OVERHEAD_CYCLES/2)       
#elif ONE_WIRE_DYNAMIC_IO_CHANGE == 0
#define OW_DQ_CHANGE_OVERHEAD_CYCLES    1
#define OW_DQ_TEST_OVERHEAD_CYCLES      2
#endif

/*#####################################################################################################*/
/*#####################################################################################################*/
/*#####################################################################################################*/
#if OW_WILL_USE_A_TIMER == 0
/*#####################################################################################################*/
/*#####################################################################################################*/
/*#####################################################################################################*/


/********************************************************************************************************/
/*                        "DELAY_OW_RECOVERY_TIME()" macro definition.                                  */
/********************************************************************************************************/

#warning " ONE WIRE WILL USE INLINE DELAY FUNCTIONS"

#define OW_RECOVERY_TIME_US          CONCAT1(ONE_WIRE_RECOVERY_TIME_US, L)

#define OW_RECOVERY_TIME_ACCURATE    ((OW_RECOVERY_TIME_US*10000L)/(1000000000/F_CPU))
#define OW_RECOVERY_TIME_REAL        ((OW_RECOVERY_TIME_US*1000L)/(1000000000/F_CPU))

#if   OW_RECOVERY_TIME_ACCURATE - (OW_RECOVERY_TIME_REAL*10) >=5

#define OW_RECOVERY_TIME_TEMP        (OW_RECOVERY_TIME_REAL+1)

#else 

#define OW_RECOVERY_TIME_TEMP        OW_RECOVERY_TIME_REAL

#endif   /* #if   OW_RECOVERY_TIME_ACCURATE - (OW_RECOVERY_TIME_REAL*10) >=5 */

/* The recovery time in cycles is taking in to account the DQ logic overhead ("OW_RECOVERY_TIME"). */
#define OW_RECOVERY_TIME             (OW_RECOVERY_TIME_TEMP-OW_DQ_CHANGE_OVERHEAD_CYCLES)

/* Timing code building for the "DELAY_OW_RECOVERY_TIME()" macro. */
/*------------------------------------------------------------------------------------------------------*/
#if  OW_RECOVERY_TIME<= 0
#define DELAY_OW_RECOVERY_TIME()     { }

#elif  OW_RECOVERY_TIME == 1
#define DELAY_OW_RECOVERY_TIME()     __asm__ volatile("nop")

#elif OW_RECOVERY_TIME == 2
#define DELAY_OW_RECOVERY_TIME()     { __asm__ volatile("nop"); __asm__ volatile("nop"); }

#elif OW_RECOVERY_TIME >= 3 

#define  OW_RECOVERY_DELAY_LOOPS     (OW_RECOVERY_TIME/3)

#if  OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 0
#define DELAY_OW_RECOVERY_TIME()     delay_c(OW_RECOVERY_DELAY_LOOPS)

#elif OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 1

#define DELAY_OW_RECOVERY_TIME()     { delay_c(OW_RECOVERY_DELAY_LOOPS);        \
                                       __asm__ volatile("nop");                 \
                                     } 
#elif OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 2

#define DELAY_OW_RECOVERY_TIME()     { delay_c(OW_RECOVERY_DELAY_LOOPS);        \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                     }
#elif OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 3

#define DELAY_OW_RECOVERY_TIME()     { delay_c(OW_RECOVERY_DELAY_LOOPS);        \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                       __asm__ volatile("nop");                          \
                                     } 

#endif /* #if  OW_RECOVERY_TIME - OW_RECOVERY_DELAY_LOOPS == 0 */

#endif  /* #if  OW_RECOVERY_TIME<= 0 -> #elif  OW_RECOVERY_TIME >= 21 */


/********************************************************************************************************/
/*                          "DELAY_OW_READ_POS()" macro definition.                                     */
/********************************************************************************************************/
/* 
    IMPORTANT!!! The 1 wire recovery time cycles are taken in to account
                 So if you select recovery time of 1 us and read position @ 15 us
                 then the cycles that are calculated below will reflect a delay of 
                 15-1=14 us
*/

#define OW_READ_POSITION_US       CONCAT1(ONE_WIRE_READ_POSITION_US, L)

#define OW_READ_POS_ACCURATE      ((OW_READ_POSITION_US*10000L)/(1000000000/F_CPU))
#define OW_READ_POS_REAL          ((OW_READ_POSITION_US*1000L)/(1000000000/F_CPU))

#if  (OW_READ_POS_ACCURATE - (OW_READ_POS_REAL*10)) >=5

#define OW_READ_POS_TEMP1         ((OW_READ_POS_REAL+1)-OW_DQ_TEST_OVERHEAD_CYCLES)

#else 

#define OW_READ_POS_TEMP1         (OW_READ_POS_REAL-OW_DQ_TEST_OVERHEAD_CYCLES)

#endif   /* #if  (OW_READ_POS_ACCURATE - (OW_READ_POS_REAL*10)) >=5 */

/* 
   In order to read at exactly the wanted position starting from the time the DQ line went low,
   we must subtract from "OW_READ_POS_TEMP1" the cycles that have passed as recovery time.
   To do that we must observe the subtraction "OW_RECOVERY_TIME_TEMP-OW_DQ_CHANGE_OVERHEAD_CYCLES".
   If this subtraction result is positive, that means that "OW_RECOVERY_TIME_TEMP" cycles in total
   have elapsed and that "DELAY_OW_RECOVERY_TIME()" macro contains real code which provides
   a delay of "OW_RECOVERY_TIME_TEMP-OW_DQ_CHANGE_OVERHEAD_CYCLES" cycles.
   If this subtraction result is negative, that means that the "DELAY_OW_RECOVERY_TIME()" macro is
   an empty macro so only "OW_DQ_CHANGE_OVERHEAD_CYCLES" have elapsed.
   Also the overhead cycles involved when testing the DQ line, must be taken also in to account.
   To conclude, the appropriate elapsed cycles are subtracted from the "OW_READ_POS" cycles.
*/
#if   OW_RECOVERY_TIME >= 0
#define OW_READ_POS               (OW_READ_POS_TEMP1-OW_RECOVERY_TIME_TEMP)
#elif OW_RECOVERY_TIME < 0         
#define OW_READ_POS               (OW_READ_POS_TEMP1-OW_DQ_CHANGE_OVERHEAD_CYCLES)
#endif



#if  OW_READ_POS<= 0
#warning " READ TIME POSITION DELAY IS 0"
#define DELAY_OW_READ_POS()     { }

#elif  OW_READ_POS== 1
#define DELAY_OW_READ_POS()     __asm__ volatile("nop")

#elif OW_READ_POS == 2
#define DELAY_OW_READ_POS()     { __asm__ volatile("nop"); __asm__ volatile("nop"); }

#elif OW_READ_POS == 3
#define DELAY_OW_READ_POS()     { __asm__ volatile("nop"); __asm__ volatile("nop"); __asm__ volatile("nop"); }

#elif OW_READ_POS > 3 

#define  OW_READ_POS_DELAY_LOOPS     (OW_READ_POS/4)

#if  OW_READ_POS - (OW_READ_POS_DELAY_LOOPS*4) == 0

#define DELAY_OW_READ_POS()     delay_i(OW_READ_POS_DELAY_LOOPS)

#elif OW_READ_POS - (OW_READ_POS_DELAY_LOOPS*4) == 1

#define DELAY_OW_READ_POS()     { delay_i(OW_READ_POS_DELAY_LOOPS);        \
                                       __asm__ volatile("nop");                          \
                                     } 

#elif OW_READ_POS - (OW_READ_POS_DELAY_LOOPS*4) == 2

#define DELAY_OW_READ_POS()     { delay_i(OW_READ_POS_DELAY_LOOPS);        \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                     }
#elif OW_READ_POS - (OW_READ_POS_DELAY_LOOPS*4) == 3

#define DELAY_OW_READ_POS()     { delay_i(OW_READ_POS_DELAY_LOOPS);        \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                       __asm__ volatile("nop");                          \
                                     } 

#endif /* #if  OW_READ_POS - OW_READ_POS_DELAY_LOOPS == 0 */

#endif  /* #if  OW_READ_POS<= 0 -> #elif OW_READ_POS >= 3 */




/* It takes 2 cycles in order to load the registers for the delay count down. */
#define DELAY_US(us)            delay_i( (((((us)*1000L)/(1000000000/F_CPU))-2)/4) )  

/*#####################################################################################################*/
/*#####################################################################################################*/
/*#####################################################################################################*/
#elif OW_WILL_USE_A_TIMER == 1 /* #if OW_WILL_USE_A_TIMER == 0  */
/*#####################################################################################################*/
/*#####################################################################################################*/
/*#####################################################################################################*/
#warning " ONE WIRE WILL USE AN 8 BIT TIMER"

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if defined(__AVR_ATmega103__) || defined(__AVR_ATmega128__) || \
    defined(__AVR_ATmega8535__) || defined(__AVR_ATmega8515__)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if OW_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define OW_TCNT            TCNT0
#define OW_TCCR            TCCR0
#define OW_TOIE            TOIE0
#define OW_OCIE            OCIE0
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif OW_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define OW_TCNT            TCNT2
#define OW_TCCR            TCCR2
#define OW_TOIE            TOIE2
#define OW_OCIE            OCIE2
#define TIMER_SIGNAL()     SIGNAL(SIG_OVERFLOW2)

#endif

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || \
       defined(__AVR_ATmega64__)  
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if OW_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     0
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define OW_TCNT            TCNT0
#define OW_TCCR            TCCR0
#define OW_TOIE            TOIE0
#define OW_OCIE            OCIE0
#define TIMER_SIGNAL()     SIGNAL(SIG_OVERFLOW0)

#elif OW_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define OW_TCNT            TCNT2
#define OW_TCCR            TCCR2
#define OW_TOIE            TOIE2
#define OW_OCIE            OCIE2
#define TIMER_SIGNAL()     SIGNAL(SIG_OVERFLOW2)
                         
#endif

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_ATmega161__) || defined(__AVR_ATmega163__) || defined(__AVR_AT90S8515__) || \
       defined(__AVR_AT90S8535__) || defined(__AVR_AT90S4434__)  
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 2

#if OW_USE_TIMER == 0  || NUMBER_OF_8_BIT_TIMERS == 1

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define OW_TCNT            TCNT0
#define OW_TCCR            TCCR0
#define OW_TOIE            TOIE0
#if defined(__AVR_ATmega161__)
#define OW_OCIE            OCIE0
#else
#define OW_OCIE            0
#endif
#define TIMER_SIGNAL()      SIGNAL(SIG_OVERFLOW0)

#elif OW_USE_TIMER == 2 && NUMBER_OF_8_BIT_TIMERS >= 2

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_64     ( (1<<2) )
#define TIMER_DIV_BY_128    ( (1<<2)|(1<<0) )
#define TIMER_DIV_BY_256    ( (1<<2)|(1<<1) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<1)|(1<<0) )

#define OW_TCNT            TCNT2
#define OW_TCCR            TCCR2
#define OW_TOIE            TOIE2
#define OW_OCIE            OCIE2
#define TIMER_SIGNAL()     SIGNAL(SIG_OVERFLOW2)
                         
#endif


/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  defined(__AVR_AT90S4433__)  || defined(__AVR_AT90S2343__) || defined(__AVR_AT90S2323__)
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#define NUMBER_OF_8_BIT_TIMERS 1

#if OW_USE_TIMER > 0
#undef OW_USE_TIMER
#warning " THERE IS NOT SUCH AN 8 BIT TIMER. WILL USE TIMER 0 "
#define OW_USE_TIMER 0
#endif

#define TIMER_DIV_BY_1      ( (1<<0) )
#define TIMER_DIV_BY_8      ( (1<<1) )
#define TIMER_DIV_BY_32     0
#define TIMER_DIV_BY_64     ( (1<<1)|(1<<0) )
#define TIMER_DIV_BY_128    0
#define TIMER_DIV_BY_256    ( (1<<2) )
#define TIMER_DIV_BY_1024   ( (1<<2)|(1<<0) )

#define OW_TCNT            TCNT0
#define OW_TCCR            TCCR0
#define OW_TOIE            TOIE0
#define OW_OCIE            OCIE0
#define TIMER_SIGNAL()     SIGNAL(SIG_OVERFLOW0)

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/* TIMER SELECTION RELATED ERROR MESSAGES */

#if    NUMBER_OF_8_BIT_TIMERS == 2 
#if    OW_USE_TIMER != 0 && OW_USE_TIMER != 2 
#error "SELECT TIMER 0 OR 2 PLEASE! " 
#endif
#endif /* #if NUMBER_OF_8_BIT_TIMERS == 2  */


/* Set up the timer for the ONE_WIRE_READ_POSITION_US time. */
#define OW_READ_POS_CYCLES           ((ONE_WIRE_READ_POSITION_US*1000L)/(1000000000/F_CPU))

#if OW_READ_POS_CYCLES <= 0
#error " OW READ_POSITION TIME TOO SMALL "

#elif OW_READ_POS_CYCLES <= 250

#define OW_READ_POS_PRESCALER_BITS  TIMER_DIV_BY_1
#define OW_READ_POS_PRESCALER     1

#elif OW_READ_POS_CYCLES > 250

#if   TIMER_DIV_BY_8 > 0 && (OW_DELAY_ACCURATE/8) <= 250

#define OW_READ_POS_PRESCALER_BITS  TIMER_DIV_BY_8
#define OW_READ_POS_PRESCALER     8

#elif TIMER_DIV_BY_32 > 0 && (OW_READ_POS_CYCLES/32) <= 250

#define OW_READ_POS_PRESCALER_BITS  TIMER_DIV_BY_32
#define OW_READ_POS_PRESCALER     32

#elif TIMER_DIV_BY_64 > 0 && (OW_READ_POS_CYCLES/64) <= 250

#define OW_READ_POS_PRESCALER_BITS  TIMER_DIV_BY_64
#define OW_READ_POS_PRESCALER     64

#elif TIMER_DIV_BY_128 > 0 && (OW_READ_POS_CYCLES/128) <= 250

#define OW_READ_POS_PRESCALER_BITS  TIMER_DIV_BY_128
#define OW_READ_POS_PRESCALER     128

#elif TIMER_DIV_BY_256 > 0 && (OW_READ_POS_CYCLES/256) <= 250

#define OW_READ_POS_PRESCALER_BITS  TIMER_DIV_BY_256
#define OW_READ_POS_PRESCALER     256

#elif TIMER_DIV_BY_1024 > 0 && (OW_READ_POS_CYCLES/1024) <= 250

#define OW_READ_POS_PRESCALER_BITS  TIMER_DIV_BY_1024
#define OW_READ_POS_PRESCALER     1024

#endif  
#endif 

#define OW_READ_POS_TIMER_COUNT  ( OW_READ_POS_CYCLES/OW_READ_POS_PRESCALER )
#define DELAY_OW_READ_POS()      { OW_TCCR = OW_READ_POS_PRESCALER_BITS; OW_TCNT = 0;  \
                                   while(OW_TCNT < OW_READ_POS_TIMER_COUNT); }  




/* Set up the timer for the ONE_WIRE_READ_SLOT_TIME_US time. */
#define OW_READ_SLOT_US          (ONE_WIRE_READ_SLOT_TIME_US-ONE_WIRE_READ_POSITION_US)
#define OW_READ_SLOT_CYCLES           ((OW_READ_SLOT_US*1000L)/(1000000000/F_CPU))

#if OW_READ_SLOT_CYCLES <= 0
#error " OW READ_SLOT TIME TOO SMALL "

#elif OW_READ_SLOT_CYCLES <= 250

#define OW_READ_SLOT_PRESCALER_BITS  TIMER_DIV_BY_1
#define OW_READ_SLOT_PRESCALER     1

#elif OW_READ_SLOT_CYCLES > 250

#if   TIMER_DIV_BY_8 > 0 && (OW_DELAY_ACCURATE/8) <= 250

#define OW_READ_SLOT_PRESCALER_BITS  TIMER_DIV_BY_8
#define OW_READ_SLOT_PRESCALER     8

#elif TIMER_DIV_BY_32 > 0 && (OW_READ_SLOT_CYCLES/32) <= 250

#define OW_READ_SLOT_PRESCALER_BITS  TIMER_DIV_BY_32
#define OW_READ_SLOT_PRESCALER     32

#elif TIMER_DIV_BY_64 > 0 && (OW_READ_SLOT_CYCLES/64) <= 250

#define OW_READ_SLOT_PRESCALER_BITS  TIMER_DIV_BY_64
#define OW_READ_SLOT_PRESCALER     64

#elif TIMER_DIV_BY_128 > 0 && (OW_READ_SLOT_CYCLES/128) <= 250

#define OW_READ_SLOT_PRESCALER_BITS  TIMER_DIV_BY_128
#define OW_READ_SLOT_PRESCALER     128

#elif TIMER_DIV_BY_256 > 0 && (OW_READ_SLOT_CYCLES/256) <= 250

#define OW_READ_SLOT_PRESCALER_BITS  TIMER_DIV_BY_256
#define OW_READ_SLOT_PRESCALER     256

#elif TIMER_DIV_BY_1024 > 0 && (OW_READ_SLOT_CYCLES/1024) <= 250

#define OW_READ_SLOT_PRESCALER_BITS  TIMER_DIV_BY_1024
#define OW_READ_SLOT_PRESCALER     1024

#endif 
#endif 

#define OW_READ_SLOT_TIMER_COUNT  ( OW_READ_SLOT_CYCLES/OW_READ_SLOT_PRESCALER )
#define DELAY_OW_READ_SLOT()      { OW_TCCR = OW_READ_SLOT_PRESCALER_BITS; OW_TCNT = 0;  \
                                   while(OW_TCNT < OW_READ_SLOT_TIMER_COUNT); }  




/* Set up the timer for the ONE_WIRE_WRITE_SLOT_TIME_US time. */
#define OW_WRITE_SLOT_CYCLES           ((ONE_WIRE_WRITE_SLOT_TIME_US*1000L)/(1000000000/F_CPU))

#if OW_WRITE_SLOT_CYCLES <= 0
#error " OW WRITE_SLOT TIME TOO SMALL "

#elif OW_WRITE_SLOT_CYCLES <= 250

#define OW_WRITE_SLOT_PRESCALER_BITS  TIMER_DIV_BY_1
#define OW_WRITE_SLOT_PRESCALER     1

#elif OW_WRITE_SLOT_CYCLES > 250

#if   TIMER_DIV_BY_8 > 0 && (OW_DELAY_ACCURATE/8) <= 250

#define OW_WRITE_SLOT_PRESCALER_BITS  TIMER_DIV_BY_8
#define OW_WRITE_SLOT_PRESCALER     8

#elif TIMER_DIV_BY_32 > 0 && (OW_WRITE_SLOT_CYCLES/32) <= 250

#define OW_WRITE_SLOT_PRESCALER_BITS  TIMER_DIV_BY_32
#define OW_WRITE_SLOT_PRESCALER     32

#elif TIMER_DIV_BY_64 > 0 && (OW_WRITE_SLOT_CYCLES/64) <= 250

#define OW_WRITE_SLOT_PRESCALER_BITS  TIMER_DIV_BY_64
#define OW_WRITE_SLOT_PRESCALER     64

#elif TIMER_DIV_BY_128 > 0 && (OW_WRITE_SLOT_CYCLES/128) <= 250

#define OW_WRITE_SLOT_PRESCALER_BITS  TIMER_DIV_BY_128
#define OW_WRITE_SLOT_PRESCALER     128

#elif TIMER_DIV_BY_256 > 0 && (OW_WRITE_SLOT_CYCLES/256) <= 250

#define OW_WRITE_SLOT_PRESCALER_BITS  TIMER_DIV_BY_256
#define OW_WRITE_SLOT_PRESCALER     256

#elif TIMER_DIV_BY_1024 > 0 && (OW_WRITE_SLOT_CYCLES/1024) <= 250

#define OW_WRITE_SLOT_PRESCALER_BITS  TIMER_DIV_BY_1024
#define OW_WRITE_SLOT_PRESCALER     1024

#endif 
#endif 

#define OW_WRITE_SLOT_TIMER_COUNT  ( OW_WRITE_SLOT_CYCLES/OW_WRITE_SLOT_PRESCALER )
#define DELAY_OW_WRITE_SLOT()      { OW_TCCR = OW_WRITE_SLOT_PRESCALER_BITS; OW_TCNT = 0;  \
                                     while(OW_TCNT < OW_WRITE_SLOT_TIMER_COUNT); }  

 



 

/* Set up the timer for the ONE_WIRE_WAIT_FOR_PRESENSE_US time. */
#define OW_WAIT_FOR_PRESENSE_CYCLES        ((ONE_WIRE_WAIT_FOR_PRESENSE_US*1000L)/(1000000000/F_CPU))

#if OW_WAIT_FOR_PRESENSE_CYCLES <= 0
#error " OW WAIT FOR PRESENCE TIME TOO SMALL "

#elif OW_WAIT_FOR_PRESENSE_CYCLES <= 250

#define OW_WAIT_FOR_PRESENSE_PRESCALER_BITS  TIMER_DIV_BY_1
#define OW_WAIT_FOR_PRESENSE_PRESCALER     1

#elif OW_WAIT_FOR_PRESENSE_CYCLES > 250

#if   TIMER_DIV_BY_8 > 0 && (OW_WAIT_FOR_PRESENSE_CYCLES/8) <= 250

#define OW_WAIT_FOR_PRESENSE_PRESCALER_BITS  TIMER_DIV_BY_8
#define OW_WAIT_FOR_PRESENSE_PRESCALER     8

#elif TIMER_DIV_BY_32 > 0 && (OW_WAIT_FOR_PRESENSE_CYCLES/32) <= 250

#define OW_WAIT_FOR_PRESENSE_PRESCALER_BITS  TIMER_DIV_BY_32
#define OW_WAIT_FOR_PRESENSE_PRESCALER     32

#elif TIMER_DIV_BY_64 > 0 && (OW_WAIT_FOR_PRESENSE_CYCLES/64) <= 250

#define OW_WAIT_FOR_PRESENSE_PRESCALER_BITS  TIMER_DIV_BY_64
#define OW_WAIT_FOR_PRESENSE_PRESCALER     64

#elif TIMER_DIV_BY_128 > 0 && (OW_WAIT_FOR_PRESENSE_CYCLES/128) <= 250

#define OW_WAIT_FOR_PRESENSE_PRESCALER_BITS  TIMER_DIV_BY_128
#define OW_WAIT_FOR_PRESENSE_PRESCALER     128

#elif TIMER_DIV_BY_256 > 0 && (OW_WAIT_FOR_PRESENSE_CYCLES/256) <= 250

#define OW_WAIT_FOR_PRESENSE_PRESCALER_BITS  TIMER_DIV_BY_256
#define OW_WAIT_FOR_PRESENSE_PRESCALER     256

#elif TIMER_DIV_BY_1024 > 0 && (OW_WAIT_FOR_PRESENSE_CYCLES/1024) <= 250

#define OW_WAIT_FOR_PRESENSE_PRESCALER_BITS  TIMER_DIV_BY_1024
#define OW_WAIT_FOR_PRESENSE_PRESCALER     1024

#endif  
#endif  

#define OW_WAIT_FOR_PRESENSE_TIMER_COUNT  ( OW_WAIT_FOR_PRESENSE_CYCLES/OW_WAIT_FOR_PRESENSE_PRESCALER )
#define DELAY_OW_WAIT_FOR_PRESENSE()      { OW_TCCR = OW_WAIT_FOR_PRESENSE_PRESCALER_BITS; OW_TCNT = 0;  \
                                            while(OW_TCNT < OW_WAIT_FOR_PRESENSE_TIMER_COUNT); }

 

/* Set up the timer for the ONE_WIRE_PRESENSE_DURATION_US time. */
#define OW_PRESENSE_DURATION_CYCLES        ((ONE_WIRE_PRESENSE_DURATION_US*1000L)/(1000000000/F_CPU))

#if OW_PRESENSE_DURATION_CYCLES <= 0
#error " OW PRESENCE DURATION TIME TOO SMALL "

#elif OW_PRESENSE_DURATION_CYCLES <= 250

#define OW_PRESENCE_DURATION_PRESCALER_BITS  TIMER_DIV_BY_1
#define OW_PRESENCE_DURATION_PRESCALER     1

#elif OW_PRESENSE_DURATION_CYCLES > 250

#if   TIMER_DIV_BY_8 > 0 && (OW_PRESENCE_DURATION_CYCLES/8) <= 250

#define OW_PRESENCE_DURATION_PRESCALER_BITS  TIMER_DIV_BY_8
#define OW_PRESENCE_DURATION_PRESCALER     8

#elif TIMER_DIV_BY_32 > 0 && (OW_PRESENSE_DURATION_CYCLES/32) <= 250

#define OW_PRESENCE_DURATION_PRESCALER_BITS  TIMER_DIV_BY_32
#define OW_PRESENCE_DURATION_PRESCALER     32

#elif TIMER_DIV_BY_64 > 0 && (OW_PRESENSE_DURATION_CYCLES/64) <= 250

#define OW_PRESENCE_DURATION_PRESCALER_BITS  TIMER_DIV_BY_64
#define OW_PRESENCE_DURATION_PRESCALER     64

#elif TIMER_DIV_BY_128 > 0 && (OW_PRESENSE_DURATION_CYCLES/128) <= 250

#define OW_PRESENCE_DURATION_PRESCALER_BITS  TIMER_DIV_BY_128
#define OW_PRESENCE_DURATION_PRESCALER     128

#elif TIMER_DIV_BY_256 > 0 && (OW_PRESENSE_DURATION_CYCLES/256) <= 250

#define OW_PRESENCE_DURATION_PRESCALER_BITS  TIMER_DIV_BY_256
#define OW_PRESENCE_DURATION_PRESCALER     256

#elif TIMER_DIV_BY_1024 > 0 && (OW_PRESENSE_DURATION_CYCLES/1024) <= 250

#define OW_PRESENCE_DURATION_PRESCALER_BITS  TIMER_DIV_BY_1024
#define OW_PRESENCE_DURATION_PRESCALER     1024

#endif  
#endif  

#define OW_PRESENCE_DURATION_TIMER_COUNT  ( OW_PRESENSE_DURATION_CYCLES/OW_PRESENCE_DURATION_PRESCALER )
#define DELAY_OW_PRESENCE_DURATION()      { OW_TCCR = OW_PRESENCE_DURATION_PRESCALER_BITS; OW_TCNT = 0;  \
                                            while(OW_TCNT < OW_PRESENCE_DURATION_TIMER_COUNT); }

 

/* Set up the timer for the ONE_WIRE_ALARM_TIME_US-ONE time. */
#define OW_ALARM_US                 (ONE_WIRE_ALARM_TIME_US-ONE_WIRE_PRESENSE_DURATION_US)

#define OW_ALARM_CYCLES             ((OW_ALARM_US*1000L)/(1000000000/F_CPU))

#if OW_ALARM_CYCLES <= 0
#error " OW ALARM TIME TOO SMALL "

#elif OW_ALARM_CYCLES <= 250

#define OW_ALARM_PRESCALER_BITS  TIMER_DIV_BY_1
#define OW_ALARM_PRESCALER     1

#elif OW_ALARM_CYCLES > 250

#if   TIMER_DIV_BY_8 > 0 && (OW_ALARM_CYCLES/8) <= 250

#define OW_ALARM_PRESCALER_BITS  TIMER_DIV_BY_8
#define OW_ALARM_PRESCALER     8

#elif TIMER_DIV_BY_32 > 0 && (OW_ALARM_CYCLES/32) <= 250

#define OW_ALARM_PRESCALER_BITS  TIMER_DIV_BY_32
#define OW_ALARM_PRESCALER     32

#elif TIMER_DIV_BY_64 > 0 && (OW_ALARM_CYCLES/64) <= 250

#define OW_ALARM_PRESCALER_BITS  TIMER_DIV_BY_64
#define OW_ALARM_PRESCALER     64

#elif TIMER_DIV_BY_128 > 0 && (OW_ALARM_CYCLES/128) <= 250

#define OW_ALARM_PRESCALER_BITS  TIMER_DIV_BY_128
#define OW_ALARM_PRESCALER     128

#elif TIMER_DIV_BY_256 > 0 && (OW_ALARM_CYCLES/256) <= 250

#define OW_ALARM_PRESCALER_BITS  TIMER_DIV_BY_256
#define OW_ALARM_PRESCALER     256

#elif TIMER_DIV_BY_1024 > 0 && (OW_ALARM_CYCLES/1024) <= 250

#define OW_ALARM_PRESCALER_BITS  TIMER_DIV_BY_1024
#define OW_ALARM_PRESCALER     1024

#endif  
#endif  

#define OW_ALARM_TIMER_COUNT  ( OW_ALARM_CYCLES/OW_ALARM_PRESCALER )
#define DELAY_OW_ALARM()      { OW_TCCR = OW_ALARM_PRESCALER_BITS; OW_TCNT = 0;  \
                                while(OW_TCNT < OW_ALARM_TIMER_COUNT); }



/* Set up the timer for the ONE_WIRE_TIMEOUT_US time. */
#define OW_TIMEOUT_US     (ONE_WIRE_TIMEOUT_US-ONE_WIRE_ALARM_TIME_US-ONE_WIRE_PRESENSE_DURATION_US) 

#define OW_TIMEOUT_CYCLES        ((OW_TIMEOUT_US*1000L)/(1000000000/F_CPU))

#if OW_TIMEOUT_CYCLES <= 0
#error " OW TIMEOUT TIME TOO SMALL "

#elif OW_TIMEOUT_CYCLES <= 250

#define OW_TIMEOUT_PRESCALER_BITS  TIMER_DIV_BY_1
#define OW_TIMEOUT_PRESCALER     1

#elif OW_TIMEOUT_CYCLES > 250

#if   TIMER_DIV_BY_8 > 0 && (OW_TIMEOUT_CYCLES/8) <= 250

#define OW_TIMEOUT_PRESCALER_BITS  TIMER_DIV_BY_8
#define OW_TIMEOUT_PRESCALER     8

#elif TIMER_DIV_BY_32 > 0 && (OW_TIMEOUT_CYCLES/32) <= 250

#define OW_TIMEOUT_PRESCALER_BITS  TIMER_DIV_BY_32
#define OW_TIMEOUT_PRESCALER     32

#elif TIMER_DIV_BY_64 > 0 && (OW_TIMEOUT_CYCLES/64) <= 250

#define OW_TIMEOUT_PRESCALER_BITS  TIMER_DIV_BY_64
#define OW_TIMEOUT_PRESCALER     64

#elif TIMER_DIV_BY_128 > 0 && (OW_TIMEOUT_CYCLES/128) <= 250

#define OW_TIMEOUT_PRESCALER_BITS  TIMER_DIV_BY_128
#define OW_TIMEOUT_PRESCALER     128

#elif TIMER_DIV_BY_256 > 0 && (OW_TIMEOUT_CYCLES/256) <= 250

#define OW_TIMEOUT_PRESCALER_BITS  TIMER_DIV_BY_256
#define OW_TIMEOUT_PRESCALER     256

#elif TIMER_DIV_BY_1024 > 0 && (OW_TIMEOUT_CYCLES/1024) <= 250

#define OW_TIMEOUT_PRESCALER_BITS  TIMER_DIV_BY_1024
#define OW_TIMEOUT_PRESCALER     1024

#endif  
#endif  

#define OW_TIMEOUT_TIMER_COUNT  ( OW_TIMEOUT_CYCLES/OW_TIMEOUT_PRESCALER )
#define DELAY_OW_TIMEOUT()      { OW_TCCR = OW_TIMEOUT_PRESCALER_BITS; OW_TCNT = 0;  \
                                while(OW_TCNT < OW_TIMEOUT_TIMER_COUNT); }



/* Set up the timer for the ONE_WIRE_RESET_TIME_US time. */
#define OW_RESET_CYCLES        ((ONE_WIRE_RESET_TIME_US*1000L)/(1000000000/F_CPU))

#if OW_RESET_CYCLES <= 0
#error " OW RESET TIME TOO SMALL "

#elif OW_RESET_CYCLES <= 250

#define OW_RESET_PRESCALER_BITS  TIMER_DIV_BY_1
#define OW_RESET_PRESCALER     1

#elif OW_RESET_CYCLES > 250

#if   TIMER_DIV_BY_8 > 0 && (OW_RESET_CYCLES/8) <= 250

#define OW_RESET_PRESCALER_BITS  TIMER_DIV_BY_8
#define OW_RESET_PRESCALER     8

#elif TIMER_DIV_BY_32 > 0 && (OW_RESET_CYCLES/32) <= 250

#define OW_RESET_PRESCALER_BITS  TIMER_DIV_BY_32
#define OW_RESET_PRESCALER     32

#elif TIMER_DIV_BY_64 > 0 && (OW_RESET_CYCLES/64) <= 250

#define OW_RESET_PRESCALER_BITS  TIMER_DIV_BY_64
#define OW_RESET_PRESCALER     64

#elif TIMER_DIV_BY_128 > 0 && (OW_RESET_CYCLES/128) <= 250

#define OW_RESET_PRESCALER_BITS  TIMER_DIV_BY_128
#define OW_RESET_PRESCALER     128

#elif TIMER_DIV_BY_256 > 0 && (OW_RESET_CYCLES/256) <= 250

#define OW_RESET_PRESCALER_BITS  TIMER_DIV_BY_256
#define OW_RESET_PRESCALER     256

#elif TIMER_DIV_BY_1024 > 0 && (OW_RESET_CYCLES/1024) <= 250

#define OW_RESET_PRESCALER_BITS  TIMER_DIV_BY_1024
#define OW_RESET_PRESCALER     1024

#endif  
#endif  

#define OW_RESET_TIMER_COUNT  ( OW_RESET_CYCLES/OW_RESET_PRESCALER )
#define DELAY_OW_RESET()      { OW_TCCR = OW_RESET_PRESCALER_BITS; OW_TCNT = 0;  \
                                while(OW_TCNT < OW_RESET_TIMER_COUNT); }


/* Set up the delay for the ONE_WIRE_RECOVERY_TIME_US. the time is very small so a delay is used. */
#define OW_RECOVERY_TIME_US          CONCAT1(ONE_WIRE_RECOVERY_TIME_US, L)

#define OW_RECOVERY_TIME_ACCURATE    ((OW_RECOVERY_TIME_US*10000L)/(1000000000/F_CPU))
#define OW_RECOVERY_TIME_REAL        ((OW_RECOVERY_TIME_US*1000L)/(1000000000/F_CPU))

#if   OW_RECOVERY_TIME_ACCURATE - (OW_RECOVERY_TIME_REAL*10) >=5

#define OW_RECOVERY_TIME_TEMP        (OW_RECOVERY_TIME_REAL+1)

#else 

#define OW_RECOVERY_TIME_TEMP        OW_RECOVERY_TIME_REAL

#endif   /* #if   OW_RECOVERY_TIME_ACCURATE - (OW_RECOVERY_TIME_REAL*10) >=5 */

/* The recovery time in cycles is taking in to account the DQ logic overhead ("OW_RECOVERY_TIME"). */
#define OW_RECOVERY_TIME             (OW_RECOVERY_TIME_TEMP-OW_DQ_CHANGE_OVERHEAD_CYCLES)

/* Timing code building for the "DELAY_OW_RECOVERY_TIME()" macro. */
#if  OW_RECOVERY_TIME<= 0
#define DELAY_OW_RECOVERY_TIME()     { }

#elif  OW_RECOVERY_TIME == 1
#define DELAY_OW_RECOVERY_TIME()     __asm__ volatile("nop")

#elif OW_RECOVERY_TIME == 2
#define DELAY_OW_RECOVERY_TIME()     { __asm__ volatile("nop"); __asm__ volatile("nop"); }

#elif OW_RECOVERY_TIME >= 3 

#define  OW_RECOVERY_DELAY_LOOPS     (OW_RECOVERY_TIME/3)

#if  OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 0
#define DELAY_OW_RECOVERY_TIME()     delay_c(OW_RECOVERY_DELAY_LOOPS)

#elif OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 1

#define DELAY_OW_RECOVERY_TIME()     { delay_c(OW_RECOVERY_DELAY_LOOPS);        \
                                       __asm__ volatile("nop");                          \
                                     } 
#elif OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 2

#define DELAY_OW_RECOVERY_TIME()     { delay_c(OW_RECOVERY_DELAY_LOOPS);        \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                     }
#elif OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 3

#define DELAY_OW_RECOVERY_TIME()     { delay_c(OW_RECOVERY_DELAY_LOOPS);        \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                       __asm__ volatile("nop");                          \
                                     } 

#endif /* #if  OW_RECOVERY_TIME - OW_RECOVERY_DELAY_LOOPS == 0 */

#endif  /* #if  OW_RECOVERY_TIME<= 0 -> #elif  OW_RECOVERY_TIME >= 21 */
 



/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/
#endif /* #if OW_WILL_USE_A_TIMER == 0  --> #elif OW_WILL_USE_A_TIMER == 1  */
/*******************************************************************************************************/
/*******************************************************************************************************/
/*******************************************************************************************************/


/* Pin states */
#if ONE_WIRE_DYNAMIC_IO_CHANGE == 0

#warning "Static io selected"

#define ONE_WIRE_DQ_1()        { ONE_WIRE_DDR_REG &= (~(1<<ONE_WIRE_PIN)); }
#define ONE_WIRE_DQ_0()        { ONE_WIRE_DDR_REG |= (1<<ONE_WIRE_PIN); }

#define OW_PULL_HARD_DQ()      { ONE_WIRE_OUT_REG |= (1<<ONE_WIRE_PIN); \
                                 ONE_WIRE_DDR_REG |= (1<<ONE_WIRE_PIN); }

#define OW_FLOAT_DQ()          { ONE_WIRE_DDR_REG &= (~(1<<ONE_WIRE_PIN)); \
                                 ONE_WIRE_OUT_REG &= (~(1<<ONE_WIRE_PIN)); }

#define BIT_IS_SET(port,pin)   ( bit_is_set(ONE_WIRE_PIN_REG, ONE_WIRE_PIN) )
#define BIT_IS_CLEAR(port,pin) ( bit_is_clear(ONE_WIRE_PIN_REG, ONE_WIRE_PIN) )

#elif ONE_WIRE_DYNAMIC_IO_CHANGE == 1

#warning "Dynamic io change selected"

#define ONE_WIRE_DQ_1()        { *(ow_ddr_reg) &= (~(ow_pin)); }

#define ONE_WIRE_DQ_0()        { *(ow_ddr_reg) |= ow_pin; }

#define OW_PULL_HARD_DQ()      { *(ow_out_reg) |= ow_pin; *(ow_ddr_reg) |= ow_pin; }

#define OW_FLOAT_DQ()          { *(ow_ddr_reg) &= (~(ow_pin)); *(ow_out_reg) &= (~(ow_pin));  }

#define BIT_IS_SET(port,pin)   ( *(port) & pin )

#define BIT_IS_CLEAR(port,pin) ( (*(port) & pin) == 0 )

#endif /* #if ONE_WIRE_DYNAMIC_IO_CHANGE == 0 -> #elif ONE_WIRE_DYNAMIC_IO_CHANGE == 1 */






/********************************************************************************************************/
/*                                   TYPE DEFINITIONS                                                   */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   GLOBAL VARIABLES                                                   */
/********************************************************************************************************/
unsigned char          ow_system_message=0;
#if OW_SEARCH_ROM_NEEDED == 1
unsigned char          last_descrepancy;
unsigned char          done_flag;
unsigned char          rom_codes_found;
#if OW_CALCULATE_CRC8_NEEDED == 1
unsigned char          rom_code_buf[TOTAL_ROM_SIZE];
#endif
#endif   /* #if OW_SEARCH_ROM_NEEDED == 1 */

#if ONE_WIRE_DYNAMIC_IO_CHANGE == 1
volatile unsigned char*    ow_out_reg = (&(ONE_WIRE_OUT_REG));
volatile unsigned char*    ow_ddr_reg = (&(ONE_WIRE_DDR_REG));
volatile unsigned char*    ow_pin_reg = (&(ONE_WIRE_PIN_REG));
unsigned char              ow_pin=(1<<ONE_WIRE_PIN);
#endif
/********************************************************************************************************/
/*                                   LOCAL FUNCTION PROTOTYPES                                          */
/********************************************************************************************************/

static inline void delay_c(uint8_t __count) __attribute__((always_inline));
static inline void delay_i(uint16_t __count) __attribute__((always_inline));


/********************************************************************************************************/
/*                                   LOCAL FUNCTION DEFINITIONS                                         */
/********************************************************************************************************/
void  delay_i(uint16_t __count)
{
	__asm__ volatile (
		"1: sbiw %0,1" "\n\t"
		"brne 1b"
		: "=w" (__count)
		: "0" (__count)
	);
} 

void  delay_c(uint8_t __count)
{
	__asm__ volatile (
		"1: dec %0" "\n\t"
		"brne 1b"
		: "=r" (__count)
		: "0" (__count)
	);
}

/********************************************************************************************************/
/*                                   PUBLIC FUNCTION DEFINITIONS                                        */
/********************************************************************************************************/
#if ONE_WIRE_DYNAMIC_IO_CHANGE == 1
void ow_config_io(volatile unsigned char* port, unsigned char pin)
{
ow_out_reg=port;
ow_ddr_reg=(port-1);
ow_pin_reg=(port-2);
ow_pin=(1<<pin);
OW_FLOAT_DQ();
DELAY_OW_RECOVERY_TIME();

return;
}
#endif

inline unsigned char ow_get_bit(void)
{
unsigned char x=0,global_int_mem=0; 

if(SREG & 0x80) { asm("cli"); global_int_mem=1; }

ONE_WIRE_DQ_0();
DELAY_OW_RECOVERY_TIME();
ONE_WIRE_DQ_1();
DELAY_OW_READ_POS();

if( BIT_IS_SET(ow_pin_reg, ow_pin )) { x=1; }

#if OW_WILL_USE_A_TIMER == 0 
DELAY_US(ONE_WIRE_READ_SLOT_TIME_US-ONE_WIRE_READ_POSITION_US);
#elif OW_WILL_USE_A_TIMER == 1
DELAY_OW_READ_SLOT();                  
#endif

if(global_int_mem) { asm("sei"); }

return(x);
}
/*######################################################################################################*/

inline void ow_put_bit(unsigned char value)
{
unsigned char global_int_mem=0; 

if( SREG & 0x80) { asm("cli"); global_int_mem=1; }

if(value)
 {
   ONE_WIRE_DQ_0();
   DELAY_OW_RECOVERY_TIME();
   ONE_WIRE_DQ_1(); 

#if OW_WILL_USE_A_TIMER == 0
   DELAY_US(ONE_WIRE_WRITE_SLOT_TIME_US);  
#elif OW_WILL_USE_A_TIMER == 1   
   DELAY_OW_WRITE_SLOT(); 
#endif
   
 }
else{
       ONE_WIRE_DQ_0();

#if OW_WILL_USE_A_TIMER == 0
       DELAY_US(ONE_WIRE_WRITE_SLOT_TIME_US);   
#elif OW_WILL_USE_A_TIMER == 1       
       DELAY_OW_WRITE_SLOT(); 
#endif
       
       ONE_WIRE_DQ_1();
       DELAY_OW_RECOVERY_TIME();
    }

if(global_int_mem) { asm("sei"); }

return;
}
/*######################################################################################################*/
#if OW_PULL_HARD_DQ_NEEDED == 1
inline void ow_pull_hard_dq(unsigned char on_off) 
{ 
if(on_off) { OW_PULL_HARD_DQ(); } else{ OW_FLOAT_DQ(); }

return;
}
#endif
/*######################################################################################################*/

void ow_reset(void)
{
#if OW_SAVE_MORE_CODE_SPACE == 0
unsigned char x=0;
#endif
unsigned char global_int_mem=0; 

OW_FLOAT_DQ();
if(SREG & 0x80) { asm("cli"); global_int_mem=1; }

/* Perform the 1 wire reset which i must say is rather timing complicated */
ONE_WIRE_DQ_0();
#if OW_WILL_USE_A_TIMER == 0
DELAY_US(ONE_WIRE_RESET_TIME_US); 
#elif OW_WILL_USE_A_TIMER == 1
DELAY_OW_RESET();
#endif

ONE_WIRE_DQ_1();

/* Wait until the DQ line goes low with timeout */
do{
     DELAY_OW_RECOVERY_TIME();
     x++;
     if( x > (ONE_WIRE_WAIT_FOR_PRESENSE_US/ONE_WIRE_RECOVERY_TIME_US) )
      { 
          ow_system_message=OW_NO_DEVICE_PRESENT;
          return;
      } 

  }while(BIT_IS_SET(ow_pin_reg, ow_pin ));


/* Noise reduction. Wait for the required presense pulse width in order to be considered valid */

#if OW_WILL_USE_A_TIMER == 0
DELAY_US(ONE_WIRE_PRESENSE_DURATION_US); 
#elif OW_WILL_USE_A_TIMER == 1
DELAY_OW_PRESENCE_DURATION();
#endif

if( BIT_IS_CLEAR(ow_pin_reg, ow_pin ) )
 {
    ow_system_message=OW_NO_ERROR; 
 } 
else{ ow_system_message=OW_NO_DEVICE_PRESENT; return; }


/* Now check for an alarm condition */
#if OW_WILL_USE_A_TIMER == 0
DELAY_US(ONE_WIRE_ALARM_TIME_US-ONE_WIRE_PRESENSE_DURATION_US);  
#elif OW_WILL_USE_A_TIMER == 1
DELAY_OW_ALARM();
#endif

/* Check to see if the DQ line returned to high, else issue an error */
if( BIT_IS_CLEAR(ow_pin_reg, ow_pin ) )
 { 
    ow_system_message=OW_ALARM_DETECTED;
 }

 
/* And finally check for a hardware error condition */
#if OW_WILL_USE_A_TIMER == 0
DELAY_US(ONE_WIRE_TIMEOUT_US-ONE_WIRE_ALARM_TIME_US-ONE_WIRE_PRESENSE_DURATION_US); 
#elif OW_WILL_USE_A_TIMER == 1
DELAY_OW_TIMEOUT();
#endif

/* If the line is still low then something is damaged or stuck. */
if( BIT_IS_CLEAR(ow_pin_reg, ow_pin ) )
 { 
    ow_system_message=OW_BUS_ERROR;
 }



if(global_int_mem) { asm("sei"); }


return;
}
/*######################################################################################################*/

void ow_put_byte(unsigned char data)
{
unsigned char bit=0;

        /* write the byte, lsb first */
	for(bit=0; bit<=7; bit++)
	  {
              ow_put_bit(data & 0x01);
	      data = (data>>1);
	  }

return;
}
/*######################################################################################################*/

unsigned char ow_get_byte(void)
{
register unsigned char bit=0;
unsigned char byte=0;


        /* read the byte, lsb first */
	for(bit=0; bit<=7; bit++)
	  {
             if(ow_get_bit()) { byte|=(1<<bit); }
      	  }

return(byte);
}
/*######################################################################################################*/
#if OW_READ_ROM_NEEDED == 1
void ow_read_rom(unsigned char *mem_array)
{

#if OW_CALCULATE_CRC8_NEEDED == 1

unsigned char x=0, error_counter=0;

/* Clear the destination array */
for(x=0; x<TOTAL_ROM_SIZE; x++) { *(mem_array+x)=0; } 

do{
     /* Clear the rom code buffer array */
     for(x=0; x<TOTAL_ROM_SIZE; x++) { *(rom_code_buf+x)=0; } 
     error_counter++;
     if(error_counter>OW_CRC8_ERROR_RETRIES)
      {
         ow_system_message=OW_CRC_ERROR;
         return;
      }
     ow_reset();
     ow_command(OW_READ_ROM);
     for(x=0; x<TOTAL_ROM_SIZE; x++) { *(rom_code_buf+x)=ow_get_byte(); }

  }while( calculate_crc8(rom_code_buf, ROM_CODE_BYTES) != *(rom_code_buf+ROM_CODE_BYTES) );

/* Since we know now that the rom code is correct, copy the contents of the buffer to the destination */
for(x=0; x<TOTAL_ROM_SIZE; x++) { *(mem_array+x)=(*(rom_code_buf+x)); }  
ow_reset();

#elif OW_CALCULATE_CRC8_NEEDED == 0

unsigned char x=0;

     ow_reset();
     ow_command(OW_READ_ROM);
     for(x=0; x<TOTAL_ROM_SIZE; x++) { *(mem_array+x)=ow_get_byte(); }
     ow_reset();

#endif

/* If we reached here everything went ok. */
ow_system_message=OW_NO_ERROR;

return;
}
#endif   /* #if OW_READ_ROM_NEEDED == 1 */
/*######################################################################################################*/

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if OW_SEARCH_ROM_NEEDED == 1
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if OW_CALCULATE_CRC8_NEEDED == 1
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

unsigned char ow_search_rom(unsigned char *mem_array, unsigned char command)
{
unsigned char a=0, b=0, bit=0, descrepancy_marker=0, rom_bit_index=0, bit_pos=0, byte_pos=0, bit_mask=0;
unsigned char error_counter=0;

ow_reset();
if(ow_get_msg()==OW_NO_DEVICE_PRESENT || ow_get_msg()==OW_BUS_ERROR || command == OW_RESET_SEARCH_ROM)
 {
    last_descrepancy=0;
    done_flag=0;
    rom_codes_found=0;
    /* Clear the code containing arrays */
    for(a=0; a<TOTAL_ROM_SIZE; a++) { *(mem_array+a)=0; *(rom_code_buf+a)=0; }
    ow_system_message=OW_SEARCH_ROM_IS_RESET;
    return(rom_codes_found);
 } 

if(done_flag) { ow_system_message=OW_SEARCH_ROM_FINISHED; return(rom_codes_found); }

/*
   The main do loop is executed as many times it is needed to get a valid crc8 result. 
   That means that if the crc8 calculation matches the 8th byte of the rom code the function 
   returns the rom code found, else it repeats the loop as many times it is needed to get
   the correct and error free rom code.  
*/
do{

/*  Copy the contents of the mem_array to the rom_code_buf. After a search rom reset the array 
    contains 0's but after each succesfull rom search it contains the previusly found rom code. 
    The copying code is executed always after the 1st CRC8 error detected. */ 
    if(error_counter) { for(a=0; a<TOTAL_ROM_SIZE; a++){ *(rom_code_buf+a)=*(mem_array+a); } }

    error_counter++;
    if(error_counter>OW_CRC8_ERROR_RETRIES)
     {
       ow_search_rom(mem_array, OW_RESET_SEARCH_ROM); 
       ow_system_message=OW_CRC_ERROR;
       return(rom_codes_found);
     }
    /* Initialization of non static variables */
    rom_bit_index=1;
    descrepancy_marker=0;
    bit_pos=0;
    byte_pos=0;
    /* Do one wire reset and give the search_rom or alarm_search command */
    ow_reset(); 
    ow_command(command);
    /* Start a loop to get all 64 bits of a rom code */
    while(1)
        {
          bit_mask=(1<<bit_pos);
          a=ow_get_bit();
          b=ow_get_bit();

          if(a==b && a==1) 
           {
             last_descrepancy=0;
       	     rom_codes_found=0;
             ow_system_message=OW_SEARCH_ROM_FINISHED;
             return(rom_codes_found);
           }
          else if(a==b && a==0)
                {
                  if(rom_bit_index==last_descrepancy)
                   { 
                     *(rom_code_buf+byte_pos) |= bit_mask; 
                     ow_put_bit(1);
                     rom_bit_index++;
                   }
                  else{
                        if(rom_bit_index > last_descrepancy)
                         {
                           *(rom_code_buf+byte_pos) &= (~bit_mask); 
                           descrepancy_marker=rom_bit_index;
                           ow_put_bit(0);
                           rom_bit_index++;
                         }    
                        else{
                              bit=( *(rom_code_buf+byte_pos) & bit_mask ); 
                              if(bit == 0)          
                               {
                                 descrepancy_marker=rom_bit_index;
                               }
                              ow_put_bit(bit);
                              rom_bit_index++; 
                            }
                      }    
            
                }
               else{
                     if(a) { *(rom_code_buf+byte_pos) |= bit_mask;    }
                     else  { *(rom_code_buf+byte_pos) &= (~bit_mask); } 	
                     ow_put_bit(a);
                     rom_bit_index++;
                   }    
          bit_pos++;
          if(bit_pos == 8) { bit_pos=0; byte_pos++; }
          if(rom_bit_index>(TOTAL_ROM_SIZE*8)) { break; } /* If all bits have be found exit */

        }  /* end of whle(1) loop */

  /* out of the while(1) loop code so lets check crc8 and repeat if needed */
  }while( calculate_crc8(rom_code_buf, ROM_CODE_BYTES) != *(rom_code_buf+ROM_CODE_BYTES) );

/* Since we know now that the rom code is correct, copy the contents of the buffer to the destination */
for(a=0; a<TOTAL_ROM_SIZE; a++) { *(mem_array+a)=*(rom_code_buf+a); }

/* out of crc8 checking loop so let's set the variables for the next call of this function */
last_descrepancy=descrepancy_marker;
rom_codes_found++;

/* If last_descrepancy == 0 means that all rom codes have been found, else the rom search is not finished */
if(last_descrepancy == 0)
 { 
   done_flag=1;
   ow_system_message=OW_SEARCH_ROM_FINISHED;
 }
else{ ow_system_message=OW_SEARCH_ROM_ACTIVE; } 



return(rom_codes_found); 
}

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif OW_CALCULATE_CRC8_NEEDED == 0
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

unsigned char ow_search_rom(unsigned char *mem_array, unsigned char command)
{
unsigned char a=0, b=0, bit=0, descrepancy_marker=0, rom_bit_index=0, bit_pos=0, byte_pos=0, bit_mask=0;


ow_reset();
if(ow_get_msg()==OW_NO_DEVICE_PRESENT || ow_get_msg()==OW_BUS_ERROR || command == OW_RESET_SEARCH_ROM)
 {
    last_descrepancy=0;
    done_flag=0;
    rom_codes_found=0;
    /* Clear the code containing array */
    for(a=0; a<TOTAL_ROM_SIZE; a++) { *(mem_array+a)=0; }
    ow_system_message=OW_SEARCH_ROM_IS_RESET;
    return(rom_codes_found);
 } 

if(done_flag) { ow_system_message=OW_SEARCH_ROM_FINISHED; return(rom_codes_found); }

/* Initialization of non static variables */
rom_bit_index=1;
descrepancy_marker=0;

ow_command(command);

    while(1)
        {
          bit_mask=(1<<bit_pos);
          a=ow_get_bit();
          b=ow_get_bit();

          if(a==b && a==1) 
           {
             last_descrepancy=0;
       	     rom_codes_found=0;
             ow_system_message=OW_SEARCH_ROM_FINISHED;
             return(rom_codes_found);
           }
          else if(a==b && a==0)
                {
                  if(rom_bit_index==last_descrepancy)
                   { 
                     *(mem_array+byte_pos) |= bit_mask; 
                     ow_put_bit(1);
                     rom_bit_index++;
                   }
                  else{
                        if(rom_bit_index > last_descrepancy)
                         {
                           *(mem_array+byte_pos) &= (~bit_mask); 
                           descrepancy_marker=rom_bit_index;
                           ow_put_bit(0);
                           rom_bit_index++;
                         }    
                        else{
                              bit=( *(mem_array+byte_pos) & bit_mask ); 
                              if(bit == 0)          
                               {
                                 descrepancy_marker=rom_bit_index;
                               }
                              ow_put_bit(bit);
                              rom_bit_index++; 
                            }
                      }    
            
                }
               else{
                     if(a) { *(mem_array+byte_pos) |= bit_mask;    }
                     else  { *(mem_array+byte_pos) &= (~bit_mask); } 	
                     ow_put_bit(a);
                     rom_bit_index++;
                   }    
          bit_pos++;
          if(bit_pos == 8) { bit_pos=0; byte_pos++; }
          if(rom_bit_index>(TOTAL_ROM_SIZE*8)) { break; } /* If all bits have be found exit */

        }  /* end of whle(1) loop */


/* out of the while(1) loop code that sets the variables for the next call of this function */
last_descrepancy=descrepancy_marker;
rom_codes_found++;

/* If last_descrepancy == 0 means that all rom codes have been found, else the rom search is not finished */
if(last_descrepancy == 0)
 { 
   done_flag=1;
   ow_system_message=OW_SEARCH_ROM_FINISHED;
 }
else{ ow_system_message=OW_SEARCH_ROM_ACTIVE; } 



return(rom_codes_found); 
}

/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif   /* #if OW_CALCULATE_CRC8_NEEDED == 0 -> #elif OW_CALCULATE_CRC8_NEEDED == 0 */
/*222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif   /* #if OW_SEARCH_ROM_NEEDED == 1 */
/*111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/*######################################################################################################*/

#if OW_CALCULATE_CRC8_NEEDED == 1
unsigned char calculate_crc8(unsigned char* data_pointer, unsigned char number_of_bytes)
{
unsigned char temp1=0, bit_counter=0, feedback_bit=0, crc8_result=0;

 while(number_of_bytes--)
     {
        temp1=(*(data_pointer++));
 
        for(bit_counter=8; bit_counter; bit_counter--)
          {
             feedback_bit=(crc8_result & 0x01);
             crc8_result=(crc8_result >> 1);
             if(feedback_bit ^ (temp1 & 0x01))
              { 
                 crc8_result=(crc8_result ^ 0x8c);
              }
             temp1=(temp1 >> 1);
          }

     }     

return(crc8_result);
}
#endif   /* #if OW_CRC8_FUNCTION_NEEDED == 1 */
/*######################################################################################################*/

unsigned char ow_get_msg(void)
{

return(ow_system_message);
}




/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/


