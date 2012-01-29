#ifndef USART_H
#define USART_H 1

	//Includes
	
	#include "GlobalDefinitions.h"

	//Prototypes
	extern void Enable_UART (void);

	//Global vars
	extern volatile unsigned char Packet_RX_ed;
	extern volatile unsigned char RX_Over_Timed;

#endif
