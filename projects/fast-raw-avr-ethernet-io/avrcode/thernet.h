#ifndef ETHERNET_H
#define ETHERNET_H

//------------------------------------------------------------------------------
// Ethernet packet defines
//------------------------------------------------------------------------------

/// field or packet lengths
#define ETH_HEADER_LEN	14
#define ETH_CHECKSUM_LEN 4
#define ETH_ENVELOPE_LEN (ETH_HEADER_LEN + ETH_CHECKSUM_LEN)
#define ETH_PAYLOAD_MIN 46
#define ETH_PAYLOAD_MAX 1500
#define ETH_PACKET_MIN 64
#define ETH_PACKET_MAX 1518

/// field locations in ethernet packet
#define ETH_DST_MAC_P 0
#define ETH_SRC_MAC_P 6
#define ETH_TYPE_H_P 12
#define ETH_TYPE_L_P 13


#endif
