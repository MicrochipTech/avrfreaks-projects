#include <mega8.h>

// Alphanumeric LCD Module functions
#asm
   .equ __lcd_port=0x12 ;PORTD
#endasm 

#include <lcd.h>
#include <delay.h>

#define set_up    PINB.6
#define set_down  PINB.7

#define fan       PORTB.5
#define buzzer    PORTB.4
#define fan_led   PORTB.3
#define alarm     PORTB.2
#define normal    PORTB.1
#define backlight PORTB.0

unsigned int gas_data;
 
eeprom unsigned int gas_set=500;

const unsigned char text_g[]="GAS VALUE : ";
const unsigned char text_a[]="ALARM SET : "; 
flash unsigned char text_s[]="* SENSOR ERROR *";
flash unsigned char text_o[]="** NO  SENSOR **";

// Read the AD conversion result
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | 0x40;
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=0x40;//0100 0000
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}

void main(void)
{

// Input/Output Ports initialization
// Port B initialization
// Func7=In Func6=In Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=P State6=P State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTB=0xC0; // 1100 0000
DDRB=0x3F;  // 0011 1111

// Port C initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTD=0x00;
DDRD=0xFF;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
MCUCR=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

// ADC initialization
// ADC Clock frequency: 250.000 kHz
// ADC Voltage Reference: AVCC pin
ADMUX=0x40; //0100 0000;
ADCSRA=0x85;

// LCD module initialization
lcd_init(16);

while (1) //for(;;){}
      {    
      gas_data=read_adc(0);
      
      lcd_gotoxy(0,0);
      if(gas_data<10)
        {
        lcd_putsf(text_o);
        }
      else if(gas_data>1020)
        {
        lcd_putsf(text_s);
        }
      else
        {
        lcd_putsf(text_g); 
        lcd_putchar(gas_data/1000|0x30); //2568
        lcd_putchar((gas_data/100)%10|0x30);
        lcd_putchar((gas_data/10)%10|0x30);
        lcd_putchar(gas_data%10|0x30);
        }
        
      lcd_gotoxy(0,1);
      lcd_putsf(text_a);
      lcd_putchar(gas_set/1000|0x30);
      lcd_putchar((gas_set/100)%10|0x30);
      lcd_putchar((gas_set/10)%10|0x30);
      lcd_putchar(gas_set%10|0x30); 
      
      delay_ms(250);
      
      if(gas_data>gas_set && gas_data<1020)
        {
        fan=1;
        fan_led=1;
        normal=0;
        alarm=~alarm;
        buzzer=~buzzer;
        backlight=~backlight;
        }
      else
        {
        if(gas_data<10 || gas_data>1020)
            alarm=~alarm;
        else
            alarm=0;
                
        fan=0; 
        fan_led=0;  
        buzzer=0;
        normal=1;
        backlight=1;
        }
        
      //**********************************
      if(set_up==0)
        {
        if(gas_set<1000)
                gas_set+=50;
        delay_ms(500);
        }
        
      if(set_down==0)
        {
        if(gas_set>100)
                gas_set-=50;
        delay_ms(500);
        }
     //***************************************
     
      };//end while
}
