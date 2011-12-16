#include <util/delay.h>
#include "sipo_595.h"

void night_rider(uint8_t color, uint8_t delay){
	uint8_t i=1, n=1;
	while(1){
		buff_content[color]=i;
		sipo_sendbyte(buff_content);
		if(i==0x80)	n=0;
		if(i==0x01) n=1;
		if(n==1){
			i=i<<1;
		}else{
			i=i>>1;
		}
		_delay_ms(delay);	// delay header file is included in sipo_595.h
	}
}

int main(void)
{
	sipo_init();

	// Night rider demo
	uint8_t speed = 40;			// Set running light speed (less is faster)
	night_rider(ERROR, speed);
}
