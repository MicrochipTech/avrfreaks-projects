/******************************************************************************
 *
 * ID      : $Id: ambientcolor.c 26 2010-11-21 12:47:24Z SH $ 
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
 * Include files
 ******************************************************************************/
 #include <stdlib.h>
 #include <string.h>
 #include "ambientcolor.h"
 #include "timer.h"
 #include "net.h"
 #include "log.h"


/******************************************************************************
 * IsAmbientColorMessage
 ******************************************************************************/
 int IsAmbientColorMessage( unsigned long address, unsigned int port, char* msg )
 {
    int match = 0;

    Ethernet_Header * ethernet = (Ethernet_Header *) msg;
    IP_Header * ip = (IP_Header *) (msg + Ethernet_DataOffset);
   UDP_Header * udp  = (UDP_Header *) (msg + IP_DataOffset);

    log("Ethernet Type: 0x%x\n\r",htons( ethernet->Type ));


   /* Check for address and IP data frame
    */
   if ( ip->DestinationAddress == address && ethernet->Type == htons(IP_DATAGRAM) )
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


       log("\n\rReceived AmbientColor Message\n\r" );


       token = strtok_r ( msg + UDP_DataOffset,":",&context);
       while (token != NULL)
       {
           switch( index )
           {
              case 1: red = atoi( token );
                      log("Setting RED to 0x%x\n\r", red);
                      break;

              case 2: green = atoi(token);
                      log("Setting GREEN to 0x%x\n\r", green );
                      break;

              case 3: blue = atoi(token);
                      log("Setting BLUE to 0x%x\n\r", blue );
                      break;
           }   
           ++index;        
           token = strtok_r (NULL,":",&context);
       }
       SetColors(red, green, blue);
       log("\n\r" );

    }
 
 }

