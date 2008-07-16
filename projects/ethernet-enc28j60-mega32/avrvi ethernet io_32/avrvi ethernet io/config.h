#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "build.h"

/////////////////////////////// PASSWORD SETUP ////////////////////////////////////////////
#define HTTPD_ADMIN_AUTH_LOGIN "admin"
#define HTTPD_ADMIN_AUTH_PASS  "1eth1"

/////////////////////////////// NETWORK SETUP ////////////////////////////////////////////

//IP address of _this_ server:
#define NIC_IP_ADDRESS             IP(192,168,1,8)

//Netmask of _this_ network:
#define NIC_IP_NETMASK             IP(255,255,255,0)

//IP address of default gateway:
#define NIC_GATEWAY_IP_ADDRESS     IP(192,168,1,1)


//NTP Server ip: 64.142.28.65
#define NTP_SERVER_IP_ADDRESS      IP(64,142,28,65)

//nic ethernet address. i picked an address that is free for experimental/private use (really?)
//see the last line: http://standards.ieee.org/regauth/oui/oui.txt
//-> AC-DE-48-xx-xx-xx
/*
#define NIC_MAC0 0xAC  
#define NIC_MAC1 0xDE
#define NIC_MAC2 0x48
#define NIC_MAC3 0xAA
#define NIC_MAC4 0xBB
#define NIC_MAC5 0xCC
*/

#define NIC_MAC0 0x00  
#define NIC_MAC1 0x56
#define NIC_MAC2 0x4C
#define NIC_MAC3 0x41
#define NIC_MAC4 0x42
#define NIC_MAC5 0x30



/////////////////////////////// PIN SETUP ////////////////////////////////////////////
//-> io/dataflash
#define DATAFLASH_PIN_CS  3
#define DATAFLASH_PIN_RST 0
#define DATAFLASH_PORT    PORTB
#define DATAFLASH_DDR     DDRB

//-> io/softuart
#define SOFTUART_PORT PORTC
#define SOFTUART_DDR  DDRC
#define SOFTUART_TXPIN 1

//-> enc28j60
#define ENC28J60_PORT	  PORTB
#define ENC28J60_DDR	  DDRB
#define ENC28J60_PIN_CS	2
#define ENC28J60_PIN_RESET	1

//-> status led
#define LED_PORT PORTC
#define LED_DDR   DDRC
#define LED_PIN      0

//-> mca25
#define MCA25_RESET_PORT     PORTD
#define MCA25_RESET_PORT_DIR DDRD
#define MCA25_RESET_PIN      5

//-> mpf10
#define MPF10_ENABLE_PORT PORTD
#define MPF10_ENABLE_DDR   DDRD
#define MPF10_ENABLE_PIN      4

//-> servo (use timer output OCR2!)
#define SERVO_PORT        PORTD
#define SERVO_DDR          DDRD
#define SERVO_PIN             7

//-> lm75
//#define LM75_DEVICE_ID 0x9E
#define LM75_DEVICE_ID 0x90

/////////////////////////////// HELPERS ////////////////////////////////////////////
//DO NOT CHANGE ANYTHING BELOW!

//convert config ip to long:
#define IP(a3,a2,a1,a0) ((((unsigned long)(a3))<<24)|(((unsigned long)(a2))<<16)|(((unsigned long)(a1))<<8)|(unsigned long)(a0))

/*#define NIC_IP_ADDRESS \
				(((uint32_t)NIC_IP_ADDRESS_0)<<24 | \
				((uint32_t)NIC_IP_ADDRESS_1)<<16 | \
				((uint32_t)NIC_IP_ADDRESS_2)<< 8 | \
				((uint32_t)NIC_IP_ADDRESS_3))

#define NIC_IP_NETMASK \
				(((uint32_t)NIC_IP_NETMASK_0)<<24 | \
				((uint32_t)NIC_IP_NETMASK_1)<<16 | \
				((uint32_t)NIC_IP_NETMASK_2)<< 8 | \
				((uint32_t)NIC_IP_NETMASK_3))


#define NIC_GATEWAY_IP_ADDRESS \
				(((uint32_t)NIC_GATEWAY_IP_ADDRESS_0)<<24 | \
				((uint32_t)NIC_GATEWAY_IP_ADDRESS_1)<<16 | \
				((uint32_t)NIC_GATEWAY_IP_ADDRESS_2)<< 8 | \
				((uint32_t)NIC_GATEWAY_IP_ADDRESS_3))

#define NTP_SERVER_IP_ADDRESS \
				(((uint32_t)NTP_SERVER_IP_ADDRESS_0)<<24 | \
				((uint32_t)NTP_SERVER_IP_ADDRESS_1)<<16 | \
				((uint32_t)NTP_SERVER_IP_ADDRESS_2)<< 8 | \
				((uint32_t)NTP_SERVER_IP_ADDRESS_3))
*/

#define sbi(reg,bit)    reg |= (1<<(bit));
#define cbi(reg,bit)    reg &= ~(1<<(bit));
#define MIN(a,b)        ((a<b)?(a):(b))

#define lo8(x) ((x   )&0xFF)
#define hi8(x) (((x)>>8)&0xFF)


#ifndef F_CPU
	#define F_CPU F_OSC
#endif


#endif

