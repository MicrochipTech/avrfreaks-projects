/****************************************************************************
 Title  :   C Test program for the Software UART FUNCTIONS library (test_uart.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      10/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  Based on application note AVR 305 
            This software is FREE.

*****************************************************************************/

#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION       330        /* AVRGCC version for including the correct files  */
#endif
 
#if   AVRGCC_VERSION == 330
#include <avr/io.h>
#elif AVRGCC_VERSION == 320
#include <io.h>
#else
#include <io-avr.h>
#endif

#include "suart.h"




void main(void)
{

unsigned char buffer[20];
unsigned char string[]="\r\nSoftware UART testing...";
static unsigned char eeprom_string[]__attribute__((section(".eeprom")))="suart_0_puts_e Testing...";


/* Initialize the software uart to 115200 bps and enable receiving. */
   
#if SUART_0_DYNAMIC_BAUD_CHANGE == 1  

unsigned long baudrate=115200;
unsigned char error=0;

/* If there is an error go to a lower BAUDRATE */
while( (error=suart_0_init(baudrate)) == SUART_0_BAUDRATE_TOO_HIGH )
    {
      baudrate/=2;     
      if(baudrate==28800) { baudrate=38400; }
    } 

#elif SUART_0_DYNAMIC_BAUD_CHANGE == 0

suart_0_init();

#endif

#if SUART_0_DYNAMIC_IO_SETTINGS == 1
suart_0_config_io(PORTE,0,1);
#endif

while(1)
    {

      suart_0_puts(string);
      suart_0_puts_P("\r\n");
/*********************************************************************************************************/
      suart_0_puts_P("Type something + ENTER");
/*
   In interrupt mode check the "suart_0_string_received" var for an indication that a string is received
   or x chars are received. 
   In non interrupt mode checking is not needed as the suart_0_gets(x) does not return until a string is
   received or x chars are received or timeout occured (If enabled). If a timeout occured,
   the "suart_0_string_received" and "suart_0_rx_count" vars will have a value of 255 (0xFF)
   if nothing was received prior to timeout or a number representing the received chars
   before the timeout occured.
   In suart_0_gets(x) the argument passed is the maximum chars wanted. If x=255 then there is no limit.
   IN INTERRUPT MODE THERE IS NO TIMEOUT. IT IS NOT NEEDED ANYWAY.
*/
      suart_0_gets(SUART_0_NO_RX_LIMIT);                                  

#if SUART_0_WILL_USE_INTERRUPT == 1 
      while(suart_0_string_received == 0);
#endif

#if   SUART_0_TIMEOUT_ENABLED == 1
      if(suart_0_timeout_flag) { suart_0_puts_P(" TIMEOUT!\r\n"); }
#endif
      suart_0_puts_P("\r\n");
      suart_0_puts_P("sizeof(received_string)= ");
      suart_0_puti(suart_0_string_received, 0);
      suart_0_puts_P("\r\n1) ACTUAL BUFFER CONTENTS: ");
      suart_0_puts((unsigned char*)suart_0_rx_buffer);
      suart_0_puts_P("\r\n2) RETRIEVED BUFFER DATA: ");
/*
      "suart_0_get_buffer(buffer);" gets as may as the destination string can hold.
      After the buffer has been read the used chars are removed from the suart_0_rx_buffer
      and their space is reclaimed.  You can also get the actual suart_0_rx_buffer as demonstrated. 
      This way the suart_0_rx_buffer is left intact. 
*/ 
      suart_0_get_buf(buffer, suart_0_string_received);
      suart_0_puts(buffer);
      suart_0_puts_P("\r\n");

/*********************************************************************************************************/

      suart_0_puts_P("Type at least 10 chars");
      suart_0_gets(10);                                 
#if SUART_0_WILL_USE_INTERRUPT == 1 
      while(suart_0_string_received == 0);
#endif

#if   SUART_0_TIMEOUT_ENABLED == 1
      if(suart_0_timeout_flag) { suart_0_puts_P(" TIMEOUT!\r\n"); }
#endif
      suart_0_puts_P("\r\n");
      suart_0_puts_P("sizeof(received_string)= ");
      suart_0_puti(suart_0_string_received, 0);
      suart_0_puts_P("\r\n1) ACTUAL BUFFER CONTENTS: ");
      suart_0_puts((unsigned char*)suart_0_rx_buffer);
      suart_0_puts_P("\r\n2) RETRIEVED BUFFER DATA: ");
      suart_0_get_buf(buffer, suart_0_rx_count);
      suart_0_puts(buffer);
      suart_0_puts_P("\r\n");

/*********************************************************************************************************/      
/*
   In suart_0_raw_rx(x) the argument passed is the maximum chars wanted. If x=0 then there is no limit.
   In interrupt mode suart_0_raw_rx(x) receives chars of any kind until x chars are received
   or you disable receiving using suart_0_rx_off(). 
   In non interrupt mode suart_0_raw_rx(x) does not return until x chars of any kind are received
   or timeout occurs if enabled. 
   MAKE SURE THAT TIMEOUT IS ENABLED IF YOU USE THE suart_0_raw_rx(x) FUNCTION IN THE NON INTERRUPT MODE
   OTHERWISE YOU WILL BE WAITING FOREVER.
*/
      suart_0_puts_P("Type at least 5 chars");
      suart_0_raw_rx(5);
#if SUART_0_WILL_USE_INTERRUPT == 1 
      while(suart_0_rx_count < 5); 
#endif
#if   SUART_0_TIMEOUT_ENABLED == 1
      if(suart_0_timeout_flag) { suart_0_puts_P(" TIMEOUT!\r\n"); }
#endif
      suart_0_get_buf(buffer, suart_0_rx_count);        /* Read the buffer */
      suart_0_puts_P("\r\n");
      suart_0_puts(buffer);                           /* After the buffer has been read the chars are */
      suart_0_puts_P("\r\n");                         /* no longer in the buffer and more space is usable */

/*********************************************************************************************************/
      
      suart_0_puts_P("Type 5 more chars");
      suart_0_raw_rx(SUART_0_NO_RX_LIMIT);              /* ENABLE RAW DATA RECEIVING. (All chars are accepted) */
#if SUART_0_WILL_USE_INTERRUPT == 1 
      while(suart_0_rx_count < 5); 
#endif
#if   SUART_0_TIMEOUT_ENABLED == 1
      if(suart_0_timeout_flag) { suart_0_puts_P(" TIMEOUT!\r\n"); }
#endif
      suart_0_get_buf(buffer, suart_0_rx_count);        /* Read the buffer */
      suart_0_puts_P("\r\n");
      suart_0_puts(buffer);                           /* After the buffer has been read the chars are */
      suart_0_puts_P("\r\n");                         /* no longer in the buffer and more space is usable */

/*********************************************************************************************************/

      suart_0_puts_e(eeprom_string);
      suart_0_puts_P("\r\nsuart_0_puti Testing...   ");
      suart_0_puti(1963,1);
      suart_0_putc('\r');
      suart_0_putc('\n');
      


      suart_0_puts_P("Press ENTER to repeat");
      while(suart_0_getc()!='\r');
#if   SUART_0_TIMEOUT_ENABLED == 1
      if(suart_0_timeout_flag) { suart_0_puts_P(" TIMEOUT!\r\n"); }
#endif
      suart_0_putc('\r');
      suart_0_putc('\n');
      suart_0_puts_P("\r\n");

#if SUART_0_WILL_USE_INTERRUPT == 1
      suart_0_rx_off();
#endif
      
    }
        
}
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/


