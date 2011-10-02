#include "pwm.h"

void PWM_T0_init(void)
{
	TCCR0A |= (1<<COM0A1)|(1<<COM0A0)|(1<<COM0B1)|(1<<COM0B0)|(1<<WGM01)|(1<<WGM00);//tryb szybkiej mod. PWM 
	TCCR0B |= (1<<CS01);										//preskaler 8, przy f=4MHz Fpwm=960 Hz dla obu kanalow PWM
	PWMT0A_DDR  |=  _BV(PWM_T0A_OUT); 							//linia portu dla PWMT0A jako wyjście
	PWMT0A_PORT &= ~_BV(PWM_T0A_OUT);							//bez podciągania
	OCR0A = 0x00;												//poczatkowy stan licznika dla PWM od T0A

	PWMT0B_DDR  |=  _BV(PWM_T0B_OUT); 							//linia portu dla PWMT0A jako wyjście
	PWMT0B_PORT &= ~_BV(PWM_T0B_OUT);							//bez podciągania
	OCR0B = 0x00;												//poczatkowy stan licznika dla PWM od T0B
}	

void PWM_T1_init(void)
{
	TCCR1A |= (1<<COM1A1)|(1<<COM1A0)|(1<<COM1B1)|(1<<COM1B0)|(1<<WGM12)|(1<<WGM10);//tryb szybkiej mod. PWM 
	TCCR1B |= (1<<CS11);										//preskaler 8, przy f=4MHz Fpwm=960 Hz dla obu kanalow PWM
	PWMT1A_DDR  |=  _BV(PWM_T1A_OUT); 							//linia portu dla PWMT1A jako wyjście
	PWMT1A_PORT &= ~_BV(PWM_T1A_OUT);							//bez podciągania
	OCR1A = 0x00;												//poczatkowy stan licznika dla PWM od T1A

	PWMT1B_DDR  |=  _BV(PWM_T1B_OUT); 							//linia portu dla PWMT1A jako wyjście
	PWMT1B_PORT &= ~_BV(PWM_T1B_OUT);							//bez podciągania
	OCR1B = 0x00;												//poczatkowy stan licznika dla PWM od T1B	
}

