/*
 * blink.c
 *
 * Created: 11-06-2011 18:10:38
 *  Author: limapapy
 */ 
#include <avr/io.h>
#include <util/delay.h>

int main1(){
	DDRC=0xFF;
	while(1){
	PORTC ^=0xFF;
	_delay_ms(1000);
	}	
	return 0;
}	
