#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>
#define F_CPU 1000000ul
#include <util/delay.h>
#define LCD_PRT PORTD
#define LCD_DDR DDRD
#define LCD_PIN PIND
#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2

void delay_us(int d)
{
   _delay_us(d);

   }

void delay_ms(int d)
   {
      _delay_ms(d);
      }


void LCDCOMMAND(unsigned char command)
{
   LCD_PRT = (LCD_PRT & 0x0F) | (command & 0xF0);
   LCD_PRT &= ~ (1 << LCD_RS);
  LCD_PRT &= ~ (1 << LCD_RW); 
   LCD_PRT |= (1 << LCD_EN);
   delay_us(1);
   LCD_PRT &= ~ (1 << LCD_EN);
   delay_us(20);
   LCD_PRT = (LCD_PRT & 0x0F) | (command << 4);
   LCD_PRT |= (1 << LCD_EN);
   delay_us(1);
   LCD_PRT &= ~(1 << LCD_EN);
   }


void LCDDATA(unsigned char data)
   {
      LCD_PRT = (LCD_PRT & 0x0F) | (data & 0xF0);
      LCD_PRT |= (1 << LCD_RS);
      LCD_PRT &= ~(1 << LCD_RW);
      LCD_PRT |= (1 << LCD_EN);
      delay_us(1);
      LCD_PRT &= ~(1 << LCD_EN);
      
      
      LCD_PRT = (LCD_PRT & 0x0F) | (data << 4);
      LCD_PRT |= (1 << LCD_RS);
      LCD_PRT &= ~(1 << LCD_RW);
      LCD_PRT |= (1 << LCD_EN);
      delay_us(1);
      LCD_PRT &= ~(1 << LCD_EN);         
      }
   

void LCDINIT()
      {
	 LCD_DDR = 0xFF;
	 LCD_PRT &= ~(1 << LCD_EN);
	 delay_us(2000);
	 LCDCOMMAND(0x33);
	 delay_us(100);
	 LCDCOMMAND(0x32);
	 delay_us(100);
	 LCDCOMMAND(0x28);
	 delay_us(100);
	 LCDCOMMAND(0x0e);
	 delay_us(100);
	 LCDCOMMAND(0x01);
	 delay_us(2000);
	 LCDCOMMAND(0x06);
	 delay_us(100);
      }
      
void LCDGOTOXY(unsigned char x, unsigned char y)
{
unsigned char FIRSTCHARADDR[] = {0x80,0xc0};
LCDCOMMAND(FIRSTCHARADDR[y-1]+x-1);
delay_us(100);
}

void LCDPRINT(char*str)
{
   unsigned char i=0;
   while(str[i] != 0)
   { 
   LCDDATA(str[i]);
   i++;
   }
}


READADCRAW(uint8_t ch)
{
     DDRA = 0x00;
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX = (1 << REFS0);
   	ch=ch&0b00000111;
	ADMUX|=ch;
	_delay_us(10);
ADCSRA |= (1 << ADSC); //Start conversion
     while(( ADCSRA & (1 << ADIF)) == 0);
      ADCSRA|=(1<<ADIF); 
    _delay_us(30);
	return(ADC);
    
}



READADCAVG(uint8_t chsel)

{
  int V[256];
  int i;
  unsigned long SUM=0;  //must initialize 
  int AVG;
  for(i=0;i<256;i++)
  {
  V[i]=READADCRAW(chsel);
  SUM+=V[i];
  }

  AVG=SUM/256;
  

return(AVG);

}


  
void LCDDISPLAY(unsigned long C)
{
unsigned long D;
	unsigned long E;
    unsigned char F;
	unsigned char G;
	unsigned char H;

D = C*1464;
	   
	   E = D/1000000;
	   E = E+48;
           LCDGOTOXY(1,2);
           LCDDATA(E);
           
	   E = D%1000000;
           F = E/100000;
           F = F+48;
           
LCDDATA(F);


           LCDGOTOXY(4,2);
           
           E = E%100000;
           G = E/10000;
           G = G+48;
LCDDATA(G);
  
           E = E%10000;
           H = E/1000;
           H = H+48;
LCDDATA(H);

}


LCDDISPLAY1(int tempreading) 
{
char BUFFER[10];
tempreading=tempreading/2;
itoa(tempreading,BUFFER,10);
LCDPRINT(BUFFER);
return 0;
}



int main()
 { 
    
	unsigned long ADCREAD1;
	unsigned long ADCREAD2;
	int A;
	


    
    LCDINIT();
    LCDGOTOXY(1,1);
    LCDPRINT("Volt");
    LCDGOTOXY(3,2);
    LCDPRINT(".");
    LCDGOTOXY(8,1);
    LCDPRINT("Temp");
    LCDGOTOXY(10,2);
	LCDDATA(0xDF);  //degree sign
	LCDPRINT("c");
    
	while(1)
    
   {
    
       ADCREAD1=READADCAVG(0);	   
       _delay_us(100);
	   ADCREAD2=READADCAVG(0);
       
	   if((ADCREAD1+1==ADCREAD2)||(ADCREAD1-1==ADCREAD2)||(ADCREAD1+2==ADCREAD2)||(ADCREAD1-2==ADCREAD2))

	  {

       LCDDISPLAY(ADCREAD1);

	   }


else
{

LCDDISPLAY(ADCREAD2);

}

_delay_us(100);

A=READADCAVG(1);  //showing temparature
       LCDGOTOXY(8,2);
	   LCDDISPLAY1(A);
_delay_us(100);

 }   
    
   return 0;
 }



