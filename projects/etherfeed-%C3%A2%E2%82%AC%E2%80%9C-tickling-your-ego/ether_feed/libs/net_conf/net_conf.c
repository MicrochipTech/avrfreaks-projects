#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/eeprom.h>

#include <net_conf.h>

#include <uip.h>
#include "uip_arp.h"
#include <network.h>

#include <ctype.h>



//EEPROM parameters (TCP/IP parameters)
uint8_t EEMEM ee_enable_dhcp=USE_DHCP;

uint8_t EEMEM ee_eth_addr[6]=
   {UIP_ETHADDR0,UIP_ETHADDR1,UIP_ETHADDR2,UIP_ETHADDR3,UIP_ETHADDR4,UIP_ETHADDR5};

uint8_t EEMEM ee_ip_addr[4]= 
      {UIP_IPADDR0, UIP_IPADDR1, UIP_IPADDR2, UIP_IPADDR3};

uint8_t EEMEM ee_net_mask[4]= 
        {UIP_NETMASK0, UIP_NETMASK1, UIP_NETMASK2, UIP_NETMASK3};

uint8_t EEMEM ee_gateway[4]=
        {UIP_DRIPADDR0, UIP_DRIPADDR1, UIP_DRIPADDR2, UIP_DRIPADDR3};

// TCP/IP parameters in data memory
uint8_t net_conf_enable_dhcp;
uint8_t net_conf_eth_addr[6];
uint8_t net_conf_ip_addr[4];
uint8_t net_conf_net_mask[4];
uint8_t net_conf_gateway[4];
uint8_t net_conf_four_bytes[4];

static struct uip_eth_addr  my_eth_addr = { .addr = {UIP_ETHADDR0,UIP_ETHADDR1,
                                                     UIP_ETHADDR2,UIP_ETHADDR3,
												     UIP_ETHADDR4,UIP_ETHADDR5}};
bool init_load_done = 0;

void net_conf_uip_set(void)
{
    uip_ipaddr_t ipaddr;

    uip_ipaddr(ipaddr, net_conf_ip_addr[0], net_conf_ip_addr[1],
               net_conf_ip_addr[2], net_conf_ip_addr[3]);
    uip_sethostaddr(ipaddr);
    uip_ipaddr(ipaddr, net_conf_gateway[0], net_conf_gateway[1],
               net_conf_gateway[2], net_conf_gateway[3]);
    uip_setdraddr(ipaddr);
    uip_ipaddr(ipaddr, net_conf_net_mask[0], net_conf_net_mask[1],
               net_conf_net_mask[2], net_conf_net_mask[3]);
    uip_setnetmask(ipaddr);
}


int net_conf_init(void)
{
    uip_ipaddr_t ipaddr;

    if (!init_load_done)
    {
	net_conf_load();
    }
    init_load_done = 1;

//net_conf_enable_dhcp=0;

    if ((net_conf_enable_dhcp != 1) &&
		(net_conf_enable_dhcp != 0))
    {   // if the setting is invalid, enable by default
#if UIP_CONF_BROADCAST == 1
        net_conf_enable_dhcp = 1;
#else
		net_conf_enable_dhcp = 0;
#endif
		// update the eeprom with the correct data
		net_conf_save();
    }

    // if the mac address in eeprom looks bad, use the defaults
    if(net_conf_eth_addr[0] == 0xff)
    {
		net_conf_eth_addr[0] = UIP_ETHADDR0;
		net_conf_eth_addr[1] = UIP_ETHADDR1;
		net_conf_eth_addr[2] = UIP_ETHADDR2;
		net_conf_eth_addr[3] = UIP_ETHADDR3;
		net_conf_eth_addr[4] = UIP_ETHADDR4;
		net_conf_eth_addr[5] = UIP_ETHADDR5;
		net_conf_mac_save();
	}

	my_eth_addr.addr[0] = net_conf_eth_addr[0];
	my_eth_addr.addr[1] = net_conf_eth_addr[1];
	my_eth_addr.addr[2] = net_conf_eth_addr[2];
	my_eth_addr.addr[3] = net_conf_eth_addr[3];
	my_eth_addr.addr[4] = net_conf_eth_addr[4];
	my_eth_addr.addr[5] = net_conf_eth_addr[5];

	uip_setethaddr(my_eth_addr);

    if (!net_conf_enable_dhcp)
    {
        // if the IP looks good in flash, use it
        if ((net_conf_ip_addr[0] != 255) &&
			(net_conf_ip_addr[0] != 0))
        {
            net_conf_uip_set();
        }
        else
        {
			// ip in flash didn't look good... use default
            uip_ipaddr(ipaddr, UIP_IPADDR0, UIP_IPADDR1,
			                   UIP_IPADDR2, UIP_IPADDR3);
            uip_sethostaddr(ipaddr);
			net_conf_ip_addr[0] = UIP_IPADDR0;
			net_conf_ip_addr[1] = UIP_IPADDR1;
			net_conf_ip_addr[2] = UIP_IPADDR2;
			net_conf_ip_addr[3] = UIP_IPADDR3;

            uip_ipaddr(ipaddr, UIP_DRIPADDR0, UIP_DRIPADDR1,
			                   UIP_DRIPADDR2, UIP_DRIPADDR3);
            uip_setdraddr(ipaddr);
			net_conf_gateway[0] = UIP_DRIPADDR0;
			net_conf_gateway[1] = UIP_DRIPADDR1;
			net_conf_gateway[2] = UIP_DRIPADDR2;
			net_conf_gateway[3] = UIP_DRIPADDR3;

            uip_ipaddr(ipaddr, UIP_NETMASK0, UIP_NETMASK1,
			                   UIP_NETMASK2, UIP_NETMASK3);
            uip_setnetmask(ipaddr);
			net_conf_net_mask[0] = UIP_NETMASK0;
			net_conf_net_mask[1] = UIP_NETMASK1;
			net_conf_net_mask[2] = UIP_NETMASK2;
			net_conf_net_mask[3] = UIP_NETMASK3;

			// update the eeprom with the correct data
			net_conf_save();
        }
    }

	return 0;
}

//
// Take the given string of the format
// 192.168.1.99 or 00:11:22:33:44:55
uint8_t network_string_to_byte_array(char *net_string,
                                     uint8_t *byte_array,
									 uint8_t byte_array_len)
{
    char *to_convert = net_string;
	char *the_end;
	uint8_t eindex = strlen(net_string);
	uint8_t windex;
	uint8_t bindex = 0;
    uint8_t base = 10;

    for (windex = 0; windex != eindex; windex++)
	{
        if ((net_string[windex] == '.') || ((net_string[windex] == ':') && (base = 16)))
		{
			net_string[windex] = '\0';
            byte_array[bindex] = (uint8_t) strtol(to_convert, &the_end, base);
			bindex++;
			to_convert = &net_string[windex + 1];

			// if nothing was converted to a value
			// if the index is == to the length we are one byte over the array
			if ((the_end == to_convert) ||
				(bindex == byte_array_len))
			{
                return (1);
			}
		}
	}

    byte_array[bindex] = (uint8_t) strtol(to_convert, NULL, base);
	return (0);
}


uint8_t *net_conf_get_ip (void)
{
    return (net_conf_ip_addr);
}

void net_conf_set_ip (uint8_t *new_ip)
{
    memcpy(net_conf_ip_addr, new_ip, 4);
}

void net_conf_ipaddr_to_bytes(const uip_ipaddr_t addr, uint8_t *net_conf_four_bytes)
{
    net_conf_four_bytes[0] = 0x00FF & (addr[0]);
    net_conf_four_bytes[1] = (addr[0]) >> 8;
    net_conf_four_bytes[2] = 0x00FF & (addr[1]);
    net_conf_four_bytes[3] = (addr[1]) >> 8;
}

void net_conf_set_ip_ipaddr(const uip_ipaddr_t addr)
{
    net_conf_ipaddr_to_bytes(addr, net_conf_four_bytes);
    net_conf_set_ip(net_conf_four_bytes);
}

int8_t net_conf_get_ip_string (char* ip_string, int8_t ip_string_len)
{
	return(snprintf(ip_string, ip_string_len, "%d.%d.%d.%d",
			net_conf_ip_addr[0], net_conf_ip_addr[1],
			net_conf_ip_addr[2], net_conf_ip_addr[3]));
}

// 
uint8_t net_conf_set_ip_string (char *ip_string)
{
    //return network_string_to_byte_array("192.168.2.88", net_conf_ip_addr, 4);    
    return network_string_to_byte_array(ip_string, net_conf_ip_addr, 4);    
}


uint8_t *net_conf_get_gw (void)
{
    return(net_conf_gateway);
}

void net_conf_set_gw (uint8_t* new_gw)
{
    memcpy(net_conf_gateway, new_gw, 4);
}

void net_conf_set_gw_ipaddr(const uip_ipaddr_t addr)
{
    net_conf_ipaddr_to_bytes(addr, net_conf_four_bytes);
/*    net_conf_four_bytes[0] = 0x00FF & (addr[0]);
    net_conf_four_bytes[1] = (addr[0]) >> 8;
    net_conf_four_bytes[2] = 0x00FF & (addr[1]);
    net_conf_four_bytes[3] = (addr[1]) >> 8;
*/
    net_conf_set_gw(net_conf_four_bytes);
}

int8_t net_conf_get_gw_string (char* gw_string, int8_t gw_string_len)
{
	return(snprintf(gw_string, gw_string_len, "%d.%d.%d.%d",
			net_conf_gateway[0], net_conf_gateway[1],
			net_conf_gateway[2], net_conf_gateway[3]));
}

uint8_t net_conf_set_gw_string (char* gw_string)
{
    //return network_string_to_byte_array(gw_string, net_conf_gateway, 4);
   return network_string_to_byte_array(gw_string, net_conf_gateway, 4);
//	return (strlen(gw_string));
//	net_conf_gateway[3] = 4;
}



uint8_t *net_conf_get_nm (void)
{
    return net_conf_net_mask;
}

void net_conf_set_nm (uint8_t *new_nm)
{
    memcpy(net_conf_net_mask, new_nm, 4);
}

void net_conf_set_nm_ipaddr(const uip_ipaddr_t addr)
{
    net_conf_ipaddr_to_bytes(addr, net_conf_four_bytes);
/*    net_conf_four_bytes[0] = 0x00FF & (addr[0]);
    net_conf_four_bytes[1] = (addr[0]) >> 8;
    net_conf_four_bytes[2] = 0x00FF & (addr[1]);
    net_conf_four_bytes[3] = (addr[1]) >> 8;
*/
    net_conf_set_nm(net_conf_four_bytes);
}

int8_t net_conf_get_nm_string (char* nm_string, int8_t nm_string_len)
{
	return(snprintf(nm_string, nm_string_len, "%d.%d.%d.%d",
			net_conf_net_mask[0], net_conf_net_mask[1],
			net_conf_net_mask[2], net_conf_net_mask[3]));
}

uint8_t net_conf_set_nm_string (char *nm_string)
{
    return network_string_to_byte_array(nm_string, net_conf_net_mask, 4);
}


uint8_t *net_conf_get_mac (void)
{
    return net_conf_eth_addr;
}

void net_conf_set_mac (uint8_t *new_eth_addr)
{
	memcpy(net_conf_eth_addr, new_eth_addr, 6);
}

int8_t net_conf_get_mac_string (char* mac_string, int8_t mac_string_len)
{
	return(snprintf(mac_string, mac_string_len, "%02x:%02x:%02x:%02x:%02x:%02x",
			net_conf_eth_addr[0], net_conf_eth_addr[1],
			net_conf_eth_addr[2], net_conf_eth_addr[3],
			net_conf_eth_addr[4], net_conf_eth_addr[5]));
}

uint8_t net_conf_set_mac_string (char *mac_string)
{
	return network_string_to_byte_array(mac_string, net_conf_eth_addr, 6);
}


// 0 == off , 1 == on 
void net_conf_set_dhcpc(uint8_t mode)
{
	if ((mode == 0) || (mode == 1))
	{
        net_conf_enable_dhcp = mode;
	}
}


uint8_t net_conf_set_dhcpc_string(char *mode)
{
	if (strncmp("on", mode, 2) == 0)
	{
		net_conf_set_dhcpc(1);
		return (0);
	}
	else if (strncmp("of", mode, 2) == 0)
	{
		net_conf_set_dhcpc(0);
		return (0);
	}

	return (1);
}


// is the system in dhcp mode
// 0 == no, 1 == true
uint8_t net_conf_is_dhcpc(void)
{
    return (net_conf_enable_dhcp);
}

void net_conf_save(void)
{
// update functions write if data is different
#if defined(eeprom_update_block)
    // added to avr-lib @ version 1.6.7
    eeprom_update_block ((const void *)net_conf_ip_addr, (void *)&ee_ip_addr,4);
    eeprom_update_block ((const void *)net_conf_net_mask,(void *)&ee_net_mask,4);
    eeprom_update_block ((const void *)net_conf_gateway, (void *)&ee_gateway,4); 
	eeprom_update_byte  (&ee_enable_dhcp, net_conf_enable_dhcp);
#else
    eeprom_write_block ((const void *)net_conf_ip_addr, (void *)&ee_ip_addr,4);
    eeprom_write_block ((const void *)net_conf_net_mask,(void *)&ee_net_mask,4);
    eeprom_write_block ((const void *)net_conf_gateway, (void *)&ee_gateway,4); 
    eeprom_write_block ((const void *)net_conf_gateway, (void *)&ee_gateway,4); 
	eeprom_write_byte  (&ee_enable_dhcp, net_conf_enable_dhcp);
#endif
	// note we don't write the mac here.
}


// split out saving the mac address since it shouldn't be changed
void net_conf_mac_save(void)
{
    eeprom_write_block ((const void *)net_conf_eth_addr, (void *)&ee_eth_addr,6);
}


void net_conf_load(void)
{
    eeprom_read_block ((void *)net_conf_ip_addr, (const void *)&ee_ip_addr,4);
    eeprom_read_block ((void *)net_conf_net_mask, (const void *)&ee_net_mask,4);
    eeprom_read_block ((void *)net_conf_gateway,(const void *)&ee_gateway,4);
    net_conf_enable_dhcp = eeprom_read_byte(&ee_enable_dhcp);

    // load the mac here
    eeprom_read_block ((void *)net_conf_eth_addr, (const void *)&ee_eth_addr,6);
}
