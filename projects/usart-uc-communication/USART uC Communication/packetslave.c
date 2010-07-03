/**
   @author ErikDT,vektor dewanto
   @ver Jan 10
*/
#include "packetSlave.h"
#include <mega8.h>

bool packet_check (Packet* packet)
    {
        uint8_t i;
        uint8_t sum = 0;
        uint8_t len = packet->length;
        if (len > MAXIMUM_PACKET_PAYLOAD_LENGTH)
            {
                return false;
            }
        for (i = 0; i < len + 2; ++i)
            {
                sum += ((uint8_t*) packet)[i];
            }
        return sum == 0;
    }
bool packet_compute_header (Packet* packet, uint8_t length)
    {
        uint8_t sum = length;
        uint8_t i;
        if (length > MAXIMUM_PACKET_PAYLOAD_LENGTH)
            {
                return false;
            }
        packet->length = length;
        for (i = 0; i < length; ++i)
            {
                sum += packet->payload [i];
            }
        packet->checksum = -sum;
        return true;
    }
bool packet_send (Packet* packet)
    {
        if (packet_check (packet))
            {
                uint8_t num = packet->length + 2;
                uint8_t i;
                for (i = 0; i < num; ++i)
                    {
                        putchar (((uint8_t*)packet)[i]);//depends on USARTn used
                    }
                return true;
            }
        return false;
    }

/*!
 *  \param  length  Payload length.
 */
bool packet_receive (Packet* packet)
    {
        uint8_t i;
        packet->checksum = getchar (); //depends on USARTn used
        packet->length = getchar (); //depends on USARTn used
        if (packet->length > MAXIMUM_PACKET_PAYLOAD_LENGTH)
            {
                return false;
            }
        for (i = 0; i < packet->length; ++i)
            {
                packet->payload[i] = getchar ();//depends on USARTn used
            }
        return packet_check (packet);
    }
