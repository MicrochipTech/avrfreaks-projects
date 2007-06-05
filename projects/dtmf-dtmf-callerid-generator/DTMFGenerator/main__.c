//******************************************************
// behzad khazama 2007/06/04 SABZEVAR-IRAN
// www.khazama.com
//
// to use this project as DTMF caller id generator
// connect tel lines as bellow
//
//  use 2 series capacitor as bellow
//  TEL(+) --||--*--||-- PORTB.1 (OC1A)
//          +  -   -  +
//       (1uf 50v) (1uf 50v)
//
//  TEL(-) -- connect to GND
//
//******************************************************


//******************************************************
// DTMF caller id signals generate before first ring
// it starts with DTMF signal D (A or B may use too)
// and terminate with DTMF signal C
// between D and C 'caller number' will generate
//
// D(number...)C ------- first ring
//******************************************************

#include <mega8.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
#include <string.h>
#include "dtmf.h"

void main(void){
   char num[15];
        dtmf_init();
        strcpyf(num,"D09155714862C");
        Send_DTMF(num);
        while(1);
}
//**************************************************************************
// This Module has ported from Atmel applivation note 314 for atmega8
// I have modifed that app note a bit to remove
// free runnig osilation between two medulated signal.
// behzad khazama 2007/06/04 SABZEVAR-IRAN
// www.khazama.com
//**************************************************************************

#include <string.h>
#include <mega8.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x80
	.EQU __sm_mask=0x70
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0x60
	.EQU __sm_ext_standby=0x70
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif
#include <delay.h>

//************************** SIN TABLE *************************************
// Samples table : one period sampled on 128 samples and
// quantized on 7 bit
//**************************************************************************
flash unsigned char auc_SinParam [128] = {
64,67,
70,73,
76,79,
82,85,
88,91,
94,96,
99,102,
104,106,
109,111,
113,115,
117,118,
120,121,
123,124,
125,126,
126,127,
127,127,
127,127,
127,127,
126,126,
125,124,
123,121,
120,118,
117,115,
113,111,
109,106,
104,102,
99,96,
94,91,
88,85,
82,79,
76,73,
70,67,
64,60,
57,54,
51,48,
45,42,
39,36,
33,31,
28,25,
23,21,
18,16,
14,12,
10,9,
7,6,
4,3,
2,1,
1,0,
0,0,
0,0,
0,0,
1,1,
2,3,
4,6,
7,9,
10,12,
14,16,
18,21,
23,25,
28,31,
33,36,
39,42,
45,48,
51,54,
57,60};

//***************************  x_SW  ***************************************
//Table of x_SW (excess 8): x_SW = ROUND(8*N_samples*f*510/Fck)
//**************************************************************************

//high frequency (coloun)
//1209hz  ---> x_SW = 79
//1336hz  ---> x_SW = 87
//1477hz  ---> x_SW = 96
//1633hz  ---> x_SW = 107

const unsigned char auc_frequencyH [4] = {
107,96,
87,79};

//low frequency (row)
//697hz  ---> x_SW = 46
//770hz  ---> x_SW = 50
//852hz  ---> x_SW = 56
//941hz  ---> x_SW = 61

const unsigned char auc_frequencyL [4] = {
61,56,
50,46};


//**************************  global variables  ****************************
unsigned char x_SWa = 0x00;               // step width of high frequency
unsigned char x_SWb = 0x00;               // step width of low frequency
unsigned int  i_CurSinValA = 0;           // position freq. A in LUT (extended format)
unsigned int  i_CurSinValB = 0;           // position freq. B in LUT (extended format)
unsigned int  i_TmpSinValA;               // position freq. A in LUT (actual position)
unsigned int  i_TmpSinValB;               // position freq. B in LUT (actual position)

void stop_dtmf(void){
 x_SWb=0;
 x_SWb=0;
 TCCR1A &= 127;
 }

void start_dtmf(void){
 TCCR1A |= 128 ;
}

//**************************************************************************
// Timer overflow interrupt service routine
//**************************************************************************
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
  // move Pointer about step width aheaed
  i_CurSinValA += x_SWa;
  i_CurSinValB += x_SWb;
  // normalize Temp-Pointer
  i_TmpSinValA  =  (char)(((i_CurSinValA+4) >> 3)&(0x007F));
  i_TmpSinValB  =  (char)(((i_CurSinValB+4) >> 3)&(0x007F));
  // calculate PWM value: high frequency value + 3/4 low frequency value
  OCR1A = (auc_SinParam[i_TmpSinValA] + (auc_SinParam[i_TmpSinValB]-(auc_SinParam[i_TmpSinValB]>>2)));
}

//**************************************************************************
// Initialization
//**************************************************************************
void dtmf_init (void)
{
    TIMSK  = 0x04;                     // Int T1 Overflow enabled
    TCCR1A=0x81;
    TCCR1B=0x01;
    DDRB   = 0x02 ;                    // PORTB.1 (OC1A) as output
    stop_dtmf();
    #asm("sei");                       // Interrupts enabled
}

void Send_DTMF (char * number)
{
  char ch=0,i=0;
  for(i=0;i<strlen(number);i++){
    ch=number[i];
        if(ch=='1'){
                x_SWb = auc_frequencyL[3];
                x_SWa = auc_frequencyH[3];
        }
        else if(ch=='2'){
                x_SWb = auc_frequencyL[3];
                x_SWa = auc_frequencyH[2];
        }
        else if(ch=='3'){
                x_SWb = auc_frequencyL[3];
                x_SWa = auc_frequencyH[1];
        }
        else if(ch=='4'){
                x_SWb = auc_frequencyL[2];
                x_SWa = auc_frequencyH[3];
        }
        else if(ch=='5'){
                x_SWb = auc_frequencyL[2];
                x_SWa = auc_frequencyH[2];
        }
        else if(ch=='6'){
                x_SWb = auc_frequencyL[2];
                x_SWa = auc_frequencyH[1];
        }
        else if(ch=='7'){
                x_SWb = auc_frequencyL[1];
                x_SWa = auc_frequencyH[3];
        }
        else if(ch=='8'){
                x_SWb = auc_frequencyL[1];
                x_SWa = auc_frequencyH[2];
        }
        else if(ch=='9'){
                x_SWb = auc_frequencyL[1];
                x_SWa = auc_frequencyH[1];
        }
        else if(ch=='0'){
                x_SWb = auc_frequencyL[0];
                x_SWa = auc_frequencyH[2];
        }
        else if(ch=='*'){
                x_SWb = auc_frequencyL[0];
                x_SWa = auc_frequencyH[3];
        }
        else if(ch=='#'){
                x_SWb = auc_frequencyL[0];
                x_SWa = auc_frequencyH[1];
        }
        else if(ch=='A'){
                x_SWb = auc_frequencyL[3];
                x_SWa = auc_frequencyH[0];
        }
        else if(ch=='B'){
                x_SWb = auc_frequencyL[2];
                x_SWa = auc_frequencyH[0];
        }
        else if(ch=='C'){
                x_SWb = auc_frequencyL[1];
                x_SWa = auc_frequencyH[0];
        }
        else if(ch=='D'){
                x_SWb = auc_frequencyL[0];
                x_SWa = auc_frequencyH[0];
        }

        start_dtmf();
        delay_ms(150);
        stop_dtmf();
        delay_ms(150);
    }; // end of for
 stop_dtmf();
}

