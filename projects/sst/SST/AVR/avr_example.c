/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "example.h"
#include "main_test.h"


#define T0DIV       256UL
#define T0CLKDIV    _BV(CS02)
// 10ms
#define TIMERFREQ   100UL
#define T0_INIT     (256-(((F_CPU+(TIMERFREQ/2))/TIMERFREQ)/T0DIV))


#define T1DIV       64UL
#define TIMER1FREQ  10UL

#define T1_INIT     ((uint16_t) (((F_CPU + (TIMER1FREQ * T1DIV)) / (2UL * TIMER1FREQ * T1DIV)) - 1UL))

static bool is_uart_ready = true;


void UART_put(uint8_t byte)
{
    UDR = byte;
    UCR |= _BV(UDRIE);
    is_uart_ready = false;
}

extern bool Is_UART_Ready(void)
{
    return is_uart_ready;
}

ISR(SIG_OVERFLOW0)
{
    SST_DECLARE_ISR;

    SST_ISR_ENTRY(SST_MIN_INT_PRIO+1);

    SST_TimerTick(&ticker_10ms);
    SST_ScheduleTask(btn_task_id);

    SST_ISR_EXIT();

    TCNT0 += T0_INIT;
}


ISR(SIG_OUTPUT_COMPARE1A)
{
    SST_DECLARE_ISR;

    SST_ISR_ENTRY(SST_MIN_INT_PRIO+1);

    SST_TimerTick(&ticker_100ms);

    SST_ISR_EXIT();
}


ISR(SIG_UART_RECV)
{
    SST_DECLARE_ISR;
    uint8_t byte;

    byte = UDR;

    SST_ISR_ENTRY(SST_MIN_INT_PRIO+3);

    if (0 == (USR & _BV(FE)))
    {
        SST_PostEvent(interp_task_id, 0, byte);
    }

    SST_ISR_EXIT();
}


ISR(SIG_UART_DATA)
{
    if (IS_BUF_EMPTY(tx_buf))
    {
        UCR &= ~_BV(UDRIE);
        is_uart_ready = true;
    }
    else
    {
        UDR = get_buf(&tx_buf);
        is_uart_ready = false;
    }
}


/*******************************************************************************/
void ioinit(void)
{
    set_sleep_mode(SLEEP_MODE_IDLE);

    TCNT0 = T0_INIT;
    // set CK/TDIV prescaler for timer 0
    TCCR0 = T0CLKDIV;

    TIMSK = _BV(TOIE0);

    TCCR1A = 0;
    TCCR1B = _BV(CTC1);
    OCR1A = T1_INIT;
    TIMER100MS_ON();

    // output & low
    LED1_PORTD |= _BV(LED1_BIT);
    LED1_PORT &= ~_BV(LED1_BIT);

    // output & low
    LED2_PORTD |= _BV(LED2_BIT);
    LED2_PORT &= ~_BV(LED2_BIT);

    // input & pullup
    SW1_PORTD &= ~_BV(SW1_BIT);
    SW1_PORT |= _BV(SW1_BIT);

	// set 19200 baud rate for 4Mhz CPU clock
    UBRR = 12;
    UCR = _BV(RXCIE) | _BV(TXEN) | _BV(RXEN);
}


void SST_OnIdle(void)
{
    sleep_enable();
    sleep_cpu();
    //sleep_disable();
}
