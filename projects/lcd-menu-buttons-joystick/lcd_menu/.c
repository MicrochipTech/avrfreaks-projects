/*
Description:
    A library to easily access standard lcd modules with AVR series of
    MCUs from Atmel. Use with AVR studio and WinAVR.

Author:
    Panos Mavrogiannopoulos
Web:
    http://nmav.homeip.net
*/

#include <avr/io.h>
#include <inttypes.h>

#ifndef F_CPU
	#define F_CPU 2000000UL
#endif

#include <util/delay.h>

#include "lcd_menu.h"
#include "lcd.h"

int line_num=0;				 //current line, never make it uint8_t cause we use it with minus.
uint8_t number_of_lines; //takes the value of the lines for each menu
uint8_t previews_table_number=0;

char line_data[6][16]= 
{
{'M','E','N','U',' ','1','#','#','#','#','#','#','#','#','#','#'},
{'E','N','T','E','R',' ','S','U','B',' ','M','E','N','U','#','#'},
{'M','E','N','U',' ','3','#','#','#','#','#','#','#','#','#','#'},
{'M','E','N','U',' ','4','#','#','#','#','#','#','#','#','#','#'},
{'M','E','N','U',' ','5','#','#','#','#','#','#','#','#','#','#'},
{'M','E','N','U',' ','6','#','#','#','#','#','#','#','#','#','#'} 
};

	
char letter_select[4][16]= 
{
{'S','U','B',' ','M','E','N','U',' ','1','#','#','#','#','#','#'},
{'S','U','B',' ','M','E','N','U',' ','2','#','#','#','#','#','#'},
{'S','U','B',' ','M','E','N','U',' ','3','#','#','#','#','#','#'},
{'S','U','B',' ','M','E','N','U',' ','4','#','#','#','#','#','#'}
};



char menu_table[0][16];	//We copy the desired table here



uint8_t rotate_menu(uint8_t direction, uint8_t table_number)
{
uint8_t i;



	//here i select the menu i selected depending the table_number
	if(table_number==0)
	{
	number_of_lines = 6;
 	memcpy(menu_table, line_data, sizeof(line_data));
	}
	else if(table_number==1)
	{
	number_of_lines = 4;
	memcpy(menu_table, letter_select, sizeof(letter_select));
	}
	//end of menu selection

	if(previews_table_number != table_number) //if we change menu starts over the line_num
	{line_num=0;}

	if(direction==0)
	{
	line_num--;	
	if(line_num<0)
	{line_num=number_of_lines-1;}	
	}

	else if(direction==1)
	{
	line_num++;
	if(line_num>number_of_lines-1)
	{line_num=0;}
	}

	else if(direction==2)
	{
	return line_num;
	}

previews_table_number=table_number;	//keeps the previews menu so if it change to start the line number from 0 again

LCDClear();

for(i=0;i<15;i++)
{

	


	if(menu_table[line_num][i] != '#')
	{
	//first line
	LCDGotoXY(i,0);
	LCDByte(menu_table[line_num][i], 1);
	}
	
	

	if(menu_table[line_num+1][i] != '#' && line_num<number_of_lines-1)
	{
	//second line
	LCDGotoXY(i,1);
	LCDByte(menu_table[line_num+1][i], 1);
	}
	
	if(menu_table[0][i] != '#' && line_num==number_of_lines-1)
	{
	//second line
	LCDGotoXY(i,1);
	LCDByte(menu_table[0][i], 1);
	}




}
LCDGotoXY(15,0);
LCDByte('<', 1);
_delay_ms(100);
return line_num;
}


void initial_menu(uint8_t table_number)
{
int i;
line_num=0;	//i make it zero so when it returns to start over again from beginning
	//here i select the menu i selected depending the table_number
	if(table_number==0)
	{
	
	memcpy(menu_table, line_data, sizeof(line_data));
	}
	else if(table_number==1)
	{

	memcpy(menu_table, letter_select, sizeof(letter_select));
	}
	//end of menu selection

LCDClear();
for(i=0;i<15;i++)
{
	if(menu_table[0][i] != '#')
	{
	//first line
	LCDGotoXY(i,0);
	LCDByte(menu_table[0][i], 1);
	}
	
	if(menu_table[1][i] != '#')
	{
	//second line
	LCDGotoXY(i,1);
	LCDByte(menu_table[1][i], 1);
	}
	
}
LCDGotoXY(15,0);
LCDByte('<', 1);

}
