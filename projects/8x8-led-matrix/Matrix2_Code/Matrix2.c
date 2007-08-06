
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/iom32.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "LEDMtrx.c"
#include "game_of_life.h"

/*
*********************************************************************************************************
* Constants
*********************************************************************************************************
*/
const uint8_t number_rows = 16;
const uint8_t number_columns = 16; 

void r_rand(uint8_t reps);
uint8_t check_bit(uint8_t i, uint8_t j);
uint8_t Stuck;

/*
*********************************************************************************************************
* Code
*********************************************************************************************************
*/
int main(void)
{
uint16_t reps;

	MAX7219_Init();

	Clear_Screen(1);
	Screen1_pos = 16;
	Disp_Char(1, 'J', 0);
	Disp_Char(1, 'o', 1);
	Disp_Char(1, 'e', 1);
	Screen1_pos = 5;
	Disp_Char(1, 'C', 1);
	Show_Screen(1);
	delay(10);
	Trans_1();
	delay(2);
	Trans_2();
	delay(2);
	Trans_3();
	delay(2);
	Trans_4();
	delay(2);

	while (1)
	{
		Stuck = 0;
		//run life iterations up to 1000 times or until pattern gets stuck in a loop
		for (reps=0;(reps < 1000) & (Stuck == 0);reps++)
			gol();

		//generate a new random screen with 50 to 200 leds lit
		reps = (rand () % 150) + 50;
		r_rand(reps);


	}
}


/* sets array to reflect next state of GOL iteration.  
*/
uint8_t reset_array (void)
{
uint8_t col;
	
	// if Screen2 = Screen1 or Screen2 = Screen3 then we are at end of transitions
	Stuck = 1;
	for (col = 0; col<32; col++)
	{
		if (Screen2[col] != Screen3[col])
		{
			Stuck = 0;
		}
	}

	if (Stuck == 0)
	{
		Stuck = 1;
		for (col = 0; col<32; col++)
		{
			if (Screen2[col] != Screen1[col])
			{
				Stuck = 0;
			}
		}
	}

	if (Stuck == 0)
	{
		Copy_Screen(1, 3);
		Copy_Screen(2, 1);
		return(0);
	}
	else
	{
		return(1);
	}
	
}

/* computes 1 iteration of the game of life */
/*
Survivals. Every counter with two or three neighboring counters survives for the next generation. 
Deaths. Each counter with four or more neighbors dies (is removed) from overpopulation. 
		Every counter with one neighbor or none dies from isolation. 
Births. Each empty cell adjacent to exactly three neighbors--no more, no fewer--is a birth cell. 
		A counter is placed on it at the next move. 
*/

void gol (void)
{
	uint8_t i=0;
	uint8_t j=0;
	uint8_t number_neighbors=0;
	
	//variables to hold boundary condition information
	uint8_t i_minus1=0;
	uint8_t i_plus1=0;
	uint8_t j_minus1=0;
	uint8_t j_plus1=0;
	/* note: the boundaries of the screen are the top and bottom 
			 rows and the left and right columns.
			 we will treat the display as a torus, meaning that 
			 the cells on the top row will be neighbors to the cells on the bottom
			 and vice versa.
			 likewise, the cells in the first and last columns will be neighbors.
	*/
	
	//display current state
	Show_Screen(1);
	Clear_Screen(2);
	delay(5);

	//compute next state
	while (i<number_rows) 
	{
		//compute boundary condition information
		i_plus1 = i+1;
		i_minus1 = i-1;
		if (i==number_rows-1)
			//  o x o
			//  o o o
			//  o n o
			i_plus1 = 0;
		if (i==0)
			//  o n o
			//  o o o
			//  o x o
			i_minus1 = number_rows-1;
		j=0;
		while (j<number_columns)
		{
			j_plus1 = j+1;
			j_minus1 = j-1;
			if (j==number_columns-1)
				//  o o o
				//  n o x
				//  o o o
				j_plus1 = 0;
			if (j==0)
				//  o o o
				//  x o n
				//  o o o
				j_minus1 = number_columns-1;
				
			//compute number of neighbors for each cell
			number_neighbors = 0;

			if (check_bit(i_minus1, j_minus1) == 1)
				number_neighbors++;
			if (check_bit(i_minus1, j) == 1)
				number_neighbors++;
			if (check_bit(i_minus1, j_plus1) == 1)
				number_neighbors++;
			if (check_bit(i, j_minus1) == 1) 
				number_neighbors++;
			if (check_bit(i, j_plus1) == 1) 
				number_neighbors++;
			if (check_bit(i_plus1, j_minus1) == 1) 
				number_neighbors++;
			if (check_bit(i_plus1, j) == 1) 
				number_neighbors++;
			if (check_bit(i_plus1, j_plus1) == 1) 
				number_neighbors++;

		
			//compute rule
			if (check_bit(i, j) == 1) 
			{
				if ((number_neighbors == 2) | (number_neighbors == 3))
				{
					set_led(2, j, i, 1);
				}

			}
			else
			{
				if (number_neighbors == 3)
				{
					set_led(2, j, i, 1);
				}

			}
			j++;
		}
		i++;
	}
	
	//reset array to reflect next state
	reset_array();
}

uint8_t check_bit(uint8_t i, uint8_t j)
{
	if (j >= 8)
	{
		if (bit_is_set(Screen1[i+16], (j-8)))
			return(1);
		else
			return(0);
	}
	else
	{
			if (bit_is_set(Screen1[i], j))
			return(1);
		else
			return(0);
	}
}

void r_rand(uint8_t reps)
{
uint8_t i, j, x;

	Clear_Screen(1);
	for (x=0;x<reps;x++)
	{
		i = (rand () % 16) + 1;
		j = (rand () % 16) + 1;
		set_led(1, j, i, 1);

	}
}


