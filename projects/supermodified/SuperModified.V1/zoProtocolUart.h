#ifndef ZO_PROTOCOL_UART
#define ZO_PROTOCOL_UART

#include "zoTypes.h"
#include "zoProtocol.h"
#include "zoError.h"
#include "zoUart.h"

#define ZO_PROTOCOL_UART_ERROR_NONE			0x10

extern ZO_ERROR* zoProtocolUartError;

bool zoProtocolUartInit(ZO_PROTOCOL_HW_TYPE hw, u08 ownNodeID, u32 baudRate);

bool zoProtocolUartPutPacket(const ZO_PROTOCOL_PACKET* packet);
bool zoProtocolUartGetPacket(ZO_PROTOCOL_PACKET* packet);
void zoProtocolUartSetLAM(u08 localAcceptanceMask);
bool zoProtocolUartSetBitrate(u32 bitsPerSecond);

#endif // ZO_PROTOCOL_UART
