#include <io.h>
#include <inttypes.h>
#include <stdlib.h>
#include <math.h>

#include <..\libraries\avrx\avrx\avrx-io.h>
#include <..\libraries\avrx\avrx\avrx-signal.h>
#include <..\libraries\avrx\avrx\AvrX.h>

#include <avr\wdt.h>

#include <stdio.h>
#include <progmem.h>
#include <interrupt.h>

#include <..\libraries\avrx\cserialio\serialio.h>

#include "ControlLamp.h"

static void InitControllers(void);

static void SetPWM( int16_t pwm, sController *p );
static void InitPortsRegisters (void);

static void ManageSwitch(sController  *p);
static void StepBrighter(sController  *p);
static void Dimmer(sController *p, int dec);


static TimerControlBlock   timerDelay; // general timer

// controller records. Holds state of each switch & pwm.

static sController controller1;
static sController controller2;

// define the steps in brighness when stepping up 

static int16_t BrightPWMSteps[NUM_INC_STEPS]	={minPWM,s2PWM,s3PWM,maxPWM};


// #define TWO_LIGHTS					//enable for two lights

//-----------------------------------------------------------------------------------


static Mutex intFired;

AVRX_SIGINT(SIG_INTERRUPT0)				// 1st switch pressed
{
	IntProlog();
	controller1.switchPressed = 1;
	AvrXIntSetSemaphore(&intFired);
	Epilog();
}

#ifdef TWO_LIGHTS	

AVRX_SIGINT(SIG_INTERRUPT1)				// 2nd switch pressed
{
	IntProlog();
	controller2.switchPressed = 1;
	AvrXIntSetSemaphore(&intFired);
	Epilog();

}

#endif


//-----------------------------------------------------------------------------------
//--------------------------- THE THREAD --------------------------------------------
//-----------------------------------------------------------------------------------
// Main thread.
// will support two switches. Just need to enable the logic in the while loop for 
// controller2.

#define TSK_LAMP_PRI    1 //hi priority

AVRX_GCC_TASKDEF(taskLamp, 350, TSK_LAMP_PRI) 
{ 
	
	wdt_reset();

	//printf_P(PSTR("running\n"));
	AvrXResetSemaphore(&intFired);			//initialise

	InitControllers();
   
	SetPWM(maxPWM,&controller1);
	SetPWM(maxPWM,&controller2);

	wdt_disable();							//ok if here.. let it run without.

	while (1)
	{	

		AvrXWaitSemaphore(&intFired);
		
		if (controller1.switchPressed) 		// 1st switch
		{
			ManageSwitch(&controller1);
			controller1.switchPressed = 0;
			AvrXResetSemaphore(&intFired);	//no pending ones needed
			//printf("1");
		
		}
	
		if (controller2.switchPressed)   	// for 2nd dimmer if wanted.
		{
			ManageSwitch(&controller2);
			controller2.switchPressed = 0;
			AvrXResetSemaphore(&intFired);	//no pending ones needed
			//printf("2");
		}

	}
  
} 

//-----------------------------------------------------------------------------------
// press and hold button and lamp will (continuously) dim.
// press and hold button when lamp is dimmest and it'll turn off.
// press and release button and lamp will brighten to next set point.

static void ManageSwitch(sController  *p)
{
	
	//see what the user wants, if switch is released before a specified time then
    //the user wants to brighten.
	
	const uint8_t chunks=10;					//divide sample time into 10 chunks
	
	for (int i=0;i<chunks;i++)					//process chunks and check in between
	{

		AvrXDelay(&timerDelay,DECISION_DELAY/chunks);
		if ((*p->switchPin & p->switchBit) != 0)				//switch released? (reverse logic on switch )
		{
			StepBrighter(p);									//released within sample time, so brighten
			return;
		}
	}
	
	// wants to turn off?  Turn off lamp only if already at dimest setting
	
	if (p->PWMSetPoint == minPWM)
	{
		SetPWM(0,p);
		AvrXDelay(&timerDelay,POST_OFF_DELAY);
		return;
	}
	
	// wants to dim. Keep dimming while switch is pressed.
	
	do
	{
		Dimmer(p,DIMMER_DEC_PER_STEP);
		AvrXDelay(&timerDelay,DIMMER_DELAY);
	} 
	while ((*p->switchPin & p->switchBit) ==0);		//switch still down? (reverse logic on switch )

	AvrXDelay(&timerDelay,POST_DIM_DELAY);
	
}

//-----------------------------------------------------------------------------------
// get the current PWM value, then look for next brightest pwn value in the brightness table.
// (current value can be anything as the lamp may have been dimmed.)
// eg if brightness table is 100, 150, 200 & 255. and the current value is 160, the next value in
// the table is 200.So the light will be brightened to 255. 
// Dont go to the next as it might only be one more, so go one beyond.

static void StepBrighter(sController  *p)
{
	uint8_t i;
	
	
	for (i=0;i<NUM_INC_STEPS-1;i++)
	{
		if (p->PWMSetPoint <= BrightPWMSteps[i])
		{
			i++;
			break;
		}
	}
	SetPWM(BrightPWMSteps[i],p);

}
//-----------------------------------------------------------------------------------
// knock down the brightness by specified amount

static void Dimmer(sController  *p, int dec)
{
	SetPWM(p->PWMSetPoint - dec, p);
}
//-----------------------------------------------------------------------------------

static void InitControllers(void)
{

	
	//PWMtimer will hold the SFR (ram address) of the OCR1AL & OCR1BL registers.
	//Take address, otherwise will load contents of OCR1AL, we use the address later
	//when OCR1AL is written to. This is because OCR1AL is a macro that will set/get
	//the register contents, not the SFR, eg OCR1AL=10; is valid, see sfr_defs.h
	
	controller1.PWMTimer		= (uint16_t *)&OCR1A;
	controller1.controllerId	= eLamp1;   	  	
	controller1.switchPort 		= (uint8_t *)&L_SWITCH_PORT;
	controller1.switchBit  		= L_SWITCH_BIT; 
	controller1.switchPin		= (uint8_t *)&L_SWITCH_PIN;
	controller1.switchPressed	= 0;

	controller2.PWMTimer  		= (uint16_t *)&OCR1B;
	controller2.controllerId	= eLamp2;    
	controller2.switchPort 		= (uint8_t *)&R_SWITCH_PORT;
	controller2.switchBit  		= R_SWITCH_BIT; 
	controller2.switchPin		= (uint8_t *)&R_SWITCH_PIN;
	controller2.switchPressed	= 0;

	InitPortsRegisters();    
	
	// set PWM 
	// 8Bit PWM
	
	TCCR1A = BV(COM1A1) | BV(COM1B1)  | BV(WGM10);	 //for mega32 and mega8              	
  //TCCR1A = BV(COM1A1) | BV(COM1B1)  | BV(PWM10);   //for other chips
	
	TCCR1B |= BV(CS12);								 // timer 1 source CLK/256
}

//-----------------------------------------------------------------------------------

static void InitPortsRegisters(void)
{
	// switches to input (clear bits)
	
	L_SWITCH_PORT_DD &= ~L_SWITCH_BIT;
	R_SWITCH_PORT_DD &= ~R_SWITCH_BIT;

	// turn off pull up resistors (interrupt triggered, so no use)
	
	L_SWITCH_PIN &= ~L_SWITCH_BIT;
	R_SWITCH_PIN &= ~R_SWITCH_BIT;
	
	//turn off port, PWM stuff will take over later
	
	L_PWM_PORT &=~L_PWM;	
	R_PWM_PORT &=~R_PWM;


	// enable interrupt pins
	// switches are interrupt driven
	
	MCUCR |=BV(ISC01);//| BV(ISC00);   // int0, falling edge
	MCUCR |=BV(ISC11);//| BV(ISC10);   // int1, falling  edge
	GIMSK |=BV(INT0);      			   // enable int0 
	
#ifdef TWO_LIGHTS		

	GIMSK |=BV(INT1);       		   // enable int1
	
#endif	

	// set pwm ports to output
	// turn off mosfets
	
    L_PWM_DD |= L_PWM;		// PB1 & PB2 are PWM outputs (ie OC1A/B) 
	R_PWM_DD |= R_PWM;		 
	
	// clear timers/pwm 
	
	TCCR1A =0;
	TCCR1B =0;
	OCR1A  =0;			
	OCR1B  =0;
		
	   
}

//-----------------------------------------------------------------------------

static void SetPWM( int16_t pwm, sController  *p )
{
    		    		
  	if ((pwm !=0) && (pwm < minPWM))	// always a min (should never happen)
  	  pwm = minPWM;
	  
 	p->PWMSetPoint = pwm;  
 	
  	// write out the PWM value, 8bit PWM so only lower byte.
  	// 0 value will stop pwm output.
	
	*p->PWMTimer = pwm;	// PWMTImer holds the SRAM address (SFR) of OCR1AL/OCR1BL 

	//printf("%d ", pwm);

}

