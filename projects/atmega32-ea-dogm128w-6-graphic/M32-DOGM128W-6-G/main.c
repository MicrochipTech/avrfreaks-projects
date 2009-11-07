#include "global.h"
#include "delay.h"

void delay_us(u32 time_us);

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include <avr/pgmspace.h>
#include "graphics.h" 


#include "Test_bitmap.h"



int main(void)
{

		InitDelay();


//		g_draw_bitmap_from_flash(1,1,Kamera);

		spi_init();

		Delay(1000); //Inicialization 100ms Default delay is 100us

		disp_init();

		disp_clear();

		while(1)
		{

		disp_set_pixel(5,5,1);
		disp_set_pixel(22,23,1);
		g_draw_rect(20,20,30,30);
		g_draw_string(10,10,"Test",1);
		g_fill_rect(36,25,15,14);
	ks0108DrawCircle(64,32,15,1);
		g_draw_data_column_transparentBG(18,24,1);
		g_draw_line(0,0,32,0); // horizontal
		g_draw_line(0,0,32,1);// vertical
		GLCD_Line(0,0,128,64,1);
	  disp_frame_end();
	//	g_draw_bitmap_from_flash(0, 0, Kamera);
		Delay(10000); //Inicialization 100ms Default delay is 100us

		disp_clear();

	ks0108DrawRoundRect(20,20,35,35,5,1);
	ks0108DrawCircle(64,32,15,1);

		GLCD_Line(0,0,17,0,1);
		GLCD_Line(0,0,0,17,1);
		GLCD_Line(17,17,0,17,1);
		GLCD_Line(17,17,17,0,1);
	  disp_frame_end();

		Delay(10000); //Inicialization 100ms Default delay is 100us

		g_draw_bitmap_from_flash(1,1,A_);
		disp_frame_end();
		Delay(10000); //Inicialization 100ms Default delay is 100us

		g_draw_bitmap_from_flash(1,1,B_);
		disp_frame_end();
		Delay(10000); //Inicialization 100ms Default delay is 100us

		g_draw_bitmap_from_flash(1,1,C_);
		disp_frame_end();
		Delay(10000); //Inicialization 100ms Default delay is 100us

		g_draw_bitmap_from_flash(1,1,D_);
		disp_frame_end();
		Delay(10000); //Inicialization 100ms Default delay is 100us

		g_draw_bitmap_from_flash(1,1,E_);
		disp_frame_end();
		Delay(10000); //Inicialization 100ms Default delay is 100us

		g_draw_bitmap_from_flash(1,1,F_);
		disp_frame_end();
		Delay(10000); //Inicialization 100ms Default delay is 100us

		g_draw_bitmap_from_flash(1,1,G_);
		disp_frame_end();
		Delay(10000); //Inicialization 100ms Default delay is 100us

		disp_clear();

		g_draw_bitmap_from_flash(1,1,Kamera);
		disp_frame_end();
		Delay(10000); //Inicialization 100ms Default delay is 100us
		disp_clear();

		}


}


void delay_us(u32 time_us)
{
	u32 i;
	if (time_us < 1 ) time_us = 1;
	for (i=0; i < time_us*2; i++) {};
}


