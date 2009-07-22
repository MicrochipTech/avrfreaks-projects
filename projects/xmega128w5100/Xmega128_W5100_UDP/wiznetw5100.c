 
#include <avr/io.h>
#include "wiznetw5100.h"
//#include "wiz5100.h"
#include "stdio.h"
#include "spi_xmega.h"


// circular buffer for data received from w5100


static uint16_t local_port;

static uint16_t SMASK[MAX_SOCK_NUM]; /**< Variable for Tx buffer MASK in each channel */
static uint16_t RMASK[MAX_SOCK_NUM]; /**< Variable for Rx buffer MASK in each channel */
static uint16_t SSIZE[MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
static uint16_t RSIZE[MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */
static uint16_t SBUFBASEADDRESS[MAX_SOCK_NUM]; /**< Tx buffer base address by each channel */
static uint16_t RBUFBASEADDRESS[MAX_SOCK_NUM]; /**< Rx buffer base address by each channel */



 /**
@brief	This function writes the data into W5100 registers.
*/
uint8_t IINCHIP_WRITE(uint16_t addr,uint8_t data)
{
	IINCHIP_CSoff();                             // CS=0, SPI start

	IINCHIP_SpiSendData(0xF0);
	IINCHIP_SpiSendData((addr & 0xFF00) >> 8);
	IINCHIP_SpiSendData(addr & 0x00FF);
	IINCHIP_SpiSendData(data);

	IINCHIP_CSon();    

	return 1;
}



/**
@brief	This function reads the value from W5100 registers.
*/
uint8_t IINCHIP_READ(uint16_t addr)
{
	uint8_t data;
 
	IINCHIP_CSoff();                             // CS=0, SPI start

	IINCHIP_SpiSendData(0x0F);
	IINCHIP_SpiSendData((addr & 0xFF00) >> 8);
	IINCHIP_SpiSendData(addr & 0x00FF);
	
	
	IINCHIP_SpiSendData(0);
	data = IINCHIP_SpiRecvData();

	IINCHIP_CSon();                          	// SPI end

	return data;
}
/**
@brief	This function gets Interrupt register in common register.
 */
uint8_t getIR( void )
{
   return IINCHIP_READ(IR);
}



/**
@brief	This function set the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)

If any bit in IMR is set as '0' then there is not interrupt signal though the bit is
set in IR register.
*/
void setIMR(uint8_t mask)
{
	IINCHIP_WRITE(IMR,mask); // must be setted 0x10.
}
 
/**
@brief	get socket interrupt status

These below functions are used to read the Interrupt & Soket Status register
*/
uint8_t getSn_IR(SOCKET s)
{
   return IINCHIP_READ(Sn_IR(s));
}



/**
@brief	This function set the transmit & receive buffer size as per the channels is used

Note for TMSR and RMSR bits are as follows\n
bit 1-0 : memory size of channel #0 \n
bit 3-2 : memory size of channel #1 \n
bit 5-4 : memory size of channel #2 \n
bit 7-6 : memory size of channel #3 \n\n
Maximum memory size for Tx, Rx in the W5100 is 8K Bytes,\n
In the range of 8KBytes, the memory size could be allocated dynamically by each channel.\n
Be attentive to sum of memory size shouldn't exceed 8Kbytes\n
and to data transmission and receiption from non-allocated channel may cause some problems.\n
If the 8KBytes memory is already  assigned to centain channel, \n
other 3 channels couldn't be used, for there's no available memory.\n
If two 4KBytes memory are assigned to two each channels, \n
other 2 channels couldn't be used, for there's no available memory.\n
*/ 
void sysinit(
	uint8_t tx_size, 	/**< tx_size Tx memory size (00 - 1KByte, 01- 2KBtye, 10 - 4KByte, 11 - 8KByte) */
	uint8_t rx_size		/**< rx_size Rx memory size (00 - 1KByte, 01- 2KBtye, 10 - 4KByte, 11 - 8KByte) */
	)
{
	int16_t i;
	int16_t ssum,rsum;


	ssum = 0;
	rsum = 0;
	
	IINCHIP_WRITE(TMSR,tx_size); /* Set Tx memory size for each channel */
	IINCHIP_WRITE(RMSR,rx_size);	 /* Set Rx memory size for each channel */

	SBUFBASEADDRESS[0] = (uint16_t)(__DEF_IINCHIP_MAP_TXBUF__);		/* Set base address of Tx memory for channel #0 */
	RBUFBASEADDRESS[0] = (uint16_t)(__DEF_IINCHIP_MAP_RXBUF__);		/* Set base address of Rx memory for channel #0 */


   for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
	{
		SSIZE[i] = (int16_t)(0);
		RSIZE[i] = (int16_t)(0);
		if (ssum < 8192)
		{
         switch((tx_size >> i*2) & 0x03)  // Set Tx memory size
			{
			case 0:
				SSIZE[i] = (int16_t)(1024);
				SMASK[i] = (uint16_t)(0x03FF);
				break;
			case 1:
				SSIZE[i] = (int16_t)(2048);
				SMASK[i] = (uint16_t)(0x07FF);
				break;
			case 2:
				SSIZE[i] = (int16_t)(4096);
				SMASK[i] = (uint16_t)(0x0FFF);
				break;
			case 3:
				SSIZE[i] = (int16_t)(8192);
				SMASK[i] = (uint16_t)(0x1FFF);
				break;
			}
		}
		if (rsum < 8192)
		{
         switch((rx_size >> i*2) & 0x03)     // Set Rx memory size
			{
			case 0:
				RSIZE[i] = (int16_t)(1024);
				RMASK[i] = (uint16_t)(0x03FF);
				break;
			case 1:
				RSIZE[i] = (int16_t)(2048);
				RMASK[i] = (uint16_t)(0x07FF);
				break;
			case 2:
				RSIZE[i] = (int16_t)(4096);
				RMASK[i] = (uint16_t)(0x0FFF);
				break;
			case 3:
				RSIZE[i] = (int16_t)(8192);
				RMASK[i] = (uint16_t)(0x1FFF);
				break;
			}
		}
		ssum += SSIZE[i];
		rsum += RSIZE[i];

      if (i != 0)             // Sets base address of Tx and Rx memory for channel #1,#2,#3
		{
			SBUFBASEADDRESS[i] = SBUFBASEADDRESS[i-1] + SSIZE[i-1];
			RBUFBASEADDRESS[i] = RBUFBASEADDRESS[i-1] + RSIZE[i-1];
		}
	}
}


/**
@brief	This function is for resetting of the iinchip. Initializes the iinchip to work in whether DIRECT or INDIRECT mode
*/ 
void iinchip_init(void)
{	
	setMR( MR_RST );
}


/**
@brief	This function sets up gateway IP address.
*/ 
void setGatewayAddress(
	uint8_t * addr	/**< a pointer to a 4 -byte array responsible to set the Gateway IP address. */
	)
{
	IINCHIP_WRITE((GAR0 + 0),addr[0]);
	IINCHIP_WRITE((GAR0 + 1),addr[1]);
	IINCHIP_WRITE((GAR0 + 2),addr[2]);
	IINCHIP_WRITE((GAR0 + 3),addr[3]);
}



/**
@brief	It sets up SubnetMask address
*/ 
void setSubnetMask(
	uint8_t * addr	/**< a pointer to a 4 -byte array responsible to set the SubnetMask address */
	)
{
	IINCHIP_WRITE((SUBR0 + 0),addr[0]);
	IINCHIP_WRITE((SUBR0 + 1),addr[1]);
	IINCHIP_WRITE((SUBR0 + 2),addr[2]);
	IINCHIP_WRITE((SUBR0 + 3),addr[3]);
}


/**
@brief	This function sets up MAC address.
*/ 
void setMacAddress(
	uint8_t * addr	/**< a pointer to a 6 -byte array responsible to set the MAC address. */
	)
{
	IINCHIP_WRITE((SHAR0 + 0),addr[0]);
	IINCHIP_WRITE((SHAR0 + 1),addr[1]);
	IINCHIP_WRITE((SHAR0 + 2),addr[2]);
	IINCHIP_WRITE((SHAR0 + 3),addr[3]);
	IINCHIP_WRITE((SHAR0 + 4),addr[4]);
	IINCHIP_WRITE((SHAR0 + 5),addr[5]);
}


/**
@brief	This function sets up Source IP address.
*/
void setIPAddress(
	uint8_t * addr	/**< a pointer to a 4 -byte array responsible to set the Source IP address. */
	)
{
	IINCHIP_WRITE((SIPR0 + 0),addr[0]);
	IINCHIP_WRITE((SIPR0 + 1),addr[1]);
	IINCHIP_WRITE((SIPR0 + 2),addr[2]);
	IINCHIP_WRITE((SIPR0 + 3),addr[3]);
}








/**
@brief	This Socket function initialize the channel in perticular mode, and set the port and wait for W5100 done it.
@return 	1 for sucess else 0.
*/  
uint8_t socket(
	SOCKET s, 		/**< for socket number */
	uint8_t protocol, 	/**< for socket protocol */
	uint16_t port, 		/**< the source port for the socket */
	uint8_t flag		/**< the option for the socket */
	)
{
	uint8_t ret;
	if ((protocol == Sn_MR_TCP) || (protocol == Sn_MR_UDP) || (protocol == Sn_MR_IPRAW) || (protocol == Sn_MR_MACRAW) || (protocol == Sn_MR_PPPOE))
	{
		close(s);
		IINCHIP_WRITE(Sn_MR(s),protocol | flag);
		if (port != 0) {
			IINCHIP_WRITE(Sn_PORT0(s),(uint8_t)((port & 0xff00) >> 8));
			IINCHIP_WRITE((Sn_PORT0(s) + 1),(uint8_t)(port & 0x00ff));
		} else {
			local_port++; // if don't set the source port, set local_port number.
			IINCHIP_WRITE(Sn_PORT0(s),(uint8_t)((local_port & 0xff00) >> 8));
			IINCHIP_WRITE((Sn_PORT0(s) + 1),(uint8_t)(local_port & 0x00ff));
		}
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_OPEN); // run sockinit Sn_CR

		/* +20071122[chungs]:wait to process the command... */
		while( IINCHIP_READ(Sn_CR(s)) ) 
			;
		/* ------- */
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	//printf("Sn_SR = %.2x , Protocol = %.2x\r\n", IINCHIP_READ(Sn_SR(s)), IINCHIP_READ(Sn_MR(s)));
	return ret;
}


/**
@brief	This function close the socket and parameter is "s" which represent the socket number
*/ 
void close(SOCKET s)
{
	
	IINCHIP_WRITE(Sn_CR(s),Sn_CR_CLOSE);

	/* +20071122[chungs]:wait to process the command... */
	while( IINCHIP_READ(Sn_CR(s)) ) 
		;

	IINCHIP_WRITE(Sn_IR(s), 0xFF);
}


/**
@brief	This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
@return	1 for success else 0.
*/ 
uint8_t listen(
	SOCKET s	/**< the socket number */
	)
{
	uint8_t ret;
	if (IINCHIP_READ(Sn_SR(s)) == SOCK_INIT)
	{
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_LISTEN);
		/* +20071122[chungs]:wait to process the command... */
		while( IINCHIP_READ(Sn_CR(s)) ) 
			;
		/* ------- */
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	return ret;
}


/**
@brief	This function established  the connection for the channel in Active (client) mode. 
		This function waits for the untill the connection is established.
		
@return	1 for success else 0.
*/ 
uint8_t connect(SOCKET s, uint8_t * addr, uint16_t port)
{
	uint8_t ret;
	if 
		(
			((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
		 	((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
		 	(port == 0x00) 
		) 
 	{
 		ret = 0;
	}
	else
	{
		ret = 1;
		// set destination IP
		IINCHIP_WRITE(Sn_DIPR0(s),addr[0]);
		IINCHIP_WRITE((Sn_DIPR0(s) + 1),addr[1]);
		IINCHIP_WRITE((Sn_DIPR0(s) + 2),addr[2]);
		IINCHIP_WRITE((Sn_DIPR0(s) + 3),addr[3]);
		IINCHIP_WRITE(Sn_DPORT0(s),(uint8_t)((port & 0xff00) >> 8));
		IINCHIP_WRITE((Sn_DPORT0(s) + 1),(uint8_t)(port & 0x00ff));
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_CONNECT);
      /* m2008.01 [bj] :  wait for completion */
		while ( IINCHIP_READ(Sn_CR(s)) ) ;

	}

	return ret;
}



/**
@brief	This function used for disconnect the socket and parameter is "s" which represent the socket number
@return	1 for success else 0.
*/ 
void disconnect(SOCKET s)
{
	IINCHIP_WRITE(Sn_CR(s),Sn_CR_DISCON);

	/* +20071122[chungs]:wait to process the command... */
	while( IINCHIP_READ(Sn_CR(s)) ) 
		;
}


/**
@brief	This function used to send the data in TCP mode
@return	1 for success else 0.
*/ 
uint16_t send(
	SOCKET s, 		/**< the socket index */
	const uint8_t * buf, 	/**< a pointer to data */
	uint16_t len		/**< the data size to be send */
	)
{
	uint8_t status=0;
	uint16_t ret=0;
	uint16_t freesize=0;

   if (len > getIINCHIP_TxMAX(s)) ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
   else ret = len;

   // if freebuf is available, start.
	do 
	{
		freesize = getSn_TX_FSR(s);
		status = IINCHIP_READ(Sn_SR(s));
		if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
		{
			ret = 0; 
			break;
		}
	} while (freesize < ret);

      // copy data
	send_data_processing(s, (uint8_t *)buf, ret);
	IINCHIP_WRITE(Sn_CR(s),Sn_CR_SEND);

	/* +20071122[chungs]:wait to process the command... */
	while( IINCHIP_READ(Sn_CR(s)) ) 
		;
	/* ------- */

	while ( (IINCHIP_READ(Sn_IR(s)) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK ) 
	{
		/* m2008.01 [bj] : reduce code */
		if ( IINCHIP_READ(Sn_SR(s)) == SOCK_CLOSED )
		{
			close(s);
			return 0;
		}
  	}
	IINCHIP_WRITE(Sn_IR(s), Sn_IR_SEND_OK);
  	return ret;
}


/**
@brief	This function is an application I/F function which is used to receive the data in TCP mode.
		It continues to wait for data as much as the application wants to receive.
		
@return	received data size for success else -1.
*/ 
uint16_t recv(
	SOCKET s, 	/**< socket index */
	uint8_t * buf, 	/**< a pointer to copy the data to be received */
	uint16_t len	/**< the data size to be read */
	)
{
	uint16_t ret=0;


	if ( len > 0 )
	{
		recv_data_processing(s, buf, len);
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_RECV);

		/* +20071122[chungs]:wait to process the command... */
		while( IINCHIP_READ(Sn_CR(s)) ) 
			;
		ret = len;
	}
	return ret;
}


/**
@brief	This function is an application I/F function which is used to send the data for other then TCP mode. 
		Unlike TCP transmission, The peer's destination address and the port is needed.
		
@return	This function return send data size for success else -1.
*/ 
uint16_t sendto(
	SOCKET s, 		/**< socket index */
	const uint8_t * buf, 	/**< a pointer to the data */
	uint16_t len, 		/**< the data size to send */
	uint8_t * addr, 		/**< the peer's Destination IP address */
	uint16_t port		/**< the peer's destination port number */
	)
{
//	uint8_t status=0;
//	uint8_t isr=0;
	uint16_t ret=0;
	
   if (len > getIINCHIP_TxMAX(s)) ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
   else ret = len;

	if
		(
		 	((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
		 	((port == 0x00)) ||(ret == 0)
		) 
 	{
 	   /* +2008.01 [bj] : added return value */
 	   ret = 0;
	}
	else
	{
		IINCHIP_WRITE(Sn_DIPR0(s),addr[0]);
		IINCHIP_WRITE((Sn_DIPR0(s) + 1),addr[1]);
		IINCHIP_WRITE((Sn_DIPR0(s) + 2),addr[2]);
		IINCHIP_WRITE((Sn_DIPR0(s) + 3),addr[3]);
		IINCHIP_WRITE(Sn_DPORT0(s),(uint8_t)((port & 0xff00) >> 8));
		IINCHIP_WRITE((Sn_DPORT0(s) + 1),(uint8_t)(port & 0x00ff));

  		// copy data
  		send_data_processing(s, (uint8_t *)buf, ret);
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_SEND);

		/* +20071122[chungs]:wait to process the command... */
		while( IINCHIP_READ(Sn_CR(s)) ) 
			;
		/* ------- */
		
/* +2008.01 bj */	
	   while ( (IINCHIP_READ(Sn_IR(s)) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK ) 
		{
	      if (IINCHIP_READ(Sn_IR(s)) & Sn_IR_TIMEOUT)
			{
         	IINCHIP_WRITE(Sn_IR(s), (Sn_IR_SEND_OK | Sn_IR_TIMEOUT)); /* clear SEND_OK & TIMEOUT */
			return 0;
			}
		}

/* +2008.01 bj */	
	   IINCHIP_WRITE(Sn_IR(s), Sn_IR_SEND_OK);

	}
	return ret;
}


/**
@brief	This function is an application I/F function which is used to receive the data in other then
	TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well. 
	
@return	This function return received data size for success else -1.
*/ 
uint16_t recvfrom(
	SOCKET s, 	/**< the socket number */
	uint8_t * buf, 	/**< a pointer to copy the data to be received */
	uint16_t len, 	/**< the data size to read */
	uint8_t * addr, 	/**< a pointer to store the peer's IP address */
	uint16_t *port	/**< a pointer to store the peer's port number. */
	)
{
	uint8_t head[8];
	uint16_t data_len=0;
	uint16_t ptr=0;

	if ( len > 0 )
	{
   	ptr = IINCHIP_READ(Sn_RX_RD0(s));
   	ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(s) + 1);
   	switch (IINCHIP_READ(Sn_MR(s)) & 0x07)
   	{
   	case Sn_MR_UDP :
   			read_data(s, (uint8_t *)ptr, head, 0x08);
   			ptr += 8;
   			// read peer's IP address, port number.
    			addr[0] = head[0];
   			addr[1] = head[1];
   			addr[2] = head[2];
   			addr[3] = head[3];
   			*port = head[4];
   			*port = (*port << 8) + head[5];
   			data_len = head[6];
   			data_len = (data_len << 8) + head[7];
   			
			read_data(s, (uint8_t *)ptr, buf, data_len); // data copy.
			ptr += data_len;

			IINCHIP_WRITE(Sn_RX_RD0(s),(uint8_t)((ptr & 0xff00) >> 8));
			IINCHIP_WRITE((Sn_RX_RD0(s) + 1),(uint8_t)(ptr & 0x00ff));
   			break;
   
   	case Sn_MR_IPRAW :
   			read_data(s, (uint8_t *)ptr, head, 0x06);
   			ptr += 6;
   
   			addr[0] = head[0];
   			addr[1] = head[1];
   			addr[2] = head[2];
   			addr[3] = head[3];
   			data_len = head[4];
   			data_len = (data_len << 8) + head[5];
   	
			read_data(s, (uint8_t *)ptr, buf, data_len); // data copy.
			ptr += data_len;

			IINCHIP_WRITE(Sn_RX_RD0(s),(uint8_t)((ptr & 0xff00) >> 8));
			IINCHIP_WRITE((Sn_RX_RD0(s) + 1),(uint8_t)(ptr & 0x00ff));
   			break;

   	case Sn_MR_MACRAW :
   			read_data(s,(uint8_t*)ptr,head,2);
   			ptr+=2;
   			data_len = head[0];
   			data_len = (data_len<<8) + head[1] - 2;

   			read_data(s,(uint8_t*) ptr,buf,data_len);
   			ptr += data_len;
   			IINCHIP_WRITE(Sn_RX_RD0(s),(uint8_t)((ptr & 0xff00) >> 8));
   			IINCHIP_WRITE((Sn_RX_RD0(s) + 1),(uint8_t)(ptr & 0x00ff));
   			
			break;

   	default :
   			break;
   	}
		IINCHIP_WRITE(Sn_CR(s),Sn_CR_RECV);

		/* +20071122[chungs]:wait to process the command... */
		while( IINCHIP_READ(Sn_CR(s)) ) 
			;
	}
 	return data_len;
}

/**
@brief	 This function is being called by send() and sendto() function also. 

This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
register. User should read upper byte first and lower byte later to get proper value.
*/
void send_data_processing(SOCKET s, uint8_t *data, uint16_t len)
{
	uint16_t ptr;
	ptr = IINCHIP_READ(Sn_TX_WR0(s));
	ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR0(s) + 1);
	write_data(s, data, (uint8_t *)(ptr), len);
	ptr += len;
	IINCHIP_WRITE(Sn_TX_WR0(s),(uint8_t)((ptr & 0xff00) >> 8));
	IINCHIP_WRITE((Sn_TX_WR0(s) + 1),(uint8_t)(ptr & 0x00ff));
}


/**
@brief	for copy the data form application buffer to Transmite buffer of the chip.

This function is being used for copy the data form application buffer to Transmite
buffer of the chip. It calculate the actual physical address where one has to write
the data in transmite buffer. Here also take care of the condition while it exceed
the Tx memory uper-bound of socket.
*/
void write_data(SOCKET s, volatile uint8_t * src, volatile uint8_t * dst, uint16_t len)
{
	uint16_t size;
	uint16_t dst_mask;
	uint8_t * dst_ptr;

	dst_mask = (uint16_t)dst & getIINCHIP_TxMASK(s);
	dst_ptr = (uint8_t *)(getIINCHIP_TxBASE(s) + dst_mask);
	
	if (dst_mask + len > getIINCHIP_TxMAX(s)) 
	{
		size = getIINCHIP_TxMAX(s) - dst_mask;
		wiz_write_buf((uint16_t)dst_ptr, (uint8_t*)src, size);
		src += size;
		size = len - size;
		dst_ptr = (uint8_t *)(getIINCHIP_TxBASE(s));
		wiz_write_buf((uint16_t)dst_ptr, (uint8_t*)src, size);
	} 
	else
	{
		wiz_write_buf((uint16_t)dst_ptr, (uint8_t*)src, len);
	}
}


/**
@brief	This function writes into W5100 memory(Buffer)
*/ 
uint16_t wiz_write_buf(uint16_t addr,uint8_t* buf,uint16_t len)
{
	uint16_t idx = 0;
      
	for(idx=0;idx<len;idx++)
	   {
		IINCHIP_CSoff();                             // CS=0, SPI start 

		IINCHIP_SpiSendData(0xF0);
		IINCHIP_SpiSendData(((addr+idx) & 0xFF00) >> 8);
		IINCHIP_SpiSendData((addr+idx) & 0x00FF);
		IINCHIP_SpiSendData(buf[idx]);

		IINCHIP_CSon();                             // CS=0, SPI end 
	   }


	return len;
}


/**
@brief	This function reads into W5100 memory(Buffer)
*/ 
uint16_t wiz_read_buf(uint16_t addr, uint8_t* buf,uint16_t len)
{
	uint16_t idx = 0;
   
	for (idx=0; idx<len; idx++)
      	   {
		IINCHIP_CSoff();                             // CS=0, SPI start 

		IINCHIP_SpiSendData(0x0F);
		IINCHIP_SpiSendData(((addr+idx) & 0xFF00) >> 8);
		IINCHIP_SpiSendData((addr+idx) & 0x00FF);


		IINCHIP_SpiSendData(0);
		buf[idx] = IINCHIP_SpiRecvData();

		IINCHIP_CSon();                             // CS=0, SPI end 	   
	   }


	return len;
}



/**
@brief	This function is being called by recv() also.

This function read the Rx read pointer register
and after copy the data from receive buffer update the Rx write pointer register.
User should read upper byte first and lower byte later to get proper value.
*/
void recv_data_processing(SOCKET s, uint8_t *data, uint16_t len)
{
	uint16_t ptr;
	ptr = IINCHIP_READ(Sn_RX_RD0(s));
	ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(s) + 1);
#ifdef __DEF_IINCHIP_DBG__
	printf("ISR_RX: rd_ptr : %.4x\r\n", ptr);
#endif
	read_data(s, (uint8_t *)ptr, data, len); // read data
	ptr += len;
	IINCHIP_WRITE(Sn_RX_RD0(s),(uint8_t)((ptr & 0xff00) >> 8));
	IINCHIP_WRITE((Sn_RX_RD0(s) + 1),(uint8_t)(ptr & 0x00ff));
}


/**
@brief	This function is being used for copy the data form Receive buffer of the chip to application buffer.

It calculate the actual physical address where one has to read
the data from Receive buffer. Here also take care of the condition while it exceed
the Rx memory uper-bound of socket.
*/
void read_data(SOCKET s, volatile uint8_t * src, volatile uint8_t * dst, uint16_t len)
{
	uint16_t size;
	uint16_t src_mask;
	uint8_t * src_ptr;

	src_mask = (uint16_t)src & getIINCHIP_RxMASK(s);
	src_ptr = (uint8_t *)(getIINCHIP_RxBASE(s) + src_mask);
	
	if( (src_mask + len) > getIINCHIP_RxMAX(s) ) 
	{
		size = getIINCHIP_RxMAX(s) - src_mask;
		wiz_read_buf((uint16_t)src_ptr, (uint8_t*)dst,size);
		dst += size;
		size = len - size;
		src_ptr = (uint8_t *)(getIINCHIP_RxBASE(s));
		wiz_read_buf((uint16_t)src_ptr, (uint8_t*) dst,size);
	} 
	else
	{
		wiz_read_buf((uint16_t)src_ptr, (uint8_t*) dst,len);
	}
}

/**
@brief	 get socket status
*/
uint8_t getSn_SR(SOCKET s)
{
   return IINCHIP_READ(Sn_SR(s));
}


/**
@brief	 get socket RX recv buf size

This gives size of received data in receive buffer. 
*/
uint16_t getSn_RX_RSR(SOCKET s)
{
	uint16_t val=0,val1=0;
	do
	{
		val1 = IINCHIP_READ(Sn_RX_RSR0(s));
		val1 = (val1 << 8) + IINCHIP_READ(Sn_RX_RSR0(s) + 1);
      if(val1 != 0)
		{
   			val = IINCHIP_READ(Sn_RX_RSR0(s));
   			val = (val << 8) + IINCHIP_READ(Sn_RX_RSR0(s) + 1);
		}
	} while (val != val1);
   return val;
}

uint16_t getIINCHIP_RxMAX(uint8_t s)
{
   return RSIZE[s];
}
uint16_t getIINCHIP_TxMAX(uint8_t s)
{
   return SSIZE[s];
}
uint16_t getIINCHIP_RxBASE(uint8_t s)
{
   return RBUFBASEADDRESS[s];
}
uint16_t getIINCHIP_TxBASE(uint8_t s)
{
   return SBUFBASEADDRESS[s];
}

void getSn_DIPR(SOCKET s, uint8_t * addr)
{
	addr[0] = IINCHIP_READ(Sn_DIPR0(s));
	addr[1] = IINCHIP_READ(Sn_DIPR0(s)+1);
	addr[2] = IINCHIP_READ(Sn_DIPR0(s)+2);
	addr[3] = IINCHIP_READ(Sn_DIPR0(s)+3);
}

/**
@brief	get socket TX free buf size

This gives free buffer size of transmit buffer. This is the data size that user can transmit.
User shuold check this value first and control the size of transmitting data
*/
uint16_t getSn_TX_FSR(SOCKET s)
{
	uint16_t val=0,val1=0;
	do
	{
		val1 = IINCHIP_READ(Sn_TX_FSR0(s));
		val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR0(s) + 1);
      if (val1 != 0)
		{
   			val = IINCHIP_READ(Sn_TX_FSR0(s));
   			val = (val << 8) + IINCHIP_READ(Sn_TX_FSR0(s) + 1);
		}
	} while (val != val1);
   return val;
}

void setMR(uint8_t val)
{
	IINCHIP_WRITE(MR,val);
}
uint16_t getIINCHIP_RxMASK(uint8_t s)
{
   return RMASK[s];
}
uint16_t getIINCHIP_TxMASK(uint8_t s)
{
   return SMASK[s];
}
