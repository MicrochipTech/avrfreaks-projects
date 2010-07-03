#ifndef _PACKET_SLAVE_INCLUDED_
#define _PACKET_SLAVE_INCLUDED_

#include <stdint.h>
#include <stdbool.h>

#define     MAXIMUM_PACKET_PAYLOAD_LENGTH   14

typedef struct
_Packet
    {
        uint8_t checksum;
        uint8_t length; /*!< Payload length. */
        uint8_t payload [MAXIMUM_PACKET_PAYLOAD_LENGTH];
    }
Packet;

#pragma used+
	bool packet_check (Packet* packet);
	bool packet_compute_header (Packet* packet, uint8_t length);
	bool packet_send (Packet* packet);
	bool packet_receive (Packet* packet);
#pragma used-

#endif