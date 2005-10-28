//#include <ina90.h>
#include <90s8515.h>

void low_level_init(void)
{
        
        UBRR=0x2f;          // 19200bps @ 4 MHz
    //UCR  = 0x08;        // RX enable, TX enable
    GIMSK= 0x40;        // Enable INT0 interrupt
    //UCR=0x58; 
    UCR=0xD8; 

#asm("sei")
    //return 1;
}

