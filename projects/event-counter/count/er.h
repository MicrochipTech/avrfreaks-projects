//header file for main file "counter.c"
#include <inttypes.h>

#define FILTER_MASK 0xFFU //0xff means that 8 samples must by the same

int16_t main_counter; //main counter of rotations

#define LED_A_OD	PD0
#define LED_B_OD	PD1
#define LED_C_OD	PD2 
#define LED_D_OD	PD3
#define LED_E_OD	PD4
#define LED_F_OB	PB6
#define LED_G_OB	PB7

#define SEG1000_OB	PB0
#define SEG100_OD	PD7
#define SEG10_OD	PD6
#define SEG1_OD		PD5

#define BUTT1000_IB	PB1
#define BUTT100_IB  PB2
#define BUTT10_IB	PB3
#define BUTT1_IB	PB4
#define BUTT_RUN_IB	PB5

#define CLOCK_OC 	PC2
#define DIR_OC		PC3
#define CLOCK_IC	PC0
#define DIR_IC 		PC1

#define RELAY_OC    PC4



