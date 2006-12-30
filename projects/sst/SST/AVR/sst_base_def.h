#ifndef SST_BASE_DEF_H_
#define SST_BASE_DEF_H_

/**************************************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *************************************************************************************************/

#include <avr/interrupt.h>


#define SST_MAX_NO_OF_TASKS     5
#define SST_MAX_EV_QUEUE_LEN    0
#define SST_MAX_TASK_PRIO       100
#define SST_USE_EOI             0

#define SST_USE_SIGNAL_MASK     1
#define SST_USE_LAZY_TIMER      1


typedef uint8_t CPU_Base_T;
typedef uint8_t CPU_SR_T;

#if 0
static inline CPU_SR_T SST_disable_ints(void)
{
    CPU_SR_T old;
    old = SREG & _BV(SREG_I);
    cli();
    return old;
}

static inline CPU_SR_T SST_enable_ints(void)
{
    CPU_SR_T old;
    old = SREG & _BV(SREG_I);
    sei();
    return old;
}
#endif

#define SST_Start_Nested_Ints() sei()
#define SST_Stop_Nested_Ints()  do { } while (0)

#define SST_disable_ints_save(_old_)    \
    do {                                \
        (_old_) = SREG & _BV(SREG_I);   \
        cli();                          \
    } while (0)

#define SST_disable_ints()  cli()

#define SST_enable_ints()   sei()

#define SST_restore_ints(_old_)         \
    do {                                \
        SREG |= (_old_);                \
    } while(0)

#endif /*SST_BASE_DEF_H_*/
