//----------------------------------------------------------------------------
//
//	AVRSTEPPER.c: 3 AXIS Bipolar Stepper Motor Control
//	
//		Work with :
//			LB1845 from Sanyo Electronics
//			A2917SEB, A2919 from Allegro microsystems
//
//	Start:				29/06/2008
//	Last Modification:	29/08/2008
//	Comercial Version:	none
//
//	By: Fabian Traczuk
//	Chivilcoy - Buenos Aires - Argentina
//
//	Hardware Model (PROTOTIPO)
//	Hardware Version	1.0
//	Hardware Revision	1
//	Firmware Version	1.0
//	Firmware Revision	1 
//
//	Code for Atmel AVR microcontrollers (ATMEGA8515)
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#include "AVRSTEPPER.h"

//---------------------------------------------------------------------------------
int main(void) 
{	

	//Init ALL
	StartUp();

	while(1){
		//X AXIS
		if(CHECKBIT(SIGNALS_X_INPUT, STEP_X_PULSE)){
			//Hasta que no pase a LOW no proceso
			while (CHECKBIT(SIGNALS_X_INPUT, STEP_X_PULSE));
			if(CHECKBIT(SIGNALS_X_INPUT, STEP_X_DIR))
				SPK_xMoveStepForward();
			else
				SPK_xMoveStepReverse();
		}
		//Y AXIS
		if(CHECKBIT(SIGNALS_Y_INPUT, STEP_Y_PULSE)){
			//Hasta que no pase a LOW no proceso
			while (CHECKBIT(SIGNALS_Y_INPUT, STEP_Y_PULSE));
			if(CHECKBIT(SIGNALS_Y_INPUT, STEP_Y_DIR))
				SPK_yMoveStepForward();
			else
				SPK_yMoveStepReverse();
		}
		//Z AXIS
		if(CHECKBIT(SIGNALS_Z_INPUT, STEP_Z_PULSE)){
			//Hasta que no pase a LOW no proceso
			while (CHECKBIT(SIGNALS_Z_INPUT, STEP_Z_PULSE));
			if(CHECKBIT(SIGNALS_Z_INPUT, STEP_Z_DIR))
				SPK_zMoveStepForward();
			else
				SPK_zMoveStepReverse();
		}
	}	
	return 0;
}

//---------------------------------------------------------------------------------
void StartUp(void){

	CONFIG_X_MOTOR = 0x3F;	//PORT D MOTOR X Control(output) & STEP-DIR(input)
	PORT_X_MOTOR = 0x00;

	CONFIG_Y_MOTOR = 0x3F;	//PORT A MOTOR Y Control(output) & STEP-DIR(input)
	PORT_Y_MOTOR = 0x00;

	CONFIG_Z_MOTOR = 0x3F;	//PORT C MOTOR Z Control(output) & STEP-DIR(input)
	PORT_Z_MOTOR = 0x00;

	PORT_X_MOTOR = STOP_MOTOR;
	PORT_Y_MOTOR = STOP_MOTOR;
	PORT_Z_MOTOR = STOP_MOTOR;

	TIMSK = 0x00;	//Disable Interrups Timers

	//Definir el modo de trabajo (Descomentar solo uno de los modos)
	//MICRO (800pasos/Vuelta)
	StepTablePtr = STEPS_TABLE;
	TopSteps = TOP_STEPS;
	BottomSteps = BOTTOM_STEPS;
	FirstSteps = FIRST_STEP;
	LastSteps = LAST_STEP;

	//HALF (400pasos/Vuelta)
	//StepTablePtr = STEPS_HALF_TABLE;
	//TopSteps = H_TOP_STEPS;
	//BottomSteps = H_BOTTOM_STEPS;
	//FirstSteps = H_FIRST_STEP;
	//LastSteps = H_LAST_STEP;

	//FULL (200pasos/Vuelta)
	//StepTablePtr = STEPS_FULL_TABLE;
	//TopSteps = F_TOP_STEPS;
	//BottomSteps = F_BOTTOM_STEPS;
	//FirstSteps = F_FIRST_STEP;
	//LastSteps = F_LAST_STEP;
}

//----------------------------------------------------------------------------
//	SPK_xMoveStepForward
//----------------------------------------------------------------------------
void SPK_xMoveStepForward(void)
{
	LastXStep++;

	if (LastXStep == TopSteps)
		LastXStep = FirstSteps;

	StepValue = pgm_read_word(&StepTablePtr[LastXStep]);
	
	PORT_X_MOTOR = StepValue;

}

//----------------------------------------------------------------------------
//	SPK_xMoveStepReverse
//----------------------------------------------------------------------------
void SPK_xMoveStepReverse(void)
{
	LastXStep--;

	if (LastXStep == BottomSteps)
		LastXStep = LastSteps;

	StepValue = pgm_read_word(&StepTablePtr[LastXStep]);
	
	PORT_X_MOTOR = StepValue;
}

//----------------------------------------------------------------------------
//	SPK_yMoveStepForward
//----------------------------------------------------------------------------
void SPK_yMoveStepForward(void)
{
	LastYStep++;

	if (LastYStep == TopSteps)
		LastYStep = FirstSteps;

	StepValue = pgm_read_word(&StepTablePtr[LastYStep]);
	
	PORT_Y_MOTOR = StepValue;

}

//----------------------------------------------------------------------------
//	SPK_yMoveStepReverse
//----------------------------------------------------------------------------
void SPK_yMoveStepReverse(void)
{
	LastYStep--;

	if (LastYStep == BottomSteps)
		LastYStep = LastSteps;

	StepValue = pgm_read_word(&StepTablePtr[LastYStep]);
	
	PORT_Y_MOTOR = StepValue;
}

//----------------------------------------------------------------------------
//	SPK_zMoveStepForward
//----------------------------------------------------------------------------
void SPK_zMoveStepForward(void)
{
	LastZStep++;

	if (LastZStep == TopSteps)
		LastZStep = FirstSteps;

	StepValue = pgm_read_word(&StepTablePtr[LastZStep]);
	
	PORT_Z_MOTOR = StepValue;

}

//----------------------------------------------------------------------------
//	SPK_xMoveStepReverse
//----------------------------------------------------------------------------
void SPK_zMoveStepReverse(void)
{
	LastZStep--;

	if (LastZStep == BottomSteps)
		LastZStep = LastSteps;

	StepValue = pgm_read_word(&StepTablePtr[LastZStep]);
	
	PORT_Z_MOTOR = StepValue;
}

