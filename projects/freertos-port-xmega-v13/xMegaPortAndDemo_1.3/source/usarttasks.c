//
// file: usarttasks.c
//
// xmega demo program VG Feb,Mar,Oct 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
// Version: 1.2.1
//
#include "all.h"

typedef void tskTCB;
extern volatile tskTCB * volatile pxCurrentTCB;

#define comNO_BLOCK		( ( portTickType ) 0 )


extern xTaskHandle hUSARTC0_Send;
extern xTaskHandle hUSARTE0_Receive;


// Channel queues
// these queues hold the datagrams ready to be sent out
//
xQueueHandle vUSARTC1_channelQueue;
xQueueHandle vUSARTE0_channelQueue;

// This channel holds one entry per datagram that is ready to be sent.
// Each entry is an index to the channel queue handle via 
// the channelIndex array.
//
xQueueHandle vChannelDispatchQueue;

// indexing via an array from channel number to channel queue
// note: only indexes 1..4 are used
xQueueHandle channelIndex[5];


u32 numDatagramsSent_E0;
u32 numDatagramsSent_C1;
u32 bytesReceived_E0;
u32 bytesReceived_C1;
u32 bytesSent_C0;


//-----------------------------------------------------------
// This is the sending task for USARTC0 ( RXD0, TXD0 ) 
//
// Whenever the corresponding queue ("outputQueue") has a char available
// this task is waked up, receives the char and sends its out.
// 

void vUSARTC0_Send(void *pvParameters)
{
	// Just to stop compiler warnings.
	(void) pvParameters;

	uint8_t      dataByte;
	xQueueHandle currentQueue;

	for(;;)
	{
		// where is the next datagram available?
		if (xQueueReceive( vChannelDispatchQueue, &dataByte, portMAX_DELAY ))
		{
			if(dataByte==0 || dataByte>4)
				error(ERR_INVALID_CHANNELINDEX);
			
			// read and send the complete datagram from the indexed queue
			currentQueue = channelIndex[dataByte];

			do
			{
				// no blocking, there MUST be bytes available, otherwise error
				if(xQueueReceive( currentQueue, &dataByte, comNO_BLOCK ))
				{
					if(!USART_TXBuffer_PutByte(&USARTC0_data, dataByte, portMAX_DELAY))
						error(ERR_QUEUE_SEND_FAILED);
					bytesSent_C0++;
				}
				else
					error(ERR_BYTES_SHOULD_BE_AVAILABLE);
			}while( dataByte != DG_EOM); // datagrams end with EOM
		}
		else
		{
			error(ERR_QUEUE_RECEIVE_FAILED);
		}
	}
}

//-----------------------------------------------------------
// 
// This is the working task for USARTC1 ( RXD1, TXD1 ):
//
// Whenever the corresponding input queue has a char available
// this task is waked up, receives the char, converts the data
// and and sends it out to the output queue.
//

void vUSARTC1_Receive(void *pvParameters)
{
	// Just to stop compiler warnings.
	(void) pvParameters;

	uint8_t dataByte;

	numDatagramsSent_C1 = 0;
	bytesReceived_C1 = 0;

	for( ;; )
	{
		USART_RXBuffer_GetByte(&USARTC1_data, &dataByte, portMAX_DELAY);
		bytesReceived_C1++;


        // some processing ...


		// do not expect to block here ever when sending
		if (xQueueSend( vUSARTC1_channelQueue, &dataByte, portMAX_DELAY ) != pdPASS)
		{
			error(ERR_QUEUE_SEND_FAILED);
		}

		if( dataByte == DG_EOM ) // each datagram ends with a EOM
		{
			uint8_t channelNo;
			// we have a new complete datagram ready to send
			channelNo = C1_channelNo;
			if (xQueueSend( vChannelDispatchQueue, &channelNo, portMAX_DELAY ) != pdPASS)
			{
				error(ERR_DISPATCH_QUEUE_FULL);
			}
			numDatagramsSent_C1++;
		}
	}

}


//-----------------------------------------------------------
// 
// This is the working task for USARTE0 ( RXD1, TXD1 ):
//
// Whenever the corresponding input queue has a char available
// this task is waked up, receives the char, converts the data
// and and sends it out to the output queue.
//

void vUSARTE0_Receive(void *pvParameters)
{
	// Just to stop compiler warnings.
	(void) pvParameters;

	uint8_t dataByte;

	numDatagramsSent_E0 = 0;
	bytesReceived_E0 = 0;

	for( ;; )
	{
		USART_RXBuffer_GetByte(&USARTE0_data, &dataByte, portMAX_DELAY);
		bytesReceived_E0++;


        // some processing ...


		// do not expect to block here ever when sending
		if (xQueueSend( vUSARTE0_channelQueue, &dataByte, portMAX_DELAY ) != pdPASS)
		{
			error(ERR_QUEUE_SEND_FAILED);
		}

		if( dataByte == DG_EOM ) // each datagram ends with a EOM
		{
			uint8_t channelNo;
			// we have a new complete datagram ready to send
			channelNo = E0_channelNo;
			if (xQueueSend( vChannelDispatchQueue, &channelNo, portMAX_DELAY ) != pdPASS)
			{
				error(ERR_DISPATCH_QUEUE_FULL);
			}
			numDatagramsSent_E0++;
		}
	}

}

//-----------------------------------------------------------
//
// There is one channel queue per serial receiver.
//
void initChannelQueues(void)
{
	const unsigned portBASE_TYPE chQueueLength       = CHANNEL_QUEUE_LENGTH;  
	const unsigned portBASE_TYPE dispatchQueueLength = 5;   


	if((vUSARTC1_channelQueue = xQueueCreate(chQueueLength, (unsigned portBASE_TYPE ) sizeof(signed char))) == NULL)
		error(ERR_QUEUE_CREATE_HANDLE_NULL);

	if((vUSARTE0_channelQueue = xQueueCreate(chQueueLength, (unsigned portBASE_TYPE ) sizeof(signed char))) == NULL)
		error(ERR_QUEUE_CREATE_HANDLE_NULL);

	channelIndex[C1_channelNo] = vUSARTC1_channelQueue;
	channelIndex[E0_channelNo] = vUSARTE0_channelQueue;


	// The dispatch queue tells which channel has the next 
	// datagram complete to send.
	if((vChannelDispatchQueue = xQueueCreate(dispatchQueueLength, (unsigned portBASE_TYPE ) sizeof(signed char))) == NULL)
		error(ERR_QUEUE_CREATE_HANDLE_NULL);
}

// eof usarttasks.c


