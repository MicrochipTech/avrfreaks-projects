#include "zoSmsMaster.h"
#include "zoCommands.h"

ZO_ERROR *zoSmsMasterError = &zoCommandsError;

void zoSmsMasterInit(ZO_PROTOCOL_HW_TYPE hw,ZO_PROTOCOL_HAL *hal, u08 localNodeID)
{
	zoProtocolInit(hw,hal,localNodeID);
	zoCommandsInit(hal);	
}
