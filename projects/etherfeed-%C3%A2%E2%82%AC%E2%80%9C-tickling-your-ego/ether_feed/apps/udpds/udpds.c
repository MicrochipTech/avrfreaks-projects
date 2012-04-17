/*
 * Copyright (c) 2011, Quentin Arce.
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
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

#include "udpds.h"
#include "uip.h"
#include "timer.h"


#include <string.h>

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

static struct uip_udp_conn *udpds_conn = NULL;
static struct timer udpds_interval_timer;
uint8_t udpds_enable_state = 1;


/*---------------------------------------------------------------------------*/
/** \internal
 * The main UDP function.
 */
/*---------------------------------------------------------------------------*/
void
udpds_appcall(void)
{
  if(uip_poll()) {
    if(udpds_enable_state && 
		timer_expired(&udpds_interval_timer)) {
      uip_udp_send(udpds_set_payload());
      timer_reset(&udpds_interval_timer);
	}
  }
}
/*---------------------------------------------------------------------------*/
void udpds_conf(u16_t *to_ip, u16_t to_port, u8_t interval)
{
  if(udpds_conn != NULL) {
    uip_udp_remove(udpds_conn);
  }

  timer_set(&udpds_interval_timer, (CLOCK_CONF_SECOND * interval));
  udpds_conn = uip_udp_new((uip_ipaddr_t *)to_ip, HTONS(to_port));
}

/* Functions. */
void udpds_enable(void)
{
  udpds_enable_state = 1;
}


void udpds_disable(void)
{
  udpds_enable_state = 0;
}


uint8_t udpds_state(void)
{
    return (udpds_enable_state);
}

