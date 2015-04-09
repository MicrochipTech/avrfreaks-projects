/*
 * Functions.h
 *
 *  Created on: 18 mar 2015
 *      Author: M.R.O.
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "Globals_Defs.h"

void PID(void);
int16_t ABS (int16_t value);
void PIDParameters(int8_t index, int8_t kp, int8_t ki, int8_t kd, int16_t maxerror, int16_t maxerrori ,
		int16_t maxerrord, int16_t maxcv, int16_t Deadband);
void PIDDefParameters(void);
void Onlinecontrol (uint8 index,uint8 button);
void FL_Pidparameters(uint8 index, uint8 kp, uint8 ki, uint8 kd);
uint8 EngineStatus6(void);
uint8 EngineStatus5(void);
void Calibration(void);
void Vodka(void);
void GripperWork(void);
void Excitation(int16 Steps, uint8 Axis, uint8 Time);
void SafeRobot (uint8 source);
void DmaChangeLength (volatile DMA_CH_t *channel, uint8 length);
void SendData(uint8 length);

#endif /* FUNCTIONS_H_ */
