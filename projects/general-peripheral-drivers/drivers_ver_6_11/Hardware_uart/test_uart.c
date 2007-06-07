
/****************************************************************************
 Title  :   C Test program  for the UART FUNCTIONS library (testuart.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      30/Aug/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/

#if !defined(AVRGCC_VERSION)
#if ((__GNUC__ * 100) + __GNUC_MINOR__) > 303
#define AVRGCC_VERSION      304        /* AVRGCC version for including the correct files  */
#elif ((__GNUC__ * 100) + __GNUC_MINOR__) >= 300  &&  ((__GNUC__ * 100) + __GNUC_MINOR__) <= 303
#define AVRGCC_VERSION      303        /* AVRGCC version for including the correct files  */
#else 
#define AVRGCC_VERSION   300    
#endif
#endif

#if AVRGCC_VERSION >= 303

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#else

#include <io.h>
#include <eeprom.h>
#include <pgmspace.h>

#endif

#include "uart.h"


int main(void)
{
static unsigned char eeprom_string[]__attribute__((section(".eeprom")))={"uart_puts_e testing..."};
static unsigned char flash_string[]__attribute__((progmem))="uart_puts_p testing...\n\r";

unsigned char        rx_buffer_contents[21];
int                  number=0;
unsigned char        string0[]="uart_putc testing...\n\r";
unsigned char        *ptr="uart_puts_e testing...";
unsigned char        in_char='\0';
unsigned char        c='\0';

#if    UART_DYNAMIC_BAUD_CHANGE == 1
/* 
    When UART_DYNAMIC_BAUD_CHANGE == 1, uart_init() takes the Baudrate and a command as parameters.
    Also note that if you use 2 UARTS, ONLY THE SELECTED UART WILL BE INITIALIZED!
    If you use 2 UARTS you should intialize the second UART also by another uart_init() command.
    You can select the UART to be initialized by giving a separate uart_command(SELECT_UARTx)
    prior to the uart_init() command, or within the same uart_init() statement.
*/
#if UART_MULTI_COMMAND_SUPPORT == 1        

/* UART 0 WILL BE INITIALIZED ONLY ! */ 
uart_init(115200, SELECT_UART0 | UART_RX_OFF | UART_TX_OFF | UART_ECHO_OFF);     

#elif UART_MULTI_COMMAND_SUPPORT == 0

/* UART 0 WILL BE INITIALIZED ONLY ! */ 
uart_init(115200, SELECT_UART0);  

#endif

#elif  UART_DYNAMIC_BAUD_CHANGE == 0
/* 
    When UART_DYNAMIC_BAUD_CHANGE == 0, Baudrate(s) is(are)fixed. uart_init() takes no parameters.
    Also note that if you use 2 UARTS, BOTH UARTS WILL BE INITIALIZED!
    You can still select Uart and give commands separetly to each UART.
*/
uart_init();  
#endif /* #elif  UART_DYNAMIC_BAUD_CHANGE == 0 */

/* ENABLE GLOBAL INTERRUPTS */
asm("sei");

#if    NUMBER_OF_UARTS == 2 && UART_DYNAMIC_BAUD_CHANGE == 1

uart_init(115200, SELECT_UART1);
uart_command(UART_TX_ON);
uart_puts_P("\r\nThis is the Second uart\r\n");
uart_command(UART_TX_OFF);
uart_command(SELECT_UART0);

#elif  NUMBER_OF_UARTS == 2 && UART_DYNAMIC_BAUD_CHANGE == 0

uart_command(SELECT_UART1);
uart_command(UART_TX_ON);
uart_puts_P("\r\nThis is the Second uart\r\n");
uart_command(UART_TX_OFF);
uart_command(SELECT_UART0);

#endif


    while(1)
        { 
          /* transmitting */
          uart_command( UART_TX_ON );
          uart_puts_P("\r\n");
          uart_puts("Constant String testing...\n\r");
          ptr=string0;
          while((c=*ptr++)) uart_putc(c);
          uart_puts_p(flash_string);
          uart_puts_P("uart_puts_P testing...\n\r");
          uart_puts_e(eeprom_string); uart_puts("\r\n");
          uart_command(UART_TX_OFF); uart_puts_P("This line should never be shown...\n\r");
          uart_command(UART_TX_ON);
          uart_puts_P("Number testing....\n\r");
          uart_puti((number=-123),0);   uart_puts("  "); 
          uart_puti((number=9999),0);   uart_puts("  ");
          uart_puti((number=-456),1);   uart_puts("  ");
          uart_puti((number=-456),3);   uart_puts("  ");
          uart_puti((number=-32767),5); uart_puts("  ");
          uart_puti((number=12987),0);  uart_puts("\r\n");
#if UART0_AT_COMMAND_SUPPORT == 1
/**************************************************************************************************/
/* 
   "AT" COMMAND TESTING.
   NOTE! After a command is received the receiving stop! 
         Deal with the received "AT" command and then if you want re-enable "AT" receiving. 
*/  
          uart_command(UART_ECHO_ON);             /* optional you can have no echo if you like... */
          uart_puts_P("AT testing... (20 char max)\n\r");                      
/* enable AT command receiving (turns RX_ON automatically and flushes the rx buffer) */
          uart_command(UART_GET_AT_CMD);          
/* test to see if somethig was received. if not wait. (you dont need to wait in your programs) */          
          while( !uart_command(UART_AT_CMD_RECEIVED) ); 

#if UART0_ERROR_CHECK_SUPPORT == 1
          if(uart_command(UART_GET_ERROR))
           {
               uart_puts_P("\r\n");
               uart_puts_P("UART ERROR : ");
               uart_puti(uart_command(UART_GET_ERROR),0);
           }
#endif

          uart_get_buffer(rx_buffer_contents);
          uart_puts_P("\r\n");
          uart_puts_P("You Typed :  "); 
          uart_puts(rx_buffer_contents);  uart_puts("\r\n");
#endif
/**************************************************************************************************/

/* RAW DATA  TESTING  if enabled */ 
#if UART0_RAW_DATA_SUPPORT == 1
          
          uart_puts_P("uart RAW DATA testing...\n\r"); 
          uart_puts_P("Type at least 10 chars...\n\r"); 

          /* enable RAW data receiving (turns RX_ON automatically and flushes the rx buffer) */
          uart_command(UART_GET_RAW_DATA);  

          /* If 10 chars were received print them else wait */
          while( uart_command(UART_GET_RX_COUNT) < 10 );   

          /* Error checking if enabled */         
#if UART0_ERROR_CHECK_SUPPORT == 1
          if(uart_command(UART_GET_ERROR))
           {
               uart_puts_P("\r\n");
               uart_puts_P("UART ERROR : ");
               uart_puti(uart_command(UART_GET_ERROR),0);
           }
#endif
          /* Get the first 5 chars */
          uart_get_buf(rx_buffer_contents, 5);
          uart_puts_P("\r\n");
          uart_puts_P("You Typed :  "); 
          uart_puts(rx_buffer_contents);  uart_puts("\r\n");

          /* Wait untill the buffer gets filled with at least 10 chars again */ 
          uart_puts_P("Type some more ");
          uart_puts_P("\r\n");
          while( uart_command(UART_GET_RX_COUNT) < 10 ); 

          /* Error checking if enabled */ 
#if UART0_ERROR_CHECK_SUPPORT == 1
          if(uart_command(UART_GET_ERROR))
           {
               uart_puts_P("\r\n");
               uart_puts_P("UART ERROR : ");
               uart_puti(uart_command(UART_GET_ERROR),0);
           }
#endif

          uart_puts_P("\r\n");
          uart_puts_P("Here are the leftovers  ");
          uart_get_buffer(rx_buffer_contents);
          uart_puts(rx_buffer_contents);  uart_puts("\r\n");
#endif            

/**************************************************************************************************/
/* OTHER UART TESTS */

          /* receive a string test */
          uart_puts_P("uart_gets testing...\n\r");
          uart_command(UART_GET_STRING); 
          while( !uart_command(UART_STRING_RECEIVED) );

          /* Error checking if enabled */ 
#if UART0_ERROR_CHECK_SUPPORT == 1
          if(uart_command(UART_GET_ERROR))
           {
               uart_puts_P("\r\n");
               uart_puts_P("UART ERROR : ");
               uart_puti(uart_command(UART_GET_ERROR),0);
           }
#endif

          uart_puts("\r\n");
          uart_puts_P("You Typed :  "); 
          uart_get_buffer(rx_buffer_contents);
          uart_puts(rx_buffer_contents);  uart_puts("\r\n");

          /* using polled mode with uart_putc() */         
          uart_command(UART_ECHO_OFF);
          number=20;
          uart_puts("Type some more...\n\r");
          while(number) {in_char=uart_getc(); uart_putc(in_char); number--; }
          uart_putc('\n'); uart_putc('\r');
/* Prompt user for a repetition of the whole loop */                                                        
          uart_puts_P("Here we go again...\n\r");
          uart_puts_P("PRESS ENTER TO CONTINUE");
          while(uart_getc()!='\r'); uart_puts("\r\n");
          uart_command(UART_RX_OFF | UART_TX_OFF);              
        }
               
}
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/

