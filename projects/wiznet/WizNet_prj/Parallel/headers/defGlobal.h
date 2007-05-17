#include <avr/io.h>
#include "headers/defStruct.h"


/*	Определение основных сетевых настроек */
#define MASKNET		255,255,255,0			// маска подсети
#define IPGATEWAY	192,168,0,1				// IP адрес шлюза
#define IPDEV		192,168,0,200			// IP адрес локального устройства WIZnet
#define MACDEV		00,0x77,0x99,0x55,0x66,0x11		// MAC адрес локального устройства WIZnet
#define PORTDEV		0x27,0x0f				// SPR 9999 (Source Port Register) порт локального устройства
#define IPREMOT		192,168,0,1				// IP адрес удаленного устройства WIZnet
#define PORTREMOT	0x27,0x0f				// DPR 9999 (Destination Port Register) порт удаленного устройства

#define PROTOCOL	0x02					// SOPR (Socket Option and Protocol Register) UDP

/****************************************************************************/
/****************************************************************************
  Global structs definitions
****************************************************************************/
struct TD	Tdata;		//структура для управления настройкой модуля WIZnet

struct ETH_status	ETH_stat;		
struct Request		ETH_Send;
struct Request		ETH_Rec;
/****************************************************************************
  Global variables definitions
****************************************************************************/
//массив сетевых установок модуля WIZnet, используется после рестарта.
const unsigned char Net_Settings[23] =
{
	IPGATEWAY,          	// Gateway IP-address
	MASKNET,	    	 	// Subnet Mask 255,255,255,0,
	MACDEV,					// MAC-address for WIZnet (не используется)
	IPDEV,        			// IP-address 192,168,0,200
	
	0x0F, 0xA0,   			// TimeOut for Retransmission (0xFA0 = 400ms)
	10,						// Retry count
	0x03,					// RX Data Memory sizes for Channels (see Datasheet)0x03-8k
	0x03					// TX Data Memory sizes for Channels (see Datasheet)0x03-8k
};
//массив установок сокета модуля WIZnet, используется после рестарта.
const unsigned char Socket_Settings[19] =
{
	PROTOCOL,      			// SOPR (Socket Option and Protocol Register) UDP
	0,0,0,0,0,0,			// Reserv
	IPREMOT,				// DIR (Destination IP Address Register)
	PORTREMOT,				// DPR (Destination Port Register)
	PORTDEV,				// SPR (Source Port Register)
	0x11,					// IPR (IP Protocol Register) the Protocol Field of IP Header
	0,						// TOSR (TOS Register) (Type Of Service) Field of IP Header,
	0x05,0xB4				// MSSR (MSS Register) (Maximum Segment Size) =1460
};
//массив пользовательских сетевых установок и установок сокета модуля WIZnet.
unsigned char UNet_Settings[23]=		//массив пользовательских сетевых установок
{
	IPGATEWAY,          	// Gateway IP-address
	MASKNET,	    	 	// Subnet Mask 255,255,255,0,
	MACDEV,					// MAC-address for WIZnet (не используется)
	IPDEV,        			// IP-address 192,168,0,200
	
	0x0F, 0xA0,   			// TimeOut for Retransmission (0xFA0 = 400ms)
	10,						// Retry count
	0x03,					// RX Data Memory sizes for Channels (see Datasheet)0x03-8k
	0x03					// TX Data Memory sizes for Channels (see Datasheet)0x03-8k
};
unsigned char USocket_Settings[19]=	//массив пользовательских установок сокета
{
	PROTOCOL,      			// SOPR (Socket Option and Protocol Register) UDP
	0,0,0,0,0,0,			// Reserv
	IPREMOT,				// DIR (Destination IP Address Register)
	PORTREMOT,				// DPR (Destination Port Register)
	PORTDEV,				// SPR (Source Port Register)
	0x11,					// IPR (IP Protocol Register) the Protocol Field of IP Header
	0,						// TOSR (TOS Register) (Type Of Service) Field of IP Header,
	0x05,0xB4				// MSSR (MSS Register) (Maximum Segment Size) =1460
};
unsigned char UserIPdestination[4];	//новый destination IP модуля
/***************************************************************************/
/***************************************************************************/
unsigned char TPoint_Settings[4]={0x00,0x00,0x1F,0xFF};
unsigned char RPoint_Settings[4]={0x00,0x00,0x1F,0xFF};
/***************************************************************************/
unsigned char ethTRbuff[ETH_TR_SIZE];		// ETH_TR_SIZE=32 буфер для сохранения данных которые приложение приготовило для передачи по ethernet
unsigned char ethRCbuff[ETH_RC_SIZE];		// ETH_RC_SIZE=40 буфер для сохранения данных которые пришли по сети ethernet должен быть на 8 байт больше
											// буфера на передачу, так как включает заголовок: длина-2, ip-4, port-2.(длина всего пакета вместе с заголовком)

volatile unsigned char time_out[NT]={12,12,12,12};			// массив переменных для организации задержек
