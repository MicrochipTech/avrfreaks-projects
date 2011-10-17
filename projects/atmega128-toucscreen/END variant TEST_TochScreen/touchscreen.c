#include <avr/io.h> //Стандартная библиотека ввода/вывода
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "lcd_driver.h"
#include "lcd_driver.c"
#include "UART_routines.h"

void waitms(int ms);

uint8_t fg=0, bg=255;

uint8_t  print_hex(uint8_t x, uint8_t y, uint16_t char_, uint8_t fg, uint8_t bg);

static uint8_t s_hex[16] PROGMEM =
{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

int main(void)
{

	uint16_t x,y,u;
	uint8_t h,l;

	init_lcd();
	clrscr(0,0,131,131,255);
//	putstr_pgm(10,5, hello,fg,bg);
//	print_hex(50,50,0x1234,fg,bg);
//		while(1){}

	ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);	//АЦП включен, прескаллер=/128 
		waitms(10);
		while(1)
		{ 
/*		DDRF=0x01;			//Ждем нажатия
		PORTF=0x00; 

		waitms(1);


		ADMUX=(1<<REFS0)|(1<<MUX1)|(1<<MUX0);
		ADCSRA|=(1<<ADSC);
	
		while ((ADCSRA&_BV(ADIF))==0x00); 	//Ждем окончания преобразования  ;        
			l=ADCL;
			h=ADCH;
			//u=ADCH;

			u = l + ((unsigned int)h << 8);					
			
		putstr_ram(5, 25, "U=", fg, bg);
		print_hex(20,25,u,fg,bg);	
			
				 
			if (u<2) //Если было нажатие 
			 
			{  	
*/		
			DDRF= 0x0C;		//Считываем X координату   
			PORTF=0x08; 
			 
	        waitms(1);
  
			ADMUX=(1<<REFS0)|(1<<MUX0);   // ADC0         
			ADCSRA|=(1<<ADSC);
	         
			while ((ADCSRA&_BV(ADIF))==0x00); 	// Ждем окончания преобразования;         		   

			 l=ADCL;
			 h=ADCH;
			 //x=ADCH;

			x = l + ((unsigned int)h << 8);

/////////////////////////////////////////////////////////////////////////////////////////

			DDRF= 0x03;		// Считываем Y координату
			PORTF=0x01;  
	        
			waitms(1);
    
			ADMUX=(1<<REFS0)|(1<<MUX0)|(1<<MUX1);    // ADC3      
			ADCSRA|=(1<<ADSC);
	         
			while ((ADCSRA&_BV(ADIF))==0x00); 	// Ждем окончания преобразования 
			 l=ADCL;
			 h=ADCH;

			// y=ADCH;
			 y = l + ((unsigned int)h << 8);	
			 		
		// 10bit - ADLAR=0	                  		
		//y-=0x6D; -min ; 0x374 -max
		//x-=0x4D; -min ; 0x3B6 -max

		// 8bit - ADLAR=1
		//y-=0x1A; //0x1A-min ; 0xDD - max
		//x-=0x10; //0x10-min ; 0xEC - max
		
		// y = 0x2E //  болтанка по x & y
		// x = 0x44 //


			y=(y-0x6D)/(0x376/128);		//Вычисляем координаты нажатия		   
			x=(x-0x4D)/(0x3B6/128); 

		
		

			set_Pixel(x,y,fg,4);


		putstr_ram(5, 5, "X=", fg, bg);
		print_hex(20,5,x,fg,bg);

		putstr_ram(5, 15, "Y=", fg, bg);				
		print_hex(20,15,y,fg,bg);					   

	

			//}//if
		}// while

}//main



///////////////////////////////////
void waitms(int ms) {
  int i;
  for (i=0;i<ms;i++) _delay_ms(1);
}
///////////////////////////////////


uint8_t  print_hex(uint8_t x,uint8_t y,uint16_t char_,uint8_t  fg,uint8_t bg)
{//вывести на LCD 16ти битное число
	uint16_t temp;

	temp = char_;
	temp &= 0xF000;
	temp = temp>>12;

	unsigned char *f = s_hex + temp;
	uint8_t a = pgm_read_byte(f);
	
	x = putch(x,y,a,fg,bg);
	
//////////////////////////////////	
	temp = char_;
	temp &= 0xF00;
	temp = temp>>8;

	unsigned char *g = s_hex + temp;
	a = pgm_read_byte(g);
	
	x = putch(x,y,a,fg,bg);

	temp = char_;
	temp &= 0xF0;
	temp = temp>>4;

	unsigned char *p = s_hex + temp;
	a = pgm_read_byte(p);	

	x = putch(x,y,a,fg,bg);
/////////////////////////////////////
	temp = char_;
	temp &= 0x0F;
	
	unsigned char *s = s_hex + temp;
	a = pgm_read_byte(s);
	
	x = putch(x,y,a,fg,bg);	

////////////////////////////////////
return x;					
}





