//----------------------------------------------------------------------------
//
//	AVRSTEPPER.c: 3 AXIS Bipolar Stepper Motor Control
//	
//		Work with :
//			LB1845 from Sanyo Electronics
//			A2917SEB, A2919 from Allegro microsystems
//
//
//	Start:				29/06/2008
//	Last Modification:	29/08/2008
//	Comercial Version:	none
//
//	By: Fabian Traczuk
//	Chivilcoy - Buenos Aires - Argentina
//
//	Hardware Model		(PROTOTIPO)
//	Hardware Version	1.0
//	Hardware Revision	1
//	Firmware Version	1.0
//	Firmware Revision	1 
//
//	Code for Atmel AVR microcontrollers (ATMEGA8515)
//
//----------------------------------------------------------------------------
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <avr\io.h>
#include <avr\pgmspace.h>

//Version
#define MAYOR_FIRMWARE_VERSION 1
#define MINOR_FIRMWARE_VERSION 0
#define REV_FIRMWARE_VERSION 0

//Crystal Frequency and BaudRate
#ifndef FOSC
	#define FOSC 8000000UL
#endif

//---------------------------------------------------------------------
//Macros
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT))


//MOTOR X Control-----------------------------------------
#define PORT_X_MOTOR	PORTD
#define CONFIG_X_MOTOR	DDRD
#define SIGNALS_X_INPUT	PIND
#define STEP_X_DIR		PD6
#define STEP_X_PULSE	PD7

//MOTOR Y Control-----------------------------------------
#define PORT_Y_MOTOR	PORTA
#define CONFIG_Y_MOTOR	DDRA
#define SIGNALS_Y_INPUT	PINA
#define STEP_Y_DIR		PA6
#define STEP_Y_PULSE	PA7

//MOTOR Z Control-----------------------------------------
#define PORT_Z_MOTOR	PORTC
#define CONFIG_Z_MOTOR	DDRC
#define SIGNALS_Z_INPUT	PINC
#define STEP_Z_DIR		PC6
#define STEP_Z_PULSE	PC7

//#define MAX_STEPS_TO_TOP 0x258


//--------------------------------------------------------
//	TABLAS DE MODOS DE TRABAJO DE LOS MOTORES
//--------------------------------------------------------
#define F_STEP_0		0x20
#define F_STEP_1		0x24
#define F_STEP_2		0x04
#define F_STEP_3		0x00

const unsigned char STEPS_FULL_TABLE[4] PROGMEM = {
	F_STEP_0,
	F_STEP_1,
	F_STEP_2,
	F_STEP_3
};

#define F_TOP_STEPS		0x04
#define F_BOTTOM_STEPS	0xFF
#define F_FIRST_STEP	0x00
#define F_LAST_STEP		0x03

#define H_STEP_0		0x03
#define H_STEP_1		0x09
#define H_STEP_2		0x38
#define H_STEP_3		0x29
#define H_STEP_4		0x27
#define H_STEP_5		0x2D
#define H_STEP_6		0x1C
#define H_STEP_7		0x0C

const unsigned char STEPS_HALF_TABLE[8] PROGMEM = {
	H_STEP_0,
	H_STEP_1,
	H_STEP_2,
	H_STEP_3,
	H_STEP_4,
	H_STEP_5,
	H_STEP_6,
	H_STEP_7
};

#define H_TOP_STEPS		0x08
#define H_BOTTOM_STEPS	0xFF
#define H_FIRST_STEP	0x00
#define H_LAST_STEP		0x07

#define STEP_0		0x03	//xx00 0011
#define STEP_1		0x02	//xx00 0010
#define STEP_2		0x09	//xx00 1001
#define STEP_3		0x10	//xx01 0000
#define STEP_4		0x38	//xx11 1000
#define STEP_5		0x30	//xx11 0000
#define STEP_6		0x29	//xx10 1001
#define STEP_7		0x22	//xx10 0010
#define STEP_8		0x27	//xx10 0111
#define STEP_9		0x26	//xx10 0110
#define STEP_10		0x2D	//xx10 1101
#define STEP_11		0x34	//xx11 0100
#define STEP_12		0x1C	//xx01 1100
#define STEP_13		0x14	//xx01 0100
#define STEP_14		0x0C	//xx00 1100
#define STEP_15		0x06	//xx00 0110

const unsigned char STEPS_TABLE[16] PROGMEM = {
	STEP_0,
	STEP_1,
	STEP_2,
	STEP_3,
	STEP_4,
	STEP_5,
	STEP_6,
	STEP_7,
	STEP_8,
	STEP_9,
	STEP_10,
	STEP_11,
	STEP_12,
	STEP_13,
	STEP_14,
	STEP_15
};
#define TOP_STEPS		0x10
#define BOTTOM_STEPS	0xFF
#define FIRST_STEP		0x00
#define LAST_STEP		0x0F

const unsigned char *StepTablePtr;
unsigned char TopSteps;
unsigned char BottomSteps;
unsigned char FirstSteps;
unsigned char LastSteps;
//--------------------------------------------------------

#define STOP_MOTOR	0x1B
#define LOCK_MOTOR	0x00
//----------------------------------------------------------

volatile unsigned char LastXStep = 0xFF;
volatile unsigned char LastYStep = 0xFF;
volatile unsigned char LastZStep = 0xFF;
volatile unsigned char StepValue = 0x00;

//----------------------------------------------------------------------------
//	Control Functions
void StartUp(void);
void SPK_xMoveStepForward(void);
void SPK_xMoveStepReverse(void);
void SPK_yMoveStepForward(void);
void SPK_yMoveStepReverse(void);
void SPK_zMoveStepForward(void);
void SPK_zMoveStepReverse(void);
