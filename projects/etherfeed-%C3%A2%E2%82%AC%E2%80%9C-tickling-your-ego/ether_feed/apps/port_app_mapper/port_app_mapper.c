#include "uip.h"
#include "uiplib.h"
#include <string.h>
#include "port_app_mapper.h"
#include "global-conf.h"


#ifndef WEBSERVER_APP_CALL_MAP
#define WEBSERVER_APP_CALL_MAP
#endif

#ifndef WEBCLIENT_APP_CALL_MAP
#define WEBCLIENT_APP_CALL_MAP
#endif

#ifndef TELNET_APP_CALL_MAP
#define TELNET_APP_CALL_MAP
#endif

#ifndef SIMPLE_HTTPD_APP_CALL_MAP
#define SIMPLE_HTTPD_APP_CALL_MAP
#endif

#ifndef DHCPC_APP_CALL_MAP
#define DHCPC_APP_CALL_MAP
#endif

#ifndef RESOLV_APP_CALL_MAP
#define RESOLV_APP_CALL_MAP
#endif

#ifndef NTPCLIENT_APP_CALL_MAP
#define NTPCLIENT_APP_CALL_MAP
#endif


struct port_appcall_map tcp_port_app_map[] = {
    WEBSERVER_APP_CALL_MAP
    WEBCLIENT_APP_CALL_MAP
    TELNET_APP_CALL_MAP
    SIMPLE_HTTPD_APP_CALL_MAP
    {NULL, 0, 0},
};

struct port_appcall_map udp_port_app_map[] = {
    DHCPC_APP_CALL_MAP
    RESOLV_APP_CALL_MAP
    NTPCLIENT_APP_CALL_MAP
    {NULL, 0, 0},
};

// define a basic common type for udp and tcp connections so I can look at the ports
struct uip_base_conn {
  uip_ipaddr_t ripaddr;   /**< The IP address of the remote host. */

  u16_t lport;        /**< The local port, in network byte order. */
  u16_t rport; 
};

struct uip_base_conn *base_conn;


// this function maps the current packet against the mappings
//   pointed to by the param
void uip_port_app_mapper(struct port_appcall_map* cur_map)
{

    // if dhcpc is enabled and running we want our ip ASAP so skip all others
    if(dhcpc_running && uip_poll())
    {
        dhcpc_appcall();
        base_conn = NULL;
        return;    
    }

   if(resolv_running && uip_poll())
    {
        resolv_appcall();
        base_conn = NULL;
        return;    
    }

    // yes this will walk the entire list which is up to 4 items at the moment
    while ((base_conn != NULL) && (cur_map->an_appcall != NULL))
    {
        // Now match the app to the packet.
        // local AND/OR remote ports match
        // firs check remote port matches and local may or may not
        // then check local port matches
        // can't do it in one statement due to the l & r ports could both be zero and match all apps.
        if (((base_conn->rport == HTONS(cur_map->rport)) &&
            ((cur_map->lport == 0) || (base_conn->lport == HTONS(cur_map->lport)))) ||
            ((base_conn->lport == HTONS(cur_map->lport)) &&
             ((cur_map->rport == 0) || (base_conn->rport == HTONS(cur_map->rport)))))
        {
            cur_map->an_appcall();
            base_conn = NULL;
            break;
        }
        cur_map++;
    }
}


// wrapper around the uip_port_app_mapper
// pass it the list of apps to match against
void uip_udp_appcall_port_app_mapper()
{
    base_conn = (struct uip_base_conn *) uip_udp_conn;
    uip_port_app_mapper(udp_port_app_map);
}

void uip_appcall_port_app_mapper()
{
    base_conn = (struct uip_base_conn *) uip_conn;
    uip_port_app_mapper(tcp_port_app_map);
}
