
#include <inttypes.h>

//------ motors -----
//one  MC33186 hbridge drive per motor

//LEFT MOTOR (uses timer 1)
//in1 = PB5
//in2 = PB6
//di1 = PB4
#define L_DRIVE_PORT    PORTB
#define L_DRIVE_PORT_DD DDRB
#define L_OUTBITS		(BV(PB5) | BV(PB6) | BV(PB4))
#define L_DRIVEBITS		(BV(PB5) | BV(PB6))

#define L_FWD			BV(PB5)		//in1=pwm OC1A, in2=0
#define L_BCK			BV(PB6)		//in1=0, in2=pwm OC1B
#define L_ENABLE		BV(PB4)		//enabled when di1=0
#define L_FREEWHEEL		0

//fault detection

#define L_STATE_PORT    PINB
#define L_INBITS		BV(PINB7)
#define L_FAULT			BV(PINB7)  //in


//RIGHT MOTOR (uses timer 3)
//in1 = PE3
//in2 = PE4
//di1 = PE2
#define R_DRIVE_PORT    PORTE
#define R_DRIVE_PORT_DD DDRE
#define R_OUTBITS		(BV(PE3) | BV(PE4) | BV(PE2))
#define R_DRIVEBITS		(BV(PE3) | BV(PE4) )

#define R_FWD			BV(PE3) 		//in1=pwm OC3A, in2=0
#define R_BCK			BV(PE4)			//in1=0, in2=pwm
#define R_ENABLE		BV(PE2)			//enabled when di1=0
#define R_FREEWHEEL		0

//fault detection

#define R_STATE_PORT    PINE
#define R_INBITS		BV(PINE5)
#define R_FAULT			BV(PINE5) //in


//-----------------------------------------------------------------------
enum e_MotorDir {eForward,eBackward,eCoast};


// define two identifers for the motor direction control macros (below)

enum e_MotorId {eLeftMotor, eRightMotor };



typedef struct
{
		
	enum e_MotorId motorId;  // motor mask ident. used in the motor direction macros
	uint8_t motorFault;
	uint8_t motorInBits;
	
	uint16_t *PWMFwdTimer;        // address of OCR1A or OCR3A (16bit register)
	uint16_t *PWMBckTimer;        // address of OCR1B or OCR3B (16bit register)
	uint8_t  *PWMControl;         // address of TCCR1A or TCCR3A (8bit register)
	
	enum e_MotorDir dir;		//status
	
	
	int16_t pwm;

} MotorPID;


