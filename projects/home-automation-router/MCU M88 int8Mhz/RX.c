#include "RX.h"
#include "USART.h"
#include <util/delay.h>
#include "RF.h"

#define mS_Wait_till_CLR	300

//extern
unsigned char Message[Message_Length];

//Prototypes
unsigned char Calc_Checksum ( void );
void Clear_RX_Buffer ( void );
void Set_Outputs (unsigned char, unsigned char, unsigned char );

/* Structure
Message[]
[0]:Sync
[1]: How_Many_Times_To_TX | Msg type
[2]: Data1
[3]: Data2
[4]: Checksum
[5]: <Cr> (0x0D)
*/

unsigned char Validity_Check ( void )	//returns 0 if failed, 1 if passed
{
	//Make sure Sync is valid
	if ( Message[0] != Sync_Byte )
		return 0;
	
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	//Check for valid end
	if ( Message[Message_Length-1] != End_Byte )
		return 0;

	if( Calc_Checksum() != Checksum_Byte )
		return 0;
	
	//Everything was fine, proceed
	return 1;
}


unsigned char Calc_Checksum ( void )
{
	unsigned char sum = 0;
	
	//leave out Sync & Checksum_Byte & End_Byte
	for (unsigned char i=1;i<Message_Length-2;i++)	
		sum += Message[i];		
	
	sum = ~sum;			//Same as XOR 0xFF
	sum++;				//Must add 1
	//sum&= 0xFF;		//only if int!
	
	return sum;
}


void Proccess ( void )
{
	#define How_Many_Times_To_TX	((Message[1] >> 4) + 1)

	//Check MSG type
	//if ( (Message[1] & 0x0F) /*== 1*/ )	//Keep_outputs
	//{
		unsigned int tmp = (Message[2] << 8) | Message[3];

		for (unsigned char i=0;i<How_Many_Times_To_TX;i++)	
			Send_Out_Frame ( tmp );	//Must use tmp!
	//}

}	//Proccess


void UART_RX_Handler( void )
{
	Disable_UART_RX;	//Do not overwrite existing data

	if ( Validity_Check() == 1 )	
		Proccess();
	
	Clear_RX_Buffer();	//For new Msgs
	
	Packet_RX_ed = 0;
	Enable_UART_RX;		//Ready to RX next one
}

void Clear_RX_Buffer ( void )
{
	for (unsigned char i=0;i<Message_Length;i++)
		Message[i] = 0;
}
