/******************************************************************************
 *
 * ID      : $Id: arp.c 33 2010-11-21 20:42:00Z SH $ 
 * Revsion : $Rev: 33 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-11-21 21:42:00 +0100 (So, 21 Nov 2010) $   
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
 #include <avr/pgmspace.h>
 #include <string.h>
 #include "net.h"
 #include "arp.h"
 #include "log.h"

/******************************************************************************
 * IsArpMessage
 ******************************************************************************/
 int IsArpMessage( unsigned char* msg )
 {
    int result = 0;

    Ethernet_Header * ethernet = (Ethernet_Header *) msg;
    if ( htons( ethernet->Type ) == ARP_DATAGRAM )
    {
       result = 1;
    }
    return result; 
 }

/******************************************************************************
 * CreateArpMessage
 ******************************************************************************/
 void CreateArpMessage ( ARP_Packet * arp, int opcode, ip4_address sa, mac_address sm, ip4_address da, mac_address dm  )
 {
    if (arp != NULL )
    {
       arp->HardwareType = htons( 1     );
       arp->ProtocolType = htons( 0x800 );
       arp->HardwareLength = MACLEN;
       arp->ProtocolLength = IP4LEN;
       arp->Opcode = htons( opcode );
       
       memcpy( arp->SourceMac, sm, MACLEN);
       arp->SourceAddress.IP = sa.IP;

       memcpy( arp->DestinationMac, dm, MACLEN);
       arp->DestinationAddress.IP = da.IP;
    }
 }


/******************************************************************************
 * ProcessArpMessage
 ******************************************************************************/
 unsigned char ProcessArpMessage( ARP_Packet_Ptr arp )
 {
    unsigned char result = 0;

    if ( arp->DestinationAddress.IP == MyAddress.IP )
    {
       log ("Handle ARP request for %i.%i.%i.%i\n\r", MyAddress.Byte[0],
                                                      MyAddress.Byte[1],
                                                      MyAddress.Byte[2],
                                                      MyAddress.Byte[3]  );

       DumpArpMessage(arp);


      /* Remember host´s IP and MAC address
       */
       ip4_address host;
       host.IP = arp->SourceAddress.IP;

       mac_address mac;
       memcpy( mac, arp->SourceMac, MACLEN);

       int len =  Ethernet_DataOffset + sizeof(ARP_Packet);

      /* Prepare ethernet frame
       */
       Ethernet_Header_Ptr eth = (Ethernet_Header_Ptr) MyNetworkBuffer;

       memcpy( eth->Destination, mac, MACLEN );
       memcpy( eth->Source, zg_get_mac(), MACLEN );
       eth->Type = htons(ARP_DATAGRAM) ;

      /* Prepare arp response ( opcode 2 )
       */
       ARP_Packet_Ptr response = (ARP_Packet_Ptr) (MyNetworkBuffer + Ethernet_DataOffset); 
       CreateArpMessage( response, 0x2, MyAddress, zg_get_mac(), host, mac );

      /* Send response via network
       */
       SendNetworkData( len );
       result = 1;
    }

    return result;
 }

/******************************************************************************
 * DumpArpMessage
 ******************************************************************************/
 void DumpArpMessage(  ARP_Packet * arp )
 {
       log("ARP Packet:\n\r");
       log("Hardware type   : 0x%x\n\r", htons(arp->HardwareType));
       log("Protocol type   : 0x%x\n\r", htons(arp->ProtocolType));
       log("Hardware length : 0x%x\n\r", arp->HardwareLength     );
       log("Protocol length : 0x%x\n\r", arp->ProtocolLength     );
       log("Opcode          : 0x%x\n\r", htons(arp->Opcode)      );

       int index = 0;
       
      /* Dump source mac address
       */
       log("Source MAC      : " );
       while ( index < 5 )
       {
          log("%02x-", arp->SourceMac[ index ]);
          ++index;
       }
       log("%02x\n\r", arp->SourceMac[ index ] );


      /* Dump source ip address
       */
       log("Source IP       : %i.%i.%i.%i\n\r",
                   arp->SourceAddress.Byte[0],
                   arp->SourceAddress.Byte[1],
                   arp->SourceAddress.Byte[2],
                   arp->SourceAddress.Byte[3] );

      /* Dump destination mac address
       */
       index = 0;
       log("Destination MAC : " );
       while ( index < 5 )
       {
          log("%02x-", arp->DestinationMac[ index ]);
          ++index;
       }
       log("%02x\n\r", arp->DestinationMac[ index ]);

      /* Dump destination ip address
       */
       log("Destination IP  : %i.%i.%i.%i\n\r",
                    arp->DestinationAddress.Byte[0],
                    arp->DestinationAddress.Byte[1],
                    arp->DestinationAddress.Byte[2],
                    arp->DestinationAddress.Byte[3]  );
       
       log("\n\r");

 }
