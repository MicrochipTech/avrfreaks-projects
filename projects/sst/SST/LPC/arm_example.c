/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

#include "example.h"
#include "main_test.h"

#define F_CPU           60000000UL
#define PRESCALER_FREQ  1000UL
#define TIMER_FREQ      100UL

#define PRESCALER_VAL   (((F_CPU + (PRESCALER_FREQ/2UL)) / PRESCALER_FREQ) - 1UL)
#define TIMER_MATCH     ((PRESCALER_FREQ + (TIMER_FREQ/2UL))/TIMER_FREQ)

#define TIMER100MS_FREQ      10UL
#define TIMER100MS_MATCH     ((PRESCALER_FREQ + (TIMER_FREQ/2UL))/TIMER_FREQ)

#define UART_SPEED      19200UL

#define UART_DIV        ((F_CPU + ((UART_SPEED * 16UL)/2UL)) / (UART_SPEED * 16UL))
#define UART_DIV_LSB    (UART_DIV & 0xFF)
#define UART_DIV_MSB    ((UART_DIV >> 8) & 0xFF)


#define TIMER0_INT  4
#define TIMER1_INT  5
#define UART0_INT   6

static bool is_uart_ready = true;

void T0_MatchC0_ISR (void)   __attribute__ ((interrupt("IRQ")));
void T1_MatchC0_ISR (void)   __attribute__ ((interrupt("IRQ")));
void UART0_ISR (void)   __attribute__ ((interrupt("IRQ")));


void UART_put(uint8_t byte)
{
    UART0_THR = byte;
    is_uart_ready = false;
}

bool Is_UART_Ready(void)
{
    return is_uart_ready /*(UART0_LSR & (1 << 6))*/;
}

void T0_MatchC0_ISR (void)
{
    SST_DECLARE_ISR;
    T0_IR = 0x1;

    SST_ISR_ENTRY(SST_MIN_INT_PRIO+1);

    SST_TimerTick(&ticker_100ms);

    SST_ISR_EXIT();
}


void T1_MatchC0_ISR (void)
{
    SST_DECLARE_ISR;
    T1_IR = 0x1;

    SST_ISR_ENTRY(SST_MIN_INT_PRIO+1);

    SST_TimerTick(&ticker_10ms);
    SST_ScheduleTask(btn_task_id);

    SST_ISR_EXIT();
}


void UART0_ISR (void)
{
    SST_DECLARE_ISR;
    uint32_t byte;

    switch (UART0_IIR & 0x0E)
    {
        // RLS
        case 0x06:
            byte = UART0_LSR;
            LED_TOGGLE(LED7);
            break;
        // RX
        case 0x04:
        // CTI
        case 0x0C:
            byte = UART0_RBR;
            LED_TOGGLE(LED6);
            SST_ISR_ENTRY(SST_MIN_INT_PRIO+3);
            if (0 == (UART0_LSR & (1 << 7)))
            {
                //SST_PostEvent(rx_task_id, 0, byte);
                SST_PostEvent(interp_task_id, 0, byte);
            }
            SST_ISR_EXIT();
            break;

        // TX
        case 0x02:
            LED_TOGGLE(LED5);
            if (IS_BUF_EMPTY(tx_buf))
            {
                is_uart_ready = true;
            }
            else
            {
                UART0_THR = get_buf(&tx_buf);
                is_uart_ready = false;
            }
            break;
        default:
            break;
    }

    VICVectAddr = 0xff;
}


void SST_OnIdle(void)
{
    // idle mode
    SCB_PCON = 1;
}


void ioinit(void)
{
    // pclk == cclk
    SCB_VPBDIV = 0x1;

    // power on only TIMER1 and UART0
    SCB_PCONP = (1 << 3) | (1 << 2);

    GPIO0_IODIR = 0;
    GPIO1_IODIR = LED7 | LED6 | LED5 | LED4 | LED3 | LED2 | LED1 | LED0;
    GPIO1_IOCLR = LED7 | LED6 | LED5 | LED4 | LED3 | LED2 | LED1 | LED0;

    // Initialize Timer 0
    T0_TCR = 0x0;
    T0_TC = 0x0;
    T0_PR = PRESCALER_VAL;
    T0_PC = 0x0;
    T0_MR0 = TIMER100MS_MATCH;
    // Reset and interrupt on match
    T0_MCR = 0x3;

    // Initialize Timer 1
    T1_TCR = 0x0;
    T1_TC = 0x0;
    T1_PR = PRESCALER_VAL;
    T1_PC = 0x0;
    T1_MR0 = TIMER_MATCH;
    // Reset and interrupt on match
    T1_MCR = 0x3;

    // enable pins for UART0
    PCB_PINSEL0 = (1 << 2) | (1 << 0);
    //PCB_PINSEL0 = (1 << 18) | (1 << 16);
    // clear RX and TX FIFOs, end enable them at level 0 (one byte)
    UART0_FCR = (1 << 2) | (1 << 1) | (1 << 0);
    // enable divisor latches access, 8 bits
    UART0_LCR = (1 << 7) | 3;
    // set UART speed
    UART0_DLL = UART_DIV_LSB;
    UART0_DLM = UART_DIV_MSB;
    // disable divisor latches access, 8 bits
    UART0_LCR = 3;
    // enable RX interrupt
    UART0_IER = (1 << 2) | (1 << 1) | (1 << 0);
    //UART0_IER = 0;
    // Initialize VIC
    // Preliminary setup of the VIC. Assign all interrupt chanels to IRQ
    VICIntSelect = 0;
    // Disable all interrupts
    VICIntEnClear = 0xFFFFFFFF;
    // Clear all software interrutps
    VICSoftIntClear = 0xFFFFFFFF;
    // VIC registers can be accessed in User or privileged mode
    VICProtection = 0;
    // Clear interrupt
    VICVectAddr = 0;
    // Clear address of the default ISR
    VICDefVectAddr = 0;

    // Timer 1 and UART0 interrupts enabled
    VICIntEnable = (1 << UART0_INT) | (1 << TIMER1_INT) | (1 << TIMER0_INT);
    VICVectCntl0 = (1 << 5) | TIMER0_INT;
    VICVectCntl1 = (1 << 5) | TIMER1_INT;
    VICVectCntl2 = (1 << 5) | UART0_INT;
    // Address of the ISR
    VICVectAddr0 = (uint32_t)T0_MatchC0_ISR;
    VICVectAddr1 = (uint32_t)T1_MatchC0_ISR;
    VICVectAddr2 = (uint32_t)UART0_ISR;
    // Start timer
    T1_TCR = 0x1;
}

