/**
 * \file
 *
 * \brief Blinky light demo
 *
 */

/**
 * \mainpage
 *
 * This example uses a breadboarded LED to indicate that the program is alive.
 */
#include "pmic.h"
#include "user_board.h"
#include "sysclk.h"
#include "sleepmgr.h"
#include "rtc.h"



/**
 * \brief RTC interrupt-driven blinker.
 */
static void blink(uint32_t time)
{
  rtc_set_alarm(time);

  ioport_toggle_pin(LED0_GPIO);
}


/**
 * \brief Main function.
 */
int main(void)
{
    pmic_init();
    board_init();
    sysclk_init();
    sleepmgr_init();
    rtc_init();

    rtc_set_callback(blink);
    cpu_irq_enable();
    rtc_set_alarm_relative(0);

    for (;;) {
        /* Go to sleep, everything is handled by interrupts. */
        sleepmgr_enter_sleep();
    }
}
