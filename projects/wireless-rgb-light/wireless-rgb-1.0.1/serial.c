/******************************************************************************
 *
 * ID      : $Id: serial.c 29 2010-11-21 12:50:50Z SH $ 
 * Revsion : $Rev: 29 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-11-21 13:50:50 +0100 (So, 21 Nov 2010) $   
 *
 ******************************************************************************
 *
 * License
 *
 * Copyright (c) 2010 by Stefan Heesch, http://www.heesch.net
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 ******************************************************************************/


/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <serial.h>
 #include <avr/io.h>
 #include <avr/pgmspace.h>
 #include <avr/interrupt.h>

/****************************************************************************
 *                Global variables
 ****************************************************************************/
 volatile unsigned char   uart_receivedChar;
          unsigned char   uart_rxChar;

/****************************************************************************
 *
 * Signal Name:   SIG_UART_TRANS
 *
 * Description:   UART Receive Complete Interrupt Function 
 *
 ***************************************************************************/
 SIGNAL(SIG_UART_RECV)      
 {
   /* Store received character 
    */
    uart_rxChar = UDR;

   /* Indicate that the UART has received a character 
    */
    uart_receivedChar = 1;
}

/****************************************************************************
 *
 * Function Name: uart_putc( unsigned char byte )
 *
 * Description:   Send a single byte
 *
 * Parameters:    byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void uart_putc( char data )
 {
    /* wait for UART to become available 
     */
     while(!( UCSRA & (1<<UDRE)));  

    /* Send character 
     */
     UDR = data;
 
 } /* uart_putc */


/****************************************************************************
 *
 * Function Name: uart_puts_p( char* string )
 *
 * Description:   Print a string from the program memory
 *
 * Parameters:    unsigned char* string
 *
 * Return:        void
 *
 ***************************************************************************/
 void uart_puts_p( char* string )
 {
     char c = pgm_read_byte( string++ );

     while ( c != 0 )
     {
        uart_putc( c );
        c = pgm_read_byte( string++ );
     }

 
 } /* uart_puts_p */


/****************************************************************************
 *
 * Function Name: uart_puts( char* str )
 *
 * Description:   Send a single byte
 *
 * Parameters:    byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void uart_puts( char* str )
 {
    while (*str)
    {
       uart_putc(*str++);
    }
   
 } /* uart_puts */



/****************************************************************************
 *
 * Function Name: uart_getc( void )
 *
 * Description:   Receive a single byte
 *
 * Parameters:    void
 *
 * Return:        unsigned char
 *
 ***************************************************************************/
 char uart_getc( void )
 {
   /* wait for UART to indicate that a character has been received 
    */
    while(!uart_receivedChar);
    uart_receivedChar = 0;

   /* read byte from UART data buffer 
    */
    return uart_rxChar;
 
 } /* uart_getc */



/****************************************************************************
 *
 * Function Name: uart_init( void )
 *
 * Description:   Initialise the UART
 *
 * Parameters:    unsigned long baudrate
 *
 * Return:        void
 *
 ***************************************************************************/
 void uart_init( unsigned long baudrate )
 {
    /* initialise global variables
     */
     uart_receivedChar = 0;
   
     UCSRB =(1 << TXEN | 1 << RXEN | 1<< RXCIE);
     UBRRL =(F_CPU / (baudrate * 16L) - 1);

 } /* uart_init */



