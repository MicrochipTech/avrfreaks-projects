#include<avr/io.h>
#define F_CPU 16000000UL
#include<util/delay.h>
#include "pstwo.h"

#define DELX (dapack[1]-(((dapack[0] & (1 << 4)) >> 4) << 8))
#define DELY (dapack[2]-(((dapack[0] & (1 << 5)) >> 5) << 8))


void led(uint8_t n) {
    DDRD |= (1 << 6);
    if(n==0)
        PORTD &= ~(1 << 6);
    else if(n==1)
        PORTD |= (1 << 6);
    return;
}

void init_mouse(void) {
	send_packet(0xff);
	read_packet(); //Ack
	read_packet(); //Bat
	read_packet(); //dev ID
	////
	send_packet(0xf4); //Enable Data reporting
	read_packet();	// Ack
	////
	//send_packet(0xe8); //Set Resolution
	//read_packet(); //Ack
	//send_packet(0x01); //8counts/mm
	//read_packet(); //Ack
	////
    //send_packet(0xf3); //SetSample rate
    //read_packet(); //Ack
    //send_packet(0x64); //200 smaples a second
	send_packet(0xf0); //Set remote mode
	read_packet(); //Ack
}

int main(void) {
    init_mouse();
    uint8_t i,dapack[3];
    while(1) {
        send_packet(0xeb);
        read_packet(); //Ack

        for(i=0;i<3;i++)
            dapack[i]=read_packet();
        led(dapack[0] & (1 << 0));
    }
}

