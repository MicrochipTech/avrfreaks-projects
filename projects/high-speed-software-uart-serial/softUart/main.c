#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"



#define F_CPU 20000000UL	/* CPU clock in Hertz */



volatile char data[32] = {
0xAA, 
0xC5,
0x00,
0x1C,
0xFD,
0x60,
0x00,
0x00,
0xAA,
0xCC,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x38,
0xAB,
0x81,
0x9A,
0x00,
0x0F,
0x68,
0x00,
0x0D,
0xA4,
0xA7,
0xB0,
0x3B,
0xAA,
0xCA
};







int main(void)
{

uartInit();         // Init the Uart
uint8_t i = 0;      // Counter Varible

for(;;){
sendByte(data[i]);  // Send byte of Data
i++;                // Add one to are Counter

if(i == 32){        // End of Data
i = 0;              // Reset Counter
_delay_ms(20);      // Delay a bit before sending another block of data
}


}

return 0;
}


