#ifndef __SERIAL_AVR_H__
#define __SERIAL_AVR_H__
/****************************************************************************
 *
 *  Module:       $Filename: D:\Projects\web\microlib\include\os\serial_avr.h $
 *
 *  Author:       $Author: SH $
 *
 *  Revision:     $Revision: 1.2 $
 *
 *  Version:      $Version: 0.3.0 $
 *
 *  Date:         $Date: Samstag, 4. Mai 2002 14:39:34 $
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
 *  $Log: D:\Daten\QVCS\archive\microlib\include\os\serial_avr.i $
 *  
 *    MicroLIB - serial port functions for ATMEL AVR micro controlers
 *  
 *  Revision 1.2  by: SH  Rev date: 04.05.2002 14:39:34
 *    Updated copyright to LGPL
 *  
 *  Revision 1.1  by: SH  Rev date: 04.05.2002 12:39:56
 *    - Replaced header <signal.h> by <sig-avr.h> 
 *    
 *  
 *  Revision 1.0  by: SH  Rev date: 17.09.2000 17:43:14
 *    Initial revision.
 *  
 *  $Endlog$
 *
 ****************************************************************************/

/****************************************************************************
 *                 Include Definitions
 ****************************************************************************/
 #include <common.h>
 #include <io.h>
 #include <interrupt.h>
 #include <sig-avr.h>
 #include <progmem.h>

                                            
/****************************************************************************
 *                 Macro Definitions
 ****************************************************************************/

/* Constants for UART baud rate calculation 
 */
 #define CPU_CLOCK         8000000L   /* 8 Mhz */

/* Output macros
 */
 #define PRINT(string)     (uart_printProgString(PSTR(string)))
 #define EOL               uart_printfEndOfLine
 
/****************************************************************************
 *                 Public variables
 ****************************************************************************/
 extern volatile byte  uart_receivedChar;

/****************************************************************************
 *                 Public functions
 ****************************************************************************/
 extern void uart_sendByte(byte Data);
 extern byte uart_receiveByte(void);
 extern void uart_printProgString(byte* pBuf);
 extern void uart_printfEndOfLine(void);
 extern void uart_init(unsigned long baudrate);



 #endif /* __SERIAL_AVR_H__ */




