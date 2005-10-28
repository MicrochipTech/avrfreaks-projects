//#include <stdio.h>
//#include <pgmspace.h>
#include <90s8515.h>            /* SFR declarations */

#include "serial.h"

//#define ESC 0x1b
#define BUFF_SIZE 64


//flash char CLR[8] = {ESC, '[','H', ESC, '[', '2', 'J'};

unsigned char UART_buffer[BUFF_SIZE];
unsigned char *inptr, *outptr;
unsigned char buff_cnt;


void init_uart(void)
{
    inptr =  UART_buffer;
    outptr = UART_buffer;
    buff_cnt = 0;
}

//void clr(void)
//{
//   printf("%s",CLR);                                // Send a 'clear screen' to a VT100 terminal
//}

int putchar(int c)
{
    if (buff_cnt<BUFF_SIZE)
   {
       *inptr = c;                             // Put character into buffer
        inptr++;                                // Increment pointer
        
       buff_cnt++;
        
       if (inptr >= UART_buffer + BUFF_SIZE)   // Pointer wrapping
            inptr = UART_buffer;
 
        UCR = 0x28;                             // Enable UART Data register
                                                // empty interrupt
        
        return 1;
    } else {
       return 0;                               // Buffer is full
    }
        
//}

// Interrupt driven transmitter

interrupt [UART_TXC] void uart_tx_isr(void)
{
    UDR = *outptr;                              // Send next byte
    outptr++;                                   // Increment pointer
    
    if (outptr >= UART_buffer + BUFF_SIZE)      // Pointer wrapping
        outptr = UART_buffer;

    if(--buff_cnt == 0)                         // If buffer is empty:
        UCR = UCR && (1<<UDRIE);                //    disabled interrupt
}
    
