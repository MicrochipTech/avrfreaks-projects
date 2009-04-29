/*****************************************************
This program was produced by the
CodeWizardAVR V2.03.8a Standard
Automatic Program Generator
© Copyright 1998-2008 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 2009-04-11
Author  : Thomas Johansson
Company : 
Comments: Simple program that convert WGS84 latitude and longitude to 
          the new Swedish grid SweRef99 TM
          See also an Excel-file in this package.  


Chip type           : ATmega2561V
Program type        : Application
Clock frequency     : 3,686400 MHz
But runs in         : 3,705040 MHz
Memory model        : Small
External RAM size   : 0
Data Stack size     : 2048
*****************************************************/

#include <mega2561.h>

// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x08 ;PORTC
#endasm
#include <lcd.h>

#include <delay.h>  //delay_ms()
#include <math.h>
#include <stdio.h>  //sprintf()
#include <stdlib.h>
#include <string.h>

//Some structures that can be handy in future applications
struct struct_sweref99  
{
    float n;
    float e;     
}; 

struct struct_sweref99 swe;

struct struct_sweref99 wgs842sweref( float latitude, float longitude );

char buf[20];      //LCD-buffer
char stemp1[10];   //Used for float to ascii conversion.
char stemp2[10];

// This is a test array from the PDF-document "kontrollpunkter_sweref99tm.pdf"
// Note that this is converted to degrees only.

// idx, used to select one of 15 values
int idx;

float control_values[15][2] = {
        {55,12.75}, 
        {55,14.25},
        {57,12.75},
        {57,19.50},
        {59,11.25},
        {59,19.50},
        {61,12.75},
        {61,18.75},
        {63,12.00},
        {63,19.50},
        {65,13.50},
        {65,21.75},
        {67,16.50},
        {67,24.00},
        {69,21.00}
      };



void main(void)
{


// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Port E initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTE=0x00;
DDRE=0x00;

// Port F initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTF=0x00;
DDRF=0x00;

// Port G initialization
// Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State5=T State4=T State3=T State2=T State1=T State0=T 
PORTG=0x00;
DDRG=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=FFh
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x00;
TCCR0B=0x00;
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 3686,400 kHz
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// OC1C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: On
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;
OCR1CH=0x00;
OCR1CL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer 2 Stopped
// Mode: Normal top=FFh
// OC2A output: Disconnected
// OC2B output: Disconnected
ASSR=0x00;
TCCR2A=0x00;
TCCR2B=0x00;
TCNT2=0x00;
OCR2A=0x00;
OCR2B=0x00;

// Timer/Counter 3 initialization
// Clock source: System Clock
// Clock value: Timer 3 Stopped
// Mode: Normal top=FFFFh
// OC3A output: Discon.
// OC3B output: Discon.
// OC3C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 3 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR3A=0x00;
TCCR3B=0x00;
TCNT3H=0x00;
TCNT3L=0x00;
ICR3H=0x00;
ICR3L=0x00;
OCR3AH=0x00;
OCR3AL=0x00;
OCR3BH=0x00;
OCR3BL=0x00;
OCR3CH=0x00;
OCR3CL=0x00;

// Timer/Counter 4 initialization
// Clock source: System Clock
// Clock value: Timer 4 Stopped
// Mode: Normal top=FFFFh
// OC4A output: Discon.
// OC4B output: Discon.
// OC4C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 4 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR4A=0x00;
TCCR4B=0x00;
TCNT4H=0x00;
TCNT4L=0x00;
ICR4H=0x00;
ICR4L=0x00;
OCR4AH=0x00;
OCR4AL=0x00;
OCR4BH=0x00;
OCR4BL=0x00;
OCR4CH=0x00;
OCR4CL=0x00;

// Timer/Counter 5 initialization
// Clock source: System Clock
// Clock value: Timer 5 Stopped
// Mode: Normal top=FFFFh
// OC5A output: Discon.
// OC5B output: Discon.
// OC5C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 5 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR5A=0x00;
TCCR5B=0x00;
TCNT5H=0x00;
TCNT5L=0x00;
ICR5H=0x00;
ICR5L=0x00;
OCR5AH=0x00;
OCR5AL=0x00;
OCR5BH=0x00;
OCR5BL=0x00;
OCR5CH=0x00;
OCR5CL=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
// INT3: Off
// INT4: Off
// INT5: Off
// INT6: Off
// INT7: Off
EICRA=0x00;
EICRB=0x00;
EIMSK=0x00;
// PCINT0 interrupt: Off
// PCINT1 interrupt: Off
// PCINT2 interrupt: Off
// PCINT3 interrupt: Off
// PCINT4 interrupt: Off
// PCINT5 interrupt: Off
// PCINT6 interrupt: Off
// PCINT7 interrupt: Off
// PCINT8 interrupt: Off
// PCINT9 interrupt: Off
// PCINT10 interrupt: Off
// PCINT11 interrupt: Off
// PCINT12 interrupt: Off
// PCINT13 interrupt: Off
// PCINT14 interrupt: Off
// PCINT15 interrupt: Off
// PCINT16 interrupt: Off
// PCINT17 interrupt: Off
// PCINT18 interrupt: Off
// PCINT19 interrupt: Off
// PCINT20 interrupt: Off
// PCINT21 interrupt: Off
// PCINT22 interrupt: Off
// PCINT23 interrupt: Off
PCMSK0=0x00;
PCMSK1=0x00;
PCMSK2=0x00;
PCICR=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=0x00;
// Timer/Counter 1 Interrupt(s) initialization
TIMSK1=0x00;
// Timer/Counter 2 Interrupt(s) initialization
TIMSK2=0x00;
// Timer/Counter 3 Interrupt(s) initialization
TIMSK3=0x00;
// Timer/Counter 4 Interrupt(s) initialization
TIMSK4=0x00;
// Timer/Counter 5 Interrupt(s) initialization
TIMSK5=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
ADCSRB=0x00;


// LCD module initialization
lcd_init(20);
      
  // Show Splash screen      
  lcd_gotoxy(0,0);
  lcd_putsf("WGS84 to SWEREF99TM");

for (idx =0; idx < 14; idx++) { 
               
  // This is the actual routine for conversion   
  swe = wgs842sweref( control_values[idx][0], control_values[idx][1] );

  // Convert the actual Northing and Easting
  ftoa( swe.n, 0, stemp1 );         // Convert Float to string with 0 decimals
  ftoa( swe.e, 0, stemp2 );         //

  lcd_gotoxy(0,1);                    // Display N at row 2
  sprintf(buf,"N: %s", stemp1  );
  lcd_puts(buf); 
     
  lcd_gotoxy(0,2);                    // Display E at row 3
  sprintf(buf,"E: %s", stemp2  );
  lcd_puts(buf);
      
  delay_ms(5000);
}
        
while (1){ };
}


// atanh doesn't exist in standard math.h
// I just added it here
float atanh(float value)
{
  float t;
  t = log((1 / value + 1) / (1 / value -1)) / 2;
  return t;
}


struct struct_sweref99 wgs842sweref( float latitude, float longitude )
{
    struct struct_sweref99 ret;
    
    //Convert to radian
    float lat_rad  = latitude  * 0.01745329;    // lat  * PI/180
    float long_rad = longitude * 0.01745329;    // long * PI/180    
    
    // long  wgs84_a  = 6378137;                // Not used
    float wgs84_e2 = 0.006694380023;

    
    //Constants, please see documents in this package for further explanation
    float tr_a = wgs84_e2;
    float tr_b = 0.0000372956;	
    //float tr_c = 0.0000002593;	           // Not used
    //float tr_d = 0.0000000020;               // Not used
    float tr_beta_1 = 0.000837731825;          // S6	
	float tr_beta_2 = 0.000000760853;	       // S7
    float tr_beta_3 = 0.000000001198;	       // S8
    //float tr_beta_4 = 0.000000000002;	       // Not used

    float tr_a_tak = 6367449.14577;	
    float tr_lambda0 = 0.261799387799;	
    float tr_korr0 = 0.9996;
    float tr_FE = 500000;
    float tr_r_lambda = long_rad - tr_lambda0; 
    // =H12-SIN(H12)*COS(H12)*((M6+M7*SIN(H12)*SIN(H12)))
    float tr_fi_star = lat_rad - sin(lat_rad)*cos(lat_rad)*((tr_a+tr_b*sin(lat_rad)*sin(lat_rad)));
    //=ATAN(TAN(M16)/COS(M12))
    float tr_teta_prim = atan(tan(tr_fi_star)/cos(tr_r_lambda)); //M17
    //=ATANH(COS(M16)*SIN(M12))
    float tr_n_prim = atanh(cos(tr_fi_star)*sin(tr_r_lambda));   //M18
    
    
    // N: =M13*M10*((M17+S6*SIN(2*M17)*COSH(2*M18)+S7*SIN(4*M17)*COSH(4*M18)+S8*SIN(6*M17)*COSH(6*M18)))
    float sweref99TM_N = tr_korr0*tr_a_tak*((tr_teta_prim+tr_beta_1*sin(2*tr_teta_prim)*cosh(2*tr_n_prim)+tr_beta_2*sin(4*tr_teta_prim)*cosh(4*tr_n_prim)+tr_beta_3*sin(6*tr_teta_prim)*cosh(6*tr_n_prim)));
    
    // E: =M13*M10*((M18+S6*COS(2*M17)*SINH(2*M18)+S7*COS(4*M17)*SINH(4*M18)))+M15
    float sweref99TM_E = tr_korr0*tr_a_tak*((tr_n_prim+tr_beta_1*cos(2*tr_teta_prim)*sinh(2*tr_n_prim)+tr_beta_2*cos(4*tr_teta_prim)*sinh(4*tr_n_prim)))+tr_FE;
    
    ret.n = sweref99TM_N;
    ret.e = sweref99TM_E;
    return ret;
}