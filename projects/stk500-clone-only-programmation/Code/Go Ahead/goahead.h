/*****************************************************************************
* Author: 	Jidan Al-Eryani,	jidan@gmx.net 
* Date: 	18.4.2007
*
*
* Purpose: 
* Simple program to test the interaction between the microcontroller and FPGA.
* This Program makes the robot platform move forward (hence the name of program = Go Ahead)
* and turn randomly right or left around an obstacle if one is detected.
* The sensors are two infrared sensors and the motors encoders.
*
*
****************************************************************************/

#define SW2_PIN PF0
#define SW3_PIN PF1

#define LED_PIN PA0
#define SENSR_PIN PF5
#define SENSL_PIN PF7

/*FPGA pins*/
//Outputs
#define FPGA_IRSENSE PC7 //P74
#define FPGA_OBSTACLE PC6 //P76
#define FPGA_MCU_CLK PC4 //P73

//Inputs
#define FPGA_COMMAND1 PC5 //(INIT: P58 )
#define FPGA_COMMAND2 PC1 //(DIN: P65)

/* Function prototypes */
void init_goahead(void);
void go_around(unsigned char turn);
void goahead(void);
