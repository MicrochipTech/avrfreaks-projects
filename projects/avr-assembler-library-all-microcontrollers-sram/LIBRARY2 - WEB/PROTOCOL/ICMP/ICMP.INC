;<$DS_HEADER>
;+--------------------------------------------------------------------------------+              
;|João D´Artagnan A. Oliveira Programmer and Author; 				  |
;|										  |
;| All rights reserved.								  |
;|										  |
;|   Redistribution and use in source and binary forms, with or without		  |
;|  modification, are permitted provided that the following conditions are met:	  |
;|										  |
;|   * Redistributions of source code must retain the above copyright		  |
;|     notice, this list of conditions and the following disclaimer.		  |
;|										  |
;|   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  |
;|   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    |
;|   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   |
;|   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     |
;|   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR	  |
;|   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF	  |
;|   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS	  |
;|   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN	  |
;|   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)	  |
;|   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   |
;|   POSSIBILITY OF SUCH DAMAGE.						  |
;|										  |
;|   Brasília, Brazil November 30,2015 	06:28 am				  |
;+--------------------------------------------------------------------------------+
;</$DS_HEADER>
;+--------------------------------------------------------------+---+
;| BY JOÃO DARTAGNAN ANTUNES OLIVEIRA			|
;| DATA:4/12/2005					|
;>------------------------------------------------------<
;| A V R  I P  P R O T O C O L  I P V 4  		|
;| V E R S I O N  1.0					|
;+------------------------------------------------------+
;| THIS DRIVE HAS INTEND TO USE WITH AVRS WITH SRAM	|
;+------------------------------------------------------+
;|
;|  THIS PROTOCOL WAS BE DEVELOPED BASED ON RFC 792 DESCRIBE BELOW
;|		
;|	                   INTERNET CONTROL MESSAGE PROTOCOL
;|	
;|	                         DARPA INTERNET PROGRAM
;|	                         PROTOCOL SPECIFICATION
;|	
;|	Introduction
;|	
;|	   The Internet Protocol (IP) [1] is used for host-to-host datagram
;|	   service in a system of interconnected networks called the
;|	   Catenet [2].  The network connecting devices are called Gateways.
;|	   These gateways communicate between themselves for control purposes
;|	   via a Gateway to Gateway Protocol (GGP) [3,4].  Occasionally a
;|	   gateway or destination host will communicate with a source host, for
;|	   example, to report an error in datagram processing.  For such
;|	   purposes this protocol, the Internet Control Message Protocol (ICMP),
;|	   is used.  ICMP, uses the basic support of IP as if it were a higher
;|	   level protocol, however, ICMP is actually an integral part of IP, and
;|	   must be implemented by every IP module.
;|	
;|	   ICMP messages are sent in several situations:  for example, when a
;|	   datagram cannot reach its destination, when the gateway does not have
;|	   the buffering capacity to forward a datagram, and when the gateway
;|	   can direct the host to send traffic on a shorter route.
;|	
;|	   The Internet Protocol is not designed to be absolutely reliable.  The
;|	   purpose of these control messages is to provide feedback about
;|	   problems in the communication environment, not to make IP reliable.
;|	   There are still no guarantees that a datagram will be delivered or a
;|	   control message will be returned.  Some datagrams may still be
;|	   undelivered without any report of their loss.  The higher level
;|	   protocols that use IP must implement their own reliability procedures
;|	   if reliable communication is required.
;|	
;|	   The ICMP messages typically report errors in the processing of
;|	   datagrams.  To avoid the infinite regress of messages about messages
;|	   etc., no ICMP messages are sent about ICMP messages.  Also ICMP
;|	   messages are only sent about errors in handling fragment zero of
;|	   fragemented datagrams.  (Fragment zero has the fragment offeset equal
;|	   zero).
;|	
;|	Message Formats
;|	
;|	   ICMP messages are sent using the basic IP header.  The first octet of
;|	   the data portion of the datagram is a ICMP type field; the value of
;|	   this field determines the format of the remaining data.  Any field
;|	   labeled "unused" is reserved for later extensions and must be zero
;|	   when sent, but receivers should not use these fields (except to
;|	   include them in the checksum).  Unless otherwise noted under the
;|	   individual format descriptions, the values of the internet header
;|	   fields are as follows:
;|	
;|	   Version
;|	
;|	      4
;|	
;|	   IHL
;|	
;|	      Internet header length in 32-bit words.
;|	
;|	   Type of Service
;|	
;|	      0
;|	
;|	   Total Length
;|	
;|	      Length of internet header and data in octets.
;|	
;|	   Identification, Flags, Fragment Offset
;|	
;|	      Used in fragmentation, see [1].
;|	
;|	   Time to Live
;|	
;|	      Time to live in seconds; as this field is decremented at each
;|	      machine in which the datagram is processed, the value in this
;|	      field should be at least as great as the number of gateways which
;|	      this datagram will traverse.
;|	
;|	   Protocol
;|	
;|	      ICMP = 1
;|	
;|	   Header Checksum
;|	
;|	      The 16 bit one's complement of the one's complement sum of all 16
;|	      bit words in the header.  For computing the checksum, the checksum
;|	      field should be zero.  This checksum may be replaced in the
;|	      future.
;|	
;|	   Source Address
;|	
;|	      The address of the gateway or host that composes the ICMP message.
;|	      Unless otherwise noted, this can be any of a gateway's addresses.
;|	
;|	   Destination Address
;|	
;|	      The address of the gateway or host to which the message should be
;|	      sent.
;|	
;|	
;|	Destination Unreachable Message
;|	
;|	    0                   1                   2                   3
;|	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Type      |     Code      |          Checksum             |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |                             unused                            |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |      Internet Header + 64 bits of Original Data Datagram      |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	
;|	   IP Fields:
;|	
;|	   Destination Address
;|	
;|	      The source network and address from the original datagram's data.
;|	
;|	   ICMP Fields:
;|	
;|	   Type
;|	
;|	      3
;|	
;|	   Code
;|	
;|	      0 = net unreachable;
;|	
;|	      1 = host unreachable;
;|	
;|	      2 = protocol unreachable;
;|	
;|	      3 = port unreachable;
;|	
;|	      4 = fragmentation needed and DF set;
;|	
;|	      5 = source route failed.
;|	
;|	   Checksum
;|	
;|	      The checksum is the 16-bit ones's complement of the one's
;|	      complement sum of the ICMP message starting with the ICMP Type.
;|	      For computing the checksum , the checksum field should be zero.
;|	      This checksum may be replaced in the future.
;|	
;|	   Internet Header + 64 bits of Data Datagram
;|	
;|	      The internet header plus the first 64 bits of the original
;|	      datagram's data.  This data is used by the host to match the
;|	      message to the appropriate process.  If a higher level protocol
;|	      uses port numbers, they are assumed to be in the first 64 data
;|	      bits of the original datagram's data.
;|	
;|	   Description
;|	
;|	      If, according to the information in the gateway's routing tables,
;|	      the network specified in the internet destination field of a
;|	      datagram is unreachable, e.g., the distance to the network is
;|	      infinity, the gateway may send a destination unreachable message
;|	      to the internet source host of the datagram.  In addition, in some
;|	      networks, the gateway may be able to determine if the internet
;|	      destination host is unreachable.  Gateways in these networks may
;|	      send destination unreachable messages to the source host when the
;|	      destination host is unreachable.
;|	
;|	      If, in the destination host, the IP module cannot deliver the
;|	      datagram  because the indicated protocol module or process port is
;|	      not active, the destination host may send a destination
;|	      unreachable message to the source host.
;|	
;|	      Another case is when a datagram must be fragmented to be forwarded
;|	      by a gateway yet the Don't Fragment flag is on.  In this case the
;|	      gateway must discard the datagram and may return a destination
;|	      unreachable message.
;|	
;|	      Codes 0, 1, 4, and 5 may be received from a gateway.  Codes 2 and
;|	      3 may be received from a host.
;|	
;|	
;|	Time Exceeded Message
;|	
;|	    0                   1                   2                   3
;|	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Type      |     Code      |          Checksum             |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |                             unused                            |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |      Internet Header + 64 bits of Original Data Datagram      |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	
;|	   IP Fields:
;|	
;|	   Destination Address
;|	
;|	      The source network and address from the original datagram's data.
;|	
;|	   ICMP Fields:
;|	
;|	   Type
;|	
;|	      11
;|	
;|	   Code
;|	
;|	      0 = time to live exceeded in transit;
;|	
;|	      1 = fragment reassembly time exceeded.
;|	
;|	   Checksum
;|	
;|	      The checksum is the 16-bit ones's complement of the one's
;|	      complement sum of the ICMP message starting with the ICMP Type.
;|	      For computing the checksum , the checksum field should be zero.
;|	      This checksum may be replaced in the future.
;|	
;|	   Internet Header + 64 bits of Data Datagram
;|	
;|	      The internet header plus the first 64 bits of the original
;|	      datagram's data.  This data is used by the host to match the
;|	      message to the appropriate process.  If a higher level protocol
;|	      uses port numbers, they are assumed to be in the first 64 data
;|	      bits of the original datagram's data.
;|	
;|	   Description
;|	
;|	      If the gateway processing a datagram finds the time to live field
;|	      is zero it must discard the datagram.  The gateway may also notify
;|	      the source host via the time exceeded message.
;|	
;|	      If a host reassembling a fragmented datagram cannot complete the
;|	      reassembly due to missing fragments within its time limit it
;|	      discards the datagram, and it may send a time exceeded message.
;|	
;|	      If fragment zero is not available then no time exceeded need be
;|	      sent at all.
;|	
;|	      Code 0 may be received from a gateway.  Code 1 may be received
;|	      from a host.
;|	
;|	
;|	Parameter Problem Message
;|	
;|	    0                   1                   2                   3
;|	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Type      |     Code      |          Checksum             |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |    Pointer    |                   unused                      |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |      Internet Header + 64 bits of Original Data Datagram      |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	
;|	   IP Fields:
;|	
;|	   Destination Address
;|	
;|	      The source network and address from the original datagram's data.
;|	
;|	   ICMP Fields:
;|	
;|	   Type
;|	
;|	      12
;|	
;|	   Code
;|	
;|	      0 = pointer indicates the error.
;|	
;|	   Checksum
;|	
;|	      The checksum is the 16-bit ones's complement of the one's
;|	      complement sum of the ICMP message starting with the ICMP Type.
;|	      For computing the checksum , the checksum field should be zero.
;|	      This checksum may be replaced in the future.
;|	
;|	   Pointer
;|	
;|	      If code = 0, identifies the octet where an error was detected.
;|	
;|	   Internet Header + 64 bits of Data Datagram
;|	
;|	      The internet header plus the first 64 bits of the original
;|	      datagram's data.  This data is used by the host to match the
;|	      message to the appropriate process.  If a higher level protocol
;|	      uses port numbers, they are assumed to be in the first 64 data
;|	      bits of the original datagram's data.
;|	
;|	
;|	   Description
;|	
;|	      If the gateway or host processing a datagram finds a problem with
;|	      the header parameters such that it cannot complete processing the
;|	      datagram it must discard the datagram.  One potential source of
;|	      such a problem is with incorrect arguments in an option.  The
;|	      gateway or host may also notify the source host via the parameter
;|	      problem message.  This message is only sent if the error caused
;|	      the datagram to be discarded.
;|	
;|	      The pointer identifies the octet of the original datagram's header
;|	      where the error was detected (it may be in the middle of an
;|	      option).  For example, 1 indicates something is wrong with the
;|	      Type of Service, and (if there are options present) 20 indicates
;|	      something is wrong with the type code of the first option.
;|	
;|	      Code 0 may be received from a gateway or a host.
;|	
;|	
;|	Source Quench Message
;|	
;|	    0                   1                   2                   3
;|	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Type      |     Code      |          Checksum             |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |                             unused                            |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |      Internet Header + 64 bits of Original Data Datagram      |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	
;|	   IP Fields:
;|	
;|	   Destination Address
;|	
;|	      The source network and address of the original datagram's data.
;|	
;|	   ICMP Fields:
;|	
;|	   Type
;|	
;|	      4
;|	
;|	   Code
;|	
;|	      0
;|	
;|	   Checksum
;|	
;|	      The checksum is the 16-bit ones's complement of the one's
;|	      complement sum of the ICMP message starting with the ICMP Type.
;|	      For computing the checksum , the checksum field should be zero.
;|	      This checksum may be replaced in the future.
;|	
;|	   Internet Header + 64 bits of Data Datagram
;|	
;|	      The internet header plus the first 64 bits of the original
;|	      datagram's data.  This data is used by the host to match the
;|	      message to the appropriate process.  If a higher level protocol
;|	      uses port numbers, they are assumed to be in the first 64 data
;|	      bits of the original datagram's data.
;|	
;|	   Description
;|	
;|	      A gateway may discard internet datagrams if it does not have the
;|	      buffer space needed to queue the datagrams for output to the next
;|	      network on the route to the destination network.  If a gateway
;|	      discards a datagram, it may send a source quench message to the
;|	      internet source host of the datagram.  A destination host may also
;|	      send a source quench message if datagrams arrive too fast to be
;|	      processed.  The source quench message is a request to the host to
;|	      cut back the rate at which it is sending traffic to the internet
;|	      destination.  The gateway may send a source quench message for
;|	      every message that it discards.  On receipt of a source quench
;|	      message, the source host should cut back the rate at which it is
;|	      sending traffic to the specified destination until it no longer
;|	      receives source quench messages from the gateway.  The source host
;|	      can then gradually increase the rate at which it sends traffic to
;|	      the destination until it again receives source quench messages.
;|	
;|	      The gateway or host may send the source quench message when it
;|	      approaches its capacity limit rather than waiting until the
;|	      capacity is exceeded.  This means that the data datagram which
;|	      triggered the source quench message may be delivered.
;|	
;|	      Code 0 may be received from a gateway or a host.
;|	
;|	
;|	Redirect Message
;|	
;|	    0                   1                   2                   3
;|	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Type      |     Code      |          Checksum             |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |                 Gateway Internet Address                      |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |      Internet Header + 64 bits of Original Data Datagram      |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	
;|	   IP Fields:
;|	
;|	   Destination Address
;|	
;|	      The source network and address of the original datagram's data.
;|	
;|	   ICMP Fields:
;|	
;|	   Type
;|	
;|	      5
;|	
;|	   Code
;|	
;|	      0 = Redirect datagrams for the Network.
;|	
;|	      1 = Redirect datagrams for the Host.
;|	
;|	      2 = Redirect datagrams for the Type of Service and Network.
;|	
;|	      3 = Redirect datagrams for the Type of Service and Host.
;|	
;|	   Checksum
;|	
;|	      The checksum is the 16-bit ones's complement of the one's
;|	      complement sum of the ICMP message starting with the ICMP Type.
;|	      For computing the checksum , the checksum field should be zero.
;|	      This checksum may be replaced in the future.
;|	
;|	   Gateway Internet Address
;|	
;|	      Address of the gateway to which traffic for the network specified
;|	      in the internet destination network field of the original
;|	      datagram's data should be sent.
;|	
;|	
;|	   Internet Header + 64 bits of Data Datagram
;|	
;|	      The internet header plus the first 64 bits of the original
;|	      datagram's data.  This data is used by the host to match the
;|	      message to the appropriate process.  If a higher level protocol
;|	      uses port numbers, they are assumed to be in the first 64 data
;|	      bits of the original datagram's data.
;|	
;|	   Description
;|	
;|	      The gateway sends a redirect message to a host in the following
;|	      situation.  A gateway, G1, receives an internet datagram from a
;|	      host on a network to which the gateway is attached.  The gateway,
;|	      G1, checks its routing table and obtains the address of the next
;|	      gateway, G2, on the route to the datagram's internet destination
;|	      network, X.  If G2 and the host identified by the internet source
;|	      address of the datagram are on the same network, a redirect
;|	      message is sent to the host.  The redirect message advises the
;|	      host to send its traffic for network X directly to gateway G2 as
;|	      this is a shorter path to the destination.  The gateway forwards
;|	      the original datagram's data to its internet destination.
;|	
;|	      For datagrams with the IP source route options and the gateway
;|	      address in the destination address field, a redirect message is
;|	      not sent even if there is a better route to the ultimate
;|	      destination than the next address in the source route.
;|	
;|	      Codes 0, 1, 2, and 3 may be received from a gateway.
;|	
;|	
;|	Echo or Echo Reply Message
;|	
;|	    0                   1                   2                   3
;|	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Type      |     Code      |          Checksum             |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |           Identifier          |        Sequence Number        |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Data ...
;|	   +-+-+-+-+-
;|	
;|	   IP Fields:
;|	
;|	   Addresses
;|	
;|	      The address of the source in an echo message will be the
;|	      destination of the echo reply message.  To form an echo reply
;|	      message, the source and destination addresses are simply reversed,
;|	      the type code changed to 0, and the checksum recomputed.
;|	
;|	   IP Fields:
;|	
;|	   Type
;|	
;|	      8 for echo message;
;|	
;|	      0 for echo reply message.
;|	
;|	   Code
;|	
;|	      0
;|	
;|	   Checksum
;|	
;|	      The checksum is the 16-bit ones's complement of the one's
;|	      complement sum of the ICMP message starting with the ICMP Type.
;|	      For computing the checksum , the checksum field should be zero.
;|	      If the total length is odd, the received data is padded with one
;|	      octet of zeros for computing the checksum.  This checksum may be
;|	      replaced in the future.
;|	
;|	   Identifier
;|	
;|	      If code = 0, an identifier to aid in matching echos and replies,
;|	      may be zero.
;|	
;|	   Sequence Number
;|	|	
;|	      If code = 0, a sequence number to aid in matching echos and
;|	      replies, may be zero.
;|	
;|	   Description
;|	
;|	      The data received in the echo message must be returned in the echo
;|	      reply message.
;|	
;|	      The identifier and sequence number may be used by the echo sender
;|	      to aid in matching the replies with the echo requests.  For
;|	      example, the identifier might be used like a port in TCP or UDP to
;|	      identify a session, and the sequence number might be incremented
;|	      on each echo request sent.  The echoer returns these same values
;|	      in the echo reply.
;|	
;|	      Code 0 may be received from a gateway or a host.
;|	
;|	
;|	Timestamp or Timestamp Reply Message
;|	
;|	    0                   1                   2                   3
;|	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Type      |      Code     |          Checksum             |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |           Identifier          |        Sequence Number        |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Originate Timestamp                                       |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Receive Timestamp                                         |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Transmit Timestamp                                        |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	
;|	   IP Fields:
;|	
;|	   Addresses
;|	
;|	      The address of the source in a timestamp message will be the
;|	      destination of the timestamp reply message.  To form a timestamp
;|	      reply message, the source and destination addresses are simply
;|	      reversed, the type code changed to 14, and the checksum
;|	      recomputed.
;|	
;|	   IP Fields:
;|	
;|	   Type
;|	
;|	      13 for timestamp message;
;|	
;|	      14 for timestamp reply message.
;|	
;|	   Code
;|	
;|	      0
;|	
;|	   Checksum
;|	
;|	      The checksum is the 16-bit ones's complement of the one's
;|	      complement sum of the ICMP message starting with the ICMP Type.
;|	      For computing the checksum , the checksum field should be zero.
;|	      This checksum may be replaced in the future.
;|	
;|	   Identifier
;|	
;|	
;|	      If code = 0, an identifier to aid in matching timestamp and
;|	      replies, may be zero.
;|	
;|	   Sequence Number
;|	
;|	      If code = 0, a sequence number to aid in matching timestamp and
;|	      replies, may be zero.
;|	
;|	   Description
;|	
;|	      The data received (a timestamp) in the message is returned in the
;|	      reply together with an additional timestamp.  The timestamp is 32
;|	      bits of milliseconds since midnight UT.  One use of these
;|	      timestamps is described by Mills [5].
;|	
;|	      The Originate Timestamp is the time the sender last touched the
;|	      message before sending it, the Receive Timestamp is the time the
;|	      echoer first touched it on receipt, and the Transmit Timestamp is
;|	      the time the echoer last touched the message on sending it.
;|	
;|	      If the time is not available in miliseconds or cannot be provided
;|	      with respect to midnight UT then any time can be inserted in a
;|	      timestamp provided the high order bit of the timestamp is also set
;|	      to indicate this non-standard value.
;|	
;|	      The identifier and sequence number may be used by the echo sender
;|	      to aid in matching the replies with the requests.  For example,
;|	      the identifier might be used like a port in TCP or UDP to identify
;|	      a session, and the sequence number might be incremented on each
;|	      request sent.  The destination returns these same values in the
;|	      reply.
;|	
;|	      Code 0 may be received from a gateway or a host.
;|	
;|	
;|	Information Request or Information Reply Message
;|	
;|	    0                   1                   2                   3
;|	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |     Type      |      Code     |          Checksum             |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	   |           Identifier          |        Sequence Number        |
;|	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;|	
;|	   IP Fields:
;|	
;|	   Addresses
;|	
;|	      The address of the source in a information request message will be
;|	      the destination of the information reply message.  To form a
;|	      information reply message, the source and destination addresses
;|	      are simply reversed, the type code changed to 16, and the checksum
;|	      recomputed.
;|	
;|	   IP Fields:
;|	
;|	   Type
;|	
;|	      15 for information request message;
;|	
;|	      16 for information reply message.
;|	
;|	   Code
;|	
;|	      0
;|	
;|	   Checksum
;|	
;|	      The checksum is the 16-bit ones's complement of the one's
;|	      complement sum of the ICMP message starting with the ICMP Type.
;|	      For computing the checksum , the checksum field should be zero.
;|	      This checksum may be replaced in the future.
;|	
;|	   Identifier
;|	
;|	      If code = 0, an identifier to aid in matching request and replies,
;|	      may be zero.
;|	
;|	   Sequence Number
;|	
;|	      If code = 0, a sequence number to aid in matching request and
;|	      replies, may be zero.
;|	
;|	
;|	   Description
;|	
;|	      This message may be sent with the source network in the IP header
;|	      source and destination address fields zero (which means "this"
;|	      network).  The replying IP module should send the reply with the
;|	      addresses fully specified.  This message is a way for a host to
;|	      find out the number of the network it is on.
;|	
;|	      The identifier and sequence number may be used by the echo sender
;|	      to aid in matching the replies with the requests.  For example,
;|	      the identifier might be used like a port in TCP or UDP to identify
;|	      a session, and the sequence number might be incremented on each
;|	      request sent.  The destination returns these same values in the
;|	      reply.
;|	
;|	      Code 0 may be received from a gateway or a host.
;|	
;|	
;|	Summary of Message Types
;|	
;|	    0  Echo Reply
;|	
;|	    3  Destination Unreachable
;|	
;|	    4  Source Quench
;|	
;|	    5  Redirect
;|	
;|	    8  Echo
;|	
;|	   11  Time Exceeded
;|	
;|	   12  Parameter Problem
;|	
;|	   13  Timestamp
;|	
;|	   14  Timestamp Reply
;|	
;|	   15  Information Request
;|	
;|	   16  Information Reply
;|	
;|	
;|	References
;|	
;|	   [1]  Postel, J. (ed.), "Internet Protocol - DARPA Internet Program
;|	         Protocol Specification," RFC 791, USC/Information Sciences
;|	         Institute, September 1981.
;|	
;|	   [2]   Cerf, V., "The Catenet Model for Internetworking," IEN 48,
;|	         Information Processing Techniques Office, Defense Advanced
;|	         Research Projects Agency, July 1978.
;|	
;|	   [3]   Strazisar, V., "Gateway Routing:  An Implementation
;|	         Specification", IEN 30, Bolt Beranek and Newman, April 1979.
;|	
;|	   [4]   Strazisar, V., "How to Build a Gateway", IEN 109, Bolt Beranek
;|	         and Newman, August 1979.
;|	
;|	   [5]   Mills, D., "DCNET Internet Clock Service," RFC 778, COMSAT
;|	         Laboratories, April 1981.
;|			RCALL	_SLIP_SET_TIMEOUT_ADDR						|
;|												|
;|	SLIP_SET_TX_BUFFER not necessary to be setted during initialization, only must be setted|
;|	before _SLIP_SEND function								|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_RX_BUFFER	SET SLIP RECEIVE DATA BUFFER FOR ADDRESS AND SIZE			|
;|												|
;|			ldiaw	SLIP_SIZE	;set SLIP data buffer size			|
;|			ldiw	Z,SLIP_BUFFER	;set SLIP data buffer pointer			|
;|			RCALL	_SLIP_SET_RX_BUFFER;SLIP initialize			 	|
;+----------------------------------------------------------------------------------------------+
;| _SLIP_GET_RX_BUFFER	GET SLIP RECEIVE DATA BUFFER ADDRESS AND SIZE				|
;|												|
;|			RCALL	_SLIP_GET_RX_BUFFER:get buffer size and pointer after this call	|
;|						;Z-->DATA BUFFER;AccH:Acc DATA BUFFER SIZE	|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_TX_BUFFER	SET SLIP TRANSMITTER DATA BUFFER FOR ADDRESS AND SIZE			|
;|												|
;|			ldiaw	SLIP_SIZE	;set SLIP data buffer size			|
;|			ldiw	Z,SLIP_BUFFER	;set SLIP data buffer pointer			|
;|			RCALL	_SLIP_SET_TX_BUFFER;SLIP initialize			 	|
;+----------------------------------------------------------------------------------------------+
;| _SLIP_GET_TX_BUFFER	GET SLIP TRANSMITTER DATA BUFFER ADDRESS AND SIZE			|
;|												|
;|			RCALL	_SLIP_GET_TX_BUFFER:get buffer size and pointer after this call	|
;|						;Z-->DATA BUFFER;AccH:Acc DATA BUFFER SIZE	|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_RX_ADDR	SET SLIP RX FUNCTION ADDDRESS						|
;|												|
;|			ldiw	Z,RX_FUNC_ADDR		;Z-->address of RX function that return	|
;|							;rx data into Acc register		|
;|			RCALL	_SLIP_SET_RX_ADDR	;					|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_TX_ADDR	SET SLIP TX FUNCTION ADDDRESS						|
;|												|
;|			ldiw	Z,TX_FUNC_ADDR		;Z-->address of TX function with data	|
;|							;to send must be into Acc register	|
;|			RCALL	_SLIP_SET_TX_ADDR	;					|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_TIMEOUT_ADDR	SET SLIP TIMEOUT FUNCTION ADDDRESS				|
;|												|
;|			ldiw	Z,TIMEOUT_FUNC_ADDR	;Z-->address of timeout function with 	|
;|							;timeout into AccH:Acc			|
;|			ldiaw	100			;settime out to 100ms			|
;|			RCALL	_SLIP_SET_TIMOUT_ADDR	;					|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SET_INDEX	SET SLIP DATA INDEX							|
;|												|
;|			LDIAW	50			;SET INDEX TO 50nd data			|
;|			RCALL	_SLIP_SET_INDEX		;SET POINTER				|
;|							;cy=1 if request pointer>SLIP buf size	|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_GET_INDEX	GET SLIP DATA INDEX							|
;|												|
;|			RCALL	_SLIP_GET_DATA_PTR	;AccH:Acc INDEX	VALUE			|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_GET_DATA	GET DATA FROM SLIP BUFFER INDEXED BY LAST DATA_PTR SETTING		|
;|												|
;|			RCALL	_SLIP_GET_DATA		;Acc DATA				|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_NEXT_DATA	POINT TO NEXT DATA INTO SLIP BUFFER					|
;|												|
;|			RCALL	_SLIP_NEXT_DATA		;POINT TO NEXT DATA			|
;|							;cy=1 if no more data			|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_POLLING	POLLING RX LINE FOR SLIP PACKETS					|
;|												|
;|			RCALL	_SLIP_POLLING		;Acc=_OK if received SLIP block ok	|
;|							;AccTH:AccT number of bytes received	|
;|							;Acc=_NOTOK if received SLIP block fail	|
;|							:AccTH:AccT number of bytes before fail	|
;|----------------------------------------------------------------------------------------------|
;| _SLIP_SEND		SEND DATA AT TX_BUFFER							|
;|												|
;|			RCALL	_SLIP_SEND		;send data into tx_buffer		|
;+----------------------------------------------------------------------------------------------+
#ifndef __ICMP_PROTOCOL__
	#define __ICMP_PROTOCOL__
	.message "+---------------+"
	.message "|               |"
	.message "| ICMP PROTOCOL |"
	.message "|               |"
	.message "+---------------+"
	;+------------------------------+
	;|				|
	;| NECESSARY INCLUDE FILES	|
	;|				|
	;+------------------------------+
	#ifndef	__CHECKSUM16__
		.INCLUDE	"MATH\INTEGER\CHECKSUM16\CHECKSUM16.INC"
	#endif
;				   //////////////////////\\\\\\\\\\\\\\\\\\\\\\\
;				 //						\\
;-------------------------------||	       I C M P  P R O T O C O L  	||-----------------------------------
;				 \\						//
;				  \\\\\\\\\\\\\\\\\\\\\\\///////////////////////
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| DESCRIPTION:This object perform all resorces for implementa- |
	;|	       tion of ICMP PROTOCOL				|
	;>--------------------------------------------------------------<
	;>----------------------<
	;| OBJECT DEFINITIONS	|
	;>----------------------<
	;-------------------------< CONSTANT DEFINITONS >------------------------
	;-------------------------< VARIABLES DEFINITONS >------------------------
	.DSEG
		_ICMP_HEADER_ADDR:	.BYTE 2				;HOLD A IP HEADER ADDRESS
	.CSEG
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_SET_HEADER					|
	;>--------------------------------------------------------------<
	;| PERFORM    : Set ICMP Header address				|
	;>--------------------------------------------------------------<
	;| INPUT      :	Z --> ICMPP address				|
	;| OUTPUT     :	None						|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_SET_HEADER:
		stsw	_ICMP_HEADER_ADDR,Z				;store 
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMPP_GET_HEADER				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Get ICMP Header address				|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Z --> ICMP address				|
	;| DESTROYED  :	None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_GET_HEADER:
		ldsw	Z,_ICMP_HEADER_ADDR				;restore
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_GET_TYPE					|
	;>--------------------------------------------------------------<
	;| PERFORM    : Get ICMP Type from ICMP header			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc Type					|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_GET_TYPE:
		pushw	Z						;save Z
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		ld	Acc,Z						;get type
		;+---------------------------------
		;| RESTORE Z REGISTER AND RETURN
		;+---------------------------------
	_ICMP_REST_Z:
		popw	Z
		ret
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_SET_TYPE					|
	;>--------------------------------------------------------------<
	;| PERFORM    : Set ICMP Type on ICMP header			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc Type					|
	;| OUTPUT     :	None						|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_SET_TYPE:
		pushw	Z						;save Z
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		st	Z,Acc						;store type
		rjmp	_ICMP_REST_Z
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_GET_CODE					|
	;>--------------------------------------------------------------<
	;| PERFORM    : Get ICMP Code from ICMP header			|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	Acc Code					|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_GET_CODE:
		pushw	Z						;save Z
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		ldd	Acc,Z+1						;get type
		rjmp	_ICMP_REST_Z
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_SET_CODE					|
	;>--------------------------------------------------------------<
	;| PERFORM    : Set ICMP Code on ICMP header			|
	;>--------------------------------------------------------------<
	;| INPUT      :	Acc Code					|
	;| OUTPUT     :	None						|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_SET_CODE:
		pushw	Z						;save Z
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		std	Z+1,Acc						;store type
		rjmp	_ICMP_REST_Z
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_GET_IDENTIFIER				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Get ICMP Identifier from ICMP Header		|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	AccH:Acc Identifier				|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_GET_IDENTIFIER:
		pushw	Z						;save Z
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		ldd	Acc,Z+4						;get Identifier low byte
		ldd	AccH,Z+5					;get Identifier high byte
		rjmp	_IP_REST_Z					;restore Z and return
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_SET_IDENTIFIER				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Set ICMP Identifier on ICMP Header		|
	;>--------------------------------------------------------------<
	;| INPUT      :	AccH:Acc Identifier				|
	;| OUTPUT     :	None						|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_SET_IDENTIFIER:
		pushw	Z
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		std	Z+4,Acc						;store Identifier byte low
		std	Z+5,AccH					;store Identifier byte high
		rjmp	_IP_REST_Z					;restore Z and return
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_GET_SEQUENCE_NUMBER			|
	;>--------------------------------------------------------------<
	;| PERFORM    : Get ICMP Sequence number from ICMP Header	|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	AccH:Acc Sequence				|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_GET_SEQUENCE_NUMBER:
		pushw	Z						;save Z
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		ldd	Acc,Z+6						;get Sequence number low byte
		ldd	AccH,Z+7					;get seguence number high byte
		rjmp	_IP_REST_Z					;restore Z and return
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_SET_SEQUENCE_NUMBER			|
	;>--------------------------------------------------------------<
	;| PERFORM    : Set ICMP sequence number on ICMP Header		|
	;>--------------------------------------------------------------<
	;| INPUT      :	AccH:Acc sequence number			|
	;| OUTPUT     :	None						|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_SET_SEQUENCE_NUMBER:
		pushw	Z
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		std	Z+6,Acc						;store sequence number byte low
		std	Z+7,AccH					;store sequence number byte high
		rjmp	_IP_REST_Z					;restore Z and return
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_GET_HEADER_CHECKSUM			|
	;>--------------------------------------------------------------<
	;| PERFORM    : Get ICMP Header Checksum from ICMP Header	|
	;>--------------------------------------------------------------<
	;| INPUT      :	None						|
	;| OUTPUT     :	AccH:Acc Header Checksum			|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_GET_HEADER_CHECKSUM:
		pushw	Z						;save Z
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		ldd	Acc,Z+2						;get header checksum low byte
		ldd	AccH,Z+3					;get header checksum high byte
		rjmp	_IP_REST_Z					;restore Z and return
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_SET_HEADER_CHECKSUM			|
	;>--------------------------------------------------------------<
	;| PERFORM    : Set ICMP Header Checksum on ICMP Header		|
	;>--------------------------------------------------------------<
	;| INPUT      :	AccH:Acc Header Checksum			|
	;| OUTPUT     :	None						|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_SET_HEADER_CHECKSUM:
		pushw	Z
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		std	Z+2,Acc						;store header checksum byte low
		std	Z+3,AccH					;store header checksum byte high
		rjmp	_IP_REST_Z					;restore Z and return
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_GET_CHECKSUM				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Verify if checksum is valid			|
	;>--------------------------------------------------------------<
	;| INPUT      :	AccH:Acc number of byte in ICMP header		|
	;| OUTPUT     :	AccH:Acc checksum value 0xffff if correct	|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_GET_CHECKSUM:
		pushw	Z
		pushw	X
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		rcall	_CHECKSUM16					;compute checksum
		popw	X
		rjmp	_IP_REST_Z					;restore Z and return
	;>--------------------------------------------------------------<
	;| OBJECT     : ICMP						|
	;>--------------------------------------------------------------<
	;| FUNCTION   : ICMP_SET_CHECKSUM				|
	;>--------------------------------------------------------------<
	;| PERFORM    : Compute new checksum and put into field		|
	;|		CHECKSUM					|
	;>--------------------------------------------------------------<
	;| INPUT      : AccH:Acc number of words in ICMP header		|
	;| OUTPUT     :	None						|
	;| DESTROYED  : None						|
	;| TIME	      :	----						|
	;>--------------------------------------------------------------<
	;| OBSERVATION:							|
	;+--------------------------------------------------------------+-------------------------------------------------------------------------------\
	;|																		|
	_ICMP_SET_CHECKSUM:
		pushw	Z
		pushw	X
		push	Acc
		push	AccH
		clr	Acc
		clr	AccH
		rcall	_ICMP_SET_HEADER_CHECKSUM			;clear old checksum
		pop	AccH
		pop	Acc
		push	Acc
		push	AccH
		ldsw	Z,_ICMP_HEADER_ADDR				;Z-->ICMP HEADER
		rcall	_CHECKSUM16					;compute checksum
		com	Acc						;complemente result
		com	AccH
		rcall	_ICMP_SET_HEADER_CHECKSUM			;set new value checksum
		pop	AccH
		pop	Acc
		popw	X
		rjmp	_IP_REST_Z					;restore Z and return		
#endif

