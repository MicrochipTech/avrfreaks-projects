/* 
   Author : OZHAN KD 
   www.KnowledgePlus.ir    
   
   ---------------------------------------------------------------
   
   Sep 18, 2013   
   
   ADC0 (PF0): Frequency setting (0.5 Hz - 100 Hz with 0.1Hz step) 
   ADC1 (PF1): Acceleration setting (100Hz frequency change time) (0.99s - 253.7s)
   ADC2 (PF2): Deceleration setting (100Hz frequency change time) (0.99s - 253.7s)
   ADC3 (PF3): Boost volatage setting (9.8% - 21.9%)
   
   PE2: Run push botton (active low)
   PE3: Stop push botton (active low)
   PE4: Dir push botton (active low)
   
   PB5: SPWM phase1
   PB6: SPWM phase2
   PB7: SPWM phase3
   
   PD0: Motor enable output (active high)
   PD1: IR2130 fault indicator input (active low)
   PD2: Run LED (active low)
   PD3: Motor direction LED1 (active low)
   PD4: Motor direction LED2 (active low)
   
   mega64 oscillator frequency = 8MHz
   Carrier frequency = 15.6 KHz

   ---------------------------------------------------------------   

*/

#include <mega64.h>
#include <delay.h>
#include "defines.h"
#include "tables.c"

enum
{
 STOP_MODE=0,
 RUN_MODE,
 FAULT_MODE
};
enum
{
 RUN_CODE=0,
 STOP_CODE,
 DIR_CODE,
 NO_KEY_CODE
};

unsigned char a_calc(unsigned char,unsigned char);
volatile unsigned int speed=0,adc_speed=0;
volatile unsigned char acceleration=1,deceleration=1,temp_key_code=NO_KEY_CODE,
mode=STOP_MODE,a_boost=A_BOOST_MIN;

struct 
{
 volatile unsigned char 
 dir_f:1,
 deceleration_f:1,
 dir_change_f:1;
}flags={0,0,0};

//----------------------------------------------------------------

void main(void)
{
 unsigned char key_code,old_key_code=NO_KEY_CODE;  
 ENABLE_DDR|=1<<ENABLE_bp;
 DISABLE_MOTOR;
 FAULT_PORT|=1<<FAULT_bp; // Turn on fault input pullup
 RUN_LED_DDR|=1<<RUN_LED_bp;
 RUN_LED_OFF;
 DIR_LED1_DDR|=1<<DIR_LED1_bp;
 DIR_LED2_DDR|=1<<DIR_LED2_bp;
 DIR_LED1_ON;
 DIR_LED2_OFF;
 KEY_PORT|=(1<<RUN_KEY_bp)|(1<<STOP_KEY_bp)|(1<<DIR_KEY_bp); // Turn on pullups 
 
 DDRB=(1<<PORTB7)|(1<<PORTB6)|(1<<PORTB5); // PWM outputs
 ADMUX=1<<REFS0; // AVcc as reference - ADC0
 ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1);  // ClkADC = ClkIO/64
 OCR1A=sine[0];
 OCR1B=sine[85];
 OCR1CL=sine[171];
 TCCR1A=1<<WGM10;
 TCCR1B=1<<CS10;  
 OCR2=124; // 1ms
 TCCR2=(1<<WGM21)|(1<<CS21)|(1<<CS20);
 TIMSK=1<<OCIE2;
 #asm("sei")
 delay_ms(1000); // For DC bus capacitor charging by NTC
 while (1)
 {
  key_code=temp_key_code;
  switch(mode)
  {
  
//----------------------------------------------------------------
  
  case STOP_MODE:
   if((FAULT_PIN&(1<<FAULT_bp))==0)
   {
    old_key_code=key_code;
    mode=FAULT_MODE; // IR2130 fault condition
    break;
   }
   if((key_code==RUN_CODE)&&(key_code!=old_key_code))
   {
    old_key_code=key_code;
    mode=RUN_MODE;
    RUN_LED_ON;
    ENABLE_MOTOR;       
   }
   else if((key_code==DIR_CODE)&&(key_code!=old_key_code))
   {
    old_key_code=key_code;
    if(flags.dir_f)
    {     
     DIR_LED1_ON;
     DIR_LED2_OFF;     
    }
    else
    {     
     DIR_LED1_OFF;
     DIR_LED2_ON;
    }
    flags.dir_f^=1;
   }
   else old_key_code=key_code;   
   break;
  
//----------------------------------------------------------------
  
  case RUN_MODE: 
   if((FAULT_PIN&(1<<FAULT_bp))==0)
   {
    DISABLE_MOTOR;
    flags.deceleration_f=0;
    flags.dir_change_f=0;
    old_key_code=key_code;
    mode=FAULT_MODE; // Fault condition
    break;
   }
   if((key_code==STOP_CODE)&&(key_code!=old_key_code))
   {
    old_key_code=key_code;    
    flags.deceleration_f=1;    
   }
   else if((key_code==DIR_CODE)&&(key_code!=old_key_code))
   {
    old_key_code=key_code;
    #asm("cli")    
    flags.deceleration_f=1;
    flags.dir_change_f=1;
    #asm("sei")    
   }
   else old_key_code=key_code;
   break;  
   
//----------------------------------------------------------------
  
  case FAULT_MODE:  
   if((FAULT_PIN&(1<<FAULT_bp))&&(key_code==STOP_CODE)&&(key_code!=old_key_code))    
   {
     old_key_code=key_code;
     mode=STOP_MODE;
     RUN_LED_OFF;
   }   
   else old_key_code=key_code;
   break;   
  }
 }
}

//----------------------------------------------------------------

unsigned char a_calc(unsigned char sine , unsigned char amplitude)
{
 unsigned int sine_temp;
 unsigned char sine1;
 sine1=((sine<128)?~sine:sine);
 sine_temp=(((unsigned int)sine1)<<1);
 sine_temp-=255;
 sine_temp*=amplitude;
 sine_temp>>=8;
 sine_temp+=255;
 sine_temp>>=1;
 sine_temp++;  
 sine1=(unsigned char)sine_temp;
 if (sine<128) sine1=~sine1;
 return sine1;
}

//----------------------------------------------------------------

interrupt [TIM2_COMP] void timer2_compare_isr(void)
{
 unsigned int final_speed;
 static unsigned char x20=0,x10=0,
 acceleration_counter=0, deceleration_counter=0;
 
 final_speed=(flags.deceleration_f ? 0 : adc_speed); 
 if((mode!=RUN_MODE)||(speed==final_speed))
 {
  acceleration_counter=0;
  deceleration_counter=0;
 }
 else if(speed<final_speed)
 {
  deceleration_counter=0;
  if(++acceleration_counter>=acceleration)
  {
   acceleration_counter=0;
   speed++;
   }
  }
 else
 {
  acceleration_counter=0;
  if(++deceleration_counter>=deceleration)
  {
   deceleration_counter=0;
   speed--;
   if((speed==0)&&(flags.deceleration_f==1))
   {
    flags.deceleration_f=0;
    if(flags.dir_change_f)
    {          
     flags.dir_change_f=0;
     if(flags.dir_f)
     {            
      DIR_LED1_ON;
      DIR_LED2_OFF;     
     }
     else
     {      
      DIR_LED1_OFF;
      DIR_LED2_ON;
     }
     flags.dir_f^=1;
    }
    
    else
    {     
     mode=STOP_MODE;
     RUN_LED_OFF;
     DISABLE_MOTOR;
    } 
   }
  }
 }

 if(speed&&(mode==RUN_MODE))
 {
  unsigned int temp16=OCR_value[speed-1];
  OCR3AH=(unsigned char)(temp16>>8);
  OCR3AL=(unsigned char)temp16;
  TCCR1A|=((1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1));
  TCCR3A=(1<<WGM31)|(1<<WGM30);
  TCCR3B=(1<<WGM33)|(1<<WGM32)|(1<<CS30);
  ETIMSK|=(1<<OCIE3A);
 }
 else
 {
  TCCR1A&=~((1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1));
  TCCR3A=0; 
  TCCR3B=0; // Return to normal mode for next immediate update
  TCNT3H=0;
  TCNT3L=0;
  ETIMSK&=~(1<<OCIE3A);
  ETIFR=(1<<OCF3A);
 }
 if(++x20==20) // 20ms 
 {
  unsigned char temp8;
  x20=0;
  temp8=KEY_PIN&((1<<DIR_KEY_bp)|(1<<STOP_KEY_bp)|(1<<RUN_KEY_bp));
  switch(temp8)
  {
  case RUN_gc:
   temp_key_code=RUN_CODE;
   break; 
  case STOP_gc:
   temp_key_code=STOP_CODE;
   break;
  case DIR_gc:
   temp_key_code=DIR_CODE;
   break;
  default:
   temp_key_code=NO_KEY_CODE;  
  }
  if(++x10==10) // 200 ms
  {
   x10=0;
   if(mode==FAULT_MODE)TOGGLE_RUN_LED;
  } 
 }
}

//----------------------------------------------------------------

interrupt [TIM3_COMPA] void timer3_comparea_isr(void)
{
 static unsigned char index=0; 
 if (speed>795)index+=16;
 else if(speed>595)index+=8;
 else if(speed>395)index+=4;
 else if(speed>195)index+=2;
 else ++index;
 
 if(speed>=SPEED_BASE)
 {
  if (flags.dir_f)
  {
   OCR1A=sine[index];  
   OCR1B=sine[(unsigned char)(index+85U)];
  }
  else
  {
   OCR1B=sine[index];  
   OCR1A=sine[(unsigned char)(index+85U)];
  } 
  OCR1CL=sine[(unsigned char)(index+171U)];
 }
 
 else
 {
  unsigned long amplitude;
  amplitude=(((255-a_boost)*(unsigned long)(speed-SPEED_BOOST))/(SPEED_BASE-SPEED_BOOST))+a_boost;
  if (flags.dir_f)
  {
   OCR1A=a_calc(sine[index],(unsigned char)amplitude);
   OCR1B=a_calc(sine[(unsigned char)(index+85U)],(unsigned char)amplitude);
  }
  else
  {
   OCR1B=a_calc(sine[index],(unsigned char)amplitude);
   OCR1A=a_calc(sine[(unsigned char)(index+85U)],(unsigned char)amplitude);
  } 
  OCR1CL=a_calc(sine[(unsigned char)(index+171U)],(unsigned char)amplitude);
 }  
}

//----------------------------------------------------------------

interrupt [ADC_INT] void ADC_INT_isr(void)
{
 static unsigned int adc_temp=0;
 static unsigned char n=0,adc_select=0;
 unsigned int temp;

 adc_temp+=ADCW;

 if(++n==64)
 {
  n=0;
  temp=(adc_temp>>6);
  adc_temp=0;
  switch (adc_select)
  {
  case 0:   
   if(temp<14)adc_speed=0;
   else if (temp<1010)adc_speed=temp-13;
   else adc_speed=996;   
   ADMUX=(1<<REFS0)|(1<<MUX0);   
   break;

  case 1:
   acceleration=(temp?(temp>>2):1);   
   ADMUX=(1<<REFS0)|(1<<MUX1);   
   break;

  case 2:
   deceleration=(temp?(temp>>2):1);   
   ADMUX=(1<<REFS0)|(1<<MUX1)|(1<<MUX0);
   break;
   
  case 3:   
   a_boost=(temp>>5)+A_BOOST_MIN; // 25 <= a_boost <= 56   
   ADMUX=(1<<REFS0);
  }
  if(++adc_select==4)adc_select=0;
 }
 ADCSRA|=(1<<ADSC);
}

//----------------------------------------------------------------

