/*
 * image_process.c
 *
 * Created: 01-06-2011 23:44:26
 *  Author: limapapy
 */ 
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
//#include <common.h>
//#include <time.h>
#include "globals.h"
#include "image_process.h"
#include "spi_com.h"
#include "bit_tools.h"

#ifdef ROM
#include "images/image_tux65.h"
//#include "images/image_semb.h"
#include "images/image_lams.h"
//#include "images/image_phj.h"
#include "images/image_win7.h"
#endif // ROM
#ifdef RAM
#include "images/image_tux51.h"
#endif // RAM

//#define PROTEUS_DEBUG
#ifdef PROTEUS_DEBUG
	short int xg,yg,ang,cosg,sing;
	rgb_im_t ledsg[32];
#endif // PROTEUS_DEBUG

#define X_PIXEL				65
#define Y_PIXEL				65
#define RADIUS				32
#define SHIFT_RGB			4
#define SHIFT_RGB_IMR		10
#define SHIFT_RGB_IMG		9
#define SHIFT_RGB_IMB		9
#define INV360				0.0027777777778				
#ifdef ROM
#define CURRENT_IMAGE		image_tux65
#define CURRENT_IMAGE2		image_lams
#define CURRENT_IMAGE3		image_win7
//#define CURRENT_IMAGE4		image_phj
#endif // ROM
#ifdef RAM
#define CURRENT_IMAGE		image_tux51
#endif // RAM
//rgb_t IMAGE[X_PIXEL][Y_PIXEL];
unsigned int CURRENT_ANGLE;
short int t=0;
//convert the rectangular to polar
coord_t polar_to_rectangular(float theta,int radius)
{
	coord_t pos;
	pos.x=(int)((float)radius*cos(theta)+32-TRANSLATE);
	pos.y=(int)(-(float)radius*sin(theta)+32-TRANSLATE);
	return pos;
}

//get the next state of the leds from the matrix 
void update_train(short int angle,rgb_t *leds){
	short int i=0;
	short int x,y;
	short int t_sin,t_cos;
	short int sin_ang;sin_ang= (angle>=90) ? angle-90 : 0-(angle-90);
	t_sin= pgm_read_word(&cosine[sin_ang]);
	t_cos=pgm_read_word(&cosine[angle]);
#ifdef PROTEUS_DEBUG
	cosg=t_cos;
	sing=t_sin;
	ang=angle;
#endif // _DEBUG
	for (i=1;i<RADIUS+1;){		
		x=i*t_cos;
		x=x>>7;
		x+=32;
		y=i*t_sin;
		y=y>>7;
		y=32-y;
#ifdef ROM
		switch(image_index){
			case 1:
				leds[i-1].r=pgm_read_byte(&CURRENT_IMAGE[y][x].r);
				leds[i-1].g=pgm_read_byte(&CURRENT_IMAGE[y][x].g);
				leds[i-1].b=pgm_read_byte(&CURRENT_IMAGE[y][x].b);
			
				break;
			case 2:
				leds[i-1].r=pgm_read_byte(&CURRENT_IMAGE2[y][x].r);
				leds[i-1].g=pgm_read_byte(&CURRENT_IMAGE2[y][x].g);
				leds[i-1].b=pgm_read_byte(&CURRENT_IMAGE2[y][x].b);
				break;
			case 3:
				leds[i-1].r=pgm_read_byte(&CURRENT_IMAGE3[y][x].r);
				leds[i-1].g=pgm_read_byte(&CURRENT_IMAGE3[y][x].g);
				leds[i-1].b=pgm_read_byte(&CURRENT_IMAGE3[y][x].b);			
			default:
				/*leds[i-1].r=pgm_read_byte(&CURRENT_IMAGE4[y][x].r);
				leds[i-1].g=pgm_read_byte(&CURRENT_IMAGE4[y][x].g);
				leds[i-1].b=pgm_read_byte(&CURRENT_IMAGE4[y][x].b);	*/
			break;
			
			
		}
		
		
#endif // ROM
		i++;
		
		
		
	}
#ifdef PROTEUS_DEBUG
	copy_rgb(leds,ledsg,32);
#endif // PROTEUS_DEBUG
	

}

#ifdef RAM
//get the next state of the leds from the matrix 
void update_train_im(short int angle,rgb_im_t *leds){
	short int i=0;
	short int x,y;
	rgb_im_t col_tmp;
	short int t_sin,t_cos;
	rgb_im_t col={0,0,0};
	short int sin_ang;sin_ang= (angle>=90) ? angle-90 : 0-(angle-90);
	t_sin= pgm_read_word(&cosine[sin_ang]);
	t_cos=pgm_read_word(&cosine[angle]);
#ifdef PROTEUS_DEBUG
	cosg=t_cos;
	sing=t_sin;
	ang=angle;
#endif // _DEBUG
	for (i=1;i<RADIUS+1;){		
		x=i*t_cos;
		x=x>>7;
		x+=32-TRANSLATE;
		y=i*t_sin;
		y=y>>7;
		y=32-y-TRANSLATE;
		leds[i-1]=CURRENT_IMAGE[y][x];
		i++;		
	}
#ifdef PROTEUS_DEBUG
	copy_rgb(leds,ledsg,32);
#endif // PROTEUS_DEBUG
	

}
#endif // RAM

//send the data to the leds drivers
void  next_train(rgb_t *leds){
	short int i;
	long int to_send=0;
	for (i=RADIUS-1;i>0;i-=2){
		//send first 3 byte
		to_send=((leds[i].b<<SHIFT_RGB));
		to_send=(to_send<<12);
		to_send|=(leds[i].g<<SHIFT_RGB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
		
		//send second 3 byte
		to_send=(leds[i].r<<SHIFT_RGB);
		to_send=(to_send<<12);
		to_send|=((leds[i-1].b)<<SHIFT_RGB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
		//send third 3 byte
		to_send=(leds[i-1].g<<SHIFT_RGB);
		to_send=(to_send<<12);
		to_send|=(leds[i-1].r<<SHIFT_RGB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
	}
	set_xlat();
	clear_xlat();
}


//send the data to the leds drivers
#ifdef RAM
void  next_train_im(rgb_im_t *leds){
	short int i;
	long int to_send=0;
	for (i=RADIUS-1;i>0;i-=2){
		//send first 3 byte
		to_send=((leds[i].b<<SHIFT_RGB_IMB));
		to_send=(to_send<<12);
		to_send|=(leds[i].g<<SHIFT_RGB_IMG);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
		
		//send second 3 byte
		to_send=(leds[i].r<<SHIFT_RGB_IMR);
		to_send=(to_send<<12);
		to_send|=((leds[i-1].b)<<SHIFT_RGB_IMB);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
		//send third 3 byte
		to_send=(leds[i-1].g<<SHIFT_RGB_IMG);
		to_send=(to_send<<12);
		to_send|=(leds[i-1].r<<SHIFT_RGB_IMR);
		SPI_send((unsigned char)(to_send>>16));
		//to_send=to_send>>8;
		SPI_send((unsigned char)(to_send>>8));
		//to_send=to_send>>8;
		SPI_send(to_send);
		
	}
}

#endif // RAM

void clear_train(void){
	unsigned char i;
	for (i=0;i<RADIUS;i++){
		train[i].r=0;
		train[i].g=0;
		train[i].b=0;
	}
	
}


void spiral(rgb_t color){
		long int leds;
		unsigned char i;
		rgb_t off={0,0,0};
		leds =0x01;
		//clear_train();
		train[0]=color;
		next_train(train);
		set_xlat();
		_delay_us(10);
		clear_xlat();
		for (i=0;i<RADIUS;i++){
			if(i<31){
				train[i]=off;
				train[i+1]=color;
			}	
			next_train(train);
			set_xlat();	
			
			clear_xlat();
			_delay_ms(10);	
			
		}
		train[RADIUS-1]=off;
		next_train(train);
}	

unsigned char my_rand(void) {
  static unsigned int seed;
  seed = 181 * seed + 359 ;
  return (seed >> 8);
}
rgb_t rand_color(void){
	rgb_t tmp_color;
	tmp_color.r=my_rand()%256;
	tmp_color.g=my_rand()%256;
	tmp_color.b=my_rand()%256;
	
	return tmp_color;
	
}


void propeller_simul(void){
	unsigned short int i=0;
	//rgb_im_t *leds;
	
	//leds=(rgb_im_t*)malloc(32*sizeof(rgb_im_t));
	while(1){
		//update_and_send(angle);
		//square_prop(10,i%360,train);
#ifdef ROM
		update_train(i%360,train);
		next_train(train);
#endif // ROM
#ifdef RAM
		update_train_im(i%360,train_im);
		next_train_im(train_im);
#endif // RAM
		set_xlat();
		//_delay_us(10);
		clear_xlat();
		//_delay_ms(3000);
		i++;
	}
	
}


void nop(void){
	unsigned char i=0,j;
	
	for (i=0;i<5;i++)
	{
		j=i;
	}
	j++;
}


int getPIXEL_CHAR(char chr,int posx, int posy, char *caract)
{		
		char pos=255;
		char pixel;
		if (chr>= 48 && chr <= 58)
			pos=chr-48;
		if (chr>= 65 && chr <= 90)
			pos=chr-54;

		if (pos==255 || posx>4 || posy>6)
			return 0;
		else{
			pixel=pgm_read_byte(&caract[pos*5+posx]);
			return  (pixel >> (7 - posy )  & 0x01);
		}		
		
}



char *getCHAR_PIXEIS(char chr, char *caract)
{
		char pos=255;

		if (chr>= 48 && chr <= 58)
			pos=chr-48;
		if (chr>= 65 && chr <= 90)
			pos=chr-54;

		if (pos==255)
			return NULL;
		else
			return &caract[pos*5];
}

unsigned int read_TCNT0(void){
	
	
	return 0;
}



float convert_encod_2_radian()
{
	return cont_encoder*0.0174532925;
}

short int convert_encod_2_degree()
{
	return ( (int)cont_encoder*0.72);
}

float convert_to_radian(short int degree){
	float rad=degree;
	rad=rad*INV360*M_PI*2;
	return rad;
}

void convert_int_to_2digits(char *dois_digitos,short int digito)
{
	dois_digitos[0]=48+(int)(digito*0.1);
	dois_digitos[1]=48+(int)(digito%10);
}

int isBlack(rgb_t COR)
{
	return !(COR.r+COR.g+COR.b);
}


void square_prop(unsigned char d,short int angle,rgb_t *leds){
	short int x,y,i;
	rgb_t col={0,0,230};
	short int t_sin,t_cos;
	unsigned char d_minus,d_plus;
	char sin_ang= (angle>=90) ? angle-90 : 0-(angle-90);
	t_sin= pgm_read_word(&cosine[sin_ang]);
	t_cos=pgm_read_word(&cosine[angle]);;
	d_minus=32-d;
	d_plus=32+d;
	for (i=1;i<33;){
		
		x=(((i*t_cos)>>7)+32);
		y=(-((i*t_sin)>>7)+32);	
		leds[i-1].r=0;
		leds[i-1].g=0;
		leds[i-1].b=0;
		//y=d
		if(x>=d_minus && x<=d_plus && y==d_minus){
			leds[i-1]=col;
		}
		//y=-d
		if(x>=d_minus && x<=d_plus && y==d_plus){
			leds[i-1]=col;
		}
		//x=d
		if(y>=d_minus && y<=d_plus && x==d_minus){
			leds[i-1]=col;
		}
		//x=-d
		if(y>=d_minus && y<=d_plus && x==d_plus){
			leds[i-1]=col;
		}
		i++;
	}
	
}

#ifdef RAM
void copy_rgb(rgb_im_t *lo,rgb_im_t *ld,short int size){
	short i=0;
	for (i=0;i<size;i++)
	{
		ld[i]=lo[i];
	}
	
}
#endif // RAM

void full_fill(rgb_t color,rgb_t *leds){
	unsigned char i;
	for (i=0;i<RADIUS;i++){
		train[i]=color;
	}	
}

void mod_div(short int *divd,short int *divr,short int *q,short int *r){
	short int A,M,Q;
	char m=16;
	A=0;
	M=*divr;
	Q=*divd;
	do{
		A=A<<1;
		A =A | (Q>>15);
		Q=Q<<1;
		if (A>=M){
			Q =Q | 0x0001;
			A=A-M;
		}else{
			Q =Q & 0xFFFE;
		}
		m--;
	} while (m>0);
	*r=A;
	*q=Q;
}


coord_t polar_2_rect(short int angulo,short int raio)
{
	coord_t coord;
	//short int sinalx, sinaly;
	if (angulo>=270)
	{
		angulo=360-angulo;
		coord.y=-1;
		coord.x=1;
	}
	else 	if (angulo>=180)
			{
				angulo=angulo-180;
				coord.y=-1;
				coord.x=-1;
			}
			else 	if (angulo>=90)
					{
						angulo=180-angulo;
						coord.y=1;
						coord.x=-1;
					}
					else
					{
						coord.x=1;
						coord.y=1;
					}
	
	
	coord.x=coord.x*pgm_read_byte(&posCOOR[angulo][raio-1])+33;//o raio é só de 0 a 31
	coord.y=(-1)*coord.y*pgm_read_byte(&posCOOR[90-angulo][raio-1])+33; // o angulo é de 0 a 90
	
	return coord;
}

//************************************************DIGITAL FUNCS**************************************************************
void digital_clock_generator(void)
{
	//unsigned char clock_window[7][40];
	//rgb_t rgb_array[32];
	//coord_t p_1;
	//window_coord p_2;
	//window_coord  pos;
	//int radius;
	//int p_x,p_y;
	short int radius;
	for (radius=1; radius<=32;radius++)
	
	{
		
		
		rgb_array[radius-1].r=0x00;
		rgb_array[radius-1].g=0x00;
		rgb_array[radius-1].b=0x00;
		
		
		
		if(encoder360>0 && encoder360<180)
		{
			p_1.x=((radius*cos_calc(encoder360))>>7)+32;
			p_1.y=(-((radius*sin_calc(encoder360))>>7))+32;
			
		
			//p_1.x=radius*cos(cont_encoder)+32;
			//p_1.y=-radius*sin(cont_encoder)+32;
			//p_1=polar_2_rect(cont_encoder,radius);
			
			p_2=get_clock_window_index(p_1.x, p_1.y);
		
			if(p_2.found==1)
			{
				rgb_array[radius-1].b=(clock_window[p_2.y][p_2.x]*255);	
			}
		}
		
		
			

				/*	pos=get_mensg_window_index(p_1.x,p_1.y);

					if(pos.found==1)
						{
							rgb_array[radius-1].b=pgm_read_byte(&shift_message[pos.y][p_1.x])*255;
						}
					else{
						rgb_array[radius-1].b=0x00;
						}*/
		
	}	
	next_train(rgb_array);
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
	short int i,j;
	for (i=0;i<7;i++)
	{
		for (j=0;j<48;j++)
		{
			clock_window[i][j]=0;
		}

	}
}


void update_clk_window(unsigned char clock_digits[8], unsigned char clock_window[7][48])
{
	
//	
	int i;
	int chr_index;
	unsigned char pattern_digit[7];
	int clk_x;
	
//
	clk_x=0;
//
	//gget_clock(clock_digits);
	// updating clock
		for(i=0;i<8;i++)
		{
			chr_index=get_chr_index(clock_digits[i]);
			get_pattern_digit(chr_index,pattern_digit);
			store_clock_digit(clk_x,clock_window,pattern_digit);
			clk_x+=6;
			
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


void store_clock_digit(unsigned char start,unsigned char clock_window[7][48], unsigned char digit[7])
{

//
	unsigned char i,j;
//

//		
	for(i=0;i<7;i++)
	{
		for(j=0;j<6;j++)
		{
			if(bit_read(digit[i],5-j))
			clock_window[i][start+j]=1;
			else
			clock_window[i][start+j]=0;
		}

	}

	
}



window_coord get_clock_window_index(int x, int y)
{

	window_coord wp;
	wp.x=x;
	wp.y=y;
	wp.found=-1;
	   
	 if(x>=X_CLK_W_START && x<=X_CLK_W_END)
	 {
		 if(y>=Y_CLK_W_START && y<=Y_CLK_W_END)
			 {
				wp.x=x-X_CLK_W_START;
				wp.y=y-Y_CLK_W_START ;
				wp.found=1;
			 }	 
	 }	
	return wp;
}

window_coord get_mensg_window_index(int x, int y)
{

	window_coord wp;
	wp.x=x;
	wp.y=y;
	wp.found=-1; 
	 if(x>=msg_window.start && x<=msg_window.end)
	 {
		 if(y>=Y_MSG_W_START && y<=Y_MSG_W_END)
			 {
				wp.x=x-msg_window.start + msg_window.array_offset;
				wp.y=y-Y_MSG_W_START ;
				wp.found=1;
			 }	 
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
	
// ****************************************FIM DIGITAL CLOCK FUNCS****************************************************