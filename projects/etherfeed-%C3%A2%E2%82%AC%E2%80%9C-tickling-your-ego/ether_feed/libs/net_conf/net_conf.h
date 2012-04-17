//Project specific configurations
#ifndef __NET_CONF_H__
#define __NET_CONF_H__

//Mac adress definition for enc28j60
#define ETHADDR0		0x00
#define ETHADDR1		0xbd
#define ETHADDR2		0x3b
#define ETHADDR3		0x33
#define ETHADDR4		0x05
#define ETHADDR5		0x71
//Mac adress definition for uip
#define UIP_ETHADDR0    ETHADDR0
#define UIP_ETHADDR1    ETHADDR1
#define UIP_ETHADDR2    ETHADDR2
#define UIP_ETHADDR3    ETHADDR3
#define UIP_ETHADDR4    ETHADDR4
#define UIP_ETHADDR5    ETHADDR5

#define USE_DHCP 0
#define UIP_IPADDR0 192
#define UIP_IPADDR1 168
#define UIP_IPADDR2 2
#define UIP_IPADDR3 77
#define UIP_NETMASK0 255
#define UIP_NETMASK1 255
#define UIP_NETMASK2 255
#define UIP_NETMASK3 0
#define UIP_DRIPADDR0 192
#define UIP_DRIPADDR1 168
#define UIP_DRIPADDR2 2
#define UIP_DRIPADDR3 1

#include "uip.h"
//
// ** do this include instead of all of uip.h **
//
#if 0
#include "uipopt.h"
typedef u16_t uip_ip4addr_t[2];
typedef u16_t uip_ip6addr_t[8];
#if UIP_CONF_IPV6
typedef uip_ip6addr_t uip_ipaddr_t;
#else /* UIP_CONF_IPV6 */
typedef uip_ip4addr_t uip_ipaddr_t;
#endif /* UIP_CONF_IPV6 */
#endif
//
// ** do this include instead of all of uip.h **
//


// function protos
int net_conf_init(void);
void net_conf_uip_set(void);
uint8_t network_string_to_byte_array(char *net_string,
                                     uint8_t byte_array[],
									 uint8_t byte_array_len);
uint8_t *net_conf_get_ip (void);
void net_conf_set_ip (uint8_t *new_ip);
void net_conf_set_ip_ipaddr(const uip_ipaddr_t addr);
int8_t net_conf_get_ip_string (char* ip_string, int8_t ip_string_len);
uint8_t net_conf_set_ip_string (char *ip_string);

uint8_t *net_conf_get_gw (void);
void net_conf_set_gw (uint8_t* new_gw);
void net_conf_set_gw_ipaddr(const uip_ipaddr_t addr);
int8_t net_conf_get_gw_string (char* gw_string, int8_t gw_string_len);
uint8_t net_conf_set_gw_string (char* gw_string);

uint8_t *net_conf_get_nm (void);
void net_conf_set_nm (uint8_t *new_nm);
void net_conf_set_nm_ipaddr(const uip_ipaddr_t addr);
int8_t net_conf_get_nm_string (char* nm_string, int8_t nm_string_len);
uint8_t net_conf_set_nm_string (char *nm_string);

uint8_t *net_conf_get_mac (void);
void net_conf_set_mac (uint8_t *new_eth_addr);
int8_t net_conf_get_mac_string (char* mac_string, int8_t mac_string_len);
uint8_t net_conf_set_mac_string (char *mac_string);

void net_conf_set_dhcpc(uint8_t mode);
uint8_t net_conf_set_dhcpc_string(char *mode);
uint8_t net_conf_is_dhcpc(void);

void net_conf_save(void);
void net_conf_load(void);
void net_conf_mac_save(void);


#endif /*__NET_CONF_H__*/
