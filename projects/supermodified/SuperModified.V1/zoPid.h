#ifndef ZO_PID_H
#define ZO_PID_H

#include "zoTypes.h"

struct ZO_PID{
	u16 GainP;
	u16 GainI;
	u16 GainD;
	s32 Error;
	s32 PrevError;
	s32 Integral;
	s32 AntiWindUp;
	s32 SetPoint;
	void ( *doPID )(struct ZO_PID *selfReference);
	void ( *setActuator )(s32 outVal);					// user provided function
	s32 ( *getFeedBack )(void);							// user provided function
};

#define ZO_PID_DEFAULTS \
		{0,0,0,0,0,0,0,0, \
		(void (*)(struct ZO_PID *))zoPidDoPid, \
		0,0}

void zoPidDoPid( struct ZO_PID *p);


#endif // ZO_PID_H