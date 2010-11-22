#ifndef _NET_H_
#define _NET_H_

/******************************************************************************
 *
 * ID      : $Id: net.h 26 2010-11-21 12:47:24Z SH $ 
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
 * Address types
 ******************************************************************************/

/* MAC address
 */
 #define MACLEN    6
 
 typedef unsigned char mac_address[MACLEN];


/* IP address
 */
 #define IP4LEN    4
 
 typedef union
 {
   unsigned char  Byte[IP4LEN];
   unsigned long  IP; 

 } ip4_address;

/******************************************************************************
 * Ethernet frame
 ******************************************************************************/
 
/* Header
 */ 
 typedef struct
 {
    mac_address Destination;
	 mac_address Source;
	 unsigned int  Type;	
 
 }  Ethernet_Header, * Ethernet_Header_Ptr;


 #define Ethernet_DataOffset  sizeof( Ethernet_Header )


/* Data types
 */
 #define IP_DATAGRAM     0x0800
 #define ARP_DATAGRAM    0x0806


/******************************************************************************
 * ARP packet
 ******************************************************************************/
 typedef struct
 {
   unsigned int  HardwareType;    
   unsigned int  ProtocolType;    
   unsigned char HardwareLength;
   unsigned char ProtocolLength;   
   unsigned int  Opcode;
   mac_address   SourceMac;
   ip4_address   SourceAddress;
   mac_address   DestinationMac;
   ip4_address   DestinationAddress;  

 } ARP_Packet, * ARP_Packet_Ptr;


/******************************************************************************
 * IP datagram
 ******************************************************************************/

/* Header
 */ 
 typedef struct
 {
	unsigned char	VersionLength;
	unsigned char	Service;
	unsigned int	Length;
	unsigned int	Ident;
   unsigned int	Offset;
   unsigned char	TTL;
   unsigned char	Protocol;
   unsigned int	Checksum;
   unsigned long	SourceAddress;
   unsigned long	DestinationAddress;
 
 }  IP_Header, * IP_Header_Ptr;

 #define IP_DataOffset ( sizeof( Ethernet_Header ) + sizeof( IP_Header ) )



/******************************************************************************
 * UDP datagram
 ******************************************************************************/

/* Header
 */ 
 typedef struct
 {
	unsigned int 	SourcePort;
	unsigned int 	DestinationPort;
	unsigned int   Length;
	unsigned int	Checksum;
 
 }  UDP_Header, * UDP_Header_Ptr;

 #define UDP_DataOffset ( IP_DataOffset + sizeof(UDP_Header))

/******************************************************************************
 * Function prototypes
 ******************************************************************************/
 void InitializeNetwork();
 unsigned char ConnectNetwork();
 
 void HandleNetworkData();
 void SendNetworkData( unsigned int bytes );

/******************************************************************************
 * Network / Host conversion macros
 ******************************************************************************/
 #define htons(x) ( ((x & 0xFF00)>> 8) + (( x & 0x00FF ) << 8) )
 #define htonl(x) ( ((x & 0xFF000000) >> 24) + \
                    ((x & 0xFF0000)   >> 8 ) + \
                    ((x & 0xFF00 )    << 8 ) + \
                    ((x & 0xFF )      << 24) )

 #define ntohs(x) htons(x)
 #define ntohl(x) htonl(x)


/******************************************************************************
 * My IP Address 
 ******************************************************************************/
 extern ip4_address    MyAddress;
 extern unsigned int   MyPort;
 extern unsigned char  MyNetworkBuffer[];

#endif
