#ifndef _NET_H_
#define _NET_H_

/******************************************************************************
 *
 * ID      : $Id: net.h 11 2010-10-24 16:42:52Z SH $ 
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
 * Include Files
 ******************************************************************************/
 #include "networkcard/enc28j60.h"


/******************************************************************************
 * MAC address
 ******************************************************************************/

/* Length of MAC address
 */
 #define MACLEN    6


/******************************************************************************
 * Ethernet frame
 ******************************************************************************/

/* MTU size
 */ 
 #define MTU_SIZE    600

 
/* Header
 */ 
 typedef struct
 {
    unsigned char Destination[MACLEN];
	 unsigned char Source[MACLEN];
	 unsigned int  Type;	
 
 }  Ethernet_Header;


#define Ethernet_DataOffset  sizeof( Ethernet_Header )


/* Data types
 */
 #define IP_DATAGRAM     0x0800
 #define ARP_DATAGRAM    0x0806


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
 
 }  IP_Header;

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
 
 }  UDP_Header;

 #define UDP_DataOffset ( IP_DataOffset + sizeof(UDP_Header))

/******************************************************************************
 * Function prototypes
 ******************************************************************************/
 void InitializeNetwork();
 void HandleNetworkData();


/******************************************************************************
 * ConversionMacros
 ******************************************************************************/
 #define IP(a,b,c,d) ((unsigned long)(d)<<24)+((unsigned long)(c)<<16)+((unsigned long)(b)<<8)+a

 #define htons(x) ( ((x & 0xFF00)>> 8) + (( x & 0x00FF ) << 8) )
 #define htonl(x) ( ((x & 0xFF000000) >> 24) +  ((x & 0xFF0000) >> 8) + (( x & 0xFF00 ) << 8) + (( x & 0xFF ) << 24) )




#endif
