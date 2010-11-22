/******************************************************************************
 *
 * ID      : $Id: net.c 37 2010-11-21 20:44:50Z SH $ 
 * Revsion : $Rev: 37 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-11-21 21:44:50 +0100 (So, 21 Nov 2010) $   
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
 #include <string.h>
 #include <avr/interrupt.h>
 #include "config.h"
 #include "log.h" 
 #include "g2100.h"
 #include "net.h"
 #include "arp.h"
 #include "ambientcolor.h"
 

/******************************************************************************
 * Buffer for sending and receiving network data
 ******************************************************************************/
 unsigned char MyNetworkBuffer[NETWORK_BUFSIZE+2];

/******************************************************************************
 * Initialize network stack
 ******************************************************************************/
 void InitializeNetwork()
 {
   /* Initialize hardware
    */
    zg_init();

   /* Enable network interrupt
    */
    ZG2100_ISR_ENABLE();
 }

/******************************************************************************
 * Connect to WLAN
 ******************************************************************************/
 unsigned char ConnectNetwork()
 {
  	 zg_drv_process();
 	 return zg_get_conn_state();
 }


/******************************************************************************
 * Interrupt service routine 
 ******************************************************************************/
 SIGNAL(SIG_INTERRUPT0) 
 {
    zg_isr();
 }

/******************************************************************************
 * HandleNetworkData
 ******************************************************************************/
 void HandleNetworkData()
 {
   /* Call network driver
    */
    zg_drv_process();

   /* Have we received some data?
    */
    unsigned int len = zg_get_rx_status();
    if ( len > 0 )
    { 
      /* Strip off the checksum
       */
       MyNetworkBuffer[len-4] = 0;

       if ( IsArpMessage ( MyNetworkBuffer ) )
       {
          ARP_Packet * arp = (ARP_Packet*) (MyNetworkBuffer + Ethernet_DataOffset);

          if ( ProcessArpMessage( arp ) )
          {
             log("Sent ARP Response :\n\r");
             DumpArpMessage( arp );
          }  
       }
       else
			
      /* Check for ambient color message
       */   
       if ( IsAmbientColorMessage( MyAddress.IP , AMBICOLOR_PORT, (char*) MyNetworkBuffer ) )
       {
          HandleAmbientColorMessage( (char*) MyNetworkBuffer );
       }
    }
 }

/******************************************************************************
 * HandleNetworkData
 ******************************************************************************/
 void SendNetworkData( unsigned int len)
 {
    zg_set_buf(MyNetworkBuffer, len);
    zg_set_tx_status(1);
 }

