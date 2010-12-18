/*****************************************************
Chip type           : ATmega8
Program type        : Application
Clock frequency     : 8.000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*****************************************************/

#include <mega8.h>    
#include<delay.h>
#include<stdio.h>


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





// Declare your global variables here
unsigned int i,j,k,l,m,n,o,p,t1,t2,t;           //ijk sensot ip,  l for loop
unsigned int std;                               // standard limit to black and white
char temp[5],sens[4];                           // sens sensor string , disp display string,




void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
TCCR0=0x00;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer 1 Stopped
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
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

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer 2 Stopped
// Mode: Normal top=FFh
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
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
lcd_putsf("starting up");
for(l=0;l<5;l++)
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
       
      delay_ms(1000);
      
      
      };
}


void read_sensor(void)          // read sensor
{
        i=read_adc(0); 
        j=read_adc(1);
        k=read_adc(2);
        sprintf(sens,"%d%d%d",i,j,k);
        sprintf(temp,"%d:%d:%d",i,j,k);
      
}


// environment calib

void env_c(void)
{       
        //black detection
        
        lcd_clear();
        lcd_putsf("Put BOT on Black");
        
        for(l=51;l>47;l--)
        { 
        lcd_gotoxy(0,1) ;
        lcd_putchar(l);
        delay_ms(1000);
        }
        
        
        lcd_clear();
        lcd_putsf("\nDetecting..");
        delay_ms(1000);
       {
                read_sensor(); 
                t1=(i+j+k)/3;
                sprintf(temp,"%d",t1);
                lcd_clear();
                lcd_putsf("Black Saved=");
                lcd_puts(temp);
                delay_ms(1000);
       }   
        
        
        //white detection
        
        
        lcd_clear();
        lcd_putsf("Put BOT on White");
       
        for(l=51;l>47;l--)
        {
        lcd_gotoxy(0,1) ;
        lcd_putchar(l);
        delay_ms(1000);
        
        }
        
        
        lcd_clear();
        lcd_putsf("\nDetecting..");
        delay_ms(1000); 
        {
                read_sensor(); 
                t2=(i+j+k)/3;
                sprintf(temp,"%d",t2);
                lcd_clear();
                lcd_putsf("White Saved=");
                lcd_puts(temp);
                delay_ms(1000);   
        }
        
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
        t=(t1+t2)/2;
        std=t;
        
}