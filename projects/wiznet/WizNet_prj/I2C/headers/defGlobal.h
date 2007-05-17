#include <avr/io.h>
#include <compat/twi.h>
#include "headers/defStruct.h"
#include "headers/defConstant.h"

/***************************************************************************/
/*	Определение основных сетевых настроек */
#define MASKNET		255,255,255,0			// маска подсети
#define IPGATEWAY	192,168,0,1				// IP адрес шлюза
#define IPDEV		192,168,0,200			// IP адрес локального устройства WIZnet
#define MACDEV		00,0x77,0x99,0x55,0x66,0x11		// MAC адрес локального устройства WIZnet (не используется)
#define PORTDEV		0x27,0x0f				// SPR 9999 (Source Port Register) порт локального устройства
#define IPREMOT		192,168,0,17			// IP адрес удаленного устройства WIZnet
#define PORTREMOT	0x27,0x0f				// DPR 9999 (Destination Port Register) порт удаленного устройства

#define PROTOCOL	0x02					// SOPR (Socket Option and Protocol Register) UDP
/****************************************************************************/

/****************************************************************************
  Global structs definitions
****************************************************************************/
struct Request		ETHtr;		//структура для запроса для передачи данных пакета модулем Ethernet
struct Request		ETHrc;		//структура для запроса для обработки принятых данных пакета от модуля Ethernet
struct Descript	ETH_dsc;	//рабочая структура функции обслуживания модуля Ethernet
struct	TWI_status	TWI_stat;	//структура обмена информацией с обработчиком прерываний TWI
/****************************************************************************
  Global variables definitions
****************************************************************************/
/***************************************************************************/
unsigned char ethTRbuff[ETH_TR_SIZE];		// ETH_TR_SIZE=32 буфер для сохранения данных которые приложение приготовило для передачи по ethernet
unsigned char ethRCbuff[ETH_RC_SIZE];		// ETH_RC_SIZE=40 буфер для сохранения данных которые пришли по сети ethernet должен быть на 8 байт б

volatile unsigned char time_out[NT]={12,12,12,12};			// массив переменных для организации задержек

unsigned char Point_Settings[4]={0x00,0x00,0x1F,0xFF};
unsigned char resetState=0;

//массив сетевых установок модуля WIZnet, используется после рестарта.
const unsigned char Net_Settings[23] =
{
	IPGATEWAY,          	// Gateway IP-address
	MASKNET,	    	 	// Subnet Mask 255,255,255,0,
	MACDEV,					// MAC-address for WIZnet
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

