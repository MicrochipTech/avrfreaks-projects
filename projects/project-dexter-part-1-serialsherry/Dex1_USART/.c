#include <stdio.h>

#include "FE_Lib_USART_100.h"

 

int main(void) {

    // Variables

        char c;   

 

    // Initialize Flying Electron USART Library

    // for operating at 9600bps @ 8Mhz

        USART_Init(115200, 3.6864);

 

    // Transmit welcome message out serial port

        printf("Flying Electron USART Library Ready!\n");

 

    // Loop forever

        while (1) {

            // Wait for a character to be received

                while (USART_DataReady() == false) {

                    // Do nothing 

                }

 

            // Read the character from the serial port

                c = getchar();

 

            // Transmit the character that was just received

            // back to the PC

                putchar(c);

        }

}
