/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\source\os\serial_avr.c $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.1 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:45:42 $
 *
 *  Copyright:    Copyright (C) 2000-2002 Stefan Heesch (heesch@cybervillage.de)
 *
 *  This software/hardware is free software/hardware; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, or (at your
 *  option) any later version.
 *
 *  This software is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 *  PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Description:  ATMEL AVR serial port functions. Heavily based on the GCC
 *                examples from Volker Roth(http://members.xoom.xom/volkeroth)
 *
 ****************************************************************************
 *
 *  History:
 *
 *  $Log: D:\Daten\QVCS\archive\microlib\source\os\serial_avr.d $
 *  
 *    MicroLIB - serial port functions for ATMEL AVR 90S8515
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 14:45:42
 *    Updated copyright to LGPL
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:49:14
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                Include Definitions
 ****************************************************************************/
 #include <os/serial_avr.h>

/****************************************************************************
 *                Global constants
 ****************************************************************************/
 prog_char uart_endOfLine[3] = {0x0d,0x0a,0};

/****************************************************************************
 *                Global variables
 ****************************************************************************/
 volatile byte  uart_ready;
 volatile byte  uart_receivedChar;
          byte  uart_rxChar;
          byte* pBuffer;


/****************************************************************************
 *
 * Signal Name:   SIG_UART_TRANS
 *
 * Description:   UART Transmit Complete Interrupt Function 
 *
 ***************************************************************************/
 SIGNAL( SIG_UART_TRANS )
 {
   /* Test if a string is being sent 
    */
    if (pBuffer != NULL)
    {
       /* Go to next character in string 
        */
        pBuffer++;
       
       /* Test if the end of string has been reached 
        */
        if (PRG_RDB(pBuffer) == 0)
        {
           /* String has been sent 
            */
            pBuffer = NULL;
           
           /* Indicate that the UART is now ready to send 
            */
            uart_ready = 1;
            return;
        }
       /* Send next character in string 
        */
        outp( PRG_RDB(pBuffer), UDR );
        return;
    }

   /* Indicate that the UART is now ready to send 
    */
    uart_ready = 1;
 
 } /* SIGNAL (SIG_UART_TRANS) */


/****************************************************************************
 *
 * Signal Name:   SIG_UART_TRANS
 *
 * Description:   UART Receive Complete Interrupt Function 
 *
 ***************************************************************************/
 SIGNAL(SIG_UART_RECV)      
 {
   /* Indicate that the UART has received a character 
    */
    uart_receivedChar = 1;

   /* Store received character 
    */
    uart_rxChar = inp(UDR);
}

/****************************************************************************
 *
 * Function Name: uart_sendByte( byte data )
 *
 * Description:   Send a single byte
 *
 * Parameters:    byte data
 *
 * Return:        void
 *
 ***************************************************************************/
 void uart_sendByte( byte data )
 {
    /* wait for UART to become available 
     */
     while(!uart_ready);
     uart_ready = 0;

    /* Send character 
     */
     outp( data, UDR );
 
 } /* uart_sendByte */

/****************************************************************************
 *
 * Function Name: uart_receiveByte( void )
 *
 * Description:   Receive a single byte
 *
 * Parameters:    void
 *
 * Return:        byte
 *
 ***************************************************************************/
 byte uart_receiveByte( void )
 {
   /* wait for UART to indicate that a character has been received 
    */
    while(!uart_receivedChar);
    uart_receivedChar = 0;

   /* read byte from UART data buffer 
    */
    return uart_rxChar;
 
 } /* uart_receiveByte */

/****************************************************************************
 *
 * Function Name: uart_printProgString( byte* string )
 *
 * Description:   Print a string from the program memory
 *
 * Parameters:    byte* string
 *
 * Return:        void
 *
 ***************************************************************************/
 void uart_printProgString( byte* string )
 {
   /* wait for UART to become available 
    */
    while(!uart_ready);
    uart_ready = 0;

    /* Indicate to ISR the string to be sent 
     */
     pBuffer = string;

    /* Send first character 
     */
     outp( PRG_RDB(pBuffer), UDR );
 
 } /* uart_printProgString */


/****************************************************************************
 *
 * Function Name: uart_printEndOfLine( void )
 *
 * Description:   Print end of line characters: LF + CR
 *
 * Parameters:    void
 *
 * Return:        void
 *
 ***************************************************************************/
 void uart_printEndOfLine( void )
 {
   /* wait for UART to become available 
    */
    while(!uart_ready);
    uart_ready = 0;
   
    /* Indicate to ISR the string to be sent 
     */
     pBuffer = uart_endOfLine;

    /* Send first character 
     */
     outp( PRG_RDB(pBuffer), UDR );
 
 } /* uart_printEndOfLine */


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
     unsigned long n;
     
    /* initialise global variables
     */
     uart_ready        = 1;
     uart_receivedChar = 0;
     pBuffer           = NULL;

    /* enable RxD/TxD and interrupts 
     */
     outp(BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN),UCR);
    
    /* set baud rate 
     */
     n = CPU_CLOCK / 16L;
     outp( n / baudrate -1, UBRR);  
    
    /* enable interrupts 
     */
     sei();
 
 } /* uart_init */



