//
// file: usarttasks.h
//
// xmega demo program VG Feb,Mar,Oct 2011
//
// CPU:   atxmega32A4, atxmega128a1
//
// Version: 1.2.1
//
#ifndef _USARTTASKS_H_
#define _USARTTASKS_H_

void vUSARTC0_Send(void *pvParameters);
void vUSARTC1_Receive(void *pvParameters);
void vUSARTE0_Receive(void *pvParameters);
void initChannelQueues(void);

#endif // _USARTTASKS_H_

