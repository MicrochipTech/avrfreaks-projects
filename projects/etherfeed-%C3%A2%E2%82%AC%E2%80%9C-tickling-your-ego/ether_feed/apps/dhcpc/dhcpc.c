// NOTE: fixed bug as per http://www.mail-archive.com/uip-users@sics.se/msg00003.html
/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack
 *
 * @(#)$Id: dhcpc.c,v 1.2 2006/06/11 21:46:37 adam Exp $
 */

#include <stdio.h>
#include <string.h>

#include "uip.h"
#include "timer.h"
#include "pt.h"
#include "dhcpc.h"

#if defined PORT_APP_MAPPER
bool dhcpc_running = 0; 
#endif

// check that the uip buffer is large enough for dhcp to work.
#if UIP_CONF_BUFFER_SIZE < 590
  #error "DHCP will not work without a large enough buffer.  Set UIP_CONF_BUFFER_SIZE > 590 bytes"
#endif

#define STATE_INITIAL         0
#define STATE_SENDING         1
#define STATE_OFFER_RECEIVED  2
#define STATE_CONFIG_RECEIVED 3
#define STATE_RENEW           4
#define STATE_FAIL            5

static struct dhcpc_state s;

struct dhcp_msg {
  u8_t op, htype, hlen, hops;
  u8_t xid[4];
  u16_t secs, flags;
  u8_t ciaddr[4];
  u8_t yiaddr[4];
  u8_t siaddr[4];
  u8_t giaddr[4];
  u8_t chaddr[16];
#ifndef UIP_CONF_DHCP_LIGHT
  u8_t sname[64];
  u8_t file[128];
#endif
  u8_t options[312];
};

#define BOOTP_BROADCAST 0x8000

#define DHCP_REQUEST        1
#define DHCP_REPLY          2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET  6
#define DHCP_MSG_LEN      236

#define DHCPC_SERVER_PORT  67
#define DHCPC_CLIENT_PORT  68

#define DHCPDISCOVER  1
#define DHCPOFFER     2
#define DHCPREQUEST   3
#define DHCPDECLINE   4
#define DHCPACK       5
#define DHCPNAK       6
#define DHCPRELEASE   7

static uint8_t msg_type;

#define DHCP_OPTION_SUBNET_MASK   1
#define DHCP_OPTION_ROUTER        3
#define DHCP_OPTION_DNS_SERVER    6
#define DHCP_OPTION_NTP_SERVER   42
#define DHCP_OPTION_REQ_IPADDR   50
#define DHCP_OPTION_LEASE_TIME   51
#define DHCP_OPTION_MSG_TYPE     53
#define DHCP_OPTION_SERVER_ID    54
#define DHCP_OPTION_REQ_LIST     55
#define DHCP_OPTION_END         255

static const u8_t xid[4] = {0xad, 0xde, 0x12, 0x23};
static const u8_t magic_cookie[4] = {99, 130, 83, 99};
/*---------------------------------------------------------------------------*/
static u8_t *
add_msg_type(u8_t *optptr, u8_t type)
{
  *optptr++ = DHCP_OPTION_MSG_TYPE;
  *optptr++ = 1;
  *optptr++ = type;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_server_id(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_SERVER_ID;
  *optptr++ = 4;
  memcpy(optptr, s.serverid, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_req_ipaddr(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_REQ_IPADDR;
  *optptr++ = 4;
  memcpy(optptr, s.ipaddr, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_req_options(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_REQ_LIST;
  *optptr++ = 3;
  *optptr++ = DHCP_OPTION_SUBNET_MASK;
  *optptr++ = DHCP_OPTION_ROUTER;
  *optptr++ = DHCP_OPTION_DNS_SERVER;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_end(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_END;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static void
create_msg(register struct dhcp_msg *m)
{
  m->op = DHCP_REQUEST;
  m->htype = DHCP_HTYPE_ETHERNET;
  m->hlen = s.mac_len;
  m->hops = 0;
  memcpy(m->xid, xid, sizeof(m->xid));
  m->secs = 0;
  m->flags = HTONS(BOOTP_BROADCAST); /*  Broadcast bit. */
  /*  uip_ipaddr_copy(m->ciaddr, uip_hostaddr);*/
  memcpy(m->ciaddr, uip_hostaddr, sizeof(m->ciaddr));
  memset(m->yiaddr, 0, sizeof(m->yiaddr)); // always zero per rfc, server def
  memset(m->siaddr, 0, sizeof(m->siaddr));
  memset(m->giaddr, 0, sizeof(m->giaddr));
  memcpy(m->chaddr, s.mac_addr, s.mac_len);
  memset(&m->chaddr[s.mac_len], 0, sizeof(m->chaddr) - s.mac_len);
#ifndef UIP_CONF_DHCP_LIGHT
  memset(m->sname, 0, sizeof(m->sname));
  memset(m->file, 0, sizeof(m->file));
#endif

  memcpy(m->options, magic_cookie, sizeof(magic_cookie));
}
/*---------------------------------------------------------------------------*/
static void
send_discover(void)
{
  u8_t *end;
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

  create_msg(m);

  end = add_msg_type(&m->options[4], DHCPDISCOVER);
  end = add_req_options(end);
  end = add_end(end);

  uip_send(uip_appdata, end - (u8_t *)uip_appdata);
}
/*---------------------------------------------------------------------------*/
static void
send_request(void)
{
  u8_t *end;
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

  create_msg(m);
  
  end = add_msg_type(&m->options[4], DHCPREQUEST);
  end = add_server_id(end);
  end = add_req_ipaddr(end);
  end = add_end(end);
  
  uip_send(uip_appdata, end - (u8_t *)uip_appdata);
}
/*---------------------------------------------------------------------------*/
static u8_t
parse_options(u8_t *optptr, uint16_t len)
{
  u8_t *end = optptr + len;
  u8_t type = 0;

  while(optptr < end) {
    switch(*optptr) {
    case DHCP_OPTION_SUBNET_MASK:
      memcpy(s.netmask, optptr + 2, 4);
      break;
    case DHCP_OPTION_ROUTER:
      memcpy(s.default_router, optptr + 2, 4);
      break;
    case DHCP_OPTION_DNS_SERVER:
      memcpy(s.dnsaddr, optptr + 2, 4);
      break;
    case DHCP_OPTION_MSG_TYPE:
      type = *(optptr + 2);
      break;
    case DHCP_OPTION_SERVER_ID:
      memcpy(s.serverid, optptr + 2, 4);
      break;
    case DHCP_OPTION_LEASE_TIME:
      memcpy(s.lease_time, optptr + 2, 4);
      break;
#ifdef __NTPCLIENT_H__
    case DHCP_OPTION_NTP_SERVER:
      memcpy(s.ntpaddr, optptr + 2, 4);
      break; 
#endif
    case DHCP_OPTION_END:
      return type;
    }

    optptr += optptr[1] + 2;
  }
  return type;
}
/*---------------------------------------------------------------------------*/
static u8_t parse_msg(void)
{
  struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

  if(m->op == DHCP_REPLY &&
     memcmp(m->xid, xid, sizeof(xid)) == 0 &&
     memcmp(m->chaddr, s.mac_addr, s.mac_len) == 0) {
    memcpy(s.ipaddr, m->yiaddr, 4);
    return parse_options(&m->options[4], uip_datalen());
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(handle_dhcp(void))
{
  PT_BEGIN(&s.pt);

#if defined PORT_APP_MAPPER
  dhcpc_running = 1; 
#endif


  if (s.state == STATE_RENEW)
      goto send_request_section;

  /* try_again:*/
  s.state = STATE_SENDING;
  s.ticks = CLOCK_SECOND;

//sendString("\r\ndhcpc handle dhcp passed: STATE_SENDING");
  do {
    send_discover();
    timer_set(&s.timer, s.ticks);
    // NOTE: fixed as per http://www.mail-archive.com/uip-users@sics.se/msg00003.html
    PT_YIELD_UNTIL(&s.pt, uip_newdata() || timer_expired(&s.timer));
//sendString("Just got something\n\r");

    if(uip_newdata())
    {
//sendString("Data\n\r");
        if (parse_msg() == DHCPOFFER)
        {
            s.state = STATE_OFFER_RECEIVED;
            break;
        }
    }
    else
    {
//sendString("Timeout\n\r");
        if(s.ticks < CLOCK_SECOND * 60) {
            s.ticks *= 2;
        }
        else
        {
            s.ticks = CLOCK_SECOND;
        }
    }

  } while(s.state != STATE_OFFER_RECEIVED);
  
//sendString("\r\ndhcpc handle dhcp passed: STATE_OFFER_RECEIVED");
  s.ticks = CLOCK_SECOND;

send_request_section:
  do {
    send_request();
    timer_set(&s.timer, s.ticks);
    // NOTE: fixed as per http://www.mail-archive.com/uip-users@sics.se/msg00003.html
    PT_YIELD_UNTIL(&s.pt, uip_newdata() || timer_expired(&s.timer));

    if(uip_newdata())
    {
        msg_type = parse_msg();
        if (msg_type == DHCPACK)
        {
            s.state = STATE_CONFIG_RECEIVED;
            break;
        }
        else if (msg_type == DHCPNAK)
        {
            s.state = STATE_FAIL;
            goto close_and_clean_up;
        }
    }
    else
    {
        if(s.ticks <= CLOCK_SECOND * 10) {
          s.ticks += CLOCK_SECOND;
        } else {
          PT_RESTART(&s.pt);
//sendString("\r\ndhcpc handle RESTARTING!");
        }
    }
  } while(s.state != STATE_CONFIG_RECEIVED);
//sendString("\r\ndhcpc handle dhcp passed: STATE_CONFIG_RECEIVED");
  
#if DEBUG_SERIAL
  printf_P(PSTR("Got IP address %d.%d.%d.%d\r\n"),
	 uip_ipaddr1(s.ipaddr), uip_ipaddr2(s.ipaddr),
	 uip_ipaddr3(s.ipaddr), uip_ipaddr4(s.ipaddr));
  printf_P(PSTR("Got netmask %d.%d.%d.%d\r\n"),
	 uip_ipaddr1(s.netmask), uip_ipaddr2(s.netmask),
	 uip_ipaddr3(s.netmask), uip_ipaddr4(s.netmask));
  printf_P(PSTR("Got DNS server %d.%d.%d.%d\r\n"),
	 uip_ipaddr1(s.dnsaddr), uip_ipaddr2(s.dnsaddr),
	 uip_ipaddr3(s.dnsaddr), uip_ipaddr4(s.dnsaddr));
  printf_P(PSTR("Got default router %d.%d.%d.%d\r\n"),
	 uip_ipaddr1(s.default_router), uip_ipaddr2(s.default_router),
	 uip_ipaddr3(s.default_router), uip_ipaddr4(s.default_router));
  printf_P(PSTR("Lease expires in %ld seconds\r\n"),
	 ntohs(s.lease_time[0])*65536ul + ntohs(s.lease_time[1]));
#endif

  dhcpc_configured(&s);
  
  /*  timer_stop(&s.timer);*/

  /*
   * PT_END restarts the thread so we do this instead. Eventually we
   * should reacquire expired leases here.
   */
/*  while(1) {
    PT_YIELD(&s.pt);
  }
*/
close_and_clean_up:
#if defined PORT_APP_MAPPER
  dhcpc_running = 0; 
#endif

  // all done with the connection, clean up
  uip_udp_remove(s.conn);
  s.conn = NULL;
//sendString("\r\ndhcpc handle dhcp passed: END");
  PT_END(&s.pt);
}
/*---------------------------------------------------------------------------*/
void
dhcpc_init(const void *mac_addr, uint8_t mac_len)
{
  uip_ipaddr_t addr;
  
  s.mac_addr = mac_addr;
  s.mac_len  = mac_len;

  s.state = STATE_INITIAL;
  uip_ipaddr(addr, 255,255,255,255);
  s.conn = uip_udp_new(&addr, HTONS(DHCPC_SERVER_PORT));
  if(s.conn != NULL) {
    uip_udp_bind(s.conn, HTONS(DHCPC_CLIENT_PORT));
  }
  PT_INIT(&s.pt);
}
/*---------------------------------------------------------------------------*/
void
dhcpc_appcall(void)
{
  // only when we ask for a new ip if we have a udp conn setup
  // since the dhcpc_appcall will be called as part of the normal uip app loop
  if ((s.conn != NULL) && (s.state != STATE_CONFIG_RECEIVED))
  {
//sendString("\r\ndhcpc appcall w/o config received");
    (void) handle_dhcp();
  }
}
/*---------------------------------------------------------------------------*/
void
dhcpc_request(void)
{
  u16_t ipaddr[2];
  
//sendString("\r\ndhcpc request called");
  if(s.state == STATE_INITIAL) {
//sendString("\r\ndhcpc request init state");
#if defined PORT_APP_MAPPER
    dhcpc_running = 1; 
#endif
    uip_ipaddr(ipaddr, 0,0,0,0);
    uip_sethostaddr(ipaddr);
    //handle_dhcp();
  }
}
/*---------------------------------------------------------------------------*/

void
dhcpc_renew(void)
{
  uip_ipaddr_t addr;

#if defined PORT_APP_MAPPER
  if (dhcpc_running)
    return;

  dhcpc_running = 1;
#else
  if (s.state != STATE_CONFIG_RECEIVED)
  {
    return;
  }
#endif

//sendString("\r\ndhcpc renew called");
  // if no server ip then we have to do a full request
  if (s.serverid[0] == 0)
  {
    dhcpc_init(s.mac_addr, s.mac_len);
    return;
  }

  // unicast to dhcp server
  uip_ipaddr(addr, s.serverid[0], s.serverid[1], s.serverid[2], s.serverid[3]);
  s.conn = uip_udp_new(&addr, HTONS(DHCPC_SERVER_PORT));
  if(s.conn != NULL) {
    uip_udp_bind(s.conn, HTONS(DHCPC_CLIENT_PORT));
  }
  s.state = STATE_RENEW;

  PT_INIT(&s.pt);
}
