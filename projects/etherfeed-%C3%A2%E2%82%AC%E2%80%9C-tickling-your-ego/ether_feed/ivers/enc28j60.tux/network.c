#include "global-conf.h"
#include "enc28j60.h"
#include <avr/io.h>
#include <util/delay.h>

#include "avrlibtypes.h"

unsigned int network_read(void)
{
	return ((uint16_t) enc28j60PacketReceive(UIP_BUFSIZE, (uint8_t *)uip_buf));
}

void network_send(void)
{
    enc28j60PacketSend(uip_len, (uint8_t *)uip_buf);
}

void network_init_mac(u08* macaddr)
{
    //Initialise the device
    enc28j60Init(macaddr);
}

void network_get_MAC(u08* macaddr)
{
    // read MAC address registers
    // NOTE: MAC address in ENC28J60 is byte-backward
    *macaddr++ = enc28j60Read(MAADR5);
    *macaddr++ = enc28j60Read(MAADR4);
    *macaddr++ = enc28j60Read(MAADR3);
    *macaddr++ = enc28j60Read(MAADR2);
    *macaddr++ = enc28j60Read(MAADR1);
    *macaddr++ = enc28j60Read(MAADR0);
}
