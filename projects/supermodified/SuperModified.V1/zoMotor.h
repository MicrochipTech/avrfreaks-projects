#ifndef ZO_MOTOR
#define ZO_MOTOR

//****************************************************************************************
//	File Name	: zoMotor.h
//	Description	: Timers and io configuration for use with MCP17511
//	Created		: 25.1.2010
//	Target MCU	: Atmel ATMega48/88/168
//	Author		: Sissakis Giannis
//  email		: info@01mech.com
//
//  Copyright (C) 2010 Zero One Mechatronics LP
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************************************

#include "zoTypes.h"

void zoMotorInit(void);
void zoMotorSet(s32 pwmDuty); 
void zoMotorSetState(bool onOff);
bool zoMotorGetState(void);
void zoMotorSetTimedEventHandler(void (*timedEventFunction)(void));

#endif	//ZO_MOTOR