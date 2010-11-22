/******************************************************************************
 *
 * ID      : $Id: log.c 26 2010-11-21 12:47:24Z SH $ 
 * Revsion : $Rev: 26 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-11-21 13:47:24 +0100 (So, 21 Nov 2010) $   
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


/******************************************************************************
 * Include Files
 ******************************************************************************/
 #include <string.h>
 #include <stdarg.h>
 #include <stdlib.h>
 #include <ctype.h>
 #include "config.h"
 #include "log.h"
 #include "serial.h"


/******************************************************************************
 * log_init
 ******************************************************************************/
 void log_init()
 {
    uart_init(BAUDRATE);
 }

/******************************************************************************
 *
 * log_write
 *
 * Writes log statements to the serial port. Supports the following format
 * strings:
 *
 *  
 *
 * Format strings have to be stored in flash memory.
 *
 ******************************************************************************/
 void log_write( const char* format, ... )
 {
 	 va_list params;
	 va_start (params, format );	

    char temp;
    char * pchar;
    char buffer[17];

    unsigned char digits  = 0;
    unsigned char padding = ' ';

    char byte = pgm_read_byte( format++ );
    while ( byte != 0 )
    {
       if ( byte != '%' ) 
       {
          uart_putc( byte );
       }
       else
       {
         /* Get next byte from format string 
          */
          byte = pgm_read_byte( format++ );
          if ( byte == 0 ) break;
          
          if ( isdigit(byte) )
          {
            /* convert to number and remember it
             */
             digits = byte - '0';

             if ( digits == 0 )
             {
                padding = '0';

               /* Get next byte from format string 
                */
                byte = pgm_read_byte( format++ );
                if ( byte == 0 ) break;

                if ( isdigit(byte) )
                {
                   digits = byte - '0';
                }
             }

            /* Get next byte from format string 
             */
             byte = pgm_read_byte( format++ );
             if ( byte == 0 ) break;
          }

          switch ( byte )
          {
             case 'c':   temp = va_arg( params, int );
                         uart_putc( temp );
                         break;
             
             case 's':   pchar = va_arg(params, char* );
                         uart_puts( pchar );
                         break;

             case 'i':   format_number( va_arg(params, int), 10, digits, padding, buffer );
                         uart_puts( buffer);
                         break;


             case 'x':   format_number( va_arg(params, int), 16, digits, padding, buffer );
                         uart_puts( buffer);
                         break;
             
             case 'b':   format_number( va_arg(params, int), 2, digits, padding, buffer );
                         uart_puts( buffer);
                         break;
          }
       }
      
      /* Get next byte from format string 
       */
       byte = pgm_read_byte( format++ );

    };
	 va_end( params);
 }

/******************************************************************************
 * Internal helper function 
 ******************************************************************************/
 void format_number( long number, int base, int digits, char padding, char* buffer )
 {
    char temp[33];
    int  pad = 0;
    int  len = 0;
    int index = 0;    
    
    memset( temp,0,17);

    if ( buffer != NULL )
    {
       ltoa( number, temp, base );
       len = strlen( temp );
      
       if ( digits > 0 )
       {
          pad = digits - len;
       }

       if ( temp[0] == '-' )
       {
          buffer[0] = '-';
          ++index;
       }
       
       while ( pad > 0 )
       {
          buffer[index] = padding;
          ++index;
          --pad;
       }
       strcpy( buffer + index, temp );
    }
 }


/******************************************************************************
 * The end.
 ******************************************************************************/
