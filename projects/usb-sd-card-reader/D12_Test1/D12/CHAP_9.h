#ifndef __CHAP_9_H__
#define __CHAP_9_H__

#include "USB_Descriptor.h"
#include "D12_Operation.h"


void Send_Descriptor(void);
U8 Get_Status(U8 *SetupPkt);
U8 Clear_Feature(U8 *SetupPkt);
U8 Set_Feature(U8 *SetupPkt);
U8 Set_Address(U8 *SetupPkt);
U8 Get_Descriptor(U8 *SetupPkt);
U8 Get_Configuration(U8 *SetupPkt);
U8 Set_Configuration(U8 *SetupPkt);
U8 Get_Interface(U8 *SetupPkt);
U8 Set_Interface(U8 *SetupPkt);
U8 Reserved(U8 *SetupPkt);
U8 Remain_Descriptor_Flag(void);
void Clear_Remain_Descriptor_Flag(void);

#endif
