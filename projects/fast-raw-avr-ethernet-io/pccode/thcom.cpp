// Written by Rogier Schouten, GPL V2
// Test code that sends 256 packets to the AVR and expects the same
// packets back. All packets have 1 byte that alternates between 0 and 255.


#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <netinet/in.h>

#define ETH_DATASIZE 1
#define ETH_PAYLOADLEN 1  // data + padding, should actually be >= 46 to conform to Ethernet standard
#define ETH_PACKET_LEN (18+ETH_PAYLOADLEN) // header (14) + data + FCS (4)  
#define ETH_DEST_MAC {0x52, 0x4f, 0x47, 0x49, 0x45, 0x52}  // "ROGIER"  
#define ETH_DEVICE "eth0"


int main(int argc, char **argv)
{
	void* buffer = (void*)malloc(ETH_PACKET_LEN); 	/*Buffer for ethernet frame*/
	unsigned char* data = &static_cast<unsigned char*>(buffer)[14];	/*Userdata in ethernet frame*/
	unsigned char src_mac[6];		/*our MAC address*/
	unsigned char dest_mac[6] = ETH_DEST_MAC;	/*other host MAC address, hardcoded...... :-(*/
	struct ifreq ifr;
	int ifindex = 0;			/*Ethernet Interface index*/
	struct sockaddr_ll socket_address;
	struct timeval begintime;
   struct timeval endtime;
	

	// open socket
	int mysocket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (mysocket == -1)
	{
		perror("opening socket failed");
		exit(1);
	}

	// retrieve ethernet interface index
	strncpy(ifr.ifr_name, ETH_DEVICE, IFNAMSIZ);
	if (ioctl(mysocket, SIOCGIFINDEX, &ifr) == -1)
	{
		perror("retrieve ethernet interface index failed");
		exit(1);
	}
	ifindex = ifr.ifr_ifindex;

	// retrieve corresponding MAC
	if (ioctl(mysocket, SIOCGIFHWADDR, &ifr) == -1)
	{
		perror("retrieve corresponding MAC failed");
		exit(1);
	}
	for (int i = 0; i < 6; i++)
	{
		src_mac[i] = ifr.ifr_hwaddr.sa_data[i];
	}
	printf("Successfully got our MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
	       src_mac[0],src_mac[1],src_mac[2],src_mac[3],src_mac[4],src_mac[5]);

	// set receive timeout
   struct timeval timeout;
	timerclear(&timeout);
	timeout.tv_sec = 1;	
	if (-1 == setsockopt(mysocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
	{
		std::cerr << "setsockopt() failed" << std::endl;
	}
	
	/*prepare sockaddr_ll*/
	socket_address.sll_family   = PF_PACKET; // RAW communication
	socket_address.sll_protocol = htons(ETH_DATASIZE); // Used as Length field 
	socket_address.sll_ifindex  = ifindex;
	socket_address.sll_hatype   = ARPHRD_ETHER; // ARP hardware identifier is ethernet
	socket_address.sll_pkttype  = PACKET_OTHERHOST; // target is another host
	socket_address.sll_halen    = ETH_ALEN; // address length
	socket_address.sll_addr[0]  = dest_mac[0];
	socket_address.sll_addr[1]  = dest_mac[1];
	socket_address.sll_addr[2]  = dest_mac[2];
	socket_address.sll_addr[3]  = dest_mac[3];
	socket_address.sll_addr[4]  = dest_mac[4];
	socket_address.sll_addr[5]  = dest_mac[5];
	socket_address.sll_addr[6]  = 0x00;
	socket_address.sll_addr[7]  = 0x00;

	// send packets while the user wants us to
	bool done = false;
	timerclear(&begintime);
	timerclear(&endtime);
	gettimeofday(&begintime, NULL);
	
	for (int i = 0; i < 256; i++)
	{
		// send data
		memcpy((void*)buffer, (void*)dest_mac, ETH_ALEN);
		memcpy((void*)(&static_cast<unsigned char*>(buffer)[ETH_ALEN]), (void*)src_mac, ETH_ALEN);
		static_cast<unsigned char*>(buffer)[2 * ETH_ALEN] = 0;
		static_cast<unsigned char*>(buffer)[2 * ETH_ALEN + 1] = ETH_DATASIZE;
		data[0] = (unsigned char)i;
		int send_result = 0;
		send_result = sendto(mysocket, buffer, ETH_PACKET_LEN, 0,
									(struct sockaddr*)&socket_address, sizeof(socket_address));
		if (send_result == -1)
		{
			std::cerr << "sending packet failed" << std::endl;
			done = true;
			continue;
		}
		
		// receive data
		int length = recv(mysocket, buffer, ETH_PACKET_LEN, 0);
		if (length == -1) {
			std::cerr << "timeout" << std::endl;
			exit(1);
		}
		else if (data[0] != (unsigned char)i)
		{
			std::cerr << "wrong data" << std::endl;
		}
	}

	// get total time spent
	gettimeofday(&endtime, NULL);		
   struct timeval result;
	timerclear(&result);
	timersub(&endtime, &begintime, &result);
	unsigned long microsecs = ((result.tv_sec * 1000000 ) + result.tv_usec );
	std::cout << "Sending 256 packets took " << microsecs << " usec" << std::endl;
	std::cout << "Average/packet: " << (microsecs/256) << " usec" << std::endl;
	
	close(mysocket);
	return 0;
}

