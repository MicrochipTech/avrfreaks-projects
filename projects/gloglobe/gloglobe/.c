/* $Id: gloglobe.c,v 1.3 2006/10/22 18:20:19 nemo Exp $ */
#include "avrconfig.h"
#include <avr/io.h>
#include "24bit_color.h"

#define color_up 0x80
#define color_down 0x40
#define _up_mask 0x7f
#define _down_mask 0xbf
#define UP 1
#define DOWN 0

#define _red	1
#define _green	4
#define _blue	2
#define _red_mask 0xfe
#define _green_mask 0xfb
#define _blue_mask 0xfd

uint8_t cycle = 0;
extern uint8_t r_pt;
extern uint8_t g_pt;
extern uint8_t b_pt;
//turn this into the timer isr
void clock_tick( void ){
	++cycle;

        //light goes low (on)
        if (r_pt == cycle){ PORTA = (PORTA & _red_mask); }
        if (g_pt == cycle){ PORTA = (PORTA & _green_mask); }
        if (b_pt == cycle){ PORTA = (PORTA & _blue_mask); }
        //light goes high (off)
        if(cycle == 0){ PORTA = PORTA | (_red | _green | _blue);}
	/*
	//light goes high (off) 
	if (r_pt == cycle){ PORTA = (PORTA | _red); }
	if (g_pt == cycle){ PORTA = (PORTA | _green); }
	if (b_pt == cycle){ PORTA = (PORTA | _blue); }
	//light goes low (on)
	if(cycle == 0){ PORTA = PORTA & (_red_mask & _green_mask & _blue_mask); }
	*/
}

//make pin change isr
//void button_press( void ){
	//uint8_t val;
	//val = PINA;  //are you sure this is how to read it???

	//active high
	//if ((val & color_up) == color_up){ color_step(UP); }
	//if ((val & color_down) == color_up){ color_step(DOWN); }

	//active low
	//if((val | _up_mask) != _up_mask){ color_step(UP); }
	//if((PORTA | _down_mask) == _down_mask){ color_step(DOWN); }
	//color_step(1); //is it working at all?
//}

void main(){
	uint8_t cnt=0,cnt2=0;
	PORTA = 0xf0; //turn on pull-up resisitors for inputs
	DDRA = 0x0f; //porta lower nibble is outputs, upper is in w/pull-ups

	/* 
		allow hardware to set the initial color:
		use pulldowns on PORTA.5, 6 or 7
	*/
	switch(PINA & 0xf0){
		case 0x70:
			initColor(2);
			break;
		case 0xb0:
			initColor(4);
			break;
		case 0xd0:
			initColor(6);
			break;
		default:
			initColor(1);
			break;
	}

	for(;;){ //loop forever
		//PORTA ^= 0x08; //flip a bit so i know you're working
		if(cnt++ == 0xff){
			//button_press(); //again - this would be interrupt driven
			if(cnt2++ == 0x0f){
 			  incColor(1);
			  cnt2 = 0;
			}
			cnt=0;
		}
		clock_tick();
	}
}

