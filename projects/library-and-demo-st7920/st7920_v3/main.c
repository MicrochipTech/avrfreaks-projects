/*
 * main.c
 *
 *  Created on: Apr 8, 2013
 *      Author: Peter Barnes
 */

#include "st7920.h"

int main(void)
{
    int i, j, k;
    char string1[16] = "     ST7920     ";
    char string3[16] = "   Character    ";
    char string2[16] = "      Mode      ";
    char string4[16] = "- Barnes, 2013 -";
    DATAPORT_DIR = 0xff;    // set DATAPORT to output
    DATA_PORT = 0x00;    // set outputs to 0 initially
    COMMAND_DIR = 0x07;    // set command port to output
    COMMAND_PORT = 0x00;    // set outputs to 0 initially
    clcd_init();    // initialize as character display
    cdisplay_clear();    // clear character screen and reset address to 0
    // set_cgram_address(0, 0);
    for (i = 0; i < 16; ++i) {
        write_data(string1[i]);
    }
    for (i = 0; i < 16; ++i) {
        write_data(string2[i]);
    }
    for (i = 0; i < 16; ++i) {
        write_data(string3[i]);
    }
    for (i = 0; i < 16; ++i) {
        write_data(string4[i]);
    }
    _delay_ms(3000);
    cdisplay_clear();

    glcd_enable();
    glcd_fill(0x0000);

    // gotoxy(0,0);    // read test; read from 0,0, write this to 4,32
    // i=read_word(1);
    // write_word(7, 63, i);
    // _delay_ms(1000);

    // draw a graph with x and y axis
    write_char_xy(7, 59, 0);
    write_gnumber_xy(113, 59, 100);
    write_char_xy(0, 0, 9);    //
    write_char_xy(0, 53, 0);
    // _delay_ms(200);
    draw_vertical_line(6,57,121);
    draw_horiz_line(6, 0, 57);

    // imitate a plot
    k=32;
    for (i=7; i<128; i++)
    {
    	j=rand() % 3;
    	set_pixel(i, k);
    	if (j == 1)
    	{
    		k=k-1;
    	}
    	if (j > 1)
    	{
    		k=k+1;
    	}
    }
}
