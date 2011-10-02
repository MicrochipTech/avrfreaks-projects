#ifndef PWM_H
#define PWM_H

#ifndef F_CPU
	#define F_CPU 		1000000ULL
#endif  /* F_CPU */

#include <avr/io.h>
#include <util/delay.h>

//TIMER0A PWM
#define PWMT0A_PORT	PORTB
#define PWMT0A_DDR	DDRB
#define PWMT0A_PIN	PINB
#define PWM_T0A_OUT	2

//TIMER0B PWM
#define PWMT0B_PORT	PORTD
#define PWMT0B_DDR	DDRD
#define PWMT0B_PIN	PIND
#define PWM_T0B_OUT	5

//TIMER1A PWM
#define PWMT1A_PORT	PORTB
#define PWMT1A_DDR	DDRB
#define PWMT1A_PIN	PINB
#define PWM_T1A_OUT	3

//TIMER1B PWM
#define PWMT1B_PORT	PORTB
#define PWMT1B_DDR	DDRB
#define PWMT1B_PIN	PINB
#define PWM_T1B_OUT	4



//TIMER1 PWM
#define PWMT1_PORT	PORTD
#define PWMT1_DDR	DDRD
#define PWMT1_PIN	PIND

#define PWM_OUT1_PIN	4
#define PWM_OUT2_PIN	5


//predefinicje funkcji:
void PWM_T0_init(void);
void PWM_T1_init(void);


#endif
