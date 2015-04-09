/*
 * Globals_Defs.c
 *
 *  Created on: 17 mar 2015
 *      Author: M.R.O.
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include <avr\pgmspace.h>
#include "Globals_Defs.h"

/***************************
 * --->>  Variables  <<--- *
 ***************************/

uint8 volatile Conversion = 0;/*interrupt.c*/
uint16 volatile A,B,C,D,E,F;
uint16 volatile Counter_Value;
uint16 volatile WaitTime = 0;

uint8 AdcMeasurmentEnableFlag = 0, Engine6Status = 0; /*main.c*/
uint8 DelayFlag = 0, Flag10ms = 0, ResetDelayFlag = 1, takse = 1;
uint16 TxBuf[16], RxBuf[RING_BUFFER_LENGTH], AdcBuf[8] , test = 1;
uint16 Counter_Flag10ms = 0;

uint8 ButtonWork = 0, ControlType = 0; /*Menu.c*/
uint8 Menulvl[3] = {1,0,0}, AnyButtonChange = 1;

uint8 SequencesQuantity = 0;	/*PcComands.c*/
uint8 ExcitationEnable = 0, SelectedEngine;
uint8 AdcSetAxis;
uint8 ReceiveFlag = 0, l = 0, i = 0;
uint8 GripperBusy = 0;
uint16 SeqArray[50][16];
uint16 StepsAmount;
uint16 Receive_1, Receive_2;
uint16 VodkaTimeOut = 0;

uint8 CalibrationStatus = 0, VodkaStatus = 0, GripperStatus = 0; /*Functions.c*/
uint16 GripperForce = 0, GripperSpeed = 0;
ZmiennePid Pid[6];

volatile uint8 usart_i;	/*usart.c*/
uint8 buf_index =0;
uint16 usart_buffer[RING_BUFFER_SIZE];
