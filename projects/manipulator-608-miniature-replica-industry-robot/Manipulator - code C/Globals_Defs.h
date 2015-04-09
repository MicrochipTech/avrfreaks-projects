/*
 * Globals_Defs.h
 *
 *  Created on: 17 mar 2015
 *      Author: M.R.O.
 */

#ifndef GLOBALS_DEFS_H_
#define GLOBALS_DEFS_H_

/********************************************************************
 * 																	*
 * ------------------------>>  Defines  <<------------------------- *
 * 																	*
 ********************************************************************/

#define uint8 uint8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define int8 int8_t
#define int16 int16_t
#define int32 int32_t

#define PLL128MHZ 16

#define RING_BUFFER_SIZE 50

#define BUTTON_UP !(PORTF.IN & PIN7_bm)
#define BUTTON_DOWN !(PORTF.IN & PIN6_bm)
#define BUTTON_OK !(PORTJ.IN & PIN2_bm)
#define BUTTON_ESC !(PORTK.IN & PIN6_bm)

#define LIMIT_SWITCH_1 (PORTJ.IN & PIN7_bm)
#define LIMIT_SWITCH_2 (PORTJ.IN & PIN6_bm)
#define LIMIT_SWITCH_3 (PORTH.IN & PIN7_bm)
#define LIMIT_SWITCH_4 (PORTH.IN & PIN6_bm)
#define LIMIT_SWITCH_5 (PORTA.IN & PIN7_bm)
#define LIMIT_SWITCH_6 (PORTF.IN & PIN5_bm)

#define VODKA_ON (PORTC.OUTCLR = PIN1_bm)
#define VODKA_OFF (PORTC.OUTSET = PIN1_bm)

#define VODKA_SENSE (PORTC.IN & 0x08)

#define M1_LEFT		PORTD.OUTCLR = PIN1_bm; PORTD.OUTSET = PIN0_bm
#define M1_RIGHT	PORTD.OUTCLR = PIN0_bm; PORTD.OUTSET = PIN1_bm
#define M1_STOP		PORTD.OUTSET = PIN1_bm; PORTD.OUTSET = PIN0_bm

#define M2_LEFT		PORTD.OUTCLR = PIN2_bm; PORTD.OUTSET = PIN3_bm
#define M2_RIGHT	PORTD.OUTCLR = PIN3_bm; PORTD.OUTSET = PIN2_bm
#define M2_STOP		PORTD.OUTSET = PIN3_bm; PORTD.OUTSET = PIN2_bm

#define M3_LEFT		PORTD.OUTCLR = PIN4_bm; PORTD.OUTSET =  PIN5_bm
#define M3_RIGHT	PORTD.OUTCLR = PIN5_bm; PORTD.OUTSET =  PIN4_bm
#define M3_STOP		PORTD.OUTSET = PIN4_bm; PORTD.OUTSET =  PIN5_bm

#define M4_LEFT		PORTD_OUT &= ~_BV(PORT6); PORTD_OUT |= _BV(PORT7)
#define M4_RIGHT	PORTD_OUT &= ~_BV(PORT7); PORTD_OUT |= _BV(PORT6)
#define	M4_STOP		PORTD_OUT |= (_BV(PORT6)|_BV(PORT7))

#define M5_LEFT		PORTE_OUT &= ~_BV(PORT4); PORTE_OUT |= _BV(PORT5)
#define M5_RIGHT	PORTE_OUT &= ~_BV(PORT5); PORTE_OUT |= _BV(PORT4)
#define M5_STOP		PORTE_OUT |= (_BV(PORT4)|_BV(PORT5))

#define M6_LEFT		PORTE_OUT &= ~_BV(PORT6); PORTE_OUT |= _BV(PORT7)
#define M6_RIGHT	PORTE_OUT &= ~_BV(PORT7); PORTE_OUT |= _BV(PORT6)
#define	M6_STOP		PORTE_OUT |= (_BV(PORT6)|_BV(PORT7))

#define M7_OPEN		PORTF_OUT &= ~_BV(PORT4); PORTF_OUT |= _BV(PORT3)
#define M7_CLOSE	PORTF_OUT &= ~_BV(PORT3); PORTE_OUT |= _BV(PORT4)
#define M7_STOP		PORTF_OUT |= (_BV(PORT3)|_BV(PORT4))

#define BUZZER_HORN_ENABLE PORTQ.OUTSET = PIN3_bm ;
#define BUZZER_HORN_DISABLE PORTQ.OUTCLR = PIN3_bm ;
#define ERROR_LED_ON PORTJ.OUTSET = PIN1_bm;
#define ERROR_LED_OFF PORTJ.OUTCLR = PIN1_bm;

#define ALARMING_VALUE 4050
#define ALARM_THRESHOLD_VALUE 12150
#define OVERLOAD 1
#define TRANSMISSION 2
#define ERROR_MESSAGE_ADC 65530
#define ERROR_MESSAGE_TRANSMISSION 65531

#define IDLE_CONTROL 0
#define INTERNAL_CONTROL 1
#define EXTERNAL_CONTROL 2

#define TRUE 1
#define FALSE 0

#define CONTINUE_LOOP 1
#define SKIP_LOOP 0

#define V_ENGINES_IN_POSITION	5
#define VI_ENGINES_IN_POSITION	6

#define STOP_RECEIVING 49014
#define NEW_SEQUENCE_NOTIFICATION 49031

#define RESET_DELAY_VAR TCD0.CTRLA = TC_CLKSEL_DIV1_gc;	ResetDelayFlag = FALSE;

#define GRIPPER_SPEED TCF0.CCCBUF
#define GRIPPER_IDLE 1

#define AXIS_1 0
#define AXIS_2 1
#define AXIS_3 2
#define AXIS_4 3
#define AXIS_5 4
#define AXIS_6 5
#define GRIPPER 6

#define LEFT 1
#define RIGHT 2
#define STOP 3

#define MENU 0
#define	VELOCITY_SETUP 1
#define OVERRIDE 2

#define DMA_Enable()    ( DMA.CTRL |= DMA_ENABLE_bm )
#define RING_BUFFER_LENGTH 100

/********************************************************************
 * 																	*
 * ---------------------->>  Variables  <<------------------------- *
 * 																	*
 ********************************************************************/

typedef struct{
	int32_t ActPoz, Error, PozRef, AbsError, Maxerror, DeadBand, Pterm, Kp, Ki, Errori, MaxErrori, Iterm,
			Errord,LastError, MaxErrord, Dterm, Kd, Cv, AbsCv, MaxCv, Saturated, Direction, LastDirection, PositionBuffor;
	int8_t Enable, OnlineSpeed;
	}ZmiennePid;

extern	ZmiennePid Pid[6];

extern	uint8 volatile Conversion;
extern	uint16 volatile A,B,C,D,E,F; /*Position variable*/
extern	uint16  volatile Counter_Value; /* Variable for time counting */

extern	uint16 volatile WaitTime;

extern	uint8 AdcMeasurmentEnableFlag, Engine6Status;
extern	uint8 DelayFlag, Flag10ms, ResetDelayFlag , takse;
extern	uint16 TxBuf[16], RxBuf[RING_BUFFER_LENGTH], AdcBuf[8] , test;
extern	uint16 Counter_Flag10ms;

extern	uint8 ButtonWork, ControlType;
extern	uint8 Menulvl[3], AnyButtonChange;

extern	uint8 SequencesQuantity;
extern	uint8 ExcitationEnable, SelectedEngine;
extern	uint8 AdcSetAxis;
extern	uint8 ReceiveFlag, l, i;
extern	uint8 GripperBusy;

extern	uint16 SeqArray[50][16];
extern	uint16 StepsAmount;
extern	uint16 Receive_1, Receive_2;
extern	uint16 VodkaTimeOut;

extern	uint8 CalibrationStatus, VodkaStatus, GripperStatus;
extern	uint16 GripperForce, GripperSpeed;

extern	volatile uint8 usart_i ;

extern	uint8 buf_index;
extern	uint16 usart_buffer[RING_BUFFER_SIZE];

#endif /* GLOBALS_DEFS_H_ */
