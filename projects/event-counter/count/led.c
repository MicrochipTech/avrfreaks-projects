#include <stdlib.h>
#include <avr/io.h>


#include "counter.h"
#include "led.h"

uint8_t display[4];

void set_display(int16_t in)
{
uint16_t in_abs;

	in_abs = abs(in);
	
	display[0] = in_abs / 1000 + 48; //+48 -> conversion to ascii char
	in_abs = (in_abs % 1000 ); 
	display[1] =  in_abs / 100 + 48;
	in_abs = in_abs % 100;
	display[2] = in_abs / 10 + 48;
	in_abs = in_abs % 10;
	display[3] = in_abs + 48;
	
	if (in < 0) display[0] = '-';
	
	display[0] = char_to_led(display[0]);	
	display[1] = char_to_led(display[1]);
	display[2] = char_to_led(display[2]);
	display[3] = char_to_led(display[3]);
	
}

//
// conversion from ascii/nr value to led display
//			 A
//			****
//		  F * G* B
//			****
//        E *  * C
//			****	
//           D
char char_to_led(char in)
{
uint8_t tmp;
	
//this can be done via array, let it be today 
	if (in == '0') 		tmp = A+B+C+D+E+F; 
	else if (in == '1')	tmp = B+C;
	else if (in == '2')	tmp = A+B+G+E+D;	
	else if (in == '3')	tmp = A+B+G+C+D;
	else if (in == '4')	tmp = F+G+B+C;
	else if (in == '5')	tmp = A+F+G+C+D;
	else if (in == '6')	tmp = A+F+G+C+D+E;
	else if (in == '7')	tmp = A+B+C;
	else if (in == '8')	tmp = A+B+C+D+E+F+G;
	else if (in == '9')	tmp = A+B+G+F+C+D;
	else if (in == '-')	tmp = G;
	else tmp=0; //unknow character

	return(tmp);
}

//called every 5ms
void refresh_led(void)
{
static uint8_t position; //position of current LEDisplay
	
	//position of currently displayed LED segment (0,1,2 or 3)
	position++;
	if (position >= NR_LED) position = 0;		
	
	set_led_display(position, display[position]);
		
}

//	hw layer
//  
void set_led_display(uint8_t control, uint8_t data)
{
	PORTB &= ~_BV(SEG1000_OB);	
	PORTD &= ~_BV(SEG100_OD);
	PORTD &= ~_BV(SEG10_OD);
	PORTD &= ~_BV(SEG1_OD);
	
	if (data & A) PORTD &= ~_BV(LED_A_OD); else PORTD |= _BV(LED_A_OD);
	if (data & B) PORTD &= ~_BV(LED_B_OD); else PORTD |= _BV(LED_B_OD);
	if (data & C) PORTD &= ~_BV(LED_C_OD); else PORTD |= _BV(LED_C_OD);
	if (data & D) PORTD &= ~_BV(LED_D_OD); else PORTD |= _BV(LED_D_OD);
	if (data & E) PORTD &= ~_BV(LED_E_OD); else PORTD |= _BV(LED_E_OD);
	if (data & F) PORTB &= ~_BV(LED_F_OB); else PORTB |= _BV(LED_F_OB);
	if (data & G) PORTB &= ~_BV(LED_G_OB); else PORTB |= _BV(LED_G_OB);
	
	if (control == 0) PORTB |= _BV(SEG1000_OB);
	else if (control == 1) PORTD |= _BV(SEG100_OD);
	else if (control == 2) PORTD |= _BV(SEG10_OD);
	else PORTD |= _BV(SEG1_OD);
}



