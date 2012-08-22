//
// file: taskPrototypes.h
//

#ifndef _USARTTASKS_H_
#define _USARTTASKS_H_

#define comNO_BLOCK		( ( portTickType ) 0 )

#define MAX_QUEUES 7

typedef enum
{
	unused           = 0,

	C1_channelNo  = 1,
//	D0_C0_channelNo  = 2,
	D1_C0_channelNo  = 3,
	E0_channelNo  = 4,

//	C0_D0_channelNo  = 5,
//	D1_D0_channelNo  = 6,

	printf_ChannelNo = 7  // must be = MAX_QUEUES !!!
} eChIndex;



// Channel queues
// these queues hold the datagrams ready to be sent out
//
extern xQueueHandle vUSART_C1_C0_channelQueue;
extern xQueueHandle vUSART_D0_C0_channelQueue;
extern xQueueHandle vUSART_D1_C0_channelQueue;
extern xQueueHandle vUSART_E0_C0_channelQueue;

extern xQueueHandle vUSART_C0_D0_channelQueue;
extern xQueueHandle vUSART_D1_D0_channelQueue;

extern xQueueHandle vPrintf_channelQueue;

// Multiplexer queues, determines what channel comes next
//
extern xQueueHandle C0_ChannelDispatchQueue;
extern xQueueHandle D0_ChannelDispatchQueue;

extern xQueueHandle channelIndex[];




void send2multiplexer(u8 dataByte,xQueueHandle muxQueue, eChIndex channelNo, xQueueHandle dispatchQueue, u32* counter);

void vUSARTC0_Send(void *pvParameters);
void vUSARTC0_Receive(void *pvParameters);

void vUSARTC1_Send(void *pvParameters);
void vUSARTC1_Receive(void *pvParameters);
void vUSARTC1_Receive_Nexusbus(void *pvParameters);

void vUSARTD0_Send(void *pvParameters);
void vUSARTD0_Receive(void *pvParameters);

void vUSARTD1_Receive(void *pvParameters);

void vUSARTE0_Receive(void *pvParameters);

void vADCTask(void *pvParameters);

void initChannelQueues(void);


#endif // _USARTTASKS_H_

