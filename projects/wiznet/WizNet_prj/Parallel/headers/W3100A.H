
#ifndef	__W3100A_H__
#define	__W3100A_H__


#define	MAX_SOCK_NUM		4		// Concurrent maxmium number of socket

#define	SEND_DATA_BUF		0x4000	// Internal Tx buffer address of W3100A
#define	RECV_DATA_BUF		0x6000	// Internal Rx buffer address of W3100A

#define MAX_SEGMENT_SIZE	1460	// Maximum TCP transmission packet size
#define MAX_BUF_SIZE1		0

/* Internal register set of W3100A */
#define	INT_STATUS			0x04
#define	INT_REG				0x08
#define	INTMASK				0x09
#define	RESETSOCK			0x0A

#define	RW_PR				(0x10)
#define	RR_PR				(0x14)
#define	TA_PR				(0x18)

#define	TW_PR				(0x40)
#define	TR_PR				(0x44)

/* Shadow Register Pointer Define */
#define SRW_PR				(0x1E0)
#define	SRR_PR				(0x1E1)
#define	STA_PR				(0x1E2)
#define	STW_PR				(0x1F0)
#define STR_PR				(0x1F1)

#define SOCK_BASE			0xA0
#define SOCK_SIZE			0x18

#define SOCK_STATUS			(0xA0)
#define OPT_PROTOCOL		(0xA1)
#define DST_HA  			(0xA2)
#define DST_IP				(0xA8)
#define DST_PORT			(0xAC)
#define SRC_PORT			(0xAE)
#define IP_PROTOCOL			(0xB0)
#define TOS					(0xB1)
#define MSS					(0xB2)
#define P_WINDOW			(0xB4)
#define WINDOW				(0xB6)


#define NET_SETTINGS		(0x80)
#define GATEWAY				(0x80)
#define SUBNET_MASK			(0x84)
#define SRC_HA				(0x88)
#define SRC_IP				(0x8E)
#define TIMEOUT				(0x92)

#define RX_DMEM_SIZE		(0x95)
#define TX_DMEM_SIZE		(0x96)

/* SOCKET OPTION(Settting OPT_PROTOCOL REG.) */
#define SOCKOPT_BROADCAST	0x80		// Transmission, Reception of broadcasting data
#define SOCKOPT_NDTIMEOUT	0x40		// Setting timeout
#define SOCKOPT_NDACK		0x20		// Setting No Delayed Ack(TCP)
#define SOCKOPT_SWS			0x10		// Setting Silly Window Syndrome(TCP)

/* OPTION(Setting OPT_PROTOCOL REG.) for MAC LAYER RAW MODE */
#define MACLOPT_RXERR		0x80		// Setting reception of error packet
#define MACLOPT_BROADCAST	0x40		// Setting reception of broadcast packet
#define MACLOPT_PROMISC		0x20		// Setting reception of promiscuous packet

/* Distinguish TCP / UDP / IP RAW / MAC RAW (Setting OPT_PROTOCOL REG.) */
#define	SOCK_CLOSEDM		0x00 		// unused socket
#define	SOCK_STREAM			0x01		// TCP
#define	SOCK_DGRAM			0x02		// UDP
#define	SOCK_IPL_RAW		0x03		// IP LAYER RAW SOCK
#define	SOCK_MACL_RAW		0x04		// MAC LAYER RAW SOCK

/* Setting IP PROTOCOL */
#define IPPROTO_IP          0           // dummy for IP 
#define IPPROTO_ICMP        1           // control message protocol 
#define IPPROTO_IGMP        2           // internet group management protocol 
#define IPPROTO_GGP         3           // gateway^2 (deprecated) 
#define IPPROTO_TCP         6           // tcp 
#define IPPROTO_PUP         12          // pup
#define IPPROTO_UDP         17          // user datagram protocol 
#define IPPROTO_IDP         22          // xns idp 
#define IPPROTO_ND          77          // UNOFFICIAL net disk proto
#define IPPROTO_RAW         255         // raw IP packet 

/* Select parameter to use */
#define SEL_CONTROL			0			// Confirm socket status
#define SEL_SEND			1	       	// Confirm Tx free buffer size
#define SEL_RECV			2	       	// Confirm Rx data size

/* Command variables */
#define CSYS_INIT			0x01	   	// To set up network information (mac, gateway, subnet, ip)
#define CSOCK_INIT			0x02		// To initialize socket
#define CCONNECT			0x04		// To establish connection as tcp client mode
#define CLISTEN				0x08		// To wait for connection request as tcp server mode
#define CCLOSE				0x10		// To terminate connection
#define CSEND				0x20		// To send data
#define CRECV				0x40		// To receive data
#define CSW_RESET			0x80		// To do software reset

/* Status Variables */
#define SSYS_INIT_OK		0x01		// Completion of CSYS_INIT command
#define SSOCK_INIT_OK		0x02		// Completion of CSOCK_INIT command
#define SESTABLISHED		0x04		// Completion of connection setup
#define SCLOSED				0x08		// Completion of CCLOSED command
#define STIMEOUT			0x10		// Timout occured at send,sendto command
#define SSEND_OK			0x20		// Completion of sending data
#define SRECV_OK			0x40		// Completion of receiving data

/* Socket Status Vabiables */
#define SOCK_CLOSED			0x00		// Status of connection closed
#define SOCK_ARP			0x01		// Status of ARP
#define SOCK_LISTEN			0x02		// Status of waiting for TCP connection setup
#define SOCK_SYNSENT		0x03		// Status of setting up TCP connection
#define SOCK_SYNSENT_ACK	0x04		// Status of setting up TCP connection
#define SOCK_SYNRECV		0x05		// Status of setting up TCP connection
#define SOCK_ESTABLISHED	0x06		// Status of TCP connection established
#define SOCK_CLOSE_WAIT		0x07		// Status of closing TCP connection
#define SOCK_LAST_ACK		0x08		// Status of closing TCP connection
#define SOCK_FIN_WAIT1		0x09		// Status of closing TCP connection
#define SOCK_FIN_WAIT2		0x0A		// Status of closing TCP connection
#define SOCK_CLOSING		0x0B		// Status of closing TCP connection
#define SOCK_TIME_WAIT		0x0C		// Status of closing TCP connection
#define SOCK_RESET			0x0D		// Status of closing TCP connection
#define SOCK_INIT			0x0E		// Status of socket initialization
#define SOCK_UDP			0x0F		// Status of UDP
#define SOCK_RAW			0x10		// Status of IP RAW

#endif	/* __W3100A_H__ */
