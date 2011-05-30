
#ifndef DIGITAL_CLOCK_H
#define DIGITAL_CLOCK_H

#define X_CLK_W_STAR  12
#define X_CLK_W_END  51

#define Y_CLK_W_STAR 13
#define Y_CLK_W_END 20

#include "i2c_com.h"
#include "bit_tools.h" 
#include "spi_com.h"
#include "propeller_lib.h"



//++++++++++++++++++++++++++++++++++++++temp vars
/*unsigned char clock_window[7][40];
unsigned char clock_digits[8];
int chr_index;
int chr_index2;
unsigned char my_array[8];
unsigned char var_test;
window_coord ps;*/
//+++++++++++++++++++++++++++++++++++++++++++++++++

unsigned char get_pixel(unsigned char j, unsigned char i, unsigned char clock_window[7][40]);
void  clear_clock_window(unsigned char clock_window[7][40]);
void update_clk_window(unsigned char clock_window[7][40]);
int get_chr_index(char ascii_value);
void get_pattern_digit(unsigned char index, unsigned char pattern_digit[7]);
void store_clock_digit(unsigned char start,unsigned char clock_window[7][40], unsigned char digit[7]);
window_coord get_clock_window_index(unsigned int x, unsigned int y);
window_coord get_mensg_window_index(int y);
void digital_clock_generator(void);
void shift_msg_window(void);
int sin_calc(int angle);
int cos_calc(int angle);

//void  next_train(struct rgb_t *leds);


void digital_clock_generator(void)
{
	//unsigned char clock_window[7][40];
	//rgb_t rgb_array[32];
	//coord_t p_1;
	//window_coord p_2;
	//window_coord  pos;
	//int radius;
	//int p_x,p_y;
 x2=0;
 y2=0;
 px=' ';

	for (radius=1; radius<=32;radius++)
	
	{
		rgb_array[radius-1].r=0x00;
		rgb_array[radius-1].g=0x00;
		
		p_1.x=(((radius*pgm_read_word(&cosine[cont_encoder]))>>7)+32);
		p_1.y=(-((radius*pgm_read_word(&cosine[(cont_encoder>=90) ? cont_encoder-90 : 0-(cont_encoder-90)]))>>7)+32);
		
		p_2=get_clock_window_index(p_1.x,p_1.y);
	
		if(p_2.found==1)
		{
			//rgb_array[radius-1].b=(get_pixel(p_2.x,p_2.y, &clock_window))<<6;
			rgb_array[radius-1].b=clock_window[p_2.y][p_2.x]<<6;
		
		}
		else
		{
			rgb_array[radius-1].b=0x00;
		}
		
		/*
			if(p_1.x>=message_x && p_1.x<MSG_END)
				{
					pos=get_mensg_window_index(p_1.y);

					if(pos.found==1)
						{
							rgb_array[radius-1].b=pgm_read_byte(&shift_message[pos.y][p_1.x]);
						}
					else{
						rgb_array[radius-1].b=0x00;
						}
				}*/

	}	
	next_train(rgb_array);

}


void shift_msg_window(void)
{ 

	shift_counter--;
	if(shift_counter==0)
	{
	if(message_x>MSG_START)
	{
		shift_counter=SHIFT_N;
		message_x--;
	}
	else
		message_x=MSG_END;
	}
}



unsigned char get_pixel(unsigned char j, unsigned char i, unsigned char clock_window[7][40])
{
	unsigned char ret_pixel;
	
	ret_pixel=clock_window[i][j];
	return ret_pixel;
}

void  clear_clock_window(unsigned char clock_window[7][40])
{
	for (int i=0;i<7;i++)
	{
		for (int j=0;j<40;j++)
		{
			clock_window[i][j]=0;
		}

	}
}


void update_clk_window(unsigned char clock_window[7][40])
{
	
//	
	int i;
	int chr_index;
	unsigned char pattern_digit[7];
	int clk_x;
	
//
	clk_x=0;
//
		for(i=0;i<8;i++)
		{
			chr_index=get_chr_index(clock_digits[i]);
			get_pattern_digit(chr_index,pattern_digit);
			store_clock_digit(clk_x,clock_window,pattern_digit);
			clk_x+=5;
			
		}
}


int get_chr_index(char ascii_value)
{

//
	unsigned int i;
	int pattern_index;
	unsigned char chr;
	unsigned char continue_to_search=1;

//
	continue_to_search=1;
//
	for(i=0;i<NUM_CHARS &&continue_to_search>0;i++)
		{
			chr=pgm_read_byte(&tab_chars[i]);
			if(ascii_value==chr)
				{
					continue_to_search=0;
					pattern_index=i;
				}
				
			else pattern_index=-1;
		}

			return pattern_index;
}


void get_pattern_digit(unsigned char index,unsigned char pattern_digit[7])
{
	unsigned char j;
	for(j=0;j<7;j++)
		{
			pattern_digit[j]=pgm_read_byte(&char_patterns[index][j]);
		}
	
}


void store_clock_digit(unsigned char start,unsigned char clock_window[7][40], unsigned char digit[7])
{

//
	unsigned char i,j;
//

//		
	for(i=0;i<7;i++)
	{
		for(j=0;j<5;j++)
		{
			if(bit_read(digit[i],4-j))
			clock_window[i][start+j]=1;
			else
			clock_window[i][start+j]=0;
		}

	}

	
}


window_coord get_clock_window_index(unsigned int x, unsigned int y)
{

	window_coord wp;
	wp.x=0;
	wp.y=0;
	wp.found=-1;
	   
	 if(x>=X_CLK_W_STAR && x<=X_CLK_W_END)
	 {
		 if(y>=Y_CLK_W_STAR && y<=Y_CLK_W_END)
			 {
				wp.x=x-12;
				wp.y=-13+y;
				wp.found=1;
			 }	 
	 }	
	return wp;
}

window_coord get_mensg_window_index(int y)
{
	int i;
	int w_index; 
	window_coord wp;
    int continue_y;
//
	wp.y=0;
	wp.x=0;
	continue_y=1;	
	for(i=0;i<7 && continue_y==1;i++)
	{
		w_index=pgm_read_byte(&window_mensg_map_y[i]);
		if(y==w_index)
		{
			wp.y=i;
			continue_y=0;
			wp.found=1;
		}
		else
		wp.found=-1;
				
	}
	
	return wp;
}

int sin_calc(int angle)
{
	short int sin_ang;
	int t_sin;
	sin_ang= (angle>=90) ? angle-90 : 0-(angle-90);
	t_sin= pgm_read_word(&cosine[sin_ang]);
	return t_sin;
}


int  cos_calc(int angle)
	{
	int t_cos;
	t_cos=pgm_read_word(&cosine[angle]);
	return t_cos;
	}
#endif
