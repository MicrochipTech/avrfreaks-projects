
#include <inttypes.h>

#define minPWM 80    // min pwm light is still ok
#define s2PWM  120
#define s3PWM  185
#define maxPWM 255  

#define NUM_INC_STEPS  4		//brightness steps

#define POST_OFF_DELAY 1000
#define POST_DIM_DELAY 500

#define DECISION_DELAY 500		//if down for this long then user want dimmer
#define DIMMER_DELAY   50
#define TOT_DIM_TIME   2000		// ms from full on to off when dimming
#define DIMMER_STEPS   (TOT_DIM_TIME/DIMMER_DELAY)
#define DIMMER_DEC_PER_STEP ((maxPWM-minPWM)/DIMMER_STEPS )

//---------------- IO PORTS FOR THIS APPLICATION  -------------------
//-------------------------------------------------------------------

//------ lamp/switch -----

#define L_SWITCH_PORT    PORTD
#define L_SWITCH_PORT_DD DDRD
#define L_SWITCH_BIT     BV(PD2)		//is an interrupt pin
#define L_SWITCH_PIN	 PIND

#define L_PWM_PORT		 PORTB
#define L_PWM_DD		 DDRB
#define L_PWM			 BV(PB2)



#define R_SWITCH_PORT    PORTD
#define R_SWITCH_PORT_DD DDRD
#define R_SWITCH_BIT     BV(PD3)
#define R_SWITCH_PIN	 PIND

#define R_PWM_PORT		 PORTB
#define R_PWM_DD		 DDRB
#define R_PWM			 BV(PB1)

// define two identifers for the lamp control macros (below)

enum e_ControllerId {eLamp1, eLamp2 };




typedef struct
{
	int16_t PWMSetPoint; 	//PWM for this controller       

	
	// io bits 
	
	enum e_ControllerId controllerId;  // lamp mask ident. 
	
	uint8_t switchBit;
	uint8_t *switchPort;		// 8 bit port register
	uint8_t *switchPin;			// input pin
	
	uint16_t *PWMTimer;        // address of OCR1AL or OCR1BH (16bit register)
	
	int	switchPressed;
	
	

} sController;



