#include <avr\io.h>
#include <avr\sfr_defs.h>
#include <avr\pgmspace.h>
#include <avr\interrupt.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "driveMotor.h"

static void InitMotors (void);
static void DriveMotors (int16_t pwm, MotorPID  *pid );
static void InitPortsRegisters (void);
static void DisableMotors(void);
static void EnableMotors(void);
static char InError(void);
static void SetMotorCoast   ( MotorPID *p);
static void SetMotorForward ( MotorPID *p);
static void SetMotorBackward( MotorPID *p);

static MotorPID leftPID;
static MotorPID rightPID;

static int16_t	maxPWM =255;  
static int16_t  minPWM =20;    // min pwm where motors still drive. creep speed.

//-----------------------------------------------------------------------------------
void GetPWMLimits(uint16_t *max, uint16_t *min)
{
	*max = maxPWM;
	*min = minPWM;
}
//-----------------------------------------------------------------------------------


SetPWMLimits(int16_t min, int16_t max)
{
	if (min>max) min=max-1;
	if (max<0)   max=0;
	if (max>255) max=255;
	if (min<0)   min=0;
	if (min>255) min=255;
	maxPWM = max;
	minPWM = min;
}

//------------------------------------------------------------------------------

void InitMotors(void)
{

	// setup PWM 
	// - inverting / 8Bit PWM
	//Two pins per motor 
	//in1 in2
	// H   L    - fwd  (H is pwm'd)
	// L   H    - bck  (H is pwm'd)
	// H   H    - hi freewheel
	// L   L    - lo freewheel
    // Two PWM outputs per motor are connected to MC33186 in1 & in2,
	// these are each connected to AVR PWM outputs.
	// - one pwm is active while the other set low. which one defines direction
	
	//PWMtimer will hold the SFR (ram address) of the OCR1AL & OCR1BL registers.
	//Take address, otherwise will load contents of OCR1AL, we use the address later
	//when OCR1AL is written to. This is because OCR1AL is a macro that will set/get
	//the register contents, not the SFR, eg OCR1AL=10; is valid, see sfr_defs.h
	
	leftPID.PWMFwdTimer  = (uint16_t *)&OCR1A;
	leftPID.PWMBckTimer  = (uint16_t *)&OCR1B;
	leftPID.PWMControl  =  (uint8_t *)&TCCR1A;

	leftPID.motorId   = eLeftMotor;   	 // motor id (used for the direction macros)  	
	leftPID.motorFault	= 	L_FAULT;
	leftPID.motorInBits	= 	L_INBITS;
	leftPID.pwm =0;
	
	rightPID.PWMFwdTimer  = (uint16_t *)&OCR3A;
	rightPID.PWMBckTimer  = (uint16_t *)&OCR3B;
	rightPID.PWMControl  =  (uint8_t *)&TCCR3A;

	rightPID.motorId   = eRightMotor;   //  motor id (used for the direction macros)   
	rightPID.motorFault	= 	 R_FAULT;
	rightPID.motorInBits= 	 R_INBITS;
	rightPID.pwm =0;

	
	InitPortsRegisters();    
	
	SetMotorCoast(&leftPID);
	SetMotorCoast(&rightPID); 
	               	
	TCCR1A = BV(WGM10);	    // BV(COM1A1)  | BV(COM1B1)  will be set later as per req. direction           	
	TCCR1B |= BV(CS12);		// timer 1 source CLK/256

	TCCR3A = BV(WGM30);	    // BV(COM1A1)  | BV(COM1B1)  will be set later as per req. direction           	
	TCCR3B |= BV(CS12);		// timer 1 source CLK/256

	EnableMotors();
	
}

//-----------------------------------------------------------------------------------
char InError(void)
{
	char error=0;

	// if enabled & fault flag then return error

	if ( ((L_DRIVE_PORT & L_ENABLE) ==0) &&			//low enabled
		 ((L_STATE_PORT & L_FAULT)  ==0)) error |=1;
	if ( ((R_DRIVE_PORT & R_ENABLE) ==0) &&
		 ((R_STATE_PORT & R_FAULT)  ==0)) error |=2;
	
	return error;

}
//-----------------------------------------------------------------------------------

void EnableMotors(void)
{
	L_DRIVE_PORT &=~L_ENABLE;				//enable motors (low enabled)
	R_DRIVE_PORT &=~R_ENABLE;				//enable motors
}
//-----------------------------------------------------------------------------------

void DisableMotors(void)
{
	L_DRIVE_PORT |=L_ENABLE;				//disable motors
	R_DRIVE_PORT |=R_ENABLE;				//disable motors
}
//-----------------------------------------------------------------------------------

void InitPortsRegisters(void)
{
   //  set direction control of motor port -
	
	L_DRIVE_PORT_DD |= L_OUTBITS;		//port bits to output
	R_DRIVE_PORT_DD |= R_OUTBITS;		//port bits to output  


	//for status input from M33186

	L_DRIVE_PORT_DD &=~L_INBITS;		//port bits to input
	R_DRIVE_PORT_DD &=~R_INBITS;		//port bits to input
	L_STATE_PORT |=L_FAULT;				//pullups on
 	R_STATE_PORT |=R_FAULT;

 	// disable drives and turns off motors
	// when a PWM channel output is off, it reverts to normal output pin,
	// therefore, want this set low so it's not driving.
	//(only one side of the drive is running pwm, other wont be )
	
	L_DRIVE_PORT &=~L_DRIVEBITS;		//clear - freewheel
	R_DRIVE_PORT &=~R_DRIVEBITS;		//clear - freewheel

	L_DRIVE_PORT |=L_ENABLE;			//disable motors (low enabled)
	R_DRIVE_PORT |=R_ENABLE;			//disable motors
	
	// clear timers/pwm (so uses above state till pwm starts up)
	
	TCCR1A =0;
	TCCR1B =0;
	OCR1A  =0;			
	OCR1B  =0;
			   
}

//-----------------------------------------------------------------------------

void DriveMotors( int16_t pwm, MotorPID  *p )
{

	p->pwm = pwm;  
	
	// pwm +/- MAX_PWM, where sign is direction.	
	// set direction
		
	if (pwm > 0 )
	{
		if (p->dir != eForward)
		{ 
			SetMotorForward(p);
		}
	}
	else
	{
	    if (p->dir != eBackward)
	    {
			SetMotorBackward(p);
		}
		pwm = -pwm;
	}
  
  	if ((pwm !=0) && (pwm < minPWM))	// dont stall motors
  	  pwm = minPWM;
  	
  	// write out the PWM value, 8bit PWM so only lower byte.
  	// 0 value will stop pwm output.
	
	*p->PWMFwdTimer = pwm;	// PWMTImer holds the SRAM address (SFR) of OCR1AL/OCR1BL 
	*p->PWMBckTimer = pwm;	// PWMTImer holds the SRAM address (SFR) of OCR1AL/OCR1BL 

}

//-----------------------------------------------------------------------------
//coast motors
//set both pwm outputs disabled

void SetMotorCoast( MotorPID *p)
{ 
	*p->PWMControl &= ~(BV(COM1A1)  | BV(COM1B1));	//set TCCRnA. COM1 mask ok for timer1 & 3 as bits are same
	p->dir=eCoast;
}
//-----------------------------------------------------------------------------
//turn motors forward
//set one output on (ie pwm output enabled), other off

void SetMotorForward( MotorPID *p)
{
	*p->PWMControl &= ~BV(COM1A1);	//set TCCRnA. COM1A1 mask ok for timer1 & 3 as bits are same
	*p->PWMControl |=  BV(COM1B1);	//set TCCRnA. COM1B1 mask ok for timer1 & 3 as bits are same
	p->dir=eForward;
}
//-----------------------------------------------------------------------------
//turn motors backward
//set one output on (ie pwm output enabled), other off

void SetMotorBackward( MotorPID *p)
{
	*p->PWMControl &= ~BV(COM1B1);	//set TCCRnA. COM1 mask ok for timer1 & 3 as bits are same
	*p->PWMControl |=  BV(COM1A1);	//set TCCRnA. COM1 mask ok for timer1 & 3 as bits are same
	p->dir=eBackward;
}

//-----------------------------------------------------------------------------
//non PWM Test

void NonPWMMotorTest(void)
{

	
	L_DRIVE_PORT    &=~L_DRIVEBITS;		//clear (low out)
	L_DRIVE_PORT_DD |= L_OUTBITS;		//port dd to output
	
	R_DRIVE_PORT    &=~R_DRIVEBITS;		//clear (low out)
	R_DRIVE_PORT_DD |= R_OUTBITS;		//port dd to output
		
	L_DRIVE_PORT &=~L_ENABLE;			//enable motors (low enabled)
	R_DRIVE_PORT &=~R_ENABLE;			//enable motors
	
	while (1)
	{	
		R_DRIVE_PORT |= R_FWD;
		L_DRIVE_PORT |= L_FWD;
		Sleep(10000);
		
		R_DRIVE_PORT &=~R_DRIVEBITS;	//all off
		L_DRIVE_PORT &=~L_DRIVEBITS;
		Sleep(10000);
		
		R_DRIVE_PORT |= R_BCK;
		L_DRIVE_PORT |= L_BCK;
		Sleep(10000);	
		
		R_DRIVE_PORT &=~R_DRIVEBITS;	//all off
		L_DRIVE_PORT &=~L_DRIVEBITS;
		Sleep(10000);
	}
}	
//-----------------------------------------------------------------------------
//PWM Test
//Test two wheels, as on a mobile robot.


void PWMMotorTest1(void)
{	
    MotorPID  *pLeft =&leftPID;
    MotorPID  *pRight=&rightPID;  
	InitMotors();

	//set speeds
	*(pLeft->PWMFwdTimer) =  200;
	*(pLeft->PWMBckTimer) =  60;
	*(pRight->PWMFwdTimer) = 60;
	*(pRight->PWMBckTimer) = 200;
	
	//just change directions (forwards & backwards)
	
	while (1)
	{
		SetMotorForward(pLeft);
		SetMotorForward(pRight);		
		Sleep(10000);
	
		SetMotorBackward(pLeft);
		SetMotorBackward(pRight);
		Sleep(10000);		
	}
	
}


void PWMMotorTest2(void)
{	
	InitMotors();

	//change direction and wheels at different speeds (turns)
	
	while (1)
	{
		DriveMotors(100,&leftPID);		//left forward
		DriveMotors(150,&rightPID);		//right forward
		Sleep(10000);
		
		DriveMotors(-150,&leftPID);		//left backwards
		DriveMotors(-100,&rightPID);	//right backwards
		Sleep(10000);
	
		
	}
	
}