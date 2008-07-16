#ifndef _TCP_H_
#define _TCP_H_
#include <stdint.h> 
#include "../main.h"
#include "httpd.h"
#include "ethernet.h"
#include "arp.h"
#include "ip.h"
#include "nethelp.h"
#include "mynic.h"
#include "../main.h"


struct tcp_socket {
	unsigned int  source_port; //= port on the client
	unsigned int  dest_port;   //= (our) port
	unsigned long source_ip;   //= ip of the client
	unsigned long seq;
	unsigned long ack;
	unsigned char state;
	unsigned char ttl;
	unsigned char misc_state; //used by application above tcp!
};

extern struct tcp_socket tcp_sockets[];

//initialise all sockets to closed state:
void tcp_init(void);
//find a free socket
unsigned char tcp_find_free_socket(void);
//handle tcp packet
void tcp_packet_in(unsigned char *buffer, unsigned int len);
//called ~every second, removes dead tcp sockets
void tcp_ttl_cleanup(void);

void tcp_send_packet(unsigned char *buffer, unsigned char socketnum, unsigned char flags, unsigned int len);
void tcp_close_socket(unsigned char socketnum);

#define TCP_FIN_FLAG (1<<0)
#define TCP_SYN_FLAG (1<<1)
#define TCP_RST_FLAG (1<<2)
#define TCP_PSH_FLAG (1<<3)
#define TCP_ACK_FLAG (1<<4)
#define TCP_URG_FLAG (1<<5)
#define TCP_ECE_FLAG (1<<6)
#define TCP_CWR_FLAG (1<<7)



#define TCP_POS_SRC_PORT 34
#define TCP_POS_DST_PORT 36
#define TCP_POS_SEQ  38
#define TCP_POS_ACK  42
#define TCP_POS_DATA_OFFSET 46
#define TCP_POS_HEADERFLAGS 47
#define TCP_POS_WINDOWSIZE 48
#define TCP_POS_CHECKSUM 50
#define TCP_POS_URGENT_PTR 52
//#define TCP_POS_PKTLEN 46
#define TCP_POS_TXDATA 54


#define TCP_SOCKET_SYN		0	
#define TCP_SOCKET_OPEN1		1
#define TCP_SOCKET_OPEN2		2
//POST req:
#define TCP_SOCKET_OPEN3		4 
#define TCP_SOCKET_CLOSE		3

#define TCP_APPSTATE_NONE       0
#define TCP_APPSTATE_EMPTYACK   1
#define TCP_APPSTATE_HAS_TXDATA 2
#define TCP_APPSTATE_FINISHED   3

//maximum connection count
#define TCP_SOCKET_COUNT 10
//60 seconds timeout:
#define TCP_TTL_TIMEOUT 60

//tcp states (see http://upload.wikimedia.org/wikipedia/de/b/ba/Tcp_verbindung.png)
#define TCP_STATE_CLOSED       0
#define TCP_STATE_SYN_SENT     1
#define TCP_STATE_LISTEN       2
#define TCP_STATE_SYN_RECEIVED 3
#define TCP_STATE_ESTABLISHED  4
#define TCP_STATE_FIN_WAIT1    5
#define TCP_STATE_FIN_WAIT2    6
#define TCP_STATE_CLOSING      7
#define TCP_STATE_TIMED_WAIT   8
#define TCP_STATE_CLOSE_WAIT   9
#define TCP_STATE_LAST_ACK    10


#endif
