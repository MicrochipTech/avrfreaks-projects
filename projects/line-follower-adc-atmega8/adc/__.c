/*****************************************************
Chip type           : ATmega8
Program type        : Application
Clock frequency     : 8.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256



PLEASE NOTE 


lcd on port d

sensors on adc 0,1 and 3
outputs
steering portb 0and 1
wheel motor on portb 6 and 7

FOR CIRCUIT AND FURTHER DETAILS MAIL TO 
                        "anoopjangra@gmail.com"
                        with subject line 
                        "REQUEST FOR LINE FOLLOWER CIRCUIT"

*****************************************************/

#include <mega8.h>    
#include<delay.h>
#include<stdio.h>
#include<string.h>


#asm
   .equ __lcd_port=0x12 ;PORTD
#endasm
#include <lcd.h>

#define ADC_VREF_TYPE 0x20

// Read the 8 most significant bits
unsigned char read_adc(unsigned char adc_input)
{
ADMUX=adc_input|ADC_VREF_TYPE;
// Start the AD conversion
ADCSRA|=0x40;
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCH;
}
    



void read_sensor();
void env_c();      
void comp_v();
void move_();       

#define STL     0x01
#define STR     0x02
#define STC     0x00
#define MOVF    0x80 
#define MOVR    0x40
#define MOVS    0x00








// Declare your global variables here
unsigned int i,j,k,l,t1,t2,t;               //ijk sensot ip,  l for loop
unsigned int s[3],sw[3],sb[3];                  // standard limit to black and white
int m,n;                                        // move variables
char x,y,z,a=0,temp[10],sens[4];                    // sens sensor string , disp display string,




void main(void)
{
// Declare your local variables here


PORTB=0x00;
DDRB=0xC3;        //0,1,6,7 are outputs

PORTC=0x00;
DDRC=0x00;

PORTD=0x00;
DDRD=0x00;

TCCR0=0x00;
TCNT0=0x00;

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

ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC Clock frequency: 125.000 kHz
// ADC Voltage Reference: AREF pin
// Only the 8 most significant bits of
// the AD conversion result are used
ADMUX=ADC_VREF_TYPE;
ADCSRA=0x86;



// LCD module initialization
lcd_init(16);

lcd_clear();
lcd_putsf("Starting Up");
for(l=0;l<10;l++)
{
        lcd_putsf(".");
        delay_ms(100);
} 

delay_ms(1000);
lcd_clear();


//inital calib
env_c();




while (1)
      {
      read_sensor();
      comp_v(); 
      lcd_clear();
      move_();
      //delay_ms(100);
       a++;    
      };
}


void read_sensor(void)          // read sensor
{
        i=read_adc(0); 
        j=read_adc(1);
        k=read_adc(2);
        
        
      
}


// environment calib

void env_c(void)
{       
        //black detection
        
        
        
        for(l=51;l>47;l--)
        {       lcd_clear();
                lcd_putsf("Put BOT on Black");
                lcd_putchar(l);
                delay_ms(1000);
        }
        
        
        lcd_clear();
        lcd_putsf("\nDetecting..");
        delay_ms(1000);
       
                read_sensor();
                sb[0]=i;
                sb[1]=j;
                sb[2]=k; 
                t1=(i+j+k)/3;
                sprintf(temp,":%d:%d:%d:",i,j,k);
                lcd_clear();
                lcd_putsf("Black=\n");
                lcd_puts(temp);
                delay_ms(1000);
          
        
        
        //white detection
        
        
        
        for(l=51;l>47;l--)
        {
                lcd_clear();
                lcd_putsf("Put BOT on White");
                lcd_putchar(l);
                delay_ms(1000);
        
        }
        
        
        lcd_clear();
        lcd_putsf("\nDetecting..");
        delay_ms(1000); 
       
                read_sensor(); 
                sb[0]=i;
                sb[1]=j;
                sb[2]=k; 
                t2=(i+j+k)/3;
                sprintf(temp,":%d:%d:%d:",i,j,k);
                lcd_clear();
                lcd_putsf("White=\n");
                lcd_puts(temp);
                delay_ms(1000);   
       
        
        // if there is a little or no diff b/w black and white
                              
        if(t2<=t1)
        {
        lcd_clear();
        lcd_putsf("Error!\nPlease Recalbrt");
        delay_ms(2000);
        env_c();
        }
        
        else
        {
                t=t2-t1;
                if(t<5)
                {
                        lcd_clear();
                        lcd_putsf("Error!\nPlease Recalbrt");
                        delay_ms(2000);
                        env_c();
                }
                
        } 
         
        lcd_clear();
        lcd_putsf("Detection Done!");
        delay_ms(2000);
        lcd_clear();
                //t=(t1+t2)
                //std=t;
        for(l=0;l<3;l++) s[l]=(sw[l]+sb[l])/2;        
}             


//assign black and white

void comp_v()
{
        if(i<=s[0])     x='B';
        else            x='W';
        if(j<=s[1])     y='B';
        else            y='W';
        if(k<=s[2])     z='B';
        else            z='W';
        
        sprintf(sens,"%c%c%c",x,y,z);
 

}   

//writing to motors

void move_()
{
      if(((x=='W')&(y=='B')&(z=='W'))||((x=='B')&(y=='W')&(z=='B'))) m=1;
      else if(((x=='W')&(y=='B')&(z=='B'))||((x=='W')&(y=='W')&(z=='B'))) m=2;
      else if(((x=='B')&(y=='W')&(z=='W'))||((x=='B')&(y=='B')&(z=='W'))) m=3;
      else if(((x=='W')&(y=='W')&(z=='W'))||((x=='B')&(y=='B')&(z=='B'))) m=0;
      
      if(m!=0) n=m;

      lcd_putsf("SENSORS  ACTION\n");
      lcd_puts(sens);      
      switch(m?m:n)
      {
        case 1: PORTB=(STC|((a%4==0)?MOVF:MOVS));lcd_putsf("      forward"); break;
        
        case 2: PORTB=(STR|((a%4==0)?MOVF:MOVS));lcd_putsf("      right"); break;
        
        case 3: PORTB=(STL|((a%4==0)?MOVF:MOVS));lcd_putsf("      left"); break;
        
        default: PORTB=0x00;
      }
      
      
}
