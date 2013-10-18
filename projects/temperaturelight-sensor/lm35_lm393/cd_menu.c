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
{'M','E','N','U',' ','2','(','1','S','T',' ','S','U','B',')','#'},
{'M','E','N','U',' ','3','#','#','#','#','#','#','#','#','#','#'},
{'M','E','N','U',' ','4','(','2','N','D',' ','S','U','B',')','#'},
{'M','E','N','U',' ','5','#','#','#','#','#','#','#','#','#','#'},
{'M','E','N','U',' ','6','#','#','#','#','#','#','#','#','#','#'} 
};

	
char first_sub[4][16]= 
{
{'S','U','B',' ','M','E','N','U',' ','1','#','#','#','#','#','#'},
{'S','U','B',' ','M','E','N','U',' ','2','#','#','#','#','#','#'},
{'S','U','B',' ','M','E','N','U',' ','3','#','#','#','#','#','#'},
{'S','U','B',' ','M','E','N','U',' ','4','#','#','#','#','#','#'}
};


char second_sub[4][16]= 
{
{'S','E','C','O','N','D',' ','S','U','B','M','E','N','U','1','#'},
{'S','E','C','O','N','D',' ','S','U','B','M','E','N','U','2','#'},
{'S','E','C','O','N','D',' ','S','U','B','M','E','N','U','3','#'},
{'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'}
};




char menu_table[0][16];	//We copy the desired table here



uint8_t rotate_menu(uint8_t direction, uint8_t table_number)
{
uint8_t i,j,k;



	//here i select the menu i selected depending the table_number
	if(table_number==0)
	{
	number_of_lines = 6;
 	memcpy(menu_table, line_data, sizeof(line_data));
	}
	else if(table_number==1)
	{
	number_of_lines = 4;
	memcpy(menu_table, first_sub, sizeof(first_sub));
	}
	else if(table_number==2)
	{
	number_of_lines = 3;
	memcpy(menu_table, second_sub, sizeof(second_sub));
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

	if(LCD_TYPE==2)
	{

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

}
	else if(LCD_TYPE==4)
	{

k=0;
		for(j=0;j<4;j++)
		{
	
			for(i=0;i<15;i++)
			{
				if(menu_table[line_num+j][i] != '#' && line_num+j<number_of_lines)
				{
				LCDGotoXY(i,j);
				LCDByte(menu_table[line_num+j][i], 1);
				}

			/*	else if(line_num+j>=number_of_lines)
				{
				if(menu_table[k][i] != '#')
				{LCDGotoXY(i,j); 
				LCDByte(menu_table[k][i], 1);
				k++;}
				}*/				

			}
		
	



		}

	}





LCDGotoXY(15,0);
LCDByte(127, 1);
_delay_ms(100);
return line_num;
}


void initial_menu(uint8_t table_number)
{
uint8_t i,j;
line_num=0;	//i make it zero so when it returns to start over again from beginning
	
	
	//here i select the menu i selected depending the table_number
	if(table_number==0)
	{
	memcpy(menu_table, line_data, sizeof(line_data));
	}
	else if(table_number==1)
	{
	memcpy(menu_table, first_sub, sizeof(first_sub));
	}
	else if(table_number==2)
	{
	memcpy(menu_table, second_sub, sizeof(second_sub));
	}
	//end of menu selection

LCDClear();
for(i=0;i<15;i++)
{
	//this for, prints one or four lines depending the selection 
	//of the lcd_menu.h file. If it is 16x2 lcd or 16x4
	for(j=0;j<LCD_TYPE;j++)
		{
			if(menu_table[j][i] != '#')
			{
			LCDGotoXY(i,j);
			LCDByte(menu_table[j][i], 1);
			}



		}




	
}
LCDGotoXY(15,0);
LCDByte(127, 1);

}
