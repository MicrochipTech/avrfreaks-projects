#include "pwm.h"

void pwm_setup(void){
	DDRB 	|= 0b00001000;
	DDRD 	|= 0b01101000;	
	TCCR0A	 = 0b10100011;
	TCCR0B   = 0b00000001; 
	TCCR2A   = 0b10100011;
	TCCR2B   = 0b00000001;
};//end pwm_setup()

void pwm_write(uint8_t ein, uint8_t zwi, uint8_t dri, uint8_t vir){
	PWMWRITE(ein,zwi,dri,vir);
	return;
};//end pwm_write
