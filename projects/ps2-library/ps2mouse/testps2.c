/****************************************************************************************
Title:    Testing interfacing ps/2 device (Mouse).
Author:   Jan Pieter de Ruiter  <janpieterd@lycos.nl>
Date:     13-Jun-2002
Software: AVR-GCC with AvrStudio
Hardware: Any ps2 mouse (ps2.c and ps2.h will also work with a keyboard, 
          only other commands should be used).
	       See http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/ps2/ps2.html
	       and http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/mouse/mouse.html
	       and http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/keyboard/atkeyboard.html
	       for more info
	       
          AT90S8515 is used with this project, but you can use the library with all 
          microcontrollers (Change the makefile)
          
P.S.:     I also added ps2mouse.h and ps2mouse.c to the archive.
          This library is made to ease interfacing a ps/2 mouse, but I found that it 
          does not work with all mice.
          So use these at your own risk.
****************************************************************************************/
#include <avr/io.h>
#include "ps2.h"
#include "ps2mouse.h"

int main(void)
{
	int BATcode;
	int MouseID;
	int mode=0;
	int ack;
	int mouseinf;
	int deltax;
	int deltay;
	int posx;
	int posy;
	int leftbuttonpressed;
	int middlebuttonpressed;
	int rightbuttonpressed;
	
	BATcode=Read_ps2data();
	if(BATcode!=0xAA)return 0;		// The mouse is defect
	MouseID=Read_ps2data();       // Receive MouseID (Should be 0x00, standard mouse)
	Write_ps2data(0xEA);		      // Set stream mode
	if(Read_ps2data()!=0xFA)      // Stream mode not supported on this mouse (like my Compaq mouse)
	{
		Write_ps2data(0xF0);  // Set remote mode
		Read_ps2data();       // Ignore acknowledge
		mode=1;
	}
	while(1)             // Loop forever
	{
		ack=0;
		if(mode==1)       // If remote mode
		{
			Write_ps2data(0xEB);    // Read data
			ack=Read_ps2data();
		}
		if((ack==0xFA)||(mode==0))  // If mouse send acknowledge or is in stream mode
		{
			mouseinf=Read_ps2data();
			deltax=Read_ps2data();
			deltay=Read_ps2data();
			if(mouseinf&0x10)deltax-=0x100;	// Add sign bit to deltax
			if(mouseinf&0x20)deltay-=0x100;	// Add sign bit to deltay
			posx+=deltax;   // Absolute X position
			posy+=deltay;   // Absolute Y position
			if(mouseinf&0x01)leftbuttonpressed=1;         // Get leftbutton status
			else leftbuttonpressed=0;
			if(mouseinf&0x04)middlebuttonpressed=1;       // Get middlebutton status
			else middlebuttonpressed=0;
			if(mouseinf&0x02)rightbuttonpressed=1;        // Get rightbutton status
			else rightbuttonpressed=0;
		}
	}
}
