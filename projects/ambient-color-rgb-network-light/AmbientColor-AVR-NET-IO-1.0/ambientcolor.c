/******************************************************************************
 *
 * ID      : $Id: ambientcolor.c 11 2010-10-24 16:42:52Z SH $ 
 * Revsion : $Rev: 11 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-10-24 18:42:52 +0200 (So, 24 Okt 2010) $   
 *
 ******************************************************************************
 *
 * The MIT License
 *
 * Copyright (c) 2010 by Stefan Heesch, http://www.heesch.net
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
 #include <stdlib.h>
 #include <string.h>
 #include "ambientcolor.h"
 #include "timer.h"
 #include "net.h"


/******************************************************************************
 * IsAmbientColorMessage
 ******************************************************************************/
 int IsAmbientColorMessage( unsigned long address, unsigned int port, char* msg )
 {
    int match = 0;

    Ethernet_Header * ethernet = (Ethernet_Header *) msg;
    IP_Header * ip = (IP_Header *) (msg + Ethernet_DataOffset);
	 UDP_Header * udp  = (UDP_Header *) (msg + IP_DataOffset);


   /* Check for multicast address and IP data frame
    */
 	 if ( (htonl(ip->DestinationAddress) == address) && ethernet->Type == htons(IP_DATAGRAM) )
 	 {
      /* Check for the correct port
       */
       if ( port == htons(udp->DestinationPort) )
       {
          match = 1;
       }
	 }
    return match;
 }


/******************************************************************************
 * HandleAmbientColorMessage
 ******************************************************************************/
 void HandleAmbientColorMessage( char* msg )
 {
    if ( msg != NULL && strncmp( msg + UDP_DataOffset, "AmbientColor:",13) == 0 )
    {
       int red   = 0;
       int green = 0;
       int blue  = 0;
       int index = 0;
       
       char * token;
       char * context;


       token = strtok_r ( msg + UDP_DataOffset,":",&context);
       while (token != NULL)
       {
           switch( index )
           {
              case 1: red = atoi( token );
                      break;

              case 2: green = atoi(token);
                      break;

              case 3: blue = atoi(token);
                      break;
           }   
           ++index;        
           token = strtok_r (NULL,":",&context);
       }
       SetColors(red, green, blue);

    }
 
 }

