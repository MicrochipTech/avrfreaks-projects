#include "larslib.h"
#include "spi.h"
#include "uart.h"
#include "ps2.h"
#include "servo.h"

int main()
{
    SPI_init();
    uart_init();
    ps2_init();
    sei();

    while(1)
    {
        _delay_ms(250);
        ps2_poll(ps2.pressure[5],PS2_MOTOR_OFF);
        uart_putc(ps2.pressure[5]);
    }

    return 0;
}
