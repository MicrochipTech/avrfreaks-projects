#ifndef	_WIZNETW5100_H_
#define	_WIZNETW5100_H_



#define MAX_SIZE 32 // size of packetBuffer and Response
uint8_t packetBuffer[MAX_SIZE]; //buffer to hold incoming packet
uint8_t remoteIp[4]; // holds recvieved packet's originating IP
uint16_t remotePort; // holds received packet's originating port
//	 SOCKET sock = 0x01;
uint8_t packetSize;  // size of packet from w5100 inc. 8 byte Header
uint8_t DataSize;	 //	res packet from peer
uint8_t Packet_Pointer;
volatile uint8_t PacketInBuf; // flag
uint8_t PacketRespTo; // flag			
uint8_t RespLen; // length of packet to be sendt
uint8_t Response[MAX_SIZE]; // buffer to hold packet to be sendt




#define COMMON_BASE 0x0000

#define __DEF_IINCHIP_MAP_TXBUF__ (COMMON_BASE + 0x4000) /* Internal Tx buffer address of the iinchip */
#define __DEF_IINCHIP_MAP_RXBUF__ (COMMON_BASE + 0x6000) /* Internal Rx buffer address of the iinchip */

#define __DEF_IINCHIP_MAP_BASE__ 0x8000
#define MR __DEF_IINCHIP_MAP_BASE__
#define IDM_OR ((__DEF_IINCHIP_MAP_BASE__ + 0x00))
#define IDM_AR0 ((__DEF_IINCHIP_MAP_BASE__ + 0x01))
#define IDM_AR1 ((__DEF_IINCHIP_MAP_BASE__ + 0x02))
#define IDM_DR ((__DEF_IINCHIP_MAP_BASE__ + 0x03))


/**
 @brief Gateway IP Register address
 */
#define GAR0				(COMMON_BASE + 0x0001)
/**
 @brief Subnet mask Register address
 */
#define SUBR0			(COMMON_BASE + 0x0005)
/**
 @brief Source MAC Register address
 */
#define SHAR0				(COMMON_BASE + 0x0009)
/**
 @brief Source IP Register address
 */
#define SIPR0				(COMMON_BASE + 0x000F)
/**
 @brief Interrupt Register
 */
#define IR					(COMMON_BASE + 0x0015)
/**
 @brief Interrupt mask register
 */
#define IMR					(COMMON_BASE + 0x0016)
/**
 @brief Timeout register address( 1 is 100us )
 */
#define RTR0				(COMMON_BASE + 0x0017)
/**
 @brief Retry count reigster
 */
#define RCR						(COMMON_BASE + 0x0019)
/**
 @brief Receive memory size reigster
 */
#define RMSR			(COMMON_BASE + 0x001A)
/**
 @brief Transmit memory size reigster
 */
#define TMSR			(COMMON_BASE + 0x001B)
/**
 @brief Authentication type register address in PPPoE mode
 */
#define PATR0					(COMMON_BASE + 0x001C)
//#define PPPALGO (COMMON_BASE + 0x001D)
#define PTIMER (COMMON_BASE + 0x0028)
#define PMAGIC (COMMON_BASE + 0x0029)

/**
 @brief Unreachable IP register address in UDP mode
 */
#define UIPR0				(COMMON_BASE + 0x002A)
/**
 @brief Unreachable Port register address in UDP mode
 */
#define UPORT0			(COMMON_BASE + 0x002E)

/**
 @brief socket register
*/
#define CH_BASE (COMMON_BASE + 0x0400)
/**
 @brief	size of each channel register map
 */
#define CH_SIZE		0x0100
/**
 @brief socket Mode register
 */
#define Sn_MR(ch)		(CH_BASE + ch * CH_SIZE + 0x0000)
/**
 @brief channel Sn_CR register
 */
#define Sn_CR(ch)				(CH_BASE + ch * CH_SIZE + 0x0001)
/**
 @brief channel interrupt register
 */
#define Sn_IR(ch)			(CH_BASE + ch * CH_SIZE + 0x0002)
/**
 @brief channel status register
 */
#define Sn_SR(ch)			(CH_BASE + ch * CH_SIZE + 0x0003)
/**
 @brief source port register
 */
#define Sn_PORT0(ch)		(CH_BASE + ch * CH_SIZE + 0x0004)
/**
 @brief Peer MAC register address
 */
#define Sn_DHAR0(ch)			(CH_BASE + ch * CH_SIZE + 0x0006)
/**
 @brief Peer IP register address
 */
#define Sn_DIPR0(ch)			(CH_BASE + ch * CH_SIZE + 0x000C)
/**
 @brief Peer port register address
 */
#define Sn_DPORT0(ch)		(CH_BASE + ch * CH_SIZE + 0x0010)
/**
 @brief Maximum Segment Size(Sn_MSSR0) register address
 */
#define Sn_MSSR0(ch)					(CH_BASE + ch * CH_SIZE + 0x0012)
/**
 @brief Protocol of IP Header field register in IP raw mode
 */
#define Sn_PROTO(ch)			(CH_BASE + ch * CH_SIZE + 0x0014)

/** 
 @brief IP Type of Service(TOS) Register 
 */
#define Sn_TOS(ch)						(CH_BASE + ch * CH_SIZE + 0x0015)
/**
 @brief IP Time to live(TTL) Register 
 */
#define Sn_TTL(ch)						(CH_BASE + ch * CH_SIZE + 0x0016)

/**
 @brief Transmit free memory size register
 */
#define Sn_TX_FSR0(ch)	(CH_BASE + ch * CH_SIZE + 0x0020)
/**
 @brief Transmit memory read pointer register address
 */
#define Sn_TX_RD0(ch)			(CH_BASE + ch * CH_SIZE + 0x0022)
/**
 @brief Transmit memory write pointer register address
 */
#define Sn_TX_WR0(ch)			(CH_BASE + ch * CH_SIZE + 0x0024)
/**
 @brief Received data size register
 */
#define Sn_RX_RSR0(ch)	(CH_BASE + ch * CH_SIZE + 0x0026)
/**
 @brief Read point of Receive memory
 */
#define Sn_RX_RD0(ch)			(CH_BASE + ch * CH_SIZE + 0x0028)
/**
 @brief Write point of Receive memory
 */
#define Sn_RX_WR0(ch)			(CH_BASE + ch * CH_SIZE + 0x002A)



/* MODE register values */
#define MR_RST			0x80 /**< reset */
#define MR_PB			0x10 /**< ping block */
#define MR_PPPOE		0x08 /**< enable pppoe */
#define MR_LB  		0x04 /**< little or big endian selector in indirect mode */
#define MR_AI			0x02 /**< auto-increment in indirect mode */
#define MR_IND			0x01 /**< enable indirect mode */

/* IR register values */
#define IR_CONFLICT	0x80 /**< check ip confict */
#define IR_UNREACH	0x40 /**< get the destination unreachable message in UDP sending */
#define IR_PPPoE		0x20 /**< get the PPPoE close message */
#define IR_SOCK(ch)	(0x01 << ch) /**< check socket interrupt */

/* Sn_MR values */
#define Sn_MR_CLOSE		0x00		/**< unused socket */
#define Sn_MR_TCP		0x01		/**< TCP */
#define Sn_MR_UDP		0x02		/**< UDP */
#define Sn_MR_IPRAW	0x03		/**< IP LAYER RAW SOCK */
#define Sn_MR_MACRAW	0x04		/**< MAC LAYER RAW SOCK */
#define Sn_MR_PPPOE		0x05		/**< PPPoE */
#define Sn_MR_ND		0x20		/**< No Delayed Ack(TCP) flag */
#define Sn_MR_MULTI		0x80		/**< support multicating */


/* Sn_CR values */
#define Sn_CR_OPEN		0x01		/**< initialize or open socket */
#define Sn_CR_LISTEN		0x02		/**< wait connection request in tcp mode(Server mode) */
#define Sn_CR_CONNECT	0x04		/**< send connection request in tcp mode(Client mode) */
#define Sn_CR_DISCON		0x08		/**< send closing reqeuset in tcp mode */
#define Sn_CR_CLOSE		0x10		/**< close socket */
#define Sn_CR_SEND		0x20		/**< updata txbuf pointer, send data */
#define Sn_CR_SEND_MAC	0x21		/**< send data with MAC address, so without ARP process */
#define Sn_CR_SEND_KEEP	0x22		/**<  send keep alive message */
#define Sn_CR_RECV		0x40		/**< update rxbuf pointer, recv data */

#ifdef __DEF_IINCHIP_PPP__
	#define Sn_CR_PCON				0x23		 
	#define Sn_CR_PDISCON			0x24		 
	#define Sn_CR_PCR					0x25		 
	#define Sn_CR_PCN					0x26		
	#define Sn_CR_PCJ					0x27		
#endif

/* Sn_IR values */
#ifdef __DEF_IINCHIP_PPP__
	#define Sn_IR_PRECV			0x80		
	#define Sn_IR_PFAIL			0x40		
	#define Sn_IR_PNEXT			0x20		
#endif
#define Sn_IR_SEND_OK			0x10		/**< complete sending */
#define Sn_IR_TIMEOUT			0x08		/**< assert timeout */
#define Sn_IR_RECV				0x04		/**< receiving data */
#define Sn_IR_DISCON				0x02		/**< closed socket */
#define Sn_IR_CON					0x01		/**< established connection */

/* Sn_SR values */
#define SOCK_CLOSED				0x00		/**< closed */
#define SOCK_INIT 				0x13		/**< init state */
#define SOCK_LISTEN				0x14		/**< listen state */
#define SOCK_SYNSENT	   		0x15		/**< connection state */
#define SOCK_SYNRECV		   	0x16		/**< connection state */
#define SOCK_ESTABLISHED		0x17		/**< success to connect */
#define SOCK_FIN_WAIT			0x18		/**< closing state */
#define SOCK_CLOSING		   	0x1A		/**< closing state */
#define SOCK_TIME_WAIT			0x1B		/**< closing state */
#define SOCK_CLOSE_WAIT			0x1C		/**< closing state */
#define SOCK_LAST_ACK			0x1D		/**< closing state */
#define SOCK_UDP				   0x22		/**< udp socket */
#define SOCK_IPRAW			   0x32		/**< ip raw mode socket */
#define SOCK_MACRAW			   0x42		/**< mac raw mode socket */
#define SOCK_PPPOE				0x5F		/**< pppoe socket */

/* IP PROTOCOL */
#define IPPROTO_IP              0           /**< Dummy for IP */
#define IPPROTO_ICMP            1           /**< Control message protocol */
#define IPPROTO_IGMP            2           /**< Internet group management protocol */
#define IPPROTO_GGP             3           /**< Gateway^2 (deprecated) */
#define IPPROTO_TCP             6           /**< TCP */
#define IPPROTO_PUP             12          /**< PUP */
#define IPPROTO_UDP             17          /**< UDP */
#define IPPROTO_IDP             22          /**< XNS idp */
#define IPPROTO_ND              77          /**< UNOFFICIAL net disk protocol */
#define IPPROTO_RAW             255         /**< Raw IP packet */


////////////////////////////SOCKET

typedef uint8_t 			SOCKET;

#define	MAX_SOCK_NUM		4	/**< Maxmium number of socket  */

uint8_t IINCHIP_WRITE(uint16_t addr,uint8_t data);
extern uint8_t IINCHIP_READ(uint16_t addr);
extern uint8_t IINCHIP_WRITE(uint16_t addr,uint8_t data);
extern uint16_t wiz_read_buf(uint16_t addr, uint8_t* buf,uint16_t len);
extern uint16_t wiz_write_buf(uint16_t addr,uint8_t* buf,uint16_t len);

extern void iinchip_init(void); // reset iinchip
extern void sysinit(uint8_t tx_size, uint8_t rx_size); // setting tx/rx buf size
void setIMR(uint8_t mask); //Setting the interrupt mask
/*
7
IM_IR7
IP Conflict Enable
6
IM_IR6
Destination unreachable Enable
5
IM_IR5
PPPoE Close Enable
4
Reserved
It should be set as ‘0’
3
IM_IR3
Occurrence of Socket 3 Socket Interrupt Enable
2
IM_IR2
Occurrence of Socket 2 Socket Interrupt Enable
1
IM_IR1
Occurrence of Socket 1 Socket Interrupt Enable
0
IM_IR0
Occurrence of Socket 0 Socket Interrupt Enable
*/
uint8_t getSn_IR(SOCKET s);

/*
7
Reserved
Reserved
6
Reserved
Reserved
5
Reserved
Reserved
4
SEND_OK
It is set as ‘1’ if send operation is completed.
3
TIMEOUT
It is set as ‘1’ if Timeout occurs during connection establishment or termination and data transmission.
2
RECV
It is set as ‘1’ whenever W5100 receives data. And it is also set as ‘1’
if received data remains after execute CMD_RECV command.
1
DISCON
It is set as ‘1’ if connection termination is requested or finished.
0
CON
It is set as ‘1’ if connection is established.
*/

uint8_t getIR( void );

void setSubnetMask(uint8_t * addr);
void setIPAddress(uint8_t * addr);
void setGatewayAddress(uint8_t * addr);
void setMacAddress(uint8_t * addr);

extern uint8_t getSn_SR(SOCKET s); // get socket status
extern uint16_t getSn_TX_FSR(SOCKET s); // get socket TX free buf size
extern uint16_t getSn_RX_RSR(SOCKET s); // get socket RX recv buf size
extern uint16_t getIINCHIP_RxMAX(uint8_t s);
extern uint16_t getIINCHIP_TxMAX(uint8_t s);
extern uint16_t getIINCHIP_RxMASK(uint8_t s);
extern uint16_t getIINCHIP_TxMASK(uint8_t s);
extern uint16_t getIINCHIP_RxBASE(uint8_t s);
extern uint16_t getIINCHIP_TxBASE(uint8_t s);
void getSn_DIPR(SOCKET s, uint8_t * addr);
void setMR(uint8_t val);

extern void send_data_processing(SOCKET s, uint8_t *data, uint16_t len);
extern void recv_data_processing(SOCKET s, uint8_t *data, uint16_t len);
extern void read_data(SOCKET s, volatile uint8_t * src, volatile uint8_t * dst, uint16_t len);
extern void write_data(SOCKET s, volatile uint8_t * src, volatile uint8_t * dst, uint16_t len);



extern uint8_t socket(SOCKET s, uint8_t protocol, uint16_t port, uint8_t flag); // Opens a socket(TCP or UDP or IP_RAW mode)
extern void close(SOCKET s); // Close socket
extern uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port); // Establish TCP connection (Active connection)
extern void disconnect(SOCKET s); // disconnect the connection
extern uint8_t listen(SOCKET s);	// Establish TCP connection (Passive connection)
extern uint16_t send(SOCKET s, const uint8_t * buf, uint16_t len); // Send data (TCP)
extern uint16_t recv(SOCKET s, uint8_t * buf, uint16_t len);	// Receive data (TCP)
extern uint16_t sendto(SOCKET s, const uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t port); // Send data (UDP/IP RAW)
extern uint16_t recvfrom(SOCKET s, uint8_t * buf, uint16_t len, uint8_t * addr, uint16_t  *port); // Receive data (UDP/IP RAW)

#endif
 
