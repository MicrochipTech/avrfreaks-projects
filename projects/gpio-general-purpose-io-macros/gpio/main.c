#include <avr/io.h>
#include "gpio.h"

//         S         P    M    L
//         Y         O    S    S
//         M         R    B    B
//         B         T
//         O
//         L
//-------------------------------
#define    LED_cfg   B,   5,   5
#define    SEL_cfg   A,   4,   2
#define    ID_cfg    D,   7,   4

int main(void)
{
    volatile unsigned char dummy;
    //
    //  The simple case -
    //  a single output bit.
    //  E.g. using LED_cfg.
    //
    gpio_conf( LED_cfg, OUTPUT, 0 );
    gpio_wr( PORT, LED_cfg, 1 );
    gpio_set( LED_cfg );
    gpio_clr( LED_cfg );
    gpio_toggle( LED_cfg );

    //
    //  A more complex case -
    //  Multiple output bits.
    //  E.g. using SEL_cfg.
    //
    gpio_conf( SEL_cfg, OUTPUT, 0x05 );
    gpio_wr( PORT, SEL_cfg, 0x07 );
    gpio_set( SEL_cfg );
    gpio_clr( SEL_cfg );
    gpio_toggle( SEL_cfg );

    //
    //  The complex case -
    //  mutliple input bits.
    //  E.g. using gpio ID_cfg.
    //
    gpio_conf( ID_cfg, INPUT, NOPULLUP );
    gpio_conf( ID_cfg, INPUT, PULLUP );
    if( gpio_rd( PIN, ID_cfg ) == 0x04 )
        dummy = 0x01;
    if( gpio_rd8( PIN, ID_cfg ) == 0 )
        dummy = 0x02;
    if( gpio_tst( ID_cfg ) )
        dummy = 0x03;
}
// -------------- end example --------------------------------
