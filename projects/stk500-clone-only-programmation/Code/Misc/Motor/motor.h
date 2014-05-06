/*****************************************************************************
*Author: 	Jidan Al-Eryani,	jidan@gmx.net 
*Date: 	16.4.2007
*
* Basic control functions for 2 x DC-Motors
*
*
****************************************************************************/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/******************Define motor pins***********/
#define M1_EN PB7
#define M1_IN1 PE2
#define M1_IN2 PE3
#define M1_A PE4
#define M1_B PE5

#define M2_EN PB6
#define M2_IN1 PB5
#define M2_IN2 PB4
#define M2_A PE6
#define M2_B PE7



/****** Define basic functions************/

//Directions
#define forward_M1 PORTE |= (1<<M1_IN1); PORTE &= ~(1<<M1_IN2);
#define backward_M1 PORTE &= ~(1<<M1_IN1); PORTE |= (1<<M1_IN2);
#define faststop_M1 PORTE &= ~(1<<M1_IN1); PORTE &= ~(1<<M1_IN2);

#define backward_M2 PORTB |= (1<<M2_IN1); PORTB &= ~(1<<M2_IN2);
#define forward_M2 PORTB &= ~(1<<M2_IN1); PORTB |= (1<<M2_IN2);
#define faststop_M2 PORTB &= ~(1<<M2_IN1); PORTB &= ~(1<<M2_IN2);


//function protoytpes
void init_mototrs(void);
void init_encoder(void);

void move_m(int speed_m1, int speed_m2);
void turn_m(signed int degree);

void disable_m(void);
void enable_m(void);

void enable_M1(void);
void enable_M2(void);
void disable_M1(void);
void disable_M2(void);

void do_ms(unsigned int ms);
