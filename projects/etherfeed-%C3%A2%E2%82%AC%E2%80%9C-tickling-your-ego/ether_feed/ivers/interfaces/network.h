/*
 * Simple common network interface that all network drivers should implement.
 */

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "avrlibtypes.h"

/*Initialize the network*/
void network_init(void);

/*Initialize the network with a mac addr*/
void network_init_mac(u08* macaddr);

/*Read from the network, returns number of read bytes*/
unsigned int network_read(void);

/*Send using the network*/
void network_send(void);

/*Sets the MAC address of the device*/
void network_set_MAC(uint8_t* mac);

/*Gets the MAC address of the device*/
void network_get_MAC(uint8_t* mac);

#endif /* __NETWORK_H__ */
