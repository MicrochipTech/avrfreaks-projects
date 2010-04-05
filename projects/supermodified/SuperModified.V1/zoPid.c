#include "zoPid.h"

void zoPidDoPid( struct ZO_PID *p )
{
	p->PrevError = p->Error;						//store previous error
	p->Error = p->getFeedBack() - p->SetPoint;		//calculate error
	p->Integral += p->Error;							//integration

 	if( p->Integral > p->AntiWindUp )					//anti wind-up implementation
 		p->Integral = p->AntiWindUp;
 	if( p->Integral < (- p->AntiWindUp) )
 		p->Integral = (-p->AntiWindUp);

	p->setActuator(( ((s32)p->GainP * p->Error + 
					 (s32)p->GainD * ( p->Error - p->PrevError  ) +
				     (((s32)p->GainI * p->Integral)>>8) ) 
				   ));	//write actuator using user provided function
	
	//the >>8 is to compensate for the control loop frequency
	//division is not used to optimize execution speed
}