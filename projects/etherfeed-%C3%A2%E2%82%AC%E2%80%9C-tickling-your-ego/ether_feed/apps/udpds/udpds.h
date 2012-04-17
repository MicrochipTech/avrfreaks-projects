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
 */
#ifndef __UDPDS_H__
#define __UDPDS_H__

typedef int uip_udp_appstate_t;
void udpds_appcall(void);
#define UIP_UDP_APPCALL udpds_appcall 

#include "uipopt.h"

/**
 * Callback function which is called when a the interval timer goes off.
 *
 * This function must be implemented the logic to fill in the UDP packet
 * data which is to be sent out.  It must return the number of bytes
 * to be sent in the udp packet.
 */
extern int udpds_set_payload(void);

/* Functions. */
void udpds_conf(u16_t *to_ip, u16_t to_port, u8_t interval);
void udpds_init(void);
void udpds_enable(void);
void udpds_disable(void);
uint8_t udpds_state(void);

#endif /* __UDPDS_H__ */

/** @} */
