#include "USART.h"
#include "RX.h"
#include <avr/interrupt.h>

//Baud rate 115200	

//Extern
volatile unsigned char Packet_RX_ed = 0;
volatile unsigned char RX_Over_Timed = 0;

unsigned char Sync_State = 0;
unsigned char Msg_Pointer = 0;


void Enable_UART (void)
{
	UBRR0 	= 8;
	UCSR0C 	= 0b00000110;	//Asynchronous USART, No Parity, 1 Stop bit
	UCSR0B 	= 0b00011000;	//Enable TX/RX, no interrupt (not set here),8bit
	bit_set( UCSR0A, BIT(U2X0));

	Enable_UART_TX;
	Enable_UART_RX;
	Enable_UART_RXC_ISR;	//interrupt driven RX
	
}	//Enable_UART()


void Add_To_RX_Buffer ( unsigned char inp_Byte )
{
	if ( Packet_RX_ed == 1 )
		return;
	
	switch ( Sync_State )
	{
		case 0:
		{ 	
			Msg_Pointer = 0;
			RX_Over_Timed = 0;
			
			if ( inp_Byte == Sync_Byte )
			{
				Message[0] = Sync_Byte;
				Msg_Pointer = 1;
				Sync_State++;
				Start_RX_Over_Timer;
			}
		
			break;
		}
			
		case 1:
		{ 

			if ( RX_Over_Timed == 1 )
			{
				Sync_State = 0;
				RX_Over_Timed = 0;
				Msg_Pointer = 0;	//safety
			}
			else
			{
				Message[Msg_Pointer] = inp_Byte; 
				Msg_Pointer++;
			}
				
			if ( Msg_Pointer == Message_Length )
			{

				Packet_RX_ed = 1;
				Stop_RX_Over_Timer;
				RST_RX_Over_Timer;
				Sync_State = 0;
			}
			else
				Start_RX_Over_Timer;
			
		
			break;
		}
			
		default:	//Error
			break;	
	
	}	//switch

}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

ISR (USART_RX_vect)		//RX Complete Handler
{	
	//interrupt driven RX

	//Make it short as possible
	Add_To_RX_Buffer( UDR0 );

}	//USART_RXC_vect
