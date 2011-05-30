#include "globals.h"
#include "image_process.h"
#include "digital_clk.h"
#include "bit_tools.h"


#define NUM_CHARS 11
#define X_CLK_W_START  8
#define X_CLK_W_END  55
#define Y_CLK_W_START 10
#define Y_CLK_W_END  16
#define WINDOW_SIZE 48

#define MSG_W_START 17
#define MSG_W_END 46
#define MSG_W_SIZE 34
#define MSG_ARRAY_SIZE 150
#define Y_MSG_W_START 44
#define Y_MSG_W_END 50
#define SHIFT_N	500
#define W_STATE0 0
#define W_STATE1 1
#define W_STATE2 2
void init_DCLK()
{
	send_enable=0;
	update_clock=0;
	encoder360=0;
	msg_window.start=MSG_W_END;;
	msg_window.end=MSG_W_END;
	msg_window.shift_counter=SHIFT_N;
}
void digital_clock_generator(rgb_t *train,short int theta)
{
    short int radius;
	char found;
	for (radius=2; radius<=32;radius++)
	
	{
		train[radius-1].r=0x00;
		train[radius-1].g=0x00;
		found=-1;
		
		coord_t p1=polar_2_rect(theta,radius);
		coord_t p2=get_clock_window_index(p1,&found);
	
		if(found==1)
			train[radius-1].b=clock_window[p2.y][p2.x]<<6;
		else
			train[radius-1].b=0x00;
		}
}

coord_t get_clock_window_index(coord_t p1, char *found)
{

	coord_t wp;
	wp.x=0;
	wp.y=0;
	
	if(p1.x>=X_CLK_W_START && p1.x<=X_CLK_W_END)
	 {
		 if(p1.y>=Y_CLK_W_START && p1.y<=Y_CLK_W_END)
			 {
				wp.x=p1.x-13;
				wp.y=p1.y-14;
				*found=1;
			 }	 
	 }	
	return wp;
}






void update_clk_window(unsigned char clock_window[7][40])
{
	short int i;
	short int chr_index;
	unsigned char pattern_digit[7];
	int clk_x;
	
	clk_x=0;

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
	unsigned int i;
	int pattern_index;
	unsigned char chr;
	unsigned char continue_to_search=1;
	continue_to_search=1;

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




void store_clock_digit(unsigned char start,unsigned char clock_window[7][40],unsigned char digit[7])
{
	unsigned char i,j;
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



void shift_msg_window(void)
{ 
	msg_window.shift_counter--;
	if(msg_window.shift_counter==0)
	{
		msg_window.shift_counter=SHIFT_N;
		switch(msg_window.state)
		{
			case W_STATE0:
						msg_window.array_offset=0;
						msg_window.start--;
						msg_window.end=MSG_W_END;
						if(msg_window.start>MSG_W_START)
						{
							msg_window.state=W_STATE0;
						}
						else
						{
							msg_window.state=W_STATE1;
						}
						break;
			case W_STATE1:
						msg_window.array_offset++;
						msg_window.start=MSG_W_START;
						msg_window.end=MSG_W_END;
						if(MSG_ARRAY_SIZE-msg_window.array_offset>MSG_W_SIZE)
						{
							msg_window.state=W_STATE1;
						}
						else
						{
							msg_window.state=W_STATE2;
						}
						break;
			case W_STATE2:
						msg_window.array_offset++;
						msg_window.start=MSG_W_START;
						msg_window.end--;
						if(msg_window.end>MSG_W_START)
						{
							msg_window.state=W_STATE2;
						}
						else
						{
							msg_window.start=MSG_W_END;
							msg_window.state=W_STATE0;
						}
						break;
			 default:
						msg_window.shift_counter=SHIFT_N;
						msg_window.start=MSG_W_END;
						msg_window.state=W_STATE0;
						break;
		}
	}
}

unsigned char get_pixel(unsigned char j, unsigned char i, unsigned char clock_window[7][48])
{
	unsigned char ret_pixel;
	
	ret_pixel=clock_window[i][j];
	return ret_pixel;
}

void  clear_clock_window(unsigned char clock_window[7][48])
{
	for (int i=0;i<7;i++)
	{
		for (int j=0;j<48;j++)
		{
			clock_window[i][j]=0;
		}

	}
}

