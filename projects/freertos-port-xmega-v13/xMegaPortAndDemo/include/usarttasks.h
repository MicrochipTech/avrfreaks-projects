//
// file: usarttasks.h
//

#ifndef _USARTTASKS_H_
#define _USARTTASKS_H_


void vUSARTC0_Send(void *pvParameters);
//void vUSARTC0_Receive(void *pvParameters);

void vUSARTC1_Send(void *pvParameters);;
void vUSARTC1_Receive(void *pvParameters);

void vUSARTD0_Send(void *pvParameters);;
void vUSARTD0_Receive(void *pvParameters);

void vUSARTD1_Receive(void *pvParameters);

void vUSARTE0_Receive(void *pvParameters);

void initChannelQueues(void);


#endif // _USARTTASKS_H_

