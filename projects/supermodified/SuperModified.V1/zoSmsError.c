#include "zoSmsError.h"
#include "zoMcu.h"
#include "zoSystemTimer.h"
#include "zoCommands.h"
#include "zoMotor.h"
#include "zoEncoder.h"
#include "zoSms.h"
#include "zoProtocolI2c.h"
#include "zoProtocolUart.h"
#include "zoI2c.h"
#include "zoUart.h"

ZO_ERROR zoSmsError = ZO_ERROR_DEFAULTS;
ZO_ERROR zoSmsWarning = ZO_ERROR_DEFAULTS;

static s32 StalledLastPosition;
static u16 StalledTimeOutTimer;

void zoSmsErrorInit(void)
{
	//initialize error buffers
	zoErrorInit(&zoSmsError);
	zoErrorInit(&zoSmsWarning);

	//Initialize the system timer
	zoSystemTimerInit();
	//initialize the timeout for the motor stall regognition function
	zoSystemTimerTimeOutInit(&StalledTimeOutTimer);
}

void zoSmsErrorPoll(void)
{
	//dummy packet pointer to enable calling of command functions
	//attention: only commands of bytecount = 0  can be called
	ZO_PROTOCOL_PACKET *dummy = 0;
	ZO_SMS_CONTROL_STATE state;
	
	enterCritical();
	state = Sms.Control.State;
	exitCritical();

	//these errors can occur only when the controller is initialized
	//no point of checking them when it is not
	if( state != ZO_SMS_CONTROL_NO_CONTROL )
	{
		//these are very serious error conditions: motor is stopped if they are encountered
		if( zoSmsMotorStalled() )
		{
			zoCommandStop(dummy);
			zoErrorPut(&zoSmsError,ZO_SMS_ERROR_MOTOR_STALLED);
		}

		if( zoEncoderPollOverFlowError() )
		{
			zoCommandStop(dummy);
			zoErrorPut(&zoSmsError,ZO_SMS_ERROR_ENCODER_OVERFLOW);
		}

		if( zoEncoderPollUnderFlowError() )
		{
			zoCommandStop(dummy);
			zoErrorPut(&zoSmsError,ZO_SMS_ERROR_ENCODER_UNDERFLOW);
		}
	}
}

void zoSmsErrorHandleParseFailure(ZO_PROTOCOL_HAL *hal,ZO_PROTOCOL_PACKET *p)
{
	u08 err;
	bool issueCommandResponseNow = FALSE;
	
	while( !zoErrorIsEmpty(hal->error) )
	{
		err = zoErrorGet(hal->error);

		if(err > ZO_PROTOCOL_ERROR_NONE)			//process protocol errors first
		{
			zoErrorPut(&zoSmsError,err + ZO_SMS_ERROR_PROTOCOL_ERRORS_OFFSET);
						
			if(err == ZO_PROTOCOL_ERROR_WRONG_LRC)  //if it was wrong lrc issue response
				issueCommandResponseNow = TRUE;		//immediately
		}
		else
		{
			switch(hal->hw)
			{
			case ZO_PROTOCOL_HW_I2C:
				zoErrorPut(&zoSmsError,err + ZO_SMS_ERROR_I2C_ERRORS_OFFSET);
				if(err == ZO_PROTOCOL_I2C_ERROR_RX_PACKET_OVERWRITTEN)
					issueCommandResponseNow = TRUE;
				if(err == ZO_PROTOCOL_I2C_ERROR_INVALID_RX_BYTECOUNT)
					issueCommandResponseNow = TRUE;
				break;

			case ZO_PROTOCOL_HW_RS232:
			case ZO_PROTOCOL_HW_HALF_DUPLEX_RS485:
			case ZO_PROTOCOL_HW_UART_5V:
				zoErrorPut(&zoSmsError,err + ZO_SMS_ERROR_UART_ERRORS_OFFSET);
				break;

			default:
				break;
			}
		}
	}

	if (issueCommandResponseNow)
		zoProtocolCommandResponse(hal,p,&zoSmsError);
}

void zoSmsErrorHandleServiceCommandFailure(void)
{
	u08 err;
	
	while( !zoErrorIsEmpty(&zoCommandsError) )
	{
		err = zoErrorGet(&zoCommandsError);

		if(err <= ZO_COMMANDS_ERROR_INVALID_COMMAND)
			zoErrorPut(&zoSmsError,err + ZO_SMS_ERROR_COMMAND_ERRORS_OFFSET);
		else
		{
			if( Sms.Settings.errorReportingLevel == 0 )
				zoErrorPut(&zoSmsWarning,err + ZO_SMS_ERROR_COMMAND_ERRORS_OFFSET);
			else
				zoErrorPut(&zoSmsError,err + ZO_SMS_ERROR_COMMAND_ERRORS_OFFSET);
		}
	}
}		

void zoSmsErrorHandleCommandResponseFailure(ZO_PROTOCOL_HAL *hal,ZO_PROTOCOL_PACKET *p)
{
	u08 i,err;

	//This is a point in code that should not be reached easily. It happens only if there 
	//is a problem transmitting a response. This can be due to arbitration loss or full Tx
	//buffers etc. Software retries 5 times to issue the response, then gives up.
	for(i=0;i<5;i++)
	{
		while(!zoErrorIsEmpty(hal->error))	
		{
			err = zoErrorGet(hal->error);
			
			switch(hal->hw)
			{
			case ZO_PROTOCOL_HW_I2C:
				if(err == ZO_I2C_ERROR_ARBITRATION_LOST)
				{
					if(Sms.Settings.errorReportingLevel == 0)
						zoErrorPut(&zoSmsWarning,err + ZO_SMS_ERROR_I2C_ERRORS_OFFSET);
					else
						zoErrorPut(&zoSmsError,err + ZO_SMS_ERROR_I2C_ERRORS_OFFSET);
				}
				else
					zoErrorPut(&zoSmsError,err + ZO_SMS_ERROR_I2C_ERRORS_OFFSET);
				break;

			case ZO_PROTOCOL_HW_RS232:
			case ZO_PROTOCOL_HW_HALF_DUPLEX_RS485:
			case ZO_PROTOCOL_HW_UART_5V:
				zoErrorPut(&zoSmsError,err + ZO_SMS_ERROR_UART_ERRORS_OFFSET);
				break;

			default:
				break;
			}
		}

		//If this retry of responding is successfull stop retrying to respond
		if( zoProtocolCommandResponse(hal,p,&zoSmsError) )
			return;
	}
}


//this should be called only if the motor is initialized
bool zoSmsMotorStalled(void)
{
	s32 currentPos;
	s32 lastPos;
	s32 commandedPos;

	enterCritical();
	lastPos = StalledLastPosition;
	currentPos = zoEncoderGetPos();
	StalledLastPosition = currentPos;
	commandedPos = Sms.Pid.SetPoint; 
	exitCritical();

	//if the motor is not enabled or if it is approximately here it was commanded to be
	//then it is not stalled	
	if( labs(currentPos - commandedPos) < 20 )
	{
		zoSystemTimerTimeOutInit(&StalledTimeOutTimer);
		return FALSE;		
	}

	//if the motor is still or moving away from the target then it is stalled. 
	//If it remains like this for more than a specific time then indicate the 
	//stalled condition.
	if( labs(currentPos - commandedPos) >= labs(lastPos - commandedPos) )
	{
		if( zoSystemTimerTimeOutExpired(&StalledTimeOutTimer,ZO_SMS_MOTOR_STALLED_TIMEOUT_MS) )
			return TRUE;
	}

	return FALSE;
}